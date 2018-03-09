# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#********************************************************************
# EGammaCommon.py 
# Schedules all tools needed for e-gamma object selection and writes
# results into SG. These may then be accessed along the train   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *

#====================================================================
# PHOTON ETA (=ETA2), ET (=E/COSH(ETA2))
#====================================================================
from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__PhotonsDirectionTool
DFCommonPhotonsDirection = DerivationFramework__PhotonsDirectionTool(name = "DFCommonPhotonsDirection",
                                                                     EtaSGEntry = "DFCommonPhotons_eta",
                                                                     PhiSGEntry = "DFCommonPhotons_phi",
                                                                     EtSGEntry = "DFCommonPhotons_et"
                                                                     #ESGEntry = "DFCommonPhotons_e"
                                                                     #PhotonContainer = "Photons"
                                                                     )
ToolSvc += DFCommonPhotonsDirection

#====================================================================
# SHOWER SHAPE FUDGING IN MC 
# (PRESELECTION=16: FUDGE FACTORS GEO21->DATA12)
#====================================================================

from PyUtils import AthFile
af = AthFile.fopen(svcMgr.EventSelector.InputCollections[0]) #opens the first file from the InputCollections list
af.fileinfos #this is a dict of dicts, take a look at what's available! Below are some examples:

isMC = 'IS_SIMULATION' in af.fileinfos['evt_type']
beam_energy = af.fileinfos['beam_energy']
conditions_tag = af.fileinfos['conditions_tag'] #useful for figuring out which mc production this is
isFullSim = False
if isMC: 
    simulationFlavour = af.fileinfos['metadata']['/Simulation/Parameters']['SimulationFlavour']
    isFullSim = simulationFlavour in ('default', 'MC12G4', 'FullG4')

print "EGammaCommon: isMC = ", isMC
if isMC: 
    print "EGammaCommon: isFullSim = ", isFullSim

if isFullSim:
#from AthenaCommon.GlobalFlags import globalflags
#print "globalflags.DataSource(): ", globalflags.DataSource()
#if globalflags.DataSource()=='geant4':
    from ElectronPhotonShowerShapeFudgeTool.ElectronPhotonShowerShapeFudgeToolConf import ElectronPhotonShowerShapeFudgeTool
    DF_ElectronPhotonShowerShapeFudgeTool = ElectronPhotonShowerShapeFudgeTool(Preselection=16)
    ToolSvc += DF_ElectronPhotonShowerShapeFudgeTool
    print DF_ElectronPhotonShowerShapeFudgeTool


#====================================================================
# ELECTRON SELECTION (loose, medium and tight LLH)
# see http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Reconstruction/egamma/egammaTools/python/EMPIDBuilderBase.py on how to configure the selectors
#====================================================================
from ROOT import LikeEnum

from ElectronPhotonSelectorTools.ConfiguredAsgElectronLikelihoodTools import ConfiguredAsgElectronLikelihoodTool

# Very Loose
ElectronLHSelectorVeryLoose = ConfiguredAsgElectronLikelihoodTool("ElectronLHSelectorVeryLoose", LikeEnum.VeryLoose)
ElectronLHSelectorVeryLoose.primaryVertexContainer = "PrimaryVertices"
ToolSvc += ElectronLHSelectorVeryLoose

# Loose
ElectronLHSelectorLoose = ConfiguredAsgElectronLikelihoodTool("ElectronLHSelectorLoose", LikeEnum.Loose)
ElectronLHSelectorLoose.primaryVertexContainer = "PrimaryVertices"
ToolSvc += ElectronLHSelectorLoose

# Medium
ElectronLHSelectorMedium = ConfiguredAsgElectronLikelihoodTool("ElectronLHSelectorMedium", LikeEnum.Medium)
ElectronLHSelectorMedium.primaryVertexContainer = "PrimaryVertices"
ToolSvc += ElectronLHSelectorMedium

# Tight
ElectronLHSelectorTight = ConfiguredAsgElectronLikelihoodTool("ElectronLHSelectorTight", LikeEnum.Tight)
ElectronLHSelectorTight.primaryVertexContainer = "PrimaryVertices"
ToolSvc += ElectronLHSelectorTight

#====================================================================
# ELECTRON SELECTION (loose, medium and tight cut-based)
#====================================================================
from ROOT import egammaPID

from ElectronPhotonSelectorTools.ConfiguredAsgElectronIsEMSelectors import ConfiguredAsgElectronIsEMSelector

# Loose
ElectronIsEMSelectorLoose = ConfiguredAsgElectronIsEMSelector("ElectronIsEMSelectorLoose", egammaPID.ElectronIDLoosePP)
ToolSvc += ElectronIsEMSelectorLoose

