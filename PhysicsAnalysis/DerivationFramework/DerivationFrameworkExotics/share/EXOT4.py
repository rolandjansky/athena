#********************************************************************
# EXOT4.py 
# reductionConf flag EXOT4 in Reco_tf.py
#********************************************************************
 
from DerivationFrameworkCore.DerivationFrameworkMaster import *

# CP group recommendations and utilities
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *

# write heavy flavour hadron information in MC
if DerivationFrameworkIsMonteCarlo:
   from DerivationFrameworkMCTruth.HFHadronsCommon import *

# utilities used to make jets on-the-fly
from JetRec.JetRecStandard import jtm
from JetRec.JetRecConf import PseudoJetGetter
from JetRecTools.JetRecToolsConf import TrackPseudoJetGetter
from JetRecTools.JetRecToolsConf import JetInputElRemovalTool

# make EXOT4 sequence
exot4Seq = CfgMgr.AthSequencer("EXOT4Sequence")


#====================================================================
# SET UP STREAM
#====================================================================
streamName = derivationFlags.WriteDAOD_EXOT4Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EXOT4Stream )
EXOT4Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT4Stream.AcceptAlgs(["EXOT4Kernel"])


# NoEl jets check if the electron and jets are close to each other
# if they are, the clusters matched to the electron are removed from
# the set of clusters to build jets from and the jet is reclustered
# this technique is under study and could be extended to other busy final state
# analyses
#=======================================
# Augmentation: No El Jets
#=======================================
# load tool to implement subtraction by adding an augmentation to flag relevant information
# Note that the electron quality criteria must be fed here,
# so one cannot use the subtracted jet in this method with any electron
# definition
# a new set of topoclusters is stored in ClusterNoElName without the subtracted clusters
# no need to save the new cluster container, unless for debugging
EXOT16ElRemovalTool=JetInputElRemovalTool(
   name                          ='EXOT16ElRemovalTool',
   ElectronContainerName         ='Electrons',
   ElectronQuality               ='DFCommonElectronsLHTight',
   ElectronMinpT                 =25000.0,
   ClusterContainerName          ='CaloCalTopoClusters',
   ClusterNoElName               ='CaloCalTopoClustersNoEl',
   ClusterRemovRadius            =0.1,
   ClusterEMFrac                 =0.8,
   TrkInputContainer             ="InDetTrackParticles",
   TrkOutputContainer            ="InDetTrackParticlesNoEl"
   )

ToolSvc +=EXOT16ElRemovalTool

# now make the input objects list for the AntiKt4EMTopo
# jet using the set of topoclusters
# that came in the output of the subtraction above
jtm += PseudoJetGetter(
   "Elermovalgetter",
   InputContainer = 'CaloCalTopoClustersNoEl',
   Label = "EMTopo",
   OutputContainer = 'CaloCalTopoClustersNoElPseudoJet',
   SkipNegativeEnergy = True,
   GhostScale = 0.0
   )

# associate tracks to the jets to calculate track-dependent variables
jtm += TrackPseudoJetGetter(
   "Elermovaltrkgetter",
   InputContainer = 'InDetTrackParticlesNoEl',
   Label = "Track",
   OutputContainer = 'InDetTrackParticlesNoElPseudoJet',
   TrackVertexAssociation = jtm.tvassoc.TrackVertexAssociation,
   SkipNegativeEnergy = True,
   GhostScale = 0.0
   )


if jtm.haveParticleJetTools:
   from ParticleJetTools.ParticleJetToolsConf import CopyTruthJetParticles

# apply truth labelling to the newly created jets
# should be the same procedure as it is usually done for akt4 jets
if isMC:
   from MCTruthClassifier.MCTruthClassifierConf import MCTruthClassifier
   truthClassifier = MCTruthClassifier(name = "JetMCTruthClassifier")
   jtm += CopyTruthJetParticles("truthpartcopyNoEl", OutputName="JetInputTruthParticlesNoEl",
                                MCTruthClassifier=truthClassifier,
                                IncludePromptLeptons=False,
                                IncludeMuons=False,IncludeNeutrinos=False)


   jtm += PseudoJetGetter(
      "truthgetNoEl",
      Label = "Truth",
      InputContainer = jtm.truthpartcopyNoEl.OutputName,
      OutputContainer = "PseudoJetTruthNoEl",
      GhostScale = 0.0,
      SkipNegativeEnergy = True,
      
      )


# now the "getters" contain all input objects to be used when making the electron-subtracted
# jet
# add it in a the jtm gettersMap: this is used by the jet maker JetEtMiss functions
jtm.gettersMap["JetElRemovalgetter"]=[jtm.Elermovalgetter,jtm.gtrackget, jtm.gmusegget]
jtm.gettersMap["TrkJetElRemovalgetter"]=[jtm.Elermovaltrkgetter]

# also add the truth input objects in the corresponding place
if isMC:
   jtm.modifiersMap["truthNoEl"] = [jtm.jetsorter]
   jtm.gettersMap["truthNoEl"] = [jtm.truthgetNoEl]

