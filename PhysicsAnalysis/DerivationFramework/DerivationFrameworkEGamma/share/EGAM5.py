#********************************************************************
# EGAM5.py
# W->enu reduction
# reductionConf flag EGAM5 in Reco_tf.py
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


#====================================================================
# W->enu selection for electron calibration
#====================================================================

# could add track isolation (if included in single electron trigger..)
#requirement_el = 'DFCommonElectronsLHTight && DFCommonElectrons_pt > 24.5*GeV'
if RecomputeElectronSelectors :
    requirement_el = '(Electrons.DFCommonElectronsIsEMTight || Electrons.DFCommonElectronsLHTight) && DFCommonElectrons_pt > 24.5*GeV'
else :
    requirement_el = '(Electrons.Tight || Electrons.DFCommonElectronsLHTight) && DFCommonElectrons_pt > 24.5*GeV'

from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__EGTransverseMassTool
EGAM5_MTTool = DerivationFramework__EGTransverseMassTool( name = "EGAM5_MTTool",
                                                          ObjectRequirements = requirement_el,
                                                          METmin = 25*GeV,
                                                          StoreGateEntryName = "WENU_TransverseMass",
                                                          ObjectMassHypothesis = 0.511*MeV,
                                                          ObjectContainerName = "Electrons",
                                                          METContainerName = "MET_LocHadTopo",
                                                          ObjectPtBranchName = "DFCommonElectrons_pt",
                                                          ObjectPhiBranchName = "DFCommonElectrons_eta"
                                                          )
ToolSvc += EGAM5_MTTool
print EGAM5_MTTool

# Skimming criteria
expression = 'count(WENU_TransverseMass>40*GeV)>=1'
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EGAM5SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "EGAM5SkimmingTool",
                                                                 expression = expression)
ToolSvc += EGAM5SkimmingTool
print "EGAM5 skimming tool:", EGAM5SkimmingTool

#====================================================================
# Cell sum decoration tool
#====================================================================

from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__CellDecorator
EGAM5_CellDecoratorTool = DerivationFramework__CellDecorator( name                    = "EGAM5_CellDecoratorTool",
                                                              SGKey_electrons         = "Electrons",
                                                              SGKey_photons           = "Photons",
                                                              CaloFillRectangularTool_5x5  = EGAMCOM_caloFillRect55,
                                                              CaloFillRectangularTool_3x5  = EGAMCOM_caloFillRect35,
                                                              CaloFillRectangularTool_3x7  = EGAMCOM_caloFillRect37,
                                                              CaloFillRectangularTool_7x11  = EGAMCOM_caloFillRect711
                                                              )
ToolSvc += EGAM5_CellDecoratorTool

#====================================================================                                                                                                    
# Max Cell sum decoration tool                                                                                                                                                  
#====================================================================                                                                                                           

from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__MaxCellDecorator
EGAM5_MaxCellDecoratorTool = DerivationFramework__MaxCellDecorator( name                    = "EGAM5_MaxCellDecoratorTool",
                                                                    SGKey_electrons         = "Electrons",
                                                                    SGKey_photons           = "Photons",
                                                                    )
ToolSvc += EGAM5_MaxCellDecoratorTool


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("EGAM5Kernel",
                                                                       AugmentationTools = [EGAM5_MTTool,EGAM5_CellDecoratorTool,EGAM5_MaxCellDecoratorTool],
                                                                       SkimmingTools = [EGAM5SkimmingTool]
                                                                       )


#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_EGAM5Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EGAM5Stream )
EGAM5Stream = MSMgr.NewPoolRootStream( streamName, fileName )
# Only events that pass the filters listed below are written out.
# Name must match that of the kernel above
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
EGAM5Stream.AcceptAlgs(["EGAM5Kernel"])


#====================================================================
# CONTENT LIST  
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
EGAM5SlimmingHelper = SlimmingHelper("EGAM5SlimmingHelper")

from DerivationFrameworkEGamma.EGAM5ExtraContent import *
EGAM5SlimmingHelper.SmartCollections = [#"Electrons",
                                        "Photons",
                                        "Muons",
                                        "TauJets",
                                        "MET_Reference_AntiKt4EMTopo",
                                        "AntiKt4EMTopoJets",
                                        "BTagging_AntiKt4EMTopo",
                                        "InDetTrackParticles",
                                        "PrimaryVertices" ]


EGAM5SlimmingHelper.ExtraVariables = ExtraContentAll
EGAM5SlimmingHelper.AllVariables = ExtraContainersElectrons

if globalflags.DataSource()=='geant4':
    EGAM5SlimmingHelper.ExtraVariables += ExtraContentAllTruth
    EGAM5SlimmingHelper.AllVariables += ExtraContainersTruth

EGAM5SlimmingHelper.AppendContentToStream(EGAM5Stream)

# Add EventShape
EGAM5Stream.AddItem("xAOD::EventShape#*")
EGAM5Stream.AddItem("xAOD::EventShapeAuxInfo#*")

# Add MET_RefFinalFix
# JRC: COMMENTED TEMPORARILY
#addMETOutputs(EGAM5Stream)

# Add egamma trigger objects
EGAM5Stream.IncludeEGammaTriggerContent = True
