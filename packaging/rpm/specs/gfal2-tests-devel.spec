%define name gfal2-tests-devel
%define version 2.0
%define release 1.17_preview


%define debug_package %{nil}

Name: %{name}
License: Apache-2.0
Summary: Grid file access library 2.0
Version: %{version}
Release: %{release}
Group: Grid/lcg
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}
BuildRequires: scons, glib2-devel, openldap-devel, libattr-devel, e2fsprogs-devel, lfc-devel, dpm-devel, dcap-devel, cgreen-devel
Requires: openldap, cgreen
Source: %{name}-%{version}-%{release}.src.tar.gz
%description
Unit and functional tests package for gfal 2.0




%clean
[ -n "$RPM_BUILD_ROOT" -a "$RPM_BUILD_ROOT" != / ] && rm -rf "$RPM_BUILD_ROOT";
scons  main_tests=yes debug=yes -c build

%prep
%setup -q

%build
NUMCPU=`grep processor /proc/cpuinfo | wc -l`; if [[ "$NUMCPU" == "0" ]]; then NUMCPU=1; fi;
scons -j $NUMCPU main_tests=yes debug=yes build


%install
[ -n "$RPM_BUILD_ROOT" -a "$RPM_BUILD_ROOT" != / ] && rm -rf "$RPM_BUILD_ROOT"; 
NUMCPU=`grep processor /proc/cpuinfo | wc -l`; if [[ "$NUMCPU" == "0" ]]; then NUMCPU=1; fi;
scons  -j $NUMCPU main_tests=yes debug=yes --install-sandbox="$RPM_BUILD_ROOT" install 


