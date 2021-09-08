# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

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
# (PRESELECTION=22: FUDGE FACTORS RUN2 2015+2016 DATA, Rel 21)
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
    from ElectronPhotonShowerShapeFudgeTool.ElectronPhotonShowerShapeFudgeToolConf import ElectronPhotonShowerShapeFudgeTool
    DF_ElectronPhotonShowerShapeFudgeTool = ElectronPhotonShowerShapeFudgeTool(Preselection=22, ConfigFile="ElectronPhotonShowerShapeFudgeTool/May2016_ShiftsForMCBasedLHTuneUsing20pt7.conf")
    ToolSvc += DF_ElectronPhotonShowerShapeFudgeTool
    print DF_ElectronPhotonShowerShapeFudgeTool


#====================================================================
# ELECTRON LH SELECTORS
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

# LooseBL
from ElectronPhotonSelectorTools.ElectronPhotonSelectorToolsConf import AsgElectronLikelihoodTool
ElectronLHSelectorLooseBL = AsgElectronLikelihoodTool("ElectronLHSelectorLooseBL", WorkingPoint="LooseBLLHElectron")
ElectronLHSelectorLooseBL.primaryVertexContainer = "PrimaryVertices"
ToolSvc += ElectronLHSelectorLooseBL

#v14 r21 tunes
ElectronLHSelectorVeryLoose_v14 = AsgElectronLikelihoodTool("ElectronLHSelectorVeryLoose_v14", WorkingPoint="VeryLooseLHElectron_v14")
ElectronLHSelectorVeryLoose_v14.primaryVertexContainer = "PrimaryVertices"
ToolSvc += ElectronLHSelectorVeryLoose_v14

ElectronLHSelectorLoose_v14 = AsgElectronLikelihoodTool("ElectronLHSelectorLoose_v14", WorkingPoint="LooseLHElectron_v14")
ElectronLHSelectorLoose_v14.primaryVertexContainer = "PrimaryVertices"
ToolSvc += ElectronLHSelectorLoose_v14

ElectronLHSelectorLooseBL_v14 = AsgElectronLikelihoodTool("ElectronLHSelectorLooseBL_v14", WorkingPoint="LooseBLLHElectron_v14")
ElectronLHSelectorLooseBL_v14.primaryVertexContainer = "PrimaryVertices"
ToolSvc += ElectronLHSelectorLooseBL_v14

ElectronLHSelectorMedium_v14 = AsgElectronLikelihoodTool("ElectronLHSelectorMedium_v14", WorkingPoint="MediumLHElectron_v14")
ElectronLHSelectorMedium_v14.primaryVertexContainer = "PrimaryVertices"
ToolSvc += ElectronLHSelectorMedium_v14

ElectronLHSelectorTight_v14 = AsgElectronLikelihoodTool("ElectronLHSelectorTight_v14", WorkingPoint="TightLHElectron_v14")
ElectronLHSelectorTight_v14.primaryVertexContainer = "PrimaryVertices"
ToolSvc += ElectronLHSelectorTight_v14


#DNN selectors
from ElectronPhotonSelectorTools.ElectronPhotonSelectorToolsConf import AsgElectronSelectorTool
ElectronDNNSelectorLoose = AsgElectronSelectorTool("ElectronDNNSelectorLoose",
                                                   WorkingPoint="LooseDNNElectron")
ToolSvc += ElectronDNNSelectorLoose

ElectronDNNSelectorMedium = AsgElectronSelectorTool("ElectronDNNSelectorMedium",
                                                    WorkingPoint="MediumDNNElectron")
ToolSvc += ElectronDNNSelectorMedium

ElectronDNNSelectorTight = AsgElectronSelectorTool("ElectronDNNSelectorTight",
                                                   WorkingPoint="TightDNNElectron")
ToolSvc += ElectronDNNSelectorTight

#====================================================================
# ELECTRON CHARGE SELECTION
#====================================================================
from ElectronPhotonSelectorTools.ElectronPhotonSelectorToolsConf import AsgElectronChargeIDSelectorTool
ElectronChargeIDSelector = AsgElectronChargeIDSelectorTool("ElectronChargeIDSelectorLoose")
ElectronChargeIDSelector.primaryVertexContainer = "PrimaryVertices"
ElectronChargeIDSelector.TrainingFile = "ElectronPhotonSelectorTools/ChargeID/ECIDS_20180731rel21Summer2018.root"
ToolSvc += ElectronChargeIDSelector

#====================================================================
# FWD ELECTRON LH SELECTORS
#====================================================================
from ElectronPhotonSelectorTools.ElectronPhotonSelectorToolsConf import AsgForwardElectronLikelihoodTool

