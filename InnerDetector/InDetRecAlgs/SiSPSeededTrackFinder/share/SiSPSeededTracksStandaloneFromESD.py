# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

###############################################################
#
# Standalone job options file to create SiSPSeededTracks
#
#==============================================================

# Configuration flags
if "doPixel" not in dir():
    doPixel = True
if "doSCT" not in dir():
    doSCT = True
if "doBeamSpot" not in dir():
    doBeamSpot = True
if "doPrint" not in dir():
    doPrint = True
if "doDump" not in dir():
    doDump = False
if "EvtMax" not in dir():
    EvtMax = -1
if "inputESDFiles" not in dir():
    inputESDFiles = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecExRecoTest/mc16_13TeV.361022.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2W.recon.ESD.e3668_s3170_r10572_homeMade.pool.root"]

# Output track location
TracksLocation = "SiSPSeededTracks"
if doPixel and not doSCT:
    TracksLocation = "SiSPSeededPixelTracks"
if not doPixel and doSCT:
    TracksLocation = "SiSPSeededSCTTracks"

#--------------------------------------------------------------
# Standard includes
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixStandardJob

# Common fragments
import sys
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AppMgr import ServiceMgr
from InDetRecExample.InDetJobProperties import InDetFlags
from InDetRecExample.InDetKeys import InDetKeys

if not doBeamSpot:
    InDetFlags.useBeamConstraint.set_Value_and_Lock(False)
if not doPixel:
    InDetKeys.PixelClusters.set_Value_and_Lock("")
    InDetKeys.PixelSpacePoints.set_Value_and_Lock("")
    # InDetKeys.OverlapSpacePoints.set_Value_and_Lock("")
if not doSCT:
    InDetKeys.SCT_Clusters.set_Value_and_Lock("")
    InDetKeys.SCT_SpacePoints.set_Value_and_Lock("")
    # InDetKeys.OverlapSpacePoints.set_Value_and_Lock("")

#--------------------------------------------------------------
# Thread-specific setup
#--------------------------------------------------------------
from AthenaCommon.ConcurrencyFlags import jobproperties
numThreads = jobproperties.ConcurrencyFlags.NumThreads()
if numThreads > 0:
    from AthenaCommon.AlgScheduler import AlgScheduler
    AlgScheduler.CheckDependencies( True )
    AlgScheduler.ShowControlFlow( True )
    AlgScheduler.ShowDataDependencies( True )

#--------------------------------------------------------------
# use auditors
#--------------------------------------------------------------
from GaudiSvc.GaudiSvcConf import AuditorSvc
ServiceMgr += AuditorSvc()
theAuditorSvc = ServiceMgr.AuditorSvc
theAuditorSvc.Auditors  += [ "ChronoAuditor"]
theAuditorSvc.Auditors  += [ "MemStatAuditor" ]
theApp.AuditAlgorithms=True

#--------------------------------------------------------------
# Load Geometry
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion="ATLAS-R2-2016-01-00-01"
globalflags.DetGeo="atlas"
globalflags.InputFormat="pool"
globalflags.DataSource="geant4"
if doPrint:
    printfunc (globalflags)

#--------------------------------------------------------------
# Set Detector setup
#--------------------------------------------------------------
# --- switch on BField, bpipe, pixel and SCT
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOff()
if doBeamSpot:
    DetFlags.bpipe_setOn()
if doPixel:
    DetFlags.pixel_setOn()
if doSCT:
    DetFlags.SCT_setOn()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.BField_setOn()

# ---- switch parts of ID off/on as follows
DetFlags.digitize.all_setOff()
DetFlags.geometry.all_setOff()
DetFlags.overlay.all_setOff()
DetFlags.pileup.all_setOff()
DetFlags.readRIOBS.all_setOff()
DetFlags.readRIOPool.all_setOff()
DetFlags.simulate.all_setOff()
DetFlags.simulateLVL1.all_setOff()
DetFlags.writeBS.all_setOff()
DetFlags.writeRIOPool.all_setOff()

import AtlasGeoModel.SetGeometryVersion
import AtlasGeoModel.GeoModelInit
import MagFieldServices.SetupField

if doPrint:
    DetFlags.Print()

#--------------------------------------------------------------
# Load IOVDbSvc
#--------------------------------------------------------------
IOVDbSvc = Service("IOVDbSvc")
from IOVDbSvc.CondDB import conddb
IOVDbSvc.GlobalTag="OFLCOND-MC16-SDR-20"
IOVDbSvc.OutputLevel = WARNING

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = inputESDFiles

import AthenaPoolCnvSvc.ReadAthenaPool
from PoolSvc.PoolSvcConf import PoolSvc
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr += PoolSvc()
PoolSvc = svcMgr.PoolSvc
EventSelector = svcMgr.EventSelector
EventSelector.InputCollections = inputESDFiles

# Set up topSequence and condSeq
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")

