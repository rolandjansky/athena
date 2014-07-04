#********************************************************************
# EGAMMA1.py 
# reductionConf flag EGAMMA1 in Reco_tf.py   
# author: giovanni.marchiori@cern.ch                                                                                                                                                       
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkEGamma.EGammaCommon import *

#====================================================================
# Z->ee selection for electron probes with pT>20 GeV 
# (calib: medium e, OS; T&P: e, OS+SS)
# it would be nice if we could add a tag-bit to flag
# events passing the T&P selection, perhaps using an augmentation
# tool?
#====================================================================

requirement = 'DFCommonElectronsMedium && DFCommonElectrons_pt > 10*GeV'
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__InvariantMassTool
EGAMMA1MassTool = DerivationFramework__InvariantMassTool( name = "EGAMMA1MassTool",
                                                                ObjectRequirements = requirement,
                                                                StoreGateEntryName = "EGAMMA1DiElectronMass",
                                                                MassHypothesis = 0.511*MeV,
                                                                ContainerName = "ElectronCollection" )
ToolSvc += EGAMMA1MassTool

#requirement_tag = '(el_tightPP || isLLHElectron_Tight) && (recommendedEl_pt > 25*GeV) && abs(recommendedEl_eta)<2.5'
#requirement_probe = '(el_mediumPP || isLLHElectron_Medium) && (recommendedEl_pt > 20*GeV)'
#requirement_probe = '(recommendedEl_pt > 20*GeV)'

#from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__InvariantMassTool2
#EGAMMA_ZEE_EEMassTool = DerivationFramework__InvariantMassTool2( name = "EGAMMA_ZEE_EEMassTool",
#                                                                 Object1Requirements = requirement_tag,
#                                                                 Object2Requirements = requirement_probe,
#                                                                 StoreGateEntryName = "ZEE_DiElectronMass",
#                                                                 Mass1Hypothesis = 0.511*MeV,
#                                                                 Mass2Hypothesis = 0.511*MeV,
#                                                                 Pt1BranchName = "recommendedEl_pt",
#                                                                 Eta1BranchName = "recommendedEl_eta",
#                                                                 Phi1BranchName = "recommendedEl_phi",
#                                                                 Q1BranchName = "el_charge",
#                                                                 Pt2BranchName = "recommendedEl_pt",
#                                                                 Eta2BranchName = "recommendedEl_eta",
#                                                                 Phi2BranchName = "recommendedEl_phi",
#                                                                 Q2BranchName = "el_charge",
#                                                                 CheckCharge = False,
                                                                 #CheckCharge = True,
#                                                                 RemoveOverlaps = False)
#ToolSvc += EGAMMA_ZEE_EEMassTool


#====================================================================
# Z->ee selection for electron probes with 10<pT<20 GeV
#====================================================================
#requirement_tag = '(el_tightPP || isLLHElectron_Tight) && (recommendedEl_pt > 25*GeV) && abs(recommendedEl_eta)<2.5'
#requirement_probe = '(recommendedEl_pt > 10*GeV && recommendedEl_pt<20*GeV) && abs(recommendedEl_eta)<2.5'

#from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__InvariantMassTool2
#EGAMMA_ZEE_EEMassTool2 = DerivationFramework__InvariantMassTool2( name = "EGAMMA_ZEE_EEMassTool2",
#                                                                  Object1Requirements = requirement_tag,
#                                                                  Object2Requirements = requirement_probe,
#                                                                  StoreGateEntryName = "ZEE_DiElectronMass2",
#                                                                  Mass1Hypothesis = 0.511*MeV,
#                                                                  Mass2Hypothesis = 0.511*MeV,
#                                                                  Pt1BranchName = "recommendedEl_pt",
#                                                                  Eta1BranchName = "recommendedEl_eta",
#                                                                  Phi1BranchName = "recommendedEl_phi",
#                                                                  Q1BranchName = "el_charge",
#                                                                  Pt2BranchName = "recommendedEl_pt",
#                                                                  Eta2BranchName = "recommendedEl_eta",
#                                                                  Phi2BranchName = "recommendedEl_phi",
#                                                                  Q2BranchName = "el_charge",
#                                                                  CheckCharge = False,
#                                                                  RemoveOverlaps = False)
#ToolSvc += EGAMMA_ZEE_EEMassTool2


#====================================================================
# Z->eg selection for T&P
#====================================================================
#requirement_tag = '(el_tightPP || isLLHElectron_Tight) && (recommendedEl_pt > 25*GeV) && abs(recommendedEl_eta)<2.5'
#requirement_probe = '(ph_cl_E/cosh(ph_etas2) > 20*GeV)'

#from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__InvariantMassTool2
#EGAMMA_ZEE_EGMassTool = DerivationFramework__InvariantMassTool2( name = "EGAMMA_ZEE_EGMassTool",
#                                                                 Object1Requirements = requirement_tag,
#                                                                 Object2Requirements = requirement_probe,
#                                                                 StoreGateEntryName = "ZEE_ElectronGammaMass",
#                                                                 Mass1Hypothesis = 0.511*MeV,
#                                                                 Mass2Hypothesis = 0.511*MeV,
#                                                                 Pt1BranchName = "recommendedEl_pt",
#                                                                 Eta1BranchName = "recommendedEl_eta",
#                                                                 Phi1BranchName = "recommendedEl_phi",
#                                                                 Q1BranchName = "el_charge",
#                                                                 Pt2BranchName = "ph_cl_pt",
#                                                                 Eta2BranchName = "ph_cl_eta",
#                                                                 Phi2BranchName = "ph_cl_phi",
#                                                                 Q2BranchName = "",
#                                                                 CheckCharge = False,
#                                                                 RemoveOverlaps = False)
#ToolSvc += EGAMMA_ZEE_EGMassTool



#====================================================================
# Skimming criteria
#====================================================================
expression = 'count(EGAMMA1DiElectronMass > 60.0*GeV)>=1'
#expression = 'count(ZEE_DiElectronMass > 60.0*GeV)>=1 || count(ZEE_DiElectronMass2 > 60.0*GeV)>=1 || count(ZEE_ElectronGammaMass > 60.0*GeV)>=1'
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EGAMMA1SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "EGAMMA1SkimmingTool",
                                                                   expression = expression)
ToolSvc += EGAMMA1SkimmingTool
print EGAMMA1SkimmingTool


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("EGAMMA1Kernel",
                                                                       AugmentationTools = [EGAMMA1MassTool],
                                                                       #AugmentationTools = [EGAMMA_ZEE_EEMassTool, EGAMMA_ZEE_EEMassTool2, EGAMMA_ZEE_EGMassTool],
                                                                       SkimmingTools = [EGAMMA1SkimmingTool]
                                                                       )


#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_EGAMMA1Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EGAMMA1Stream )
EGAMMA1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
# Only events that pass the filters listed below are written out.
# Name must match that of the kernel above
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
EGAMMA1Stream.AcceptAlgs(["EGAMMA1Kernel"])