# Medium
ElectronIsEMSelectorMedium = ConfiguredAsgElectronIsEMSelector("ElectronIsEMSelectorMedium", egammaPID.ElectronIDMediumPP)
ToolSvc += ElectronIsEMSelectorMedium

# Tight
ElectronIsEMSelectorTight = ConfiguredAsgElectronIsEMSelector("ElectronIsEMSelectorTight", egammaPID.ElectronIDTightPP)
ToolSvc += ElectronIsEMSelectorTight

#====================================================================
# ELECTRON SELECTION (multi lepton)
#====================================================================
from ElectronPhotonSelectorTools.ElectronPhotonSelectorToolsConf import AsgElectronMultiLeptonSelector
ElectronMLSelector = AsgElectronMultiLeptonSelector("ElectronMLSelector")
ToolSvc += ElectronMLSelector

#====================================================================
# PHOTON SELECTION (loose and tight cut-based)
#====================================================================
# Loose
from ElectronPhotonSelectorTools.ConfiguredAsgPhotonIsEMSelectors import ConfiguredAsgPhotonIsEMSelector
PhotonIsEMSelectorLoose = ConfiguredAsgPhotonIsEMSelector("PhotonIsEMSelectorLoose", egammaPID.PhotonIDLoose)
ToolSvc += PhotonIsEMSelectorLoose

# Tight
PhotonIsEMSelectorTight = ConfiguredAsgPhotonIsEMSelector("PhotonIsEMSelectorTight", egammaPID.PhotonIDTight)
ToolSvc += PhotonIsEMSelectorTight

#====================================================================
# RECTANGULAR CLUSTER TOOLS
#====================================================================

from CaloClusterCorrection.CaloClusterCorrectionConf import CaloFillRectangularCluster

EGAMCOM_caloFillRect55 = CaloFillRectangularCluster (	name = "EGAMCOMCaloFillRectangularCluster55",
                                                        cells_name = "AODCellContainer",
                                                        eta_size = 5,
                                                        phi_size = 5,
                                                        fill_cluster = True)
ToolSvc += EGAMCOM_caloFillRect55

EGAMCOM_caloFillRect35 = CaloFillRectangularCluster (	name = "EGAMCOMCaloFillRectangularCluster35",
                                                        cells_name = "AODCellContainer",
                                                        eta_size = 3,
                                                        phi_size = 5,
                                                        fill_cluster = True)
ToolSvc += EGAMCOM_caloFillRect35

EGAMCOM_caloFillRect37 = CaloFillRectangularCluster (	name = "EGAMCOMCaloFillRectangularCluster37",
                                                        cells_name = "AODCellContainer",
                                                        eta_size = 3,
                                                        phi_size = 7,
                                                        fill_cluster = True)
ToolSvc += EGAMCOM_caloFillRect37

EGAMCOM_caloFillRect711 = CaloFillRectangularCluster (	name = "EGAMCOMCaloFillRectangularCluster711",
                                                        cells_name = "AODCellContainer",
                                                        eta_size = 7,
                                                        phi_size = 11,
                                                        fill_cluster = True)
ToolSvc += EGAMCOM_caloFillRect711

#====================================================================
# AUGMENTATION TOOLS
#====================================================================
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__AsgSelectionToolWrapper

# decorate electrons with the output of LH very loose
ElectronPassLHVeryLoose = DerivationFramework__AsgSelectionToolWrapper( name = "ElectronPassLHVeryLoose",
                                                                        AsgSelectionTool = ElectronLHSelectorVeryLoose,
                                                                        CutType = "",
                                                                        StoreGateEntryName = "DFCommonElectronsLHVeryLoose",
                                                                        ContainerName = "Electrons")
ToolSvc += ElectronPassLHVeryLoose
print ElectronPassLHVeryLoose

# decorate electrons with the output of LH loose
ElectronPassLHLoose = DerivationFramework__AsgSelectionToolWrapper( name = "ElectronPassLHLoose",
                                                                    AsgSelectionTool = ElectronLHSelectorLoose,
                                                                    CutType = "",
                                                                    StoreGateEntryName = "DFCommonElectronsLHLoose",
                                                                    ContainerName = "Electrons")
ToolSvc += ElectronPassLHLoose
print ElectronPassLHLoose

# decorate electrons with the output of LH medium
ElectronPassLHMedium = DerivationFramework__AsgSelectionToolWrapper( name = "ElectronPassLHMedium",
                                                                     AsgSelectionTool = ElectronLHSelectorMedium,
                                                                     CutType = "",
                                                                     StoreGateEntryName = "DFCommonElectronsLHMedium",
                                                                     ContainerName = "Electrons")