# Set up EventInfo
if not hasattr(topSequence, "xAODMaker::EventInfoCnvAlg") and not hasattr(condSeq, "xAODMaker::EventInfoCnvAlg"):
    from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
    topSequence += xAODMaker__EventInfoCnvAlg()

# Set up BeamSpot
if doBeamSpot:
    if not hasattr(condSeq, "BeamSpotCondAlg"):
        conddb.addFolderSplitOnline("INDET", "/Indet/Onl/Beampos", "/Indet/Beampos", className="AthenaAttributeList")
        from BeamSpotConditions.BeamSpotConditionsConf import BeamSpotCondAlg
        condSeq += BeamSpotCondAlg("BeamSpotCondAlg")

from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags as commonGeoFlags
from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags as geoFlags

do_runI = commonGeoFlags.Run() not in ["RUN2", "RUN3"]
if do_runI:
    sys.exit("RUN1 is not supported. Bye.")

# Set up cabling
include("InDetRecExample/InDetRecCabling.py")

# Set up Pixel conditions
if doPixel:
    # Taken from InDetRecExample/share/InDetRecConditionsAccess.py
    #################
    # Module status #
    #################
    useNewDeadmapFormat = False
    useNewChargeFormat  = False

    if not useNewDeadmapFormat:
        if not (conddb.folderRequested("/PIXEL/PixMapOverlay") or conddb.folderRequested("/PIXEL/Onl/PixMapOverlay")):
            conddb.addFolderSplitOnline("PIXEL","/PIXEL/Onl/PixMapOverlay","/PIXEL/PixMapOverlay", className='CondAttrListCollection')

    if not hasattr(condSeq, "PixelConfigCondAlg"):
        from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelConfigCondAlg

        useCablingConditions = False
        IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_2016.dat"
        rodIDForSingleLink40=0
        if (globalflags.DataSource()=='geant4'):
            # ITk:
            if geoFlags.isSLHC():
                IdMappingDat = "ITk_Atlas_IdMapping.dat"
                if "BrlIncl4.0_ref" == commonGeoFlags.GeoType():
                    IdMappingDat = "ITk_Atlas_IdMapping_InclBrl4.dat"
                elif "IBrlExt4.0ref" == commonGeoFlags.GeoType():
                    IdMappingDat = "ITk_Atlas_IdMapping_IExtBrl4.dat"
                elif "BrlExt4.0_ref" == commonGeoFlags.GeoType():
                    IdMappingDat = "ITk_Atlas_IdMapping_ExtBrl4.dat"
                elif "BrlExt3.2_ref" == commonGeoFlags.GeoType():
                    IdMappingDat = "ITk_Atlas_IdMapping_ExtBrl32.dat"
            elif (geoFlags.isIBL() == False):
                IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping.dat"
            else:
                # Planar IBL
                if (geoFlags.IBLLayout() == "planar"):
                    if (geoFlags.isDBM() == True):
                        IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_inclIBL_DBM.dat"
                    else:
                        IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_inclIBL.dat"
                # Hybrid IBL plus DBM
                elif (geoFlags.IBLLayout() == "3D"):
                    IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_Run2.dat"
        
        elif (globalflags.DataSource=='data'):
            from RecExConfig.AutoConfiguration import GetRunNumber
            runNum = GetRunNumber()
            if (runNum<222222):
                useCablingConditions = False
                IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_May08.dat"
                rodIDForSingleLink40=1300000
            else:
                useCablingConditions = True
                rodIDForSingleLink40=1300000
                # Even though we are reading from COOL, set the correct fallback map.
                if (runNum >= 344494):
                    IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_344494.dat"
                elif (runNum >= 314940 and runNum < 344494):
                    IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_314940.dat"
                elif (runNum >= 289350 and runNum < 314940): # 2016
                    IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_2016.dat"
                elif (runNum >= 222222 and runNum < 289350): # 2015
                    IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_Run2.dat"
                else:
                    IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_May08.dat"

        alg = PixelConfigCondAlg(name="PixelConfigCondAlg", 
                                 UseCablingConditions=useCablingConditions,
                                 CablingMapFileName=IdMappingDat)
        if athenaCommonFlags.isOnline():
            alg.ReadDeadMapKey = ""
        condSeq += alg

    if useNewDeadmapFormat:
        if not conddb.folderRequested("/PIXEL/PixelModuleFeMask"):
            conddb.addFolder("PIXEL_OFL", "/PIXEL/PixelModuleFeMask", className="CondAttrListCollection")
        if not hasattr(condSeq, "PixelDeadMapCondAlg"):
            from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDeadMapCondAlg
            alg = PixelDeadMapCondAlg(name="PixelDeadMapCondAlg")
            if athenaCommonFlags.isOnline():
                alg.ReadKey = ""
            condSeq += alg

    if globalflags.DataSource=='data' and InDetFlags.usePixelDCS():
        if not conddb.folderRequested("/PIXEL/DCS/FSMSTATE"):
            conddb.addFolder("DCS_OFL", "/PIXEL/DCS/FSMSTATE", className="CondAttrListCollection")
        if not conddb.folderRequested("/PIXEL/DCS/FSMSTATUS"):
            conddb.addFolder("DCS_OFL", "/PIXEL/DCS/FSMSTATUS", className="CondAttrListCollection")

    if not hasattr(condSeq, "PixelDCSCondStateAlg"):
        from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDCSCondStateAlg
        condSeq += PixelDCSCondStateAlg(name="PixelDCSCondStateAlg")

    if not hasattr(condSeq, "PixelDCSCondStatusAlg"):
        from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDCSCondStatusAlg
        condSeq += PixelDCSCondStatusAlg(name="PixelDCSCondStatusAlg")

    #####################
    # Calibration Setup #
    #####################
    if not useNewChargeFormat:
        if not conddb.folderRequested("/PIXEL/PixCalib"):
            conddb.addFolderSplitOnline("PIXEL", "/PIXEL/Onl/PixCalib", "/PIXEL/PixCalib", className="CondAttrListCollection")
        if not hasattr(condSeq, 'PixelChargeCalibCondAlg'):
            from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelChargeCalibCondAlg
            condSeq += PixelChargeCalibCondAlg(name="PixelChargeCalibCondAlg", ReadKey="/PIXEL/PixCalib")
    else:
        if not conddb.folderRequested("/PIXEL/ChargeCalibration"):
            conddb.addFolder("PIXEL_OFL", "/PIXEL/ChargeCalibration", className="CondAttrListCollection")
        if not hasattr(condSeq, 'PixelChargeLUTCalibCondAlg'):
            from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelChargeLUTCalibCondAlg
            condSeq += PixelChargeLUTCalibCondAlg(name="PixelChargeLUTCalibCondAlg", ReadKey="/PIXEL/ChargeCalibration")

    #####################
    # Cabling map Setup #
    #####################
    if (conddb.dbdata=="CONDBR2" or (conddb.dbmc=="OFLP200" and geoFlags.isIBL()==True)) and not conddb.folderRequested("/PIXEL/HitDiscCnfg"):
        conddb.addFolderSplitMC("PIXEL","/PIXEL/HitDiscCnfg","/PIXEL/HitDiscCnfg", className="AthenaAttributeList")

        if not hasattr(condSeq, 'PixelHitDiscCnfgAlg'):
            from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelHitDiscCnfgAlg
            condSeq += PixelHitDiscCnfgAlg(name="PixelHitDiscCnfgAlg")

    if not conddb.folderRequested("/PIXEL/ReadoutSpeed"):
        if not (globalflags.DataSource() == 'geant4'):
            conddb.addFolder("PIXEL", "/PIXEL/ReadoutSpeed", className="AthenaAttributeList")
        else:
            conddb.addFolderSplitMC("PIXEL","/PIXEL/ReadoutSpeed","/PIXEL/ReadoutSpeed", className="AthenaAttributeList")

    if not hasattr(condSeq, 'PixelReadoutSpeedAlg'):
        from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelReadoutSpeedAlg
        condSeq += PixelReadoutSpeedAlg(name="PixelReadoutSpeedAlg")

    if (globalflags.DataSource=='data' and conddb.dbdata == 'CONDBR2'):
        if not conddb.folderRequested("/PIXEL/CablingMap"):
            conddb.addFolderSplitOnline("PIXEL", "/PIXEL/Onl/CablingMap","/PIXEL/CablingMap", className="AthenaAttributeList")

    if not hasattr(condSeq, 'PixelCablingCondAlg'):
        from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelCablingCondAlg
        condSeq += PixelCablingCondAlg(name="PixelCablingCondAlg",
                                       ReadKey='',
                                       MappingFile=IdMappingDat,
                                       RodIDForSingleLink40=rodIDForSingleLink40)

    if not athenaCommonFlags.isOnline():
        if not conddb.folderRequested('/PIXEL/PixdEdx'):
            conddb.addFolder("PIXEL_OFL", "/PIXEL/PixdEdx", className="AthenaAttributeList")

        if not conddb.folderRequested("/PIXEL/PixReco"):
            conddb.addFolder("PIXEL_OFL", "/PIXEL/PixReco", className="DetCondCFloat")

        if not conddb.folderRequested("/Indet/PixelDist"):
            conddb.addFolder("INDET", "/Indet/PixelDist", className="DetCondCFloat")

    if not hasattr(condSeq, 'PixelOfflineCalibCondAlg'):
        from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelOfflineCalibCondAlg
        condSeq += PixelOfflineCalibCondAlg(name="PixelOfflineCalibCondAlg", ReadKey="/PIXEL/PixReco")
        if athenaCommonFlags.isOnline():
          PixelOfflineCalibCondAlg.InputSource = 1
        else :
          PixelOfflineCalibCondAlg.InputSource = 2

    if not hasattr(ToolSvc, "PixelLorentzAngleTool"):
        from SiLorentzAngleTool.PixelLorentzAngleToolSetup import PixelLorentzAngleToolSetup
        pixelLorentzAngleToolSetup = PixelLorentzAngleToolSetup()

    if not hasattr(condSeq, 'PixelDistortionAlg'):
        from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDistortionAlg
        condSeq += PixelDistortionAlg(name="PixelDistortionAlg")

    if not hasattr(condSeq, 'PixeldEdxAlg'):
        from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixeldEdxAlg
        condSeq += PixeldEdxAlg(name="PixeldEdxAlg")
        if not athenaCommonFlags.isOnline():
            PixeldEdxAlg.ReadFromCOOL = True
        else:
            PixeldEdxAlg.ReadFromCOOL = False
            if (globalflags.DataSource=='data'):
                PixeldEdxAlg.CalibrationFile="dtpar_signed_234.txt"
            else:
                PixeldEdxAlg.CalibrationFile="mcpar_signed_234.txt"

    # Takne from InDetRecExample/share/InDetRecLoadTools.py
    from InDetRecExample.TrackingCommon import createAndAddCondAlg,getPixelClusterNnCondAlg,getPixelClusterNnWithTrackCondAlg
    createAndAddCondAlg( getPixelClusterNnCondAlg,         "PixelClusterNnCondAlg",          GetInputsInfo = do_runI)
    createAndAddCondAlg( getPixelClusterNnWithTrackCondAlg,"PixelClusterNnWithTrackCondAlg", GetInputsInfo = do_runI)
    if not hasattr(condSeq, "InDetSiDetElementBoundaryLinksPixelCondAlg"):
        from SiCombinatorialTrackFinderTool_xk.SiCombinatorialTrackFinderTool_xkConf import InDet__SiDetElementBoundaryLinksCondAlg_xk
        condSeq += InDet__SiDetElementBoundaryLinksCondAlg_xk(name = "InDetSiDetElementBoundaryLinksPixelCondAlg",
                                                              ReadKey = "PixelDetectorElementCollection",
                                                              WriteKey = "PixelDetElementBoundaryLinks_xk")
    if numThreads >= 2:
        condSeq.InDetSiDetElementBoundaryLinksPixelCondAlg.Cardinality = numThreads

