#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.Logging import logging
from AthenaCommon.GlobalFlags import globalflags
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
log = logging.getLogger(__name__)

### Output data name ###
from TrigEDMConfig.TriggerEDMRun3 import recordable
from MuonConfig.MuonBytestreamDecodeConfig import RpcBytestreamDecodeCfg, TgcBytestreamDecodeCfg, MdtBytestreamDecodeCfg, CscBytestreamDecodeCfg, sTgcBytestreamDecodeCfg, MmBytestreamDecodeCfg
from MuonConfig.MuonRdoDecodeConfig import RpcRDODecodeCfg, TgcRDODecodeCfg, MdtRDODecodeCfg, CscRDODecodeCfg, CscClusterBuildCfg, StgcRDODecodeCfg, MMRDODecodeCfg
from RegionSelector.RegSelToolConfig import regSelTool_RPC_Cfg, regSelTool_TGC_Cfg, regSelTool_MDT_Cfg, regSelTool_CSC_Cfg, regSelTool_STGC_Cfg, regSelTool_MM_Cfg
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.AccumulatorCache import AccumulatorCache
from AthenaConfiguration.ComponentFactory import CompFactory

theFTF_name = "FTFTracks_Muons" #Obsolete?
CBTPname = recordable("HLT_CBCombinedMuon_RoITrackParticles")
CBTPnameFS = recordable("HLT_CBCombinedMuon_FSTrackParticles")
ExtrpTPname = recordable("HLT_MSExtrapolatedMuons_RoITrackParticles")
ExtrpTPnameFS = recordable("HLT_MSExtrapolatedMuons_FSTrackParticles")
MSextrpTPname = recordable("HLT_MSOnlyExtrapolatedMuons_FSTrackParticles")

from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags

from AthenaCommon.BeamFlags import jobproperties
beamFlags = jobproperties.Beam

class muonNames(object):
  def __init__(self):
    #EFSA and EFCB containers have different names 
    #for RoI and FS running. Other containers are 
    #produced in RoIs only.

    self.L2SAName = recordable("HLT_MuonL2SAInfo")
    self.L2CBName = recordable("HLT_MuonL2CBInfo")
    self.EFSAName = "Muons"
    self.EFCBName = "MuonsCB"
    self.EFCBOutInName = "MuonsCBOutsideIn"
    self.EFCBInOutName = "HLT_MuonsCBInsideOut"
    self.L2IsoMuonName = recordable("HLT_MuonL2ISInfo")
    self.EFIsoMuonName = recordable("HLT_MuonsIso")
    self.L2forIDName   = "RoIs_fromL2SAViews"

  def getNames(self, name):

    if "FS" in name:
      self.EFSAName = recordable("HLT_Muons_FS")
      self.EFCBName = recordable("HLT_MuonsCB_FS")
    if "RoI" in name:
      self.EFSAName = recordable("HLT_Muons_RoI")
      self.EFCBName = recordable("HLT_MuonsCB_RoI")
    if "LRT" in name:
      self.L2CBName = recordable("HLT_MuonL2CBInfoLRT")
      self.EFSAName = recordable("HLT_Muons_RoI")
      self.EFCBName = recordable("HLT_MuonsCB_LRT")
    return self

muNames = muonNames().getNames('RoI')
muNamesFS = muonNames().getNames('FS')
muNamesLRT = muonNames().getNames('LRT')

def isCosmic():
  #FIXME: this might not be ideal criteria to determine if this is cosmic chain but used to work in Run2 and will do for now, ATR-22758
  return (beamFlags.beamType() == 'cosmics')

def isLRT(name):
  return "LRT" in name

#Returns relevant track collection name
def getIDTracks(name=''):
  if isCosmic():
      if isLRT(name): return recordable("HLT_IDTrack_MuonLRT_FTF")
      else:           return recordable("HLT_IDTrack_Cosmic_IDTrig") #IDTrig as we are already retrieving PT collection
  else:
      ### TODO This should be probably set in the IDTrigConfig, ATR-22755
      if isLRT(name): return recordable("HLT_IDTrack_MuonLRT_FTF") 
      else:           return recordable("HLT_IDTrack_Muon_FTF") 


def MuDataPrepViewDataVerifierCfg(flags):
    result = ComponentAccumulator()
    dataobjects=[( 'RpcPrepDataCollection_Cache' , 'StoreGateSvc+RpcPrdCache' ),
                 ( 'TgcRdo_Cache' , 'StoreGateSvc+TgcRdoCache' ),
                 ( 'MdtCsm_Cache' , 'StoreGateSvc+MdtCsmRdoCache' ),
                 ( 'RpcPad_Cache' , 'StoreGateSvc+RpcRdoCache' ),
                 ( 'RpcCoinDataCollection_Cache' , 'StoreGateSvc+RpcCoinCache' )]
    if flags.Input.isMC:
      dataobjects += [( 'MdtCsmContainer' , 'StoreGateSvc+MDTCSM' ),
                      ( 'RpcPadContainer' , 'StoreGateSvc+RPCPAD' ),
                      ('TgcRdoContainer' , 'StoreGateSvc+TGCRDO' )]
    if flags.Detector.GeometryCSC:
      dataobjects+=[( 'CscRawDataCollection_Cache' , 'StoreGateSvc+CscRdoCache' )]
      if flags.Input.isMC:
        dataobjects += [( 'CscRawDataContainer' , 'StoreGateSvc+CSCRDO' ),
                        ( 'CscRawDataCollection_Cache' , 'StoreGateSvc+CscRdoCache' )]
    if flags.Detector.GeometrysTGC and flags.Detector.GeometryMM and flags.Input.isMC:
      dataobjects += [( 'Muon::STGC_RawDataContainer' , 'StoreGateSvc+sTGCRDO' ),
                      ( 'Muon::MM_RawDataContainer' , 'StoreGateSvc+MMRDO' )]
    alg = CompFactory.AthViews.ViewDataVerifier( name = "VDVMuDataPrep",
                                                 DataObjects = dataobjects)
    result.addEventAlgo(alg)
    return result

