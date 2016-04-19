#!/bin/sh


export USETCMALLOC=true
export LEAKCHECK=false
export TCMALLOC_LIB="libtcmalloc_minimal.so"

for a in ${@}
do
    if [[ "$a" = "--leak-check"* ]] || [ "$a" = "-Q" ] ; then
        LEAKCHECK=true
    elif [ "$a" = "--tcmalloc" ]; then
        USETCMALLOC=true
    elif [ "$a" = "--stdcmalloc" ]; then
        USETCMALLOC=false	
    elif [ "$a" = "--no-ers-signal-handlers" ]; then
	export TDAQ_ERS_NO_SIGNAL_HANDLERS=1
    fi
done

if [ $USETCMALLOC == true ]; then
    if [ $LEAKCHECK == true ]; then
        echo 'WARNING: cannot use tcmalloc and do leak check simultaneously. Will use libc malloc instead'
        USETCMALLOC=false
    elif [ -z TCMALLOCDIR ]; then
        echo "WARNING: TCMALLOCDIR not defined. Will use libc malloc instead"
        USETCMALLOC=false
    fi
fi

if [ $USETCMALLOC == true ]; then
    if [ ! -e "$TCMALLOCDIR/$TCMALLOC_LIB" ]; then
        echo "ERROR: $TCMALLOCDIR/$TCMALLOC_LIB does not exist"
        exit 1
    else
        echo "Preloading $TCMALLOC_LIB"
        export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$TCMALLOCDIR"
# if 32 bit binaries on an 64 bit OS are used add also the path to the 64 bit tcmalloc version
# this may be needed in subshells which are opened by the code  
	if [ `echo $CMTCONFIG | cut -d '-' -f1` == "i686" ]; then
	    echo "Preloading also 64 bit version of $TCMALLOC_LIB"
	    cmtconfig64=`echo $CMTCONFIG | sed s/i686/x86_64/`
	    export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$TCMALLOCDIR/../../$cmtconfig64/lib"
	fi
        if [ -z $LD_PRELOAD ]; then
            export LD_PRELOAD="$TCMALLOC_LIB"
        else
            export LD_PRELOAD="$TCMALLOC_LIB:$LD_PRELOAD"
        fi
    fi
fi
