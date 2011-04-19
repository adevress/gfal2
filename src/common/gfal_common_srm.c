/*
 * Copyright (c) Members of the EGEE Collaboration. 2004.
 * See http://www.eu-egee.org/partners/ for details on the copyright holders.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file gfal_common.c
 * @brief the header file with the main srm funcs of the common API
 * @author Devresse Adrien
 * @version 2.0
 * @date 12/04/2011
 * */
 
#include "gfal_common.h"
#include "gfal_common_srm.h"

/**
 * check the validity of the current handle
 */
static gboolean gfal_handle_checkG(gfal_handle handle, GError** err){
	if(handle->initiated == 1)
		return TRUE;
	g_set_error(err,0, EINVAL,"[gboolean gfal_handle_checkG] gfal_handle not set correctly");
	return FALSE;
}

/**
 * set the bdii value of the handle specified
 */
void gfal_set_nobdiiG(gfal_handle handle, gboolean no_bdii_chk){
	handle->no_bdii_check = no_bdii_chk;
}

/**
 *  return 0 if a full endpoint is contained in surl  
 * 
*/
int gfal_check_fullendpoint_in_surl(const char * surl, GError ** err){
	regex_t rex;
	int ret = regcomp(&rex, "^srm://([:alnum:]|-|/|\.|_)+:[0-9]+/([:alnum:]|-|/|\.|_)+?SFN=",REG_ICASE | REG_EXTENDED);
	g_return_val_err_if_fail(ret==0,-1,err,"[gfal_check_fullendpoint_in_surl] fail to compile regex, report this bug");
	ret=  regexec(&rex,surl,0,NULL,0);
	return ret;	
}

/**
 *  @brief create a full endpath from a surl with full endpath
 * */
char* gfal_get_fullendpoint(const char* surl, GError** err){
	char* p = strstr(surl,"?SFN=");
	const int len_prefix = strlen(GFAL_PREFIX_SRM);						// get the srm prefix length
	const int len_endpoint_prefix = strlen(GFAL_ENDPOINT_DEFAULT_PREFIX); // get the endpoint protocol prefix len 
	g_return_val_err_if_fail(p && len_prefix && (p>(surl+len_prefix)) && len_endpoint_prefix,NULL,err,"[gfal_get_fullendpoint] full surl must contain ?SFN= and a valid prefix, fatal error");	// assertion on params
	char* resu = calloc(p-surl-len_prefix+len_endpoint_prefix, sizeof(char));	
	strncpy(resu, GFAL_ENDPOINT_DEFAULT_PREFIX, len_endpoint_prefix);	// copy prefix
	strncpy(resu + len_endpoint_prefix, surl+len_prefix, p- surl-len_prefix);		// copy endpoint
	return resu;
}

/**
 * @brief get endpoint from the bdii system only
 * @return 0 if success with endpoint and srm_type set correctly else -1 and err set
 * 
 * */
int gfal_get_endpoint_and_setype_from_bdii(gfal_handle handle, char** endpoint, enum gfal_srm_proto* srm_type, GList* surls, GError** err){
	g_return_val_err_if_fail(handle && endpoint && srm_type && surls, -1, err, "[gfal_get_endpoint_and_setype_from_bdii] invalid parameters");
	char** tab_endpoint;
	char** tab_se_type;

	return -1;
	
}

/**
 * @brief get the hostname from a surl
 *  @return return NULL if error and set err else return the hostname value
 */
 char*  gfal_get_hostname_from_surl(const char * surl, GError* err){
	 const int srm_prefix_len = strlen(GFAL_PREFIX_SRM);
	 const int surl_len = strnlen(surl,2048);
	 g_return_val_err_if_fail(surl &&  (srm_prefix_len <surl_len)  && surl_len < 2048,NULL, err, "[gfal_get_hostname_from_surl] invalid value in params");
	 char* p = strchr(surl+srm_prefix_len,'/');
	 char* prep = strstr(surl, GFAL_PREFIX_SRM);
	 if(prep != surl){
		 g_set_error(err,0, EINVAL, "[gfal_get_hostname_from_surl not a valid surl");
		 return NULL;
	 }
	 return strndup(surl+srm_prefix_len, p-surl-srm_prefix_len);	 
 }

