#====================================================================
# JETM15.py
# reductionConf flag JETM15 in Reco_tf.py
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkFlavourTag.HbbCommon import *
#
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
  addStandardTruthContents()
  # We probably don't need this since we aren't doing real systematics?
  #import DerivationFrameworkCore.WeightMetadata
  #import DerivationFrameworkCore.LHE3WeightMetadata

  from DerivationFrameworkMCTruth import MCTruthCommon
  MCTruthCommon.addStandardTruthContents()
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
                 (count( AntiKt10LCTopoCSSKJets.pt > 300.*GeV && abs(AntiKt10LCTopoCSSKJets.eta) < 2.5 ) >=1) ||  \
                 (count( AntiKt10EMPFlowCSSKJets.pt > 300.*GeV && abs(AntiKt10EMPFlowCSSKJets.eta) < 2.5 ) >=1)  ||  \
                 (count( AntiKt10TrackCaloClusterJets.pt > 300.*GeV && abs(AntiKt10TrackCaloClusterJets.eta) < 2.5 ) >=1) || \
                 (count( AntiKt10UFOCHSJets.pt > 300.*GeV && abs(AntiKt10UFOCHSJets.eta) < 2.5 ) >=1) || \
                 (count( AntiKt10UFOCSSKJets.pt > 300.*GeV && abs(AntiKt10UFOCSSKJets.eta) < 2.5 ) >=1) \
                )' 

orstr  = ' || '
andstr = ' && '

mutrigsel = '(EventInfo.eventTypeBitmask==1) || '+orstr.join(muonTriggers)
muofflinesel = andstr.join(['count((Muons.pt > 20*GeV) && (Muons.DFCommonMuonsPreselection)) >= 1',
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
                                                                        ThinningService         = "JETM15ThinningSvc",
                                                                        JetKey                  = "AntiKt10LCTopoJets",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                        ApplyAnd                = False)
ToolSvc += JETM15Akt10JetTPThinningTool
thinningTools.append(JETM15Akt10JetTPThinningTool)

JETM15Akt10JetTPThinningToolCSSK = DerivationFramework__JetTrackParticleThinning( name          = "JETM15Akt10CSSKLCTopoJetTPThinningTool",
                                                                        ThinningService         = "JETM15ThinningSvc",
                                                                        JetKey                  = "AntiKt10LCTopoCSSKJets",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                        ApplyAnd                = False)
ToolSvc += JETM15Akt10JetTPThinningToolCSSK
thinningTools.append(JETM15Akt10JetTPThinningToolCSSK)

JETM15Akt10JetTPThinningToolPFO = DerivationFramework__JetTrackParticleThinning( name          = "JETM15Akt10CSSKPFOJetTPThinningTool",
                                                                        ThinningService         = "JETM15ThinningSvc",
                                                                        JetKey                  = "AntiKt10EMPFlowCSSKJets",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                        ApplyAnd                = False)
ToolSvc += JETM15Akt10JetTPThinningToolPFO
thinningTools.append(JETM15Akt10JetTPThinningToolPFO)

JETM15Akt10JetTPThinningToolTCC = DerivationFramework__JetTrackParticleThinning( name          = "JETM15Akt10TCCJetTPThinningTool",
                                                                        ThinningService         = "JETM15ThinningSvc",
                                                                        JetKey                  = "AntiKt10TrackCaloClusterJets",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                        ApplyAnd                = False)
ToolSvc += JETM15Akt10JetTPThinningToolTCC
thinningTools.append(JETM15Akt10JetTPThinningToolTCC)

JETM15Akt10JetEMUFOThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "JETM15Akt10JetCHSUFOThinningTool",
                                                                        ThinningService         = "JETM15ThinningSvc",
                                                                        JetKey                  = "AntiKt10UFOCHSJets",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                        ApplyAnd                = False)
ToolSvc += JETM15Akt10JetEMUFOThinningTool
thinningTools.append(JETM15Akt10JetEMUFOThinningTool)

