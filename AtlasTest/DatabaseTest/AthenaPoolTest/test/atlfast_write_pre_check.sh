#
 echo "Starting atlfast_write_pre_check.sh"
#Cleaning
echo "## cleaning"
test=$1
/bin/rm -f ${test}.log
#echo "## setting up run time files"
# source ../share/RecExCommon_links.sh
#
echo Event data files
#link to first 10 events of the same file on disk
# at cern ATLASTESTDATA = /afs/cern.ch/atlas/offline/data/test
get_files -symlink -data dc1.001003.simul.redig.00001.test.h130_2e2mu_extract.zebra
ln -fs dc1*.zebra  ZEBRA.P

echo PGDTABLE
get_files -symlink -data PDGTABLE.MeV

echo "## setting up environment "
 source ../cmt/setup.sh
echo "## ready to run"
echo $CMTPATH
echo $JOBOPTSEARCHPATH
