#include <davix.hpp>
#include <unistd.h>
#include "gfal_http_plugin.h"
#include <transfer/gfal_transfer_plugins.h>
#include <cstdio>

struct PerformanceMarker {
    int    index, count;

    time_t begin, latest;
    off_t  transferred;

    off_t transferAvg;
    off_t transferInstant;

    PerformanceMarker(): index(0), count(0), begin(0), latest(0),
            transferred(0),
            transferAvg(0), transferInstant(0) {}
};

struct PerformanceData {
    time_t begin, latest;

    int markerCount;
    PerformanceMarker* array;

    PerformanceData(): begin(0), latest(0),
            markerCount(0), array(NULL) {}

    ~PerformanceData() {
        if(array)
        	delete [] array;
    }

    void update(const PerformanceMarker& in) {
        if (markerCount != in.count) {
            if(array)
	    	delete [] array;
            markerCount = in.count;
            array = new PerformanceMarker[markerCount];
        }
        if (in.index < 0 || in.index > markerCount)
            return;

        PerformanceMarker& marker = array[in.index];

        if (marker.begin == 0)
            marker.begin = in.latest;

        // Calculate differences
        time_t absElapsed  = in.latest - marker.begin;
        time_t diffElapsed = in.latest - marker.latest;
        off_t  diffSize    = in.transferred - marker.transferred;

        // Update
        marker.index       = in.index;
        marker.count       = in.count;
        marker.latest      = in.latest;
        marker.transferred = in.transferred;
        if (absElapsed)
            marker.transferAvg = marker.transferred / absElapsed;
        if (diffElapsed)
            marker.transferInstant = diffSize / diffElapsed;

        if (begin == 0 || begin < marker.begin)
            begin = marker.begin;
        if (latest < marker.latest)
            latest = marker.latest;
    }

    time_t absElapsed() const {
        return latest - begin;
    }

    off_t avgTransfer(void) const {
        off_t total = 0;
        for (int i = 0; i < markerCount; ++i)
            total += array[i].transferAvg;
        return total;
    }

    off_t diffTransfer() const {
        off_t total = 0;
        for (int i = 0; i < markerCount; ++i)
            total += array[i].transferInstant;
        return total;
    }

    off_t totalTransferred() const {
        off_t total = 0;
        for (int i = 0; i < markerCount; ++i)
            total += array[i].transferred;
        return total;
    }
};



std::string gfal_http_3rdcopy_full_url(const std::string ref,
        const std::string& uri)
{
    std::string final;

    if (uri.substr(0, 7).compare("http://") == 0) {
        final = uri;
    }
    else if (uri.substr(0, 8).compare("https://") == 0) {
        final = uri;
    }
    else if (uri[0] == '/') {
        size_t colon = ref.find(':');
        size_t slash = std::string::npos;
        if (colon != std::string::npos)
            slash = ref.find('/', colon + 3);
        if (slash != std::string::npos) {
            std::string base = ref.substr(0, slash);
            final = base + uri;
        }
    }
    else {
        final = ref + uri;
    }

    return final;
}



std::string gfal_http_3rdcopy_full_delegation_endpoint(const std::string ref,
        const std::string& uri, GError** err)
{
    std::string final = gfal_http_3rdcopy_full_url(ref, uri);
    if (final.substr(7).compare("http://") == 0) {
        g_error_new(http_plugin_domain, EINVAL,
                    "Plain http can not be used for delegation (%s)",
                    uri.c_str());
        final.clear();
    }
    return final;
}