# Set up SCT conditions
SCT_ConditionsSummaryTool = None
if doSCT:
    # Taken from InDetRecExample/share/InDetRecConditionsAccess.py
    from SCT_ConditionsTools.SCT_ConfigurationConditionsToolSetup import SCT_ConfigurationConditionsToolSetup
    sct_ConfigurationConditionsToolSetup = SCT_ConfigurationConditionsToolSetup()
    sct_ConfigurationConditionsToolSetup.setup()
    from SCT_ConditionsTools.SCT_ReadCalibDataToolSetup import SCT_ReadCalibDataToolSetup
    sct_ReadCalibDataToolSetup = SCT_ReadCalibDataToolSetup()
    sct_ReadCalibDataToolSetup.setup()
    from SCT_ConditionsTools.SCT_DCSConditionsToolSetup import SCT_DCSConditionsToolSetup
    sct_DCSConditionsToolSetup = SCT_DCSConditionsToolSetup()
    sct_DCSConditionsToolSetup.setup()
    from SCT_ConditionsTools.SCT_ConditionsSummaryToolSetup import SCT_ConditionsSummaryToolSetup
    from SCT_ConditionsTools.SCT_FlaggedConditionToolSetup import SCT_FlaggedConditionToolSetup
    sct_FlaggedConditionToolSetup = SCT_FlaggedConditionToolSetup()
    sct_FlaggedConditionToolSetup.setup()
    sct_ConditionsSummaryToolSetup = SCT_ConditionsSummaryToolSetup()
    sct_ConditionsSummaryToolSetup.setup()
    SCT_ConditionsSummaryTool = sct_ConditionsSummaryToolSetup.getTool()
    SCT_ConditionsSummaryTool.ConditionsTools=[sct_ConfigurationConditionsToolSetup.getTool().getFullName(),
                                               sct_FlaggedConditionToolSetup.getTool().getFullName(),
                                               sct_ReadCalibDataToolSetup.getTool().getFullName(),
                                               sct_DCSConditionsToolSetup.getTool().getFullName()]
    from SiLorentzAngleTool.SCTLorentzAngleToolSetup import SCTLorentzAngleToolSetup
    sctLorentzAngleToolSetup = SCTLorentzAngleToolSetup()
    # Taken from InDetRecExample/share/InDetRecPreProcessingSilicon.py
    if not hasattr(condSeq, "InDetSiElementPropertiesTableCondAlg"):
        from SiSpacePointFormation.SiSpacePointFormationConf import InDet__SiElementPropertiesTableCondAlg
        condSeq += InDet__SiElementPropertiesTableCondAlg(name = "InDetSiElementPropertiesTableCondAlg")
    # Taken from InDetRecExample/share/InDetRecLoadTools.py
    if not hasattr(condSeq, "InDetSiDetElementBoundaryLinksSCTCondAlg"):
        from SiCombinatorialTrackFinderTool_xk.SiCombinatorialTrackFinderTool_xkConf import InDet__SiDetElementBoundaryLinksCondAlg_xk
        condSeq += InDet__SiDetElementBoundaryLinksCondAlg_xk(name = "InDetSiDetElementBoundaryLinksSCTCondAlg",
                                                              ReadKey = "SCT_DetectorElementCollection",
                                                              WriteKey = "SCT_DetElementBoundaryLinks_xk")

