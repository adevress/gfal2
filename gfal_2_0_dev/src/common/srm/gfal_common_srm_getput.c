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
 * @brief file with the get/getasync srm funcs 
 * @author Devresse Adrien
 * @version 2.0
 * @date 12/06/2011
 * */
 
 #define _GNU_SOURCE 

#include <regex.h>
#include <time.h> 
#include <stdio.h>
#include <malloc.h>
 
#include "gfal_common_srm.h"
#include "gfal_srm_request.h"
#include <common/gfal_common_internal.h>
#include <common/gfal_common_errverbose.h>
#include <common/gfal_common_plugin.h>
#include "gfal_common_srm_internal_layer.h"
#include "gfal_common_srm_endpoint.h"






static int gfal_srm_convert_filestatuses_to_srm_result(struct srmv2_pinfilestatus* statuses, char* reqtoken, int n, gfal_srm_result** resu, GError** err){
	g_return_val_err_if_fail(statuses && n && resu, -1, err, "[gfal_srm_convert_filestatuses_to_srm_result] args invalids");
	*resu = calloc(n, sizeof(gfal_srm_result));
	int i=0;
	for(i=0; i< n; ++i){
		if(statuses[i].turl)
			g_strlcpy((*resu)[i].turl, statuses[i].turl, GFAL_URL_MAX_LEN);
		if(statuses[i].explanation)
			g_strlcpy((*resu)[i].err_str, statuses[i].explanation, GFAL_URL_MAX_LEN);
		(*resu)[i].err_code = statuses[i].status;	
		(*resu)[i].reqtoken = reqtoken;
	}
	return 0;
}

int gfal_srmv2_get_global(gfal_srm_plugin_t handle, gfal_srm_params_t params, struct srm_context* context,
						struct srm_preparetoget_input* input, gfal_srm_result** resu,  GError** err){
	g_return_val_err_if_fail(handle != NULL && input != NULL && resu != NULL,-1,err,"[gfal_srmv2_get_global] tab null ");

	GError* tmp_err=NULL;
	char errbuf[GFAL_URL_MAX_LEN];
	errbuf[0]='\0';
	int ret=0;
	struct srm_preparetoget_output preparetoget_output;
	
	ret = gfal_srm_external_call.srm_prepare_to_get(context,input,&preparetoget_output);
	if(ret < 0){
		gfal_srm_report_error(errbuf, &tmp_err);
	} else{
		gfal_srm_convert_filestatuses_to_srm_result(preparetoget_output.filestatuses, preparetoget_output.token, ret, resu,  &tmp_err);
    	gfal_srm_external_call.srm_srmv2_pinfilestatus_delete(preparetoget_output.filestatuses, ret);
    	gfal_srm_external_call.srm_srm2__TReturnStatus_delete(preparetoget_output.retstatus);
	}
	G_RETURN_ERR(ret, tmp_err, err);		
}


int gfal_srmv2_put_global(gfal_srm_plugin_t handle, gfal_srm_params_t params, struct srm_context* context,
						struct srm_preparetoput_input* input, gfal_srm_result** resu,  GError** err){
	g_return_val_err_if_fail(handle != NULL && input != NULL && resu != NULL,-1,err,"[gfal_srmv2_put_global] tab null ");

	GError* tmp_err=NULL;
	char errbuf[GFAL_URL_MAX_LEN];
	errbuf[0]='\0';
	int ret=0;
	struct srm_preparetoput_output preparetoput_output;
	
	ret = gfal_srm_external_call.srm_prepare_to_put(context, input, &preparetoput_output);
	if(ret < 0){
		gfal_srm_report_error(errbuf, &tmp_err);
	} else{
		gfal_srm_convert_filestatuses_to_srm_result(preparetoput_output.filestatuses, preparetoput_output.token, ret, resu, &tmp_err);
    	gfal_srm_external_call.srm_srmv2_pinfilestatus_delete(preparetoput_output.filestatuses, ret);
    	gfal_srm_external_call.srm_srm2__TReturnStatus_delete(preparetoput_output.retstatus);
	}
	G_RETURN_ERR(ret, tmp_err, err);		
}




