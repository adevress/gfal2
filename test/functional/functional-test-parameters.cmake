#
# compilation lines for test parameters

## STAT Tests

SET(MY_VO "dteam")
SET(MY_VO_STORM "${MY_VO}")
SET(TEST_ENVIRONMENT "PRODUCTION"
        CACHE STRING "Define the target for functional test")

## Global environment

IF(TEST_ENVIRONMENT STREQUAL "TESTBED_RC")

## Testbed environment
SET(file_base_path "/tmp/")
SET(srm_prefix_storm "srm://storm.ifca.es:8444/srm/managerv2?SFN=/${MY_VO_STORM}/gfal2-tests/")
SET(dcap_prefix "gsidcap://dcache-door-desy09.desy.de:22128/pnfs/desy.de/${MY_VO}/gfal2-tests/")
SET(srm_prefix_dcache "srm://grid002.ft.uam.es:8443/srm/managerv2?SFN=/pnfs/ft.uam.es/data/${MY_VO}/gfal2-tests/")
SET(lfc_prefix "lfn:/grid/${MY_VO}")
SET(lfc_host_name "prod-lfc-shared-central.cern.ch")
SET(gsiftp_prefix_dpm "gsiftp://lxfsra04a04.cern.ch/dpm/cern.ch/home/${MY_VO}/gfal2-tests")
SET(srm_prefix_dpm "srm://lxfsra04a04.cern.ch:8446/dpm/cern.ch/home/${MY_VO}/gfal2-tests")
SET(davs_prefix_dpm "davs+3rd://lxfsra04a04.cern.ch/dpm/cern.ch/home/${MY_VO}/gfal2-tests")
SET(root_prefix_dpm "root://lxfsra04a04.cern.ch/dpm/cern.ch/home/${MY_VO}/gfal2-tests")

ELSEIF(TEST_ENVIRONMENT STREQUAL "TESTBED_TRUNK")

## Testbed trunk environment
SET(file_base_path "/tmp/")
SET(srm_prefix_storm "srm://storm.ifca.es:8444/srm/managerv2?SFN=/${MY_VO_STORM}/gfal2-tests/")
SET(dcap_prefix "gsidcap://vm-dcache-deploy6.desy.de/data/${MY_VO}/gfal2-tests/")
SET(srm_prefix_dcache "srm://vm-dcache-deploy6.desy.de:8443/data/${MY_VO}/gfal2-tests")
SET(lfc_prefix "lfn:/grid/${MY_VO}")
SET(lfc_host_name "prod-lfc-shared-central.cern.ch")
SET(gsiftp_prefix_dpm "gsiftp://lxfsra04a04.cern.ch/dpm/cern.ch/home/${MY_VO}/gfal2-tests")
SET(srm_prefix_dpm "srm://lxfsra04a04.cern.ch:8446/dpm/cern.ch/home/${MY_VO}/gfal2-tests")
SET(davs_prefix_dpm "davs+3rd://lxfsra04a04.cern.ch/dpm/cern.ch/home/${MY_VO}/gfal2-tests")
SET(root_prefix_dpm "root://lxfsra04a04.cern.ch/dpm/cern.ch/home/${MY_VO}/gfal2-tests")

ELSE(TEST_ENVIRONMENT STREQUAL "TESTBED_RC")

## Production environment : default
SET(file_base_path "/tmp/")
SET(srm_prefix_storm "srm://storm.ifca.es:8444/srm/managerv2?SFN=/${MY_VO_STORM}/gfal2-tests/")
SET(dcap_prefix "gsidcap://dcache-door-desy09.desy.de:22128/pnfs/desy.de/${MY_VO}/gfal2-tests/")
SET(srm_prefix_dcache "srm://grid002.ft.uam.es:8443/srm/managerv2?SFN=/pnfs/ft.uam.es/data/${MY_VO}/gfal2-tests/")
SET(lfc_prefix "lfn:/grid/${MY_VO}")
SET(lfc_host_name "prod-lfc-shared-central.cern.ch")
SET(gsiftp_prefix_dpm "gsiftp://hepgrid11.ph.liv.ac.uk/dpm/ph.liv.ac.uk/home/${MY_VO}/gfal2-tests/")
SET(srm_prefix_dpm "srm://hepgrid11.ph.liv.ac.uk:8446/srm/managerv2?SFN=/dpm/ph.liv.ac.uk/home/${MY_VO}/gfal2-tests/")

