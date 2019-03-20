#################
### Steering options
#################
## Load common flags
from AthenaCommon.JobProperties import jobproperties as athCommonFlags
from DerivationFrameworkInDet.InDetCommon import *

from InDetPrepRawDataToxAOD.InDetDxAODJobProperties import InDetDxAODFlags

# Select active sub-systems
dumpPixInfo = InDetDxAODFlags.DumpPixelInfo()
dumpSctInfo = InDetDxAODFlags.DumpSctInfo()
dumpTrtInfo = InDetDxAODFlags.DumpTrtInfo()

# event selection for TRT tag and probe
TrtZSel = InDetDxAODFlags.TrtZSelection()
TrtJSel = InDetDxAODFlags.TrtJpsiSelection()

DRAWZSel = InDetDxAODFlags.DRAWZSelection()

# Thin hits to store only the ones on-track
thinHitsOnTrack= InDetDxAODFlags.ThinHitsOnTrack()

# Thin track collection, if necessary
# Example (p_T > 1.0 GeV && delta_z0 < 5 mm):
# InDetTrackParticles.pt > 1*GeV && abs(DFCommonInDetTrackZ0AtPV) < 5.0
thinTrackSelection = InDetDxAODFlags.ThinTrackSelection() ##"InDetTrackParticles.pt > 0.1*GeV"

# Bytestream errors (for sub-systems who have implemented it)
dumpBytestreamErrors=InDetDxAODFlags.DumpByteStreamErrors() #True

# Unassociated hits decorations
dumpUnassociatedHits= InDetDxAODFlags.DumpUnassociatedHits() #True

# Add LArCollisionTime augmentation tool
dumpLArCollisionTime=InDetDxAODFlags.DumpLArCollisionTime() #True

# Force to do not dump truth info if set to False
#  (otherwise determined by autoconf below)
dumpTruthInfo=InDetDxAODFlags.DumpTruthInfo() # True

# Saves partial trigger information in the output stream (none otherwise)
dumpTriggerInfo= InDetDxAODFlags.DumpTriggerInfo()  #True

# Print settings for main tools
printIdTrkDxAODConf = InDetDxAODFlags.PrintIdTrkDxAODConf()  # True

# Create split-tracks if running on cosmics
makeSplitTracks = InDetDxAODFlags.MakeSplitCosmicTracks() and athCommonFlags.Beam.beamType() == 'cosmics'

## Autoconfiguration adjustements
isIdTrkDxAODSimulation = False
if (globalflags.DataSource == 'geant4'):
    isIdTrkDxAODSimulation = True

if ( 'dumpTruthInfo' in dir() ):
    dumpTruthInfo = dumpTruthInfo and isIdTrkDxAODSimulation

if InDetFlags.doSLHC():
    dumpTrtInfo=False

if isIdTrkDxAODSimulation:
    # should only be used for data
    TrtZSel=False
    TrtJSel=False

if TrtZSel or TrtJSel:
    # ensure that normal data TRT options are configured
    dumpTrtInfo=True
    dumpTriggerInfo=True

if DRAWZSel:
    dumpTriggerInfo=True

## Other settings
# Prefix for decoration, if any
prefixName = ""

## More fine-tuning available for each tool/alg below (default value shown)

#################
### Setup tools
#################
from AthenaCommon import CfgMgr
IDDerivationSequence = CfgMgr.AthSequencer("InDetDxAOD_seq")

if dumpTrtInfo:
    from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_StrawNeighbourSvc
    TRTStrawNeighbourSvc=TRT_StrawNeighbourSvc()
    ServiceMgr += TRTStrawNeighbourSvc

    from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_CalDbSvc
    TRTCalibDBSvc=TRT_CalDbSvc()
    ServiceMgr += TRTCalibDBSvc

#Setup charge->ToT back-conversion to restore ToT info as well
if dumpPixInfo: 
    from PixelCalibAlgs.PixelCalibAlgsConf import PixelChargeToTConversion 
    PixelChargeToTConversionSetter = PixelChargeToTConversion(name = "PixelChargeToTConversionSetter") 
    IDDerivationSequence += PixelChargeToTConversionSetter 
    if (printIdTrkDxAODConf):
        print PixelChargeToTConversionSetter
        print PixelChargeToTConversionSetter.properties()

#Setup SCT extension efficiency algorithm if running pixel tracklets
#if InDetFlags.doTrackSegmentsPixel():
#    include ("SCTExtension/SCTExtensionAlg.py")

