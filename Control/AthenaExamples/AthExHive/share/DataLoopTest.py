#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#


# 
## can only run if Hive enabled
#
from AthenaCommon.Logging import log as msg

from AthenaCommon.ConcurrencyFlags import jobproperties as jp
nThreads = jp.ConcurrencyFlags.NumThreads()
if (nThreads < 1) :
   msg.fatal('numThreads must be >0. Did you set the --threads=N option?')
   sys.exit(AthenaCommon.ExitCodes.CONFIGURATION_ERROR)

#
## Override Setup for Hive
#

from AthenaServices.AthenaServicesConf import AthenaHiveEventLoopMgr
# svcMgr.AthenaHiveEventLoopMgr.OutputLevel = INFO

from StoreGate.StoreGateConf import SG__HiveMgrSvc
# svcMgr.EventDataSvc.OutputLevel = INFO

from GaudiHive.GaudiHiveConf import AlgResourcePool
# svcMgr += AlgResourcePool( OutputLevel = INFO );

from AthenaCommon.AlgScheduler import AlgScheduler
AlgScheduler.OutputLevel( INFO )
AlgScheduler.ShowControlFlow( True )
AlgScheduler.ShowDataDependencies( True )
AlgScheduler.setDataLoaderAlg( "SGInputLoader" )


# ThreadPoolService thread local initialization
from GaudiHive.GaudiHiveConf import ThreadPoolSvc
svcMgr += ThreadPoolSvc("ThreadPoolSvc")
svcMgr.ThreadPoolSvc.ThreadInitTools = ["ThreadInitTool"]

from GaudiHive.GaudiHiveConf import AlgResourcePool
svcMgr += AlgResourcePool(OutputLevel=DEBUG)

#---------------------------------------------------------------------------------#

#
## Uncomment following to avoid long waits when segfaulting,
## and add "Root.Stacktrace: no" to your .rootrc file
#
# import ROOT
# ROOT.SetSignalPolicy( ROOT.kSignalFast )

#---------------------------------------------------------------------------------#


#
## AlgSequence
#

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from SGComps.SGCompsConf import SGInputLoader
topSequence+=SGInputLoader(OutputLevel=INFO, ShowEventDump=False)

from AthExHive.AthExHiveConf import *
HA = HiveAlgA("AlgA",OutputLevel=DEBUG,Time=20)
HB = HiveAlgB("AlgB",OutputLevel=DEBUG,Time=10)
HC = HiveAlgC("AlgC",OutputLevel=DEBUG,Time=190)
HE = HiveAlgE("AlgE",OutputLevel=DEBUG,Time=10)

# add a data loop in a sequence to test strict ordering
from AthenaCommon.AlgSequence import AthSequencer
alp = AthSequencer("LoopSeq", OutputLevel=DEBUG, Sequential=True)

loopKey = "l2"
HL1 = HiveAlgL1("AlgL1",OutputLevel=DEBUG,Key_W1=loopKey,Time=110)
HL2 = HiveAlgL2("AlgL2",OutputLevel=DEBUG,Key_R1=loopKey,Key_U1=loopKey,Time=320)
HL3 = HiveAlgL3("AlgL3",OutputLevel=DEBUG,Key_U1=loopKey,Time=120)

# AlgM tests to see if sequence was ordered properly
HM = HiveAlgM("AlgM",OutputLevel=DEBUG,Key_R2=loopKey,Offset=3,Time=30)

alp += HL1
# alp += HL2
# alp += HL3
alp += HM
topSequence += alp

topSequence += HA
topSequence += HB
topSequence += HC
topSequence += HE


#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------

theApp.EvtMax = 10

nProc = jp.ConcurrencyFlags.NumProcs()
if (nProc > 0) :

   #
   ## Basic setup for MP/Hive
   #
   import AthenaCommon.AtlasUnixGeneratorJob

   #
   ## For MP/Hive we need to set the chunk size
   #

   from AthenaCommon.Logging import log as msg
   if (theApp.EvtMax == -1) : 
      msg.fatal('EvtMax must be >0 for hybrid configuration')
      sys.exit(AthenaCommon.ExitCodes.CONFIGURATION_ERROR)

   if ( theApp.EvtMax % nProc != 0 ) :
      msg.warning('EvtMax[%s] is not divisible by nProcs[%s]: MP Workers will not process all requested events',theApp.EvtMax,nProc)

   chunkSize = int (theApp.EvtMax / nProc)

   from AthenaMP.AthenaMPFlags import jobproperties as jps 
   jps.AthenaMPFlags.PreCountedEvents=theApp.EvtMax
   jps.AthenaMPFlags.ChunkSize= chunkSize

   msg.info('AthenaMP workers will process %s events each',chunkSize)



print "==========================================================================================\n"

#
## set which Algorithms can be cloned
#

#  set algCardinality = 1 to disable cloning for all Algs
algCardinality = jp.ConcurrencyFlags.NumThreads()

if (algCardinality != 1):
   for alg in topSequence:
       name = alg.name()
       alg.Cardinality = algCardinality
           