/**
 * @brief get endpoint
 *  determine the best endpoint associated with the list of url and the params of the actual handle (no bdii check or not)
 *  see the diagram in doc/diagrams/surls_get_endpoint_activity_diagram.svg for more informations
 *  @return return 0 with endpoint and types set if success else -1 and set Error
 * */
int gfal_auto_get_srm_endpoint(gfal_handle handle, char** endpoint, enum gfal_srm_proto* srm_type, GList* surls, GError** err){
	g_return_val_err_if_fail(handle && endpoint && srm_type && surls,-1, err, "[gfal_auto_get_srm_endpoint] invalid value in params"); // check params
	
	GError* tmp_err=NULL;
	char * tmp_endpoint=NULL;;
	gboolean isFullEndpoint = gfal_check_fullendpoint_in_surl(surls->data, &tmp_err)?FALSE:TRUE;		// check if a full endpoint exist
	if(tmp_err){
		g_propagate_prefixed_error(err, tmp_err, "[gfal_auto_get_srm_endpoint]");
		return -2;
	}
	if(handle->no_bdii_check == TRUE && isFullEndpoint == FALSE){ // no bdii checked + no full endpoint provided = error 
		g_set_error(err,0,EINVAL,"[gfal_auto_get_srm_endpoint] no_bdii_check option need a full endpoint in the first surl");
		return -3;
	}
	if( isFullEndpoint == TRUE  ){ // if full endpoint contained in url, get it and set type to default type
		if( (tmp_endpoint = gfal_get_fullendpoint(surls->data,&tmp_err) ) == NULL){
				g_propagate_prefixed_error(err, tmp_err, "[gfal_aut_get_srm_endpoint]");
				return -4;
			}
		*endpoint = tmp_endpoint;
		*srm_type= handle->srm_proto_type;
		return 0;
	}
	return -1;
}

/**
 * initiate a gfal's context with default parameters for use
 * @return a gfal_handle, need to be free after usage or NULL if errors
 */