#Setup split-tracks reconstruction in cosmic-mode and produce xAOD::TrackParticles
if makeSplitTracks:
    # Set input/output container names
    # Setup algorithm to create split tracks
    from InDetTrackSplitterTool.InDetTrackSplitterToolConf import InDet__InDetTrackSplitterTool
    splittertoolcomb= InDet__InDetTrackSplitterTool(name="SplitterTool",
                                                    TrackFitter=ToolSvc.InDetTrackFitter,
                                                    OutputUpperTracksName = "TracksUpperSplit",
                                                    OutputLowerTracksName = "TracksLowerSplit") 
    ToolSvc += splittertoolcomb

    from InDetTrackValidation.InDetTrackValidationConf import InDet__InDetSplittedTracksCreator
    splittercomb=InDet__InDetSplittedTracksCreator(name='CombinedTrackSplitter',
    TrackSplitterTool     = splittertoolcomb,
    TrackCollection       = "Tracks",
    OutputTrackCollection = "Tracks_splitID")
    IDDerivationSequence +=splittercomb
    if (printIdTrkDxAODConf):
        print splittercomb
        print splittercomb.properties()

    # Create xAOD::TrackParticles out of them
    from TrkParticleCreator.TrkParticleCreatorConf import Trk__TrackParticleCreatorTool
    InDetxAODSplitParticleCreatorTool = Trk__TrackParticleCreatorTool(name = "InDetSplitxAODParticleCreatorTool", 
                                                                      Extrapolator            = InDetExtrapolator,
                                                                      TrackSummaryTool        = InDetTrackSummaryToolSharedHits,
                                                                      ForceTrackSummaryUpdate = False,
                                                                      KeepParameters          = True)
    ToolSvc += InDetxAODSplitParticleCreatorTool
    # The following adds truth information, but needs further testing
    #include ("InDetRecExample/ConfiguredInDetTrackTruth.py")
    #if isIdTrkDxAODSimulation:
    #    InDetSplitTracksTruth = ConfiguredInDetTrackTruth("Tracks_splitID",'SplitTrackDetailedTruth','SplitTrackTruth')

    xAODSplitTrackParticleCnvAlg = xAODMaker__TrackParticleCnvAlg('InDetSplitTrackParticles')
    xAODSplitTrackParticleCnvAlg.xAODContainerName = 'InDetSplitTrackParticles'
    xAODSplitTrackParticleCnvAlg.xAODTrackParticlesFromTracksContainerName = 'InDetSplitTrackParticles'
    xAODSplitTrackParticleCnvAlg.TrackParticleCreator = InDetxAODSplitParticleCreatorTool
    xAODSplitTrackParticleCnvAlg.TrackContainerName = 'Tracks_splitID'
    xAODSplitTrackParticleCnvAlg.ConvertTrackParticles = False
    xAODSplitTrackParticleCnvAlg.ConvertTracks = True
    xAODSplitTrackParticleCnvAlg.AddTruthLink = False #isIdTrkDxAODSimulation
    if (isIdTrkDxAODSimulation):
        xAODSplitTrackParticleCnvAlg.TrackTruthContainerName = 'SplitTrackTruth'
    xAODSplitTrackParticleCnvAlg.PrintIDSummaryInfo = True
    IDDerivationSequence += xAODSplitTrackParticleCnvAlg


