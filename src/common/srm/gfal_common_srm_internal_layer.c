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
 * @file gfal_common_srm_internal_layer.c
 * @brief file for the srm external function mapping for mocking purpose
 * @author Devresse Adrien
 * @version 2.0
 * @date 09/06/2011
 * */


#include "gfal_common_srm_internal_layer.h"



struct _gfal_srm_external_call gfal_srm_external_call = { 
	.srm_context_init = &srm_context_init,
	.srm_ls = &srm_ls,
	.srm_rmdir = &srm_rmdir,
	.srm_mkdir = &srm_mkdir,
	.srm_getpermission = &srm_getpermission,
	.srm_check_permission = &srm_check_permission,
	.srm_srmv2_pinfilestatus_delete = &srm_srmv2_pinfilestatus_delete,
	.srm_srmv2_mdfilestatus_delete = &srm_srmv2_mdfilestatus_delete,
	.srm_srmv2_filestatus_delete = &srm_srmv2_filestatus_delete,
	.srm_srm2__TReturnStatus_delete = &srm_srm2__TReturnStatus_delete,
	.srm_prepare_to_get= &srm_prepare_to_get
};