/**
 *  @brief execute a srmv2 request sync "GET" on the srm_ifce
*/
int gfal_srm_getTURLS_srmv2_internal(gfal_srmv2_opt* opts,  gfal_srm_params_t params, char* endpoint, char** surls, gfal_srm_result** resu,  GError** err){
	g_return_val_err_if_fail(surls!=NULL,-1,err,"[gfal_srmv2_getasync] tab null ");
			
	GError* tmp_err=NULL;
	struct srm_context context;
	int ret=0;
	struct srm_preparetoget_input preparetoget_input;
	const int err_size = 2048;
	
	char errbuf[err_size]; errbuf[0]='\0';
	size_t n_surl = g_strv_length (surls);									// n of surls
		
	// set the structures datafields	
	preparetoget_input.desiredpintime = opts->opt_srmv2_desiredpintime;		
	preparetoget_input.nbfiles = n_surl;
	preparetoget_input.protocols = gfal_srm_params_get_protocols(params);
	preparetoget_input.spacetokendesc = opts->opt_srmv2_spacetokendesc;
	preparetoget_input.surls = surls;	
	gfal_srm_external_call.srm_context_init(&context, endpoint, errbuf, err_size, gfal_get_verbose());	
	
	ret = gfal_srmv2_get_global(opts, params, &context, &preparetoget_input, resu, &tmp_err);
	G_RETURN_ERR(ret, tmp_err, err);
}




/**
 *  @brief execute a srmv2 request sync "PUT" on the srm_ifce
*/
int gfal_srm_putTURLS_srmv2_internal(gfal_srmv2_opt* opts , gfal_srm_params_t params, char* endpoint, char** surls, gfal_srm_result** resu,  GError** err){
	g_return_val_err_if_fail(surls!=NULL,-1,err,"[gfal_srm_putTURLS_srmv2_internal] GList passed null");
			
	GError* tmp_err=NULL;
	struct srm_context context;
	int ret=0,i=0;
	struct srm_preparetoput_input preparetoput_input;
	const int err_size = 2048;

	
	char errbuf[err_size]; errbuf[0]='\0';
	size_t n_surl = g_strv_length (surls);									// n of surls
	gint64 filesize_tab[n_surl];
	for(i=0; i < n_surl;++i)
		filesize_tab[i] = opts->filesizes;
		
	// set the structures datafields	
	preparetoput_input.desiredpintime = opts->opt_srmv2_desiredpintime;		
	preparetoput_input.nbfiles = n_surl;
	preparetoput_input.protocols = gfal_srm_params_get_protocols(params);
	preparetoput_input.spacetokendesc = opts->opt_srmv2_spacetokendesc;
	preparetoput_input.surls = surls;	
	preparetoput_input.filesizes = filesize_tab;
	gfal_srm_external_call.srm_context_init(&context, endpoint, errbuf, err_size, gfal_get_verbose());	
	
	
	ret = gfal_srmv2_put_global(opts, params, &context, &preparetoput_input, resu, &tmp_err);
	G_RETURN_ERR(ret, tmp_err, err);	
}

/***
 * Internal function of gfal_srm_getTurls without argument check for internal usage
 * 
 * */
int gfal_srm_mTURLS_internal(gfal_srmv2_opt* opts, gfal_srm_params_t params, srm_req_type req_type, char** surls, gfal_srm_result** resu,   GError** err){
	GError* tmp_err=NULL;
	int ret=-1;	

	char full_endpoint[GFAL_URL_MAX_LEN];
	enum gfal_srm_proto srm_types;

	if((gfal_srm_determine_endpoint(opts, *surls, full_endpoint, GFAL_URL_MAX_LEN, &srm_types, &tmp_err)) == 0){		// check & get endpoint										
		gfal_print_verbose(GFAL_VERBOSE_NORMAL, "[gfal_srm_mTURLS_internal] endpoint %s", full_endpoint);

		if (srm_types == PROTO_SRMv2){
			if(req_type == SRM_GET)
				ret= gfal_srm_getTURLS_srmv2_internal(opts, params, full_endpoint, surls, resu,  &tmp_err);
			else
				ret= gfal_srm_putTURLS_srmv2_internal(opts, params, full_endpoint, surls, resu,  &tmp_err);
		} else if(srm_types == PROTO_SRM){
			g_set_error(&tmp_err,0, EPROTONOSUPPORT, "support for SRMv1 is removed in gfal 2.0, failure");
		} else{
			g_set_error(&tmp_err,0,EPROTONOSUPPORT, "Unknow SRM protocol, failure ");
		}		
	}

	if(tmp_err)
		g_propagate_prefixed_error(err, tmp_err, "[%s]", __func__);
	return ret;		
}

