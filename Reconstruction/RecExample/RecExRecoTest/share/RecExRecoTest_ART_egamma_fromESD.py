#
#  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#

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

from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
topSequence+=xAODMaker__EventInfoCnvAlg()

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
dataFile = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecExRecoTest/mc16_13TeV.361022.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2W.recon.ESD.e3668_s3170_r10572_homeMade.pool.root"

from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
athenaCommonFlags.FilesInput=[dataFile,dataFile]

# AutoConfiguration
from RecExConfig.RecFlags import rec
rec.AutoConfiguration = ['everything']
import RecExConfig.AutoConfiguration as auto
auto.ConfigureFromListOfKeys(rec.AutoConfiguration())

from RecExConfig.ObjKeyStore import objKeyStore, CfgKeyStore
from RecExConfig.InputFilePeeker import inputFileSummary
objKeyStore.addManyTypesInputFile(inputFileSummary['eventdata_itemsList'])

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
from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc
AtlasTrackingGeometrySvc  = svcMgr.AtlasTrackingGeometrySvc

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

from LArGeoAlgsNV.LArGeoAlgsNVConf import LArDetectorToolNV
from TileGeoModel.TileGeoModelConf import TileDetectorTool

ServiceMgr.GeoModelSvc.DetectorTools += [ LArDetectorToolNV(ApplyAlignments = True, GeometryConfig = "RECO"),
                                          TileDetectorTool(GeometryConfig = "RECO")
                                          ]


from CaloDetMgrDetDescrCnv import CaloDetMgrDDCnv

include( "TileConditions/TileConditions_jobOptions.py" )

include( "CaloConditions/LArTTCellMap_ATLAS_jobOptions.py")
include( "CaloConditions/CaloTTIdMap_ATLAS_jobOptions.py")

include( "LArConditionsCommon/LArConditionsCommon_MC_jobOptions.py" )
include( "LArConditionsCommon/LArIdMap_MC_jobOptions.py" )
from LArConditionsCommon import LArAlignable
ServiceMgr.DetDescrCnvSvc.DecodeIdDict = True

include ("LArRecUtils/LArAffectedRegion.py")
include("InDetBeamSpotService/BeamCondSvc.py")
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

from egammaAlgs.egammaTopoClusterCopier import egammaTopoClusterCopier
try:
   egammaTopoClusterCopier()
except Exception:
   treatExeption("could not get handle to egammaTopoClusterCopier")

include( "McParticleAlgs/TruthParticleBuilder_jobOptions.py" )

from egammaRec.egammaRecFlags import jobproperties

include( "egammaRec/egammaRec_jobOptions.py" )

import AthenaPoolCnvSvc.WriteAthenaPool
logRecoOutputItemList_jobOptions = logging.getLogger( 'py:RecoOutputItemList_jobOptions' )
from OutputStreamAthenaPool.OutputStreamAthenaPool import  createOutputStream

StreamESD=createOutputStream("StreamESD","myESD.pool.root",True)
include ("CaloRecEx/CaloRecOutputItemList_jobOptions.py")
StreamESD.ItemList+=CaloESDList
include ("egammaRec/egammaOutputItemList_jobOptions.py")
StreamESD.ItemList+=egammaESDList

print StreamESD.ItemList

include ( "RecExRecoTest/RecExRecoTest_EgammaHiveRenames.py" )

print "==========================================================================================\n"

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