# setup Z and J/psi selectors for TRT
ZeeMassTool=None
ZmmMassTool=None
Z_SkimmingTool=None
JPSIeeMassTool=None
JPSImmMassTool=None
JPSI_SkimmingTool=None
if TrtZSel or TrtJSel:
    from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__InvariantMassTool
    if TrtZSel:
        triggersE = [
            # Single electron
            'HLT_e24_lhmedium_L1EM18VH',
            'HLT_e24_lhmedium_L1EM20VH',
            'HLT_e60_lhmedium',
            'HLT_e120_lhloose',
            'HLT_e5_lhvloose',
            'HLT_e12_lhvloose_L1EM10VH',
            'HLT_e12_lhvloose_nod0_L1EM10VH',
            'HLT_e15_lhvloose_L1EM7',
            'HLT_e20_lhvloose',
            'HLT_e20_lhvloose_L1EM12',
            'HLT_e20_lhvloose_nod0',
            'HLT_e24_lhmedium_nod0_L1EM20VH',
            'HLT_e24_lhtight_nod0_ivarloose',
            'HLT_e26_lhtight_nod0_ivarloose',
            'HLT_e26_lhtight_smooth_ivarloose',
            'HLT_e28_lhmedium_nod0_L1EM20VH',
            'HLT_e28_lhtight_nod0_ivarloose',
            'HLT_e28_lhtight_smooth_ivarloose',
            'HLT_e50_lhvloose_L1EM15',
            'HLT_e60_medium',
            'HLT_e60_lhmedium_nod0',
            'HLT_e140_lhloose_nod0',
            'HLT_e300_etcut']
        expression_trigE = ' || '.join(triggersE)
        
        triggersM = [
            # Single muon
            'HLT_mu26_imedium',
            'HLT_mu26_ivarmedium',
            'HLT_mu28_imedium',
            'HLT_mu28_ivarmedium',
            'HLT_mu40',
            'HLT_mu50' 
            ]
        expression_trigM = ' || '.join(triggersM)

        # Zee TnP
        requirement_Zee_tag = '(Electrons.Tight || Electrons.LHTight) && Electrons.pt > 24.5*GeV'
        requirement_Zee_probe = 'Electrons.pt > 6.5*GeV'
        
        ZeeMassTool = DerivationFramework__InvariantMassTool( name = "ZeeMassTool",
                                                              ObjectRequirements = requirement_Zee_tag,
                                                              SecondObjectRequirements = requirement_Zee_probe,
                                                              StoreGateEntryName = "Zee_DiElectronMass",
                                                              MassHypothesis = 0.511*MeV,
                                                              SecondMassHypothesis = 0.511*MeV,
                                                              ContainerName = "Electrons",
                                                              SecondContainerName = "Electrons")
        
        ToolSvc+=ZeeMassTool
        expression_Zee = 'count(Zee_DiElectronMass > 75.0*GeV && Zee_DiElectronMass < 105.0*GeV)>=1'
        
        # Zmumu TnP
        requirement_Zmm_tag = 'Muons.ptcone40/Muons.pt < 0.3 && Muons.pt > 10.*GeV'
        requirement_Zmm_probe = 'Muons.ptcone40/Muons.pt < 0.3 && Muons.pt > 4.5*GeV'
        
        ZmmMassTool = DerivationFramework__InvariantMassTool( name = "ZmmMassTool",
                                                              ObjectRequirements = requirement_Zmm_tag,
                                                              SecondObjectRequirements = requirement_Zmm_probe,
                                                              StoreGateEntryName = "Zmm_DiMuonMass",
                                                              MassHypothesis = 105.66*MeV,
                                                              SecondMassHypothesis = 105.66*MeV,
                                                              ContainerName = "Muons",
                                                              SecondContainerName = "Muons")
        ToolSvc+=ZmmMassTool
        expression_Zmm = 'count(Zmm_DiMuonMass > 75.0*GeV && Zmm_DiMuonMass < 105.0*GeV)>=1'

        expression = '( ' + expression_Zee + ' && ( ' + expression_trigE + ' ) ) || ( ' + expression_Zmm + ' && ( ' + expression_trigM + ' ) )'
        
        # Event selection tool
        from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
        Z_SkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "Z_SkimmingTool",
                                                                     expression = expression)
        
        ToolSvc += Z_SkimmingTool
        print Z_SkimmingTool
        
    if TrtJSel:
        triggersE = [
            # Di-electron
            'HLT_e9_etcut_e5_lhtight_nod0_Jpsiee',
            'HLT_e14_etcut_e5_lhtight_nod0_Jpsiee',
            'HLT_e5_lhtight_nod0_e4_etcut',
            'HLT_e5_lhtight_nod0_e4_etcut_Jpsiee',
            'HLT_e9_lhtight_nod0_e4_etcut_Jpsiee',
            'HLT_e14_lhtight_nod0_e4_etcut_Jpsiee',
            ]
        expression_trigE = ' || '.join(triggersE)
        
        triggersM = [
            # Di-muon
            'HLT_2mu4',
            'HLT_2mu6',
            'HLT_2mu6_bJpsimumu',
            'HLT_2mu6_bJpsimumu_delayed',
            'HLT_2mu6_bJpsimumu_Lxy0_delayed',
            'HLT_2mu10_bJpsimumu',
            'HLT_2mu10_bJpsimumu_delayed',
            'HLT_2mu10_bJpsimumu_noL2',
            'HLT_2mu14',
            'HLT_2mu14_nomucomb',
            'HLT_2mu15'
            ]
        expression_trigM = ' || '.join(triggersM)

        # JPSIee TnP
        requirement_JPSIee_tag = '(Electrons.Tight || Electrons.LHTight) && Electrons.pt > 4.5*GeV'
        requirement_JPSIee_probe = 'Electrons.pt > 4.5*GeV'
        
        JPSIeeMassTool = DerivationFramework__InvariantMassTool( name = "JPSIeeMassTool",
                                                                 ObjectRequirements = requirement_JPSIee_tag,
                                                                 SecondObjectRequirements = requirement_JPSIee_probe,
                                                                 StoreGateEntryName = "JPSIee_DiElectronMass",
                                                                 MassHypothesis = 0.511*MeV,
                                                                 SecondMassHypothesis = 0.511*MeV,
                                                                 ContainerName = "Electrons",
                                                                 SecondContainerName = "Electrons")
        
        ToolSvc+=JPSIeeMassTool
        expression_JPSIee = '(count(JPSIee_DiElectronMass > 2.0*GeV && JPSIee_DiElectronMass < 4.0*GeV)>=1)'
        
        # JPSImumu TnP
        requirement_JPSImm_tag = '(Muons.ptcone40/Muons.pt < 0.3 && Muons.pt > 4.5*GeV)'
        requirement_JPSImm_probe = '(Muons.ptcone40/Muons.pt < 0.3 && Muons.pt > 4.5*GeV)'
        
        JPSImmMassTool = DerivationFramework__InvariantMassTool( name = "JPSImmMassTool",
                                                                 ObjectRequirements = requirement_JPSImm_tag,
                                                                 SecondObjectRequirements = requirement_JPSImm_probe,
                                                                 StoreGateEntryName = "JPSImm_DiMuonMass",
                                                                 MassHypothesis = 105.66*MeV,
                                                                 SecondMassHypothesis = 105.66*MeV,
                                                                 ContainerName = "Muons",
                                                                 SecondContainerName = "Muons")
        ToolSvc+=JPSImmMassTool
        expression_JPSImm = '(count(JPSImm_DiMuonMass > 2.0*GeV && JPSImm_DiMuonMass < 4.0*GeV)>=1)'

        expression = '( ' + expression_JPSIee + ' && ( ' + expression_trigE + ' ) ) || ( ' + expression_JPSImm + ' && ( ' + expression_trigM + ' ) )'
        
        
# Event selection tool
        from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
        JPSI_SkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "JPSI_SkimmingTool",
                                                                        expression = expression)

        ToolSvc += JPSI_SkimmingTool
        print JPSI_SkimmingTool



