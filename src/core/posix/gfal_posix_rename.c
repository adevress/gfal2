/*
* Copyright @ Members of the EMI Collaboration, 2010.
* See www.eu-emi.eu for details on the copyright holders.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/


/*
 * @file gfal_posix_rename.c
 * @brief file for the internal rename func
 * @author Devresse Adrien
 * @version 2.0
 * @date 11/05/2011
 * */

#include <stdio.h>
#include <glib.h>

#include <common/gfal_prototypes.h>
#include <common/gfal_types.h>
#include <common/gfal_common_plugin.h>

#include "gfal_posix_internal.h"

#include "gfal_posix_api.h"

int gfal_posix_internal_rename(const char* oldpath, const char* newpath){
	GError* tmp_err = NULL;
	gfal2_context_t handle;
	int ret=-1;

	if( (handle = gfal_posix_instance() ) ==NULL){
		errno = EIO;
		return -1;
	}

    ret = gfal2_rename(handle, oldpath, newpath, &tmp_err);
	if(tmp_err){
		gfal_posix_register_internal_error(handle, "[gfal_rename]", tmp_err);
		errno = tmp_err->code;
	}

	return (ret)?-1:0;
}
