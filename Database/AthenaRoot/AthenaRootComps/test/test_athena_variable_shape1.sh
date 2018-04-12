#!/bin/bash

set -e

testname=test_athena_variable_shape/shape1
rm -rf $testname
mkdir -p $testname
cd $testname

echo "::: generate f1.root..."
athena.py \
    -c 'EVTMAX=10; BRANCHES=["RunNumber", "EventNumber", "el_n", "el_eta"]; OUTBRANCHES=["el_n","el_eta"]' \
    AthenaRootComps/test_athena_variable_shape_ntuple.py \
    >| log.001.txt 2>| log.001.stderr.txt \
    || exit 1
/bin/mv d3pd.root f1.root || exit 1
/bin/mv data.var.txt data.1.txt || exit 1
(acmd.py dump-root f1.root 2> f1.stderr.ascii) \
    | grep "^egamma" \
    | egrep "RunNumber|EventNumber|el_n" \
    | tee f1.ascii \
    || return 1
cat f1.ascii| cut -d. -f3 >| f1.ascii.todiff || exit 1

exit 0