# in the next lines: actually make the AntiKt4Truth, Track and EMTopo jet using
# the topo clusters lists that have the electron-matched clusters removed
if isMC:
   jfind_noel_truth = jtm.addJetFinder("AntiKt4TruthNoElJets", "AntiKt", 0.4, "truthNoEl","truthNoEl", ghostArea=0.0 , ptmin=10000, ptminFilter=70000,calibOpt="")

jfind_smallnoel_trk = jtm.addJetFinder("AntiKt4TrackNoElJets", "AntiKt", 0.4, "TrkJetElRemovalgetter", "track",
                ghostArea=0.01 , ptmin=10000, ptminFilter=10000
                )

jfind_smallnoel_emtopo = jtm.addJetFinder("AntiKt4EMTopoNoElJets", "AntiKt", 0.4, "JetElRemovalgetter", "emtopo_ungroomed",
                ghostArea=0.01 , ptmin=10000, ptminFilter=10000,
                calibOpt="none")

# the jet finder tool has been created, but
# the JetAlgorithm is what, added in the exot4Seq actually
# makes jets (by just calling the finder)

from JetRec.JetRecConf import JetAlgorithm

jetalg_smallnoel_emtopo = JetAlgorithm("jetalgAntiKt4EMTopoNoElJets", Tools = [  jfind_smallnoel_emtopo] )
jetalg_smallnoel_trk = JetAlgorithm("jetalgAntiKt4trkNoElJets", Tools = [EXOT16ElRemovalTool, jfind_smallnoel_trk] )
if isMC:
   jetalg_smallnoel_truth =  JetAlgorithm("jetalgAntiKt4truthNoElJets", Tools = [jtm.truthpartcopyNoEl, jfind_noel_truth] )

# now we have a JetAlgorithm instance that calls the full chain
# add it in the sequence:
if isMC:
   exot4Seq += jetalg_smallnoel_truth
exot4Seq += jetalg_smallnoel_trk
exot4Seq += jetalg_smallnoel_emtopo 


# at this point we have new on-the-fly jets
# in the containers AntiKt4TruthNoElJets, AntiKt4TrackNoElJets, AntiKt4EMTopoNoElJets
# we now need to be able to do b-tagging on them and to define a MET calculation
# using them

# map the calibration constants for the b-tagging
from BTagging.BTaggingFlags import BTaggingFlags
BTaggingFlags.CalibrationChannelAliases += ["AntiKt4EMTopoNoEl->AntiKt4EMTopo"]
BTaggingFlags.CalibrationChannelAliases += ["AntiKt4TrackNoEl->AntiKt4EMTopo"]

# add algorithms in the sequence that calculate the b-tagging variables
# for the el-subtracted jets
from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit
FlavorTagInit(JetCollections = ["AntiKt4EMTopoNoElJets","AntiKt4TrackNoElJets"], Sequencer=exot4Seq)

# now schedule the sequence to re-calculate the MET using the electron-subtracted jets
from DerivationFrameworkJetEtMiss.METCommon import maplist
maplist.append('AntiKt4EMTopoNoEl') 
from DerivationFrameworkExotics.JetDefinitions import scheduleMETCustomClustJet
#from DerivationFrameworkJetEtMiss import METCommon

scheduleMETCustomClustJet ( "AntiKt4EMTopoNoEl", "CaloCalTopoClustersNoEl",outputlist = "EXOT4", configlist = "EXOT4" )

# we now have the el-subtracted jets, a new MET calculation using them and a b-tagging variable calculated
# using them as a seed


#=====================
# TRIGGER NAV THINNING
#=====================
# this removes some information in the trigger navigation
# the trigger navigation allows one to associate a trigger object, containing eta, phi, pt, etc
# with a trigger chain (ie: HLT_e30), so that one can know if that trigger object
# is matched to a chain that has been triggered
# this feature is used for the trigger matching cuts in all analyses
# however, this takes a lot of space and the analyses that use
# EXOT4 only need the electron and muon trigger chains
# new studies are being done in the ttbar resonances analysis to
# recover the 30% muon trigger inefficiency by OR'ing it with an MET trigger
# the monotop HQT analysis is also using the MET trigger for the top + MET signal
# It is not clear whether the HLT_xe.* chains navigation informaiton are necessary
# (since no trigger matching is done with the MET trigger), but they are kept for
# debugging purposes (studies can then match the truth neutrino to the MET trigger direction)
# Jet triggers were also studied in some cases, to recover the 30% muon trigger inefficiency
# but they have been dropped due to size considerations, given that the MET trigger
# for now seems to do better
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
EXOT4ThinningHelper = ThinningHelper( "EXOT4ThinningHelper" )

#trigger navigation content
#EXOT4ThinningHelper.TriggerChains = 'HLT_j.*|HLT_e.*|HLT_mu.*|HLT_xe.*'
EXOT4ThinningHelper.TriggerChains = 'HLT_e.*|HLT_mu.*|HLT_xe.*'
EXOT4ThinningHelper.AppendToStream( EXOT4Stream )

