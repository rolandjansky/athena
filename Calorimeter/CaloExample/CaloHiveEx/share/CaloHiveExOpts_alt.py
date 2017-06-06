#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#


#
# A new configuration based on RecExCommon via AthenaPython/iread_file.py. 
# Seems to work for the original file, but data and the new file still need
# to be sorted out. This is still a work in progress.
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

from AthenaCommon.AlgScheduler import AlgScheduler
AlgScheduler.OutputLevel( INFO )
AlgScheduler.ShowControlFlow( True )
AlgScheduler.ShowDataDependencies( True )
AlgScheduler.setDataLoaderAlg( 'SGInputLoader' )

from RecExConfig.RecFlags import rec
rec.doTruth.set_Value_and_Lock(False)
rec.doWritexAOD.set_Value_and_Lock(False)
rec.doInDet.set_Value_and_Lock(False)
rec.doMuon.set_Value_and_Lock(False)

from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doxAOD.set_Value_and_Lock(False)

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

# declare if you want to use jets and if so how many
#  this needs to be done before data dependencies are declared
useJets=False
nJets=25

#
## force loading of data. make sure this alg is at the front of the
## AlgSequence
#

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from SGComps.SGCompsConf import SGInputLoader
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

# The original file
dataFile = "/afs/cern.ch/atlas/offline/ReleaseData/v16/testfile/mc09_7TeV.105200.T1_McAtNlo_Jimmy.digit.RDO.e510_s765_s767_d297_tid124958_00_RDO.124958._000001_10evt.pool.root"

# A newer input file (may also require change of geometry)
#dataFile = "/afs/cern.ch/atlas/project/rig/referencefiles/MC/valid1.110401.PowhegPythia_P2012_ttbar_nonallhad.recon.RDO.e3099_s2081_r6112_10evt.pool.root"

# An input data file
#dataFile = 'root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/tct/rec_input/q431/data16_13TeV.00297447.physics_Main.daq.RAW._lb0555._SFO-1._0001.data'

#from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags

FNAME=[dataFile,dataFile,dataFile,dataFile,dataFile,dataFile,dataFile,dataFile,dataFile,dataFile,dataFile]
include( "AthenaPython/iread_file.py" )

from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.AppMgr import ToolSvc,theApp,ServiceMgr

## AutoConfiguration
#from RecExConfig.RecFlags import rec
#rec.AutoConfiguration = ['everything']
#import RecExConfig.AutoConfiguration as auto
#auto.ConfigureFromListOfKeys(rec.AutoConfiguration())

# ToolSvc.OutputLevel = INFO
# ToolSvc.numSlots = numStores
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


#from AthenaCommon.KeyStore import CfgItemList, CfgKeyStore
from RecExConfig.ObjKeyStore import objKeyStore, CfgKeyStore
from RecExConfig.InputFilePeeker import inputFileSummary
objKeyStore.addManyTypesInputFile(inputFileSummary['eventdata_itemsList'])


topSequence = AlgSequence()

#from AtlasGeoModel import SetGeometryVersion
#from AtlasGeoModel import GeoModelInit

# import DetDescrCnvSvc.DetStoreConfig
# ServiceMgr.DetDescrCnvSvc.IdDictFromRDB = True

# from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
# from DetDescrCnvSvc.DetDescrCnvSvcConf import DetDescrCnvSvc
# ServiceMgr += GeoModelSvc()
# theApp.CreateSvc += [ "GeoModelSvc"]

#from LArGeoAlgsNV.LArGeoAlgsNVConf import LArDetectorToolNV
#from TileGeoModel.TileGeoModelConf import TileDetectorTool

#ServiceMgr.GeoModelSvc.DetectorTools += [ LArDetectorToolNV(ApplyAlignments = True, GeometryConfig = "RECO"),
#                                          TileDetectorTool(GeometryConfig = "RECO")
#                                          ]

# # I think this is no longer required
#ServiceMgr.GeoModelSvc.AtlasVersion='ATLAS-GEO-10-00-00' # for the original input file
#ServiceMgr.GeoModelSvc.AtlasVersion='ATLAS-R2-2015-02-01-00' # for the new input file

#from CaloDetMgrDetDescrCnv import CaloDetMgrDDCnv

#include( "TileConditions/TileConditions_jobOptions.py" )

#include( "CaloConditions/LArTTCellMap_ATLAS_jobOptions.py")
#include( "CaloConditions/CaloTTIdMap_ATLAS_jobOptions.py")

#include( "LArConditionsCommon/LArConditionsCommon_MC_jobOptions.py" )
#include( "LArConditionsCommon/LArIdMap_MC_jobOptions.py" )
#from LArConditionsCommon import LArAlignable
#ServiceMgr.DetDescrCnvSvc.DecodeIdDict = True

#All from AllDet_detDescr.py


#import AthenaPoolCnvSvc.ReadAthenaPool  #Maybe better to break up to get rid of MetaData stuff

#svcMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput()

#from GaudiAlg.GaudiAlgConf import EventCounter
#topSequence+=EventCounter(Frequency=2)
topSequence.EventCounter.Frequency=2

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
from OutputStreamAthenaPool.OutputStreamAthenaPool import  createOutputStream

StreamESD=createOutputStream("StreamESD","myESD.pool.root",True)
include ("CaloRecEx/CaloRecOutputItemList_jobOptions.py")
StreamESD.ItemList+=CaloESDList

print StreamESD.ItemList

#topSequence.CaloTopoCluster.TopoMaker.OutputLevel=DEBUG

#topSequence.remove(topSequence.LArRawChannelBuilder)

# make sure we own the cells
topSequence.CaloCellMaker.OwnPolicy=0

del topSequence.AllExecutedEvents
del topSequence.JobInfo
del topSequence.GlobalEventTagBuilder
del topSequence.CreateLumiBlockCollectionFromFile

svcMgr.MessageSvc.Format = "% F%40W%S%4W%e%s%7W%R%T %0W%M"
svcMgr.MessageSvc.showStats=False

#----------------------------------------------------------------------------#
#
## Alg Data Dependencies
#

### These should be moved to the jobOpts of the individual packages.

include ( "CaloHiveEx/CaloHiveDeps.py" )

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
      if name in ["CaloCellMaker","StreamESD","EventCounter","xAODMaker::EventInfoCnvAlg"] :
         # suppress INFO message about Alg unclonability
         alg.Cardinality = 1
      else:
         alg.Cardinality = algCardinality
           

#print topSequence
#print CfgMgr.AthSequencer("AthRegSeq")
svcMgr.AuditorSvc.FPEAuditor.Enable=False
#print svcMgr.AuditorSvc

