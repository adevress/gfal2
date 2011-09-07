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
 * @file gfal_dcap_plugin_main.c
 * @brief file for the external dcap plugin for gfal ( based on the old dcap part in gfal legacy )
 * @author Devresse Adrien
 * @date 20/07/2011
 * 
 **/

#define _GNU_SOURCE

#include <regex.h>
#include <time.h> 
#include <string.h>
#include <stdlib.h>
#include "../gfal_common_internal.h"
#include "../gfal_common_errverbose.h"
#include "../gfal_common_catalog.h"
#include "../gfal_types.h"
#include "gfal_dcap_plugin_layer.h"
#include "gfal_dcap_plugin_main.h"
#include "gfal_dcap_plugin_bindings.h"

void gfal_dcap_destroyG(catalog_handle handle);
gboolean gfal_dcap_check_url(catalog_handle ch, const char* url,  catalog_mode mode, GError** err);

static int gfal_dcap_regex_compile(regex_t * rex, GError** err){
	int ret = regcomp(rex, "^(dcap|gsidcap)://([:alnum:]|-|/|\.|_)+$",REG_ICASE | REG_EXTENDED);
	g_return_val_err_if_fail(ret==0,-1,err,"[gfal_dcap_regex_compile] fail to compile regex, report this bug");	
	return ret;
}

static gfal_plugin_dcap_handle gfal_dcap_init_handle(gfal_handle handle, GError** err){
	gfal_plugin_dcap_handle ret = g_new0(struct _gfal_plugin_dcap_handle, 1);
	ret->ops = gfal_dcap_internal_loader(err);
	ret->handle = handle;
	gfal_dcap_regex_compile(&(ret->rex), err);
	return ret;	
}

/**
 * Init function, called before all
 * */
gfal_catalog_interface gfal_plugin_init(gfal_handle handle, GError** err){
	gfal_catalog_interface dcap_catalog;
	GError* tmp_err=NULL;
	memset(&dcap_catalog,0,sizeof(gfal_catalog_interface));	// clear the catalog	
	
	dcap_catalog.handle = (catalog_handle) gfal_dcap_init_handle(handle, &tmp_err);
	
	dcap_catalog.catalog_delete = &gfal_dcap_destroyG;
	dcap_catalog.getName= &gfal_dcap_getName;
	dcap_catalog.openG = &gfal_dcap_openG;
	dcap_catalog.closeG = &gfal_dcap_closeG;
	dcap_catalog.readG = &gfal_dcap_readG;
	dcap_catalog.writeG= &gfal_dcap_writeG;
	dcap_catalog.lseekG= &gfal_dcap_lseekG;
	dcap_catalog.check_catalog_url = &gfal_dcap_check_url;
	dcap_catalog.statG= &gfal_dcap_statG;
	dcap_catalog.lstatG = &gfal_dcap_lstatG;
	
	if(tmp_err)
		g_propagate_prefixed_error(err, tmp_err, "[%s]", __func__);
	return dcap_catalog;
}


void gfal_dcap_destroyG(catalog_handle handle){
	gfal_plugin_dcap_handle h = (gfal_plugin_dcap_handle) handle;
	free(h->ops);
	regfree(&(h->rex));
	free(h);
}


gboolean gfal_dcap_internal_check_url(gfal_plugin_dcap_handle dh, const char* surl, GError** err){
	if(surl == NULL || strnlen(surl, GFAL_URL_MAX_LEN) == GFAL_URL_MAX_LEN){
		g_set_error(err, 0, EINVAL, "[%s] Invalid surl, surl too long or NULL",__func__);
		return FALSE;
	}	
	int ret=  regexec(&dh->rex,surl,0,NULL,0);
	return (ret==0)?TRUE:FALSE;
}


/**
 *  Check the dcap url in the gfal module way
 * */
gboolean gfal_dcap_check_url(catalog_handle ch, const char* url,  catalog_mode mode, GError** err){
	int ret;
	GError* tmp_err=NULL;
	gfal_plugin_dcap_handle rh = (gfal_plugin_dcap_handle) ch;
	switch(mode){
			case GFAL_CATALOG_OPEN:
			case GFAL_CATALOG_LSTAT:
			case GFAL_CATALOG_STAT:
				ret = gfal_dcap_internal_check_url(rh, url, &tmp_err);
				break;
			default:
				ret =  FALSE;
				break;
	}	
	if(tmp_err)
		g_propagate_prefixed_error(err, tmp_err, "[%s]", __func__);
	return ret;
}