gfal_handle gfal_initG (GError** err)
{
	gfal_handle handle = calloc(1,sizeof(struct gfal_handle_));// clear allocation of the struct and set defautl options
	if(handle == NULL){
		errno= ENOMEM;
		g_set_error(err,0,ENOMEM, "[gfal_initG] bad allocation, no more memory free");
		return NULL;
	}
	handle->err= NULL;
	handle->srm_proto_type = PROTO_SRMv2;
	handle->initiated = 1;
	handle->srmv2_opt = calloc(1,sizeof(struct _gfal_srmv2_opt));	// define the srmv2 option struct and clear it
	/*
    int i = 0;
    char **se_endpoints;
    char **se_types;
    char *srmv1_endpoint = NULL;
    char *srmv2_endpoint = NULL;
    int isclassicse = 0;
    int endpoint_offset=0;

    if (req == NULL || req->nbfiles < 0 || (req->endpoint == NULL && req->surls == NULL)) {
        gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR,
                "[GFAL][gfal_init][EINVAL] Invalid request: Endpoint or SURLs must be specified");
        errno = EINVAL;
        return (-1);
    }
    if (req->oflag != 0 && req->filesizes == NULL) {
        gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR,
                "[GFAL][gfal_init][EINVAL] Invalid request: File sizes must be specified for put requests");
        errno = EINVAL;
        return (-1);
    }
    if (req->srmv2_lslevels > 1) {
        gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR,
                "[GFAL][gfal_init][EINVAL] Invalid request: srmv2_lslevels must be 0 or 1");
        errno = EINVAL;
        return (-1);
    }

    if ((*gfal = (gfal_handle) malloc (sizeof (struct gfal_handle_))) == NULL) {
        errno = ENOMEM;
        return (-1);
    }

    memset (*gfal, 0, sizeof (struct gfal_handle_));
    memcpy (*gfal, req, sizeof (struct gfal_request_));
    /// Use default SRM timeout if not specified in request 
    if (!(*gfal)->timeout)
        (*gfal)->timeout = gfal_get_timeout_srm ();

    if ((*gfal)->no_bdii_check) {
        if ((*gfal)->surls != NULL && ((*gfal)->setype != TYPE_NONE ||
                    ((*gfal)->setype = (*gfal)->defaultsetype) != TYPE_NONE)) {
            if ((*gfal)->setype == TYPE_SE) {
                gfal_handle_free (*gfal);
                *gfal = NULL;
                gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR,
                        "[GFAL][gfal_init][EINVAL] Invalid request: Disabling BDII checks is not compatible with Classic SEs");
                errno = EINVAL;
                return (-1);
            }
            else if ((*gfal)->setype != TYPE_SE && (*gfal)->endpoint == NULL && ((*gfal)->free_endpoint = 1) &&
                    ((*gfal)->endpoint = endpointfromsurl ((*gfal)->surls[0], errbuf, errbufsz, 1)) == NULL) {
                gfal_handle_free (*gfal);
                *gfal = NULL;
                return (-1);
            }
            else {
                // Check if the endpoint is full or not 
                if(strncmp ((*gfal)->endpoint, ENDPOINT_DEFAULT_PREFIX, ENDPOINT_DEFAULT_PREFIX_LEN) == 0)
                    endpoint_offset = ENDPOINT_DEFAULT_PREFIX_LEN;
                else
                    endpoint_offset = 0;
                const char *s = strchr ((*gfal)->endpoint + endpoint_offset, '/');
                const char *p = strchr ((*gfal)->endpoint + endpoint_offset, ':');

                if (((*gfal)->setype == TYPE_SRMv2 && s == NULL) || p == NULL || (s != NULL && s < p)) {
                    gfal_handle_free (*gfal);
                    *gfal = NULL;
                    gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR,
                            "[GFAL][gfal_init][EINVAL] Invalid request: When BDII checks are disabled, you must provide full endpoint");
                    errno = EINVAL;
                    return (-1);
                }

                return (0);
            }

        } else {
            gfal_handle_free (*gfal);
            *gfal = NULL;
            gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR,
                    "[GFAL][gfal_init][EINVAL] Invalid request: When BDII checks are disabled, you must provide SURLs and endpoint types");
            errno = EINVAL;
            return (-1);
        }
    }

    if ((*gfal)->endpoint == NULL) {
        // (*gfal)->surls != NULL 
        if ((*gfal)->surls[0] != NULL) {
            if (((*gfal)->endpoint = endpointfromsurl ((*gfal)->surls[0], errbuf, errbufsz, 0)) == NULL)
                return (-1);
            (*gfal)->free_endpoint = 1;
        } else {
            gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR,
                    "[GFAL][gfal_init][EINVAL] Invalid request: You have to specify either an endpoint or at least one SURL");
            gfal_handle_free (*gfal);
            *gfal = NULL;
            errno = EINVAL;
            return (-1);
        }
    }
    if ((strchr ((*gfal)->endpoint, '.') == NULL)) {
        gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR,
                "[GFAL][gfal_init][EINVAL] No domain name specified for storage element endpoint");
        gfal_handle_free (*gfal);
        *gfal = NULL;
        errno = EINVAL;
        return (-1);
    }
    if (setypesandendpoints ((*gfal)->endpoint, &se_types, &se_endpoints, errbuf, errbufsz) < 0) {
        gfal_handle_free (*gfal);
        *gfal = NULL;
        return (-1);
    }

    while (se_types[i]) {
        if (srmv1_endpoint == NULL && strcmp (se_types[i], "srm_v1") == 0)
            srmv1_endpoint = se_endpoints[i];
        else if (srmv2_endpoint == NULL && strcmp (se_types[i], "srm_v2") == 0)
            srmv2_endpoint = se_endpoints[i];
        else {
            free (se_endpoints[i]);
            if ((strcmp (se_types[i], "disk")) == 0)
                isclassicse = 1;
        }

        free (se_types[i]);
        ++i;
    }

    free (se_types);
    free (se_endpoints);

    if ((*gfal)->surls != NULL && strncmp ((*gfal)->surls[0], "sfn:", 4) == 0 &&
            (isclassicse || srmv1_endpoint || srmv2_endpoint)) {
        // if surls start with sfn:, we force the SE type to classic SE //
        (*gfal)->setype = TYPE_SE;
        if (srmv1_endpoint) free (srmv1_endpoint);
        if (srmv2_endpoint) free (srmv2_endpoint);
        return (0);
    }

    // srmv2 is the default if nothing specified by user! //
    if ((*gfal)->setype == TYPE_NONE) {
        if (srmv2_endpoint &&
                ((*gfal)->defaultsetype == TYPE_NONE || (*gfal)->defaultsetype == TYPE_SRMv2 ||
                 ((*gfal)->defaultsetype == TYPE_SRM && !srmv1_endpoint))) {
            (*gfal)->setype = TYPE_SRMv2;
        } else if (!(*gfal)->srmv2_spacetokendesc && !(*gfal)->srmv2_desiredpintime &&
                !(*gfal)->srmv2_lslevels && !(*gfal)->srmv2_lsoffset &&	!(*gfal)->srmv2_lscount) {
            if (srmv1_endpoint && (*gfal)->defaultsetype != TYPE_SE)
                (*gfal)->setype = TYPE_SRM;
            else if (srmv2_endpoint || srmv1_endpoint || isclassicse)
                (*gfal)->setype = TYPE_SE;
        }
    }
    else if ((*gfal)->setype == TYPE_SRMv2 && !srmv2_endpoint) {
        (*gfal)->setype = TYPE_NONE;
    } else if ((*gfal)->srmv2_spacetokendesc || (*gfal)->srmv2_desiredpintime ||
            (*gfal)->srmv2_lslevels || (*gfal)->srmv2_lsoffset || (*gfal)->srmv2_lscount) {
        (*gfal)->setype = TYPE_NONE;
    } else {
        if ((*gfal)->setype == TYPE_SRM && !srmv1_endpoint)
            (*gfal)->setype = TYPE_NONE;
        else if ((*gfal)->setype == TYPE_SE && !srmv2_endpoint && !srmv1_endpoint && !isclassicse)
            (*gfal)->setype = TYPE_NONE;
    }

    if ((*gfal)->setype == TYPE_SRMv2) {
        if ((*gfal)->free_endpoint) free ((*gfal)->endpoint);
        (*gfal)->endpoint = srmv2_endpoint;
        (*gfal)->free_endpoint = 1;
        if (srmv1_endpoint) free (srmv1_endpoint);
    } else if ((*gfal)->setype == TYPE_SRM) {
        if ((*gfal)->free_endpoint) free ((*gfal)->endpoint);
        (*gfal)->endpoint = srmv1_endpoint;
        (*gfal)->free_endpoint = 1;
        if (srmv2_endpoint) free (srmv2_endpoint);
    } else if ((*gfal)->setype == TYPE_NONE) {
        gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR,
                "[GFAL][gfal_init][EINVAL] Invalid request: Desired SE type doesn't match request parameters or SE");
        gfal_handle_free (*gfal);
        *gfal = NULL;
        if (srmv1_endpoint) free (srmv1_endpoint);
        if (srmv2_endpoint) free (srmv2_endpoint);
        errno = EINVAL;
        return (-1);
    }

    if ((*gfal)->generatesurls) {
        if ((*gfal)->surls == NULL) {
            if (generate_surls (*gfal, errbuf, errbufsz) < 0)
                return (-1);
        } else {
            gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR,
                    "[GFAL][gfal_init][EINVAL] Invalid request: No SURLs must be specified with 'generatesurls' activated");
            gfal_handle_free (*gfal);
            *gfal = NULL;
            errno = EINVAL;
            return (-1);
        }
    }
	*/
}

