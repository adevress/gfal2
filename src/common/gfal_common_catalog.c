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
 * @file gfal_common_catalog.c
 * @brief the file of the common lib for the catalog management
 * @author Devresse Adrien
 * @version 0.0.1
 * @date 8/04/2011
 * */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gfal_common_catalog.h"
#include "lfc/gfal_common_lfc.h"
#include "gfal_constants.h"
#include "gfal_common_errverbose.h"
#include "gfal_common_filedescriptor.h"
#include "gfal_common_srm_open.h"


/**
 *  Note that hte default catalog is the first one
 */
static gfal_catalog_interface (*constructor[])(gfal_handle,GError**)  = { &lfc_initG}; // JUST MODIFY THIS TWO LINES IN ORDER TO ADD CATALOG
static const int size_catalog = 1;

/**
 * Instance all catalogs for use if it's not the case
 *  return the number of catalog available
 */
int gfal_catalogs_instance(gfal_handle handle, GError** err){
	g_return_val_err_if_fail(handle, -1, err, "[gfal_catalogs_instance]  invalid value of handle");
	const int catalog_number = handle->catalog_opt.catalog_number;
	if(catalog_number <= 0){
		GError* tmp_err=NULL;
		int i;
		for(i=0; i < size_catalog ;++i){
			gfal_catalog_interface catalog = constructor[i](handle, &tmp_err);
			handle->catalog_opt.catalog_list[i] = catalog;
			if(tmp_err){
				g_propagate_prefixed_error(err, tmp_err, "[%s]", __func__);
				return -1;
			}
		}
		handle->catalog_opt.catalog_number=i;
	}
	return handle->catalog_opt.catalog_number;
}
/***
 *  generic catalog operation executor
 *  Check alls catalogs, if a catalog is valid execute the given operation on this catalog and return result, 
 *  else return nagative value and set GError to the correct error
 * */
 int gfal_catalogs_operation_executor(gfal_handle handle, gboolean (*checker)(gfal_catalog_interface*, GError**),
										int (*executor)(gfal_catalog_interface*, GError**) , GError** err){
	GError* tmp_err=NULL;
	int i;
	int ret = -1;
	const int n_catalogs = gfal_catalogs_instance(handle, &tmp_err);
	if(n_catalogs > 0){
		gfal_catalog_interface* cata_list = handle->catalog_opt.catalog_list;
		for(i=0; i < n_catalogs; ++i, ++cata_list){
			gboolean comp =  checker(cata_list, &tmp_err);
			if(tmp_err)
				break;
				
			if(comp){
				ret = executor(cata_list, &tmp_err);
				break;		
			}	
		}
	}
	if(tmp_err){	
		g_propagate_prefixed_error(err, tmp_err, "[%s]", __func__);
	}else if(ret){
		g_set_error(err,0,EPROTONOSUPPORT, "[%s] Protocol not supported or path/url invalid", __func__);
	}
	return ret;
	 
 }
 
/**
 *  Execute an access function on the first catalog compatible in the catalog list
 *  return the result of the first valid catalog for a given URL
 *  @return result of the access method or -1 if error and set GError with the correct value
 *  error : EPROTONOSUPPORT means that the URL is not matched by a catalog
 *  */
int gfal_catalogs_accessG(gfal_handle handle, const char* path, int mode, GError** err){
	g_return_val_err_if_fail(handle && path, EINVAL, err, "[gfal_catalogs_accessG] Invalid arguments");
	GError* tmp_err=NULL;
	int i;
	
	gboolean access_checker(gfal_catalog_interface* cata_list, GError** terr){
		return cata_list->check_catalog_url(cata_list->handle, path,  GFAL_CATALOG_ACCESS, terr);
	}
	int access_executor(gfal_catalog_interface* cata_list, GError** terr){
		return cata_list->accessG(cata_list->handle, path, mode, terr);
	}
	
	int ret = gfal_catalogs_operation_executor(handle, &access_checker, &access_executor, &tmp_err);
	if(tmp_err)
		g_propagate_prefixed_error(err, tmp_err, "[%s]", __func__);
	return ret;
}

