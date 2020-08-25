# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

###############################################################
#
# Standalone job options file to create SiSPSeededTracks
#
#==============================================================

# Configuration flags
if not "doPixel" in dir():
    doPixel = False
if not "doSCT" in dir():
    doSCT = True
if not "doBeamSpot" in dir():
    doBeamSpot = True
if not "doPrint" in dir():
    doPrint = True
if not "EvtMax" in dir():
    EvtMax = 10
if not "inputBSFiles" in dir():
    # Set input byte stream file (from q431 test)
    inputBSFiles = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data17_13TeV.00330470.physics_Main.daq.RAW._lb0310._SFO-1._0001.data"]

# Combinations of
# doPixel = True,  doSCT = True,  doBeamSpot = True,  doPrint = True
# doPixel = True,  doSCT = False, doBeamSpot = False, doPrint = True
# doPixel = False, doSCT = True,  doBeamSpot = False, doPrint = True
# doPixel = False, doSCT = True,  doBeamSpot = True,  doPrint = True
# were tested.

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
globalflags.InputFormat="bytestream"
globalflags.DataSource="data"
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
conddb.dbdata="CONDBR2"
IOVDbSvc.GlobalTag="CONDBR2-BLKPA-2018-03"
IOVDbSvc.OutputLevel = WARNING

include("ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py")
ServiceMgr.EventSelector.Input = inputBSFiles
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = inputBSFiles

# Set up topSequence and condSeq
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")

# Set up EventInfo
if not hasattr(topSequence, "xAODMaker::EventInfoCnvAlg") and not hasattr(condSeq, "xAODMaker::EventInfoCnvAlg"):
    from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
    topSequence += xAODMaker__EventInfoCnvAlg()

# Set up byte stream converters
# Pixel: PixelRawDataProvider, PixelRawDataProviderTool, PixelRodDecoder
# SCT: SCTRawDataProvider (reentrant), SCTRawDataProviderTool, SCT_RodDecoder, SCTEventFlagWriter (reentrant)
include("InDetRecExample/InDetReadBS_jobOptions.py")
if doPixel:
    topSequence.InDetPixelRawDataProvider.OutputLevel = INFO
    topSequence.InDetPixelRawDataProvider.ProviderTool.Decoder.OutputLevel = INFO