/**
 *  simple wrapper to getTURLs for the gfal_module layer
 * */
int gfal_srm_getTURLS_plugin(plugin_handle ch, const char* surl, char* buff_turl, int size_turl, char** reqtoken,  GError** err){
	gfal_srmv2_opt* opts = (gfal_srmv2_opt*)ch;
	gfal_srm_result* resu=NULL;
	GError* tmp_err=NULL;
	char* surls[]= { (char*)surl, NULL };
	int ret = -1;
	
	gfal_srm_params_t params = gfal_srm_params_new(opts, & tmp_err);
	if(params != NULL){
		ret= gfal_srm_mTURLS_internal(opts, params, SRM_GET, surls, &resu, &tmp_err);
		if(ret >0){
			if(resu[0].err_code == 0){
				g_strlcpy(buff_turl, resu[0].turl, size_turl);			
				if(reqtoken)
					*reqtoken = resu[0].reqtoken;
				ret=0;			
			}else{
				g_set_error(&tmp_err,0 , resu[0].err_code, " error on the turl request : %s ", resu[0].err_str);
				ret = -1;
			}
			free(resu);
		}
		gfal_srm_params_free(params);
	}	
	if(tmp_err)
		g_propagate_prefixed_error(err, tmp_err, "[%s]", __func__);
	return ret;			
}


/**
 *  execute a get for thirdparty transfer turl
 *  @param ch : plugin handle
 *  @param surl : surl to resovle in turl
 *  @param buff_turl : buffer where write the result
 *  @param size_tur : maximum size of the buffer to use
 *  @param request token if needed
 *  @param gerror : err report
 * */
int gfal_srm_get_rd3_turl(plugin_handle ch, const char* surl, char* buff_turl, int size_turl, char** reqtoken,  GError** err){
	gfal_srmv2_opt* opts = (gfal_srmv2_opt*)ch;
	gfal_srm_result* resu=NULL;
	GError* tmp_err=NULL;
	char* surls[]= { (char*)surl, NULL };
	int ret = -1;
	
	gfal_srm_params_t params = gfal_srm_params_new(opts, & tmp_err);
	gfal_srm_params_set_protocols(params, opts->opt_srmv2_tp3_protocols);
	if(params != NULL){
	ret= gfal_srm_mTURLS_internal(opts, params, SRM_GET, surls, &resu, &tmp_err);
		if(ret >=0){
			if(resu[0].err_code == 0){
				g_strlcpy(buff_turl, resu[0].turl, size_turl);			
				if(reqtoken)
					*reqtoken = resu[0].reqtoken;
				ret=0;			
			}else{
				g_set_error(&tmp_err,0 , resu[0].err_code, " error on the turl request : %s ", resu[0].err_str);
				ret = -1;
			}
			free(resu);
		}
		gfal_srm_params_free(params);
	}	
	G_RETURN_ERR(ret, tmp_err, err);		
}

/**
 * @brief launch a surls-> turls translation in the synchronous mode
 * @param opts : srmv2 opts initiated
 * @param surls : tab of string, last char* must be NULL
 * @param resu : pointer to a table of gfal_srm_result
 * @param err : GError** for error report
 * @return return positive if success else -1, check GError for more information
 */