/**
 *  Execute a stat function on the lfc catalog
 * */
int gfal_catalog_statG(gfal_handle handle, const char* path, struct stat* st, GError** err){
	g_return_val_err_if_fail(handle && path, EINVAL, err, "[gfal_catalog_statG] Invalid arguments");
	GError* tmp_err=NULL;
	int i;
	
	gboolean stat_checker(gfal_catalog_interface* cata_list, GError** terr){
		return cata_list->check_catalog_url(cata_list->handle, path,  GFAL_CATALOG_STAT, terr);
	}
	int stat_executor(gfal_catalog_interface* cata_list, GError** terr){
		return cata_list->statG(cata_list->handle, path, st, terr);
	}
	
	int ret = gfal_catalogs_operation_executor(handle, &stat_checker, &stat_executor, &tmp_err);
	if(tmp_err)
		g_propagate_prefixed_error(err, tmp_err, "[%s]",__func__); 
	return ret;	
}


/**
 *  Execute a lstat function in the lfc
 * */
int gfal_catalog_lstatG(gfal_handle handle, const char* path, struct stat* st, GError** err){
	g_return_val_err_if_fail(handle && path, EINVAL, err, "[gfal_catalog_lstatG] Invalid arguments");
	GError* tmp_err=NULL;
	int i;
	
	gboolean lstat_checker(gfal_catalog_interface* cata_list, GError** terr){
		return cata_list->check_catalog_url(cata_list->handle, path,  GFAL_CATALOG_LSTAT, terr);
	}
	int lstat_executor(gfal_catalog_interface* cata_list, GError** terr){
		return cata_list->lstatG(cata_list->handle, path, st, terr);
	}
	
	int ret = gfal_catalogs_operation_executor(handle, &lstat_checker, &lstat_executor, &tmp_err);
	if(tmp_err)
		g_propagate_prefixed_error(err, tmp_err, "[%s]",__func__); 
	return ret;	
}

/**
 * Delete all instance of catalogs 
 */
int gfal_catalogs_delete(gfal_handle handle, GError** err){
	g_return_val_err_if_fail(handle, -1, err, "[gfal_catalogs_delete] Invalid value of handle");
	const int catalog_number = handle->catalog_opt.catalog_number;
	if(catalog_number > 0){
			int i;
			for(i=0; i< catalog_number; ++i){
				handle->catalog_opt.catalog_list[i].catalog_delete( handle->catalog_opt.catalog_list[i].handle );
			}
		
		handle->catalog_opt.catalog_number =-1;
	}
	return 0;
}
/**
 *  Execute the chmod function on the first compatible catalog ( checked with check_url func )
 *  @return 0 if success or -1 and set the GError to the correct errno value with a description msg
 * */
 int gfal_catalog_chmodG(gfal_handle handle, const char* path, mode_t mode, GError** err){
	g_return_val_err_if_fail(handle && path, -1, err, "[gfal_catalog_chmodG] Invalid arguments");	
	GError* tmp_err = NULL;		
	int i;
	
		
	gboolean chmod_checker(gfal_catalog_interface* cata_list, GError** terr){
		return cata_list->check_catalog_url(cata_list->handle, path, GFAL_CATALOG_CHMOD, terr);
	}
	int chmod_executor(gfal_catalog_interface* cata_list, GError** terr){
		return cata_list->chmodG(cata_list->handle, path, mode, terr);
	}
	
	int ret = gfal_catalogs_operation_executor(handle, &chmod_checker, &chmod_executor, &tmp_err);
	if(tmp_err)
		g_propagate_prefixed_error(err, tmp_err, "[%s]",__func__);
	return ret; 
 }
 
 /**
 *  Execute the rename function on the first compatible catalog ( checked with check_url func )
 *  @return 0 if success or -1 and set the GError to the correct errno value with a description msg
 * */
