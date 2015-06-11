#********************************************************************
# EGAM2.py
# J/psi->ee derivation for calibration
# reductionConf flag EGAM2 in Reco_tf.py
# author: giovanni.marchiori@cern.ch
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *

RecomputeElectronSelectors = True
#RecomputeElectronSelectors = False

#====================================================================
# SKIMMING TOOLS
#====================================================================

# SELECTION FOR CALIBRATION

#====================================================================
# J/psi->ee selection for e/gamma calibration
# 2 tight or medium e (depends on Run2 triggers..), pT>4.5 GeV, author=1 or 3, OS, 1<Mee<5 GeV
#====================================================================
# author requirement not needed in Run2: forward electrons are in separate collection
# and no tracking-based (soft-e) electron reconstruction is run
# electronAuthorRequirement = '(Electrons.author==1 || Electrons.author==3)'
electronPtRequirement = '(DFCommonElectrons_pt > 4.5*GeV)'
if RecomputeElectronSelectors :
    electronQualityRequirement = '(Electrons.DFCommonElectronsIsEMMedium || Electrons.DFCommonElectronsLHMedium)'
else :
    electronQualityRequirement = '(Electrons.Medium || Electrons.DFCommonElectronsLHMedium)'
#electronQualityRequirement='(Electrons.Tight || DFCommonElectronsLHTight)'
#requirement_el = '(' + electronQualityRequirement + '&&' + electronPtRequirement + '&&' + electronAuthorRequirement + ')'
requirement_el = '(' + electronQualityRequirement + '&&' + electronPtRequirement + ')'


from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__EGInvariantMassTool
EGAM2_JPSIEEMassTool = DerivationFramework__EGInvariantMassTool( name = "EGAM2_JPSIEEMassTool",
                                                                 Object1Requirements = requirement_el,
                                                                 Object2Requirements = requirement_el,
                                                                 StoreGateEntryName = "EGAM2_DiElectronMass",
                                                                 Mass1Hypothesis = 0.511*MeV,
                                                                 Mass2Hypothesis = 0.511*MeV,
                                                                 Container1Name = "Electrons",
                                                                 Container2Name = "Electrons",
                                                                 Pt1BranchName = "DFCommonElectrons_pt",
                                                                 Eta1BranchName = "DFCommonElectrons_eta",
                                                                 Phi1BranchName = "DFCommonElectrons_phi",
                                                                 Pt2BranchName = "DFCommonElectrons_pt",
                                                                 Eta2BranchName = "DFCommonElectrons_eta",
                                                                 Phi2BranchName = "DFCommonElectrons_phi",
                                                                 CheckCharge = True,
                                                                 DoTransverseMass = False,
                                                                 MinDeltaR = 0.0)
ToolSvc += EGAM2_JPSIEEMassTool
print EGAM2_JPSIEEMassTool

# SELECTION FOR T&P

#====================================================================
# Jpsi->ee selection based on Jpsi->e+cluster trigger, for low pT (7-20 GeV) central photons
# Tag: 1 tight e, central, pT>5 GeV
# Probe: 1 e, central, pT>5 GeV
# OS+SS
# 1<mee<6 GeV
# dR>0.15
#====================================================================
if RecomputeElectronSelectors :
    requirement_el_tag = '(Electrons.DFCommonElectronsIsEMTight || Electrons.DFCommonElectronsLHTight) && DFCommonElectrons_pt > 4.5*GeV'
else :
    requirement_el_tag = '(Electrons.Tight || Electrons.DFCommonElectronsLHTight) && DFCommonElectrons_pt > 4.5*GeV'
requirement_el_probe = 'DFCommonElectrons_pt > 4.5*GeV'

EGAM2_JPSIEEMassTool2 = DerivationFramework__EGInvariantMassTool( name = "EGAM2_JPSIEEMassTool2",
                                                                  Object1Requirements = requirement_el_tag,
                                                                  Object2Requirements = requirement_el_probe,
                                                                  StoreGateEntryName = "EGAM2_DiElectronMass2",
                                                                  Mass1Hypothesis = 0.511*MeV,
                                                                  Mass2Hypothesis = 0.511*MeV,
                                                                  Container1Name = "Electrons",
                                                                  Container2Name = "Electrons",
                                                                  Pt1BranchName = "DFCommonElectrons_pt",
                                                                  Eta1BranchName = "DFCommonElectrons_eta",
                                                                  Phi1BranchName = "DFCommonElectrons_phi",
                                                                  Pt2BranchName = "DFCommonElectrons_pt",
                                                                  Eta2BranchName = "DFCommonElectrons_eta",
                                                                  Phi2BranchName = "DFCommonElectrons_phi",
                                                                  CheckCharge = False,
                                                                  DoTransverseMass = False,
                                                                  MinDeltaR = 0.15)