@AccumulatorCache
def muonDecodeCfg(flags, RoIs):

    acc = ComponentAccumulator()
    RegSelTool_RPC = acc.popToolsAndMerge(regSelTool_RPC_Cfg(flags))
    RegSelTool_TGC = acc.popToolsAndMerge(regSelTool_TGC_Cfg(flags))
    RegSelTool_MDT = acc.popToolsAndMerge(regSelTool_MDT_Cfg(flags))
    if flags.Detector.GeometryCSC:
        RegSelTool_CSC = acc.popToolsAndMerge(regSelTool_CSC_Cfg(flags))
    if flags.Detector.GeometrysTGC and flags.Detector.GeometryMM:
      RegSelTool_STGC = acc.popToolsAndMerge(regSelTool_STGC_Cfg(flags))
      RegSelTool_MM = acc.popToolsAndMerge(regSelTool_MM_Cfg(flags))
    doSeededDecoding =True
    if 'FSRoI' in RoIs:
      doSeededDecoding = False
    acc.merge(MuDataPrepViewDataVerifierCfg(flags))
    # Get RPC BS decoder
    if not flags.Input.isMC:
        rpcAcc = RpcBytestreamDecodeCfg( flags, name = "RpcRawDataProvider_"+RoIs )
        rpcAcc.getEventAlgo("RpcRawDataProvider_"+RoIs).RoIs = RoIs
        rpcAcc.getEventAlgo("RpcRawDataProvider_"+RoIs).DoSeededDecoding = doSeededDecoding
        rpcAcc.getEventAlgo("RpcRawDataProvider_"+RoIs).RegionSelectionTool = RegSelTool_RPC
        acc.merge( rpcAcc )
    # Get RPC RDO convertor
    rpcAcc = RpcRDODecodeCfg( flags, name= "RpcRdoToRpcPrepData_"+RoIs )
    rpcAcc.getEventAlgo("RpcRdoToRpcPrepData_"+RoIs).RoIs = RoIs
    rpcAcc.getEventAlgo("RpcRdoToRpcPrepData_"+RoIs).DoSeededDecoding = doSeededDecoding
    acc.merge( rpcAcc )
    # Get TGC BS decoder
    if not flags.Input.isMC:
        tgcAcc = TgcBytestreamDecodeCfg( flags, name="TgcRawDataProvider_"+RoIs )
        tgcAcc.getEventAlgo("TgcRawDataProvider_"+RoIs).RoIs = RoIs
        tgcAcc.getEventAlgo("TgcRawDataProvider_"+RoIs).DoSeededDecoding = doSeededDecoding
        tgcAcc.getEventAlgo("TgcRawDataProvider_"+RoIs).RegionSelectionTool = RegSelTool_TGC
        acc.merge( tgcAcc )
    # Get TGC RDO convertor
    tgcAcc = TgcRDODecodeCfg( flags, name="TgcRdoToTgcPrepData_"+RoIs )
    tgcAcc.getEventAlgo("TgcRdoToTgcPrepData_"+RoIs).RoIs = RoIs
    tgcAcc.getEventAlgo("TgcRdoToTgcPrepData_"+RoIs).DoSeededDecoding = doSeededDecoding
    acc.merge( tgcAcc )
    # Get MDT BS decoder
    if not flags.Input.isMC:
        mdtAcc = MdtBytestreamDecodeCfg( flags, name="MdtRawDataProvider_"+RoIs )
        mdtAcc.getEventAlgo("MdtRawDataProvider_"+RoIs).RoIs = RoIs
        mdtAcc.getEventAlgo("MdtRawDataProvider_"+RoIs).DoSeededDecoding = doSeededDecoding
        mdtAcc.getEventAlgo("MdtRawDataProvider_"+RoIs).RegionSelectionTool = RegSelTool_MDT
        acc.merge( mdtAcc )
    # Get MDT RDO convertor
    mdtAcc = MdtRDODecodeCfg( flags, name="MdtRdoToMdtPrepData_"+RoIs )
    mdtAcc.getEventAlgo("MdtRdoToMdtPrepData_"+RoIs).RoIs = RoIs
    mdtAcc.getEventAlgo("MdtRdoToMdtPrepData_"+RoIs).DoSeededDecoding = doSeededDecoding
    acc.merge( mdtAcc )
    # Get CSC BS decoder
    if flags.Detector.GeometryCSC:
        if not flags.Input.isMC:
            cscAcc = CscBytestreamDecodeCfg( flags, name="CscRawDataProvider_"+RoIs )
            cscAcc.getEventAlgo("CscRawDataProvider_"+RoIs).RoIs = RoIs
            cscAcc.getEventAlgo("CscRawDataProvider_"+RoIs).DoSeededDecoding = doSeededDecoding
            cscAcc.getEventAlgo("CscRawDataProvider_"+RoIs).RegionSelectionTool = RegSelTool_CSC
            acc.merge( cscAcc )
        # Get CSC RDO convertor
        cscAcc = CscRDODecodeCfg( flags, name="CscRdoToCscPrepData_"+RoIs )
        cscAcc.getEventAlgo("CscRdoToCscPrepData_"+RoIs).RoIs = RoIs
        cscAcc.getEventAlgo("CscRdoToCscPrepData_"+RoIs).DoSeededDecoding = doSeededDecoding
        acc.merge( cscAcc )
        # Get CSC cluster builder
        cscAcc = CscClusterBuildCfg( flags, name="CscThresholdClusterBuilder_"+RoIs )
        acc.merge( cscAcc )
    #sTGC and MM BS decoder
    if flags.Detector.GeometrysTGC and flags.Detector.GeometryMM:
      if not flags.Input.isMC:
        stgcAcc = sTgcBytestreamDecodeCfg(flags, name="sTgcRawDataProvider_"+RoIs)
        stgcAcc.getEventAlgo("sTgcRawDataProvider_"+RoIs).RoIs = RoIs
        stgcAcc.getEventAlgo("sTgcRawDataProvider_"+RoIs).DoSeededDecoding = doSeededDecoding
        stgcAcc.getEventAlgo("sTgcRawDataProvider_"+RoIs).RegionSelectionTool = RegSelTool_STGC
        acc.merge( stgcAcc )
        mmAcc = MmBytestreamDecodeCfg(flags, name="MMRawDataProvider_"+RoIs)
        mmAcc.getEventAlgo("MMRawDataProvider_"+RoIs).RoIs = RoIs
        mmAcc.getEventAlgo("MMRawDataProvider_"+RoIs).DoSeededDecoding = doSeededDecoding
        mmAcc.getEventAlgo("MMRawDataProvider_"+RoIs).RegionSelectionTool = RegSelTool_MM
        acc.merge( mmAcc )
      #sTGC and MM RDO converter
      stgcAcc = StgcRDODecodeCfg( flags, name="StgcRdoToStgcPrepData_"+RoIs )
      acc.merge( stgcAcc )
      mmAcc = MMRDODecodeCfg( flags, name="MMRdoToMMPrepData_"+RoIs )
      acc.merge( mmAcc )

    return acc