# Need to find something better!
SET(davs_prefix_dpm "davs+3rd://lxfsra04a04.cern.ch/dpm/cern.ch/home/${MY_VO}/gfal2-tests")
SET(root_prefix_dpm "root://hepgrid11.ph.liv.ac.uk/dpm/ph.liv.ac.uk/home/${MY_VO}/gfal2-tests")

ENDIF(TEST_ENVIRONMENT STREQUAL "TESTBED_RC")

#dpm parameters
SET(srm_valid_dpm_stat "${srm_prefix_dpm}/testread0011")
SET(srm_valid_dpm_bigfile "${srm_prefix_dpm}/testbig")
SET(srm_valid_dir_root "${srm_prefix_dpm}")
SET(srm_valid_dpm_src_file "${srm_valid_dpm_stat}")

# dcache parameters
SET(srm_valid_dcache_bigfile "${srm_prefix_dcache}/testbig")
SET(srm_valid_dcache_stat "${srm_prefix_dcache}/testread0011")
SET(srm_valid_dcache_dir_root "${srm_prefix_dcache}")
SET(srm_valid_dcache_src_file "${srm_valid_dcache_stat}")

# storm parameters

SET(srm_valid_storm_stat "${srm_prefix_storm}/testread00111")
SET(srm_valid_storm_dir_root "${srm_prefix_storm}")
SET(srm_valid_storm_src_file "${srm_prefix_storm}")

## gsiftp parameters
#
SET(gsiftp_valid_dpm_stat "${gsiftp_prefix_dpm}/testread0011")
SET(gsiftp_valid_dpm_src_file "${gsiftp_valid_dpm_stat}")
SET(gsiftp_valid_dir_root "${gsiftp_prefix_dpm}")

## https parameters
#
SET(davs_valid_dpm_stat     "${davs_prefix_dpm}/testread0011")
SET(davs_valid_dpm_src_file "${davs_valid_dpm_stat}")
SET(davs_valid_dir_root     "${davs_prefix_dpm}")

## xrootd parameters
SET(root_valid_dpm_stat     "${root_prefix_dpm}/testread0011")
SET(root_valid_dpm_src_file "${root_valid_dpm_stat}")
SET(root_valid_dir_root     "${root_prefix_dpm}")

## lfc parameters
SET(lfc_stat_ok "${lfc_prefix}/testread0011")
SET(lfc_chmod_ok "${lfc_prefix}/test_change_right")
SET(lfc_valid_dir_root "${lfc_prefix}")
SET(guid_stat_ok "guid:f2f49f94-a4fc-4bad-95d7-cc9479b0123b")
SET(lfc_full_prefix "lfc://${lfc_host_name}/grid/${MY_VO}")
SET(lfc_full_stat_ok "${lfc_full_prefix}/testread0011")
SET(lfc_full_chmod_ok "${lfc_full_prefix}/test_change_right")
SET(lfc_full_valid_dir_root "${lfc_full_prefix}")

# dcap dcache parameters
SET(dcap_stat_ok "${dcap_prefix}/testread0011")
SET(dcap_valid_dir_root "${dcap_prefix}")

#local file parameters
SET(file_prefix "file://${file_base_path}")
FILE(WRITE "${file_base_path}/testread_0011" "hello world agdlkmgfmklmklklmvc;!:c;:!;:!xc;!:vx!;:bvx!;:!;:o=)=)à=àdg:;;:!:!;!:;b")
SET(file_stat_ok "${file_prefix}/testread_0011")
SET(file_valid_chmod "${file_prefix}/test_change_right")

