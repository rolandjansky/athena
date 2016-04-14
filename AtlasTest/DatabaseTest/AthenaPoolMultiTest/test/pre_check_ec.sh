#!/bin/sh -xv
# /** @file pre_check_ec.sh
# @brief generate PoolFileCatalog.xml need by CheckEDM test
# @param test_name
#
# @author Jack Cranshaw (Jack.Cranshaw@cern.ch)
# $Id: pre_check_ec.sh,v 1.2 2005-01-05 21:13:25 cranshaw Exp $
##
# echo "Starting pre_check.sh"
#Cleaning
#echo "## cleaning"
test=$1
/bin/rm -f ${test}.log
# Build pool catalog
cat > PoolFileCatalog.xml <<EOF
<?xml version="1.0" encoding="UTF-8" standalone="no" ?>
<!-- Edited By POOL -->
<!DOCTYPE POOLFILECATALOG SYSTEM "InMemory">
<POOLFILECATALOG>
 
  <File ID="740D8273-E5C8-D811-964F-00D0B7B86559">
    <physical>
      <pfn filetype="ROOT_All" name="/afs/cern.ch/atlas/offline/data/testfile/dc2.002885.pyt_z_ee.g4dig805._0001.pool.root"/>
    </physical>
    <logical/>
  </File>
 
</POOLFILECATALOG>
EOF
#
#echo "## setting up environment "
# source ../cmt/setup.sh
#echo "## ready to run"
#echo $CMTPATH
#echo $JOBOPTSEARCHPATH

# Turn off pool verbose printing
export POOL_OUTMSG_LEVEL=4

# For test remove data file
#echo " For test remove data file"
#/bin/rm -f SimplePoolFile.root
