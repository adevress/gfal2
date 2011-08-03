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
 * @brief header file for bindings for rfio funcs
 * @author Devresse Adrien
 * @version 0.1
 * @date 30/06/2011
 * 
 **/


#include <regex.h>
#include <time.h> 
#include <glib.h>
#include "../gfal_common_internal.h"
#include "../gfal_common_errverbose.h"
#include "../gfal_common_catalog.h"
#include "../gfal_common_filedescriptor.h"
#include "../gfal_types.h"
#include "gfal_rfio_plugin_bindings.h"
#include "gfal_rfio_plugin_main.h"
#include "gfal_rfio_plugin_layer.h"

static void rfio_report_error(gfal_plugin_rfio_handle h,  const char * func_name, GError** err){
	char buff_error[2048];
	int status = h->rf->geterror();
	status = (status > 1000)?ECOMM:status;
	strerror_r(status, buff_error, 2048);
	g_set_error(err, 0, status, "[%s] Error reported by the external library rfio : %s", func_name, buff_error);
}

gfal_file_handle gfal_rfio_openG(catalog_handle handle , const char* path, int flag, mode_t mode, GError** err){
	gfal_plugin_rfio_handle h = (gfal_plugin_rfio_handle) handle;
	gfal_file_handle ret = NULL;
	int fd= h->rf->open(path, flag, mode);
	if(fd <= 0)
		rfio_report_error(h, __func__, err);
	else
		ret = gfal_file_handle_new(gfal_rfio_getName(), GINT_TO_POINTER(fd));
	return ret;
}

int gfal_rfio_readG(catalog_handle handle , gfal_file_handle fd, void* buff, size_t s_buff, GError** err){
	gfal_plugin_rfio_handle h = (gfal_plugin_rfio_handle) handle;
	int ret = h->rf->read(GPOINTER_TO_INT(fd->fdesc), buff, s_buff);
	if(ret <0)
		rfio_report_error(h, __func__, err);
	else
		errno =0;
	return ret;
}

int gfal_rfio_lseekG(catalog_handle handle , gfal_file_handle fd, off_t offset, int whence, GError** err){
	gfal_plugin_rfio_handle h = (gfal_plugin_rfio_handle) handle;
	off_t ret = h->rf->lseek(GPOINTER_TO_INT(fd->fdesc), offset, (int) whence);
	if(ret == ((off_t)0)-1)
		rfio_report_error(h, __func__, err);
	else
		errno =0;
	return (int)ret;
}

int gfal_rfio_writeG(catalog_handle handle , gfal_file_handle fd, void* buff, size_t s_buff, GError** err){
	gfal_plugin_rfio_handle h = (gfal_plugin_rfio_handle) handle;
	int ret = h->rf->write(GPOINTER_TO_INT(fd->fdesc), buff, s_buff);
	if(ret <0)
		rfio_report_error(h, __func__, err);
	else
		errno =0;
	return ret;
}




int gfal_rfio_closeG(catalog_handle handle, gfal_file_handle fd, GError ** err){
	gfal_plugin_rfio_handle h = (gfal_plugin_rfio_handle) handle;
	int ret= h->rf->close(GPOINTER_TO_INT(fd->fdesc));
	if(ret != 0){
		rfio_report_error(h, __func__, err);
	}else
		free(fd);
	return ret;	
}

int gfal_rfio_statG(catalog_handle handle, const char* name, struct stat* buff, GError ** err){
	gfal_plugin_rfio_handle h = (gfal_plugin_rfio_handle) handle;
	int ret= h->rf->stat(name, buff);
	if(ret != 0){
		rfio_report_error(h, __func__, err);
	}
	return ret;	
}

int gfal_rfio_lstatG(catalog_handle handle, const char* name, struct stat* buff, GError ** err){
	gfal_plugin_rfio_handle h = (gfal_plugin_rfio_handle) handle;
	int ret= h->rf->lstat(name, buff);
	if(ret != 0){
		rfio_report_error(h, __func__, err);
	}
	return ret;	
}

gfal_file_handle gfal_rfio_opendirG(catalog_handle handle, const char* name, GError ** err){
	gfal_plugin_rfio_handle h = (gfal_plugin_rfio_handle) handle;
	DIR * ret = h->rf->opendir(name);
	if(ret == NULL){
		rfio_report_error(h, __func__, err);
		return NULL;
	}
	return gfal_file_handle_new(gfal_rfio_getName(), (gpointer) ret);
}



const char* gfal_rfio_getName(){
	return "rfio";
}

