
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
jps.AthenaMPFlags.EventsBeforeFork=3  

import multiprocessing
cpu_list = range( multiprocessing.cpu_count() ) # [0,1,2,..,ncpus] for many-core machine 
cpu_list.reverse() #reverse the cpu-proc pinning order
jps.AthenaMPFlags.AffinityCPUList=cpu_list



from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
# schedule our analysis algorithm
# from file(.py) import classname
from AthenaMP.VetoFirstEvent import VetoFirstEvent
job += VetoFirstEvent(name='EventOutputVeto')
job.EventOutputVeto.OutputLevel = INFO
job.EventOutputVeto.EventsBeforeFork=jps.AthenaMPFlags.EventsBeforeFork

#----------------------------------------------------------------------
# Setting fast-reco w/o Calo, Muon or Trigger.
#----------------------------------------------------------------------

# expect RecExCommon_links.sh to be sourced in curdir

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acFlags
acFlags.EvtMax=10
acFlags.PoolESDOutput="ESD.pool.root"

from RecExConfig.RecFlags import rec
rec.doPerfMon=True

rec.doCalo=False
rec.doMuon=False
#rec.doID=True
rec.doTrigger=False

rec.doESD=True
rec.doAOD=False
rec.doHist=False
rec.doWriteESD=True
rec.doWriteAOD=False
rec.doWriteTAG=False

# if needed to configure trigger
# see https://twiki.cern.ch/twiki/bin/view/Atlas/TriggerFlags
# include ( "TriggerRelease/TriggerFlags.py" )

# if needed to configure AOD building
# see https://twiki.cern.ch/twiki/bin/view/Atlas/UserAnalysisTest#The_AOD_Production_Flags
# from ParticleBuilderOptions.AODFlags import AODFlags

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")


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

StreamESD.VetoAlgs    += ["EventOutputVeto"]
#StreamAOD.VetoAlgs    += ["EventOutputVeto"]
AANTupleStream.ExistDataHeader = False
