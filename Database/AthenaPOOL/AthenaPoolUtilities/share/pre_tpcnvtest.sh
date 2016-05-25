#!/bin/sh

test=$1
if [ "$test" = "" ]; then
  exit 0
fi

testdir=${test}-${CMTCONFIG}
rm -rf $testdir
mkdir $testdir

cd $testdir