ToolSvc += ElectronPassLHMedium
print ElectronPassLHMedium

# decorate electrons with the output of LH tight
ElectronPassLHTight = DerivationFramework__AsgSelectionToolWrapper( name = "ElectronPassLHTight",
                                                                    AsgSelectionTool = ElectronLHSelectorTight,
                                                                    CutType = "",
                                                                    StoreGateEntryName = "DFCommonElectronsLHTight",
                                                                    ContainerName = "Electrons")
ToolSvc += ElectronPassLHTight
print ElectronPassLHTight


from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__EGSelectionToolWrapper

# decorate electrons with the output of IsEM loose
ElectronPassIsEMLoose = DerivationFramework__EGSelectionToolWrapper( name = "ElectronPassIsEMLoose",
                                                                     EGammaSelectionTool = ElectronIsEMSelectorLoose,
                                                                     EGammaFudgeMCTool = None,
                                                                     CutType = "",
                                                                     StoreGateEntryName = "DFCommonElectronsIsEMLoose",
                                                                     ContainerName = "Electrons")

ToolSvc += ElectronPassIsEMLoose
print ElectronPassIsEMLoose

# decorate electrons with the output of IsEM medium
ElectronPassIsEMMedium = DerivationFramework__EGSelectionToolWrapper( name = "ElectronPassIsEMMedium",
                                                                     EGammaSelectionTool = ElectronIsEMSelectorMedium,
                                                                     EGammaFudgeMCTool = None,
                                                                     CutType = "",
                                                                     StoreGateEntryName = "DFCommonElectronsIsEMMedium",
                                                                     ContainerName = "Electrons")


ToolSvc += ElectronPassIsEMMedium
print ElectronPassIsEMMedium

# decorate electrons with the output of IsEM tight
ElectronPassIsEMTight = DerivationFramework__EGSelectionToolWrapper( name = "ElectronPassIsEMTight",
                                                                     EGammaSelectionTool = ElectronIsEMSelectorTight,
                                                                     EGammaFudgeMCTool = None,
                                                                     CutType = "",
                                                                     StoreGateEntryName = "DFCommonElectronsIsEMTight",
                                                                     ContainerName = "Electrons")

ToolSvc += ElectronPassIsEMTight
print ElectronPassIsEMTight

# decorate electrons with the output of MultiLepton
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__AsgSelectionToolWrapper
ElectronPassML = DerivationFramework__AsgSelectionToolWrapper( name = "ElectronPassML",
                                                               AsgSelectionTool = ElectronMLSelector,
                                                               CutType = "",
                                                               StoreGateEntryName = "DFCommonElectronsML",
                                                               ContainerName = "Electrons")
ToolSvc += ElectronPassML
print ElectronPassML


from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__EGSelectionToolWrapper

# decorate photons with the output of IsEM loose
# on MC, fudge the shower shapes before computing the ID (but the original shower shapes are not overridden)
#if globalflags.DataSource()=='geant4':
if isFullSim:
    PhotonPassIsEMLoose = DerivationFramework__EGSelectionToolWrapper( name = "PhotonPassIsEMLoose",
                                                                       EGammaSelectionTool = PhotonIsEMSelectorLoose,
                                                                       EGammaFudgeMCTool = DF_ElectronPhotonShowerShapeFudgeTool,
                                                                       CutType = "",
                                                                       StoreGateEntryName = "DFCommonPhotonsIsEMLoose",
                                                                       ContainerName = "Photons")
else:
    PhotonPassIsEMLoose = DerivationFramework__EGSelectionToolWrapper( name = "PhotonPassIsEMLoose",
                                                                       EGammaSelectionTool = PhotonIsEMSelectorLoose,
                                                                       EGammaFudgeMCTool = None,
                                                                       CutType = "",
                                                                       StoreGateEntryName = "DFCommonPhotonsIsEMLoose",
                                                                       ContainerName = "Photons")
ToolSvc += PhotonPassIsEMLoose
print PhotonPassIsEMLoose
 
# decorate photons with the output of IsEM tight
# on full-sim MC, fudge the shower shapes before computing the ID (but the original shower shapes are not overridden)
#if globalflags.DataSource()=='geant4':
if isFullSim:
    PhotonPassIsEMTight = DerivationFramework__EGSelectionToolWrapper( name = "PhotonPassIsEMTight",
                                                                       EGammaSelectionTool = PhotonIsEMSelectorTight,
                                                                       EGammaFudgeMCTool = DF_ElectronPhotonShowerShapeFudgeTool,
                                                                       CutType = "",
                                                                       StoreGateEntryName = "DFCommonPhotonsIsEMTight",
                                                                       ContainerName = "Photons")
