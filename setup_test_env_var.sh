#!/bin/bash
export LFC_HOST=cvitblfc1.cern.ch
export LCG_GFAL_INFOSYS=certtb-bdii-top.cern.ch:2170
export GFAL_PLUGIN_DIR=`pwd`/plugins/
export GFAL_CONFIG_DIR=`pwd`/../dist/etc/gfal2/
export LD_LIBRARY_PATH=`pwd`/src:$LD_LIBRARY_PATH
$@
