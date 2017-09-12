#!/bin/bash

set -e

testname=test_athena_variable_shape/shape2
rm -rf $testname
mkdir -p $testname
cd $testname

echo "::: generate f2.root..."
athena.py \
    -c 'EVTMAX=10; BRANCHES=["RunNumber", "EventNumber", "el_n", "el_phi"]; OUTBRANCHES=["el_n","el_phi"]' \
    AthenaRootComps/test_athena_variable_shape_ntuple.py \
    >| log.002.txt 2>| log.002.stderr.txt \
    || exit 1
/bin/mv d3pd.root f2.root || exit 1
/bin/mv data.var.txt data.2.txt || exit 1
(acmd.py dump-root f2.root 2> f2.stderr.txt) \
    | grep "^egamma" \
    | egrep "RunNumber|EventNumber|el_n" \
    | tee f2.ascii \
    || exit 1
cat f2.ascii| cut -d. -f3 >| f2.ascii.todiff || exit 1

exit 0