int gfal_catalog_renameG(gfal_handle handle, const char* oldpath, const char* newpath, GError** err){
	g_return_val_err_if_fail(oldpath && newpath, -1, err, "[gfal_catalog_renameG] invalid value in args oldpath, handle or newpath");
	GError* tmp_err=NULL;
	int i;
	
	gboolean rename_checker(gfal_catalog_interface* cata_list, GError** terr){
		return (cata_list->check_catalog_url(cata_list->handle, oldpath, GFAL_CATALOG_RENAME, terr) &&
					cata_list->check_catalog_url(cata_list->handle, newpath, GFAL_CATALOG_RENAME, terr));
	}
	int rename_executor(gfal_catalog_interface* cata_list, GError** terr){
		return cata_list->renameG(cata_list->handle, oldpath, newpath, terr);
	}
	
	int ret = gfal_catalogs_operation_executor(handle, &rename_checker, &rename_executor, &tmp_err);
	if(tmp_err)
		g_propagate_prefixed_error(err, tmp_err, "[%s]",__func__);
	return ret; 
	
}
/**
 * Execute a mkdir function on the first compatible catalog ( checked with check url func )
 *  @param handle handle of the current context
 *  @param path path to create
 *  @param mode right of the file created
 *  @param pflag if TRUE, execute the request recursively if necessary else work as the common mkdir system call
 *  @param GError error report system
 *  @warning no check on the path, please check the path before
 *  @return return 0 if success else return -1
 * 
 * */
int gfal_catalog_mkdirp(gfal_handle handle, const char* path, mode_t mode, gboolean pflag,  GError** err){
	g_return_val_err_if_fail(handle && path , -1, err, "[gfal_catalog_mkdirp] Invalid argumetns in path or/and handle");
	GError* tmp_err=NULL;	

	gboolean mkdirp_checker(gfal_catalog_interface* cata_list, GError** terr){
		return cata_list->check_catalog_url(cata_list->handle, path, GFAL_CATALOG_MKDIR, terr);
	}
	int mkdirp_executor(gfal_catalog_interface* cata_list, GError** terr){
		return cata_list->mkdirpG(cata_list->handle, path, mode, pflag, terr);
	}

	int ret = gfal_catalogs_operation_executor(handle, &mkdirp_checker, &mkdirp_executor, &tmp_err);
	if(tmp_err)
		g_propagate_prefixed_error(err, tmp_err, "[%s]",__func__);
	return ret; 	
}

/**
 * Execute a rmdir function on the first compatible catalog ( checked with check url func )
 *  @param handle handle of the current context
 *  @param path path to delete
 *  @param GError error report system
 *  @warning no check on the path, please check the path before
 *  @return return 0 if success else return -1
 * 
 * */
int gfal_catalog_rmdirG(gfal_handle handle, const char* path, GError** err){
	g_return_val_err_if_fail(handle && path , -1, err, "[gfal_catalog_rmdirp] Invalid argumetns in path or/and handle");
	GError* tmp_err=NULL;	

	gboolean rmdir_checker(gfal_catalog_interface* cata_list, GError** terr){
		return cata_list->check_catalog_url(cata_list->handle, path, GFAL_CATALOG_RMDIR, terr);
	}
	int rmdir_executor(gfal_catalog_interface* cata_list, GError** terr){
		return cata_list->rmdirG(cata_list->handle, path, terr);
	}

	int ret = gfal_catalogs_operation_executor(handle, &rmdir_checker, &rmdir_executor, &tmp_err);
	if(tmp_err)
		g_propagate_prefixed_error(err, tmp_err, "[%s]",__func__);
	return ret; 	
}

