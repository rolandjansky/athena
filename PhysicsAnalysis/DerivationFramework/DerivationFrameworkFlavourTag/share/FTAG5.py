#====================================================================
# FTAG5.py
# It requires the reductionConf flag FTAG5 in Reco_tf.py
#====================================================================

# Set up common services and job objects
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkFlavourTag.HbbCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
if globalflags.DataSource()!='data':
    from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
    addStandardTruthContents()
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
from AthenaCommon.AthenaCommonFlags import jobproperties as jp

jp.AthenaCommonFlags.EvtMax.set_Value_and_Lock(-1)

FTAG5Seq = CfgMgr.AthSequencer("FTAG5Sequence")

#===================================================================
# Variable R track jets
#===================================================================
vr_parameters = [
        ("30", 30000, "4", 0.4, "02", 0.02)
]

from DerivationFrameworkExotics.JetDefinitions import *
from JetRec.JetRecStandard import jtm
from JetRec.JetRecConf import JetAlgorithm
from JetRec.JetRecConf import PseudoJetGetter

for vr_parameter in vr_parameters:
	rho_str = vr_parameter[0]
	rho_num = vr_parameter[1]
	rmax_str = vr_parameter[2]
	rmax_num = vr_parameter[3]
	rmin_str = vr_parameter[4]
	rmin_num = vr_parameter[5]
	vr_jet_collection_name = "AntiKtVR" + rho_str + "Rmax" + rmax_str + "Rmin" + rmin_str + "PV0TrackJets"
	vr_btagging_name = "AntiKtVR" + rho_str + "Rmax" + rmax_str + "Rmin" + rmin_str + "Track"
	vr_jet_ghost_label = "GhostVR" + rho_str + "Rmax" + rmax_str + "Rmin" + rmin_str + "PV0TrackJet"
	jalg_name = "jfind_" + vr_jet_collection_name
	pseudojetgetter_name = "gvr" + rho_str + "rmax" + rmax_str + "rmin" + rmin_str + "trackget"
	pseudojet_name = "PseudoJet" + vr_jet_ghost_label

	jfind = jtm.addJetFinder(vr_jet_collection_name, "AntiKt", rmax_num, "pv0track", "pv0track",
				 ghostArea = 0 , ptmin = 2000, ptminFilter = 7000,
				 variableRMinRadius = rmin_num, variableRMassScale = rho_num, calibOpt = "none")

	jalg = JetAlgorithm(jalg_name, Tools = [jfind])

	FTAG5Seq += jalg

	jtm += PseudoJetGetter(
		pseudojetgetter_name, # give a unique name
		InputContainer = jetFlags.containerNamePrefix() + vr_jet_collection_name, # SG key
		Label = vr_jet_ghost_label, # this is the name you'll use to retrieve ghost associated VR track jets
		OutputContainer = pseudojet_name,
		SkipNegativeEnergy = True,
		GhostScale = 1.e-20, # this makes the PseudoJet Ghosts, and thus the reco flow will treat them as such
	)

#===================================================================
# Build New AntiKt R=1.0 LCTopo Jets
#===================================================================

jtm.gettersMap["newlctopo"] = list(jtm.gettersMap["lctopo"])

for vr_parameter in vr_parameters:
	rho_str = vr_parameter[0]
	rho_num = vr_parameter[1]
	rmax_str = vr_parameter[2]
	rmax_num = vr_parameter[3]
	rmin_str = vr_parameter[4]
	rmin_num = vr_parameter[5]
	vr_jet_collection_name = "AntiKtVR" + rho_str + "Rmax" + rmax_str + "Rmin" + rmin_str + "PV0TrackJets"
	vr_btagging_name = "AntiKtVR" + rho_str + "Rmax" + rmax_str + "Rmin" + rmin_str + "Track"
	vr_jet_ghost_label = "GhostVR" + rho_str + "Rmax" + rmax_str + "Rmin" + rmin_str + "PV0TrackJet"
	jalg_name = "jfind_" + vr_jet_collection_name
	pseudojetgetter_name = "gvr" + rho_str + "rmax" + rmax_str + "rmin" + rmin_str + "trackget"
	pseudojet_name = "PseudoJet" + vr_jet_ghost_label

	pseudojetgetter_name = "gvr" + rho_str + "rmax"	+ rmax_str + "rmin" + rmin_str + "trackget"

	jtm.gettersMap["newlctopo"] += [jtm[pseudojetgetter_name]] # [jtm.gvr30rmax4rmin02trackget]