IF(PLUGIN_FILE)
        test_gfal_test_del_nonex("FILE" "file://${file_base_path}")
        test_gfal_test_del("FILE" "file://${file_base_path}")
        test_gfal_test_mkdir_unlink("FILE" "file://${file_base_path}")
        stat_test_all("FILE" ${file_prefix})
        chmod_test_all("FILE" file://${file_base_path} 0565 060 360 767)
        mkdir_test_all("FILE" ${file_prefix})
        rmdir_test_all("FILE" ${file_prefix})
        test_readdir_full("FILE" ${file_prefix})
        rwt_test_all("FILE" ${file_prefix} 4578)
        rwt_test_all("FILE" ${file_prefix} 1)
        rwt_test_all("FILE" ${file_prefix} 100000)
        rwt_test_seq("FILE" ${file_prefix} 100 4560)
        checksum_test_simple("FILE_MD5" ${file_prefix} MD5)
        checksum_test_simple("FILE_ADLER32" ${file_prefix} ADLER32)
        checksum_test_simple("FILE_CRC32" ${file_prefix} CRC32)
ENDIF(PLUGIN_FILE)

IF(PLUGIN_SRM)
        # del dir test
        test_gfal_test_del_nonex("SRM_DPM"    "${srm_prefix_dpm}")
        test_gfal_test_del_nonex("SRM_STORM"  "${srm_prefix_storm}")
        test_gfal_test_del_nonex("SRM_DCACHE" "${srm_prefix_dcache}")
        test_gfal_test_del("SRM_DPM"     "${srm_prefix_dpm}")
        test_gfal_test_del("SRM_STORM"   "${srm_prefix_storm}")
        test_gfal_test_del("SRM_DCACHE"  "${srm_prefix_dcache}")
        # mkdir tests
        test_gfal_test_mkdir_unlink("SRM_DPM"     "${srm_prefix_dpm}")
        test_gfal_test_mkdir_unlink("SRM_STORM"   "${srm_prefix_storm}")
        test_gfal_test_mkdir_unlink("SRM_DCACHE"  "${srm_prefix_dcache}")
        # stat tests
        stat_test_all("SRM_DPM" ${srm_prefix_dpm})
        stat_test_all("SRM_DCACHE" ${srm_prefix_dcache})
        # checksum tests
        checksum_test_simple("SRM_DPM_ADLER32" ${srm_prefix_dpm} ADLER32)
        checksum_test_simple("SRM_DPM_MD5" ${srm_prefix_dpm} MD5)
        checksum_test_simple("SRM_DCACHE_ADLER32" ${srm_prefix_dcache} ADLER32)
       # checksum_test_simple("SRM_DCACHE_MD5" ${srm_valid_dcache_stat} MD5) Dcache does not support dynamic checksum calculation
        mkdir_test_all("SRM_DPM" ${srm_prefix_dpm})
        mkdir_test_all("SRM_DCACHE" ${srm_prefix_dcache})
        chmod_test_all("SRM_DPM" ${srm_valid_dir_root} 0575 070 370 777)
        rmdir_test_all("SRM_DPM" ${srm_valid_dir_root})
        test_readdir_full("SRM_DPM" ${srm_valid_dir_root} )     
        rwt_test_all("SRM_DPM" ${srm_valid_dir_root} 4578)
        rwt_test_all("SRM_DPM_little" ${srm_valid_dir_root} 10)
        rwt_test_all("SRM_DPM_single" ${srm_valid_dir_root} 1)  
        rwt_test_all("SRM_DCAP" ${srm_valid_dcache_dir_root} 4578)
        rwt_test_all("SRM_DCAP_little" ${srm_valid_dcache_dir_root} 10) 
        rwt_test_all("SRM_DCAP_single" ${srm_valid_dcache_dir_root} 1)          
        rwt_test_seq("SRM_DPM" ${srm_valid_dir_root} 100 4560)  
        rwt_test_seq("SRM_DPM_unit" ${srm_valid_dir_root} 1 10)                 
        rwt_test_seq("SRM_DCAP" ${srm_valid_dir_root} 100 4560) 

        rwt_test_seq("SRM_STORM" ${srm_prefix_storm} 100 4560)
        
        # Bringonline
        test_bringonline("SRM_DPM" ${srm_prefix_dpm})
        test_bringonline("SRM_DCACHE" ${srm_prefix_dcache})
        # Note: STORM may not support tapes
                
#       chmod_test_all("SRM_DCACHE" ${srm_valid_dcache_chmod} 0565 060 360 767)  -> disabled, since unavailable on dcache
#       stat_test_all( "SRM_EOS" ${srm_valid_EOS_stat})
ENDIF(PLUGIN_SRM)

IF(PLUGIN_LFC)
        stat_test_all( "LFC" ${lfc_prefix})
        mkdir_test_all("LFC" ${lfc_prefix})     
        chmod_test_all("LFC" ${lfc_valid_dir_root} 0565 060 0360 0767)
        rmdir_test_all("LFC" ${lfc_valid_dir_root})
        test_readdir_full("LFC" ${lfc_valid_dir_root} )
        #guid test
        stat_test_all("GUID" ${guid_stat_ok})
        # lfc full url style test
        stat_test_all( "LFC_FULL" ${lfc_full_prefix})
        mkdir_test_all("LFC_FULL" ${lfc_full_prefix})
        chmod_test_all("LFC_FULL" ${lfc_valid_dir_root} 0565 060 0360 0767)
        rmdir_test_all("LFC_FULL" ${lfc_full_valid_dir_root})
        test_readdir_full("LFC_FULL" ${lfc_full_valid_dir_root} )
ENDIF(PLUGIN_LFC)


IF(PLUGIN_DCAP)
        stat_test_all( "DCAP" ${dcap_prefix})

        mkdir_test_all("DCAP" ${dcap_prefix})
        # disable not supported by most of the server, impossible to test
        # chmod_test_all("DCAP" ${dcap_prefix} 0565 000 0320 0767)
        # disable not supported by most of the server, impossible to test
        #test_readdir_full("DCAP" ${dcap_valid_dir_root} )      
        #rmdir_test_all("DCAP" ${dcap_valid_dir_root})
        rwt_test_all("DCAP" ${dcap_valid_dir_root} 4578)        
        rwt_test_seq("DCAP" ${dcap_valid_dir_root} 100 4560)    
ENDIF(PLUGIN_DCAP)


IF(PLUGIN_GRIDFTP)
        test_gfal_test_del_nonex("GRIDFTP_DPM"  "${gsiftp_prefix_dpm}")
        test_gfal_test_del("GRIDFTP_DPM"   "${gsiftp_prefix_dpm}")
        test_gfal_test_mkdir_unlink("GRIDFTP" "${gsiftp_prefix_dpm}")
        stat_test_all( "GRIDFTP" ${gsiftp_valid_dir_root})
        checksum_test_simple("GRIDFTP_ADLER32" ${gsiftp_valid_dir_root} ADLER32)
        checksum_test_simple("GRIDFTP_MD5" ${gsiftp_valid_dir_root} MD5)
        checksum_test_simple("GRIDFTP_CRC32" ${gsiftp_valid_dir_root} CRC32)
        mkdir_test_all("GRIDFTP" ${gsiftp_prefix_dpm})  
        chmod_test_all("GRIDFTP" ${gsiftp_prefix_dpm} 0565 060 0360 0767)
        rmdir_test_all("GRIDFTP" ${gsiftp_valid_dir_root})
        test_readdir_full("GRIDFTP" ${gsiftp_valid_dir_root})
        rwt_test_all("GRIDFTP" ${gsiftp_valid_dir_root} 4578)
        rwt_test_all("GRIDFTP_single" ${gsiftp_valid_dir_root} 1)               
        rwt_test_seq("GRIDFTP" ${gsiftp_valid_dir_root} 100 4560)
        rwt_test_seq("GRIDFTP_unit" ${gsiftp_valid_dir_root} 1 10)              
ENDIF(PLUGIN_GRIDFTP)

IF(PLUGIN_HTTP)
        test_gfal_test_del_nonex("DAVS_DPM" ${davs_valid_dir_root})
        test_gfal_test_del("DAVS_DPM" ${davs_valid_dir_root})
        test_gfal_test_mkdir_unlink("DAVS_DPM" ${davs_valid_dir_root})
        stat_test_all("DAVS" ${davs_valid_dir_root})
        checksum_test_simple("DAVS_ADLER32" ${davs_valid_dir_root} ADLER32)
        checksum_test_simple("DAVS_MD5"     ${davs_valid_dir_root} MD5)
        checksum_test_simple("DAVS_CRC32"   ${davs_valid_dir_root} CRC32)
        mkdir_test_all("DAVS" ${davs_valid_dir_root})
        # chmod not supported
        rmdir_test_all("DAVS" ${davs_valid_dir_root})
        test_readdir_full("DAVS" ${davs_valid_dir_root})
        rwt_test_all("DAVS" ${davs_valid_dir_root} 4578)
        rwt_test_all("DAVS_single" ${davs_valid_dir_root} 1)
        # sequencial writes not supported
ENDIF(PLUGIN_HTTP)

IF(EXISTS "/usr/lib64/gfal2-plugins/libgfal_plugin_xrootd.so" OR
  EXISTS "/usr/lib/gfal2-plugins/libgfal_plugin_xrootd.so") 
    test_gfal_test_del_nonex("XROOTD_DPM" ${root_valid_dir_root})
    test_gfal_test_del("XROOTD_DPM" ${root_valid_dir_root})
    test_gfal_test_mkdir_unlink("XROOTD_DPM" ${root_valid_dir_root})
    stat_test_all("XROOTD" ${root_valid_dir_root})
    # Checksum not supported yet in the XrdCl library
    # Chmod does not work in posix-style, so the test can not be used
    mkdir_test_all("XROOTD" ${root_valid_dir_root})
    rmdir_test_all("XROOTD" ${root_valid_dir_root})
    test_readdir_full("XROOTD" ${root_valid_dir_root})
    rwt_test_all("XROOTD" ${root_valid_dir_root} 4578)
    rwt_test_all("XROOTD_single" ${root_valid_dir_root} 1)
    rwt_test_seq("XROOTD" ${root_valid_dir_root} 100 4560)
    rwt_test_seq("XROOTD_single" ${root_valid_dir_root} 1 10)
ENDIF()

IF (MAIN_TRANSFER)
        copy_file_test_full("GRIDFTP_DPM"               ${gsiftp_prefix_dpm} ${gsiftp_prefix_dpm})   
        copy_file_test_full("SRM_DPM"                   ${srm_valid_dir_root} ${srm_valid_dir_root})
        copy_file_test_full("SRM_DPM_TO_DCACHE"         ${srm_valid_dir_root} ${srm_valid_dcache_dir_root})
        copy_file_test_full("SRM_DCACHE_TO_SRM"         ${srm_valid_dcache_dir_root} ${srm_valid_dir_root})
        copy_file_test_full("GSIFTP_DPM_TO_SRM_DCACHE"  ${gsiftp_prefix_dpm} ${srm_valid_dcache_dir_root})
        copy_file_test_full("SRM_DCACHE"                ${srm_valid_dcache_dir_root} ${srm_valid_dcache_dir_root})
        copy_file_test_full("SRM_TO_GRIDFTP"            ${srm_valid_dir_root} ${gsiftp_prefix_dpm})
        copy_file_test_full("GRIDFTP_TO_SRM"            ${gsiftp_prefix_dpm} ${srm_valid_dir_root})
        copy_file_test_full("DAVS_TO_DAVS"              ${davs_valid_dir_root} ${davs_valid_dir_root})

        # global transfer tests for storage compatibility

        # storm <-> storm
        copy_file_test_full("STORM_TO_STORM" ${srm_prefix_storm}  ${srm_prefix_storm})
        # storm -> dpm
        copy_file_test_full("STORM_TO_SRM_DPM" ${srm_prefix_storm}  ${srm_valid_dir_root})
        # storm -> dcache
        copy_file_test_full("STORM_TO_SRM_DCACHE" ${srm_prefix_storm}  ${srm_valid_dcache_dir_root})

        # local transfer
        #

        # local <-> DPM
        copy_file_test_simple("FILE_TO_SRM_DPM" ${file_prefix} ${srm_valid_dir_root})
        copy_file_test_simple("SRM_DPM_TO_FILE" ${srm_valid_dir_root}  ${file_prefix})

        # local <-> dcache
        copy_file_test_simple("FILE_TO_SRM_DCACHE" ${file_prefix} ${srm_valid_dcache_dir_root})
        copy_file_test_simple("SRM_DCACHE_TO_FILE" ${srm_valid_dcache_dir_root}  ${file_prefix})
        copy_file_test_simple("FILE_TO_FILE" ${file_prefix}  ${file_prefix})

        # gsiftp dpm <-> local
        copy_file_test_simple("GSIFTP_TO_FILE" ${gsiftp_prefix_dpm}  ${file_prefix})
        copy_file_test_simple("FILE_TO_GSIFTP" ${file_prefix}  ${gsiftp_prefix_dpm})

        # local <-> storm
        copy_file_test_simple("STORM_TO_FILE" ${srm_prefix_storm}  ${file_prefix})
        copy_file_test_simple("FILE_TO_STORM" ${file_prefix}  ${srm_prefix_storm})

        # generic timeout tests
        # not reliable test, disable auto execution
        #copy_file_test_timeout("SRM_DPM"    "${srm_valid_dcache_bigfile}" "${srm_valid_dir_root}")
        #copy_file_test_timeout("SRM_DCACHE" "${srm_valid_dcache_bigfile}" "${srm_valid_dcache_dir_root}" )
        #copy_file_test_timeout("SRM_STORM" "${srm_valid_dcache_bigfile}" "${srm_prefix_storm}" )
ENDIF (MAIN_TRANSFER)
