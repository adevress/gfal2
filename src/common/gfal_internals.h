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

/*
 * @(#)$RCSfile: gfal_internals.h,v $ $Revision: 1.6 $ $Date: 2009/10/08 15:32:39 $ CERN Remi Mollon
 */

#ifndef _GFAL_INTERNALS_H
#define _GFAL_INTERNALS_H

/* enforce proper calling convention */
#ifdef __cplusplus
extern "C"
{
#endif


#include "gfal_constants.h"
#include "gfal_common.h"

// externals imports


// remain a transition file : fix it







/******************** gfal.c ********************/

const char *gfal_version ();
//void gfal_errmsg (char *, int, const char *, int);
void gfal_errmsg (char *, int, int, const char *, ...);
char *gfal_get_userdn (char *errbuf, int errbufsz);
char *gfal_get_vo (char *errbuf, int errbufsz);
int gfal_get_fqan (char ***fqan, char *errbuf, int errbufsz);
//int gfal_is_nobdii ();
int gfal_is_purifydisabled ();
int gfal_register_file (const char *, const char *, const char *, mode_t, GFAL_LONG64, int, char *, int);
void gfal_internal_free (gfal_internal);
void gfal_spacemd_free (int,gfal_spacemd *);
char *get_catalog_endpoint(char *, int);
int guid_exists (const char *, char *, int);
int gfal_guidsforpfns (int, const char **, int, char ***, int **, char *, int);
char *gfal_guidforpfn (const char *, char *, int);
char *guidfromlfn (const char *, char *, int);
char **gfal_get_aliases (const char *, const char *, char *, int);
int register_alias (const char *, const char *, char *, int);
int unregister_alias (const char *, const char *, char *, int);
int gfal_unregister_pfns (int, const char **, const char **, int **, char *, int);
char **gfal_get_replicas (const char *, const char *, char *, int);
char *gfal_get_hostname (const char *, char *, int);

/* legacy method for EDG Catalog where size is set on pfn, not guid */
int setfilesize (const char *, GFAL_LONG64, char *, int);

char *get_default_se(char *, int);
int purify_surl (const char *, char *, const int);
int setypesandendpointsfromsurl (const char *, char ***, char ***, char *, int);
int setypesandendpoints (const char *, char ***, char ***, char *, int);
int canonical_url (const char *, const char *, char *, int, char *, int);
int parseturl (const char *, char *, int, char *, int, char*, int);
int replica_exists(const char*, char*, int);
int getdomainnm (char *name, int namelen);
char **get_sup_proto ();
struct proto_ops *find_pops (const char *);
int mapposixerror (struct proto_ops *, int);


/******************** gfal_file.c ********************/




/******************** mds_ifce.c ********************/

int get_bdii (char *, int, int *, char *, int);
int get_ce_ap (const char *, char **, char *, int);
int get_lfc_endpoint (char **, char *, int);
int get_rls_endpoints (char **, char **, char *, int);
int get_storage_path (const char *, const char *, char **, char **, char *, int);
int get_seap_info (const char *, char ***, int **, char *, int);
int get_se_types_and_endpoints (const char *, char ***, char ***, char *, int);


/******************** lrc_ifce.c ********************/
/* Lrc Support removed : fix it
int lrc_deletepfn (const char *, const char *, char *, int);
int lrc_deletesurl (const char *, char *, int);
char *lrc_get_catalog_endpoint(char *, int);
char *lrc_guidforpfn (const char *, char *, int);
int lrc_guid_exists (const char *, char *, int);
int lrc_register_pfn (const char *, const char *, char *, int);
int lrc_replica_exists(const char* ,char*, int);
int lrc_setfilesize (const char *, GFAL_LONG64, char *, int);
int lrc_unregister_pfn (const char *, const char *, char *, int);
char **lrc_surlsfromguid (const char *, char *, int);
int lrc_fillsurls (gfal_file);
*/

/******************** rmc_ifce.c ********************/
/* mrc removed : fix it
char *rmc_guidfromlfn (const char *, char *, int);
char **rmc_lfnsforguid (const char *, char *, int);
int rmc_register_alias (const char *, const char *, char *, int);
int rmc_unregister_alias (const char *, const char *, char *, int);

*/
/******************** gridftp_ifce.c ********************/
/*
int gridftp_delete (char *, char *, int, int);
int gridftp_ls (char *, int *, char ***, struct stat64 **, char *, int, int);

*/
/******************** sfn_ifce.c ********************/

/* fix it : removed int sfn_deletesurls (int, const char **, struct sfn_filestatus **, char *, int, int);*/
int sfn_getfilemd (int, const char **, struct srmv2_mdfilestatus **, char *, int, int);
int sfn_turlsfromsurls (int, const char **, char **, struct sfn_filestatus **, char *, int);





int mdtomd32 (struct stat64 *, struct stat *);
int copy_gfal_results (gfal_internal, enum status_type);
int check_gfal_internal (gfal_internal, int, char *, int);
void free_gfal_results (gfal_filestatus *, int);
void free_srmv2_mdstatuses (struct srmv2_mdfilestatus *, int);


#ifdef __cplusplus
}
#endif

#endif