if doPixel or doSCT:
    # This is for both Pixel and SCT.
    # Takne from InDetRecExample/share/InDetRecLoadTools.py
    from InDetRecExample.TrackingCommon import createAndAddCondAlg, getRIO_OnTrackErrorScalingCondAlg
    createAndAddCondAlg(getRIO_OnTrackErrorScalingCondAlg,"RIO_OnTrackErrorScalingCondAlg")

# Set up Pixel neutral network tools
clusterSplitProbTool = None
clusterSplitterTool = None
if doPixel:
    # Taken from InDetRecExample/share/InDetRecLoadTools.py
    from TrkNeuralNetworkUtils.TrkNeuralNetworkUtilsConf import Trk__NeuralNetworkToHistoTool
    NeuralNetworkToHistoTool=Trk__NeuralNetworkToHistoTool(name = "NeuralNetworkToHistoTool")
    ToolSvc += NeuralNetworkToHistoTool
    from SiClusterizationTool.SiClusterizationToolConf import InDet__NnClusterizationFactory
    NnClusterizationFactory = InDet__NnClusterizationFactory(name                         = "NnClusterizationFactory",
                                                             NnCollectionJSONReadKey      = "",
                                                             PixelLorentzAngleTool        = ToolSvc.PixelLorentzAngleTool,
                                                             useToT                       = InDetFlags.doNNToTCalibration(),
                                                             NnCollectionReadKey          = "PixelClusterNN",
                                                             NnCollectionWithTrackReadKey = "PixelClusterNNWithTrack")
    ToolSvc += NnClusterizationFactory

