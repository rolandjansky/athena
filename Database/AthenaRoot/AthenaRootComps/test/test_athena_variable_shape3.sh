#!/bin/bash

set -e

testname=test_athena_variable_shape/shape3
rm -rf $testname
mkdir -p $testname
cd $testname

echo "::: generate f3.root..."
athena.py \
    -c 'EVTMAX=10; BRANCHES=["RunNumber", "EventNumber", "el_n", "el_eta"]; OUTBRANCHES=["el_n","el_eta"]' \
    AthenaRootComps/test_athena_variable_shape_ntuple.py \
    >| log.003.txt 2>| log.003.stderr.txt \
    || exit 1
/bin/mv d3pd.root f3.root || exit 1
/bin/mv data.var.txt data.3.txt || exit 1
(acmd.py dump-root f3.root 2> f3.stderr.txt) \
    | grep "^egamma" \
    | egrep "RunNumber|EventNumber|el_n" \
    | tee f3.ascii \
    || exit 1
cat f3.ascii| cut -d. -f3 >| f3.ascii.todiff || exit 1

exit 0