else:
    PhotonPassIsEMTight = DerivationFramework__EGSelectionToolWrapper( name = "PhotonPassIsEMTight",
                                                                       EGammaSelectionTool = PhotonIsEMSelectorTight,
                                                                       EGammaFudgeMCTool = None,
                                                                       CutType = "",
                                                                       StoreGateEntryName = "DFCommonPhotonsIsEMTight",
                                                                       ContainerName = "Photons")
ToolSvc += PhotonPassIsEMTight
print PhotonPassIsEMTight

# list of all the decorators so far
EGAugmentationTools = [DFCommonPhotonsDirection,
                       ElectronPassLHVeryLoose,ElectronPassLHLoose,ElectronPassLHMedium,ElectronPassLHTight,
                       ElectronPassIsEMLoose,ElectronPassIsEMMedium,ElectronPassIsEMTight,
                       PhotonPassIsEMLoose,
                       PhotonPassIsEMTight,
                       ElectronPassML]

#==================================================
# Truth Related tools 
from RecExConfig.RecFlags  import rec
if  rec.doTruth(): 

    # Decorate Electron with bkg electron type/origin
    from MCTruthClassifier.MCTruthClassifierBase import MCTruthClassifier as BkgElectronMCTruthClassifier    
    from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__BkgElectronClassification  
    BkgElectronClassificationTool = DerivationFramework__BkgElectronClassification (name = "BkgElectronClassificationTool",
                                                                                    MCTruthClassifierTool = BkgElectronMCTruthClassifier)


    ToolSvc += BkgElectronClassificationTool
    print BkgElectronClassificationTool
    EGAugmentationTools.append(BkgElectronClassificationTool)

    # Decorate egammaTruthParticles with truth-particle-level etcone20,30,40
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthIsolationTool as dtit
    TruthEgetIsolationTool = dtit(name = "TruthEgetIsolationTool",
                                  isoParticlesKey        = "egammaTruthParticles",
                                  allParticlesKey        = "TruthParticles",
                                  particleIDsToCalculate = [-11,11,22],
                                  IsolationConeSizes     = [0.2,0.3,0.4],
                                  excludeIDsFromCone     = [-16,-14,-13,-12,12,13,14,16],
                                  IsolationVarNamePrefix = 'etcone',
                                  ChargedParticlesOnly   = False)
    ToolSvc += TruthEgetIsolationTool
    print TruthEgetIsolationTool
    EGAugmentationTools.append(TruthEgetIsolationTool)

    # Decorate egammaTruthParticles with truth-particle-level ptcone20,30,40
    TruthEgptIsolationTool = dtit(name = "TruthEgptIsolationTool",
                                  isoParticlesKey        = "egammaTruthParticles",
                                  allParticlesKey        = "TruthParticles",
                                  particleIDsToCalculate = [-11,11,22],
                                  IsolationConeSizes     = [0.2,0.3,0.4],
                                  IsolationVarNamePrefix = 'ptcone',
                                  ChargedParticlesOnly   = True)
    ToolSvc += TruthEgptIsolationTool
    print TruthEgptIsolationTool
    EGAugmentationTools.append(TruthEgptIsolationTool)
    
    # Compute the truth-particle-level energy density in the central eta region
    from EventShapeTools.EventDensityConfig import configEventDensityTool, EventDensityAlg
    from JetRec.JetRecStandard import jtm
    tc=configEventDensityTool("EDTruthCentralTool", jtm.truthget,
                              0.5,
                              AbsRapidityMax      = 1.5,
                              OutputContainer     = "TruthIsoCentralEventShape",
                              OutputLevel = 3,
    )                             
    ToolSvc += tc

    # Compute the truth-particle-level energy density in the forward eta region
    tf=configEventDensityTool("EDTruthForwardTool", jtm.truthget,
                              0.5,
                              AbsRapidityMin      = 1.5,
                              AbsRapidityMax      = 3.0,
                              OutputContainer     = "TruthIsoForwardEventShape",
                              OutputLevel = 3,
    )                             
    ToolSvc += tf

    # Schedule the two energy density tools for running
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    topSequence += EventDensityAlg("EDTruthCentralAlg", EventDensityTool = tc )
    topSequence += EventDensityAlg("EDTruthForwardAlg", EventDensityTool = tf )

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__CommonAugmentation
DerivationFrameworkJob += CfgMgr.DerivationFramework__CommonAugmentation("EGammaCommonKernel",
                                                                         AugmentationTools = EGAugmentationTools
                                                                         )