# Set up InDet__SiTrackerSpacePointFinder (alg)
# Taken from InDetRecExample/share/InDetRecPreProcessingSilicon.py
from SiSpacePointTool.SiSpacePointToolConf import InDet__SiSpacePointMakerTool
InDetSiSpacePointMakerTool = InDet__SiSpacePointMakerTool(name = "InDetSiSpacePointMakerTool")
from SiSpacePointFormation.SiSpacePointFormationConf import InDet__SiTrackerSpacePointFinder
InDetSiTrackerSpacePointFinder = InDet__SiTrackerSpacePointFinder(name                   = "InDetSiTrackerSpacePointFinder",
                                                                  SiSpacePointMakerTool  = InDetSiSpacePointMakerTool,
                                                                  PixelsClustersName     = InDetKeys.PixelClusters(),
                                                                  SCT_ClustersName       = InDetKeys.SCT_Clusters(),
                                                                  SpacePointsPixelName   = InDetKeys.PixelSpacePoints(),
                                                                  SpacePointsSCTName     = InDetKeys.SCT_SpacePoints(),
                                                                  SpacePointsOverlapName = InDetKeys.OverlapSpacePoints(),
                                                                  ProcessPixels          = DetFlags.haveRIO.pixel_on(),
                                                                  ProcessSCTs            = DetFlags.haveRIO.SCT_on(),
                                                                  ProcessOverlaps        = DetFlags.haveRIO.pixel_on() and DetFlags.haveRIO.SCT_on(),
                                                                  OverrideBeamSpot       = not doBeamSpot)
if numThreads >= 2:
    InDetSiTrackerSpacePointFinder.Cardinality = numThreads
topSequence += InDetSiTrackerSpacePointFinder

# Set up ConfiguredNewTrackingCuts
# Taken from InDetRecExample/share/InDetRec_jobOptions.py
from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
NewTrackingCuts = None
if doPixel and doSCT:
    NewTrackingCuts = ConfiguredNewTrackingCuts("Offline")
elif doPixel:
    NewTrackingCuts = ConfiguredNewTrackingCuts("Pixel")
elif doSCT:
    NewTrackingCuts = ConfiguredNewTrackingCuts("SCT")

# Set up InDet__InDetPRD_AssociationToolGangedPixels (public)
if (NewTrackingCuts.mode() == "LowPt" or
    NewTrackingCuts.mode() == "VeryLowPt" or
    NewTrackingCuts.mode() == "LargeD0" or
    NewTrackingCuts.mode() == "LowPtLargeD0" or
    NewTrackingCuts.mode() == "BeamGas" or
    NewTrackingCuts.mode() == "ForwardTracks" or
    NewTrackingCuts.mode() == "ForwardSLHCTracks" or
    NewTrackingCuts.mode() == "Disappearing" or
    NewTrackingCuts.mode() == "VeryForwardSLHCTracks" or
    NewTrackingCuts.mode() == "SLHCConversionFinding"):

    usePrdAssociationTool = True