JETM15Akt10JetUFOCSSKThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "JETM15Akt10JetUFOCSSKThinningTool",
                                                                          ThinningService         = "JETM15ThinningSvc",
                                                                          JetKey                  = "AntiKt10UFOCSSKJets",
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                          ApplyAnd                = False)
ToolSvc += JETM15Akt10JetUFOCSSKThinningTool
thinningTools.append(JETM15Akt10JetUFOCSSKThinningTool)

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
JETM15MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name     = "JETM15MuonTPThinningTool",
                                                                         ThinningService         = "JETM15ThinningSvc",
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM15MuonTPThinningTool
thinningTools.append(JETM15MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
JETM15ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "JETM15ElectronTPThinningTool",
                                                                               ThinningService         = "JETM15ThinningSvc",
                                                                               SGKey                   = "Electrons",
                                                                               InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM15ElectronTPThinningTool
thinningTools.append(JETM15ElectronTPThinningTool)


from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__JetCaloClusterThinning

JETM15AKt10CSThinningTool = DerivationFramework__JetCaloClusterThinning(name                = "JETM15AKt10CSThinningTool",
                                                                      ThinningService       = "JETM15ThinningSvc",
                                                                      SGKey                 = "AntiKt10LCTopoCSSKJets",
                                                                      TopoClCollectionSGKey = "CaloCalTopoClusters",
                                                                      SelectionString       = "AntiKt10LCTopoCSSKJets.pt > 300*GeV && abs(AntiKt10LCTopoCSSKJets.eta) < 2.8",
                                                                      AdditionalClustersKey = ["LCOriginTopoClusters", "LCOriginCSSKTopoClusters"],
                                                                      ApplyAnd                = False)
ToolSvc += JETM15AKt10CSThinningTool
thinningTools.append(JETM15AKt10CSThinningTool)

JETM15AKt10CCThinningTool = DerivationFramework__JetCaloClusterThinning(name                = "JETM15AKt10CCThinningTool",
                                                                      ThinningService       = "JETM15ThinningSvc",
                                                                      SGKey                 = "AntiKt10LCTopoJets",
                                                                      TopoClCollectionSGKey = "CaloCalTopoClusters",
                                                                      SelectionString       = "(AntiKt10LCTopoJets.pt > 300*GeV && abs(AntiKt10LCTopoJets.eta) < 2.8)",
                                                                      AdditionalClustersKey = ["LCOriginTopoClusters", "LCOriginCSSKTopoClusters"],
                                                                      ApplyAnd                = False)
ToolSvc += JETM15AKt10CCThinningTool
thinningTools.append(JETM15AKt10CCThinningTool)


# Tracks and CaloClusters associated with TCCs
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TCCTrackParticleThinning
JETM15TCCTPThinningTool = DerivationFramework__TCCTrackParticleThinning(name                         = "JETM15TCCTPThinningTool",
                                                                       ThinningService              = "JETM15ThinningSvc",
                                                                       JetKey                       = "AntiKt10TrackCaloClusterJets",
                                                                       TCCKey                       = "TrackCaloClustersCombinedAndNeutral",
                                                                       InDetTrackParticlesKey       = "InDetTrackParticles",
                                                                       CaloCalTopoClustersKey       = "CaloCalTopoClusters",
                                                                       ThinOriginCorrectedClusters  = True,
                                                                       OriginCaloCalTopoClustersKey = "LCOriginTopoClusters")
ToolSvc += JETM15TCCTPThinningTool
thinningTools.append(JETM15TCCTPThinningTool)


from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TCCTrackParticleThinning
JETM15TCCTPThinningToolCSSK = DerivationFramework__TCCTrackParticleThinning(name                         = "JETM15TCCTPThinningToolCSSK",
                                                                       ThinningService              = "JETM15ThinningSvc",
                                                                       JetKey                       = "AntiKt10TrackCaloClusterJets",
                                                                       TCCKey                       = "TrackCaloClustersCombinedAndNeutral",
                                                                       InDetTrackParticlesKey       = "InDetTrackParticles",
                                                                       CaloCalTopoClustersKey       = "CaloCalTopoClusters",
                                                                       ThinOriginCorrectedClusters  = True,
                                                                       OriginCaloCalTopoClustersKey = "LCOriginCSSKTopoClusters")
ToolSvc += JETM15TCCTPThinningToolCSSK
thinningTools.append(JETM15TCCTPThinningToolCSSK)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__UFOTrackParticleThinning

JETM15EMUFOTPThinningTool = DerivationFramework__UFOTrackParticleThinning(name                      = "JETM15EMUFOTPThinningTool",
                                                                       ThinningService              = "JETM15ThinningSvc",
                                                                       JetKey                       = "AntiKt10UFOCHSJets",
                                                                       UFOKey                       = "UFOCHS",
                                                                       InDetTrackParticlesKey       = "InDetTrackParticles",
                                                                       PFOCollectionSGKey       = "JetETMiss",
                                                                        AdditionalPFOKey  = ["CSSK", "CHS"])
ToolSvc += JETM15EMUFOTPThinningTool
thinningTools.append(JETM15EMUFOTPThinningTool)

JETM15EMUFOCSSKTPThinningTool = DerivationFramework__UFOTrackParticleThinning(name                  = "JETM15UFOCSSKTPThinningTool",
                                                                       ThinningService              = "JETM15ThinningSvc",
                                                                       JetKey                       = "AntiKt10UFOCSSKJets",
                                                                       UFOKey                       = "UFOCSSK",
                                                                       InDetTrackParticlesKey       = "InDetTrackParticles",
                                                                       PFOCollectionSGKey       = "JetETMiss",
                                                                        AdditionalPFOKey  = ["CSSK", "CHS"])
ToolSvc += JETM15EMUFOCSSKTPThinningTool
thinningTools.append(JETM15EMUFOCSSKTPThinningTool)



from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__JetPFlowThinning
JETM15AKt10PFOThinningTool = DerivationFramework__JetPFlowThinning(name                = "JETM15Ak10PFlowThinningTool",
                                                                      ThinningService       = "JETM15ThinningSvc",
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


#====================================================================
# Special jets
#====================================================================

# Add alternative rho definitions
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addCHSPFlowObjects
addCHSPFlowObjects()

from DerivationFrameworkJetEtMiss.JetCommon import defineEDAlg
jetm15Seq += defineEDAlg(R=0.4, inputtype="EMPFlowPUSB")
jetm15Seq += defineEDAlg(R=0.4, inputtype="EMPFlowNeut")

#SCHEDULE BTAGGING FOR PFLOW JETS
from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit
FlavorTagInit(JetCollections=['AntiKt4EMPFlowJets'], Sequencer=jetm15Seq)

#=======================================
# RESTORE AOD-REDUCED JET COLLECTIONS
#=======================================
reducedJetList = ["AntiKt2PV0TrackJets",
                  "AntiKt4PV0TrackJets",
                  "AntiKt4TruthJets",
                  "AntiKt10TruthJets",
                  "AntiKt10LCTopoJets"]

replaceAODReducedJets(reducedJetList,jetm15Seq,"JETM15")

addDefaultTrimmedJets(jetm15Seq,"JETM15")

addConstModJets("AntiKt", 1.0, "LCTopo", ["CS", "SK"], jetm15Seq, "JETM15", ptmin=40000, 
                    ptminFilter=50000, mods="lctopo_ungroomed", addGetters=[])

addConstModJets("AntiKt", 1.0, "EMPFlow", ["CS", "SK"], jetm15Seq, "JETM15", ptmin=40000, 
                    ptminFilter=50000)

# add VR jets
addVRJets(jetm15Seq)
addVRJets(jetm15Seq,training='201903')



# Now we can run the UFO building taking our unified PFlow container as input
from TrackCaloClusterRecTools.TrackCaloClusterConfig import runUFOReconstruction
emufoAlg = runUFOReconstruction(jetm15Seq,ToolSvc, PFOPrefix="CHS",caloClusterName="LCOriginTopoClusters")
emcsskufoAlg = runUFOReconstruction(jetm15Seq,ToolSvc, PFOPrefix="CSSK", caloClusterName='LCOriginTopoClusters')

from JetRec.JetRecConf import PseudoJetGetter

csskufopjgetter = PseudoJetGetter("csskufoPJGetter", InputContainer="UFOCSSK", OutputContainer="UFOCSSKPJ", Label="UFO", SkipNegativeEnergy=True)
jtm+=csskufopjgetter
ufopjgetter     = PseudoJetGetter("ufoPJGetter",     InputContainer="UFOCHS",     OutputContainer="UFOCHSPJ",     Label="UFO", SkipNegativeEnergy=True)
jtm+=ufopjgetter

# These lines make sure that we also retrieve all of the other getters, like track association
# addConstModJets doesn't have a good way of dealing with UFOs at the moment unfortunately
csskufogetters = [csskufopjgetter]+list(jtm.gettersMap["tcc"])[1:]
chsufogetters = [ufopjgetter]+list(jtm.gettersMap["tcc"])[1:]
addStandardJets("AntiKt", 1.0, "UFOCSSK", ptmin=40000, ptminFilter=50000, algseq=jetm15Seq, outputGroup="JETM15", customGetters = csskufogetters, constmods=["CSSK"])
addStandardJets("AntiKt", 1.0, "UFOCHS", ptmin=40000, ptminFilter=50000, algseq=jetm15Seq, outputGroup="JETM15", customGetters = chsufogetters, constmods=["CHS"])

# Create TCC objects
from TrackCaloClusterRecTools.TrackCaloClusterConfig import runTCCReconstruction
# Set up geometry and BField
import AthenaCommon.AtlasUnixStandardJob
include("RecExCond/AllDet_detDescr.py")
runTCCReconstruction(jetm15Seq, ToolSvc, "LCOriginTopoClusters", "InDetTrackParticles",outputTCCName="TrackCaloClustersCombinedAndNeutral")
addTCCTrimmedJets(jetm15Seq,"JETM15")



from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
jetm15Seq += CfgMgr.DerivationFramework__DerivationKernel( name = "JETM15MainKernel",
                                                          AugmentationTools = [TrigMatchAug] ,
                                                          SkimmingTools = [JETM15OfflineSkimmingTool],
                                                          ThinningTools = thinningTools)

#====================================================================
# SET UP STREAM
#====================================================================
streamName = derivationFlags.WriteDAOD_JETM15Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_JETM15Stream )
JETM15Stream = MSMgr.NewPoolRootStream( streamName, fileName )
JETM15Stream.AcceptAlgs(["JETM15MainKernel"])
# for thinning
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="JETM15ThinningSvc", outStreams=[evtStream] )