int gfal_srm_getTURLS(gfal_srmv2_opt* opts, char** surls, gfal_srm_result** resu,  GError** err){
	g_return_val_err_if_fail(opts!=NULL,-1,err,"[gfal_srm_getTURLS] handle passed is null");
	
	GError* tmp_err=NULL;
	int ret=-1;
	
	gfal_srm_params_t params = gfal_srm_params_new(opts, & tmp_err);
	if(params != NULL){
		if( gfal_srm_surl_group_checker	(opts, surls, &tmp_err) == TRUE){				
			ret = gfal_srm_mTURLS_internal(opts, params, SRM_GET, surls, resu,  &tmp_err);
		}
		gfal_srm_params_free(params);
	}
	G_RETURN_ERR(ret, tmp_err, err);
}

/**
 *  execute a put for thirdparty transfer turl
 *  @param ch : plugin handle
 *  @param surl : surl to resovle in turl
 *  @param buff_turl : buffer where write the result
 *  @param size_tur : maximum size of the buffer to use
 *  @param request token if needed
 *  @param gerror : err report
 * */
int gfal_srm_put_rd3_turl(plugin_handle ch, const char* surl, char* buff_turl, int size_turl, char** reqtoken, GError** err){
	gfal_srmv2_opt* opts = (gfal_srmv2_opt*)ch;
	gfal_srm_result* resu=NULL;
	GError* tmp_err=NULL;
	char* surls[]= { (char*)surl, NULL };
	int ret = -1;
	
	gfal_srm_params_t params = gfal_srm_params_new(opts, & tmp_err);
	gfal_srm_params_set_protocols(params, opts->opt_srmv2_tp3_protocols);	
	if(params != NULL){
		ret= gfal_srm_mTURLS_internal(opts, params, SRM_PUT, surls, &resu,  &tmp_err);
		if(ret >=0){
			if(resu[0].err_code == 0){
				g_strlcpy(buff_turl, resu[0].turl, size_turl);
				if(reqtoken)
					*reqtoken = resu[0].reqtoken;
				ret=0;			
			}else{
				g_set_error(&tmp_err,0 , resu[0].err_code, " error on the turl request : %s ", resu[0].err_str);
				ret = -1;
			}
		
		}
		gfal_srm_params_free(params);
	}
	
	if(tmp_err)
		g_propagate_prefixed_error(err, tmp_err, "[%s]", __func__);
	return ret;			
}


/**
 *  simple wrapper to putTURLs for the gfal_module layer
 * */
int gfal_srm_putTURLS_plugin(plugin_handle ch, const char* surl, char* buff_turl, int size_turl, char** reqtoken, GError** err){
	gfal_srmv2_opt* opts = (gfal_srmv2_opt*)ch;
	gfal_srm_result* resu=NULL;
	GError* tmp_err=NULL;
	char* surls[]= { (char*)surl, NULL };
	int ret = -1;
	
	gfal_srm_params_t params = gfal_srm_params_new(opts, & tmp_err);
	if(params != NULL){
		ret= gfal_srm_mTURLS_internal(opts, params, SRM_PUT, surls, &resu,  &tmp_err);
		if(ret >0){
			if(resu[0].err_code == 0){
				g_strlcpy(buff_turl, resu[0].turl, size_turl);
				if(reqtoken)
					*reqtoken = resu[0].reqtoken;
				ret=0;			
			}else{
				g_set_error(&tmp_err,0 , resu[0].err_code, " error on the turl request : %s ", resu[0].err_str);
				ret = -1;
			}
		
		}
		gfal_srm_params_free(params);
	}
	
	if(tmp_err)
		g_propagate_prefixed_error(err, tmp_err, "[%s]", __func__);
	return ret;			
}

/**
 * @brief launch a surls-> turls translation in the synchronous mode for file creation
 * @param opts : srmv2 opts initiated
 * @param surls : tab of string, last char* must be NULL
 * @param resu : pointer to a table of gfal_srm_result
 * @param err : GError** for error report
 * @return return positive if success else -1, check GError for more information
 */