/**
 * @brief accessor for the default storage type definition
 * */
void gfal_set_default_storageG(gfal_handle handle, enum gfal_srm_proto proto){
	handle->srm_proto_type = proto;
}

/**
 * @biref convert glist to surl char* to char**
 *  @return return NULL if error or pointer to char**
 */
char** gfal_GList_to_tab(GList* surls){
	int surl_size = g_list_length(surls);
	int i;
	char **resu = surl_size?((char**)calloc(surl_size+1, sizeof(char*))):NULL;
	for(i=0;i<surl_size; ++i){
		resu[i]= surls->data;
		surls = g_list_next(surls);
	}
	return resu;
}


/**
 * parse a surl to check the validity
 */
int gfal_surl_checker(const char* surl, GError** err){
	regex_t rex;
	int ret = regcomp(&rex, "^srm://([:alnum:]|-|/|\.|_)+$",REG_ICASE | REG_EXTENDED);
	g_return_val_err_if_fail(ret==0,-1,err,"[gfal_surl_checker_] fail to compile regex, report this bug");
	ret=  regexec(&rex,surl,0,NULL,0);
	if(ret) 
		g_set_error(err,0,EINVAL,"[gfal_surl_checker] Incorrect surl, impossible to parse surl %s :", surl);
	return ret;
} 