else:
    usePrdAssociationTool = False

InDetPrdAssociationTool = None
if usePrdAssociationTool:
    # Taken from InDetRecExample/share/InDetRecLoadTools.py
    from InDetAssociationTools.InDetAssociationToolsConf import InDet__InDetPRD_AssociationToolGangedPixels
    InDetPrdAssociationTool = InDet__InDetPRD_AssociationToolGangedPixels(name                           = "InDetPrdAssociationTool",
                                                                          PixelClusterAmbiguitiesMapName = InDetKeys.GangedPixelMap(),
                                                                          addTRToutliers                 = False) ###
    ToolSvc += InDetPrdAssociationTool

# Set up InDet__SiSpacePointsSeedMaker_ATLxk (private)
# Taken from InDetRecExample/share/ConfiguredNewTrackingSiPattern.py
from SiSpacePointsSeedTool_xk.SiSpacePointsSeedTool_xkConf import InDet__SiSpacePointsSeedMaker_ATLxk as SiSpacePointsSeedMaker
InDetSiSpacePointsSeedMaker = SiSpacePointsSeedMaker(name                   = "InDetSpSeedsMaker"+NewTrackingCuts.extension(),
                                                     pTmin                  = NewTrackingCuts.minPT(),
                                                     maxdImpact             = NewTrackingCuts.maxPrimaryImpact(),
                                                     maxZ                   = NewTrackingCuts.maxZImpact(),
                                                     minZ                   = -NewTrackingCuts.maxZImpact(),
                                                     usePixel               = NewTrackingCuts.usePixel(),
                                                     SpacePointsPixelName   = InDetKeys.PixelSpacePoints(),
                                                     useSCT                 = (NewTrackingCuts.useSCT() and NewTrackingCuts.useSCTSeeding()),
                                                     SpacePointsSCTName     = InDetKeys.SCT_SpacePoints(),
                                                     useOverlapSpCollection = NewTrackingCuts.usePixel() and (NewTrackingCuts.useSCT() and NewTrackingCuts.useSCTSeeding()), ###
                                                     SpacePointsOverlapName = InDetKeys.OverlapSpacePoints(),
                                                     radMax                 = NewTrackingCuts.radMax(),
                                                     RapidityCut            = NewTrackingCuts.maxEta(),
                                                     PRDtoTrackMap          = "",
                                                     maxdImpactPPS = NewTrackingCuts.maxdImpactPPSSeeds(),
                                                     maxdImpactSSS = NewTrackingCuts.maxdImpactSSSSeeds())
if not doBeamSpot:
    InDetSiSpacePointsSeedMaker.BeamSpotKey = ""

# Set up InDet__SiZvertexMaker_xk (private)
# Taken from ConfiguredNewTrackingSiPattern.py
from SiZvertexTool_xk.SiZvertexTool_xkConf import InDet__SiZvertexMaker_xk
InDetZvertexMaker = InDet__SiZvertexMaker_xk(name          = "InDetZvertexMaker"+NewTrackingCuts.extension(),
                                             Zmax          = NewTrackingCuts.maxZImpact(),
                                             Zmin          = -NewTrackingCuts.maxZImpact(),
                                             minRatio      = 0.17,
                                             SeedMakerTool = InDetSiSpacePointsSeedMaker)

# Set up Trk__RungeKuttaPropagator (public)
# Taken from InDetRecExample/share/InDetRecLoadTools.py
from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import Trk__RungeKuttaPropagator as Propagator
InDetPatternPropagator = Propagator(name = "InDetPatternPropagator")
ToolSvc += InDetPatternPropagator

# Set up InDet__SiDetElementsRoadMaker_xk (private)
# Taken from InDetRecExample/share/ConfiguredNewTrackingSiPattern.py
if not hasattr(condSeq, "InDet__SiDetElementsRoadCondAlg_xk"):
    from SiDetElementsRoadTool_xk.SiDetElementsRoadTool_xkConf import InDet__SiDetElementsRoadCondAlg_xk
    condSeq += InDet__SiDetElementsRoadCondAlg_xk(name = "InDet__SiDetElementsRoadCondAlg_xk",
                                                  usePixel = doPixel,
                                                  useSCT = doSCT)
from SiDetElementsRoadTool_xk.SiDetElementsRoadTool_xkConf import InDet__SiDetElementsRoadMaker_xk
InDetSiDetElementsRoadMaker = InDet__SiDetElementsRoadMaker_xk(name               = "InDetSiRoadMaker"+NewTrackingCuts.extension(),
                                                               PropagatorTool     = InDetPatternPropagator,
                                                               usePixel           = NewTrackingCuts.usePixel(),
                                                               PixManagerLocation = InDetKeys.PixelManager(),
                                                               useSCT             = NewTrackingCuts.useSCT(),
                                                               RoadWidth          = NewTrackingCuts.RoadWidth())

