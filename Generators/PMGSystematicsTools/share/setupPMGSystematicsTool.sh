source setupRivet.sh
source setupLHAPDF.sh
linkpath=`which systematicsTool.py`
toolpath=`readlink -f $linkpath`
tooldir=`dirname $toolpath`
export SYSTTOOLSPATH=`dirname ${tooldir}` 
export PYTHONPATH=$PYTHONPATH:${tooldir}
