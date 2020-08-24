#!/bin/sh

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

## @package athena_wrapper
#
#  @brief Wrapper script for @c athena.py.
#  @details This script attempts to:
#  @li Mimic @c athena.py's memory management settings.
#  @li Translate relevant options given at the transform command line to @c athena.py options.
#  @li Handle any exceptions that arise from the execution of @c athena.py and create the
#  appropriate AtlasErrorCodes.ErrorInfo object and a corresponding job report file.
#

"""date"      # executable for the shell, but a string block to python 
# First part (shell part) copied from athena.py

# "detect" valgrind
usesvalgrind=`echo "$LD_PRELOAD" | grep valgrind`

python_path=`which python`

### Ugly hack to preload libtcmalloc.so

#use tcmalloc by default if TCMALLOCDIR is defined
export USETCMALLOC=0

# possibly, allow user to run with full tcmalloc by setting this variable
if [ -z $USETCMALLOCMINIMAL ]; then
    export USETCMALLOCMINIMAL=1
fi

# if any of these variables is defined, we use tcmalloc to do some
# profiling, then we need the full library !
if [ -n "$CPUPROFILE" ] || [ -n "$HEAPPROFILE" ] || [ -n "$HEAPCHECK" ]; then
    USETCMALLOCMINIMAL=0
fi

if [ -z $TCMALLOCDIR ]; then
   echo "WARNING: TCMALLOCDIR not defined, will use libc malloc"
   USETCMALLOC=0
else
   USETCMALLOC=1
fi

export ATHENA_ADD_PRELOAD=''

genericOptions=""

for a in ${@}
do
    case $a in
        --leak-check*)      
            USETCMALLOC=0
            genericOptions="$genericOptions $a";;
        --delete-check*)    
            USETCMALLOC=0
            genericOptions="$genericOptions $a";;
        --stdcmalloc)        
            USETCMALLOC=0
            genericOptions="$genericOptions $a";;
        --tcmalloc)         
            USETCMALLOC=1
            genericOptions="$genericOptions $a";;
        --rss*)
            rssval=`expr "$a" : '--rss=\([0-9]*\)'`
            if [ "$rssval" != "" ]; then
                ulimit -m $((rssval*1024))
                echo "Maximum resident memory: " `ulimit -m` "Kb"
            fi;;
        --vmem*)
            vmemval=`expr "$a" : '--vmem=\([0-9]*\)'`
            if [ "$vmemval" != "" ]; then
                ulimit -v $((vmemval*1024))
                echo "Maximum virtual memory: " `ulimit -v` "Kb"
            fi;;
        --preloadlib*)
            ATHENA_ADD_PRELOAD=${a#*=};;
        *)
            genericOptions="$genericOptions $a";;
    esac
done

if [ $USETCMALLOC == 1 ]; then
   if [ -z $TCMALLOCDIR ]; then
      echo "ERROR: TCMALLOCDIR not defined"
      exit 1
   fi
   # test, if minimal tcmalloc is available. fallback to full library, if not
   if [ $USETCMALLOCMINIMAL == 1 ]; then
      if [ ! -e "$TCMALLOCDIR/libtcmalloc_minimal.so" ]; then
         echo "WARNING: $TCMALLOCDIR/libtcmalloc_minimal.so does not exist. Falling back to libtcmalloc"
         USETCMALLOCMINIMAL=0
      else
         echo "Preloading tcmalloc_minimal.so"
      fi
   fi
   # finally, preload and run with correct tcmalloc, if requested
   if [ $USETCMALLOCMINIMAL == 0 ]; then
      if [ ! -e "$TCMALLOCDIR/libtcmalloc.so" ]; then
         echo "ERROR: $TCMALLOCDIR/libtcmalloc.so does not exist"
         exit 1
      fi
      echo "Preloading tcmalloc.so"
      if [ -z $LD_PRELOAD ]; then
         export LD_PRELOAD="$TCMALLOCDIR/libtcmalloc.so"
      else
         export LD_PRELOAD="$TCMALLOCDIR/libtcmalloc.so:$LD_PRELOAD"
      fi
   else
      if [ -z $LD_PRELOAD ]; then
         export LD_PRELOAD="$TCMALLOCDIR/libtcmalloc_minimal.so"
      else
         export LD_PRELOAD="$TCMALLOCDIR/libtcmalloc_minimal.so:$LD_PRELOAD"
      fi
   fi
fi

# optionally add user-specific preload library
if [ -n "$ATHENA_ADD_PRELOAD" ]; then
    echo "Preloading $ATHENA_ADD_PRELOAD"
    if [ -z $LD_PRELOAD ]; then
        export LD_PRELOAD="$ATHENA_ADD_PRELOAD"
    else
        export LD_PRELOAD="$ATHENA_ADD_PRELOAD:$LD_PRELOAD"
    fi
else
    unset ATHENA_ADD_PRELOAD
fi


if [ "$usesvalgrind" != "" ]; then
   ${python_path} -tt "$0" ${genericOptions}        # no exec, a requirement of valgrind?
   exit $?
else
   exec ${python_path} -tt "$0" ${genericOptions}
fi
"""           # python execution starts here, the shell never reaches this
#"""  # extra line to fool emacs

