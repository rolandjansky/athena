#====================================================================
# DAOD_PHYSVAL.py
# This defines DAOD_PHYSVAL, which produces a full set of 
# reconstructed object containers for the purposes of making physics 
# validation plots. In addition it makes a full copy of the AOD
# contents. No skimming, slimming or thinning is run.
# It requires the reductionConf flag PHYSVAL in Reco_tf.py   
#====================================================================

# Set up common services and job object. 
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("PHYSVALKernel")

#====================================================================
# JET/MET   
#====================================================================

OutputJets["PHYSVAL"] = []
reducedJetList = ["AntiKt2PV0TrackJets",
                  "AntiKt4PV0TrackJets",
                  "AntiKt4TruthJets"]
replaceAODReducedJets(reducedJetList,DerivationFrameworkJob,"PHYSVAL")

#====================================================================
# SET UP STREAM   
#====================================================================

# SKIMMING
# The base name (DAOD_PHYSVAL here) must match the string in 
# DerivationFrameworkProdFlags (in DerivationFrameworkCore) 
streamName = derivationFlags.WriteDAOD_PHYSVALStream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_PHYSVALStream )
PHYSVALStream = MSMgr.NewPoolRootStream( streamName, fileName )
# Only events that pass the filters listed below are written out.
# Name must match that of the kernel above
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
PHYSVALStream.AcceptAlgs(["PHYSVALKernel"])

#====================================================================
# CONTENTS   
#====================================================================
#from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput
#excludeList = []
#dpdOutput.addAllItemsFromInputExceptExcludeList( streamName, excludeList )
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
PHYSVALSlimmingHelper = SlimmingHelper("PHYSVALSlimmingHelper")
PHYSVALSlimmingHelper.AllVariables = [ "Electrons", "Photons", "Muons", "PrimaryVertices",
                                     "AntiKt4EMTopoJets","AntiKt4LCTopoJets","AntiKt4EMPFlowJets",
                                     "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                     "BTagging_AntiKt4EMTopo",
                                     "BTagging_AntiKt2Track","MuonTruthParticles", "egammaTruthParticles",
                                     "TruthParticles", "TruthEvents", "TruthVertices",
                                     "MuonSegments",
                                     "Kt4EMTopoOriginEventShape","Kt4LCTopoOriginEventShape","Kt4EMPFlowEventShape"
                                     ]

# Trigger content
PHYSVALSlimmingHelper.IncludeJetTriggerContent = True
PHYSVALSlimmingHelper.IncludeMuonTriggerContent = True
PHYSVALSlimmingHelper.IncludeEGammaTriggerContent = True
PHYSVALSlimmingHelper.IncludeJetTauEtMissTriggerContent = True
PHYSVALSlimmingHelper.IncludeJetTriggerContent = True
PHYSVALSlimmingHelper.IncludeTauTriggerContent = True
PHYSVALSlimmingHelper.IncludeEtMissTriggerContent = True
PHYSVALSlimmingHelper.IncludeBJetTriggerContent = True
PHYSVALSlimmingHelper.IncludeBPhysTriggerContent = True
PHYSVALSlimmingHelper.IncludeMinBiasTriggerContent = True

# Add the jet containers to the stream
addJetOutputs(PHYSVALSlimmingHelper,["SmallR","JETM1"],[], # smart list
              [
               "AntiKt4TruthWZJets",
               ]# veto list
              )

# Final construction of output stream
PHYSVALSlimmingHelper.AppendContentToStream(PHYSVALStream)

