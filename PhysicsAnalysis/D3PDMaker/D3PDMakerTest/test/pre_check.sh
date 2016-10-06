#!/bin/sh

test=$1
if [ "$test" = "" ]; then
  exit 0
fi

testdir=${test}-${CMTCONFIG}
rm -rf $testdir
mkdir $testdir

cd $testdir

# Make sure dcache is set up when running on usatlas.
if [[ `hostname` =~ '.*.usatlas.bnl.gov' ]]; then
  . /afs/usatlas.bnl.gov/i386_redhat72/opt/dcache/dcache_client_config.sh
  if [[ $CMTCONFIG =~ i686 ]]; then
    export LD_LIBRARY_PATH=/afs/usatlas.bnl.gov/i386_redhat72/opt/dcache/vdt/dcache/dcap-2.47.5-0-SL5-i386/lib:$LD_LIBRARY_PATH
  fi
fi