# Set up InDetPatternUpdator (public)
# Taken from InDetRecExample/share/InDetRecLoadTools.py
from TrkMeasurementUpdator_xk.TrkMeasurementUpdator_xkConf import Trk__KalmanUpdator_xk
InDetPatternUpdator = Trk__KalmanUpdator_xk(name = "InDetPatternUpdator")
ToolSvc += InDetPatternUpdator

# Set up InDet__PixelClusterOnTrackTool (public)
PixelClusterOnTrackTool = None
if doPixel:
    # Taken from InDetRecExample/share/InDetRecLoadTools.py
    from SiClusterOnTrackTool.SiClusterOnTrackToolConf import InDet__PixelClusterOnTrackTool
    PixelClusterOnTrackTool = InDet__PixelClusterOnTrackTool("InDetPixelClusterOnTrackTool",
                                                             LorentzAngleTool   = ToolSvc.PixelLorentzAngleTool,
                                                             DisableDistortions = (InDetFlags.doFatras() or InDetFlags.doDBMstandalone()),
                                                             applyNNcorrection = ( InDetFlags.doPixelClusterSplitting() and
                                                                                   InDetFlags.pixelClusterSplittingType() == "NeuralNet" and not InDetFlags.doSLHC()),
                                                             NNIBLcorrection = ( InDetFlags.doPixelClusterSplitting() and
                                                                                 InDetFlags.pixelClusterSplittingType() == "NeuralNet" and not InDetFlags.doSLHC()),
                                                             SplitClusterAmbiguityMap = InDetKeys.SplitClusterAmbiguityMap(),
                                                             RunningTIDE_Ambi = InDetFlags.doTIDE_Ambi())
    PixelClusterOnTrackTool.NnClusterizationFactory  = NnClusterizationFactory
    ToolSvc += PixelClusterOnTrackTool

# Set up InDet__SCT_ClusterOnTrackTool (private)
SCT_ClusterOnTrackTool = None
if doSCT:
    # Taken from InDetRecExample/share/InDetRecLoadTools.py
    from SiClusterOnTrackTool.SiClusterOnTrackToolConf import InDet__SCT_ClusterOnTrackTool
    SCT_ClusterOnTrackTool = InDet__SCT_ClusterOnTrackTool("InDetSCT_ClusterOnTrackTool",
                                                           CorrectionStrategy = 0,  # do correct position bias
                                                           ErrorStrategy      = 2,  # do use phi dependent errors
                                                           LorentzAngleTool   = sctLorentzAngleToolSetup.SCTLorentzAngleTool)

# Set up InDetRotCreator (public)
# Taken from InDetRecExample/share/InDetRecLoadTools.py
from TrkRIO_OnTrackCreator.TrkRIO_OnTrackCreatorConf import Trk__RIO_OnTrackCreator
InDetRotCreator = Trk__RIO_OnTrackCreator(name             = "InDetRotCreator",
                                          ToolPixelCluster = PixelClusterOnTrackTool,
                                          ToolSCT_Cluster  = SCT_ClusterOnTrackTool,
                                          ToolTRT_DriftCircle = None,
                                          Mode             = "indet")
ToolSvc += InDetRotCreator

# Set up SiCombinatorialTrackFinder_xk (private)
# Taken from InDetRecExample/share/InDetRecLoadTools.py
from SiCombinatorialTrackFinderTool_xk.SiCombinatorialTrackFinderTool_xkConf import InDet__SiCombinatorialTrackFinder_xk
InDetSiComTrackFinder = InDet__SiCombinatorialTrackFinder_xk(name                  = "InDetSiComTrackFinder",
                                                             PropagatorTool        = InDetPatternPropagator,
                                                             UpdatorTool           = InDetPatternUpdator,
                                                             RIOonTrackTool        = InDetRotCreator,
                                                             SctSummaryTool        = SCT_ConditionsSummaryTool,
                                                             usePixel              = DetFlags.haveRIO.pixel_on(),
                                                             useSCT                = DetFlags.haveRIO.SCT_on(),
                                                             PixelClusterContainer = InDetKeys.PixelClusters(),
                                                             SCT_ClusterContainer  = InDetKeys.SCT_Clusters())

