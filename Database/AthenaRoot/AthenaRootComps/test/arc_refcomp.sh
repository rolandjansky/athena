#!/bin/bash

refname=$1
ignore=$2

reffile=`arc_linkinputs.sh`/${refname}
chkfile=d3pd.ascii

if [ -n "${ignore}" ]; then
    ignore="-I${ignore}"
fi

diff -urN ${ignore} $reffile $chkfile >& d3pd.ascii.diff
sc=$?
echo "::: comparing ascii dumps...[$sc]"
if [ $sc -ne 0 ]; then
    cat d3pd.ascii.diff
    exit $sc
fi