# Truth particle thinning
doTruthThinning = True
preserveAllDescendants = False

if DerivationFrameworkIsMonteCarlo:

  from DerivationFrameworkCore.ThinningHelper import ThinningHelper
  JETM15ThinningHelper = ThinningHelper( "JETM15ThinningHelper" )
  JETM15ThinningHelper.AppendToStream( JETM15Stream )
  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
  TruthThinningTool = DerivationFramework__MenuTruthThinning(name               = "JETM15TruthThinning",
                                                             ThinningService    = JETM15ThinningHelper.ThinningSvc(),
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
  "Electrons", 
  "Muons",
  "InDetTrackParticles",
  "PrimaryVertices",
  "MET_Reference_AntiKt4EMTopo",
  "MET_Reference_AntiKt4EMPFlow",
  "AntiKt4EMTopoJets",
  "AntiKt4EMPFlowJets",
  "AntiKt4EMPFlowJets_BTagging201810",
  "AntiKt4EMPFlowJets_BTagging201903",
  "AntiKt4EMTopoJets_BTagging201810",
  "BTagging_AntiKt4EMPFlow_201810",
  "BTagging_AntiKt4EMPFlow_201903",
  "BTagging_AntiKt4EMTopo_201810",                                         
  "BTagging_AntiKtVR30Rmax4Rmin02Track_201810",
  "BTagging_AntiKtVR30Rmax4Rmin02Track_201903",
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

JETM15SlimmingHelper.AppendToDictionary["UFOCSSK"] = 'xAOD::TrackCaloClusterContainer'
JETM15SlimmingHelper.AppendToDictionary['UFOCSSKAux'] = 'xAOD::TrackCaloClusterAuxContainer'
JETM15SlimmingHelper.ExtraVariables +=[ 'UFOCSSK.pt.eta.phi.m.taste' ]

JETM15SlimmingHelper.AppendToDictionary["UFOCHS"] = 'xAOD::TrackCaloClusterContainer'
JETM15SlimmingHelper.AppendToDictionary['UFOCHSAux'] = 'xAOD::TrackCaloClusterAuxContainer'
JETM15SlimmingHelper.ExtraVariables +=[ 'UFOCHS.pt.eta.phi.m.taste' ]


JETM15SlimmingHelper.AppendToDictionary["CSSKChargedParticleFlowObjects"]='xAOD::PFOContainer'
JETM15SlimmingHelper.AppendToDictionary["CSSKChargedParticleFlowObjectsAux"]='xAOD::ShallowAuxContainer'
JETM15SlimmingHelper.ExtraVariables.append('CSSKChargedParticleFlowObjects.pt.eta.phi.m.e.charge.matchedToPV')

JETM15SlimmingHelper.AppendToDictionary["CSSKNeutralParticleFlowObjects"]='xAOD::PFOContainer'
JETM15SlimmingHelper.AppendToDictionary["CSSKNeutralParticleFlowObjectsAux"]='xAOD::ShallowAuxContainer'
JETM15SlimmingHelper.ExtraVariables.append('CSSKNeutralParticleFlowObjects.pt.eta.phi.m.e.charge')

JETM15SlimmingHelper.AppendToDictionary["CHSChargedParticleFlowObjects"]='xAOD::PFOContainer'
JETM15SlimmingHelper.AppendToDictionary["CHSChargedParticleFlowObjectsAux"]='xAOD::ShallowAuxContainer'
JETM15SlimmingHelper.ExtraVariables.append('CHSChargedParticleFlowObjects.pt.eta.phi.m.e.charge.matchedToPV')

JETM15SlimmingHelper.AppendToDictionary["CHSNeutralParticleFlowObjects"]='xAOD::PFOContainer'
JETM15SlimmingHelper.AppendToDictionary["CHSNeutralParticleFlowObjectsAux"]='xAOD::ShallowAuxContainer'
JETM15SlimmingHelper.ExtraVariables.append('CHSNeutralParticleFlowObjects.pt.eta.phi.m.e.charge')

JETM15SlimmingHelper.AllVariables  += ["LCOriginTopoClusters", "LCOriginCSSKTopoClusters", "CSSKNeutralParticleFlowObjects", "CSSKChargedParticleFlowObjects", "TrackCaloClustersCombinedAndNeutral", "CHSChargedParticleFlowObjects", "CHSNeutralParticleFlowObjects"]
JETM15SlimmingHelper.ExtraVariables.append("AntiKt4EMTopoJets.btaggingLink")
JETM15SlimmingHelper.ExtraVariables.append("BTagging_AntiKt4EMTopo.MSV_N2Tpair.MSV_badTracksIP.MSV_energyTrkInJet.MSV_normdist.MSV_nvsec.MSV_vertices.MV1_discriminant.MV2c00_discriminant.MV2c100_discriminant.MV2c10_discriminant.MV2c20_discriminant.MV2m_pb.MV2m_pc.MV2m_pu.MultiSVbb1_discriminant.MultiSVbb2_discriminant.SV0_N2Tpair.SV1_pb.SV1_pc.SV1_pu.IP3D_pb.IP3D_pc.IP3D_pu.DL1_pb.DL1_pc.DL1_pu.DL1mu_pb.DL1mu_pc.DL1mu_pu.DL1rnn_pb.DL1rnn_pc.DL1rnn_pu.JetVertexCharge_discriminant")

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


