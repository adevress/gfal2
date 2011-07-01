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
 * @file gfal_rfio_plugin_main.c
 * @brief header file for the external rfio's plugin for gfal ( based on the old rfio part in gfal legacy )
 * @author Devresse Adrien
 * @version 0.1
 * @date 30/06/2011
 * 
 **/


#include <regex.h>
#include <time.h> 
#include "../gfal_common_catalog.h"
#include "../gfal_types.h"

typedef struct _gfal_plugin_rfio_handle{
	gfal_handle handle;
	struct rfio_proto_ops* rf;
}* gfal_plugin_rfio_handle;


gboolean gfal_rfio_check_url(catalog_handle, const char* url,  catalog_mode mode, GError** err);

gboolean gfal_rfio_internal_check_url(const char* surl, GError** err);


gfal_catalog_interface gfal_plugin_init(gfal_handle handle, GError** err);



