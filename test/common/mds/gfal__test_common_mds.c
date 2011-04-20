

/* unit test for common_mds*/


#include <check.h>
#include "gfal_common.h"



START_TEST(test_check_bdii_endpoints_srm)
{
	char **se_types=NULL;
	char **se_endpoints=NULL;
	GError * err=NULL;
	int ret=-1;
	char* endpoints[] = { "grid-cert-03.roma1.infn.it",
						NULL };
	char** ptr = endpoints;
	while(*ptr != NULL){
		 ret = gfal_mds_get_se_types_and_endpoints (*ptr, &se_types, &se_endpoints, &err);
		if(ret != 0){
				fail(" ret of bdii with error");
				perror(" bdii err  ");
				return;
		}
		fail_unless(ret == 0 && se_types != NULL && se_endpoints != NULL);
		//fprintf(stderr, " se_types : %s , se_endpoints", se_types[1], se_endpoints[1]);
		ptr++;
	}
	
	ret = gfal_mds_get_se_types_and_endpoints ("google.com", &se_types, &se_endpoints, &err);		
	fail_unless(ret != 0 &&  err->code == EINVAL , " must fail, invalid url");
	g_clear_error(&err);
	


}
END_TEST