# add MC sum of weights in the CutBookkeeper metadata information
# this also adds the sum of MC*PDF weights calculated from LHAPDF
# to calculate the normalisation change induced by the PDF reweighting
# this is important for analyses that wnat to decouple the PDF norm. variation effect
# from the PDF effect in the cross section, because the PDF norm. variation is
# also considered in the cross section, but usually at a higher order in the ME calculation
from DerivationFrameworkCore.WeightMetadata import *

# add the sum of generator weights in the CutBookkeeper metadata information
from DerivationFrameworkCore.LHE3WeightMetadata import *


#=======================================
# SKIMMING   
#=======================================
# now apply the skimming
expression_lep = ''
expression_jet = ''

# check if we are using MC
# if yes, do not apply any trigger skimming, otherwise we
# cannot do trigger efficiency studies in MC
SkipTriggerRequirement=(globalflags.DataSource()=='geant4')
if SkipTriggerRequirement:
    expression_trige = "1"
    expression_trigmu = "1"
else:
    # note that the muon triggers are OR'ed with the MET triggers
    # that is done to recover the muon trigger inefficiency in data
    # studies on this are on going in the tt resonances group
    expression_trige = '(HLT_e24_lhmedium_L1EM20VH || HLT_e60_lhmedium || HLT_e120_lhloose || HLT_e24_lhtight_nod0_ivarloose || HLT_e60_lhmedium_nod0 || HLT_e140_lhloose_nod0 || HLT_e60_medium || HLT_e300_etcut ||  HLT_e26_lhtight_nod0_ivarloose )'
    expression_trigmu = '(HLT_mu20_iloose_L1MU15 || HLT_mu50 || HLT_mu24_ivarmedium || HLT_mu50 || HLT_xe90_mht_L1XE50 || HLT_xe80_tc_lcw_L1XE50 || HLT_mu24_iloose || HLT_mu24_ivarloose || HLT_mu40 || HLT_mu24_imedium || HLT_mu26_ivarmedium || HLT_mu26_imedium )'
# now make up the skimming selection expression
# the pseudo-logic for the lepton selection is:
#   --> [ (at least one loose electron (analyses use medium electron for the control regions) AND el. trigger) OR ..
#   -->   (at least one good non-isolated muon (analyses use non-isolated muons for the control regions) AND (mu. trigger OR MET trigger) ]
expression_lep = '(((count(Electrons.DFCommonElectronsLHLoose  && Electrons.pt > 15*GeV && Electrons.eta > -2.7 && Electrons.eta < 2.7)>0) && '+expression_trige+') || (( count(Muons.DFCommonGoodMuon && Muons.DFCommonMuonsPreselection && Muons.pt > 15*GeV && Muons.eta > -2.7 && Muons.eta < 2.7)>0) && '+expression_trigmu+'))'
# the jet selection is also configured as:
#   --> [ (at least 2 jets (analyses use the 2 jet bin in the W+jets control regions with pT > 25 GeV, but leave room for syst. variations) ) OR ...
#         (at least one akt10 trimmed jet with a pT of 150 GeV) ]
# Signal Region in tt resonances has 2 channels: resolved with >= 4 akt4 jets and boosted with >= 1 akt10 jet with pT > 300 GeV
# however the W+jets CR in tt resonances uses >= 2 jets in the W+jets CR
# studies in the W+jets CR in the boosted scenario also use akt10 with pT > 200 GeV
# furthermore, there are studies to define a semi-boosted ttbar resonance channel, which would use
# the akt10 jet for the W, with lower pT cuts
# other analyses, such as SS+jets also historically asked for looser cuts
# other analysis have also asked to require only 2 track jets for particular studies
expression_jet = '(((count((AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 15*GeV) && (abs(AntiKt4EMTopoJets.DFCommonJets_Calib_eta)<4.7)) >= 2)) || (count (AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_pt > 150*GeV && (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_eta)<2.2)) >0 ) || (count (AntiKt4PV0TrackJets.pt > 10*GeV && AntiKt4PV0TrackJets.eta > -2.5 && AntiKt4PV0TrackJets.eta< 2.5) >=2) )'
    
# now create a skimming tool to apply those cuts on leptons
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EXOT4StringSkimmingTool_lep = DerivationFramework__xAODStringSkimmingTool(name = "EXOT4StringSkimmingTool_lep",
                                                                         expression = expression_lep)

ToolSvc += EXOT4StringSkimmingTool_lep
print EXOT4StringSkimmingTool_lep

# create an independent skimming tool to apply those cuts on jets
EXOT4StringSkimmingTool_jet = DerivationFramework__xAODStringSkimmingTool(name = "EXOT4StringSkimmingTool_jet",
                                                                         expression = expression_jet)

ToolSvc += EXOT4StringSkimmingTool_jet
print EXOT4StringSkimmingTool_jet