Davix::HttpRequest* gfal_http_3rdcopy_do_copy(GfalHttpInternal* davix,
        gfalt_params_t params, const std::string& src, const std::string& dst,
        std::string &finalSource,
        GError** err)
{
    Davix::DavixError* daverr = NULL;
    std::string nextSrc(src), prevSrc(src);
    std::string delegationEndpoint;

    Davix::RequestParams requestParams(davix->params);
    requestParams.setTransparentRedirectionSupport(false);
    requestParams.setClientCertCallbackX509(&gfal_http_authn_cert_X509, NULL);

    char nstreams[8];
    snprintf(nstreams, sizeof(nstreams), "%d", gfalt_get_nbstreams(params, NULL));

    Davix::HttpRequest* request = NULL;

    do {
        nextSrc = gfal_http_3rdcopy_full_url(prevSrc, nextSrc);
        prevSrc = nextSrc;
        delete request;
        gfal_log(GFAL_VERBOSE_TRACE, "\t\t%s: Next hop = '%s'", __func__,
                nextSrc.c_str());
        request = davix->context.createRequest(nextSrc, &daverr);
        if (daverr)
            break;

        request->setRequestMethod("COPY");
        request->addHeaderField("Destination", dst);
        request->addHeaderField("X-Number-Of-Streams", nstreams);
        request->setParameters(requestParams);
        request->beginRequest(&daverr);
        if (daverr)
            break;

        // If we get a X-Delegate-To, before continuing, delegate
        if (request->getAnswerHeader("X-Delegate-To", delegationEndpoint)) {
            delegationEndpoint = gfal_http_3rdcopy_full_delegation_endpoint(src,
                    delegationEndpoint, err);
            if (*err)
                break;

            gfal_log(GFAL_VERBOSE_TRACE, "\t\t%s: Got delegation endpoint %s",
                    __func__, delegationEndpoint.c_str());
            char *dlg_id = gfal_http_delegate(delegationEndpoint, err);

            if (*err){
	        if(dlg_id){
			free(dlg_id);
			dlg_id = NULL;
		}
                break;
	    }
	    
	   if(dlg_id){
		free(dlg_id);
		dlg_id=NULL;
	   }

            gfal_log(GFAL_VERBOSE_TRACE, "\t\t%s: Delegated successfully",
                    __func__);
        }

    } while (request->getAnswerHeader("Location", nextSrc));
    finalSource = nextSrc;

    if (daverr) {
        davix2gliberr(daverr, err);
        delete daverr;
    }
    else if (!*err && request->getRequestCode() >= 300) {
        *err = g_error_new(http_plugin_domain, EIO,
                           "Invalid status code: %d", request->getRequestCode());
    }

    if (*err) {
        delete request;
        request = NULL;
    }

    return request;
}



void gfal_http_3rdcopy_do_callback(const char* src, const char* dst,
                                   gfalt_monitor_func callback, void* udata,
                                   const PerformanceData& perfData)
{
    if (callback) {
        gfalt_hook_transfer_plugin_t hook;

        hook.average_baudrate = static_cast<size_t>(perfData.avgTransfer());
        hook.bytes_transfered = static_cast<size_t>(perfData.totalTransferred());
        hook.instant_baudrate = static_cast<size_t>(perfData.diffTransfer());
        hook.transfer_time    = perfData.absElapsed();

        gfalt_transfer_status_t state = gfalt_transfer_status_create(&hook);
        callback(state, src, dst, udata);
        gfalt_transfer_status_delete(state);
    }
}



int gfal_http_3rdcopy_performance_marks(const char* src, const char* dst,
        gfalt_params_t params,
        Davix::HttpRequest* request, GError** err)
{
    Davix::DavixError* daverr = NULL;
    char buffer[1024], *p;
    size_t line_len;

    gfalt_monitor_func callback = gfalt_get_monitor_callback(params, NULL);
    void* udata = gfalt_get_user_data(params, NULL);

    PerformanceMarker holder;
    PerformanceData   performance;
    time_t lastPerfCallback = time(NULL);

    while ((line_len = request->readLine(buffer, sizeof(buffer), &daverr)) >= 0 && !daverr) {
        buffer[line_len] = '\0';
        // Skip heading whitespaces
        p = buffer;
        while (*p && p < buffer+sizeof(buffer) && isspace(*p))
            ++p;

        if (strncasecmp("Perf Marker", p, 11) == 0) {
            memset(&holder, 0, sizeof(holder));
        }
        else if (strncasecmp("Timestamp:", p, 10) == 0) {
            holder.latest = atol(p + 10);
        }
        else if (strncasecmp("Stripe Index:", p, 13) == 0) {
            holder.index = atoi(p + 13);
        }
        else if (strncasecmp("Stripe Bytes Transferred:", p, 25) == 0) {
            holder.transferred = atol(p + 26);
        }
        else if (strncasecmp("Total Stripe Count:", p, 19) == 0) {
            holder.count = atoi(p + 20);
        }
        else if (strncasecmp("End", p, 3) == 0) {
            performance.update(holder);
            time_t now = time(NULL);
            if (now - lastPerfCallback >= 1) {
                gfal_http_3rdcopy_do_callback(src, dst, callback, udata,
                                              performance);
                lastPerfCallback = now;
            }
        }
        else if (strncasecmp("success", p, 7) == 0) {
            break;
        }
        else if (strncasecmp("aborted", p, 7) == 0) {
            g_set_error(err, http_plugin_domain, ECANCELED,
                        "Transfer aborted in the remote end");
            break;
        }
        else if (strncasecmp("failed", p, 6) == 0) {
            g_set_error(err, http_plugin_domain, EIO,
                        "Transfer failed: %s", p);
            break;
        }
        else {
            g_set_error(err, http_plugin_domain, EPROTO,
                        "Unexpected message from remote host: %s", p);
            break;
        }
    }
    request->endRequest(&daverr);

    if (!*err && daverr) {
        davix2gliberr(daverr, err);
        delete daverr;
    }

    return *err != NULL;
}