jfind_largefr10_lctopo = jtm.addJetFinder("NewAntiKt10LCTopoJets", "AntiKt", 1.0, "newlctopo", "lctopo_ungroomed",
                                        ghostArea = 0 , ptmin = 2000, ptminFilter = 50000,
                                        calibOpt = "none")
jetalg_largefr10_lctopo = JetAlgorithm("jfind_largefr10_lctopo", Tools = [jfind_largefr10_lctopo])
FTAG5Seq += jetalg_largefr10_lctopo

#===================================================================
# Transfer the Ghost Links to the Original AntiKt R=1.0 LCTopo Jets
#===================================================================

OldLinkNameList = []

for vr_parameter in vr_parameters:
	rho_str = vr_parameter[0]
	rho_num = vr_parameter[1]
	rmax_str = vr_parameter[2]
	rmax_num = vr_parameter[3]
	rmin_str = vr_parameter[4]
	rmin_num = vr_parameter[5]
	vr_jet_collection_name = "AntiKtVR" + rho_str + "Rmax" + rmax_str + "Rmin" + rmin_str + "PV0TrackJets"
	vr_btagging_name = "AntiKtVR" + rho_str + "Rmax" + rmax_str + "Rmin" + rmin_str + "Track"
	vr_jet_ghost_label = "GhostVR" + rho_str + "Rmax" + rmax_str + "Rmin" + rmin_str + "PV0TrackJet"
	jalg_name = "jfind_" + vr_jet_collection_name
	pseudojetgetter_name = "gvr" + rho_str + "rmax" + rmax_str + "rmin" + rmin_str + "trackget"
	pseudojet_name = "PseudoJet" + vr_jet_ghost_label

	OldLinkNameList += [vr_jet_ghost_label]

jetassoctool = getJetExternalAssocTool('AntiKt10LCTopo', 'NewAntiKt10LCTopo', MomentPrefix='', ListOfOldLinkNames=OldLinkNameList, DeltaRMatch=True)

applyJetAugmentation('AntiKt10LCTopo', 'AugmentationAlg_LinkTransfer', FTAG5Seq, jetassoctool)

#===================================================================
# Build Trimmed large-R jet 
# It is important that the trimming happens AFTER all decoration on AntiKt10LCTopoJets
#===================================================================

addDefaultTrimmedJets(FTAG5Seq, "FTAG5")
applyJetCalibration_CustomColl("AntiKt10LCTopoTrimmedPtFrac5SmallR20", FTAG5Seq)

#===================================================================
# Run b-tagging
#===================================================================

defaultTaggers = ['IP2D', 'IP3D', 'MultiSVbb1', 'MultiSVbb2', 'SV1', 'BasicJetFitter', 'JetFitterTag', 'JetFitterNN', 'GbbNNTag', 'MV2c10', 'MV2c100', 'RNNIP', 'DL1']

from BTagging.BTaggingFlags import BTaggingFlags

# RNNIP: there are also two 'ipz_grade' with and without the RW
rnn_taggers = ['ipmp_hits', 'ipz_hits', 'ipz_hits_rw']
BTaggingFlags.RNNIP = True
BTaggingFlags.RNNIPConfig = {x: '' for x in rnn_taggers}

# alias for VR
for vr_parameter in vr_parameters:
	rho_str = vr_parameter[0]
	rho_num = vr_parameter[1]
	rmax_str = vr_parameter[2]
	rmax_num = vr_parameter[3]
	rmin_str = vr_parameter[4]
	rmin_num = vr_parameter[5]
	vr_jet_collection_name = "AntiKtVR" + rho_str + "Rmax" + rmax_str + "Rmin" + rmin_str + "PV0TrackJets"
	vr_btagging_name = "AntiKtVR" + rho_str + "Rmax" + rmax_str + "Rmin" + rmin_str + "Track"
	vr_jet_ghost_label = "GhostVR" + rho_str + "Rmax" + rmax_str + "Rmin" + rmin_str + "PV0TrackJet"
	jalg_name = "jfind_" + vr_jet_collection_name
	pseudojetgetter_name = "gvr" + rho_str + "rmax" + rmax_str + "rmin" + rmin_str + "trackget"
	pseudojet_name = "PseudoJet" + vr_jet_ghost_label

	BTaggingFlags.CalibrationChannelAliases += [vr_btagging_name + "->AntiKt4EMTopo"]