# Set up SiTrackMaker_xk (private)
# Taken from InDetRecExample/share/ConfiguredNewTrackingSiPattern.py
# useBremMode = NewTrackingCuts.mode() == "Offline" or NewTrackingCuts.mode() == "SLHC"
useBremMode = False ###
InDetFlags.doCaloSeededBrem.set_Value_and_Lock(False) ###
InDetFlags.doHadCaloSeededSSS.set_Value_and_Lock(False) ###
InDetKeys.CaloClusterROIContainer.set_Value_and_Lock("") ###
InDetKeys.HadCaloClusterROIContainer.set_Value_and_Lock("") ###
from SiTrackMakerTool_xk.SiTrackMakerTool_xkConf import InDet__SiTrackMaker_xk as SiTrackMaker
InDetSiTrackMaker = SiTrackMaker(name                      = "InDetSiTrackMaker"+NewTrackingCuts.extension(),
                                 useSCT                    = NewTrackingCuts.useSCT(),
                                 usePixel                  = NewTrackingCuts.usePixel(),
                                 RoadTool                  = InDetSiDetElementsRoadMaker,
                                 CombinatorialTrackFinder  = InDetSiComTrackFinder,
                                 pTmin                     = NewTrackingCuts.minPT(),
                                 pTminBrem                 = NewTrackingCuts.minPTBrem(),
                                 pTminSSS                  = InDetFlags.pT_SSScut(),
                                 nClustersMin              = NewTrackingCuts.minClusters(),
                                 nHolesMax                 = NewTrackingCuts.nHolesMax(),
                                 nHolesGapMax              = NewTrackingCuts.nHolesGapMax(),
                                 SeedsFilterLevel          = NewTrackingCuts.seedFilterLevel(),
                                 Xi2max                    = NewTrackingCuts.Xi2max(),
                                 Xi2maxNoAdd               = NewTrackingCuts.Xi2maxNoAdd(),
                                 nWeightedClustersMin      = NewTrackingCuts.nWeightedClustersMin(),
                                 CosmicTrack               = InDetFlags.doCosmics(),
                                 Xi2maxMultiTracks         = NewTrackingCuts.Xi2max(), # was 3.
                                 useSSSseedsFilter         = InDetFlags.doSSSfilter(),
                                 doMultiTracksProd         = True,
                                 useBremModel              = InDetFlags.doBremRecovery() and useBremMode, # only for NewTracking the brem is debugged !!!
                                 doCaloSeededBrem          = InDetFlags.doCaloSeededBrem(),
                                 doHadCaloSeedSSS          = InDetFlags.doHadCaloSeededSSS(),
                                 phiWidth                  = NewTrackingCuts.phiWidthBrem(),
                                 etaWidth                  = NewTrackingCuts.etaWidthBrem(),
                                 InputClusterContainerName = InDetKeys.CaloClusterROIContainer(), # "InDetCaloClusterROIs"
                                 InputHadClusterContainerName = InDetKeys.HadCaloClusterROIContainer(), # "InDetCaloClusterROIs"
                                 UseAssociationTool        = usePrdAssociationTool)
InDetSiTrackMaker.TrackPatternRecoInfo = "SiSPSeededFinder"
if not doBeamSpot:
    InDetSiTrackMaker.BeamSpotKey = ""

# Set up SiSPSeededTrackFinder (alg)
# InDetRecExample/share/ConfiguredNewTrackingSiPattern.py
from InDetRecExample import TrackingCommon as TrackingCommon
from SiSPSeededTrackFinder.SiSPSeededTrackFinderConf import InDet__SiSPSeededTrackFinder
InDetSiSPSeededTrackFinder = InDet__SiSPSeededTrackFinder(name           = "InDetSiSpTrackFinder"+NewTrackingCuts.extension(),
                                                          TrackTool      = InDetSiTrackMaker,
                                                          TracksLocation = TracksLocation,
                                                          SeedsTool      = InDetSiSpacePointsSeedMaker,
                                                          useZvertexTool = InDetFlags.useZvertexTool() and NewTrackingCuts.mode() != "DBM",
                                                          ZvertexTool    = InDetZvertexMaker,
                                                          TrackSummaryTool = TrackingCommon.getInDetTrackSummaryToolNoHoleSearch(),                                                                                                                                                                     useNewStrategy = InDetFlags.useNewSiSPSeededTF() and NewTrackingCuts.mode() != "DBM",
                                                          useMBTSTimeDiff = InDetFlags.useMBTSTimeDiff(),
                                                          useZBoundFinding = NewTrackingCuts.doZBoundary() and NewTrackingCuts.mode() != "DBM")
if not doBeamSpot:
    InDetSiSPSeededTrackFinder.BeamSpotKey = ""
if not doPixel:
    InDetSiSPSeededTrackFinder.SpacePointsPixelName = ""
if not doSCT:
    InDetSiSPSeededTrackFinder.SpacePointsSCTName = ""

if doPrint:
    printfunc (InDetSiSPSeededTrackFinder)
if numThreads >= 2:
    InDetSiSPSeededTrackFinder.Cardinality = numThreads
topSequence += InDetSiSPSeededTrackFinder

# Print algorithms
if doPrint:
    printfunc (topSequence)

# Set the number of events to be processed
theApp.EvtMax = EvtMax

# Output file
if doDump:
    from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
    outStream = AthenaPoolOutputStream("OutStream", "SiSPSeededTracksStandaloneFromESD.pool.root")
    outStream.ItemList  = ["xAOD::EventInfo#EventInfo", "xAOD::EventAuxInfo#EventInfoAux."]
    outStream.ItemList += ["TrackCollection#"+TracksLocation]

#--------------------------------------------------------------
# Set output lvl (VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M"

if numThreads >= 2:
    from SCT_ConditionsAlgorithms.SCTCondAlgCardinality import sctCondAlgCardinality
    sctCondAlgCardinality.set(numThreads)