DRAW_ZMUMU_SkimmingTool=None
if DRAWZSel:
  sel_muon1  = 'Muons.pt > 25*GeV && Muons.ptcone40/Muons.pt < 0.3 && Muons.passesIDCuts && (Muons.quality <= 2)'
  sel_muon2  = 'Muons.pt > 20*GeV && Muons.ptcone40/Muons.pt < 0.3 && Muons.passesIDCuts && (Muons.quality <= 2)'
  draw_zmumu = '( count (  DRZmumuMass > 70*GeV   &&  DRZmumuMass < 110*GeV ) >= 1 )'
  from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__InvariantMassTool
  DRZmumuMassTool = DerivationFramework__InvariantMassTool(name = "DRZmumuMassTool",
                                                           ContainerName              = "Muons",
                                                           ObjectRequirements         = sel_muon1,
                                                           SecondObjectRequirements   = sel_muon2,
                                                           MassHypothesis             = 105.66,
                                                           SecondMassHypothesis       = 105.66,
                                                           StoreGateEntryName         = "DRZmumuMass")
  ToolSvc += DRZmumuMassTool
  from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
  DRAW_ZMUMU_SkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "DRAW_ZMUMU_SkimmingTool",
                                                                        expression = draw_zmumu)
  ToolSvc += DRAW_ZMUMU_SkimmingTool
  print DRAW_ZMUMU_SkimmingTool


#################
### Setup decorators tools
#################

if dumpTrtInfo:

    from TRT_DriftFunctionTool.TRT_DriftFunctionToolConf import TRT_DriftFunctionTool
    InDetTRT_DriftFunctionTool = TRT_DriftFunctionTool(name = "InDetTRT_DriftFunctionTool",
                                                       IsMC = isIdTrkDxAODSimulation)

    from InDetPrepRawDataToxAOD.InDetPrepRawDataToxAODConf import TRT_PrepDataToxAOD
    xAOD_TRT_PrepDataToxAOD = TRT_PrepDataToxAOD( name = "xAOD_TRT_PrepDataToxAOD")

    ## Content steering Properties (default value shown as comment)
    xAOD_TRT_PrepDataToxAOD.TRTDriftFunctionTool = InDetTRT_DriftFunctionTool
    xAOD_TRT_PrepDataToxAOD.OutputLevel=INFO
    xAOD_TRT_PrepDataToxAOD.UseTruthInfo = dumpTruthInfo
    #xAOD_TRT_PrepDataToxAOD.WriteSDOs    = True

    IDDerivationSequence += xAOD_TRT_PrepDataToxAOD
    if (printIdTrkDxAODConf):
        print xAOD_TRT_PrepDataToxAOD
        print xAOD_TRT_PrepDataToxAOD.properties()

    # to store dEdx info
    from TRT_ToT_Tools.TRT_ToT_ToolsConf import TRT_ToT_dEdx
    TRT_dEdx_Tool = TRT_ToT_dEdx(name="TRT_ToT_dEdx")
    ToolSvc += TRT_dEdx_Tool

    # to get shared hit info
    from InDetAssociationTools.InDetAssociationToolsConf import InDet__InDetPRD_AssociationToolGangedPixels
    InDetPrdAssociationTool = InDet__InDetPRD_AssociationToolGangedPixels(name                           = "InDetPrdAssociationTool",
                                                                          PixelClusterAmbiguitiesMapName = InDetKeys.GangedPixelMap(),
                                                                          addTRToutliers                 = True,
                                                                          OutputLevel=INFO
                                                                          )
    print InDetPrdAssociationTool
    ToolSvc += InDetPrdAssociationTool
    TrackCollectionKeys = []

    from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
    InDetNewTrackingCutsPixel = ConfiguredNewTrackingCuts("Pixel")
    print InDetNewTrackingCutsPixel

    from InDetTrackPRD_Association.InDetTrackPRD_AssociationConf import InDet__InDetTrackPRD_Association
    InDetPRD_Association = InDet__InDetTrackPRD_Association(name            = 'InDetPRD_Association'+InDetNewTrackingCutsPixel.extension(),
                                                            AssociationTool = InDetPrdAssociationTool,
                                                            TracksName = ['Tracks'],
                                                            OutputLevel=INFO
                                                            )
    topSequence += InDetPRD_Association
    print InDetPRD_Association

if dumpSctInfo:
    from InDetPrepRawDataToxAOD.InDetPrepRawDataToxAODConf import SCT_PrepDataToxAOD
    xAOD_SCT_PrepDataToxAOD = SCT_PrepDataToxAOD( name = "xAOD_SCT_PrepDataToxAOD")
    ## Content steering Properties (default value shown as comment)
    xAOD_SCT_PrepDataToxAOD.OutputLevel=INFO
    xAOD_SCT_PrepDataToxAOD.UseTruthInfo        = dumpTruthInfo
    xAOD_SCT_PrepDataToxAOD.WriteRDOinformation = True
    #xAOD_SCT_PrepDataToxAOD.WriteSDOs           = True
    #xAOD_SCT_PrepDataToxAOD.WriteSiHits         = True # if available

    IDDerivationSequence += xAOD_SCT_PrepDataToxAOD
    if (printIdTrkDxAODConf):
        print xAOD_SCT_PrepDataToxAOD
        print xAOD_SCT_PrepDataToxAOD.properties()