# Don't want to use from __future__ import print_function here: that would
# then apply to everything that we read with include().
import builtins
printfunc = getattr(builtins,'print')

import sys
from past.builtins import execfile
from AthenaCommon.Include import IncludeError
from PyJobTransformsCore import trferr, trfconsts
from PyJobTransformsCore.JobReport import JobReport
from PyJobTransformsCore import AtlasErrorCodes

# flake8: noqa

## The err variable will be used to contain an ErrorInfo instance 
#  after the trferr.errorHandler handles an exception.
err = None

# Hack to support spaces in the value of the '-c' parameter.
# It is assumed that the '-c' option is the last option to be given (if at all)
try:
    ## Position of the '-c' option given at the command line.
    cPos = sys.argv.index( '-c' )
except Exception:
    pass
else:
    sys.argv[ cPos + 1 ] = ' '.join( sys.argv[ cPos + 1 : ] )
    while len( sys.argv ) > cPos + 2:
        sys.argv.pop()

try:
    sys.argv[ 0 ] = trfconsts.athena_py
    ## The athena executable is expected to be the first argument
    athena_exe = sys.argv[ 1 ]
    sys.argv.pop( 0 ) # remove first argument (i.e. athena full path)    
    printfunc (' '.join( sys.argv ) )
    execfile( athena_exe )

# Known exceptions not deriving from exceptions.Exception
# (the only way to catch the object)
except IncludeError as e:
    err = trferr.errorHandler.handleException( e )    

except KeyboardInterrupt as e: 
    err = trferr.errorHandler.handleException( e )

except SystemExit as e:
    err = trferr.errorHandler.handleException( e )
    try:
        ## Retrieve the error argument. Previous version of @c athena.py does not provide any arguments when successful.
        #  This will result in an exception when @c arg argument is accessed.
        rc = e.args[ 0 ]
        # newer versions of @c athena.py provide an error argument.
        # Raise Exception to make use of the same try/exception structure.
        if rc == 0:
            raise Exception
    except Exception: # successful athena job
        printfunc ('%s - exit code 0.' % ' '.join( sys.argv ))
        sys.exit( 0 )
    else: # unsuccessful athena job
        ## Create a blank JobReport instance and populate it with the error detected.
        jobReport = JobReport()
        jobReport.setProducer( 'athena' )
        jobReport.addError( err )
        jobReport.write()
        printfunc ('%s - exit code %s' % ( ' '.join( sys.argv ), rc ))
        sys.exit( rc )

# Exceptions derived from exceptions.Exception
except Exception as e:
    err = trferr.errorHandler.handleException( e )
    if err is None:
        err = AtlasErrorCodes.ErrorInfo( acronym = 'ATH_EXC_PYT',
                                         severity = AtlasErrorCodes.FATAL,
                                         message = '%s: %s' % ( e.__class__.__name__, e.args ) )

# Some throw a string
except str as e:
    err = AtlasErrorCodes.ErrorInfo( acronym = 'ATH_EXC_PYT',
                                     severity = AtlasErrorCodes.FATAL,
                                     message = e )
# Catch all other exceptions
except Exception:
    err = AtlasErrorCodes.ErrorInfo( acronym = 'ATH_EXC_PYT',
                                      severity = AtlasErrorCodes.FATAL )

if err is not None:
    jobReport = JobReport()
    jobReport.setProducer( 'athena' )
    jobReport.addError( err )
    jobReport.write()
    sys.exit( jobReport.exitCode() )