ToolSvc += EGAM2_JPSIEEMassTool2
print EGAM2_JPSIEEMassTool2


# Skimming criteria
expression_calib = '(count(EGAM2_DiElectronMass > 1.0*GeV && EGAM2_DiElectronMass < 5.0*GeV)>=1)'
expression_TP = '(count(EGAM2_DiElectronMass2 > 1.0*GeV && EGAM2_DiElectronMass2 < 6.0*GeV)>=1)'
expression = expression_calib + ' || ' + expression_TP
#expression = expression_calib
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EGAM2SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "EGAM2SkimmingTool",
                                                                 expression = expression)
ToolSvc += EGAM2SkimmingTool
print "EGAM2 skimming tool:", EGAM2SkimmingTool


#================
# THINNING
#================
thinningTools=[]
# TO BE ADDED

#====================================================================
# Cell sum decoration tool
#====================================================================

from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__CellDecorator
EGAM2_CellDecoratorTool = DerivationFramework__CellDecorator( name                    = "EGAM2_CellDecoratorTool",
                                                              SGKey_electrons         = "Electrons",
                                                              SGKey_photons           = "Photons",
                                                              CaloFillRectangularTool_5x5  = EGAMCOM_caloFillRect55,
                                                              CaloFillRectangularTool_3x5  = EGAMCOM_caloFillRect35,
                                                              CaloFillRectangularTool_3x7  = EGAMCOM_caloFillRect37,
                                                              CaloFillRectangularTool_7x11  = EGAMCOM_caloFillRect711
                                                              )
ToolSvc += EGAM2_CellDecoratorTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("EGAM2Kernel",
                                                                       AugmentationTools = [EGAM2_JPSIEEMassTool,EGAM2_JPSIEEMassTool2,EGAM2_CellDecoratorTool],
                                                                       SkimmingTools = [EGAM2SkimmingTool]
                                                                       )


#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_EGAM2Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EGAM2Stream )
EGAM2Stream = MSMgr.NewPoolRootStream( streamName, fileName )
# Only events that pass the filters listed below are written out.
# Name must match that of the kernel above
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
EGAM2Stream.AcceptAlgs(["EGAM2Kernel"])


#Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
# from AthenaServices.Configurables import ThinningSvc, createThinningSvc
# augStream = MSMgr.GetStream( streamName )
# evtStream = augStream.GetEventStream()
# svcMgr += createThinningSvc( svcName="EGAM2ThinningSvc", outStreams=[evtStream] )


#====================================================================
# CONTENT LIST  
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
EGAM2SlimmingHelper = SlimmingHelper("EGAM2SlimmingHelper")

from DerivationFrameworkEGamma.EGAM2ExtraContent import *
EGAM2SlimmingHelper.SmartCollections = [
				        #"Electrons",
                                        "Photons",
					"Muons",
                                        "TauJets",
                                        "MET_Reference_AntiKt4EMTopo",
                                        "AntiKt4EMTopoJets",
                                        "BTagging_AntiKt4EMTopo",
                                        "InDetTrackParticles",
                                        "PrimaryVertices" ]
# muons, tau, MET, b-tagging could be switched off if not needed and use too much space

EGAM2SlimmingHelper.ExtraVariables = ExtraContentAll
EGAM2SlimmingHelper.AllVariables = ExtraContainersElectrons

if globalflags.DataSource()=='geant4':
    EGAM2SlimmingHelper.ExtraVariables += ExtraContentAllTruth
    EGAM2SlimmingHelper.AllVariables += ExtraContainersTruth

EGAM2SlimmingHelper.AppendContentToStream(EGAM2Stream)

# Add EventShape info 
EGAM2Stream.AddItem("xAOD::EventShape#*")
EGAM2Stream.AddItem("xAOD::EventShapeAuxInfo#*")

# Add MET_RefFinalFix
# JRC: COMMENTED TEMPORARILY
#addMETOutputs(EGAM2Stream)

# Add egamma trigger objects
EGAM2Stream.IncludeEGammaTriggerContent = True
