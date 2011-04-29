#pragma once
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
  * 
  @file gfal_common_lfc.h
  @brief header file for the lfc catalog module
  @author Adrien Devresse
  @version 0.0.1
  @date 29/04/2011
 */

#include "gfal_common_catalog.h"
#include <glib.h>
#include <sys/types.h>
#include <lfc_api.h>


gfal_catalog_interface lfc_initG(GError**);
