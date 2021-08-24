#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#


# 
## can only run if Hive enabled
#

from AthenaCommon.ConcurrencyFlags import jobproperties as jp
nThreads = jp.ConcurrencyFlags.NumThreads()
if (nThreads < 1) :
   from AthenaCommon.Logging import log as msg
   msg.fatal('numThreads must be >0. Did you set the --threads=N option?')
   sys.exit(AthenaCommon.ExitCodes.CONFIGURATION_ERROR)

#
## Override Setup for Hive
#

# from AthenaServices.AthenaServicesConf import AthenaHiveEventLoopMgr
# svcMgr.AthenaHiveEventLoopMgr.OutputLevel = INFO

# from StoreGate.StoreGateConf import SG__HiveMgrSvc
# svcMgr.EventDataSvc.OutputLevel = INFO

# from GaudiHive.GaudiHiveConf import AlgResourcePool
# svcMgr += AlgResourcePool( OutputLevel = INFO );

# from GaudiHive.GaudiHiveConf import ForwardSchedulerSvc
# svcMgr += ForwardSchedulerSvc()
# svcMgr.ForwardSchedulerSvc.OutputLevel = INFO

#
## Override defaults. otherwise these are ALL EQUAL to the number of threads
## set with the command line opt --threads=N
#

# numStores = 1
# numAlgsInFlight = 1

# svcMgr.EventDataSvc.NSlots = numStores
# svcMgr.ForwardSchedulerSvc.MaxEventsInFlight = numStores
# svcMgr.ForwardSchedulerSvc.MaxAlgosInFlight = numAlgsInFlight

#---------------------------------------------------------------------------------#

#
## Uncomment following to avoid long waits when segfaulting,
## and add "Root.Stacktrace: no" to your .rootrc file
#
# import ROOT
# ROOT.SetSignalPolicy( ROOT.kSignalFast )

#---------------------------------------------------------------------------------#
# check to see if we're running hybrid mp/mt
theApp.EvtMax = 50

nProc = jp.ConcurrencyFlags.NumProcs()
if (nProc > 0) :

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
   jps.AthenaMPFlags.ChunkSize= chunkSize

   msg.info('AthenaMP workers will process %s events each',chunkSize)

#---------------------------------------------------------------------------------#

# declare if you want to use jets and if so how many
#  this needs to be done before data dependencies are declared
useJets=False
nJets=25

#---------------------------------------------------------------------------------#
#
## Alg Data Dependencies
#

include ( "CaloHiveEx/CaloHiveDeps.py" )

#
## force loading of data. make sure this alg is at the front of the
## AlgSequence
#

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from CaloHiveEx.CaloHiveExConf import SGInputLoader
topSequence+=SGInputLoader(OutputLevel=DEBUG, ShowEventDump=False)

from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
topSequence+=xAODMaker__EventInfoCnvAlg()

from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
from xAODTrigL1CaloCnv.xAODTrigL1CaloCreator import xAODMaker__TriggerTowerCnvAlg
topSequence+=xAODMaker__TriggerTowerCnvAlg(xAODKey="xAODTriggerTowers")

#---------------------------------------------------------------------------------#

#
## make sure PerfMon is off
#

include( "PerfMonGPerfTools/DisablePerfMon_jobOFragment.py" )

# replace by autoconfiguration
#from AthenaCommon.GlobalFlags import globalflags
#globalflags.ConditionsTag = 'OFLCOND-DR-BS7T-ANom-11'
#globalflags.DataSource = 'geant4'
#globalflags.DetDescrVersion = 'ATLAS-GEO-10-00-00'
#globalflags.DetGeo = 'atlas'
#globalflags.InputFormat = 'pool'

dataFile = "/afs/cern.ch/atlas/offline/ReleaseData/v16/testfile/mc09_7TeV.105200.T1_McAtNlo_Jimmy.digit.RDO.e510_s765_s767_d297_tid124958_00_RDO.124958._000001_10evt.pool.root"

from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
athenaCommonFlags.FilesInput=[dataFile,dataFile,dataFile,dataFile,dataFile,dataFile,dataFile,dataFile,dataFile,dataFile,dataFile]

from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.AppMgr import ToolSvc,theApp,ServiceMgr

# AutoConfiguration
from RecExConfig.RecFlags import rec
rec.AutoConfiguration = ['everything']
import RecExConfig.AutoConfiguration as auto
auto.ConfigureFromListOfKeys(rec.AutoConfiguration())

# ToolSvc.OutputLevel = INFO
# ToolSvc.numSlots = numStores


#from AthenaCommon.KeyStore import CfgItemList, CfgKeyStore
from RecExConfig.ObjKeyStore import objKeyStore, CfgKeyStore
from PyUtils.MetaReaderPeeker import convert_itemList
objKeyStore.addManyTypesInputFile(convert_itemList(layout = '#join'))

topSequence = AlgSequence()

import DetDescrCnvSvc.DetStoreConfig
ServiceMgr.DetDescrCnvSvc.IdDictFromRDB = True

