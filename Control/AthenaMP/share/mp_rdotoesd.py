
#==============================================================
# Job Options for fast-reco with AthenaMP
#==============================================================

# assumptions:
# 1. RecExCommon_links.sh to be sourced in curdir
# 2. CLI option --nprocs to be used in command line run of athena.py


#----------------------------------------------------------------------
# AthenaMP properties
#----------------------------------------------------------------------
# expect "--nprocs" to be used in command line options of athena.py
from AthenaMP.AthenaMPFlags import jobproperties as jps
jps.AthenaMPFlags.EventsBeforeFork=0 

import multiprocessing
cpu_list = range( multiprocessing.cpu_count() ) # [0,1,2,..,ncpus] for many-core machine 
cpu_list.reverse() #reverse the cpu-proc pinning order
jps.AthenaMPFlags.AffinityCPUList=cpu_list


#----------------------------------------------------------------------
# setting mp_rdotoesd reco
#----------------------------------------------------------------------
# expect RecExCommon_links.sh to be sourced in curdir



# main jobOption
include ("RecExCommon/rdotoesd.py")


#----------------------------------------------------------------------
# FOR DEBUGGING PURPOSES
#----------------------------------------------------------------------
#from AthenaCommon.AppMgr import theApp
#theApp.ReflexPluginDebugLevel = 10000

#from AthenaCommon.Logging import log as msg
#msg.info ( "svcMgr=%s" % svcMgr)
#msg.info ( "appMgr=%s" % theApp)


#----------------------------------------------------------------------
# user modifier should come here
#----------------------------------------------------------------------
