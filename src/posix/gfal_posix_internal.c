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
 * @file gfal_posix_local_file.c
 * @brief file for the internal func of the posix interface
 * @author Devresse Adrien
 * @version 2.0
 * @date 06/05/2011
 * */
 
#include "gfal_posix_internal.h"
#include "gfal_common.h"
 
static __thread gfal_handle handle=NULL;

gfal_handle gfal_posix_instance(){
	if(handle == NULL)
		handle= gfal_initG(NULL);
	return handle;	
}


/**
 *  register the last error in the handle and display a VERBOSE warning if an error was registered and not deleted
 * */
void gfal_posix_register_internal_error(gfal_handle handle, const char* prefix, GError * tmp_err){
	if(*gfal_get_last_gerror(handle) != NULL){
		gfal_print_verbose(GFAL_VERBOSE_NORMAL, "%s Warning : existing registered error replaced ! old err : %s ", prefix, (*gfal_get_last_gerror(handle))->message);
		g_clear_error(gfal_get_last_gerror(handle));
	}
	g_propagate_prefixed_error(gfal_get_last_gerror(handle), tmp_err, "%s", prefix);
}