from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit
# must re-tag AntiKt4LCTopoJets and AntiKt4PV0TrackJets to make JetFitterNN work with corresponding VR jets (nikola: why?)

JetCollectionsToTag = [
        "AntiKt4EMTopoJets",
        "AntiKt4PV0TrackJets",
        "AntiKt2PV0TrackJets"
]

for vr_parameter in vr_parameters:
	rho_str = vr_parameter[0]
	rho_num = vr_parameter[1]
	rmax_str = vr_parameter[2]
	rmax_num = vr_parameter[3]
	rmin_str = vr_parameter[4]
	rmin_num = vr_parameter[5]
	vr_jet_collection_name = "AntiKtVR" + rho_str + "Rmax" + rmax_str + "Rmin" + rmin_str + "PV0TrackJets"
	vr_btagging_name = "AntiKtVR" + rho_str + "Rmax" + rmax_str + "Rmin" + rmin_str + "Track"
	vr_jet_ghost_label = "GhostVR" + rho_str + "Rmax" + rmax_str + "Rmin" + rmin_str + "PV0TrackJet"
	jalg_name = "jfind_" + vr_jet_collection_name
	pseudojetgetter_name = "gvr" + rho_str + "rmax" + rmax_str + "rmin" + rmin_str + "trackget"
	pseudojet_name = "PseudoJet" + vr_jet_ghost_label

	JetCollectionsToTag += [vr_jet_collection_name]

FlavorTagInit( myTaggers = defaultTaggers, JetCollections = JetCollectionsToTag, Sequencer = FTAG5Seq )

#====================================================================
# SKIMMING TOOLS
#====================================================================
offlineExpression = '((count (AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.pt > 250 * GeV && (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.eta) < 2.0)) > 0 ))'

if globalflags.DataSource()=='data':
    triggers=[
        # primay triggers suggested for periodC
        "HLT_e24_lhmedium_L1EM18VH",
        "HLT_e60_lhmedium",
        "HLT_e120_lhloose",
        "HLT_e24_lhmedium_iloose_L1EM18VH",
        "HLT_e60_lhmedium",
        "HLT_e120_lhloose",
        "HLT_mu20_iloose_L1MU15",
        "HLT_mu40",
        # other single lepton triggers
        "HLT_e24_lhtight_iloose_L1EM20VH",
        "HLT_e24_tight_iloose_L1EM20VH",
        "HLT_mu14_iloose",
        "HLT_e17_loose"
        ]

    ORStr=" || "
    triggerStr=ORStr.join(triggers)
    triggerExpression = "((EventInfo.eventTypeBitmask==1) || (" + triggerStr +" ))"
    expression = offlineExpression+' && '+triggerExpression

else:
    triggers = []
    # disabling the skimming in the offline expression above
    expression = 'true'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
FTAG5SkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "FTAG5OfflineSkimmingTool", expression = expression)
ToolSvc += FTAG5SkimmingTool

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS
#====================================================================

# The name of the kernel (LooseSkimKernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += FTAG5Seq
# FTAG5Seq += CfgMgr.DerivationFramework__DerivationKernel("FTAG5Kernel")
FTAG5Seq += CfgMgr.DerivationFramework__DerivationKernel("FTAG5Kernel", SkimmingTools = [FTAG5SkimmingTool])

#====================================================================
# SET UP STREAM
#====================================================================

# The base name (DAOD_FTAG5 here) must match the string in
streamName = derivationFlags.WriteDAOD_FTAG5Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_FTAG5Stream )
FTAG5Stream = MSMgr.NewPoolRootStream( streamName, fileName )
# Only events that pass the filters listed below are written out.
# Name must match that of the kernel above
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
FTAG5Stream.AcceptAlgs(["FTAG5Kernel"])

