#!/bin/sh

function check_disappearing_branches()
{

echo "::: retrieve reference ascii file..."
reffile=ref.data.merged.ascii
/bin/rm -f $reffile > /dev/null
#refdir=root://eosatlas//eos/atlas/user/b/binet/utests/utests/filter-d3pd/
#refdir=${PWD}/../test/
#cp $refdir/$reffile $reffile || return 1
get_files -scripts $reffile
echo "::: retrieve reference ascii file... [ok]"

echo "::: generate f1.root..."
athena.py \
    -c 'EVTMAX=10; BRANCHES=["RunNumber", "EventNumber", "el_n", "el_eta"]; OUTBRANCHES=["el_n","el_eta"]' \
    AthenaRootComps/test_athena_variable_shape_ntuple.py \
    >| log.001.txt 2>| log.001.stderr.txt \
    || return 1
/bin/mv d3pd.root f1.root || return 1
/bin/mv data.var.txt data.1.txt || return 1
(acmd.py dump-root f1.root 2> f1.stderr.ascii) \
    | grep "^egamma" \
    | egrep "RunNumber|EventNumber|el_n" \
    | tee f1.ascii \
    || return 1
cat f1.ascii| cut -d. -f3 >| f1.ascii.todiff || return 1

echo "::: generate f2.root..."
athena.py \
    -c 'EVTMAX=10; BRANCHES=["RunNumber", "EventNumber", "el_n", "el_phi"]; OUTBRANCHES=["el_n","el_phi"]' \
    AthenaRootComps/test_athena_variable_shape_ntuple.py \
    >| log.002.txt 2>| log.002.stderr.txt \
    || return 1
/bin/mv d3pd.root f2.root || return 1
/bin/mv data.var.txt data.2.txt || return 1
(acmd.py dump-root f2.root 2> f2.stderr.txt) \
    | grep "^egamma" \
    | egrep "RunNumber|EventNumber|el_n" \
    | tee f2.ascii \
    || return 1
cat f2.ascii| cut -d. -f3 >| f2.ascii.todiff || return 1

echo "::: generate f3.root..."
athena.py \
    -c 'EVTMAX=10; BRANCHES=["RunNumber", "EventNumber", "el_n", "el_eta"]; OUTBRANCHES=["el_n","el_eta"]' \
    AthenaRootComps/test_athena_variable_shape_ntuple.py \
    >| log.003.txt 2>| log.003.stderr.txt \
    || return 1
/bin/mv d3pd.root f3.root || return 1
/bin/mv data.var.txt data.3.txt || return 1
(acmd.py dump-root f3.root 2> f3.stderr.txt) \
    | grep "^egamma" \
    | egrep "RunNumber|EventNumber|el_n" \
    | tee f3.ascii \
    || return 1
cat f3.ascii| cut -d. -f3 >| f3.ascii.todiff || return 1

echo "::: generate merged.root..."
athena.py \
    -c 'EVTMAX=30; BRANCHES=["RunNumber", "EventNumber", "el_n", "el_eta","el_phi"]; OUTBRANCHES=["el_n",]; FNAMES=["f1.root","f2.root","f3.root"]' \
    AthenaRootComps/test_athena_variable_shape_ntuple.py \
    >| log.004.txt 2>| log.004.stderr.txt \
    || return 1
/bin/mv d3pd.root f4.root || return 1
/bin/mv data.var.txt data.4.txt || return 1
(acmd.py dump-root f4.root 2> f4.stderr.txt) \
    | grep "^egamma" \
    | egrep "RunNumber|EventNumber|el_n" \
    | tee f4.ascii \
    || return 1
cat f4.ascii| cut -d. -f3 >| f4.ascii.todiff || return 1

echo "::: compare py-alg outputs..."
cat data.[123].txt > data.merged.txt || return 1
diff -urN data.merged.txt data.4.txt || return 1
echo "::: compare py-alg outputs... [ok]"

echo "::: compare py-alg output to reference..."
diff -urN data.merged.txt ref.data.merged.ascii || return 1
echo "::: compare py-alg output to reference... [ok]"

echo "::: compare dump-root outputs..."
cat f[123].ascii.todiff > merged.ascii.todiff || return 1
diff -urN merged.ascii.todiff f4.ascii.todiff || return 1
echo "::: compare dump-root outputs... [ok]"

# echo "::: compare logfiles..."
# cat log.00[123].txt | grep "py_alg" >| log.merged.txt.todiff || return 1
# grep "py_alg" log.004.txt >| log.004.txt.todiff || return 1
# diff -urN log.merged.txt.todiff log.004.txt.todiff || return 1
# echo "::: compare logfiles... [ok]"

return 0
}

check_disappearing_branches


