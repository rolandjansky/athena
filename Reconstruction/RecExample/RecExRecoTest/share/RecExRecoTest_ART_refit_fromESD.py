#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

# We're reading PRDs etc from ESD so tell the configuration this.
DetFlags.readRIOPool.all_setOn()

if hasattr(DetFlags,'BField_on'): DetFlags.detdescr.BField_setOn()
from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc
AtlasTrackingGeometrySvc  = svcMgr.AtlasTrackingGeometrySvc

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

from LArGeoAlgsNV.LArGeoAlgsNVConf import LArDetectorToolNV
from TileGeoModel.TileGeoModelConf import TileDetectorTool

ServiceMgr.DetDescrCnvSvc.DecodeIdDict = True

import AthenaPoolCnvSvc.ReadAthenaPool  #Maybe better to break up to get rid of MetaData stuff

svcMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput()

from GaudiAlg.GaudiAlgConf import EventCounter
topSequence+=EventCounter(Frequency=2)

from InDetRecExample.InDetKeys import InDetKeys

from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.init()

InDetFlags.trackFitterType = 'GlobalChi2Fitter'

InDetFlags.printInfo()
InDetFlags.print_JobProperties() 

from InDetRecExample.ConfiguredVertexingCuts import ConfiguredVertexingCuts
InDetPrimaryVertexingCuts      = ConfiguredVertexingCuts(InDetFlags.primaryVertexCutSetup())
InDetPrimaryVertexingCuts.printInfo()

from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("Offline")
InDetNewTrackingCuts.printInfo()
include ("InDetRecExample/InDetRecConditionsAccess.py")
include ("InDetRecExample/InDetRecLoadTools.py")

if not hasattr( topSequence, "LumiBlockMuWriter" ):
    include ("LumiBlockComps/LumiBlockMuWriter_jobOptions.py")

# ToolSvc.InDetBroadInDetRotCreator.OutputLevel=VERBOSE

from InDetRecExample.TrackingCommon import getInDetPRDtoTrackMapToolGangedPixels
from TrkRefitAlg.TrkRefitAlgConf import Trk__ReFitTrack
#MMyReFitTrack = Trk__ReFitTrack (name           = "MyRefitTrack",
#                                FitterTool     = InDetTrackFitter )

MyReFitTrack = Trk__ReFitTrack (name           = "MyReFitTrack",
                                         FitterTool     = InDetTrackFitter,
                                         FitterToolTRT  = InDetTrackFitterTRT,
                                         SummaryTool    = InDetTrackSummaryToolSharedHits,
                                         AssociationTool= getInDetPRDtoTrackMapToolGangedPixels(),
                                         NewTrackName   = InDetKeys.RefittedTracks(),
                                         useParticleHypothesisFromTrack = True)

#MyReFitTrack.OutputLevel=VERBOSE
topSequence += MyReFitTrack


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

ServiceMgr.MessageSvc.enableSuppression = False
ServiceMgr.MessageSvc.Format = "% F%40W%S%7W%R%T %0W%M"

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