if dumpPixInfo:
    from IOVDbSvc.CondDB import conddb
    if not conddb.folderRequested("/PIXEL/DCS/FSMSTATE"):
      conddb.addFolder("DCS_OFL", "/PIXEL/DCS/FSMSTATE", className="CondAttrListCollection")
    if not conddb.folderRequested("/PIXEL/DCS/FSMSTATUS"):
      conddb.addFolder("DCS_OFL", "/PIXEL/DCS/FSMSTATUS", className="CondAttrListCollection")

    from AthenaCommon.AlgSequence import AthSequencer
    condSeq = AthSequencer("AthCondSeq")
    if not hasattr(condSeq, "PixelDCSCondStateAlg"):
      from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDCSCondStateAlg
      condSeq += PixelDCSCondStateAlg(name="PixelDCSCondStateAlg")

    from AthenaCommon.AppMgr import ToolSvc
    if not hasattr(ToolSvc, "PixelLorentzAngleTool"):
        from SiLorentzAngleTool.PixelLorentzAngleToolSetup import PixelLorentzAngleToolSetup
        pixelLorentzAngleToolSetup = PixelLorentzAngleToolSetup()

    from InDetPrepRawDataToxAOD.InDetPrepRawDataToxAODConf import PixelPrepDataToxAOD
    xAOD_PixelPrepDataToxAOD = PixelPrepDataToxAOD( name = "xAOD_PixelPrepDataToxAOD")
    xAOD_PixelPrepDataToxAOD.LorentzAngleTool       = ToolSvc.PixelLorentzAngleTool
    xAOD_PixelPrepDataToxAOD.OutputLevel          = INFO
    xAOD_PixelPrepDataToxAOD.UseTruthInfo         = dumpTruthInfo
    xAOD_PixelPrepDataToxAOD.WriteRDOinformation  = InDetDxAODFlags.DumpPixelRdoInfo()
    xAOD_PixelPrepDataToxAOD.WriteNNinformation   = InDetDxAODFlags.DumpPixelNNInfo()
    #xAOD_PixelPrepDataToxAOD.WriteSDOs            = True
    #xAOD_PixelPrepDataToxAOD.WriteSiHits          = True # if available
    if InDetFlags.doSLHC():
        xAOD_PixelPrepDataToxAOD.WriteNNinformation=False

    IDDerivationSequence += xAOD_PixelPrepDataToxAOD
    if (printIdTrkDxAODConf):
        print xAOD_PixelPrepDataToxAOD
        print xAOD_PixelPrepDataToxAOD.properties()


#################
### Setup Augmentation tools
#################
augmentationTools=[]
if TrtZSel:
    augmentationTools.append(ZeeMassTool, ZmmMassTool)
if TrtJSel:
    augmentationTools.append(JPSIeeMassTool, JPSImmMassTool)

if DRAWZSel:
    augmentationTools.append(DRZmumuMassTool)


from AthenaCommon import CfgMgr

# Set up stream auditor
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
if not hasattr(svcMgr, 'DecisionSvc'):
        svcMgr += CfgMgr.DecisionSvc()
svcMgr.DecisionSvc.CalcStats = True


# Add the TSOS augmentation tool to the derivation framework
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackStateOnSurfaceDecorator
DFTSOS = DerivationFramework__TrackStateOnSurfaceDecorator(name = "DFTrackStateOnSurfaceDecorator",
                                                          ContainerName = "InDetTrackParticles",
                                                          DecorationPrefix = prefixName,
                                                          StoreTRT   = dumpTrtInfo,
                                                          StoreSCT   = dumpSctInfo,
                                                          StorePixel = dumpPixInfo,
                                                          IsSimulation = isIdTrkDxAODSimulation,
                                                           AssociationTool = InDetPrdAssociationTool,
                                                          OutputLevel = INFO)

if dumpTrtInfo:
    #Add tool to calculate TRT-based dEdx
    DFTSOS.TRT_ToT_dEdx = TRT_dEdx_Tool

ToolSvc += DFTSOS
augmentationTools+=[DFTSOS]
if (printIdTrkDxAODConf):
    print DFTSOS
    print DFTSOS.properties()

# If requested, decorate also split tracks (for cosmics)
if makeSplitTracks:
    DFTSOS_SplitTracks = DerivationFramework__TrackStateOnSurfaceDecorator(name = "DFSplitTracksTrackStateOnSurfaceDecorator",
                                                          ContainerName = "InDetSplitTrackParticles",
                                                          DecorationPrefix = prefixName,
                                                          StoreTRT   = dumpTrtInfo,
                                                          TrtMsosName = 'TRT_SplitTracks_MSOSs',
                                                          StoreSCT   = dumpSctInfo,
                                                          SctMsosName = 'SCT_SplitTracks_MSOSs',
                                                          StorePixel = dumpPixInfo,
                                                          PixelMsosName = 'Pixel_SplitTracks_MSOSs',
                                                          IsSimulation = isIdTrkDxAODSimulation,
                                                          OutputLevel = INFO)
    ToolSvc += DFTSOS_SplitTracks
    augmentationTools += [DFTSOS_SplitTracks]

# Add BS error augmentation tool
if dumpBytestreamErrors:
    from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EventInfoBSErrDecorator
    DFEI = DerivationFramework__EventInfoBSErrDecorator(name = "DFEventInfoBSErrDecorator",
                                                        ContainerName = "EventInfo",
                                                        DecorationPrefix = prefixName,
                                                        OutputLevel =INFO)
    ToolSvc += DFEI
    augmentationTools+=[DFEI]
    if (printIdTrkDxAODConf):
        print DFEI
        print DFEI.properties()