/**
 * Execute a opendir function on the first compatible catalog ( checked with check url func )
 * @param handle handle of the current context
 * @param path path to open
 * @param GError error report system
 * @return gfal_file_handle pointer given to the handle or NULL if error 
 */
 gfal_file_handle gfal_catalog_opendirG(gfal_handle handle, const char* name, GError** err){
	g_return_val_err_if_fail(handle && name, NULL, err,  "[gfal_catalog_opendir] invalid value");
	GError* tmp_err=NULL;	
	DIR* d=NULL;
	gfal_file_handle resu=NULL;
	gfal_catalog_interface* pcata = NULL;
	
	gboolean opendir_checker(gfal_catalog_interface* cata_list, GError** terr){
		return cata_list->check_catalog_url(cata_list->handle, name, GFAL_CATALOG_OPENDIR, terr);
	}
	int opendir_executor(gfal_catalog_interface* cata_list, GError** terr){
		d= cata_list->opendirG(cata_list->handle, name, terr);
		pcata= cata_list;
		return (d)?0:-1;
	}
	

	int ret = gfal_catalogs_operation_executor(handle, &opendir_checker, &opendir_executor, &tmp_err);
	if(!ret){
		resu = gfal_file_handle_ext_new(GFAL_EXTERNAL_MODULE_OFFSET, (gpointer) d, pcata);
	}
	if(tmp_err)
		g_propagate_prefixed_error(err, tmp_err, "[%s]",__func__);
	return resu;  
}

/**
 * 
 * close the given dir handle in the proper catalog
 */ 
int gfal_catalog_closedirG(gfal_handle handle, gfal_file_handle fh, GError** err){
	g_return_val_err_if_fail(handle && fh, -1,err, "[gfal_catalog_closedirG] Invalid args ");	
	GError* tmp_err=NULL;
	int ret = -1;
	gfal_catalog_interface* if_cata = fh->ext_data;
	ret = if_cata->closedirG(if_cata->handle, (DIR*) fh->fdesc, &tmp_err);
	if(tmp_err)
		g_propagate_prefixed_error(err, tmp_err, "[%s]",__func__);
	return ret;  	
}

/**
 * 
 *  open the file specified by path on the proper catalog with the specified flag and mode
 * */
gfal_file_handle gfal_catalog_openG(gfal_handle handle, const char * path, int flag, mode_t mode, GError ** err){
	GError* tmp_err=NULL;
	g_set_error(&tmp_err,0, ENOSYS, "not implemented");
	if(tmp_err)
		g_propagate_prefixed_error(err, tmp_err, "[%s]",__func__);
	return NULL;
}

/**
 *  close the given file handle in the proper catalog
 * */
int gfal_catalog_closeG(gfal_handle handle, gfal_file_handle fh, GError** err){
	g_return_val_err_if_fail(handle && fh, -1,err, "[gfal_catalog_closeG] Invalid args ");	
	GError* tmp_err=NULL;
	int ret = -1;
	gfal_catalog_interface* if_cata = fh->ext_data;
	ret = if_cata->closeG(if_cata->handle, GPOINTER_TO_INT(fh->fdesc), &tmp_err);
	if(tmp_err)
		g_propagate_prefixed_error(err, tmp_err, "[%s]",__func__);
	return ret;  	
}

/**
 *  execute a readdir for the given file handle on the appropriate catalog
 * 
 * */
struct dirent* gfal_catalog_readdirG(gfal_handle handle, gfal_file_handle fh, GError** err){
	g_return_val_err_if_fail(handle && fh, NULL,err, "[gfal_catalog_readdirG] Invalid args ");	
	GError* tmp_err=NULL;
	struct dirent* ret = NULL;
	gfal_catalog_interface* if_cata = fh->ext_data;
	ret = if_cata->readdirG(if_cata->handle, (DIR*) fh->fdesc, &tmp_err);
	if(tmp_err)
		g_propagate_prefixed_error(err, tmp_err, "[%s]",__func__);
	return ret; 
}

/**
 * Resolve a link on the catalog to a list of surl
 * @return pointer to a table of string with all the surls, table end with NULL, or return NULL if error 
 * @warning must be free with g_list_free_full
 */
