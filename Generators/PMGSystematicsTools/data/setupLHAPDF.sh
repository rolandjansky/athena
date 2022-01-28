tmp=`echo $LHAPATH: | while read -d ':' p ; do find ${p%share*} | grep site-packages | head -n 1 ; done`
export PYTHONPATH=$PYTHONPATH:$tmp
export PATH=$PATH:$LHAPATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${LHAPATH%share*}/lib