def muFastRecoSequence( RoIs, doFullScanID = False, InsideOutMode=False, extraLoads=None, l2mtmode=False ):

  from AthenaCommon.AppMgr import ToolSvc
  from AthenaCommon.CFElements import parOR

  # muFastRecoSequence = parOR("l2MuViewNode")
  postFix = ""
  if InsideOutMode:
    postFix = "IOmode"
  elif l2mtmode:
    postFix = "l2mtmode"
  muFastRecoSequence = parOR("l2Mu"+postFix+"ViewNode")

  # In insideout mode, need to inherit muon decoding objects for TGC, RPC, MDT, CSC
  import AthenaCommon.CfgMgr as CfgMgr
  ViewVerify = CfgMgr.AthViews__ViewDataVerifier("muFastRecoVDV"+postFix)
  if InsideOutMode:
    ViewVerify.DataObjects = [('Muon::TgcPrepDataContainer','StoreGateSvc+TGC_Measurements'),
                              ('TgcRdoContainer' , 'StoreGateSvc+TGCRDO'),
                              ('Muon::RpcPrepDataContainer','StoreGateSvc+RPC_Measurements'),
                              ('Muon::MdtPrepDataContainer','StoreGateSvc+MDT_DriftCircles')]
    if MuonGeometryFlags.hasCSC():
      ViewVerify.DataObjects += [('Muon::CscPrepDataContainer','StoreGateSvc+CSC_Clusters')]
    if MuonGeometryFlags.hasSTGC():
      ViewVerify.DataObjects += [('Muon::sTgcPrepDataContainer','StoreGateSvc+STGC_Measurements')]
    if MuonGeometryFlags.hasMM():
      ViewVerify.DataObjects += [('Muon::MMPrepDataContainer','StoreGateSvc+MM_Measurements')]
    #muFastRecoSequence+=ViewVerify
  else:
    ViewVerify.DataObjects += [( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+%s' % RoIs )]
  ViewVerify.DataObjects += [( 'xAOD::EventInfo' , 'StoreGateSvc+EventInfo' )]
  if ConfigFlags.Trigger.enableL1MuonPhase1:
    ViewVerify.DataObjects += [( 'xAOD::MuonRoIContainer' , 'StoreGateSvc+LVL1MuonRoIs' )]
  else:
    ViewVerify.DataObjects += [( 'DataVector< LVL1::RecMuonRoI >' , 'StoreGateSvc+HLT_RecMURoIs' )]

  #For L2 multi-track SA mode
  if extraLoads:
    ViewVerify.DataObjects += extraLoads
  
  muFastRecoSequence += ViewVerify

  from TrigT1MuonRecRoiTool.TrigT1MuonRecRoiToolConf import LVL1__TrigT1RPCRecRoiTool
  trigRpcRoiTool = LVL1__TrigT1RPCRecRoiTool("RPCRecRoiTool", UseRun3Config=ConfigFlags.Trigger.enableL1MuonPhase1)
  from TrigL2MuonSA.TrigL2MuonSAConf import TrigL2MuonSA__RpcClusterPreparator
  L2RpcClusterPreparator = TrigL2MuonSA__RpcClusterPreparator(name = "L2RpcClusterPreparator",
                                                              TrigT1RPCRecRoiTool = trigRpcRoiTool)
  ToolSvc += L2RpcClusterPreparator

  ### set up MuFastSteering ###
  from TrigL2MuonSA.TrigL2MuonSAConfig import TrigL2MuonSAConfig
  muFastAlg = TrigL2MuonSAConfig("Muon"+postFix)

  muFastAlg.Run2RecMuonRoI = "HLT_RecMURoIs"
  muFastAlg.RecMuonRoI = "LVL1MuonRoIs"
  muFastAlg.MuRoIs = RoIs
  muFastAlg.MuonL2SAInfo = muNames.L2SAName+postFix
  muFastAlg.L2IOCB = muNames.L2CBName+postFix
  muFastAlg.MuonCalibrationStream = "MuonCalibrationStream"+postFix
  muFastAlg.forID = muNames.L2forIDName+postFix
  muFastAlg.forMS = "forMS"+postFix
  muFastAlg.FILL_FSIDRoI = doFullScanID
  muFastAlg.InsideOutMode = InsideOutMode
  muFastAlg.TrackParticlesContainerName = getIDTracks()
  #Do not run topo road and inside-out mode at the same time
  if InsideOutMode:
    muFastAlg.topoRoad = False
  
  if l2mtmode:
    muFastAlg.multitrackMode = True
    muFastAlg.doEndcapForl2mt = False

  if ConfigFlags.Trigger.enableL1MuonPhase1:
    muFastAlg.UseRun3Config = True
  else:
    muFastAlg.UseRun3Config = False

  muFastRecoSequence += muFastAlg
  sequenceOut = muFastAlg.MuonL2SAInfo

  return muFastRecoSequence, sequenceOut


def muonIDFastTrackingSequence( RoIs, name, extraLoads=None, extraLoadsForl2mtmode=None, doLRT=False ):

  from AthenaCommon.CFElements import parOR

  viewNodeName=name+"FastIDViewNode"

  muonIDFastTrackingSequence = parOR(viewNodeName)

  ### Define input data of Inner Detector algorithms  ###
  ### and Define EventViewNodes to run the algorithms ###
  from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
  IDTrigConfig = getInDetTrigConfig( "muon"+name ) 

  from TrigInDetConfig.InDetTrigFastTracking import makeInDetTrigFastTracking
  viewAlgs, viewVerify = makeInDetTrigFastTracking( config = IDTrigConfig, rois = RoIs )
  viewVerify.DataObjects += [( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+%s' % RoIs )]
  if extraLoads:
    viewVerify.DataObjects += extraLoads

  if extraLoadsForl2mtmode:
    viewVerify.DataObjects += extraLoadsForl2mtmode

  for viewAlg in viewAlgs:
      muonIDFastTrackingSequence += viewAlg

  return muonIDFastTrackingSequence

def muonIDCosmicTrackingSequence( RoIs, name, extraLoads=None ):

  from AthenaCommon.CFElements import parOR
  viewNodeName=name+"IDTrackingViewNode"

  trackingSequence = parOR(viewNodeName)

  from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
  IDTrigConfig = getInDetTrigConfig( "cosmics" )

  from TrigInDetConfig.InDetTrigFastTracking import makeInDetTrigFastTracking
  dataPreparationAlgs, dataVerifier = makeInDetTrigFastTracking( config = IDTrigConfig, rois = RoIs, doFTF = False)
   
  dataVerifier.DataObjects += [( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+%s'%RoIs )]

  from TrigInDetConfig.EFIDTracking import makeInDetPatternRecognition
  trackingAlgs, _ = makeInDetPatternRecognition( config  = IDTrigConfig, verifier = 'VDVCosmicIDTracking' )

  if extraLoads:
    dataVerifier.DataObjects += extraLoads

  for alg in dataPreparationAlgs:
      trackingSequence += alg

  for alg in trackingAlgs:
      trackingSequence += alg

  return trackingSequence




def muCombRecoSequence( RoIs, name, l2mtmode=False ):

  from AthenaCommon.CFElements import parOR
  postFix = ""
  if l2mtmode:
    postFix = "l2mtmode"

  muCombRecoSequence = parOR("l2muCombViewNode_"+name+postFix)
  ### A simple algorithm to confirm that data has been inherited from parent view ###
  ### Required to satisfy data dependencies                                       ###
  import AthenaCommon.CfgMgr as CfgMgr
  ViewVerify = CfgMgr.AthViews__ViewDataVerifier("muFast"+postFix+"ViewDataVerifier_"+name)
  ViewVerify.DataObjects = [('xAOD::L2StandAloneMuonContainer','StoreGateSvc+%s' % muNames.L2SAName+postFix)]

  muCombRecoSequence+=ViewVerify

  ### please read out TrigmuCombConfig file ###
  ### and set up to run muComb algorithm    ###
  from TrigmuComb.TrigmuCombConfig import TrigmuCombConfig
  muCombAlg = TrigmuCombConfig("Muon"+postFix,name)
  muCombAlg.L2StandAloneMuonContainerName = muNames.L2SAName+postFix
  if ('LRT' in name):
    muCombAlg.L2CombinedMuonContainerName   = muNamesLRT.L2CBName
  else:
    muCombAlg.L2CombinedMuonContainerName   = muNames.L2CBName+postFix
    
  if l2mtmode:
    muCombAlg.TrackParticlesContainerName   = getIDTracks()
  else:
    muCombAlg.TrackParticlesContainerName   = getIDTracks(name)

  muCombRecoSequence += muCombAlg
  sequenceOut = muCombAlg.L2CombinedMuonContainerName

  return muCombRecoSequence, sequenceOut


def muEFSARecoSequence( RoIs, name ):

  import AthenaCommon.CfgGetter as CfgGetter

  from AthenaCommon import CfgMgr
  from AthenaCommon.CFElements import parOR
  from MuonRecExample.MuonStandalone import MooSegmentFinderAlg, MuonStandaloneTrackParticleCnvAlg, MuonSegmentFinderAlg, MuonStationsInterSectAlg
  from MuonCombinedRecExample.MuonCombinedAlgs import MuonCombinedMuonCandidateAlg, MuonCreatorAlg
  from MuonCombinedAlgs.MuonCombinedAlgsMonitoring import MuonCreatorAlgMonitoring

  muEFSARecoSequence = parOR("efmsViewNode_"+name)

  efAlgs = []

  EFMuonViewDataVerifier = CfgMgr.AthViews__ViewDataVerifier( "EFMuonViewDataVerifier_" + name )
  EFMuonViewDataVerifier.DataObjects = [( 'xAOD::EventInfo' , 'StoreGateSvc+EventInfo' ),
                                        ( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+%s' % RoIs )]

  efAlgs.append( EFMuonViewDataVerifier )

  #need MdtCondDbAlg for the MuonStationIntersectSvc (required by segment and track finding)
  from AthenaCommon.AlgSequence import AthSequencer
  from MuonCondAlg.MuonTopCondAlgConfigRUN2 import MdtCondDbAlg
  if not athenaCommonFlags.isOnline:
    condSequence = AthSequencer("AthCondSeq")
    if not hasattr(condSequence,"MdtCondDbAlg"):
      condSequence += MdtCondDbAlg("MdtCondDbAlg")
    # Sets up and configures the muon alignment:
    from MuonRecExample import MuonAlignConfig # noqa: F401

  MuonStationsInterSectAlg()
  if (MuonGeometryFlags.hasSTGC() and MuonGeometryFlags.hasMM()):
      theMuonLayerHough = CfgMgr.MuonLayerHoughAlg( "MuonLayerHoughAlg",  CscPrepDataContainer = ("CSC_Clusters" if MuonGeometryFlags.hasCSC() else ""),
                                                    sTgcPrepDataContainer = ("STGC_Measurements" if MuonGeometryFlags.hasSTGC() else ""),
                                                    MMPrepDataContainer = ("MM_Measurements" if MuonGeometryFlags.hasMM() else ""))
      efAlgs.append(theMuonLayerHough)

      theSegmentFinderAlg = MuonSegmentFinderAlg("TrigMuonSegmentMaker_"+name)
  else:
    theSegmentFinderAlg = MooSegmentFinderAlg("TrigMuonSegmentMaker_"+name)

  from MuonSegmentTrackMaker.MuonTrackMakerAlgsMonitoring import MuPatTrackBuilderMonitoring
  TrackBuilder = CfgMgr.MuPatTrackBuilder("TrigMuPatTrackBuilder_"+name ,MuonSegmentCollection = "TrackMuonSegments", 
                                          TrackSteering=CfgGetter.getPublicToolClone("TrigMuonTrackSteering", "MuonTrackSteering"), 
                                          MonTool = MuPatTrackBuilderMonitoring("MuPatTrackBuilderMonitoringSA_"+name))
  xAODTrackParticleCnvAlg = MuonStandaloneTrackParticleCnvAlg("TrigMuonStandaloneTrackParticleCnvAlg_"+name)
  theMuonCandidateAlg=MuonCombinedMuonCandidateAlg("TrigMuonCandidateAlg_"+name)

  msMuonName = muNames.EFSAName
  if 'FS' in name:
    msMuonName = muNamesFS.EFSAName

  themuoncreatoralg = MuonCreatorAlg("TrigMuonCreatorAlg_"+name, CreateSAmuons=True, TagMaps=[], MuonContainerLocation=msMuonName,
                                     ExtrapolatedLocation = "HLT_MSExtrapolatedMuons_"+name, MSOnlyExtrapolatedLocation = "HLT_MSOnlyExtrapolatedMuons_"+name,
                                     MuonCreatorTool = CfgGetter.getPrivateToolClone("TrigMuonCreatorToolSA", "MuonCreatorTool", TrackSummaryTool = CfgGetter.getPublicTool("MuonTrackSummaryTool")),
                                     MonTool = MuonCreatorAlgMonitoring("MuonCreatorAlgSA_"+name))


  #Algorithms to views
  efAlgs.append( theSegmentFinderAlg )
  efAlgs.append( TrackBuilder )
  efAlgs.append( xAODTrackParticleCnvAlg )
  efAlgs.append( theMuonCandidateAlg )
  efAlgs.append( themuoncreatoralg )


  # setup muEFMsonly algs
  for efAlg in efAlgs:
      if "RoIs" in efAlg.properties():
        if name == "FS":
          from HLTSeeding.HLTSeedingConfig import mapThresholdToL1RoICollection 
          efAlg.RoIs = mapThresholdToL1RoICollection("FS")
        else:
          efAlg.RoIs = RoIs
      muEFSARecoSequence += efAlg
  sequenceOut = themuoncreatoralg.MuonContainerLocation

  return muEFSARecoSequence, sequenceOut




def muEFCBRecoSequence( RoIs, name ):


  from AthenaCommon import CfgMgr
  from AthenaCommon.CFElements import parOR
  from MuonCombinedRecExample.MuonCombinedAlgs import MuonCombinedInDetCandidateAlg, MuonCombinedAlg, MuonCreatorAlg
  from MuonCombinedAlgs.MuonCombinedAlgsMonitoring import MuonCreatorAlgMonitoring

  efAlgs = []
  muEFCBRecoSequence = parOR("efcbViewNode_"+name)
  #Need ID tracking related objects and MS tracks from previous steps
  ViewVerifyMS = CfgMgr.AthViews__ViewDataVerifier("muonCBViewDataVerifier_"+name)
  ViewVerifyMS.DataObjects = [( 'Muon::MdtPrepDataContainer' , 'StoreGateSvc+MDT_DriftCircles' ),  
                              ( 'Muon::TgcPrepDataContainer' , 'StoreGateSvc+TGC_Measurements' ),
                              ( 'Muon::RpcPrepDataContainer' , 'StoreGateSvc+RPC_Measurements' ),
                              ( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+%s' % RoIs ),
                              ( 'xAOD::EventInfo' , 'StoreGateSvc+EventInfo' )]
  if "FS" in name:
    ViewVerifyMS.DataObjects +=[( 'MuonCandidateCollection' , 'StoreGateSvc+MuonCandidates_FS' )]
  else:
    ViewVerifyMS.DataObjects +=[( 'MuonCandidateCollection' , 'StoreGateSvc+MuonCandidates')]

  if MuonGeometryFlags.hasCSC():
    ViewVerifyMS.DataObjects += [( 'Muon::CscStripPrepDataContainer' , 'StoreGateSvc+CSC_Measurements' )]
  if (MuonGeometryFlags.hasSTGC() and MuonGeometryFlags.hasMM()): 
    ViewVerifyMS.DataObjects += [( 'Muon::MMPrepDataContainer' , 'StoreGateSvc+MM_Measurements'),
                                ( 'Muon::sTgcPrepDataContainer' , 'StoreGateSvc+STGC_Measurements') ]

  muEFCBRecoSequence += ViewVerifyMS

  from AthenaCommon.AlgSequence import AlgSequence
  topSequence = AlgSequence()

  if not globalflags.InputFormat.is_bytestream():
    topSequence.SGInputLoader.Load += [( 'TRT_RDO_Container' , 'StoreGateSvc+TRT_RDOs' )]
    ViewVerifyMS.DataObjects += [( 'TRT_RDO_Container' , 'StoreGateSvc+TRT_RDOs' )]


  from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
  signatureName = 'muon{}'.format( 'FS' if 'FS' in name else 'LRT' if 'LRT' in name else '' ) 
  IDTrigConfig = getInDetTrigConfig( signatureName )

  if "FS" in name:
    #Need to run tracking for full scan chains
    from TrigInDetConfig.InDetTrigFastTracking import makeInDetTrigFastTracking
    viewAlgs, viewVerify = makeInDetTrigFastTracking(config = IDTrigConfig, rois = RoIs) 

    for viewAlg in viewAlgs:
      muEFCBRecoSequence += viewAlg

  elif "LRT" in name:
    ViewVerifyTrk = CfgMgr.AthViews__ViewDataVerifier("muonCBIDViewDataVerifierLRT")
    ViewVerifyTrk.DataObjects = [( 'xAOD::TrackParticleContainer' , 'StoreGateSvc+'+getIDTracks(name) ),
                                 ( 'IDCInDetBSErrContainer' , 'StoreGateSvc+SCT_FlaggedCondData_TRIG' ),
                                 ( 'xAOD::IParticleContainer' , 'StoreGateSvc+'+ getIDTracks(name) )]

    if globalflags.InputFormat.is_bytestream():
      ViewVerifyTrk.DataObjects += [( 'IDCInDetBSErrContainer' , 'StoreGateSvc+PixelByteStreamErrs' ),
                                    ( 'IDCInDetBSErrContainer' , 'StoreGateSvc+SCT_ByteStreamErrs' )]
    muEFCBRecoSequence += ViewVerifyTrk

  else:
    ViewVerifyTrk = CfgMgr.AthViews__ViewDataVerifier("muonCBIDViewDataVerifier")
    ViewVerifyTrk.DataObjects = [( 'xAOD::TrackParticleContainer' , 'StoreGateSvc+'+getIDTracks() ),
                                 ( 'IDCInDetBSErrContainer' , 'StoreGateSvc+SCT_FlaggedCondData_TRIG' ),
                                 ( 'xAOD::IParticleContainer' , 'StoreGateSvc+'+ getIDTracks() )]

    if globalflags.InputFormat.is_bytestream():
      ViewVerifyTrk.DataObjects += [( 'IDCInDetBSErrContainer' , 'StoreGateSvc+PixelByteStreamErrs' ),
                                    ( 'IDCInDetBSErrContainer' , 'StoreGateSvc+SCT_ByteStreamErrs' )]
    muEFCBRecoSequence += ViewVerifyTrk


  #Precision Tracking
  PTAlgs = [] #List of precision tracking algs
  PTTracks = [] #List of TrackCollectionKeys
  PTTrackParticles = [] #List of TrackParticleKeys

  from TrigInDetConfig.InDetTrigPrecisionTracking import makeInDetTrigPrecisionTracking
  #When run in a different view than FTF some data dependencies needs to be loaded through verifier
  #Pass verifier as an argument and it will automatically append necessary DataObjects
  #@NOTE: Don't provide any verifier if loaded in the same view as FTF
  if 'FS' in name:
    PTTracks, PTTrackParticles, PTAlgs = makeInDetTrigPrecisionTracking( config = IDTrigConfig, rois = RoIs, verifier = False)
    PTSeq = parOR("precisionTrackingInMuonsFS", PTAlgs  )
    muEFCBRecoSequence += PTSeq
    trackParticles = PTTrackParticles[-1]
  elif 'LRT' in name:
    PTTracks, PTTrackParticles, PTAlgs = makeInDetTrigPrecisionTracking( config = IDTrigConfig, rois = RoIs,  verifier = ViewVerifyTrk )
    PTSeq = parOR("precisionTrackingInMuonsLRT", PTAlgs  )
    muEFCBRecoSequence += PTSeq
    trackParticles = PTTrackParticles[-1]
  #In case of cosmic Precision Tracking has been already called before hence no need to call here just retrieve the correct collection of tracks
  elif isCosmic():
    if 'LRT' in name:
      PTTracks, PTTrackParticles, PTAlgs = makeInDetTrigPrecisionTracking( config = IDTrigConfig, rois = RoIs,  verifier = ViewVerifyTrk )
      PTSeq = parOR("precisionTrackingInMuonsLRT", PTAlgs  )
      muEFCBRecoSequence += PTSeq
      trackParticles = PTTrackParticles[-1]
    else:
      trackParticles = getIDTracks()
  else:
    PTTracks, PTTrackParticles, PTAlgs = makeInDetTrigPrecisionTracking( config = IDTrigConfig, rois = RoIs,  verifier = ViewVerifyTrk )
    PTSeq = parOR("precisionTrackingInMuons", PTAlgs  )
    muEFCBRecoSequence += PTSeq
    trackParticles = PTTrackParticles[-1]

  #Make InDetCandidates
  theIndetCandidateAlg = MuonCombinedInDetCandidateAlg("TrigMuonCombinedInDetCandidateAlg_"+name,TrackParticleLocation = [trackParticles], 
                                                       InDetCandidateLocation="InDetCandidates_"+name)

  #No easy way to access AtlasHoleSearchTool in theIndetCandidateAlg
  from AthenaCommon.AppMgr import ToolSvc
  from InDetTrigRecExample.InDetTrigConditionsAccess import SCT_ConditionsSetup
  from SCT_ConditionsTools.SCT_ConditionsSummaryToolSetup import SCT_ConditionsSummaryToolSetup
  sct_ConditionsSummaryToolSetupWithoutFlagged = SCT_ConditionsSummaryToolSetup(SCT_ConditionsSetup.instanceName('InDetSCT_ConditionsSummaryToolWithoutFlagged'))
  sct_ConditionsSummaryToolSetupWithoutFlagged.setup()
  ToolSvc.CombinedMuonIDHoleSearch.BoundaryCheckTool.SctSummaryTool = sct_ConditionsSummaryToolSetupWithoutFlagged.getTool()
  if globalflags.InputFormat.is_bytestream():
    ToolSvc.CombinedMuonIDHoleSearch.BoundaryCheckTool.SctSummaryTool.ConditionsTools=['SCT_ConfigurationConditionsTool/InDetTrigInDetSCT_ConfigurationConditionsTool','SCT_ByteStreamErrorsTool/InDetTrigInDetSCT_ByteStreamErrorsTool']
  else:
    ToolSvc.CombinedMuonIDHoleSearch.BoundaryCheckTool.SctSummaryTool.ConditionsTools=['SCT_ConfigurationConditionsTool/InDetTrigInDetSCT_ConfigurationConditionsTool']

  #MS ID combination
  candidatesName = "MuonCandidates"
  if 'FS' in name:
    candidatesName = "MuonCandidates_FS"

  theMuonCombinedAlg = MuonCombinedAlg("TrigMuonCombinedAlg_"+name, MuonCandidateLocation=candidatesName, InDetCandidateLocation="InDetCandidates_"+name)

  cbMuonName = muNames.EFCBOutInName
  if 'FS' in name:
    cbMuonName = muNamesFS.EFCBName
  elif 'LRT' in name:
    cbMuonName = muNamesLRT.EFCBName

  themuoncbcreatoralg = MuonCreatorAlg("TrigMuonCreatorAlgCB_"+name, MuonCandidateLocation=[candidatesName], TagMaps=["muidcoTagMap"], InDetCandidateLocation="InDetCandidates_"+name,
                                       MuonContainerLocation = cbMuonName, SegmentContainerName = "xaodCBSegments", TrackSegmentContainerName = "TrkCBSegments", ExtrapolatedLocation = "CBExtrapolatedMuons",
                                       MSOnlyExtrapolatedLocation = "CBMSonlyExtrapolatedMuons", CombinedLocation = "HLT_CBCombinedMuon_"+name,
                                       MonTool = MuonCreatorAlgMonitoring("MuonCreatorAlgCB_"+name))

  #Add all algorithms
  efAlgs.append(theIndetCandidateAlg)
  efAlgs.append(theMuonCombinedAlg)
  efAlgs.append(themuoncbcreatoralg)


  # setup muEFMsonly algs
  for efAlg in efAlgs:
    muEFCBRecoSequence += efAlg
  sequenceOut = themuoncbcreatoralg.MuonContainerLocation


  return muEFCBRecoSequence, sequenceOut


def muEFInsideOutRecoSequence(RoIs, name):

  from AthenaCommon.CFElements import parOR
  from AthenaCommon import CfgMgr

  from MuonRecExample.MuonStandalone import MooSegmentFinderAlg, MuonSegmentFinderAlg
  from MuonCombinedRecExample.MuonCombinedAlgs import MuonCombinedInDetCandidateAlg, MuonInsideOutRecoAlg, MuGirlStauAlg, MuonCreatorAlg, StauCreatorAlg, MuonInDetToMuonSystemExtensionAlg
  from MuonCombinedAlgs.MuonCombinedAlgsMonitoring import MuonCreatorAlgMonitoring

  efAlgs = []

  viewNodeName="efmuInsideOutViewNode_"+name
  if "Late" in name:
    viewNodeName = "latemuInsideOutViewNode"
  efmuInsideOutRecoSequence = parOR(viewNodeName)
  
  candidatesName = "MuonCandidates"
  if 'FS' in name:
    candidatesName = "MuonCandidates_FS"

  if "Late" in name:

    #Need to run hough transform at start of late muon chain
    #need MdtCondDbAlg for the MuonStationIntersectSvc (required by segment and track finding)
    from AthenaCommon.AlgSequence import AthSequencer
    from MuonCondAlg.MuonTopCondAlgConfigRUN2 import MdtCondDbAlg
    if not athenaCommonFlags.isOnline:
      condSequence = AthSequencer("AthCondSeq")
      if not hasattr(condSequence,"MdtCondDbAlg"):
        condSequence += MdtCondDbAlg("MdtCondDbAlg")
      # Sets up and configures the muon alignment:
      from MuonRecExample import MuonAlignConfig # noqa: F401

      if (MuonGeometryFlags.hasSTGC() and MuonGeometryFlags.hasMM()):
        theMuonLayerHough = CfgMgr.MuonLayerHoughAlg( "MuonLayerHoughAlg")
        efAlgs.append(theMuonLayerHough)
        theSegmentFinderAlg = MuonSegmentFinderAlg( "TrigMuonSegmentMaker_"+name)
      else:
        theSegmentFinderAlg = MooSegmentFinderAlg("TrigLateMuonSegmentMaker_"+name)

      efAlgs.append(theSegmentFinderAlg)

    # need to run precisions tracking for late muons, since we don't run it anywhere else

    #Precision Tracking
    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
    IDTrigConfig = getInDetTrigConfig( 'muonLate' )


    PTAlgs = [] #List of precision tracking algs
    PTTracks = [] #List of TrackCollectionKeys
    PTTrackParticles = [] #List of TrackParticleKeys

    from TrigInDetConfig.InDetTrigPrecisionTracking import makeInDetTrigPrecisionTracking
    #When run in a different view than FTF some data dependencies needs to be loaded through verifier
    PTTracks, PTTrackParticles, PTAlgs = makeInDetTrigPrecisionTracking( config = IDTrigConfig, rois=RoIs)
    PTSeq = parOR("precisionTrackingInLateMuons", PTAlgs  )

    efmuInsideOutRecoSequence += PTSeq
    trackParticles = PTTrackParticles[-1]

    #Make InDetCandidates
    theIndetCandidateAlg = MuonCombinedInDetCandidateAlg("TrigMuonCombinedInDetCandidateAlg_"+name,TrackParticleLocation = [trackParticles],ForwardParticleLocation=trackParticles, 
                                                         InDetCandidateLocation="InDetCandidates_"+name)
    from AthenaCommon.AppMgr import ToolSvc
    from InDetTrigRecExample.InDetTrigConditionsAccess import SCT_ConditionsSetup
    from SCT_ConditionsTools.SCT_ConditionsSummaryToolSetup import SCT_ConditionsSummaryToolSetup
    sct_ConditionsSummaryToolSetupWithoutFlagged = SCT_ConditionsSummaryToolSetup(SCT_ConditionsSetup.instanceName('InDetSCT_ConditionsSummaryToolWithoutFlagged'))
    sct_ConditionsSummaryToolSetupWithoutFlagged.setup()
    ToolSvc.CombinedMuonIDHoleSearch.BoundaryCheckTool.SctSummaryTool = sct_ConditionsSummaryToolSetupWithoutFlagged.getTool()
    if globalflags.InputFormat.is_bytestream():
      ToolSvc.CombinedMuonIDHoleSearch.BoundaryCheckTool.SctSummaryTool.ConditionsTools=['SCT_ConfigurationConditionsTool/InDetTrigInDetSCT_ConfigurationConditionsTool','SCT_ByteStreamErrorsTool/InDetTrigInDetSCT_ByteStreamErrorsTool']
    else:
      ToolSvc.CombinedMuonIDHoleSearch.BoundaryCheckTool.SctSummaryTool.ConditionsTools=['SCT_ConfigurationConditionsTool/InDetTrigInDetSCT_ConfigurationConditionsTool']

    efAlgs.append(theIndetCandidateAlg)


  else:
    # for non-latemu chains, the decoding/hough transform is run in an earlier step
    #Need PRD containers for inside-out reco
    ViewVerifyInsideOut = CfgMgr.AthViews__ViewDataVerifier("muonInsideOutViewDataVerifier_"+name)
    ViewVerifyInsideOut.DataObjects = [( 'Muon::RpcPrepDataContainer' , 'StoreGateSvc+RPC_Measurements' ),
                                       ( 'Muon::TgcPrepDataContainer' , 'StoreGateSvc+TGC_Measurements' ),
                                       ( 'MuonCandidateCollection' , 'StoreGateSvc+'+candidatesName )]
    if not isCosmic(): ViewVerifyInsideOut.DataObjects += [( 'Muon::HoughDataPerSectorVec' , 'StoreGateSvc+HoughDataPerSectorVec')]
    if MuonGeometryFlags.hasCSC():
      ViewVerifyInsideOut.DataObjects += [( 'Muon::CscPrepDataContainer' , 'StoreGateSvc+CSC_Clusters' )]
    if (MuonGeometryFlags.hasSTGC() and MuonGeometryFlags.hasMM()): 
      ViewVerifyInsideOut.DataObjects += [( 'Muon::MMPrepDataContainer'       , 'StoreGateSvc+MM_Measurements'),
                                          ( 'Muon::sTgcPrepDataContainer'     , 'StoreGateSvc+STGC_Measurements') ]

    efmuInsideOutRecoSequence += ViewVerifyInsideOut


  #Inside-out reconstruction

  cbMuonName = muNames.EFCBInOutName
  if 'Late' in name:
    cbMuonName = cbMuonName+"_Late"
    theInsideOutRecoAlg = MuGirlStauAlg("TrigMuonLateInsideOutRecoAlg_"+name,InDetCandidateLocation="InDetCandidates_"+name)
    insideoutcreatoralg = StauCreatorAlg("TrigLateMuonCreatorAlg_"+name, TagMaps=["stauTagMap"],InDetCandidateLocation="InDetCandidates_"+name,
                                         MuonContainerLocation = cbMuonName, MonTool = MuonCreatorAlgMonitoring("LateMuonCreatorAlg_"+name))
  else:
    inDetExtensionAlg = MuonInDetToMuonSystemExtensionAlg("TrigInDetMuonExtensionAlg_"+name, InputInDetCandidates="InDetCandidates_"+name,
                                                          WriteInDetCandidates="InDetCandidatesSystemExtended_"+name)
    theInsideOutRecoAlg = MuonInsideOutRecoAlg("TrigMuonInsideOutRecoAlg_"+name,InDetCandidateLocation="InDetCandidatesSystemExtended_"+name)
    insideoutcreatoralg = MuonCreatorAlg("TrigMuonCreatorAlgInsideOut_"+name,  MuonCandidateLocation={candidatesName}, TagMaps=["muGirlTagMap"],InDetCandidateLocation="InDetCandidates_"+name,
                                         MuonContainerLocation = cbMuonName, SegmentContainerName = "xaodInsideOutCBSegments", TrackSegmentContainerName = "TrkInsideOutCBSegments", ExtrapolatedLocation = "InsideOutCBExtrapolatedMuons",
                                         MSOnlyExtrapolatedLocation = "InsideOutCBMSOnlyExtrapolatedMuons", CombinedLocation = "InsideOutCBCombinedMuon", MonTool = MuonCreatorAlgMonitoring("MuonCreatorAlgInsideOut_"+name))
    efAlgs.append(inDetExtensionAlg)

  efAlgs.append(theInsideOutRecoAlg)
  efAlgs.append(insideoutcreatoralg)

  # setup inside-out algs
  for efAlg in efAlgs:
      if "RoIs" in efAlg.properties():
        efAlg.RoIs = RoIs
      efmuInsideOutRecoSequence += efAlg
  sequenceOut = cbMuonName

  return efmuInsideOutRecoSequence, sequenceOut



def efmuisoRecoSequence( RoIs, Muons, doMSiso=False ):

  from AthenaCommon.CFElements import parOR

  name = ""
  if doMSiso:
    name = "MS"

  efmuisoRecoSequence = parOR("efmuIsoViewNode"+name)


  from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
  IDTrigConfig = getInDetTrigConfig( 'muonIso'+name )

  from TrigInDetConfig.InDetTrigFastTracking import makeInDetTrigFastTracking
  viewAlgs, viewVerify = makeInDetTrigFastTracking( config = IDTrigConfig, rois = RoIs )
  viewVerify.DataObjects += [( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+MUEFIsoRoIs'+name ),
                             ( 'xAOD::MuonContainer' , 'StoreGateSvc+IsoViewMuons'+name )]

  # Make sure required objects are still available at whole-event level
  if not globalflags.InputFormat.is_bytestream():
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    viewVerify.DataObjects += [( 'TRT_RDO_Container' , 'StoreGateSvc+TRT_RDOs' )]
    topSequence.SGInputLoader.Load += [( 'TRT_RDO_Container' , 'StoreGateSvc+TRT_RDOs' )]

  for viewAlg in viewAlgs:
    efmuisoRecoSequence += viewAlg

  #Precision Tracking
  PTAlgs = [] #List of precision tracking algs
  PTTracks = [] #List of TrackCollectionKeys
  PTTrackParticles = [] #List of TrackParticleKeys
  
  from TrigInDetConfig.InDetTrigPrecisionTracking import makeInDetTrigPrecisionTracking
  PTTracks, PTTrackParticles, PTAlgs = makeInDetTrigPrecisionTracking( config = IDTrigConfig, rois=RoIs )

  PTSeq = parOR("precisionTrackingInMuonsIso"+name, PTAlgs  )
  efmuisoRecoSequence += PTSeq

  # set up algs
  from TrigMuonEF.TrigMuonEFConfig import TrigMuonEFTrackIsolationConfig
  trigEFmuIso = TrigMuonEFTrackIsolationConfig("TrigEFMuIso"+name)
  if doMSiso:
    trigEFmuIso.requireCombinedMuon=False
  trigEFmuIso.MuonEFContainer = Muons
  trackParticles = PTTrackParticles[-1]
  trigEFmuIso.IdTrackParticles = trackParticles
  trigEFmuIso.MuonContName = muNames.EFIsoMuonName+name
  trigEFmuIso.ptcone02Name = "%s.ptcone02" % Muons
  trigEFmuIso.ptcone03Name = "%s.ptcone03" % Muons

  efmuisoRecoSequence += trigEFmuIso

  sequenceOut = trigEFmuIso.MuonContName

  return efmuisoRecoSequence, sequenceOut


def efLateMuRoISequence(): 

  from AthenaCommon.CFElements import parOR
  efLateMuRoISequence = parOR("efLateMuRoIViewNode")

  # TODO: Replace MuCTPI_RDO dependency with xAOD::MuonRoIContainer for BC+1, BC-1 candidates, ATR-25031
  import AthenaCommon.CfgMgr as CfgMgr
  efLateMuRoIVDV = CfgMgr.AthViews__ViewDataVerifier( "efLateMuRoIVDV" )
  efLateMuRoIVDV.DataObjects = [( 'MuCTPI_RDO' , 'StoreGateSvc+MUCTPI_RDO' )]
  efLateMuRoISequence += efLateMuRoIVDV

  # Make sure the RDOs are still available at whole-event level
  from AthenaCommon.AlgSequence import AlgSequence
  topSequence = AlgSequence()
  topSequence.SGInputLoader.Load += [( 'MuCTPI_RDO' , 'StoreGateSvc+MUCTPI_RDO' )]

  from TrigmuRoI.TrigmuRoIConfig import TrigmuRoIConfig
  roiAlg = TrigmuRoIConfig("TrigmuRoI")
  sequenceOut = "LateMuRoIs"
  roiAlg.RoisWriteHandleKey=sequenceOut

  efLateMuRoISequence+=roiAlg

  return efLateMuRoISequence, sequenceOut