# Add Unassociated hits augmentation tool
if dumpUnassociatedHits:
    from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__UnassociatedHitsGetterTool 
    unassociatedHitsGetterTool = DerivationFramework__UnassociatedHitsGetterTool (name = 'unassociatedHitsGetter',
                                                                                  TrackCollection = "Tracks",
                                                                                  PixelClusters = "PixelClusters",
                                                                                  SCTClusterContainer = "SCT_Clusters",
                                                                                  TRTDriftCircleContainer = "TRT_DriftCircles")
    ToolSvc += unassociatedHitsGetterTool
    if (printIdTrkDxAODConf):
        print unassociatedHitsGetterTool
        print unassociatedHitsGetterTool.properties()

    from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__UnassociatedHitsDecorator
    unassociatedHitsDecorator = DerivationFramework__UnassociatedHitsDecorator (name ='unassociatedHitsDecorator',
                                                                                UnassociatedHitsGetter = unassociatedHitsGetterTool,
                                                                                ContainerName = "EventInfo",
                                                                                DecorationPrefix = prefixName,
                                                                                OutputLevel =INFO)
    ToolSvc += unassociatedHitsDecorator
    augmentationTools+=[unassociatedHitsDecorator]
    if (printIdTrkDxAODConf):
        print unassociatedHitsDecorator
        print unassociatedHitsDecorator.properties()

# Add LArCollisionTime augmentation tool
if dumpLArCollisionTime:
    from LArCellRec.LArCollisionTimeGetter import LArCollisionTimeGetter
    from RecExConfig.ObjKeyStore           import cfgKeyStore
    # We can only do this if we have the cell container.
    if cfgKeyStore.isInInput ('CaloCellContainer', 'AllCalo'):
        LArCollisionTimeGetter (IDDerivationSequence)

        from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__LArCollisionTimeDecorator
        lArCollisionTimeDecorator = DerivationFramework__LArCollisionTimeDecorator (name ='lArCollisionTimeDecorator',
                                                                                    ContainerName = "EventInfo",
                                                                                    DecorationPrefix = prefixName+"LArCollTime_",
                                                                                    OutputLevel =INFO)
        ToolSvc += lArCollisionTimeDecorator
        augmentationTools+=[lArCollisionTimeDecorator]
        if (printIdTrkDxAODConf):
            print lArCollisionTimeDecorator
            print lArCollisionTimeDecorator.properties()

# Add decoration with truth parameters if running on simulation
if isIdTrkDxAODSimulation:
    from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParametersForTruthParticles
    TruthDecor = DerivationFramework__TrackParametersForTruthParticles( name = "TruthTPDecor",
                                                                        TruthParticleContainerName = "TruthParticles",
                                                                        DecorationPrefix = "")
    ToolSvc += TruthDecor
    augmentationTools.append(TruthDecor)
    print TruthDecor

#====================================================================
# Skimming Tools
#====================================================================
skimmingTools = []
if TrtZSel:
    skimmingTools.append(Z_SkimmingTool)
if TrtJSel:
    skimmingTools.append(JPSI_SkimmingTool)

if DRAWZSel:
  skimmingTools.append(DRAW_ZMUMU_SkimmingTool)


#minimumbiasTrig = '(L1_RD0_FILLED)'
#
#if not IsMonteCarlo:
#  from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
#  TrigSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "TrigSkimmingTool", expression = minimumbiasTrig)
#  ToolSvc += TrigSkimmingTool
#  skimmingTools.append(TrigSkimmingTool)
#  print "InDetDxAOD.py TrigSkimmingTool: ", TrigSkimmingTool

#====================================================================
# Thinning Tools
#====================================================================
thinningTools = []

# TrackParticles directly
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
IDTRKThinningTool = DerivationFramework__TrackParticleThinning(name = "IDTRKThinningTool",
                                                                 ThinningService         = "IDTRKThinningSvc",
                                                                 SelectionString         = thinTrackSelection,
                                                                 InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                 ThinHitsOnTrack = thinHitsOnTrack)
ToolSvc += IDTRKThinningTool
thinningTools.append(IDTRKThinningTool)

#====================================================================
# Create the derivation Kernel and setup output stream
#====================================================================
# Add the derivation job to the top AthAlgSeqeuence
# DerivationJob is COMMON TO ALL DERIVATIONS
IDDerivationSequence += CfgMgr.DerivationFramework__DerivationKernel("DFTSOS_KERN",
                                                                       AugmentationTools = augmentationTools,
                                                                       SkimmingTools = skimmingTools,
                                                                       ThinningTools = thinningTools,
                                                                       OutputLevel = INFO)

topSequence += IDDerivationSequence 
if (printIdTrkDxAODConf):
    print IDDerivationSequence 
    print IDDerivationSequence.properties()

#################
### Steer output file content
#################
## Add service for metadata
ToolSvc += CfgMgr.xAODMaker__TriggerMenuMetaDataTool(
"TriggerMenuMetaDataTool" )
svcMgr.MetaDataSvc.MetaDataTools += [ ToolSvc.TriggerMenuMetaDataTool ]


## Steer output file
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from D2PDMaker.D2PDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
streamName = primDPD.WriteDAOD_IDTRKVALIDStream.StreamName
fileName   = buildFileName( primDPD.WriteDAOD_IDTRKVALIDStream )
IDTRKVALIDStream = MSMgr.NewPoolRootStream( streamName, fileName )
IDTRKVALIDStream.AcceptAlgs(["DFTSOS_KERN"])
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="IDTRKThinningSvc", outStreams=[evtStream] )

excludedAuxData = "-caloExtension.-cellAssociation.-clusterAssociation.-trackParameterCovarianceMatrices.-parameterX.-parameterY.-parameterZ.-parameterPX.-parameterPY.-parameterPZ.-parameterPosition"

