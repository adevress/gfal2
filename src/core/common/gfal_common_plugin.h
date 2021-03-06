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

/* gfal_common_plugin.h
 * common lib for the plugin management
 * author : Devresse Adrien
 */

#pragma once
#ifndef GFAL_COMMON_PLUGIN_H
#define GFAL_COMMON_PLUGIN_H

#if !defined(__GFAL2_H_INSIDE__) && !defined(__GFAL2_BUILD__)
#   warning "Direct inclusion of gfal2 headers is deprecated. Please, include only gfal_api.h or gfal_plugins_api.h"
#endif

#include <stdarg.h>
#include <glib.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#include <common/gfal_prototypes.h>
#include <common/gfal_types.h>
#include <common/gfal_common_plugin_interface.h>
#include <transfer/gfal_transfer_types.h>


#define GFAL2_PLUGIN_SCOPE "GFAL2::PLUGIN"


#ifdef __cplusplus
extern "C"
{
#endif  // __cplusplus

typedef struct _plugin_pointer_handle{
    gfal_plugin_interface* plugin_api;                  // plugin official API
    void* dlhandle;                                     // dlhandle of the plugin
    void* plugin_data;                                  // plugin internal data
    char plugin_name[GFAL_URL_MAX_LEN];                 // plugin name
    char plugin_lib[GFAL_URL_MAX_LEN];                  // plugin library path
} *plugin_pointer_handle;

/*
 *
 *  This API is a plugin reserved API and should be used by GFAL 2.0's plugins only
 *  Backward compatibility of this API is not guarantee
 *
 * */


/*
 * plugins walker
 * provide a list of plugin handlers, each plugin handler is a reference to an usable plugin
 * give a NULL-ended table of plugin_pointer_handle, return NULL if error
 * */
plugin_pointer_handle gfal_plugins_list_handler(gfal2_context_t, GError** err);

plugin_handle gfal_get_plugin_handle(gfal_plugin_interface* p_interface);

int gfal_plugins_instance(gfal2_context_t, GError** err);

char** gfal_plugins_get_list(gfal2_context_t, GError** err);

int gfal_plugins_delete(gfal2_context_t, GError** err);

gboolean gfal_feature_is_supported(void * ptr, GQuark scope, const char* func_name, GError** err);

// find a compatible catalog or return NULL + error
gfal_plugin_interface* gfal_find_plugin(gfal2_context_t handle,
                                         const char * url,
                                         plugin_mode acc_mode, GError** err);



gfal_plugin_interface* gfal_plugin_map_file_handle(gfal2_context_t handle, gfal_file_handle fh, GError** err);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* GFAL_COMMON_PLUGIN_H */