from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
FTAG5SlimmingHelper = SlimmingHelper("FTAG5SlimmingHelper")

# NB: the BTagging_AntiKt4EMTopo smart collection includes both AntiKt4EMTopoJets and BTagging_AntiKt4EMTopo
# container variables. Thus BTagging_AntiKt4EMTopo is needed in SmartCollections as well as AllVariables
FTAG5SlimmingHelper.AppendToDictionary = {
"AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"    :   "xAOD::JetContainer"        ,
"AntiKt10LCTopoTrimmedPtFrac5SmallR20JetsAux" :   "xAOD::JetAuxContainer"     ,
"AntiKt10TrackJets"                               :   "xAOD::JetContainer"        ,
"AntiKt10TrackJetsAux"                            :   "xAOD::JetAuxContainer"     ,
}

for vr_parameter in vr_parameters:
	rho_str = vr_parameter[0]
	rho_num = vr_parameter[1]
	rmax_str = vr_parameter[2]
	rmax_num = vr_parameter[3]
	rmin_str = vr_parameter[4]
	rmin_num = vr_parameter[5]
	vr_jet_collection_name = "AntiKtVR" + rho_str + "Rmax" + rmax_str + "Rmin" + rmin_str + "PV0TrackJets"
	vr_btagging_name = "AntiKtVR" + rho_str + "Rmax" + rmax_str + "Rmin" + rmin_str + "Track"
	vr_jet_ghost_label = "GhostVR" + rho_str + "Rmax" + rmax_str + "Rmin" + rmin_str + "PV0TrackJet"
	jalg_name = "jfind_" + vr_jet_collection_name
	pseudojetgetter_name = "gvr" + rho_str + "rmax" + rmax_str + "rmin" + rmin_str + "trackget"
	pseudojet_name = "PseudoJet" + vr_jet_ghost_label

	FTAG5SlimmingHelper.AppendToDictionary[vr_jet_collection_name] = "xAOD::JetContainer"
	FTAG5SlimmingHelper.AppendToDictionary[vr_jet_collection_name + "Aux"] = "xAOD::JetAuxContainer"
	FTAG5SlimmingHelper.AppendToDictionary["BTagging_" + vr_btagging_name] = "xAOD::BTaggingContainer"
	FTAG5SlimmingHelper.AppendToDictionary["BTagging_" + vr_btagging_name + "Aux"] = "xAOD::BTaggingAuxContainer"

FTAG5SlimmingHelper.SmartCollections = ["Electrons","Muons",
                                        "MET_Reference_AntiKt4EMTopo",
                                        "InDetTrackParticles",
                                        "PrimaryVertices",
                                        "AntiKt4EMTopoJets",
                                        "BTagging_AntiKt4EMTopo"]

FTAG5SlimmingHelper.AllVariables = ["AntiKt2PV0TrackJets",
                                    "AntiKt4PV0TrackJets",
                                    "AntiKt4TruthJets",
                                    "TruthEvents",
                                    "MET_Truth",
                                    "MET_TruthRegions",
                                    "TruthParticles",
                                    #"TruthElectrons",
                                    #"TruthMuons",
                                    #"TruthTaus",
                                    #"TruthPhotons",
                                    #"TruthBoson",
                                    #"TruthTop",
                                    "BTagging_AntiKt4EMTopo",
                                    "BTagging_AntiKt2Track",
                                    "BTagging_AntiKt4EMTopoJFVtx",
                                    "BTagging_AntiKt2TrackJFVtx",
                                    "BTagging_AntiKt4EMTopoSecVtx",
                                    "BTagging_AntiKt2TrackSecVtx",
                                    "AntiKt10LCTopoJets",
                                    "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                    "AntiKt10TrackJets",
                                    "CaloCalTopoClusters",
                                    ]