# those two tools will be applied in the EXOT4 kernel, so that effectively we should get an or of them
# the reason why there are two tools is that we avoid making the large-R jets on-the-fly calibration for
# some of the events if we first
# apply the lepton selection and only for the events that pass the lepton selection, we apply
# the large-R jet calibration

#=======================================
# THINNING
#=======================================

# define thinning tool 
thinningTools=[]
# Tracks associated with jets (for e-in-jet OR)
# electron-in-jet studies still benefit from knowing which tracks
# are matched to the jets, to identify if the electron is in the jet
# for this reason, keep all tracks that have been matched to this jet
# these are normal jets, since those studies are done pre-electron-in-jet subtraction
# to estimate the benefits of doing it in the first place
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
EXOT4JetTPThinningTool = DerivationFramework__JetTrackParticleThinning(    	name                    = "EXOT4JetTPThinningTool",
                                                                                        ThinningService         = EXOT4ThinningHelper.ThinningSvc(),
                                                                                        JetKey                  = "AntiKt4EMTopoJets",
                                                                                        SelectionString         = "AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 15*GeV && AntiKt4EMTopoJets.DFCommonJets_Calib_eta > -2.8 && AntiKt4EMTopoJets.DFCommonJets_Calib_eta < 2.8",
                                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT4JetTPThinningTool
thinningTools.append(EXOT4JetTPThinningTool)

# also keep tracks matched to the C/A 1.5 jets
# this jet is used for the HEPTopTagger studies
# some VLQ analyses expressed interest in using the HEPTopTagger (specifically Takuya Tashiro)
# another use of this, is to perform HEPTopTagger performance studies in a lepton+jets sample
# to identify the top tagging performance (contact: David Sosa/Christoph Anders)
#FIX #ATLJETMET-744
#EXOT4CA15JetTPThinningTool = DerivationFramework__JetTrackParticleThinning(name                    = "EXOT4CA15JetTPThinningTool",
#                                                                       ThinningService         = EXOT4ThinningHelper.ThinningSvc(),
#                                                                       JetKey                  = "CamKt15LCTopoJets",
#                                                                       SelectionString         = "CamKt15LCTopoJets.pt > 150*GeV && CamKt15LCTopoJets.eta > -2.7 && CamKt15LCTopoJets.eta < 2.7",
#                                                                       InDetTrackParticlesKey  = "InDetTrackParticles")
#ToolSvc += EXOT4CA15JetTPThinningTool
#thinningTools.append(EXOT4CA15JetTPThinningTool)


# Keep tracks associated with muons
# the ConeSize is zero, so that we keep only the tracks that are ElementLink'ed
# to the muons (actually the tracks, towards which the muon has an ElementLink)
# this is important for the muon calibration
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
EXOT4MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                       = "EXOT4MuonTPThinningTool",
                                                                            ThinningService         = EXOT4ThinningHelper.ThinningSvc(),
                                                                            MuonKey                 = "Muons",
                                                                            InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                            SelectionString = "Muons.pt > 7*GeV",
                                                                            ConeSize=0)
ToolSvc += EXOT4MuonTPThinningTool
thinningTools.append(EXOT4MuonTPThinningTool)

# Keep tracks associated with electrons also for the electron calibration
# electron-in-jet studies also benefit
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EXOT4ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(    	name                    = "EXOT4ElectronTPThinningTool",
                                                                                        ThinningService         = EXOT4ThinningHelper.ThinningSvc(),
                                                                                        SGKey                   = "Electrons",
                                                                                        InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                        GSFTrackParticlesKey    = "GSFTrackParticles",
                                                                                        SelectionString         = "Electrons.pt > 7*GeV",
                                                                                        ConeSize = 0
                                                                              )
ToolSvc += EXOT4ElectronTPThinningTool
thinningTools.append(EXOT4ElectronTPThinningTool)

# calo cluster thinning
# Keep the topo-cluster to which the electron is ElementLink'ed
# needed for calibration and electron-in-jet overlap studies
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
#EXOT4ElectronCCThinningTool = DerivationFramework__CaloClusterThinning( name                  = "EXOT4ElectronCCThinningTool",
#                                                                                     ThinningService         = EXOT4ThinningHelper.ThinningSvc(),
#                                                                                     SGKey             	     = "Electrons",
#                                                                                     CaloClCollectionSGKey   = "egammaClusters",
#                                                                                     TopoClCollectionSGKey   = "CaloCalTopoClusters",
#                                                                                     SelectionString         = "Electrons.pt > 7*GeV",
#                                                                                     #FrwdClCollectionSGKey   = "LArClusterEMFrwd",
#                                                                                     ConeSize                = 0)
#ToolSvc += EXOT4ElectronCCThinningTool
#thinningTools.append(EXOT4ElectronCCThinningTool)

