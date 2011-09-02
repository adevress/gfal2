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
 * @file gfal_common_srm_stat.c
 * @brief file for the stat function on the srm url type
 * @author Devresse Adrien
 * @version 2.0
 * @date 16/05/2011
 * */
#include "gfal_common_srm.h"
#include "gfal_common_srm_access.h"
#include "../gfal_constants.h"
#include "../gfal_common_errverbose.h"
#include "gfal_common_srm_internal_layer.h" 
#include "gfal_common_srm_endpoint.h"



static int gfal_statG_srmv2_internal(gfal_srmv2_opt* opts, struct stat* buf, const char* endpoint, const char* surl, GError** err){
	g_return_val_err_if_fail( opts && endpoint && surl 
								 && buf && (sizeof(struct stat) == sizeof(struct stat64)),
								-1, err, "[gfal_statG_srmv2_internal] Invalid args handle/endpoint or invalid stat sturct size");
	GError* tmp_err=NULL;
	struct srm_context context;
	struct srm_ls_input input;
	struct srm_ls_output output;
	struct srmv2_mdfilestatus *srmv2_mdstatuses=NULL;
	const int nb_request=1;
	char errbuf[GFAL_ERRMSG_LEN]={0};
	int ret=-1;
	char* tab_surl[] = { (char*)surl, NULL};
	
	gfal_srm_external_call.srm_context_init(&context, (char*)endpoint, errbuf, GFAL_ERRMSG_LEN, gfal_get_verbose());	// init context
	
	input.nbfiles = nb_request;
	input.surls = tab_surl;
	input.numlevels = 0;
	input.offset = 0;
	input.count = 0;

	ret = gfal_srm_external_call.srm_ls(&context,&input,&output);					// execute ls

	if(ret >=0){
		srmv2_mdstatuses = output.statuses;
		if(srmv2_mdstatuses->status != 0){
			g_set_error(err, 0, srmv2_mdstatuses->status, "[%s] Error reported from srm_ifce : %d %s", __func__, 
							srmv2_mdstatuses->status, srmv2_mdstatuses->explanation);
			ret = -1;
		} else {
			memcpy(buf, &(srmv2_mdstatuses->stat), sizeof(struct stat));
			ret = 0;
			errno =0;
		}
	}else{
		gfal_srm_report_error(errbuf, &tmp_err);
		ret=-1;
	}
	gfal_srm_external_call.srm_srmv2_mdfilestatus_delete(srmv2_mdstatuses, 1);
	gfal_srm_external_call.srm_srm2__TReturnStatus_delete(output.retstatus);
	if(tmp_err)
		g_propagate_prefixed_error(err, tmp_err, "[%s]", __func__);
	return ret;	
}

/**
 * stat call, for the srm interface stat and lstat are the same call !! the default behavior is similar to stat by default and ignore links
 * 
 * */
int gfal_srm_statG(catalog_handle ch, const char* surl, struct stat* buf, GError** err){
	g_return_val_err_if_fail( ch && surl && buf, -1, err, "[gfal_srm_statG] Invalid args in handle/surl/bugg");
	GError* tmp_err = NULL;
	int ret =-1;
	char full_endpoint[GFAL_URL_MAX_LEN];
	char key_buff[GFAL_URL_MAX_LEN];
	enum gfal_srm_proto srm_type;
	gfal_srmv2_opt* opts = (gfal_srmv2_opt*) ch;
	gfal_srm_construct_key(surl, GFAL_SRM_LSTAT_PREFIX, key_buff, GFAL_URL_MAX_LEN);
	
	if( (ret = gsimplecache_take_one_kstr(opts->cache, key_buff, buf)) ==0){
		gfal_print_verbose(GFAL_VERBOSE_DEBUG, " srm_statG -> value taken from the cache");
		ret = 0;
	}else{
		ret =gfal_srm_determine_endpoint(opts, surl, full_endpoint, GFAL_URL_MAX_LEN, &srm_type,   &tmp_err);
		if( ret >=0 ){
			if(srm_type == PROTO_SRMv2){
				ret = gfal_statG_srmv2_internal(opts, buf, full_endpoint, surl, &tmp_err);
			}else if (srm_type == PROTO_SRM){
				g_set_error(&tmp_err, 0, EPROTONOSUPPORT, "support for SRMv1 is removed in 2.0, failure");
				ret = -1;
			}else {
				g_set_error(&tmp_err, 0, EPROTONOSUPPORT, "Unknow version of the protocol SRM , failure");
				ret = -1;			
			}
			
		}
	}
	if(tmp_err)
		g_propagate_prefixed_error(err, tmp_err, "[%s]", __func__);
	return ret;
}