from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
from DetDescrCnvSvc.DetDescrCnvSvcConf import DetDescrCnvSvc
ServiceMgr += GeoModelSvc()
theApp.CreateSvc += [ "GeoModelSvc"]

from LArGeoAlgsNV.LArGeoAlgsNVConf import LArDetectorToolNV
from TileGeoModel.TileGeoModelConf import TileDetectorTool

ServiceMgr.GeoModelSvc.DetectorTools += [ LArDetectorToolNV(ApplyAlignments = True, GeometryConfig = "RECO"),
                                          TileDetectorTool(GeometryConfig = "RECO")
                                          ]

ServiceMgr.GeoModelSvc.AtlasVersion='ATLAS-GEO-10-00-00'

include( "TileConditions/TileConditions_jobOptions.py" )

include( "CaloConditions/LArTTCellMap_ATLAS_jobOptions.py")
include( "CaloConditions/CaloTTIdMap_ATLAS_jobOptions.py")

include( "LArConditionsCommon/LArConditionsCommon_MC_jobOptions.py" )
include( "LArConditionsCommon/LArIdMap_MC_jobOptions.py" )
from LArConditionsCommon import LArAlignable
ServiceMgr.DetDescrCnvSvc.DecodeIdDict = True

#All from AllDet_detDescr.py


import AthenaPoolCnvSvc.ReadAthenaPool  #Maybe better to break up to get rid of MetaData stuff

svcMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput()

from GaudiAlg.GaudiAlgConf import EventCounter
topSequence+=EventCounter(Frequency=2)


#include ("CaloRec/CaloRec_jobOptions.py")

from LArROD.LArRODFlags import larRODFlags
larRODFlags.readDigits=False

from CaloRec.CaloRecFlags import jobproperties
jobproperties.CaloRecFlags.clusterCellGetterName = 'CaloRec.CaloCellGetter.CaloCellGetter'

from CaloRec.CaloCellGetter import CaloCellGetter
CaloCellGetter()

from CaloRec.CaloClusterSWCmbGetter import CaloClusterSWCmbGetter
CaloClusterSWCmbGetter()

include( "CaloRec/CaloTopoCluster_jobOptions.py" )

if useJets:
   include( "CaloHiveEx/CaloHiveJets.py" )


import AthenaPoolCnvSvc.WriteAthenaPool
logRecoOutputItemList_jobOptions = logging.getLogger( 'py:RecoOutputItemList_jobOptions' )
from OutputStreamAthenaPool.CreateOutputStreams import  createOutputStream

StreamESD=createOutputStream("StreamESD","myESD.pool.root",True)
include ("CaloRecEx/CaloRecOutputItemList_jobOptions.py")
StreamESD.ItemList+=CaloESDList

print StreamESD.ItemList

#topSequence.CaloTopoCluster.TopoMaker.OutputLevel=DEBUG

#topSequence.remove(topSequence.LArRawChannelBuilder)

# make sure we own the cells
topSequence.CaloCellMaker.OwnPolicy=0


#
### Load the Algo Dependencies into the ForwardScheduler
#

# first reset AlgoDep list in ForwardScheduler
svcMgr.ForwardSchedulerSvc.AlgosDependencies = []

print "\n============== Adding following Algorithms and DataDeps to the Scheduler ================"
## dumpSequence(topSequence)
for alg in topSequence:
   name = alg.name()

   if (name not in algDep):
      print "ERROR: Algorithm ",name," not listed in Alg Dependencies"
      exit(1)
   else:
      print " ", name, "::", algDep[name]

   svcMgr.ForwardSchedulerSvc.AlgosDependencies += [ algDep[name] ]

if ('AthOutSeq' not in algDep):
   print "AthOutSeq not in Alg Dependencies"
   exit(1)
else:
   print "  AthOutSeq ::", algDep["AthOutSeq"]
   svcMgr.ForwardSchedulerSvc.AlgosDependencies += [ algDep["AthOutSeq"] ]

if ('AthRegSeq' not in algDep):
   print "AthRegSeq not in Alg Dependencies"
   exit(1)
else:
   print "  AthRegSeq ::", algDep["AthRegSeq"]
   svcMgr.ForwardSchedulerSvc.AlgosDependencies += [ algDep["AthRegSeq"] ]


print "==========================================================================================\n"

#
## set which Algorithms can be cloned
#
# names of algs are:
#  SGInputLoader
#  xAODMaker::EventInfoCnvAlg
#  xAODMaker::TriggerTowerCnvAlg
#  EventCounter
#  CaloCellMaker
#  CmbTowerBldr
#  CaloClusterMakerSWCmb
#  CaloTopoCluster
#  StreamESD

#  set algCardinality = 1 to disable cloning for all Algs
algCardinality = nThreads

if (algCardinality > 1):   
   for alg in topSequence:      
      name = alg.name()
      if (             
         name == "SGInputLoader"
         or name == "CaloCellMaker" 
         or name == "StreamESD"
         ) :
         # Don't clone these algs
         alg.Cardinality = 1
         alg.IsClonable = False
         print " -> suppressing cloning for ", name
      else:
         alg.Cardinality = algCardinality
         alg.IsClonable = True
           