# Add generic event information
IDTRKVALIDStream.AddItem("xAOD::EventInfo#*")
IDTRKVALIDStream.AddItem("xAOD::EventAuxInfo#*")

# Add track particles collection and traclets (if available)
IDTRKVALIDStream.AddItem("xAOD::TrackParticleContainer#InDetTrackParticles")
IDTRKVALIDStream.AddItem("xAOD::TrackParticleAuxContainer#InDetTrackParticlesAux."+excludedAuxData)
if InDetFlags.doTrackSegmentsPixel():
    IDTRKVALIDStream.AddItem("xAOD::TrackParticleContainer#InDetPixelTrackParticles")
    IDTRKVALIDStream.AddItem("xAOD::TrackParticleAuxContainer#InDetPixelTrackParticlesAux."+excludedAuxData)
# Add split tracks, if requested
if makeSplitTracks:
    IDTRKVALIDStream.AddItem("xAOD::TrackParticleContainer#InDetSplitTrackParticles")
    IDTRKVALIDStream.AddItem("xAOD::TrackParticleAuxContainer#InDetSplitTrackParticlesAux."+excludedAuxData)

# Add vertices
IDTRKVALIDStream.AddItem("xAOD::VertexContainer#PrimaryVertices")
IDTRKVALIDStream.AddItem("xAOD::VertexAuxContainer#PrimaryVerticesAux.-vxTrackAtVertex")

# Add links and measurements
IDTRKVALIDStream.AddItem("xAOD::TrackStateValidationContainer#*")
IDTRKVALIDStream.AddItem("xAOD::TrackStateValidationAuxContainer#*")
IDTRKVALIDStream.AddItem("xAOD::TrackMeasurementValidationContainer#*")
IDTRKVALIDStream.AddItem("xAOD::TrackMeasurementValidationAuxContainer#*")

# Add info about electrons and muons (are small containers)
IDTRKVALIDStream.AddItem("xAOD::MuonContainer#Muons")
IDTRKVALIDStream.AddItem("xAOD::MuonAuxContainer#MuonsAux.")
IDTRKVALIDStream.AddItem("xAOD::ElectronContainer#Electrons")
IDTRKVALIDStream.AddItem("xAOD::ElectronAuxContainer#ElectronsAux.")
IDTRKVALIDStream.AddItem("xAOD::TrackParticleContainer#GSFTrackParticles")
IDTRKVALIDStream.AddItem("xAOD::TrackParticleAuxContainer#GSFTrackParticlesAux."+excludedAuxData)

if DRAWZSel:
  IDTRKVALIDStream.AddItem("xAOD::TauJetContainer#TauJets")
  IDTRKVALIDStream.AddItem("xAOD::TauJetAuxContainer#TauJetsAux.")
  IDTRKVALIDStream.AddItem("xAOD::JetContainer#AntiKt4EMTopoJets")
  IDTRKVALIDStream.AddItem("xAOD::JetAuxContainer#AntiKt4EMTopoJetsAux.")
  IDTRKVALIDStream.AddItem("xAOD::JetContainer#AntiKt4LCTopoJets")
  IDTRKVALIDStream.AddItem("xAOD::JetAuxContainer#AntiKt4LCTopoJetsAux.")
  IDTRKVALIDStream.AddItem("xAOD::JetContainer#AntiKt2PV0TrackJets")
  IDTRKVALIDStream.AddItem("xAOD::JetAuxContainer#AntiKt2PV0TrackJetsAux.")
  IDTRKVALIDStream.AddItem("xAOD::JetContainer#AntiKt3PV0TrackJets")
  IDTRKVALIDStream.AddItem("xAOD::JetAuxContainer#AntiKt3PV0TrackJetsAux.")
  IDTRKVALIDStream.AddItem("xAOD::BTaggingContainer#BTagging_AntiKt4EMTopo")
  IDTRKVALIDStream.AddItem("xAOD::BTaggingAuxContainer#BTagging_AntiKt4EMTopoAux.")
  IDTRKVALIDStream.AddItem("xAOD::BTaggingContainer#BTagging_AntiKt4LCTopo")
  IDTRKVALIDStream.AddItem("xAOD::BTaggingAuxContainer#BTagging_AntiKt4LCTopoAux.")
  IDTRKVALIDStream.AddItem("xAOD::BTaggingContainer#BTagging_AntiKt2Track")
  IDTRKVALIDStream.AddItem("xAOD::BTaggingAuxContainer#BTagging_AntiKt2TrackAux.")
  IDTRKVALIDStream.AddItem("xAOD::BTaggingContainer#BTagging_AntiKt3Track")
  IDTRKVALIDStream.AddItem("xAOD::BTaggingAuxContainer#BTagging_AntiKt3TrackAux.")
  IDTRKVALIDStream.AddItem("xAOD::MissingETContainer#MET_Calo")
  IDTRKVALIDStream.AddItem("xAOD::MissingETAuxContainer#MET_CaloAux.")
  IDTRKVALIDStream.AddItem("xAOD::MissingETContainer#MET_EMTopo")
  IDTRKVALIDStream.AddItem("xAOD::MissingETContainer#MET_EMTopoRegions")
  IDTRKVALIDStream.AddItem("xAOD::MissingETContainer#MET_Core_AntiKt4EMTopo")
  IDTRKVALIDStream.AddItem("xAOD::MissingETContainer#MET_Reference_AntiKt4EMTopo")
  IDTRKVALIDStream.AddItem("xAOD::MissingETAuxContainer#MET_EMTopoAux.")
  IDTRKVALIDStream.AddItem("xAOD::MissingETAuxContainer#MET_EMTopoRegionsAux.")
  IDTRKVALIDStream.AddItem("xAOD::MissingETAuxContainer#MET_Core_AntiKt4EMTopoAux.")
  IDTRKVALIDStream.AddItem("xAOD::MissingETAuxContainer#MET_Reference_AntiKt4EMTopoAux.")
  IDTRKVALIDStream.AddItem("xAOD::MissingETAuxContainer#MET_Core_AntiKt4EMTopoAuxDyn.softConstituents")
  IDTRKVALIDStream.AddItem("xAOD::MissingETContainer#MET_Track")
  IDTRKVALIDStream.AddItem("xAOD::MissingETAuxContainer#MET_TrackAux.")
  IDTRKVALIDStream.AddItem("xAOD::MissingETContainer#MET_LocHadTopo")
  IDTRKVALIDStream.AddItem("xAOD::MissingETContainer#MET_LocHadTopoRegions")
  IDTRKVALIDStream.AddItem("xAOD::MissingETAuxContainer#MET_LocHadTopoAux.")
  IDTRKVALIDStream.AddItem("xAOD::MissingETAuxContainer#MET_LocHadTopoRegionsAux.")
  IDTRKVALIDStream.AddItem("xAOD::MissingETContainer#MET_Core_AntiKt4LCTopo")
  IDTRKVALIDStream.AddItem("xAOD::MissingETContainer#MET_Reference_AntiKt4LCTopo")
  IDTRKVALIDStream.AddItem("xAOD::MissingETAuxContainer#MET_Core_AntiKt4LCTopoAux.")
  IDTRKVALIDStream.AddItem("xAOD::MissingETAuxContainer#MET_Reference_AntiKt4LCTopoAux.")
  IDTRKVALIDStream.AddItem("xAOD::MissingETAuxContainer#MET_Core_AntiKt4LCTopoAuxDyn.softConstituents")
  if dumpTruthInfo:
    IDTRKVALIDStream.AddItem("xAOD::MissingETContainer#MET_Truth")
    IDTRKVALIDStream.AddItem("xAOD::MissingETContainer#MET_TruthRegions")
    IDTRKVALIDStream.AddItem("xAOD::MissingETAuxContainer#MET_TruthAux.")
    IDTRKVALIDStream.AddItem("xAOD::MissingETAuxContainer#MET_TruthRegionsAux.")

