#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# 
## can only run if Hive enabled
# file copied and modified from Control/AthenaExamples/AthExHive/CondAlgOpts.py
#
from AthenaCommon.Logging import log as msg

from AthenaCommon.ConcurrencyFlags import jobproperties as jp
nThreads = jp.ConcurrencyFlags.NumThreads()
if (nThreads < 1) :
   msg.fatal('numThreads must be >0. Did you set the --threads=N option?')
   sys.exit(AthenaCommon.ExitCodes.CONFIGURATION_ERROR)

# setup the McEventSelector
import AthenaCommon.AtlasUnixGeneratorJob
svcMgr.EventSelector.FirstEvent=0
svcMgr.EventSelector.RunNumber=1
svcMgr.EventSelector.InitialTimeStamp=0
svcMgr.EventSelector.TimeStampInterval=1
svcMgr.EventSelector.FirstLB=1
svcMgr.EventSelector.EventsPerLB = 5000

#
## Override Setup for Hive
from AthenaServices.AthenaServicesConf import AthenaHiveEventLoopMgr

from StoreGate.StoreGateConf import StoreGateSvc

from AthenaCommon.AlgScheduler import AlgScheduler
AlgScheduler.OutputLevel( INFO )
AlgScheduler.ShowControlFlow( True )
AlgScheduler.ShowDataDependencies( True )
AlgScheduler.EnableConditions( True )
AlgScheduler.setDataLoaderAlg( "SGInputLoader" )

from IOVSvc.IOVSvcConf import CondSvc
svcMgr += CondSvc( OutputLevel=INFO )

## SETUP FOR TRACKING GEOMETRY

# Use Global flags and DetFlags.
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.GlobalFlags import globalflags

from AthenaCommon.ConcurrencyFlags import jobproperties as jp
nThreads = jp.ConcurrencyFlags.NumThreads()

# Just the pixel and SCT
DetFlags.ID_setOn()
DetFlags.detdescr.pixel_setOn()
DetFlags.detdescr.SCT_setOn()


# MC or data - affects which conditions database instance is used
globalflags.DataSource='geant4'
#globalflags.DataSource='data'

# Select the geometry version. 
globalflags.DetDescrVersion = 'ATLAS-R2-2016-00-00-00'

# print "HERE"
# print globalflags.DetDescrVersion

# Initialize geometry
# THIS ACTUALLY DOES STUFF!!
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetGeometryVersion

from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag('OFLCOND-SIM-00-00-00')
conddb.addOverride("/Indet/Align", "InDetAlign_R2_Nominal")

from AthenaCommon.AppMgr import ServiceMgr
from ActsGeometry.ActsGeometryConfig import TrackingGeometrySvc

trkGeomSvc = TrackingGeometrySvc()
trkGeomSvc.UseMaterialMap = False
# trkGeomSvc.MaterialMapInputFile = "MaterialMaps.pb"
trkGeomSvc.BarrelMaterialBins = [40, 60] # phi z
trkGeomSvc.EndcapMaterialBins = [50, 20] # phi r
trkGeomSvc.OutputLevel = INFO
ServiceMgr += trkGeomSvc

exCellWriterSvc = CfgMgr.Acts__ExCellWriterSvc("ExCellWriterSvc")
exCellWriterSvc.FilePath = "excells_charged_aligned.root"
ServiceMgr += exCellWriterSvc

exTool = CfgMgr.Acts__ExtrapolationTool("ExtrapolationTool")
exTool.OutputLevel = INFO
exTool.FieldMode = "Constant"
exTool.ConstantFieldVector = [0, 0, 0]
exTool.MatEngineDoELossCorrection = False

import MagFieldServices.SetupField


## END: SETUP FOR TRACKING GEOMETRY


#-----------------------------------------------------------------------------#

#
## Uncomment following to avoid long waits when segfaulting,
## and add "Root.Stacktrace: no" to your .rootrc file
#
# import ROOT
# ROOT.SetSignalPolicy( ROOT.kSignalFast )

#-----------------------------------------------------------------------------#


# from StoreGate.StoreGateConf import StoreGateSvc
# theApp.CreateSvc += ["StoreGateSvc/ConditionStore"]

#
## AlgSequence
#

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthExHive import AthExHiveConf
from ActsGeometry import ActsGeometryConf

extrapAlg = ActsGeometryConf.ActsAlignedExtrapAlg("ActsAlignedExtrapAlg_1", 
                                                    OutputLevel=VERBOSE)

extrapAlg.ExtrapolationTool = exTool

topSequence += extrapAlg

from AthenaCommon.AlgSequence import AthSequencer 
condSeq = AthSequencer("AthCondSeq") 

condSeq+=ActsGeometryConf.GeomShiftCondAlg("GeomShiftCondAlg_1",
                                            ZShiftPerLB=0.5,
                                            OutputLevel=VERBOSE)


# condDbFile = "condDb.txt"
# import os
# import string
# for dir in string.split( (".:"+os.environ.get('DATAPATH')),":"):
   # cdb = os.path.join(dir,condDbFile)
   # if (os.path.isfile( cdb ) ) :
      # found = 1
      # break

# if (found == 0):
   # msg.fatal('ASCII condDb file \"' + condDbFile + '\" not found')
   # sys.exit(AthenaCommon.ExitCodes.CONFIGURATION_ERROR)
# else:
   # msg.info( "using ASCIICondDb file from " + cdb )

# svcMgr += AthExHiveConf.ASCIICondDbSvc( OutputLevel=DEBUG, CondFile = cdb )


#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------

eventPrintFrequency = 500
if hasattr(ServiceMgr,"AthenaEventLoopMgr"):
    ServiceMgr.AthenaEventLoopMgr.EventPrintoutInterval = eventPrintFrequency
if hasattr(ServiceMgr,"AthenaHiveEventLoopMgr"):
    ServiceMgr.AthenaHiveEventLoopMgr.EventPrintoutInterval = eventPrintFrequency

theApp.EvtMax = 10000

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
           