# Keep topoclusters which are in the list of constituents of the CA 1.5
# jets
# HEPTopTagger is the only use case for CA 1.5 jets and it is done from the derivations
# that means that the HEPTopTagger algorithm does not save variables to cut on
# in the derivations: it needs the bare topo clusters to make those variables offline
# when running the analysis
#FIX #ATLJETMET-744
#from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__JetCaloClusterThinning
#EXOT4CA15CCThinningTool = DerivationFramework__JetCaloClusterThinning(name                    = "EXOT4CA15CCThinningTool",
#                                                                       ThinningService         = EXOT4ThinningHelper.ThinningSvc(),
#                                                                       SGKey                   = "CamKt15LCTopoJets",
#                                                                       TopoClCollectionSGKey   = "CaloCalTopoClusters",
#                                                                       SelectionString         = "CamKt15LCTopoJets.pt > 150*GeV")
#ToolSvc += EXOT4CA15CCThinningTool
#thinningTools.append(EXOT4CA15CCThinningTool)

# Keep topoclusters to which the akt10 trimmed jet is ElementLink'ed to
# Useful for performance studies and to rerun taggers offline
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__JetCaloClusterThinning
EXOT4Ak10CCThinningTool = DerivationFramework__JetCaloClusterThinning(name                    = "EXOT4Ak10CCThinningTool",
                                                                       ThinningService         = EXOT4ThinningHelper.ThinningSvc(),
                                                                       SGKey                   = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                                                       TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                                       SelectionString         = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_pt > 150*GeV",
                                                                       AdditionalClustersKey = ["LCOriginTopoClusters"])
ToolSvc += EXOT4Ak10CCThinningTool
thinningTools.append(EXOT4Ak10CCThinningTool)


# Set up set of thinning tools for the truth information
# this is done in steps simply to keep the top decay record intact
# most of this will destroy mother-daughter information, but we mostly care about the
# top decay products in the HQT analysis and the hard ME information

# this keeps the tau product and quark hadronization products for us to follow the type of decays
# and associate them with truth jets
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
EXOT4MCThinningTool = DerivationFramework__MenuTruthThinning(name = "EXOT4MCThinningTool",
                                                             ThinningService         = EXOT4ThinningHelper.ThinningSvc(),
	                                                     WritePartons               = False,
	                                                     WriteHadrons               = True,
	                                                     WriteBHadrons              = True,
	                                                     WriteGeant                 = False,
	                                                     GeantPhotonPtThresh        = -1.0,
	                                                     WriteTauHad                = True,
	                                                     PartonPtThresh             = -1.0,
	                                                     WriteBSM                   = False,
	                                                     WriteBosons                = False,
	                                                     WriteBSMProducts           = False,
	                                                     WriteTopAndDecays          = False,
	                                                     WriteEverything            = False,
	                                                     WriteAllLeptons            = False,
	                                                     WriteLeptonsNotFromHadrons         = True,
	                                                     WriteStatus3               = False,
	                                                     WriteFirstN                = -1,
                                                             WritettHFHadrons           = True,
                                                             PreserveDescendants        = False)

# this should keep BSM decays and top decays
# this is big in ttbar and in BSM (signal) samples, but it should be minimal otherwise
EXOT4TMCThinningTool = DerivationFramework__MenuTruthThinning(name = "EXOT4TMCThinningTool",
                                                             ThinningService         = EXOT4ThinningHelper.ThinningSvc(),
	                                                     WritePartons               = False,
	                                                     WriteHadrons               = False,
	                                                     WriteBHadrons              = False,
	                                                     WriteGeant                 = False,
	                                                     GeantPhotonPtThresh        = -1.0,
	                                                     WriteTauHad                = False,
	                                                     PartonPtThresh             = -1.0,
	                                                     WriteBSM                   = True,
	                                                     WriteBosons                = True,
	                                                     WriteBSMProducts           = True,
	                                                     WriteTopAndDecays          = True,
	                                                     WriteEverything            = False,
	                                                     WriteAllLeptons            = False,
	                                                     WriteLeptonsNotFromHadrons         = True,
	                                                     WriteStatus3               = False,
	                                                     WriteFirstN                = -1,
                                                             PreserveDescendants        = True)

# the hard ME information is stored in the first records, so keep those
# setting the WriteFirstN to 10
# that should be more than necessary for the hard matrix element only
# it also costs very little in size
# also keep top and decays here, to be sure we are saving it, although the previous one
# should also keep it
EXOT4TAMCThinningTool = DerivationFramework__MenuTruthThinning(name = "EXOT4TAMCThinningTool",
                                                             ThinningService         = EXOT4ThinningHelper.ThinningSvc(),
	                                                     WritePartons               = False,
	                                                     WriteHadrons               = False,
	                                                     WriteBHadrons              = False,
	                                                     WriteGeant                 = False,
	                                                     GeantPhotonPtThresh        = -1.0,
	                                                     WriteTauHad                = False,
	                                                     PartonPtThresh             = -1.0,
	                                                     WriteBSM                   = False,
	                                                     WriteBosons                = False,
	                                                     WriteBSMProducts           = False,
	                                                     WriteTopAndDecays          = True,
	                                                     WriteEverything            = False,
	                                                     WriteAllLeptons            = False,
	                                                     WriteLeptonsNotFromHadrons         = True,
	                                                     WriteStatus3               = False,
	                                                     WriteFirstN                = 10,
                                                             PreserveDescendants        = False,
                                                             PreserveAncestors          = True)