# Add truth-related information
if dumpTruthInfo:
    IDTRKVALIDStream.AddItem("xAOD::TruthParticleContainer#*")
    IDTRKVALIDStream.AddItem("xAOD::TruthParticleAuxContainer#TruthParticlesAux.-caloExtension")
    IDTRKVALIDStream.AddItem("xAOD::TruthVertexContainer#*")
    IDTRKVALIDStream.AddItem("xAOD::TruthVertexAuxContainer#*")
    IDTRKVALIDStream.AddItem("xAOD::TruthEventContainer#*")
    IDTRKVALIDStream.AddItem("xAOD::TruthEventAuxContainer#*")
    IDTRKVALIDStream.AddItem("xAOD::TruthPileupEventContainer#*")
    IDTRKVALIDStream.AddItem("xAOD::TruthPileupEventAuxContainer#*")
    # add pseudo tracking in xAOD
    IDTRKVALIDStream.AddItem("xAOD::TrackParticleContainer#InDetPseudoTrackParticles")
    IDTRKVALIDStream.AddItem("xAOD::TrackParticleAuxContainer#InDetPseudoTrackParticlesAux."+excludedAuxData)

# Add trigger information (including metadata)
if dumpTriggerInfo:
    IDTRKVALIDStream.AddMetaDataItem("xAOD::TriggerMenuContainer#TriggerMenu")
    IDTRKVALIDStream.AddMetaDataItem("xAOD::TriggerMenuAuxContainer#TriggerMenuAux.")
    IDTRKVALIDStream.AddItem("TileCellContainer#MBTSContainer")
    IDTRKVALIDStream.AddItem("xAOD::TrigDecision#xTrigDecision")
    IDTRKVALIDStream.AddItem("BCM_RDO_Container#BCM_RDOs")
    IDTRKVALIDStream.AddItem("xAOD::TrigNavigation#TrigNavigation")
    IDTRKVALIDStream.AddItem("xAOD::TrigConfKeys#TrigConfKeys")
    IDTRKVALIDStream.AddItem("HLT::HLTResult#HLTResult_HLT")
    IDTRKVALIDStream.AddItem("xAOD::TrigDecisionAuxInfo#xTrigDecisionAux.")
    IDTRKVALIDStream.AddItem("xAOD::TrigNavigationAuxInfo#TrigNavigationAux.")

    if dumpTrtInfo and not isIdTrkDxAODSimulation:
        # strangely these options cause crashes in R21 MC reco:  ATLASRECTS-3861 
        from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
        SlimmingHelper = SlimmingHelper("SlimmingHelper")
        SlimmingHelper.AllVariables += ["HLT_xAOD__ElectronContainer_egamma_Electrons","HLT_xAOD__MuonContainer_MuonEFInfo"]
        print SlimmingHelper
        SlimmingHelper.AppendContentToStream(IDTRKVALIDStream)

if (printIdTrkDxAODConf):
    print IDTRKVALIDStream