int gfal_srm_putTURLS(gfal_srmv2_opt* opts , char** surls, gfal_srm_result** resu,  GError** err){
	g_return_val_err_if_fail(opts!=NULL,-1,err,"[gfal_srm_putTURLS] handle passed is null");
	
	GError* tmp_err=NULL;
	int ret=-1;
	
	gfal_srm_params_t params = gfal_srm_params_new(opts, & tmp_err);
	if(params != NULL){
		if( gfal_srm_surl_group_checker	(opts, surls, &tmp_err) == TRUE){				
			ret = gfal_srm_mTURLS_internal(opts, params, SRM_PUT, surls, resu, &tmp_err);
		}
		gfal_srm_params_free(params);
	}
	if(tmp_err)
		g_propagate_prefixed_error(err, tmp_err, "[%s]", __func__);
	return ret;	
}


/**
 *  execute a srm put done on the specified surl and token, return 0 if success else -1 and errno is set
*/
static int gfal_srm_putdone_srmv2_internal(gfal_srmv2_opt* opts, char* endpoint, char** surls, char* token,  GError** err){
	g_return_val_err_if_fail(surls!=NULL,-1,err,"[gfal_srm_putdone_srmv2_internal] invalid args ");
			
	GError* tmp_err=NULL;
	struct srm_context context;
	int ret=0;
	struct srm_putdone_input putdone_input;
	struct srmv2_filestatus *statuses;
	const int err_size = 2048;
	
	char errbuf[err_size] ; memset(errbuf,0,err_size*sizeof(char));
	size_t n_surl = g_strv_length (surls);									// n of surls
		
	// set the structures datafields	
	putdone_input.nbfiles = n_surl;
	putdone_input.reqtoken = token;
	putdone_input.surls = surls;

	gfal_srm_external_call.srm_context_init(&context, endpoint, errbuf, err_size, gfal_get_verbose());	

	gfal_print_verbose(GFAL_VERBOSE_TRACE, "    [gfal_srm_putdone_srmv2_internal] start srm put done on %s", surls[0]);	
	ret = gfal_srm_external_call.srm_put_done(&context,&putdone_input, &statuses);
	if(ret < 0){
		g_set_error(&tmp_err,0,errno,"call to srm_ifce error: %s",errbuf);
	} else{
    	 ret = gfal_srm_convert_filestatuses_to_GError(statuses, ret, &tmp_err);
    	 gfal_srm_external_call.srm_srmv2_filestatus_delete(statuses, n_surl);
	}
	if(tmp_err)
		g_propagate_prefixed_error(err, tmp_err, "[%s]", __func__);
	return ret;	
}

int gfal_srm_putdone(gfal_srmv2_opt* opts , char** surls, char* token,  GError** err){
	GError* tmp_err=NULL;
	int ret=-1;	

	char full_endpoint[2048];
	enum gfal_srm_proto srm_types;
	gfal_print_verbose(GFAL_VERBOSE_TRACE, "   -> [gfal_srm_putdone] ");
	
	if((gfal_srm_determine_endpoint(opts, *surls, full_endpoint, GFAL_URL_MAX_LEN, &srm_types, &tmp_err)) == 0){		// check & get endpoint										
		gfal_print_verbose(GFAL_VERBOSE_NORMAL, "[gfal_srm_putdone] endpoint %s", full_endpoint);

		if (srm_types == PROTO_SRMv2){
			ret = gfal_srm_putdone_srmv2_internal(opts, full_endpoint, surls, token, &tmp_err);
		} else if(srm_types == PROTO_SRM){
			g_set_error(&tmp_err,0, EPROTONOSUPPORT, "support for SRMv1 is removed in gfal 2.0, failure");
		} else{
			g_set_error(&tmp_err,0,EPROTONOSUPPORT, "Unknow SRM protocol, failure ");
		}		
	}
	gfal_print_verbose(GFAL_VERBOSE_TRACE, "   [gfal_srm_putdone] <-");

	if(tmp_err)
		g_propagate_prefixed_error(err, tmp_err, "[%s]", __func__);
	return ret;		
	
}

int gfal_srm_putdone_simple(plugin_handle * handle , const char* surl, char* token,  GError** err){
	gfal_srmv2_opt* opts = (gfal_srmv2_opt*)handle;
	char* surls[]= { (char*)surl, NULL };
	return gfal_srm_putdone(opts, surls, token, err);
}