# also explicitly set this tool to keep BSM signal particles used in the analyses
# that, at the time of writing were not yet included in the set of particles to be considered when
# WriteBSM is activated
# if WriteBSM has been updated in the tool above to include these particles, adding this anyway does not
# hurt
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
EXOT4MCGenThinningTool = DerivationFramework__GenericTruthThinning(name = "EXOT4MCGenThinningTool",
                                                                ThinningService         = EXOT4ThinningHelper.ThinningSvc(),
                                                                ParticleSelectionString = "abs(TruthParticles.pdgId) ==25 || abs(TruthParticles.pdgId)==39 || abs(TruthParticles.pdgId)==32 || abs(TruthParticles.pdgId)==5100021", 
                                                                PreserveDescendants = False)

# all truth thinning tools set up, so add them in the sequence now
from AthenaCommon.GlobalFlags import globalflags
if globalflags.DataSource()=='geant4':
    ToolSvc += EXOT4MCThinningTool
    thinningTools.append(EXOT4MCThinningTool)
    ToolSvc += EXOT4TMCThinningTool
    thinningTools.append(EXOT4TMCThinningTool)
    ToolSvc += EXOT4TAMCThinningTool
    thinningTools.append(EXOT4TAMCThinningTool)
    ToolSvc += EXOT4MCGenThinningTool
    thinningTools.append(EXOT4MCGenThinningTool)


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

# set up the lepton skimming in the kernel
# note that the reason it is done here is to veto events on which
# no lepton is found
# this way the code below it is only executed if
# the event passed the lepton requirement, reducing RAM consumption
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += exot4Seq
exot4Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT4Kernel_lep", SkimmingTools = [EXOT4StringSkimmingTool_lep])


from DerivationFrameworkExotics.JetDefinitions import *
from JetRec.JetRecStandard import jtm
from JetRec.JetRecConf import JetAlgorithm

augTools = []

# this classifies leptons into background or signal leptons depending on
# where they come from
# it is an augmentation that adds this flag in the output derivation
# it is important for the tt resonances QCD estimation studies
# which try to identify whether a lepton is prompt or not in
# the MC background it subtracts from data in the QCD control region
# (used to estimate the fake rate of lepton-to-jet misid)
if isMC:
   from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
   # Includes the GenFilterTool
   # https://twiki.cern.ch/twiki/bin/view/AtlasProtected/MergingHTMETSamplesttWt
   addStandardTruthContents()
   from DerivationFrameworkMCTruth.HFHadronsCommon import *

   from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthClassificationDecorator
   EXOT4ClassificationDecorator = DerivationFramework__TruthClassificationDecorator(
                                 name              = "EXOT4ClassificationDecorator",
                                 ParticlesKey      = "TruthParticles",
                                 MCTruthClassifier = ToolSvc.DFCommonTruthClassifier) 
   ToolSvc += EXOT4ClassificationDecorator
   augTools.append(EXOT4ClassificationDecorator)
   from MCTruthClassifier.MCTruthClassifierBase import MCTruthClassifier as BkgElectronMCTruthClassifier   
   from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__BkgElectronClassification
   BkgElectronClassificationTool = DerivationFramework__BkgElectronClassification (
                                    name = "EXOT4BkgElectronClassificationTool",
                                    MCTruthClassifierTool = BkgElectronMCTruthClassifier)
   ToolSvc += BkgElectronClassificationTool
   augTools.append(BkgElectronClassificationTool)

   
# now make akt10 trimmed truth collection for MC studies
# also make the akt4 truth and TruthWZ jets
# TruthWZ jets are important for the e-in-jet overlap removal studies
# since they exclude leptons when making the jet
# usually the akt4 truth jets (without WZ in the name) are already made
# not sure why one needs to redo them here
#
#=======================================
# JETS
#=======================================

#restore AOD-reduced jet collections
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets
OutputJets["EXOT4"] = []
reducedJetList = [
    "AntiKt2PV0TrackJets", #flavour-tagged automatically
    "AntiKt4PV0TrackJets",
    "AntiKt10TruthJets",
    "AntiKt10LCTopoJets"]
replaceAODReducedJets(reducedJetList,exot4Seq,"EXOT4")

#AntiKt10*PtFrac5SmallR20Jets must be scheduled *AFTER* the other collections are replaced
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addDefaultTrimmedJets
addDefaultTrimmedJets(exot4Seq,"EXOT4")

#some jets collections are not included in the new jet restoring mechanism and need to be added the old way

# also add the C/A 1.5 jet for the HEPTopTagger studies
# this is not standard so must be done here
#FIX #ATLJETMET-744
#NOTE this is the old addStandardJets method
#addStandardJets("CamKt", 1.5, "LCTopo", mods = "calib_notruth", algseq = exot4Seq, outputGroup = "EXOT4") #CamKt15LCTopoJets

