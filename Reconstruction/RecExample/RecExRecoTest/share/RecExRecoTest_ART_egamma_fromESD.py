#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentAccumulator import CAtoGlobalWrapper
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.OldFlags2NewFlags import getNewConfigFlags
from egammaConfig.egammaReconstructionConfig import (
    egammaReconstructionCfg)
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


#---------------------------------------------------------------------------------#
# MT-specific code
# Get number of processes and threads
from AthenaCommon.ConcurrencyFlags import jobproperties as jp
nThreads = jp.ConcurrencyFlags.NumThreads()
nProc = jp.ConcurrencyFlags.NumProcs()

if nThreads >=1 :
   from AthenaCommon.AlgScheduler import AlgScheduler
   AlgScheduler.OutputLevel( INFO )
   AlgScheduler.ShowControlFlow( True )
   AlgScheduler.ShowDataDependencies( True )
   AlgScheduler.setDataLoaderAlg( 'SGInputLoader' )

   # Support for the MT-MP hybrid mode
   if (nProc > 0) :

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

   ## force loading of data. make sure this alg is at the front of the
   ## AlgSequence
   #
   if not hasattr(topSequence, "SGInputLoader"):
      from SGComps.SGCompsConf import SGInputLoader
      topSequence+=SGInputLoader(OutputLevel=DEBUG, ShowEventDump=False)

# MT-specific code
#---------------------------------------------------------------------------------#

theApp.EvtMax = 20

#---------------------------------------------------------------------------------#
# NEW Conditions access infrastructure
#
from IOVSvc.IOVSvcConf import CondInputLoader
topSequence += CondInputLoader( "CondInputLoader", OutputLevel=DEBUG,  )

import StoreGate.StoreGateConf as StoreGateConf
svcMgr += StoreGateConf.StoreGateSvc("ConditionStore")

from IOVSvc.IOVSvcConf import CondSvc
svcMgr += CondSvc()
# NEW Conditions access infrastructure
#---------------------------------------------------------------------------------#

# Make sure PerfMon is off
include( "PerfMonGPerfTools/DisablePerfMon_jobOFragment.py" )

# Input file
dataFile = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecExRecoTest/mc20e_13TeV/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.ESD.e4993_s3227_r12689/myESD.pool.root"

from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
athenaCommonFlags.FilesInput=[dataFile,dataFile]

# AutoConfiguration
from RecExConfig.RecFlags import rec
rec.AutoConfiguration = ['everything']
import RecExConfig.AutoConfiguration as auto
auto.ConfigureFromListOfKeys(rec.AutoConfiguration())

from RecExConfig.ObjKeyStore import objKeyStore, CfgKeyStore

#If old style ESD with old EventInfo, we will convert it
if (objKeyStore.isInInput("EventInfo") and not objKeyStore.isInInput( "xAOD::EventInfo")):
   from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
   topSequence+=xAODMaker__EventInfoCnvAlg()

import EventInfoMgt.EventInfoMgtInit

from PyUtils.MetaReaderPeeker import convert_itemList
objKeyStore.addManyTypesInputFile(convert_itemList(layout='#join'))

#---------------------------------------------------------------------------------#
# Detector Description

import MagFieldServices.SetupField

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo = 'atlas'
from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.all_setOff()
DetFlags.detdescr.Muon_setOn()
DetFlags.detdescr.ID_setOn()
DetFlags.detdescr.Calo_setOn()
if hasattr(DetFlags,'BField_on'): DetFlags.detdescr.BField_setOn()

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

from LArGeoAlgsNV.LArGeoAlgsNVConf import LArDetectorToolNV
from TileGeoModel.TileGeoModelConf import TileDetectorTool

ServiceMgr.GeoModelSvc.DetectorTools += [ LArDetectorToolNV(ApplyAlignments = True, GeometryConfig = "RECO"),
                                          TileDetectorTool(GeometryConfig = "RECO")
                                          ]


include( "TileConditions/TileConditions_jobOptions.py" )

include( "CaloConditions/LArTTCellMap_ATLAS_jobOptions.py")
include( "CaloConditions/CaloTTIdMap_ATLAS_jobOptions.py")

include( "LArConditionsCommon/LArConditionsCommon_MC_jobOptions.py" )
include( "LArConditionsCommon/LArIdMap_MC_jobOptions.py" )
from LArConditionsCommon import LArAlignable
ServiceMgr.DetDescrCnvSvc.DecodeIdDict = True

include ("LArRecUtils/LArAffectedRegion.py")
include("BeamSpotConditions/BeamCondAlgSetup.py")
include("InDetRecExample/InDetRecConditionsAccess.py")
# Detector Description
#---------------------------------------------------------------------------------#

import AthenaPoolCnvSvc.ReadAthenaPool  #Maybe better to break up to get rid of MetaData stuff

svcMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput()

from GaudiAlg.GaudiAlgConf import EventCounter
topSequence+=EventCounter(Frequency=2)

from LArROD.LArRODFlags import larRODFlags
larRODFlags.readDigits=False

from CaloRec.CaloRecFlags import jobproperties
jobproperties.CaloRecFlags.clusterCellGetterName = 'CaloRec.CaloCellGetter.CaloCellGetter'

from CaloRec.CaloCellFlags import jobproperties
jobproperties.CaloCellFlags.doLArDeadOTXCorr=False

include( "CaloRec/CaloTopoCluster_jobOptions.py" )

#egamma new config
ConfigFlags = getNewConfigFlags()
ConfigFlags.Detector.GeometryMuon = False
ConfigFlags.Detector.GeometryID = False
ConfigFlags.lock()

from egammaAlgs.egammaTopoClusterCopierConfig import egammaTopoClusterCopierCfg
CAtoGlobalWrapper(egammaTopoClusterCopierCfg,ConfigFlags)

include("McParticleAlgs/TruthParticleBuilder_jobOptions.py")

# Add egamma
CAtoGlobalWrapper(egammaReconstructionCfg, ConfigFlags)

import AthenaPoolCnvSvc.WriteAthenaPool
logRecoOutputItemList_jobOptions = logging.getLogger( 'py:RecoOutputItemList_jobOptions' )
from OutputStreamAthenaPool.CreateOutputStreams import  createOutputStream

#Second True disables EventInfoTagBuilder
StreamESD=createOutputStream("StreamESD","myESD.pool.root",True,True)
include ("CaloRec/CaloRecOutputItemList_jobOptions.py")
StreamESD.ItemList+=CaloESDList
include ("egammaRec/egammaOutputItemList_jobOptions.py")
StreamESD.ItemList+=egammaESDList
StreamESD.ItemList+=["xAOD::EventInfo#*","xAOD::EventAuxInfo#*"]

printfunc (StreamESD.ItemList)

include ( "RecExRecoTest/RecExRecoTest_EgammaHiveRenames.py" )

printfunc ("==========================================================================================\n")

#
## set which Algorithms can be cloned
#
# names of algs are:
#  SGInputLoader
#  xAODMaker::EventInfoCnvAlg
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
      if name in ["CaloCellMaker","StreamESD"] :
         # suppress INFO message about Alg unclonability
         alg.Cardinality = 1
      else:
         alg.Cardinality = algCardinality
           
# MT-specific code
#---------------------------------------------------------------------------------#


