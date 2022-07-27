#====================================================================
# JETM15.py
# reductionConf flag JETM15 in Reco_tf.py
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkIsMonteCarlo,DerivationFrameworkJob, buildFileName
from DerivationFrameworkPhys import PhysCommon
from DerivationFrameworkJetEtMiss.JetCommon import addJetOutputs, addOriginCorrectedClusters

#====================================================================
# SET UP STREAM
#====================================================================
streamName = derivationFlags.WriteDAOD_JETM15Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_JETM15Stream )
JETM15Stream = MSMgr.NewPoolRootStream( streamName, fileName )
JETM15Stream.AcceptAlgs(["JETM15MainKernel"])
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()

if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth import MCTruthCommon
  MCTruthCommon.addBosonsAndDownstreamParticles(generations=4,rejectHadronChildren=True)
  MCTruthCommon.addTopQuarkAndDownstreamParticles(generations=4,rejectHadronChildren=True)

#====================================================================
# SKIMMING TOOL
#====================================================================

from DerivationFrameworkJetEtMiss import TriggerLists
muonTriggers = TriggerLists.single_mu_Trig()
jetTriggers = TriggerLists.jetTrig()

# Applying a pT cut of 300 GeV on ungroomed jets, which should be sufficient for the grooming scan studies
jetSelection = '((count( AntiKt10LCTopoJets.pt > 300.*GeV && abs(AntiKt10LCTopoJets.eta) < 2.5 ) >=1) ||  \
                 (count( AntiKt10EMPFlowCSSKJets.pt > 300.*GeV && abs(AntiKt10EMPFlowCSSKJets.eta) < 2.5 ) >=1)  ||  \
                 (count( AntiKt10UFOCHSJets.pt > 300.*GeV && abs(AntiKt10UFOCHSJets.eta) < 2.5 ) >=1) || \
                 (count( AntiKt10UFOCSSKJets.pt > 300.*GeV && abs(AntiKt10UFOCSSKJets.eta) < 2.5 ) >=1) \
                )' 

orstr  = ' || '
andstr = ' && '

mutrigsel = '(EventInfo.eventTypeBitmask==1) || '+orstr.join(muonTriggers)
muofflinesel = andstr.join(['count((Muons.pt > 20*GeV) && (Muons.DFCommonMuonPassPreselection)) >= 1',
                            jetSelection])
muonSelection = ' ( (' + mutrigsel + ') && (' + muofflinesel + ') ) '

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
JETM15TrigSkimmingTool = DerivationFramework__TriggerSkimmingTool(   name           = "JETM15TrigSkimmingTool",
                                                                    TriggerListOR  = jetTriggers+muonTriggers )
ToolSvc += JETM15TrigSkimmingTool

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
JETM15OfflineSkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "JETM15OfflineSkimmingTool",
                                                                        expression = jetSelection + ' || ' +muonSelection)
ToolSvc += JETM15OfflineSkimmingTool

#Trigger matching decorations
from DerivationFrameworkCore.TriggerMatchingAugmentation import applyTriggerMatching
TrigMatchAug, NewTrigVars = applyTriggerMatching(ToolNamePrefix="JETM15",
                                   ElectronTriggers=[],
                                   MuonTriggers=muonTriggers,
                                   PhotonTriggers=[])

#====================================================================
# THINNING TOOLS
#====================================================================
thinningTools = []

#Track particles associated to any of the large-R jet collections
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
JETM15Akt10JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "JETM15Akt10JetLCTopoTPThinningTool",
                                                                        StreamName              = streamName,
                                                                        JetKey                  = "AntiKt10LCTopoJets",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM15Akt10JetTPThinningTool
thinningTools.append(JETM15Akt10JetTPThinningTool)