/**
 *  @brief execute a srmv2 request async
*/
static int gfal_srmv2_getasync(gfal_handle handle, char* endpoint, GList* surls, GError** err){
	g_return_val_err_if_fail(surls!=NULL,-1,err,"[gfal_srmv2_getasync] GList passed null");
			
	GError* tmp_err=NULL;
	struct srm_context context;
	int ret=0,i=0;
	struct srm_preparetoget_input preparetoget_input;
	struct srm_preparetoget_output preparetoget_output;
	const int size = 2048;
	
	char errbuf[size] ; memset(errbuf,0,size*sizeof(char));
	const gfal_srmv2_opt* opts = handle->srmv2_opt;							// get default opts for srmv2
	int surl_size = g_list_length(surls);										// get the length of glist
	
	char**  surls_tab = gfal_GList_to_tab(surls);								// convert glist
	
	
	// set the structures datafields	
	preparetoget_input.desiredpintime = opts->opt_srmv2_desiredpintime;		
	preparetoget_input.nbfiles = surl_size;
	preparetoget_input.protocols = opts->opt_srmv2_protocols;
	preparetoget_input.spacetokendesc = opts->opt_srmv2_spacetokendesc;
	preparetoget_input.surls = surls_tab;	
	srm_context_init(&context, endpoint, errbuf, size, gfal_get_verbose());	
	
	
	ret = srm_prepare_to_get_async(&context,&preparetoget_input,&preparetoget_output);
	if(ret != 0){
		g_set_error(err,0,errno,"[gfal_srmv2_getasync] call to srm_ifce error: %s",errbuf);
	} else{
		handle->last_request_state = calloc(1, sizeof(struct _gfal_request_state));
    	handle->last_request_state->srmv2_token = preparetoget_output.token;
    	handle->last_request_state->srmv2_pinstatuses = preparetoget_output.filestatuses;		
	}

	free(endpoint);
	free(surls_tab);
	return ret;	
}



/**
 * @brief launch a surls-> turls translation in asynchronous mode
 * @warning need a initiaed gfal_handle
 * @param handle : the gfal_handle initiated ( \ref gfal_init )
 * @param surls : GList of string of the differents surls to convert
 * @param err : GError for error report
 * @return return 0 if success else -1, check GError for more information
 */