ForwardElectronLHSelectorLoose = AsgForwardElectronLikelihoodTool("ForwardElectronLHSelectorLoose", WorkingPoint="LooseLHForwardElectron")
ToolSvc += ForwardElectronLHSelectorLoose

ForwardElectronLHSelectorMedium = AsgForwardElectronLikelihoodTool("ForwardElectronLHSelectorMedium", WorkingPoint="MediumLHForwardElectron")
ToolSvc += ForwardElectronLHSelectorMedium

ForwardElectronLHSelectorTight = AsgForwardElectronLikelihoodTool("ForwardElectronLHSelectorTight", WorkingPoint="TightLHForwardElectron")
ToolSvc += ForwardElectronLHSelectorTight


#====================================================================
# PHOTON SELECTION (loose and tight cut-based)
#====================================================================
from ROOT import egammaPID

from ElectronPhotonSelectorTools.ConfiguredAsgPhotonIsEMSelectors import ConfiguredAsgPhotonIsEMSelector
from ElectronPhotonSelectorTools.PhotonIsEMSelectorMapping import photonPIDmenu

# Loose
PhotonIsEMSelectorLoose = ConfiguredAsgPhotonIsEMSelector("PhotonIsEMSelectorLoose", egammaPID.PhotonIDLoose)
ToolSvc += PhotonIsEMSelectorLoose

# Tight (default == pt-dependent)
PhotonIsEMSelectorTight = ConfiguredAsgPhotonIsEMSelector("PhotonIsEMSelectorTight", egammaPID.PhotonIDTight)
ToolSvc += PhotonIsEMSelectorTight

# Tight (pt-inclusive)
# To be removed when pt-dependent menu above is supported with scale factors
PhotonIsEMSelectorTightPtIncl = ConfiguredAsgPhotonIsEMSelector("PhotonIsEMSelectorTightPtIncl", egammaPID.PhotonIDTight,menu=photonPIDmenu.menuPtInclJan2018)
ToolSvc += PhotonIsEMSelectorTightPtIncl



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
from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__EGSelectionToolWrapper
from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__EGElectronLikelihoodToolWrapper
# decorate electrons with the output of LH very loose
ElectronPassLHVeryLoose = DerivationFramework__EGSelectionToolWrapper( name = "ElectronPassLHVeryLoose",
                                                                       EGammaSelectionTool = ElectronLHSelectorVeryLoose,
                                                                       EGammaFudgeMCTool = "",
                                                                       CutType = "",
                                                                       StoreGateEntryName = "DFCommonElectronsLHVeryLoose",
                                                                       ContainerName = "Electrons")
ToolSvc += ElectronPassLHVeryLoose
print ElectronPassLHVeryLoose

# decorate electrons with the output of LH loose
ElectronPassLHLoose = DerivationFramework__EGSelectionToolWrapper( name = "ElectronPassLHLoose",
                                                                   EGammaSelectionTool = ElectronLHSelectorLoose,
                                                                   EGammaFudgeMCTool = "",
                                                                   CutType = "",
                                                                   StoreGateEntryName = "DFCommonElectronsLHLoose",
                                                                   ContainerName = "Electrons")
ToolSvc += ElectronPassLHLoose
print ElectronPassLHLoose

# decorate electrons with the output of LH loose+BL
ElectronPassLHLooseBL = DerivationFramework__EGSelectionToolWrapper( name = "ElectronPassLHLooseBL",
                                                                     EGammaSelectionTool = ElectronLHSelectorLooseBL,
                                                                     EGammaFudgeMCTool = "",
                                                                     CutType = "",
                                                                     StoreGateEntryName = "DFCommonElectronsLHLooseBL",
                                                                     ContainerName = "Electrons")
ToolSvc += ElectronPassLHLooseBL
print ElectronPassLHLooseBL

# decorate electrons with the output of LH medium
ElectronPassLHMedium = DerivationFramework__EGSelectionToolWrapper( name = "ElectronPassLHMedium",
                                                                    EGammaSelectionTool = ElectronLHSelectorMedium,
                                                                    EGammaFudgeMCTool = "",
                                                                    CutType = "",
                                                                    StoreGateEntryName = "DFCommonElectronsLHMedium",
                                                                    ContainerName = "Electrons")
ToolSvc += ElectronPassLHMedium
print ElectronPassLHMedium

