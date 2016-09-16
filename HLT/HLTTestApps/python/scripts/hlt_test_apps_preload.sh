#!/bin/sh


export USETCMALLOC=true
export USEIMF=true
export LEAKCHECK=false
export TCMALLOC_LIB="libtcmalloc_minimal.so"
export IMF_LIB1="libimf.so"
export IMF_LIB2="libintlc.so.5"
export ADD_PRELOAD

for a in ${@}
do
    if [[ "$a" = "--leak-check"* ]] || [ "$a" = "-Q" ] ; then
        LEAKCHECK=true
    elif [ "$a" = "--tcmalloc" ]; then
        USETCMALLOC=true
    elif [ "$a" = "--stdcmalloc" ]; then
        USETCMALLOC=false
    elif [ "$a" = "--imf" ]; then
        USEIMF=true
    elif [ "$a" = "--stdcmath" ]; then
        USEIMF=false
    elif [ "$a" = "--no-ers-signal-handlers" ]; then
        export TDAQ_ERS_NO_SIGNAL_HANDLERS=1
    elif [[ "$a" = "--preloadlib"* ]] ; then
        ADD_PRELOAD=${a#*=}
        if [ "$a" = "--preloadlib" ] ; then
            echo "ERROR: option --preloadlib needs to be specified with an equals sign (e.g. --preloadlib=foobar.so)"
            exit 1
        fi
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

if [ $USEIMF == true ]; then
fullimf1="$ATLASMKLLIBDIR_PRELOAD/$IMF_LIB1"
fullimf2="$ATLASMKLLIBDIR_PRELOAD/$IMF_LIB2"
    if [ ! -e "$fullimf1" ]; then
        echo "ERROR: $fullimf1 does not exit"
        exit 1
    elif [ ! -e "$fullimf2" ]; then
        echo "ERROR: $fullimf2 does not exit"
        exit 1
    else
        echo "Preloading $IMF_LIB1"
        echo "Preloading $IMF_LIB2"
        if [ -z $LD_PRELOAD ]; then
            export LD_PRELOAD="$fullimf1:$fullimf2"
        else
            export LD_PRELOAD="$fullimf1:$fullimf2:$LD_PRELOAD"
        fi
    fi
fi

# optionally add user-specific preload library
if [ "x$ADD_PRELOAD" != "x" ] ; then
    echo "Preloading $ADD_PRELOAD"
    if [ -z $LD_PRELOAD ]; then
        export LD_PRELOAD="$ADD_PRELOAD"
    else
        export LD_PRELOAD="$ADD_PRELOAD:$LD_PRELOAD"
    fi
else
    unset ATHENA_ADD_PRELOAD
fi
