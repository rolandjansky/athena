#====================================================================
# DAOD_TRIG.py
# This defines DAOD_TRIG, which is a prototype for a possible new
# unskimmed format for Run 3
#=================
# This version is used to put together a trigger DAOD for the AMSG-R3 taskforce
#=================
# It requires the reductionConf flag TRIG in Reco_tf.py
# For help, see: https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/DerivationFramework
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkEGamma.ElectronsCPDetailedContent import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkCore.WeightMetadata import *
from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit
from DerivationFrameworkFlavourTag.HbbCommon import *
from TriggerMenu.api.TriggerAPI import TriggerAPI
from TriggerMenu.api.TriggerEnums import TriggerPeriod, TriggerType

from DerivationFrameworkCore.ThinningHelper import ThinningHelper
ThinningHelper = ThinningHelper( "TRIGThinningHelper" )
thinningTools = []
augmentationTools = []

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#====================================================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("TRIGKernel",
                                                                       AugmentationTools = augmentationTools,
                                                                       ThinningTools = thinningTools,
                                                                    )

#====================================================================
# TRIGGER CONTENT   
#====================================================================
# placeholder for trigger skimming, navigation thinning etc



#====================================================================
# THINNING OF TrackParticle CONTAINER
# Needed by trig tracking group
#====================================================================
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "MuonTPThinningTool",
                                                                         ThinningService         = ThinningHelper.ThinningSvc(),
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += MuonTPThinningTool
thinningTools.append(MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(	name                    = "ElectronTPThinningTool",
                                                                                ThinningService         = ThinningHelper.ThinningSvc(),
                                                                                SGKey             	= "Electrons",
                                                                                InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += ElectronTPThinningTool
thinningTools.append(ElectronTPThinningTool)

# TrackParticles associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name            = "TauTPThinningTool",
                                                                   ThinningService = ThinningHelper.ThinningSvc(),
                                                                   TauKey          = "TauJets",
                                                                   InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += TauTPThinningTool
thinningTools.append(TauTPThinningTool)



#====================================================================
# SKIMMING 
# current requirements set by trig tracking group
#====================================================================
#=========== TAUS =================
TauRequirements = '(TauJets.pt >= 25.*GeV) && (abs(TauJets.eta) <= 2.47)  && (TauJets.DFCommonTausMedium)' #'(TauJets.nTracks < 4)' 
TauObjectSelection = 'count('+TauRequirements+') >= 1'


#=========== MUONS =================
MuonRequirements = ' (abs(Muons.eta) < 2.6) && medium && (Muons.pt >= 4.*GeV) && (Muons.DFCommonMuonsPreselection)'
# note: DFCommonMuonsPreselection selects veryLoose muons, will leave this for now. Tracking requires Medium muons
MuonObjectSelection = 'count('+MuonRequirements+') >=1'

#=========== ELECTRONS =================
ElectronRequirements = '(Electrons.DFCommonElectronsLHMedium ||  Electrons.DFCommonElectronsLHTight) && (Electrons.pt >= 5.*GeV)'
ElectronObjectSelection = 'count('+ElectronRequirements+') >=1'



#=========== BJETS =================
BjetRequirements = ' >= 30.*GeV jets, eta < 2.5, 2 >= 30 GeV requirement on number of ROIs'
BjetObjectSelection = 'count('+BjetRequirements+') >=1'


#=========== PHOTONS =================
PhotonRequirements = ''
PhotonObjectSelection = 'count('+PhotonRequirements+') >=1'

#=========== JETS =================
JetRequirements = ''
JetObjectSelection = 'count('+JetRequirements+') >=1'

#============= Adding it all up =================
objectSelection = '(%s) || (%s) || (%s) || (%s)' % (TauObjectSelection, MuonObjectSelection, ElectronObjectSelection, BjetObjectSelection)


from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
TRIGSkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "TRIGSkimmingTool",
                                                                expression = objectSelection)
ToolSvc += TRIGSkimmingTool




#====================================================================
# Truth collections
#====================================================================
if DerivationFrameworkHasTruth:
   from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
   addStandardTruthContents()


#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_TRIGStream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_TRIGStream )
TRIGStream = MSMgr.NewPoolRootStream( streamName, fileName )
TRIGStream.AcceptAlgs(["TRIGKernel"])


#====================================================================
# EVENT CONTENTS - SLIMMING
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
TRIGSlimmingHelper = SlimmingHelper("TRIGSlimmingHelper")

TRIGSlimmingHelper.SmartCollections = ["Electrons",
                                       "Photons", 
                                       "Muons", 
                                       "PrimaryVertices", 
                                       "InDetTrackParticles",
                                       "AntiKt4EMTopoJets",
                                       "AntiKt4LCTopoJets", 
                                       "AntiKt4EMPFlowJets",
                                       "BTagging_AntiKt4EMTopo_201810",
                                       "MET_Reference_AntiKt4EMTopo",
                                       "MET_Reference_AntiKt4LCTopo",
                                       "MET_Reference_AntiKt4EMPFlow",
                                       "TauJets",
                                       "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",      
                                      ]


# Add all variables of a given collection
TRIGSlimmingHelper.AllVariables = []

# Additional variables, beyond those provided in the smart slimming
TRIGSlimmingHelper.ExtraVariables = ['InDetTrackParticlesAux.numberOfBLayerHits',
                                     'InDetTrackParticlesAux.expectBLayerHit',
                                     'InDetTrackParticlesAux.numberOfTRTHits',
                                     'InDetTrackParticlesAux.numberOfTRTOutliers',
                                  ]
   #"InDetTrackParticles.DFCommonCovarianceMatrix", # ID track covariance matrix



#====================================================================
# Trigger content slimming -> find files in DerivationFrameworkCore/python
# except for muons -> DerivationFrameworkMuon
#====================================================================
TRIGSlimmingHelper.IncludeAdditionalTriggerContent = True
TRIGSlimmingHelper.IncludeJetTriggerContent = True
TRIGSlimmingHelper.IncludeMuonTriggerContent = True
TRIGSlimmingHelper.IncludeEGammaTriggerContent = True
TRIGSlimmingHelper.IncludeJetTauEtMissTriggerContent = True
TRIGSlimmingHelper.IncludeJetTriggerContent = True
TRIGSlimmingHelper.IncludeTauTriggerContent = True
TRIGSlimmingHelper.IncludeEtMissTriggerContent = True
TRIGSlimmingHelper.IncludeBJetTriggerContent = True
TRIGSlimmingHelper.IncludeBPhysTriggerContent = True
TRIGSlimmingHelper.IncludeMinBiasTriggerContent = True

#====================================================================
# Add the jet containers to the stream (defined in JetCommon if import needed)
#====================================================================
#addJetOutputs(TRIGSlimmingHelper,["TRIG"])



#====================================================================
# Final construction of output stream
#====================================================================
TRIGSlimmingHelper.AppendContentToStream(TRIGStream)