# decorate electrons with the output of LH tight
ElectronPassLHTight = DerivationFramework__EGSelectionToolWrapper( name = "ElectronPassLHTight",
                                                                   EGammaSelectionTool = ElectronLHSelectorTight,
                                                                   EGammaFudgeMCTool = "",
                                                                   CutType = "",
                                                                   StoreGateEntryName = "DFCommonElectronsLHTight",
                                                                   ContainerName = "Electrons")
ToolSvc += ElectronPassLHTight
print ElectronPassLHTight

# decorate electrons with the output of LH very loose V14
ElectronPassLHVeryLoose_v14 = DerivationFramework__EGSelectionToolWrapper( name = "ElectronPassLHVeryLoose_v14",
																		   EGammaSelectionTool = ElectronLHSelectorVeryLoose_v14,
																		   EGammaFudgeMCTool = "",
																		   CutType = "",
																		   StoreGateEntryName = "DFCommonElectronsLHVeryLoose_v14",
																		   ContainerName = "Electrons")
ToolSvc += ElectronPassLHVeryLoose_v14
print ElectronPassLHVeryLoose_v14

# decorate electrons with the output of LH loose V14
ElectronPassLHLoose_v14 = DerivationFramework__EGSelectionToolWrapper( name = "ElectronPassLHLoose_v14",
																	   EGammaSelectionTool = ElectronLHSelectorLoose_v14,
																	   EGammaFudgeMCTool = "",
																	   CutType = "",
																	   StoreGateEntryName = "DFCommonElectronsLHLoose_v14",
																	   ContainerName = "Electrons")
ToolSvc += ElectronPassLHLoose_v14
print ElectronPassLHLoose_v14

# decorate electrons with the output of LH loose+BL V14
ElectronPassLHLooseBL_v14 = DerivationFramework__EGSelectionToolWrapper( name = "ElectronPassLHLooseBL_v14",
																		 EGammaSelectionTool = ElectronLHSelectorLooseBL_v14,
																		 EGammaFudgeMCTool = "",
																		 CutType = "",
																		 StoreGateEntryName = "DFCommonElectronsLHLooseBL_v14",
																		 ContainerName = "Electrons")
ToolSvc += ElectronPassLHLooseBL_v14
print ElectronPassLHLooseBL_v14

# decorate electrons with the output of LH medium V14
ElectronPassLHMedium_v14 = DerivationFramework__EGSelectionToolWrapper( name = "ElectronPassLHMedium_v14",
																		EGammaSelectionTool = ElectronLHSelectorMedium_v14,
																		EGammaFudgeMCTool = "",
																		CutType = "",
																		StoreGateEntryName = "DFCommonElectronsLHMedium_v14",
																		ContainerName = "Electrons")
ToolSvc += ElectronPassLHMedium_v14
print ElectronPassLHMedium_v14

# decorate electrons with the output of LH tight V14
ElectronPassLHTight_v14 = DerivationFramework__EGSelectionToolWrapper( name = "ElectronPassLHTight_v14",
																	   EGammaSelectionTool = ElectronLHSelectorTight_v14,
																	   EGammaFudgeMCTool = "",
																	   CutType = "",
																	   StoreGateEntryName = "DFCommonElectronsLHTight_v14",
																	   ContainerName = "Electrons")
ToolSvc += ElectronPassLHTight_v14
print ElectronPassLHTight_v14


# decorate electrons with the output of DNN ----------------------------------------------------------------------
ElectronPassDNNLoose = DerivationFramework__EGElectronLikelihoodToolWrapper( name = "ElectronPassDNNLoose",
                                                                             EGammaElectronLikelihoodTool = ElectronDNNSelectorLoose,
                                                                             EGammaFudgeMCTool = DF_ElectronPhotonShowerShapeFudgeTool if isFullSim else "",
                                                                             CutType = "",
                                                                             StoreGateEntryName = "DFCommonElectronsDNNLoose",
                                                                             ContainerName = "Electrons",
                                                                             StoreTResult = True,
                                                                             StoreGateEntryMultipleNames=["DFCommonElectronsDNN_pel",
                                                                                                          "DFCommonElectronsDNN_pcf",
                                                                                                          "DFCommonElectronsDNN_ppc",
                                                                                                          "DFCommonElectronsDNN_phf",
                                                                                                          "DFCommonElectronsDNN_ple",
                                                                                                          "DFCommonElectronsDNN_plh"])
ToolSvc += ElectronPassDNNLoose
print ElectronPassDNNLoose