#jet calibration
# now apply the jet calibrations for the one that we can apply one
# this should make the jet four-momentum we skim on more realistic
# but this is never saved in the output derivation
# it also does not mean we can raise the pT threshold a lot, because
# the jets pT will vary under systematics in the analysis
applyJetCalibration_xAODColl("AntiKt4EMTopo", exot4Seq)
applyJetCalibration_xAODColl("AntiKt4EMTopoNoEl", exot4Seq)
applyJetCalibration_CustomColl("AntiKt10LCTopoTrimmedPtFrac5SmallR20", exot4Seq)


#=======================================
# SKIMMING, THINNING, AUGMENTATION
#=======================================

# now apply the jet skimming part, since we have all the elements
exot4Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT4Kernel_jet", SkimmingTools = [EXOT4StringSkimmingTool_jet])

# now that we have the skimming, done, do the thinning and augmentation in the next kernel
# the reason it is done here, is only to guarantee that this is only done for the events that pass
# the jet skimming done above
exot4Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT4Kernel", ThinningTools = thinningTools, AugmentationTools = augTools)


# these lines were removed, due to the change in structure
# needed to create EXOT4's own sequence
# in the past all derivations ran in a single sequence
# the sequence separation makes it easier to avoid that one derivation
# influences the other a lot
#
# SPECIAL LINES FOR THINNING
# Thinning service name must match the one passed to the thinning tools 
#from AthenaServices.Configurables import ThinningSvc, createThinningSvc
#augStream = MSMgr.GetStream( streamName )
#evtStream = augStream.GetEventStream()
#svcMgr += createThinningSvc( svcName="EXOT4ThinningSvc", outStreams=[evtStream] )


# now that all is said and done, we need to save it all
# save only what we want for EXOT4
# and therefore, everything that was auxiliary will be dropped
# these lists are defined in python/EXOT4ContentList.py
#
#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
# Currently using same list as EXOT4
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkExotics.EXOT4ContentList import *
EXOT4SlimmingHelper = SlimmingHelper("EXOT4SlimmingHelper")

EXOT4SlimmingHelper.SmartCollections = EXOT4SmartCollections

EXOT4SlimmingHelper.ExtraVariables = EXOT4ExtraVariables

# only add truth lists if in truth
# in the past this was generating bugs, but I
# think now there is a check somewhere in the basic
# infra-structure
EXOT4SlimmingHelper.AllVariables = EXOT4AllVariables
if globalflags.DataSource()=='geant4':
    EXOT4SlimmingHelper.AllVariables += EXOT4AllVariablesTruth

EXOT4SlimmingHelper.StaticContent = EXOT4Content
if globalflags.DataSource()=='geant4':
    EXOT4SlimmingHelper.StaticContent.extend(EXOT4TruthContent)


#EXOT4SlimmingHelper.AppendToDictionary = {}
#addJetOutputs(EXOT4SlimmingHelper, ["EXOT4"])

# note that we add the jets outputs, but not
# directly in AllVariables or in StaticContent
# we want to add only the variables of the jets that we use
# there was a large effort to single them out and put them in ExtraVariables
# this way we reduced significantly the amount of disk space used

# veto the truth jet, because we will add it in the ExtraVariables manually
#addJetOutputs(EXOT4SlimmingHelper, ["EXOT4"], ["AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets", "AntiKt4LCTopoJets", "AntiKt4EMTopoJets"], ["AntiKt10TruthTrimmedPtFrac5SmallR20Jets", "CamKt15LCTopoJets"])#FIX #ATLJETMET-744
addJetOutputs(EXOT4SlimmingHelper, ["EXOT4"], ["AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets", "AntiKt4LCTopoJets", "AntiKt4EMTopoJets"], ["AntiKt10TruthTrimmedPtFrac5SmallR20Jets"])#FIX #ATLJETMET-744

#listJets = ['CamKt15LCTopoJets', 'AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets']#FIX #ATLJETMET-744
listJets = ['AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets']#FIX #ATLJETMET-744
if globalflags.DataSource()=='geant4':
  listJets.extend(['AntiKt10TruthTrimmedPtFrac5SmallR20Jets'])
# need to add the jets that are made on-the-fly into the dictionary, otherwise there is a crash
# for the ones already in the input xAOD, this is done automatically
for i in listJets:
  EXOT4SlimmingHelper.AppendToDictionary[i] = 'xAOD::JetContainer'
  # this line is fundamental! Jets are unreadable without it!
  EXOT4SlimmingHelper.AppendToDictionary[i+'Aux'] = 'xAOD::JetAuxContainer'
  # hand picked list of variables to save -- save disk space
  EXOT4SlimmingHelper.ExtraVariables +=[i,i+'.pt.eta.phi.m.ECF1.ECF2.ECF3.Tau1_wta.Tau2_wta.Tau3_wta.Split12.Split23.NTrimSubjets.Parent.GhostAntiKt2TrackJet"']