if doSCT:
    topSequence.InDetSCTRawDataProvider.OutputLevel = INFO
    topSequence.InDetSCTRawDataProvider.ProviderTool.Decoder.OutputLevel = INFO
    topSequence.InDetSCTEventFlagWriter.OutputLevel = INFO
    if numThreads >= 2:
        topSequence.InDetSCTRawDataProvider.Cardinality = numThreads
        topSequence.InDetSCTEventFlagWriter.Cardinality = numThreads

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

        condSeq += PixelConfigCondAlg(name="PixelConfigCondAlg", 
                                      UseDeadmapConditions=(not athenaCommonFlags.isOnline()),
                                      UseDCSStateConditions=(globalflags.DataSource=='data') and InDetFlags.usePixelDCS(),
                                      UseDCSStatusConditions=(globalflags.DataSource=='data') and InDetFlags.usePixelDCS(),
                                      UseTDAQConditions=athenaCommonFlags.isOnline(),
                                      ReadDeadMapKey="/PIXEL/PixMapOverlay",
                                      UseCalibConditions=True,
                                      UseCablingConditions=useCablingConditions,
                                      CablingMapFileName=IdMappingDat)

    if useNewDeadmapFormat:
        if not conddb.folderRequested("/PIXEL/PixelModuleFeMask"):
            conddb.addFolder("PIXEL_OFL", "/PIXEL/PixelModuleFeMask", className="CondAttrListCollection")
        if not hasattr(condSeq, "PixelDeadMapCondAlg"):
            from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDeadMapCondAlg
            condSeq += PixelDeadMapCondAlg(name="PixelDeadMapCondAlg")

    if not athenaCommonFlags.isOnline():
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
    from SCT_ConditionsTools.SCT_ByteStreamErrorsToolSetup import SCT_ByteStreamErrorsToolSetup
    sct_ByteStreamErrorsToolSetup = SCT_ByteStreamErrorsToolSetup()
    sct_ByteStreamErrorsToolSetup.setConfigTool(sct_ConfigurationConditionsToolSetup.getTool())
    sct_ByteStreamErrorsToolSetup.setup()
    from SCT_ConditionsTools.SCT_ReadCalibDataToolSetup import SCT_ReadCalibDataToolSetup
    sct_ReadCalibDataToolSetup = SCT_ReadCalibDataToolSetup()
    sct_ReadCalibDataToolSetup.setup()
    from SCT_ConditionsTools.SCT_TdaqEnabledToolSetup import SCT_TdaqEnabledToolSetup
    sct_TdaqEnabledToolSetup = SCT_TdaqEnabledToolSetup()
    sct_TdaqEnabledToolSetup.setFolder("/TDAQ/Resources/ATLAS/SCT/Robins")
    sct_TdaqEnabledToolSetup.setup()
    from SCT_ConditionsTools.SCT_DCSConditionsToolSetup import SCT_DCSConditionsToolSetup
    sct_DCSConditionsToolSetup = SCT_DCSConditionsToolSetup()
    sct_DCSConditionsToolSetup.setup()
    from SCT_ConditionsTools.SCT_ConditionsSummaryToolSetup import SCT_ConditionsSummaryToolSetup
    sct_ConditionsSummaryToolSetupWithoutFlagged = SCT_ConditionsSummaryToolSetup("InDetSCT_ConditionsSummaryToolWithoutFlagged")
    sct_ConditionsSummaryToolSetupWithoutFlagged.setup()
    sct_ConditionsSummaryToolSetupWithoutFlagged.ConditionsTools=[sct_ConfigurationConditionsToolSetup.getTool().getFullName(),
                                                                  sct_ByteStreamErrorsToolSetup.getTool().getFullName(),
                                                                  sct_ReadCalibDataToolSetup.getTool().getFullName(),
                                                                  sct_TdaqEnabledToolSetup.getTool().getFullName(),
                                                                  sct_DCSConditionsToolSetup.getTool().getFullName()]
    from SCT_ConditionsTools.SCT_FlaggedConditionToolSetup import SCT_FlaggedConditionToolSetup
    sct_FlaggedConditionToolSetup = SCT_FlaggedConditionToolSetup()
    sct_FlaggedConditionToolSetup.setup()
    sct_ConditionsSummaryToolSetup = SCT_ConditionsSummaryToolSetup()
    sct_ConditionsSummaryToolSetup.setup()
    SCT_ConditionsSummaryTool = sct_ConditionsSummaryToolSetup.getTool()
    SCT_ConditionsSummaryTool.ConditionsTools=[sct_ConfigurationConditionsToolSetup.getTool().getFullName(),
                                               sct_FlaggedConditionToolSetup.getTool().getFullName(),
                                               sct_ByteStreamErrorsToolSetup.getTool().getFullName(),
                                               sct_ReadCalibDataToolSetup.getTool().getFullName(),
                                               sct_TdaqEnabledToolSetup.getTool().getFullName(),
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

# Taken from InDetRecExample/share/InDetRecPreProcessingSilicon.py
from SiClusterizationTool.SiClusterizationToolConf import InDet__ClusterMakerTool
InDetClusterMakerTool = InDet__ClusterMakerTool(name = "InDetClusterMakerTool",
                                                SCTLorentzAngleTool = sctLorentzAngleToolSetup.SCTLorentzAngleTool if doSCT else None)
if doPixel:
    InDetClusterMakerTool.PixelLorentzAngleTool = ToolSvc.PixelLorentzAngleTool
else:
    InDetClusterMakerTool.PixelCablingSvc = None
    InDetClusterMakerTool.PixelModuleData = ""
    InDetClusterMakerTool.PixelChargeCalibCondData = ""
    InDetClusterMakerTool.PixelLorentzAngleTool = None
    InDetClusterMakerTool.PixelOfflineCalibData = ""

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
                                                             PixelLorentzAngleTool        = ToolSvc.PixelLorentzAngleTool,
                                                             useToT                       = InDetFlags.doNNToTCalibration(),
                                                             NnCollectionReadKey          = "PixelClusterNN",
                                                             NnCollectionWithTrackReadKey = "PixelClusterNNWithTrack")
    ToolSvc += NnClusterizationFactory
    # Taken from InDetRecExample/share/InDetRecPreProcessingSilicon.py
    MultiplicityContent = [1 , 1 , 1]
    useBeamConstraint = InDetFlags.useBeamConstraint()
    from SiClusterizationTool.SiClusterizationToolConf import InDet__NnPixelClusterSplitProbTool as PixelClusterSplitProbTool
    NnPixelClusterSplitProbTool=PixelClusterSplitProbTool(name = "NnPixelClusterSplitProbTool",
                                                          PriorMultiplicityContent = MultiplicityContent,
                                                          NnClusterizationFactory  = NnClusterizationFactory,
                                                          useBeamSpotInfo          = useBeamConstraint)
    ToolSvc += NnPixelClusterSplitProbTool
    clusterSplitProbTool = NnPixelClusterSplitProbTool
    from SiClusterizationTool.SiClusterizationToolConf import InDet__NnPixelClusterSplitter as PixelClusterSplitter
    NnPixelClusterSplitter=PixelClusterSplitter(name                                = "NnPixelClusterSplitter",
                                                NnClusterizationFactory             = NnClusterizationFactory,
                                                ThresholdSplittingIntoTwoClusters   = 0.5,
                                                ThresholdSplittingIntoThreeClusters = 0.25,
                                                SplitOnlyOnBLayer                   = False,
                                                useBeamSpotInfo                     = useBeamConstraint)
    ToolSvc += NnPixelClusterSplitter
    clusterSplitterTool = NnPixelClusterSplitter
    from SiClusterizationTool.SiClusterizationToolConf import InDet__MergedPixelsTool
    InDetMergedPixelsTool = InDet__MergedPixelsTool(name                    = "InDetMergedPixelsTool", 
                                                    globalPosAlg            = InDetClusterMakerTool,
                                                    MinimalSplitSize        = 0,
                                                    MaximalSplitSize        = 49,
                                                    MinimalSplitProbability = 0,
                                                    DoIBLSplitting = True,
                                                    SplitClusterAmbiguityMap= "SplitClusterAmbiguityMap")
    if not InDetFlags.doTIDE_Ambi() and clusterSplitProbTool is not None : InDetMergedPixelsTool.SplitProbTool   = clusterSplitProbTool
    if not InDetFlags.doTIDE_Ambi() and clusterSplitterTool is not None  : InDetMergedPixelsTool.ClusterSplitter = clusterSplitterTool
    # Enable duplcated RDO check for data15 because duplication mechanism was used.
    from RecExConfig.RecFlags import rec
    if len(rec.projectName())>=6 and rec.projectName()[:6]=="data15":
        InDetMergedPixelsTool.CheckDuplicatedRDO = True

    ToolSvc += InDetMergedPixelsTool
    from SiClusterizationTool.SiClusterizationToolConf import InDet__PixelGangedAmbiguitiesFinder
    InDetPixelGangedAmbiguitiesFinder = InDet__PixelGangedAmbiguitiesFinder(name = "InDetPixelGangedAmbiguitiesFinder")
    ToolSvc += InDetPixelGangedAmbiguitiesFinder
    from InDetPrepRawDataFormation.InDetPrepRawDataFormationConf import InDet__PixelClusterization
    InDetPixelClusterization = InDet__PixelClusterization(name                    = "InDetPixelClusterization",
                                                          clusteringTool          = InDetMergedPixelsTool,
                                                          gangedAmbiguitiesFinder = InDetPixelGangedAmbiguitiesFinder,
                                                          DataObjectName          = InDetKeys.PixelRDOs(),
                                                          ClustersName            = InDetKeys.PixelClusters())
    topSequence += InDetPixelClusterization