int gfal_get_asyncG(gfal_handle handle, GList* surls, GError** err){
	g_return_val_err_if_fail(handle!=NULL,-1,err,"[gfal_get_asyncG] handle passed is null");
	g_return_val_err_if_fail(surls!=NULL,-2,err,"[gfal_get_asyncG] surls arg passed is null");
	g_return_val_err_if_fail(g_list_last(surls) != NULL,-3,err,"[gfal_get_asyncG] surls arg passed is empty");
	
	GError* tmp_err=NULL;
	GList* tmp_list = surls;
	int ret=0;
	
	
	if( !gfal_handle_checkG(handle, &tmp_err) ){	// check handle validity
		g_propagate_prefixed_error(err,tmp_err,"[gfal_get_asyncG]");
		return -1;
	}
	while(tmp_list != NULL){							// check all urls if valids
		if( gfal_surl_checker(tmp_list->data,&tmp_err) != 0){
			g_propagate_prefixed_error(err,tmp_err,"[gfal_get_asyncG]");	
			return -1;
		}
		tmp_list = g_list_next(tmp_list);		
	}
			
	char* full_endpoint=NULL;
	enum gfal_srm_proto srm_types;
	if((ret = gfal_auto_get_srm_endpoint(handle,&full_endpoint,&srm_types, surls, &tmp_err)) != 0){		// check & get endpoint										
		g_propagate_prefixed_error(err,tmp_err, "[gfal_get_asyncG]");
		return -1;
	}
	
	if (srm_types == PROTO_SRMv2){
		ret= gfal_srmv2_getasync(handle, full_endpoint, surls,&tmp_err);
		if(ret<0)
			g_propagate_prefixed_error(err, tmp_err, "[gfal_get_asyncG]");
	} else if(srm_types == PROTO_SRM){
			
	} else{
		ret=-1;
		g_set_error(err,0,EPROTONOSUPPORT, "[gfal_get_asyncG] Type de protocole spécifié non supporté ( Supportés : SRM & SRMv2 ) ");
	}
	return ret;

	/*int ret;

    if (check_gfal_handle (req, 0, errbuf, errbufsz) < 0)
        return (-1);

    if (req->setype == TYPE_SRMv2)
    {
    	struct srm_context context;
    	struct srm_preparetoget_input preparetoget_input;
    	struct srm_preparetoget_output preparetoget_output;

    	srm_context_init(&context,req->endpoint,errbuf,errbufsz,gfal_get_verbose());

        if (req->srmv2_pinstatuses)
        {
            free (req->srmv2_pinstatuses);
            req->srmv2_pinstatuses = NULL;
        }
        if (req->srmv2_token)
        {
            free (req->srmv2_token);
            req->srmv2_token = NULL;
        }

        preparetoget_input.desiredpintime = req->srmv2_desiredpintime;
        preparetoget_input.nbfiles = req->nbfiles;
        preparetoget_input.protocols = req->protocols;
        preparetoget_input.spacetokendesc = req->srmv2_spacetokendesc;
        preparetoget_input.surls = req->surls;

        ret = srm_prepare_to_get_async(&context,&preparetoget_input,&preparetoget_output);

    	req->srmv2_token = preparetoget_output.token;
    	req->srmv2_pinstatuses = preparetoget_output.filestatuses;


        //TODO ret = srmv2_gete (req->nbfiles, (const char **) req->surls, req->endpoint,
         //   req->srmv2_desiredpintime, req->srmv2_spacetokendesc, req->protocols,
         //   &(req->srmv2_token), &(req->srmv2_pinstatuses), errbuf, errbufsz, req->timeout);

    } else if (req->setype == TYPE_SRM) {
        if (req->srm_statuses) {
            free (req->srm_statuses);
            req->srm_statuses = NULL;
        }
//TODO REMOVE        ret = srm_getxe (req->nbfiles, (const char **) req->surls, req->endpoint, req->protocols,
 //               &(req->srm_reqid), &(req->srm_statuses), errbuf, errbufsz, req->timeout);
    } else { // req->setype == TYPE_SE
        gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR, "[GFAL][gfal_get][EPROTONOSUPPORT] SFNs aren't supported");
        errno = EPROTONOSUPPORT;
        return (-1);;
    }

    req->returncode = ret;
    return (copy_gfal_results (req, PIN_STATUS));*/
	
}

/**
 * @brief free a gfal's handle, safe if null
 * 
 */
void gfal_handle_freeG (gfal_handle handle){
	if(handle == NULL)
		return;
	g_clear_error(&(handle->err));
	free(handle->srmv2_opt);
	free(handle->last_request_state);
	free(handle);
	handle = NULL;
}