# decorate electrons with the output of DNN ----------------------------------------------------------------------
ElectronPassDNNMedium = DerivationFramework__EGElectronLikelihoodToolWrapper( name = "ElectronPassDNNMedium",
                                                                              EGammaElectronLikelihoodTool = ElectronDNNSelectorMedium,
                                                                              EGammaFudgeMCTool = DF_ElectronPhotonShowerShapeFudgeTool if isFullSim else "",
                                                                              CutType = "",
                                                                              StoreGateEntryName = "DFCommonElectronsDNNMedium",
                                                                              ContainerName = "Electrons",
                                                                              StoreTResult = False)
ToolSvc += ElectronPassDNNMedium
print ElectronPassDNNMedium

# decorate electrons with the output of DNN ----------------------------------------------------------------------
ElectronPassDNNTight = DerivationFramework__EGElectronLikelihoodToolWrapper( name = "ElectronPassDNNTight",
                                                                             EGammaElectronLikelihoodTool = ElectronDNNSelectorTight,
                                                                             EGammaFudgeMCTool = DF_ElectronPhotonShowerShapeFudgeTool if isFullSim else "",
                                                                             CutType = "",
                                                                             StoreGateEntryName = "DFCommonElectronsDNNTight",
                                                                             ContainerName = "Electrons",
                                                                             StoreTResult = False)
ToolSvc += ElectronPassDNNTight
print ElectronPassDNNTight

# decorate electrons with the output of ECIDS ----------------------------------------------------------------------
ElectronPassECIDS = DerivationFramework__EGElectronLikelihoodToolWrapper( name = "ElectronPassECIDS",
                                                                          EGammaElectronLikelihoodTool = ElectronChargeIDSelector,
                                                                          EGammaFudgeMCTool = "",
                                                                          CutType = "",
                                                                          StoreGateEntryName = "DFCommonElectronsECIDS",

                                                                 ContainerName = "Electrons",
                                                                          StoreTResult = True)
ToolSvc += ElectronPassECIDS
print ElectronPassECIDS

# decorate forward electrons with the output of LH loose
ForwardElectronPassLHLoose = DerivationFramework__EGSelectionToolWrapper( name = "ForwardElectronPassLHLoose",
                                                                          EGammaSelectionTool = ForwardElectronLHSelectorLoose,
                                                                          EGammaFudgeMCTool = "",
                                                                          CutType = "",
                                                                          StoreGateEntryName = "DFCommonForwardElectronsLHLoose",
                                                                          ContainerName = "ForwardElectrons")
ToolSvc += ForwardElectronPassLHLoose
print ForwardElectronPassLHLoose

# decorate forward electrons with the output of LH medium
ForwardElectronPassLHMedium = DerivationFramework__EGSelectionToolWrapper( name = "ForwardElectronPassLHMedium",
                                                                          EGammaSelectionTool = ForwardElectronLHSelectorMedium,
                                                                          EGammaFudgeMCTool = "",
                                                                          CutType = "",
                                                                          StoreGateEntryName = "DFCommonForwardElectronsLHMedium",
                                                                          ContainerName = "ForwardElectrons")
ToolSvc += ForwardElectronPassLHMedium
print ForwardElectronPassLHMedium

# decorate forward electrons with the output of LH tight
ForwardElectronPassLHTight = DerivationFramework__EGSelectionToolWrapper( name = "ForwardElectronPassLHTight",
                                                                          EGammaSelectionTool = ForwardElectronLHSelectorTight,
                                                                          EGammaFudgeMCTool = "",
                                                                          CutType = "",
                                                                          StoreGateEntryName = "DFCommonForwardElectronsLHTight",
                                                                          ContainerName = "ForwardElectrons")
ToolSvc += ForwardElectronPassLHTight
print ForwardElectronPassLHTight


# decorate photons with the output of IsEM loose
# on MC, fudge the shower shapes before computing the ID (but the original shower shapes are not overridden)
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

# decorate photons with the output of IsEM tight pt-inclusive menu
# Can be removed once pt-dependent cuts are fully supported.
# On full-sim MC, fudge the shower shapes before computing the ID (but the original shower shapes are not overridden)
PhotonPassIsEMTightPtIncl = DerivationFramework__EGSelectionToolWrapper( name = "PhotonPassIsEMTightPtIncl",
                                                                         EGammaSelectionTool = PhotonIsEMSelectorTightPtIncl,
                                                                         EGammaFudgeMCTool = (DF_ElectronPhotonShowerShapeFudgeTool if isFullSim else None),
                                                                         CutType = "",
                                                                         StoreGateEntryName = "DFCommonPhotonsIsEMTightPtIncl",
                                                                         ContainerName = "Photons")
ToolSvc += PhotonPassIsEMTightPtIncl
print PhotonPassIsEMTightPtIncl