# For SCT
if doSCT:
    # Taken from InDetRecExample/share/InDetRecPreProcessingSilicon.py
    from SiClusterizationTool.SiClusterizationToolConf import InDet__SCT_ClusteringTool
    InDetSCT_ClusteringTool = InDet__SCT_ClusteringTool(name = "InDetSCT_ClusteringTool",
                                                        OutputLevel = INFO,
                                                        globalPosAlg = InDetClusterMakerTool,
                                                        conditionsTool = sct_ConditionsSummaryToolSetupWithoutFlagged.getTool())
    if InDetFlags.selectSCTIntimeHits():
        if InDetFlags.InDet25nsec(): 
            InDetSCT_ClusteringTool.timeBins = "01X" 
        else: 
            InDetSCT_ClusteringTool.timeBins = "X1X"
    from InDetPrepRawDataFormation.InDetPrepRawDataFormationConf import InDet__SCT_Clusterization
    InDetSCT_Clusterization = InDet__SCT_Clusterization(name = "InDetSCT_Clusterization",
                                                        clusteringTool = InDetSCT_ClusteringTool,
                                                        DataObjectName = InDetKeys.SCT_RDOs(),
                                                        ClustersName = InDetKeys.SCT_Clusters(),
                                                        conditionsTool = sct_ConditionsSummaryToolSetupWithoutFlagged.getTool())
    if InDetFlags.cutSCTOccupancy():
        InDetSCT_Clusterization.maxFiredStrips = 384
    else:
        InDetSCT_Clusterization.maxFiredStrips = 0
    if numThreads >= 2:
        InDetSCT_Clusterization.Cardinality = numThreads
    topSequence += InDetSCT_Clusterization

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
                                                     PRDtoTrackMap          = "", # @TODO
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
                                                          TrackSummaryTool = TrackingCommon.getInDetTrackSummaryToolNoHoleSearch(), 
                                                          useNewStrategy = InDetFlags.useNewSiSPSeededTF() and NewTrackingCuts.mode() != "DBM",
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
    print (topSequence)

# Set the number of events to be processed
theApp.EvtMax = EvtMax

#--------------------------------------------------------------
# Set output lvl (VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M"

if numThreads >= 2:
    from SCT_ConditionsAlgorithms.SCTCondAlgCardinality import sctCondAlgCardinality
    sctCondAlgCardinality.set(numThreads)