int gfal_http_exists(plugin_handle plugin_data,
                     const char *dst,
                     GError** err)
{
    GError *nestedError = NULL;
    struct stat st;
    gfal_http_stat(plugin_data, dst, &st, &nestedError);

    if (nestedError && nestedError->code == ENOENT) {
        g_error_free(nestedError);
        return 0;
    }
    else if (nestedError) {
        g_propagate_prefixed_error(err, nestedError, "[%s]", __func__);
        return -1;
    }
    else {
        return 1;
    }
}



int gfal_http_3rdcopy_overwrite(plugin_handle plugin_data,
                                gfalt_params_t params,
                                const char *dst,
                                GError** err)
{
    GError *nestedError = NULL;

    if (!gfalt_get_replace_existing_file(params,NULL))
        return 0;

    int exists = gfal_http_exists(plugin_data, dst, &nestedError);

    if (exists < 0) {
        g_propagate_prefixed_error(err, nestedError, "[%s]", __func__);
        return -1;
    }
    else if (exists == 1) {
        gfal_http_unlinkG(plugin_data, dst, &nestedError);
        if (nestedError) {
            g_propagate_prefixed_error(err, nestedError, "[%s]", __func__);
            return -1;
        }

        gfal_log(GFAL_VERBOSE_TRACE,
                 "File %s deleted with success (overwrite set)", dst);
    }
    return 0;
}



char* gfal_http_get_parent(const char* url)
{
    char *parent = g_strdup(url);
    char *slash = strrchr(parent, '/');
    if (slash) {
        *slash = '\0';
    }
    else {
        g_free(parent);
        parent = NULL;
    }
    return parent;
}



int gfal_http_3rdcopy_make_parent(plugin_handle plugin_data,
                                  gfalt_params_t params,
                                  const char* dst,
                                  GError** err)
{
    GError *nestedError = NULL;

    if (!gfalt_get_create_parent_dir(params, NULL))
        return 0;

    char *parent = gfal_http_get_parent(dst);
    if (!parent) {
        *err = g_error_new(http_plugin_domain, EINVAL,
                           "[%s] Could not get the parent directory of %s",
                           __func__, dst);
        return -1;
    }

    int exists = gfal_http_exists(plugin_data, parent, &nestedError);
    // Error
    if (exists < 0) {
        g_propagate_prefixed_error(err, nestedError, "[%s]", __func__);
        return -1;
    }
    // Does exist
    else if (exists == 1) {
        return 0;
    }
    // Does not exist
    else {
        gfal_http_mkdirpG(plugin_data, parent, 0755, TRUE, &nestedError);
        if (nestedError) {
            g_propagate_prefixed_error(err, nestedError, "[%s]", __func__);
            return -1;
        }
        gfal_log(GFAL_VERBOSE_TRACE,
                 "[%s] Created parent directory %s", __func__, parent);
        return 0;
    }
}


