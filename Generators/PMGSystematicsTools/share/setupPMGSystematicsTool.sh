source setupRivet.sh
source setupLHAPDF.sh
#if [ -e /usr/lib64/atlas/libsatlas.so ]; then
#   echo "BL:AH"
#   workaroundLib="`pwd`/extraLibs"
#   if [ ! -e $workaroundLib ]; then
#     mkdir -p $workaroundLib
#     ln -s /usr/lib64/atlas/libsatlas.so $workaroundLib/libptf77blas.so.3
#     ln -s /usr/lib64/atlas/libsatlas.so $workaroundLib/libptcblas.so.3
#     ln -s /usr/lib64/atlas/libsatlas.so $workaroundLib/libatlas.so.3
#     ln -s /usr/lib64/atlas/libsatlas.so $workaroundLib/liblapack.so.3 
#   # do the same for any other atlas lib that is missing and needed
#   fi
#   export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$workaroundLib"
#fi
linkpath=`which systematicsTool.py`
toolpath=`readlink -f $linkpath`
tooldir=`dirname $toolpath`
export SYSTTOOLSPATH=`dirname ${tooldir}` 
export PYTHONPATH=$PYTHONPATH:${tooldir}
