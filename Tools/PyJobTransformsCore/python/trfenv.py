# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# prepare the runtime environment for the transformations
import os,re,sys
#check python version
minPyVersion = 0x20400f0
if sys.hexversion < minPyVersion:
    gotVersion = "%d.%d.%d" % ( (sys.hexversion >> 24) & 0xFF, (sys.hexversion >> 16) & 0xFF, (sys.hexversion >> 8 ) & 0xFF )
    minVersion = "%d.%d.%d" % ( (minPyVersion >> 24) & 0xFF, (minPyVersion >> 16) & 0xFF, (minPyVersion >> 8 ) & 0xFF )
    raise EnvironmentError( "Used python version (%s) too old. Requiring at least version %s" % (gotVersion,minVersion) )

from PyJobTransformsCore.trferr import TransformEnvironmentError
from PyJobTransformsCore import trfconsts,envutil

# no imports out of scope!
__all__ = [ ] 

trfPath = os.path.dirname(__file__)
trfPackagePath = os.path.realpath(os.path.dirname(trfPath))
athena_exe = None

# setup the run-time environment
def setup_runtime():
    # check that LD_LIBRARY_PATH is set

    try:
        os.environ[trfconsts.LD_LIBRARY_PATH]
    except KeyError:
        raise TransformEnvironmentError( 'LD_LIBRARY_PATH not set' )
    
    # add current dir to jo path
    try:
        joPathEnv = os.environ[ trfconsts.JOBOPTIONSPATH ]
    except KeyError:
        jobOptionsPath = [ '' ]
    else:
        jobOptionsPath = re.split( ',|' + os.pathsep, joPathEnv )
        if '' not in jobOptionsPath:
            jobOptionsPath.insert(0, '')
    os.environ[ trfconsts.JOBOPTIONSPATH ] = ','.join(jobOptionsPath)
    #
    # setting up some basic running environment (translated from athena.py)
    #
##     ifs = os.environ.get(trfconsts.IFS,'')
##     ifs += ':'
##     os.environ[trfconsts.IFS] = ifs


def setup_athena_runtime(athena_py=trfconsts.athena_py):
    global athena_exe
    # try to find the athena executable
    athena_exe = envutil.find_executable(athena_py)
    if not athena_exe:
        raise TransformEnvironmentError( '%s not found in %s=%s' % (trfconsts.athena_py, trfconsts.PATH, os.environ[trfconsts.PATH]) )

def cleanup_athena_runtime():
    pass