FTAG5SlimmingHelper.AppendToDictionary = {
  "TruthElectrons"                             :   "xAOD::TruthParticleContainer",
  "TruthElectronsAux"                          :   "xAODTruthParticleAuxContainer",
  "TruthMuons"                                 :   "xAOD::TruthParticleContainer",
  "TruthMuonsAux"                              :   "xAODTruthParticleAuxContainer",
  "TruthTaus"                                  :   "xAOD::TruthParticleContainer",
  "TruthTausAux"                               :   "xAODTruthParticleAuxContainer",
  "TruthPhotons"                               :   "xAOD::TruthParticleContainer",
  "TruthPhotonsAux"                            :   "xAODTruthParticleAuxContainer",
  "TruthBoson"                                 :   "xAOD::TruthParticleContainer",
  "TruthBosonAux"                              :   "xAODTruthParticleAuxContainer",
  "TruthTop"                                   :   "xAOD::TruthParticleContainer",
  "TruthTopAux"                                :   "xAODTruthParticleAuxContainer",
  }

for vr_parameter in vr_parameters:
	rho_str = vr_parameter[0]
	rho_num = vr_parameter[1]
	rmax_str = vr_parameter[2]
	rmax_num = vr_parameter[3]
	rmin_str = vr_parameter[4]
	rmin_num = vr_parameter[5]
	vr_jet_collection_name = "AntiKtVR" + rho_str + "Rmax" + rmax_str + "Rmin" + rmin_str + "PV0TrackJets"
	vr_btagging_name = "AntiKtVR" + rho_str + "Rmax" + rmax_str + "Rmin" + rmin_str + "Track"
	vr_jet_ghost_label = "GhostVR" + rho_str + "Rmax" + rmax_str + "Rmin" + rmin_str + "PV0TrackJet"
	jalg_name = "jfind_" + vr_jet_collection_name
	pseudojetgetter_name = "gvr" + rho_str + "rmax" + rmax_str + "rmin" + rmin_str + "trackget"
	pseudojet_name = "PseudoJet" + vr_jet_ghost_label

	FTAG5SlimmingHelper.AllVariables += [vr_jet_collection_name]
	FTAG5SlimmingHelper.AllVariables += ["BTagging_" + vr_btagging_name]

from DerivationFrameworkJetEtMiss.AntiKt4EMTopoJetsCPContent import AntiKt4EMTopoJetsCPContent

FTAG5SlimmingHelper.ExtraVariables.append(AntiKt4EMTopoJetsCPContent[1].replace("AntiKt4EMTopoJetsAux","AntiKt10LCTopoJets"))
FTAG5SlimmingHelper.ExtraVariables.append("AntiKt10LCTopoJets.GhostAntiKt2TrackJet")
FTAG5SlimmingHelper.ExtraVariables.append("AntiKt10LCTopoJets.GhostAntiKt2TrackJetPt")
FTAG5SlimmingHelper.ExtraVariables.append("AntiKt10LCTopoJets.GhostAntiKt2TrackJetCount")
FTAG5SlimmingHelper.ExtraVariables.append("AntiKt10LCTopoJets.ConeExclBHadronsFinal")
FTAG5SlimmingHelper.ExtraVariables.append("AntiKt10LCTopoJets.ConeExclCHadronsFinal")

FTAG5SlimmingHelper.IncludeMuonTriggerContent = True
FTAG5SlimmingHelper.IncludeEGammaTriggerContent = True
FTAG5SlimmingHelper.IncludeJetTriggerContent = True
FTAG5SlimmingHelper.IncludeEtMissTriggerContent = True
FTAG5SlimmingHelper.IncludeBJetTriggerContent = True

#FTAG5 TrigNav Thinning
FTAG5ThinningHelper = ThinningHelper( "FTAG5ThinningHelper" )
FTAG5ThinningHelper.TriggerChains = 'HLT_e24_lhmedium_L1EM18VH|HLT_e60_lhmedium|HLT_e120_lhloose|HLT_e24_lhmedium_iloose_L1EM18VH|HLT_e60_lhmedium|HLT_e120_lhloose|HLT_mu20_iloose_L1MU15|HLT_mu40|HLT_e24_lhtight_iloose_L1EM20VH|HLT_e24_tight_iloose_L1EM20VH|HLT_mu14_iloose|HLT_e17_loose'
FTAG5ThinningHelper.AppendToStream( FTAG5Stream )

FTAG5SlimmingHelper.AppendContentToStream(FTAG5Stream)
