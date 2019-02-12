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

#------------------------------------------------------------------------------#
#
## Uncomment following to avoid long waits when segfaulting,
## and add "Root.Stacktrace: no" to your .rootrc file
#
# import ROOT
# ROOT.SetSignalPolicy( ROOT.kSignalFast )
#------------------------------------------------------------------------------#

#
## AlgSequence
#

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from SGComps.SGCompsConf import SGInputLoader
topSequence+=SGInputLoader(OutputLevel=DEBUG, ShowEventDump=False)
topSequence.SGInputLoader.Load = [ ('EventInfo','StoreGateSvc+McEventInfo') ]

from xAODEventInfoCnv.xAODEventInfoCnvConf import xAODMaker__EventInfoCnvAlg
topSequence += xAODMaker__EventInfoCnvAlg()

from AthExHive.AthExHiveConf import *

topSequence+=AlgT(OutputLevel=DEBUG)

AlgT = topSequence.AlgT
AlgT.Key_W1 = "t1"
#
## uncomment this line to force an explicit circular data dep in AlgT
# AlgT.Key_R1 = "t1"

#
## implicit circular data dep with parent AlgT
AlgT.Tool1 = HiveTool( "HiveTool1")
AlgT.Tool1.Key_R1 = "t1"
AlgT.Tool1.Key_W1 = "t2"

#
## implicit circular data dep with AlgT.HiveTool1
AlgT.Tool2 = HiveTool( "HiveTool2" )
AlgT.Tool2.Key_R1 = "t2"
AlgT.Tool2.Key_W1 = "t3"

#
## implicit circular data dep with AlgT.HiveTool2
AlgT.Tool3 = HiveTool( "HiveTool3" )
AlgT.Tool3.Key_R1 = "t3"
AlgT.Tool3.Key_W1 = "t4"


#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------

theApp.EvtMax = 2

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

#
## set which Algorithms can be cloned
#

#  set algCardinality = 1 to disable cloning for all Algs
algCardinality = jp.ConcurrencyFlags.NumThreads()

if (algCardinality != 1):
   for alg in topSequence:
       name = alg.name()
       alg.Cardinality = algCardinality
           
