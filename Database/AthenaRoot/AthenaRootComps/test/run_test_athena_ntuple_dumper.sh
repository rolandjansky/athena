#!/bin/sh

function run_test() {
    echo "::: running athenarootcomps-r/w basic test..."
    IIMAX=5
    if [[ "$CMTCONFIG" == *-dbg* ]]; then
        IIMAX=1
        echo "::: setting IIMAX=1 (debug mode is slower)"
    else
        IIMAX=5
        echo "::: setting IIMAX=5 (opt mode is faster)"
    fi

    /bin/rm -rf arc-ntuple-dumper-tests
    mkdir arc-ntuple-dumper-tests
    pushd arc-ntuple-dumper-tests

    echo "::: retrieve reference ascii file..."
    reffile=ref.d3pd.ascii
    chkfile=d3pd.ascii
    /bin/rm -f $reffile > /dev/null
    /bin/rm -f $chkfile > /dev/null
    refdir=root://eosatlas//eos/atlas/user/b/binet/utests/utests/filter-d3pd/
    # Depending on the LCG version, we may get the xrdcp binary either from
    # LCG or from the system installation.  If the latter, then we need to be
    # sure we don't have the LCG libraries on LD_LIBRARY_PATH or xrdcp
    # will likely crash.
    if [ `which xrdcp` = "/usr/bin/xrdcp" ]; then
      LD_LIBRARY_PATH='' xrdcp $refdir/$reffile $reffile || return 1
    else
      xrdcp $refdir/$reffile $reffile || return 1
    fi

    echo ""
    echo "::::::::::::::::::::::::::::::::::::::::::::::"
    echo "::: run athena-ntuple-dumper... (w/ multiple output-streams)"
    time_cmd="/usr/bin/time -a -o d3pd.rw.timing.log"
    /bin/rm -rf d3pd.rw.timing.log >& /dev/null
    ($time_cmd athena.py -c'USEVARHANDLE=1; DOWRITE=2' -lERROR AthenaRootComps/test_athena_ntuple_dumper.py >& d3pd.000.0.log.txt) || return 1
    grep "user" d3pd.rw.timing.log
    echo "::: comparing ascii dumps..."
    /bin/rm -f d3pd.ascii.diff
    diff -urN $reffile $chkfile >& d3pd.ascii.diff
    sc=$?
    echo "::: comparing ascii dumps...[$sc]"
    if [ $sc -ne 0 ]; then
	    return $sc
    fi
    echo "::: comparing output n-tuples..."
    acmd.py diff-root d3pd.root d3pd_2.root -t egamma 2> /dev/null
    sc=$?
    echo "::: comparing output n-tuples...[$sc]"
    if [ $sc -ne 0 ]; then
	    return $sc
    fi

    echo ""
    echo "::::::::::::::::::::::::::::::::::::::::::::::"
    echo "::: run athena-ntuple-dumper... (w/ varhandles, w/o output)"
    time_cmd="/usr/bin/time -a -o d3pd.rw.timing.log"
    /bin/rm -rf d3pd.rw.timing.log >& /dev/null
    for ii in `seq $IIMAX`; do
        echo " - iter $ii/$IIMAX..."
        ($time_cmd athena.py -c'USEVARHANDLE=1; DOWRITE=0' -lERROR AthenaRootComps/test_athena_ntuple_dumper.py >& d3pd.001.$ii.log.txt) || return 1
    done
    grep "user" d3pd.rw.timing.log
    echo "::: comparing ascii dumps..."
    /bin/rm -f d3pd.ascii.diff
    diff -urN $reffile $chkfile >& d3pd.ascii.diff
    sc=$?
    echo "::: comparing ascii dumps...[$sc]"

    if [ $sc -ne 0 ]; then
	    return $sc
    fi

    echo ""
    echo "::::::::::::::::::::::::::::::::::::::::::::::"
    echo "::: run athena-ntuple-dumper... (w/ varhandles)"
    time_cmd="/usr/bin/time -a -o d3pd.rw.timing.log"
    /bin/rm -rf d3pd.rw.timing.log >& /dev/null
    for ii in `seq $IIMAX`; do
        echo " - iter $ii/$IIMAX..."
        ($time_cmd athena.py -c'USEVARHANDLE=1' -lERROR AthenaRootComps/test_athena_ntuple_dumper.py >& d3pd.002.$ii.log.txt) || return 1
    done
    grep "user" d3pd.rw.timing.log
    echo "::: comparing ascii dumps..."
    /bin/rm -f d3pd.ascii.diff
    diff -urN $reffile $chkfile >& d3pd.ascii.diff
    sc=$?
    echo "::: comparing ascii dumps...[$sc]"

    if [ $sc -ne 0 ]; then
	    return $sc
    fi


    echo ""
    echo "::::::::::::::::::::::::::::::::::::::::::::::"
    echo "::: run athena-ntuple-dumper... (w/o varhandles)"
    time_cmd="/usr/bin/time -a -o d3pd.rw.timing.log"
    /bin/rm -rf d3pd.rw.timing.log >& /dev/null
    for ii in `seq $IIMAX`; do
        echo " - iter $ii/$IIMAX..."
        ($time_cmd athena.py -c'USEVARHANDLE=0' -lERROR AthenaRootComps/test_athena_ntuple_dumper.py >& d3pd.003.$ii.log.txt) || return 1
    done
    grep "user" d3pd.rw.timing.log
    echo "::: comparing ascii dumps..."
    /bin/rm -f d3pd.ascii.diff
    diff -urN $reffile $chkfile >& d3pd.ascii.diff
    sc=$?
    echo "::: comparing ascii dumps...[$sc]"

    if [ $sc -ne 0 ]; then
	return $sc
    fi

    echo ""
    echo "::::::::::::::::::::::::::::::::::::::::::::::"
    echo "::: reading back n-tuple file..."
    /bin/mv d3pd.root rb.d3pd.root || return 1
    /bin/rm -f $chkfile > /dev/null
    time_cmd="/usr/bin/time -a -o d3pd.rb.timing.log"
    /bin/rm -rf d3pd.rb.timing.log >& /dev/null
    for ii in `seq $IIMAX`; do
        echo " - iter $ii/$IIMAX..."
        ($time_cmd athena.py -c'FNAMES=["rb.d3pd.root"]' -lERROR AthenaRootComps/test_athena_ntuple_dumper.py >& d3pd.004.$ii.log.txt) || return 1
    done
    grep "user" d3pd.rb.timing.log
    echo "::: comparing ascii dumps..."
    /bin/rm -f rb.d3pd.ascii.diff
    diff -urN $reffile $chkfile >& rb.d3pd.ascii.diff
    sc=$?
    echo "::: comparing ascii dumps...[$sc]"

    return $sc
}

run_test