// dst may be NULL. In that case, the user-defined checksum
// is compared with the source checksum.
// If dst != NULL, then user-defined is ignored
int gfal_http_3rdcopy_checksum(plugin_handle plugin_data,
                               gfalt_params_t params,
                               const char *src, const char *dst,
                               GError** err)
{
    if (!gfalt_get_checksum_check(params, NULL))
        return 0;

    char checksum_type[1024];
    char checksum_value[1024];
    gfalt_get_user_defined_checksum(params,
                                    checksum_type, sizeof(checksum_type),
                                    checksum_value, sizeof(checksum_value),
                                    NULL);
    if (!checksum_type[0])
        strcpy(checksum_type, "MD5");

    GError *nestedError = NULL;
    char src_checksum[1024];
    gfal_http_checksum(plugin_data, src, checksum_type,
                       src_checksum, sizeof(src_checksum),
                       0, 0, &nestedError);
    if (nestedError) {
        g_propagate_prefixed_error(err, nestedError, "[%s]", __func__);
        return -1;
    }

    if (!dst) {
        if (checksum_value[0] && strcasecmp(src_checksum, checksum_value) != 0) {
            *err = g_error_new(http_plugin_domain, EINVAL,
                               "[%s] Source and user-defined %s do not match (%s != %s)",
                               __func__, checksum_type, src_checksum, checksum_value);
            return -1;
        }
        else if (checksum_value[0]) {
            gfal_log(GFAL_VERBOSE_TRACE,
                     "[%s] Source and user-defined %s match: %s",
                     __func__, checksum_type, checksum_value);
        }
    }
    else {
        char dst_checksum[1024];
        gfal_http_checksum(plugin_data, dst, checksum_type,
                           dst_checksum, sizeof(dst_checksum),
                           0, 0, &nestedError);
        if (nestedError) {
            g_propagate_prefixed_error(err, nestedError, "[%s]", __func__);
            return -1;
        }

        if (strcasecmp(src_checksum, dst_checksum) != 0) {
            *err = g_error_new(http_plugin_domain, EINVAL,
                                       "[%s] Source and destination %s do not match (%s != %s)",
                                       __func__, checksum_type, src_checksum, dst_checksum);
            return -1;
        }

        gfal_log(GFAL_VERBOSE_TRACE,
                 "[%s] Source and destination %s match: %s",
                 __func__, checksum_type, src_checksum);
    }
    return 0;
}



int gfal_http_3rdcopy(plugin_handle plugin_data, gfal2_context_t context,
        gfalt_params_t params, const char* src, const char* dst, GError** err)
{
    GfalHttpInternal* davix = gfal_http_get_plugin_context(plugin_data);

    plugin_trigger_event(params, http_plugin_domain,
                         GFAL_EVENT_NONE, GFAL_EVENT_PREPARE_ENTER,
                         "%s => %s", src, dst);

    // When this flag is set, the plugin should handle overwriting,
    // parent directory creation,...
    if (!gfalt_get_strict_copy_mode(params, NULL)) {
        plugin_trigger_event(params, http_plugin_domain,
                             GFAL_EVENT_SOURCE, GFAL_EVENT_CHECKSUM_ENTER,
                             "");
        if (gfal_http_3rdcopy_checksum(plugin_data, params, src, NULL, err) != 0)
            return -1;
        plugin_trigger_event(params, http_plugin_domain,
                             GFAL_EVENT_SOURCE, GFAL_EVENT_CHECKSUM_EXIT,
                             "");

        if (gfal_http_3rdcopy_overwrite(plugin_data, params, dst, err) != 0 ||
            gfal_http_3rdcopy_make_parent(plugin_data, params, dst, err) != 0)
            return -1;
    }

    std::string finalSource;
    Davix::HttpRequest* request = gfal_http_3rdcopy_do_copy(davix, params,
                                                            src, dst,
                                                            finalSource,
                                                            err);
    if (!request)
        return -1;

    plugin_trigger_event(params, http_plugin_domain,
                         GFAL_EVENT_NONE, GFAL_EVENT_PREPARE_EXIT,
                         "%s => %s", src, dst);

    plugin_trigger_event(params, http_plugin_domain,
                         GFAL_EVENT_NONE, GFAL_EVENT_TRANSFER_ENTER,
                         "%s => %s", finalSource.c_str(), dst);

    int r = gfal_http_3rdcopy_performance_marks(src, dst, params, request, err);
    delete request;
    if (r != 0)
        return -1;

    plugin_trigger_event(params, http_plugin_domain,
                         GFAL_EVENT_NONE, GFAL_EVENT_TRANSFER_EXIT,
                         "%s => %s", finalSource.c_str(), dst);

    if (!gfalt_get_strict_copy_mode(params, NULL)) {
        plugin_trigger_event(params, http_plugin_domain,
                             GFAL_EVENT_DESTINATION, GFAL_EVENT_CHECKSUM_ENTER,
                             "");
        if (gfal_http_3rdcopy_checksum(plugin_data, params, src, dst, err) != 0)
            return -1;
        plugin_trigger_event(params, http_plugin_domain,
                                     GFAL_EVENT_DESTINATION, GFAL_EVENT_CHECKSUM_ENTER,
                                     "");
    }



    return 0;
}



int gfal_http_3rdcopy_check(plugin_handle plugin_data, const char* src,
        const char* dst, gfal_url2_check check)
{
    if (check != GFAL_FILE_COPY)
        return 0;

    return (strncmp(src, "https://", 8) == 0 && strncmp(dst, "https://", 8) == 0);
}