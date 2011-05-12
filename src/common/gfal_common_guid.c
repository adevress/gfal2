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
 * @file gfal_common_guid.c
 * @brief file for the guid translation, use the catalogs
 * @author Devresse Adrien
 * @version 2.0
 * @date 12/05/2011
 * */
 

#include <regex.h>
#include "gfal_common_errverbose.h"
#include "gfal_common_guid.h"
 
 /**
 * parse a guid to check the validity
 */
gboolean gfal_guid_checker(const char* guid, GError** err){
	g_return_val_err_if_fail(guid != NULL,FALSE,err,"[gfal_guid_checker] check URL failed : guid is empty");
	regex_t rex;
	int ret = regcomp(&rex, "^guid:([0-9]|[a-z]|[A-Z]){8}-([0-9]|[a-z]|[A-Z]){4}-([0-9]|[a-z]|[A-Z]){4}-([0-9]|[a-z]|[A-Z]){4}-([0-9]|[a-z]|[A-Z]){12}",REG_ICASE | REG_EXTENDED);
	g_return_val_err_if_fail(ret==0,FALSE,err,"[gfal_guid_checker] fail to compile regex, report this bug");
	ret=  regexec(&rex,guid,0,NULL,0);
	if(ret) 
		g_set_error(err,0,EINVAL,"[gfal_guid_checker] Incorrect guid, impossible to parse guid %s :", guid);
	return (!ret)?TRUE:FALSE;
} 


/**
 * Execute a guid access on the first compatible catalog
 * @param handle 
 * @param guid of the element to check
 * @param mode : mode of the access
 * @param err : Error report system
 * @return return the content of the access call, or negative value if error
 * @warning no url syntax checking
 * */
int gfal_guid_accessG(gfal_handle handle, const char* guid, int mode, GError** err){
	g_return_val_err_if_fail(handle && guid, -1, err,"[gfal_guid_accessG] Invalid arguments handle or/and guid");
	GError* tmp_err=NULL;
	int ret =-1;
	const char * cata_link = gfal_catalog_resolve_guid(handle, guid, &tmp_err); 
	if(cata_link != NULL){
		ret = gfal_catalogs_accessG(handle, cata_link, mode, &tmp_err);
	}
	if(tmp_err)
		g_propagate_prefixed_error(err, tmp_err,"[gfal_guid_accessG]");
	return ret; 
	
}

