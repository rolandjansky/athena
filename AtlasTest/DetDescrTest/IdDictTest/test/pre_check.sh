#!/bin/sh -xv
#
 echo "Starting pre_check.sh"
#Cleaning
#echo "## cleaning"
test=$1
/bin/rm -f ${test}.log
#

echo Event data files
#link to first 10 events of the same file on disk
# at cern ATLASTESTDATA = /afs/cern.ch/atlas/offline/data/test
#get_files -symlink -data dc1.001003.simul.0001.test.h130_2e2mu_extract.zebra
#ln -fs dc1*.zebra  ZEBRA.P
#get_files -symlink -data dc2.002885.pyt_z_ee.g4dig805._0001.pool.root
#pool_insertFileToCatalog dc2.002885.pyt_z_ee.g4dig805._0001.pool.root
#FCregisterLFN -p dc2.002885.pyt_z_ee.g4dig805._0001.pool.root -l zee_RDO_extract.pool

#echo PGDTABLE
#get_files -symlink -data PDGTABLE.MeV

echo "## setting up environment "
# source ../cmt/setup.sh
echo "## ready to run"
echo $CMTPATH
echo $JOBOPTSEARCHPATH

# Turn off pool verbose printing
export POOL_OUTMSG_LEVEL=4

# For test remove data file
#echo " For test remove data file"
#/bin/rm -f SimplePoolFile.root