char** gfal_catalog_getSURL(gfal_handle handle, const char* path, GError** err){
	GError* tmp_err=NULL;
	char** resu = NULL;
	
	gboolean getSURL_checker(gfal_catalog_interface* cata_list, GError** terr){
		return cata_list->check_catalog_url(cata_list->handle, path, GFAL_CATALOG_GETSURL, terr);
	}	
	int getSURL_executor(gfal_catalog_interface* cata_list, GError** terr){
		resu= cata_list->getSURLG(cata_list->handle, path, terr);
		return (resu)?0:-1;
	}
	
	gfal_catalogs_operation_executor(handle, &getSURL_checker, &getSURL_executor, &tmp_err);
	if(tmp_err)
		g_propagate_prefixed_error(err, tmp_err, "[%s]",__func__);	
	return resu;
}

/***
 *  Try to resolve the guid to a compatible catalog URL in all the catalogs.
 *  @return string of the new catalog URL or NULL value if error and set GError to the correct errno and msg
 *  @warning not safe, no checking on the url integrity
 * */
char* gfal_catalog_resolve_guid(gfal_handle handle, const char* guid, GError** err){
	g_return_val_err_if_fail(handle && guid, NULL,err, "[gfal_catalog_resolve_guid] Invalid args ");
	GError *tmp_err=NULL;
	char* ret = NULL;
	int i;
	
	const int n_catalogs = gfal_catalogs_instance(handle, &tmp_err);
	if(n_catalogs > 0 && !tmp_err){
		gfal_catalog_interface* cata_list = handle->catalog_opt.catalog_list;
		for(i=0; i< n_catalogs; ++i, ++cata_list){
				ret = cata_list->resolve_guid(cata_list->handle, guid, &tmp_err);
				if(ret || tmp_err)
					break;		
			}
	}
		
	if(tmp_err){ 			// error reported
		g_propagate_prefixed_error(err, tmp_err, "[%s]",__func__);	
		ret = NULL;
	}else if(ret==NULL){ 		// no error and no valid url 
		g_set_error(err, 0, EPROTONOSUPPORT, "[%s] Error : Protocol not supported or invalidpath/url ",__func__);
		ret =NULL;
	}
	return ret;		
}


/**
 *  Complete openG func with catalog->surl resolution
 *  This func try to resolve the path to a valid surl and open surl with the srm module
 *  else open is call on the first compatible catalog like in the normal way.
 *  @return pointer to file handle if success else NULL if error
 * 
 */ 
gfal_file_handle gfal_catalog_open_globalG(gfal_handle handle, const char * path, int flag, mode_t mode, GError** err){
	char** res_surl=NULL;
	GError* tmp_err=NULL;
	gfal_file_handle ret = NULL;
	if( (res_surl = gfal_catalog_getSURL(handle, path, &tmp_err)) != NULL){ // try a surl resolution on the catalogs
		if(gfal_surl_checker(*res_surl, NULL) == 0 )
			ret = gfal_srm_openG(handle, *res_surl, flag, mode, &tmp_err);
		else
			g_set_error(&tmp_err, 0, ECOMM, "bad surl value retrived from catalog : %s ", *res_surl);
		g_strfreev(res_surl);
	}else if(!tmp_err){ // try std open on the catalogs		
		ret = gfal_catalog_openG(handle, path, flag, mode, &tmp_err);
	}

	if(tmp_err)
		g_propagate_prefixed_error(err, tmp_err, "[%s]", __func__);
	return ret;
}

/**
 * return the catalog type configured at compilation time
 */
static char* get_default_cat(){
	return GFAL_DEFAULT_CATALOG_TYPE;
}

/***
 *  return the name of the current selected default catalog in a string form
 * */
extern char* gfal_get_cat_type(GError** err) {
    char *cat_env;
    char *cat_type;

    if((cat_env = getenv ("LCG_CATALOG_TYPE")) == NULL) {
		gfal_print_verbose(GFAL_VERBOSE_VERBOSE, "[get_cat_type] LCG_CATALOG_TYPE env var is not defined, use default var instead");
        cat_env = get_default_cat(); 
	}
    if((cat_type = strndup(cat_env, 50)) == NULL) {
		g_set_error(err,0,EINVAL,"[%s] invalid env var LCG_CATALOG_TYPE, please set it correctly or delete it",__func__);
        return NULL;
    }
    return cat_type;
}