JETM15Akt10JetTPThinningToolPFO = DerivationFramework__JetTrackParticleThinning( name          = "JETM15Akt10CSSKPFOJetTPThinningTool",
                                                                        StreamName              = streamName,
                                                                        JetKey                  = "AntiKt10EMPFlowCSSKJets",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM15Akt10JetTPThinningToolPFO
thinningTools.append(JETM15Akt10JetTPThinningToolPFO)

JETM15Akt10JetEMUFOThinningToolTCC = DerivationFramework__JetTrackParticleThinning( name          = "JETM15Akt10JetCHSUFOThinningTool",
                                                                        StreamName              = streamName,
                                                                        JetKey                  = "AntiKt10UFOCHSJets",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM15Akt10JetEMUFOThinningToolTCC
thinningTools.append(JETM15Akt10JetEMUFOThinningToolTCC)

JETM15Akt10JetCSSKUFOThinningToolTCC = DerivationFramework__JetTrackParticleThinning( name          = "JETM15Akt10JetCSSKUFOThinningTool",
                                                                        StreamName              = streamName,
                                                                        JetKey                  = "AntiKt10UFOCSSKJets",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM15Akt10JetCSSKUFOThinningToolTCC
thinningTools.append(JETM15Akt10JetCSSKUFOThinningToolTCC)

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
JETM15MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name     = "JETM15MuonTPThinningTool",
                                                                         StreamName              = streamName,
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM15MuonTPThinningTool
thinningTools.append(JETM15MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
JETM15ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "JETM15ElectronTPThinningTool",
                                                                               StreamName              = streamName,
                                                                               SGKey                   = "Electrons",
                                                                               InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM15ElectronTPThinningTool
thinningTools.append(JETM15ElectronTPThinningTool)


from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__JetCaloClusterThinning

JETM15AKt10CCThinningTool = DerivationFramework__JetCaloClusterThinning(name                = "JETM15AKt10CCThinningTool",
                                                                      StreamName             = streamName,
                                                                      SGKey                 = "AntiKt10LCTopoJets",
                                                                      TopoClCollectionSGKey = "CaloCalTopoClusters",
                                                                      SelectionString       = "(AntiKt10LCTopoJets.pt > 300*GeV && abs(AntiKt10LCTopoJets.eta) < 2.8)",
                                                                      AdditionalClustersKey = ["LCOriginTopoClusters", "LCOriginCSSKTopoClusters"])
ToolSvc += JETM15AKt10CCThinningTool
thinningTools.append(JETM15AKt10CCThinningTool)


from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__UFOTrackParticleThinning
JETM15EMUFOTPThinningTool = DerivationFramework__UFOTrackParticleThinning(name                      = "JETM15EMUFOTPThinningTool",
                                                                          StreamName                   = streamName,
                                                                          JetKey                       = "AntiKt10UFOCHSJets",
                                                                          UFOKey                       = "CHSUFO",
                                                                          InDetTrackParticlesKey       = "InDetTrackParticles",
                                                                          PFOCollectionSGKey       = "JetETMiss",
                                                                          AdditionalPFOKey  = ["CSSK", "CHS"])
ToolSvc += JETM15EMUFOTPThinningTool
thinningTools.append(JETM15EMUFOTPThinningTool)

JETM15EMCSSKUFOTPThinningTool = DerivationFramework__UFOTrackParticleThinning(name                  = "JETM15CSSKUFOTPThinningTool",
                                                                              StreamName                   = streamName,
                                                                              JetKey                       = "AntiKt10UFOCSSKJets",
                                                                              UFOKey                       = "CSSKUFO",
                                                                              InDetTrackParticlesKey       = "InDetTrackParticles",
                                                                              PFOCollectionSGKey       = "JetETMiss",
                                                                              AdditionalPFOKey  = ["CSSK", "CHS"])
ToolSvc += JETM15EMCSSKUFOTPThinningTool
thinningTools.append(JETM15EMCSSKUFOTPThinningTool)


from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__JetPFlowThinning
JETM15AKt10PFOThinningTool = DerivationFramework__JetPFlowThinning(name                = "JETM15Ak10PFlowThinningTool",
                                                                   StreamName            = streamName,
                                                                   SGKey                 = "AntiKt10EMPFlowCSSKJets",
                                                                   PFOCollectionSGKey    = "JetETMiss",
                                                                   SelectionString       = "AntiKt10EMPFlowCSSKJets.pt > 300*GeV && abs(AntiKt10EMPFlowCSSKJets.eta) < 2.8",
                                                                   AdditionalPFOKey      = ["CSSK", "CHS"],
                                                                 )
ToolSvc += JETM15AKt10PFOThinningTool
thinningTools.append(JETM15AKt10PFOThinningTool)


#=======================================
# CREATE PRIVATE SEQUENCE
#=======================================

jetm15Seq = CfgMgr.AthSequencer("JETM15Sequence")
DerivationFrameworkJob += jetm15Seq

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
jetm15Seq += CfgMgr.DerivationFramework__DerivationKernel(	name = "JETM15TrigSkimKernel",
                                                            AugmentationTools = [] ,
                                                            SkimmingTools = [JETM15TrigSkimmingTool],
                                                            ThinningTools = [])


#=======================================
# BUILD SPECIAL JETS
#=======================================


from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
jetm15Seq += CfgMgr.DerivationFramework__DerivationKernel( name = "JETM15MainKernel",
                                                          AugmentationTools = [TrigMatchAug] ,
                                                          SkimmingTools = [JETM15OfflineSkimmingTool],
                                                          ThinningTools = thinningTools)


# Truth particle thinning
doTruthThinning = True
preserveAllDescendants = False

if DerivationFrameworkIsMonteCarlo:

  from DerivationFrameworkCore.ThinningHelper import ThinningHelper
  JETM15ThinningHelper = ThinningHelper( "JETM15ThinningHelper" )
  JETM15ThinningHelper.AppendToStream( JETM15Stream )
  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
  TruthThinningTool = DerivationFramework__MenuTruthThinning(name               = "JETM15TruthThinning",
                                                             StreamName         = streamName,
                                                             WriteAllStable     = True,
                                                             WritePartons       = False,
                                                             WriteHadrons       = False,
                                                             WriteBHadrons      = True,
                                                             WriteCHadrons      = False,
                                                             WriteGeant         = False,
                                                             WriteFirstN        = -1)
  ToolSvc += TruthThinningTool
  thinningTools.append(TruthThinningTool)

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper

JETM15SlimmingHelper = SlimmingHelper("JETM15SlimmingHelper")
JETM15SlimmingHelper.AppendToDictionary = {
  "Kt4EMPFlowNeutEventShape": "xAOD::EventShape",
  "Kt4EMPFlowNeutEventShapeAux": "xAOD::AuxInfoBase",
  "Kt4EMPFlowPUSBEventShape": "xAOD::EventShape",
  "Kt4EMPFlowPUSBEventShapeAux": "xAOD::AuxInfoBase",
  "AntiKt10LCTopoCSSKJets"   :   "xAOD::JetContainer"        ,
  "AntiKt10LCTopoCSSKJetsAux.":   "xAOD::JetAuxContainer"        ,
  "AntiKt10LCTopoJets"   :   "xAOD::JetContainer"        ,
  "AntiKt10LCTopoJetsAux.":   "xAOD::JetAuxContainer"        
}

JETM15SlimmingHelper.SmartCollections = [
  "EventInfo",
  "Electrons", 
  "Muons",
  "InDetTrackParticles",
  "PrimaryVertices",
  "MET_Baseline_AntiKt4EMTopo",
  "MET_Baseline_AntiKt4EMPFlow",
  "AntiKt4EMTopoJets",
  "AntiKt4EMPFlowJets",
  "BTagging_AntiKt4EMPFlow",
  "BTagging_AntiKtVR30Rmax4Rmin02Track",
  "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"
]

JETM15SlimmingHelper.AllVariables = [
  "Kt4EMTopoOriginEventShape",
  "Kt4LCTopoOriginEventShape",
  "Kt4EMPFlowEventShape", 
  "Kt4EMPFlowPUSBEventShape",
  "Kt4EMPFlowNeutEventShape",
  "TruthParticles", 
  "TruthEvents", 
  "TruthVertices",
]

#Do we need small-R truth jets?
if DerivationFrameworkIsMonteCarlo:
  JETM15SlimmingHelper.AppendToDictionary["AntiKt10TruthJets"]="xAOD::JetContainer"
  JETM15SlimmingHelper.AppendToDictionary["AntiKt10TruthJetsAux"]="xAOD::JetAuxContainer"
  JETM15SlimmingHelper.AllVariables  += ["AntiKt10TruthJets"]
  
addOriginCorrectedClusters(JETM15SlimmingHelper,writeLC=True,writeEM=True)

JETM15SlimmingHelper.AppendToDictionary["CaloCalTopoClusters"]='xAOD::CaloClusterContainer'
JETM15SlimmingHelper.AppendToDictionary["CaloCalTopoClustersAux"]='xAOD::CaloClusterAuxContainer'
JETM15SlimmingHelper.ExtraVariables.append('CaloCalTopoClusters.calEta.calPhi.calE.calM.rawEta.rawPhi.rawE.rawM')

JETM15SlimmingHelper.AppendToDictionary["LCOriginTopoClusters"]='xAOD::CaloClusterContainer'
JETM15SlimmingHelper.AppendToDictionary["LCOriginTopoClustersAux"]='xAOD::ShallowAuxContainer'

JETM15SlimmingHelper.AppendToDictionary["LCOriginCSSKTopoClusters"]='xAOD::CaloClusterContainer'
JETM15SlimmingHelper.AppendToDictionary["LCOriginCSSKTopoClustersAux"]='xAOD::ShallowAuxContainer'

JETM15SlimmingHelper.AppendToDictionary["JetETMissNeutralParticleFlowObjects"]='xAOD::PFOContainer'
JETM15SlimmingHelper.AppendToDictionary["JetETMissNeutralParticleFlowObjectsAux"]='xAOD::PFOAuxContainer'
JETM15SlimmingHelper.ExtraVariables.append('JetETMissNeutralParticleFlowObjects.pt.eta.phi.m.e.charge')

JETM15SlimmingHelper.AppendToDictionary["JetETMissChargedParticleFlowObjects"]='xAOD::PFOContainer'
JETM15SlimmingHelper.AppendToDictionary["JetETMissChargedParticleFlowObjectsAux"]='xAOD::PFOAuxContainer'
JETM15SlimmingHelper.ExtraVariables.append('JetETMissChargedParticleFlowObjects.pt.eta.phi.m.e.charge.matchedToPV')

JETM15SlimmingHelper.AppendToDictionary["CSSKUFO"] = 'xAOD::TrackCaloClusterContainer'
JETM15SlimmingHelper.AppendToDictionary['CSSKUFOAux'] = 'xAOD::TrackCaloClusterAuxContainer'
JETM15SlimmingHelper.ExtraVariables +=[ 'CSSKUFO.pt.eta.phi.m.taste' ]

JETM15SlimmingHelper.AppendToDictionary["CHSUFO"] = 'xAOD::TrackCaloClusterContainer'
JETM15SlimmingHelper.AppendToDictionary['CHSUFOAux'] = 'xAOD::TrackCaloClusterAuxContainer'
JETM15SlimmingHelper.ExtraVariables +=[ 'CHSUFO.pt.eta.phi.m.taste' ]


JETM15SlimmingHelper.AppendToDictionary["CSSKChargedParticleFlowObjects"]='xAOD::PFOContainer'
JETM15SlimmingHelper.AppendToDictionary["CSSKChargedParticleFlowObjectsAux"]='xAOD::ShallowAuxContainer'
JETM15SlimmingHelper.ExtraVariables.append('CSSKChargedParticleFlowObjects.pt.eta.phi.m.e.charge.matchedToPV')

JETM15SlimmingHelper.AppendToDictionary["CSSKNeutralParticleFlowObjects"]='xAOD::PFOContainer'
JETM15SlimmingHelper.AppendToDictionary["CSSKNeutralParticleFlowObjectsAux"]='xAOD::ShallowAuxContainer'
JETM15SlimmingHelper.ExtraVariables.append('CSSKNeutralParticleFlowObjects.pt.eta.phi.m.e.charge')

JETM15SlimmingHelper.AppendToDictionary["CHSGChargedParticleFlowObjects"]='xAOD::PFOContainer'
JETM15SlimmingHelper.AppendToDictionary["CHSGChargedParticleFlowObjectsAux"]='xAOD::ShallowAuxContainer'
JETM15SlimmingHelper.ExtraVariables.append('CHSGChargedParticleFlowObjects.pt.eta.phi.m.e.charge.matchedToPV')

JETM15SlimmingHelper.AppendToDictionary["CHSGNeutralParticleFlowObjects"]='xAOD::PFOContainer'
JETM15SlimmingHelper.AppendToDictionary["CHSGNeutralParticleFlowObjectsAux"]='xAOD::ShallowAuxContainer'
JETM15SlimmingHelper.ExtraVariables.append('CHSGNeutralParticleFlowObjects.pt.eta.phi.m.e.charge')

JETM15SlimmingHelper.AllVariables  += ["LCOriginTopoClusters", "LCOriginCSSKTopoClusters", "CSSKNeutralParticleFlowObjects", "CSSKChargedParticleFlowObjects", "TrackCaloClustersCombinedAndNeutral", "CHSGChargedParticleFlowObjects", "CHSGNeutralParticleFlowObjects"]

# Check if we need truth muon collection
for truthc in [
  "TruthElectrons",
  "TruthMuons",
  ]:
  JETM15SlimmingHelper.StaticContent.append("xAOD::TruthParticleContainer#"+truthc)
  JETM15SlimmingHelper.StaticContent.append("xAOD::TruthParticleAuxContainer#"+truthc+"Aux.")

for truthc in [
  "TruthTopQuark",
  "TruthBosons"
  ]:
  JETM15SlimmingHelper.StaticContent.append("xAOD::TruthParticleContainer#"+truthc+"WithDecayParticles")
  JETM15SlimmingHelper.StaticContent.append("xAOD::TruthParticleAuxContainer#"+truthc+"WithDecayParticlesAux.")
  JETM15SlimmingHelper.StaticContent.append("xAOD::TruthVertexContainer#"+truthc+"WithDecayVertices")
  JETM15SlimmingHelper.StaticContent.append("xAOD::TruthVertexAuxContainer#"+truthc+"WithDecayVerticesAux.")

# Add the jet containers to the stream
# explicitely add the container we want :
addJetOutputs(JETM15SlimmingHelper,[
        "AntiKt10LCTopoJets",    "AntiKt10LCTopoCSSKJets",  "AntiKt10EMPFlowCSSKJets",  "AntiKt10TrackCaloClusterJets", "AntiKt10UFOCHSJets", "AntiKt10UFOCSSKJets",
        "JETM15", # jets defined in this file
        ])


JETM15SlimmingHelper.ExtraVariables += [
    NewTrigVars["Muons"][0],
    ]

JETM15SlimmingHelper.AppendContentToStream(JETM15Stream)

JETM15Stream.RemoveItem("xAOD::TrigNavigation#*")
JETM15Stream.RemoveItem("xAOD::TrigNavigationAuxInfo#*")