# now do the same for the version of the jets that had the electrons subtracted from them
# again: hand picked list of extra variable to select only what is needed in the analysis
listNewJets = ['AntiKt4TrackNoElJets','AntiKt4EMTopoNoElJets','AntiKt4TruthNoElJets']
for jet in listNewJets :
   
   EXOT4SlimmingHelper.AppendToDictionary[jet] = 'xAOD::JetContainer'
   EXOT4SlimmingHelper.AppendToDictionary[jet+'Aux'] = 'xAOD::JetAuxContainer'
   EXOT4SlimmingHelper.ExtraVariables +=[jet,jet+".pt.eta.phi.m.JetConstitScaleMomentum_pt.JetConstitScaleMomentum_eta.JetConstitScaleMomentum_phi.JetConstitScaleMomentum_m.HECFrac.HECQuality.JVF.LArQuality.NegativeE.NumTrkPt1000.NumTrkPt500.SumPtTrkPt500.Timing.TrackWidthPt1000.Width.ActiveArea4vec_eta.ActiveArea4vec_m.ActiveArea4vec_phi.ActiveArea4vec_pt.AverageLArQF.EMFrac.EnergyPerSampling.FracSamplingMax.GhostMuonSegment.GhostMuonSegmentCount.OriginCorrected.PileupCorrected.DetectorEta.JetOriginConstitScaleMomentum_pt.JetPileupScaleMomentum_pt.JetPileupScaleMomentum_eta.JetPileupScaleMomentum_phi.JetPileupScaleMomentum_m.JetEtaJESScaleMomentum_pt.JetEtaJESScaleMomentum_eta.JetEtaJESScaleMomentum_phi.JetEtaJESScaleMomentum_m.JetGSCScaleMomentum_pt.JetGSCScaleMomentum_eta.JetGSCScaleMomentum_phi.JetGSCScaleMomentum_m.JetInsituScaleMomentum_pt.JetInsituScaleMomentum_eta.JetInsituScaleMomentum_phi.JetInsituScaleMomentum_m.constituentLinks.btaggingLink.GhostBHadronsFinal.GhostBHadronsInitial.GhostBQuarksFinal.GhostCHadronsFinal.GhostCHadronsInitial.GhostCQuarksFinal.GhostHBosons.GhostPartons.GhostTQuarksFinal.GhostTausFinal.GhostWBosons.GhostZBosons.GhostTruth.OriginVertex.GhostAntiKt3TrackJet.GhostAntiKt4TrackJet.GhostTrack.GhostTruthAssociationLink.HighestJVFVtx.JetOriginConstitScaleMomentum_eta.JetOriginConstitScaleMomentum_m.JetOriginConstitScaleMomentum_phi.JvtJvfcorr.JvtRpt.Jvt.FracSamplingMaxIndex.LeadingClusterPt.ECPSFraction.N90Constituents.LeadingClusterSecondLambda.LeadingClusterCenterLambda.LeadingClusterSecondR.CentroidR.OotFracClusters5.OotFracClusters10.ConeTruthLabelID.PartonTruthLabelID.HadronConeExclTruthLabelID"]
   
# same procedure to add in the dictionary needs to be done for the btagging information generated on the fly
# again: hand picked list of variables to save is given
listBtagNoEl=['BTagging_AntiKt4EMTopoNoEl','BTagging_AntiKt4TrackNoEl']

for btag in listBtagNoEl :
   EXOT4SlimmingHelper.AppendToDictionary[btag] = 'xAOD::BTaggingContainer'
   EXOT4SlimmingHelper.AppendToDictionary[btag+'Aux']= 'xAOD::AuxContainerBase' 
   EXOT4SlimmingHelper.ExtraVariables +=[btag+".SV1_pb.SV1_pu.IP3D_pb.IP3D_pu.MV2c10_discriminant.MSV_vertices.MV1c_discriminant.SV1_badTracksIP.SV1_vertices.BTagTrackToJetAssociator.BTagTrackToJetAssociatorBB.JetFitter_JFvertices.JetFitter_tracksAtPVlinks.MSV_badTracksIP.MV2c100_discriminant"]

# central JetEtMiss function to save the MET necessary information
addMETOutputs(EXOT4SlimmingHelper, ["Track", "EXOT4","NoEl"], ["AntiKt4EMTopo"])

# this saves the trigger objects generated by the trigger
# that is: if the trigger finds an electron at a specific eta and phi,
# this will save the trigger electron object
# the trigger navigation associates this object with a HLT_XXX chain
# this is eta/phi-matched with the offline electron or muon objects
# and the trigger navigation is checked to know if the offline object detected
# corresponds to the same object that is matched to the successful trigger chain
EXOT4SlimmingHelper.IncludeEGammaTriggerContent = True
EXOT4SlimmingHelper.IncludeMuonTriggerContent = True

# also keep MET trigger object information for studies
EXOT4SlimmingHelper.IncludeEtMissTriggerContent = True

addOriginCorrectedClusters(EXOT4SlimmingHelper, writeLC = True, writeEM = True)


EXOT4SlimmingHelper.AppendContentToStream(EXOT4Stream)