# decorate photons with the photon cleaning flags
# on MC, fudge the shower shapes before computing the flags
from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__EGPhotonCleaningWrapper
if isFullSim:
    PhotonPassCleaning = DerivationFramework__EGPhotonCleaningWrapper( name = "PhotonPassCleaning",
                                                                       EGammaFudgeMCTool = DF_ElectronPhotonShowerShapeFudgeTool,
                                                                       StoreGateEntryName = "DFCommonPhotonsCleaning",
                                                                       ContainerName = "Photons")
else:
    PhotonPassCleaning = DerivationFramework__EGPhotonCleaningWrapper( name = "PhotonPassCleaning",
                                                                       EGammaFudgeMCTool = None,
                                                                       StoreGateEntryName = "DFCommonPhotonsCleaning",
                                                                       ContainerName = "Photons")
ToolSvc += PhotonPassCleaning
print PhotonPassCleaning

# decorate central electrons and photons with a flag to tell the the candidates are affected by the crack bug in mc16a and data 2015+2016
from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__EGCrackVetoCleaningTool as DF_EGCVCT
PhotonPassCrackVeto = DF_EGCVCT(name               = "PhotonPassCrackVeto",
                                StoreGateEntryName = "DFCommonCrackVetoCleaning",
                                ContainerName      = "Photons")
ElectronPassCrackVeto = DF_EGCVCT(name               = "ElectronPassCrackVeto",
                                  StoreGateEntryName = "DFCommonCrackVetoCleaning",
                                  ContainerName      = "Electrons")
ToolSvc += [PhotonPassCrackVeto,ElectronPassCrackVeto]

# decorate some electrons with an additional ambiguity flag against internal and early material conversion
from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__EGElectronAmbiguityTool as DF_EGEAT
ElectronAmbiguity = DF_EGEAT(name               = "ElectronAdditionnalAmbiguity",
                             isMC               = (globalflags.DataSource()!='data'))
ToolSvc += ElectronAmbiguity

# list of all the decorators so far
EGAugmentationTools = [DFCommonPhotonsDirection,
                       ElectronPassLHVeryLoose, ElectronPassLHLoose, ElectronPassLHLooseBL, ElectronPassLHMedium, ElectronPassLHTight,
                       ElectronPassLHVeryLoose_v14, ElectronPassLHLoose_v14, ElectronPassLHLooseBL_v14, ElectronPassLHMedium_v14, ElectronPassLHTight_v14,
                       ForwardElectronPassLHLoose, ForwardElectronPassLHMedium, ForwardElectronPassLHTight,
                       ElectronPassDNNLoose, ElectronPassDNNMedium, ElectronPassDNNTight, ElectronPassECIDS,
                       PhotonPassIsEMLoose, PhotonPassIsEMTight,
                       PhotonPassIsEMTightPtIncl,
                       PhotonPassCleaning,
                       PhotonPassCrackVeto,ElectronPassCrackVeto,
                       ElectronAmbiguity]

#==================================================
# Truth Related tools
from RecExConfig.RecFlags  import rec
if  rec.doTruth():

    # Decorate Electron with bkg electron type/origin
    from MCTruthClassifier.MCTruthClassifierBase import MCTruthClassifier as BkgElectronMCTruthClassifier
    BkgElectronMCTruthClassifier.barcodeG4Shift=(DerivationFrameworkSimBarcodeOffset+1)

    from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__BkgElectronClassification
    BkgElectronClassificationTool = DerivationFramework__BkgElectronClassification (name = "BkgElectronClassificationTool",
                                                                                    MCTruthClassifierTool = BkgElectronMCTruthClassifier,
                                                                                    barcodeCut=DerivationFrameworkSimBarcodeOffset)


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
    from EventShapeTools.EventDensityConfig import configEventDensityTool, EventDensityAthAlg
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
    topSequence += EventDensityAthAlg("EDTruthCentralAlg", EventDensityTool = tc )
    topSequence += EventDensityAthAlg("EDTruthForwardAlg", EventDensityTool = tf )


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__CommonAugmentation
DerivationFrameworkJob += CfgMgr.DerivationFramework__CommonAugmentation("EGammaCommonKernel",
                                                                         AugmentationTools = EGAugmentationTools
                                                                         )

#=======================================
# ADD TOOLS
#=======================================

import IsolationAlgs.IsoUpdatedTrackCones as isoCones
if not hasattr(DerivationFrameworkJob,"IsolationBuilderTight1000"):
    DerivationFrameworkJob += isoCones.GetUpdatedIsoTrackCones()