%files
%defattr (-,root,root)
/usr/share/gfal2/tests/mocked/gfal2-gfal2-test-check-bdii-endpoints-srm-ng
/usr/share/gfal2/tests/mocked/gfal2-gfal2-test-gfal-common-lfc-access-guid-file-exist
/usr/share/gfal2/tests/mocked/gfal2-test--dir-file-descriptor-high
/usr/share/gfal2/tests/mocked/gfal2-test--dir-file-descriptor-low
/usr/share/gfal2/tests/mocked/gfal2-test--gfal-common-lfc-rename
/usr/share/gfal2/tests/mocked/gfal2-test--gfal-common-lfc-statg
/usr/share/gfal2/tests/mocked/gfal2-test--gfal-convert-full-surl
/usr/share/gfal2/tests/mocked/gfal2-test--gfal-posix-chmod-read-guid
/usr/share/gfal2/tests/mocked/gfal2-test--gfal-posix-chmod-read-lfn
/usr/share/gfal2/tests/mocked/gfal2-test--gfal-posix-chmod-srm
/usr/share/gfal2/tests/mocked/gfal2-test--gfal-posix-chmod-write-lfn
/usr/share/gfal2/tests/mocked/gfal2-test--gfal-posix-lstat-guid
/usr/share/gfal2/tests/mocked/gfal2-test--gfal-posix-lstat-lfc
/usr/share/gfal2/tests/mocked/gfal2-test--gfal-posix-lstat-srm
/usr/share/gfal2/tests/mocked/gfal2-test--gfal-posix-stat-guid
/usr/share/gfal2/tests/mocked/gfal2-test--gfal-posix-stat-lfc
/usr/share/gfal2/tests/mocked/gfal2-test--gfal-posix-stat-srm
/usr/share/gfal2/tests/mocked/gfal2-test--mkdir-posix-lfc-simple
/usr/share/gfal2/tests/mocked/gfal2-test--mkdir-posix-srm-simple
/usr/share/gfal2/tests/mocked/gfal2-test--opendir-posix-lfc-simple
/usr/share/gfal2/tests/mocked/gfal2-test--opendir-posix-srm-simple-mock
/usr/share/gfal2/tests/mocked/gfal2-test--plugin-lstat
/usr/share/gfal2/tests/mocked/gfal2-test--plugin-stat
/usr/share/gfal2/tests/mocked/gfal2-test--readdir-posix-lfc-simple
/usr/share/gfal2/tests/mocked/gfal2-test--readdir-posix-srm-simple-mock
/usr/share/gfal2/tests/mocked/gfal2-test-access-posix-guid-exist
/usr/share/gfal2/tests/mocked/gfal2-test-access-posix-guid-read
/usr/share/gfal2/tests/mocked/gfal2-test-access-posix-guid-write
/usr/share/gfal2/tests/mocked/gfal2-test-access-posix-lfn-exist
/usr/share/gfal2/tests/mocked/gfal2-test-access-posix-lfn-read
/usr/share/gfal2/tests/mocked/gfal2-test-access-posix-lfn-write
/usr/share/gfal2/tests/mocked/gfal2-test-access-posix-srm-exist
/usr/share/gfal2/tests/mocked/gfal2-test-access-posix-srm-read
/usr/share/gfal2/tests/mocked/gfal2-test-access-posix-srm-write
/usr/share/gfal2/tests/mocked/gfal2-test-check-bdii-endpoints-srm
/usr/share/gfal2/tests/mocked/gfal2-test-common-lfc-checksum
/usr/share/gfal2/tests/mocked/gfal2-test-common-lfc-getcomment
/usr/share/gfal2/tests/mocked/gfal2-test-common-lfc-setcomment
/usr/share/gfal2/tests/mocked/gfal2-test-create-srm-handle
/usr/share/gfal2/tests/mocked/gfal2-test-get-cat-type
/usr/share/gfal2/tests/mocked/gfal2-test-gfal-auto-get-srm-endpoint-full-endpoint-with-no-bdiiG
/usr/share/gfal2/tests/mocked/gfal2-test-gfal-common-lfc-access
/usr/share/gfal2/tests/mocked/gfal2-test-gfal-common-lfc-check-filename
/usr/share/gfal2/tests/mocked/gfal2-test-gfal-common-lfc-define-env
/usr/share/gfal2/tests/mocked/gfal2-test-gfal-common-lfc-getSURL
/usr/share/gfal2/tests/mocked/gfal2-test-gfal-common-lfc-init
/usr/share/gfal2/tests/mocked/gfal2-test-gfal-common-lfc-no-exist
/usr/share/gfal2/tests/mocked/gfal2-test-gfal-common-lfc-resolve-sym
/usr/share/gfal2/tests/mocked/gfal2-test-gfal-full-endpoint-checkG
/usr/share/gfal2/tests/mocked/gfal2-test-gfal-get-endpoint-and-setype-from-bdiiG
/usr/share/gfal2/tests/mocked/gfal2-test-gfal-get-hostname-from-surl
/usr/share/gfal2/tests/mocked/gfal2-test-gfal-select-best-protocol-and-endpointG
/usr/share/gfal2/tests/mocked/gfal2-test-gfal-srm-determine-endpoint-full-endpointG
/usr/share/gfal2/tests/mocked/gfal2-test-gfal-srm-determine-endpoint-not-fullG
/usr/share/gfal2/tests/mocked/gfal2-test-gfal-srm-getTURLS-bad-urls
/usr/share/gfal2/tests/mocked/gfal2-test-gfal-srm-getTURLS-one-success
/usr/share/gfal2/tests/mocked/gfal2-test-gfal-srm-getTURLS-pipeline-success
/usr/share/gfal2/tests/mocked/gfal2-test-load-plugin
/usr/share/gfal2/tests/mocked/gfal2-test-open-posix-all-simple
/usr/share/gfal2/tests/mocked/gfal2-test-open-posix-guid-simple
/usr/share/gfal2/tests/mocked/gfal2-test-open-posix-lfc-simple
/usr/share/gfal2/tests/mocked/gfal2-test-open-posix-srm-simple
/usr/share/gfal2/tests/mocked/gfal2-test-plugin-access-file
/usr/share/gfal2/tests/mocked/gfal2-test-plugin-url-checker
/usr/share/gfal2/tests/mocked/gfal2-test-pread-posix-guid-simple
/usr/share/gfal2/tests/mocked/gfal2-test-pread-posix-lfc-simple
/usr/share/gfal2/tests/mocked/gfal2-test-pread-posix-srm-simple
/usr/share/gfal2/tests/mocked/gfal2-test-pwrite-posix-srm-simple
/usr/share/gfal2/tests/mocked/gfal2-test-read-posix-guid-simple
/usr/share/gfal2/tests/mocked/gfal2-test-read-posix-lfc-simple
/usr/share/gfal2/tests/mocked/gfal2-test-read-posix-srm-simple
/usr/share/gfal2/tests/mocked/gfal2-test-srm-api-no-glib-full
/usr/share/gfal2/tests/mocked/gfal2-test-srm-get-checksum
/usr/share/gfal2/tests/mocked/gfal2-test-verbose-set-get
/usr/share/gfal2/tests/mocked/gfal2-test-write-posix-lfc-simple
/usr/share/gfal2/tests/mocked/gfal2-test-write-posix-srm-simple
/usr/share/gfal2/tests/mocked/test-gskiplist-create-delete
/usr/share/gfal2/tests/mocked/test-gskiplist-insert-get-clean
/usr/share/gfal2/tests/mocked/test-gskiplist-insert-len
/usr/share/gfal2/tests/mocked/test-gskiplist-insert-multi
/usr/share/gfal2/tests/mocked/test-gskiplist-insert-search-remove
/usr/share/gfal2/tests/mocked/test-posix-set-get-false-parameter
/usr/share/gfal2/tests/mocked/test-posix-set-get-infosys-parameter
/usr/share/gfal2/tests/mocked/test-posix-set-get-parameter
/usr/share/gfal2/tests/mocked/test_verbose

%changelog
* Mon Nov 14 2011 adevress at cern.ch 
 - Initial gfal 2.0 test release