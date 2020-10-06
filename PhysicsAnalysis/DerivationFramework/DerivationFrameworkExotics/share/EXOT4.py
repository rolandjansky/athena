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
from DerivationFrameworkFlavourTag.HbbCommon import addVRJets

# write heavy flavour hadron information in MC
if DerivationFrameworkHasTruth:
    from DerivationFrameworkMCTruth.HFHadronsCommon import *

# utilities used to make jets on-the-fly
from JetRec.JetRecStandard import jtm
from JetRec.JetRecConf import PseudoJetGetter
from JetRecTools.JetRecToolsConf import TrackPseudoJetGetter
from JetRecTools.JetRecToolsConf import JetInputElRemovalTool
from BTagging.BTaggingConfiguration import defaultTrackAssoc, defaultMuonAssoc


# make EXOT4 sequence
exot4Seq = CfgMgr.AthSequencer("EXOT4Sequence")

#====================================================================
# SET UP STREAM
#====================================================================
streamName = derivationFlags.WriteDAOD_EXOT4Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EXOT4Stream )
EXOT4Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT4Stream.AcceptAlgs(["EXOT4Kernel"])


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
EXOT4ThinningHelper.TriggerChains = 'HLT_e.*|HLT_mu.*|HLT_xe.*|HLT_ht1000_L1J100|HLT_ht850_L1J100|HLT_4j100|HLT_noalg_L1J400'
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

# extra triggers (regardless of lepton ID, data only)
triglist = []
triglist.append("HLT_xe35")
triglist.append("HLT_xe50")
triglist.append("HLT_xe60")
triglist.append("HLT_xe70")
triglist.append("HLT_xe80")
triglist.append("HLT_xe80_L1XE50")
triglist.append("HLT_xe80_L1XE70")
triglist.append("HLT_xe100")
triglist.append("HLT_j80_xe80_dphi1_L1J40_DPHI-J20XE50")
triglist.append("HLT_j80_xe80_dphi1_L1J40_DPHI-J20s2XE50")
triglist.append("HLT_j100_xe80_L1J40_DPHI-J20XE50")
triglist.append("HLT_j100_xe80_L1J40_DPHI-J20s2XE50")
triglist.append("HLT_xe80_tc_lcw_L1XE50") # added on Apr 2016
triglist.append("HLT_xe90_tc_lcw_L1XE50")
triglist.append("HLT_xe100_tc_lcw_L1XE50")
triglist.append("HLT_xe110_tc_lcw_L1XE60")
triglist.append("HLT_xe80_mht_L1XE50")
triglist.append("HLT_xe90_mht_L1XE50")
triglist.append("HLT_xe100_mht_L1XE50")
triglist.append("HLT_xe100_mht_L1XE60")
triglist.append("HLT_xe110_mht_L1XE50") # added on Aug 2016
triglist.append("HLT_xe110_mht_L1XE50_AND_xe70_L1XE50") # added on Sep 2016
triglist.append("HLT_xe130_mht_L1XE50") # added on Aug 2016
triglist.append("HLT_xe90_L1XE50")
triglist.append("HLT_xe100_L1XE50")
triglist.append("HLT_xe110_L1XE60")
triglist.append("HLT_xe80_tc_em_L1XE50")
triglist.append("HLT_xe90_tc_em_L1XE50")
triglist.append("HLT_xe100_tc_em_L1XE50")
triglist.append("HLT_xe80_tc_lcw")
triglist.append("HLT_xe90_tc_lcw")
triglist.append("HLT_xe100_tc_lcw")
triglist.append("HLT_xe90_mht")
triglist.append("HLT_xe100_mht")
triglist.append("HLT_xe90_tc_lcw_wEFMu_L1XE50")
triglist.append("HLT_xe90_mht_wEFMu_L1XE50")
triglist.append("HLT_xe120_pueta")
triglist.append("HLT_xe120_pufit")
triglist.append("HLT_e15_lhtight_ivarloose_3j20_L1EM13VH_3J20")
triglist.append("HLT_mu14_ivarloose_3j20_L1MU10_3J20")
triglist.append("HLT_xe100_tc_lcw_L1XE60") # added on Jun 2016
triglist.append("HLT_xe110_tc_em_L1XE50")
triglist.append("HLT_xe110_tc_em_wEFMu_L1XE50")
triglist.append("HLT_xe120_pueta_wEFMu")
triglist.append("HLT_xe120_mht")
triglist.append("HLT_xe120_tc_lcw")
triglist.append("HLT_xe120_mht_wEFMu")
triglist.append("HLT_xe110_L1XE50")
triglist.append("HLT_xe100_L1XE60")
triglist.append("HLT_xe120_pufit_wEFMu")
triglist.append("HLT_xe120_tc_lcw_wEFMu")
triglist.append("HLT_xe120_tc_em")
triglist.append("HLT_xe110_pufit_L1XE60")
triglist.append("HLT_xe120_pufit_L1XE60")
triglist.append("HLT_xe120_mht_xe80_L1XE60")
triglist.append("HLT_xe110_pufit_L1XE55")
triglist.append("HLT_xe120_pufit_L1XE55")
triglist.append("HLT_xe120_mht_xe80_L1XE55")
triglist.append("HLT_xe110_pufit_L1XE50")
triglist.append("HLT_xe120_pufit_L1XE50")
triglist.append("HLT_xe120_mht_xe80_L1XE50")

expression_trigmet = '('+'||'.join(triglist)+')'

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
    #list of all un pre-scaled MET triggers
    MET_triggers = 'HLT_xe70 || HLT_xe70_mht || HLT_xe90_mht_L1XE50 || HLT_xe100_mht_L1XE50 || HLT_xe110_mht_L1XE50 || HLT_xe90_tc_lcw_L1XE50 || HLT_xe90_pufit_L1XE50 || HLT_xe100_pufit_L1XE55 || HLT_xe100_pufit_L1XE50 || HLT_xe110_pufit_L1XE50 || HLT_xe110_pufit_L1XE55 || HLT_xe110_pufit_xe70_L1XE50 || HLT_xe120_pufit_L1XE50 || HLT_xe110_pufit_xe65_L1XE55 || HLT_xe120_pufit_L1XE55 || HLT_xe100_pufit_xe75_L1XE60 || HLT_xe110_pufit_xe65_L1XE60 || HLT_xe120_pufit_L1XE60'

    #adding the MET triggers in OR with both single electron and muon triggers
    expression_trige = '(HLT_e24_lhmedium_L1EM20VH || HLT_e60_lhmedium || HLT_e120_lhloose || HLT_e24_lhtight_nod0_ivarloose || HLT_e60_lhmedium_nod0 || HLT_e140_lhloose_nod0 || HLT_e60_medium || HLT_e300_etcut ||  HLT_e26_lhtight_nod0_ivarloose || '+MET_triggers+' )'
    expression_trigmu = '(HLT_mu20_iloose_L1MU15 || HLT_mu50 || HLT_mu24_ivarmedium || HLT_mu50 || HLT_mu24_iloose || HLT_mu24_ivarloose || HLT_mu40 || HLT_mu24_imedium || HLT_mu26_ivarmedium || HLT_mu26_imedium || '+MET_triggers+' )'

# now make up the skimming selection expression
# the pseudo-logic for the lepton selection is:
#   --> [ (at least one loose electron (analyses use medium electron for the control regions) AND el. trigger) OR ..
#   -->   (at least one good non-isolated muon (analyses use non-isolated muons for the control regions) AND (mu. trigger OR MET trigger) ]
expression_lep = '(((count(Electrons.DFCommonElectronsLHMedium  && Electrons.pt > 20*GeV && Electrons.eta > -2.7 && Electrons.eta < 2.7)>0) && '+expression_trige+') || (( count(Muons.DFCommonGoodMuon && Muons.DFCommonMuonsPreselection && Muons.pt > 20*GeV && Muons.eta > -2.7 && Muons.eta < 2.7)>0) && '+expression_trigmu+'))'


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
expression_jet = '((count (AntiKt4EMPFlowJets.DFCommonJets_Calib_pt > 15*GeV && abs(AntiKt4EMPFlowJets.DFCommonJets_Calib_eta) < 4.7) >= 2) || (count (AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 15*GeV && abs(AntiKt4EMTopoJets.DFCommonJets_Calib_eta) < 4.7) >= 2)|| (count (AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_pt > 150*GeV && (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_eta)<2.2)) >0 ) || (count (AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903.pt > 10*GeV &&  abs(AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903.eta) < 2.5) >=2))'
    
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
EXOT4JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name                    = "EXOT4JetTPThinningTool",
                                                                        ThinningService         = EXOT4ThinningHelper.ThinningSvc(),
                                                                        JetKey                  = "AntiKt4EMTopoJets",
                                                                        SelectionString         = "AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 15*GeV && AntiKt4EMTopoJets.DFCommonJets_Calib_eta > -2.8 && AntiKt4EMTopoJets.DFCommonJets_Calib_eta < 2.8",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT4JetTPThinningTool
thinningTools.append(EXOT4JetTPThinningTool)


# Keep tracks associated with muons
# the ConeSize is zero, so that we keep only the tracks that are ElementLink'ed
# to the muons (actually the tracks, towards which the muon has an ElementLink)
# this is important for the muon calibration
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
EXOT4MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name = "EXOT4MuonTPThinningTool",
                                                                        ThinningService         = EXOT4ThinningHelper.ThinningSvc(),
                                                                        MuonKey                 = "Muons",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                        SelectionString         = "Muons.pt > 7*GeV",
                                                                        ConeSize                = 0
                                                                        )
ToolSvc += EXOT4MuonTPThinningTool
thinningTools.append(EXOT4MuonTPThinningTool)

# Keep tracks associated with electrons also for the electron calibration
# electron-in-jet studies also benefit
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EXOT4ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning( name = "EXOT4ElectronTPThinningTool",
                                                                                ThinningService         = EXOT4ThinningHelper.ThinningSvc(),
                                                                                SGKey                   = "Electrons",
                                                                                InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                GSFTrackParticlesKey    = "GSFTrackParticles",
                                                                                SelectionString         = "Electrons.pt > 7*GeV",
                                                                                ConeSize                = 0
                                                                                )
ToolSvc += EXOT4ElectronTPThinningTool
thinningTools.append(EXOT4ElectronTPThinningTool)

# calo cluster thinning
# Keep the topo-cluster to which the electron is ElementLink'ed
# needed for calibration and electron-in-jet overlap studies


#Thinning tool for akt4 topoEM jets
#pT cut 7 GeV and |Eta| cut 3.0 
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__JetCaloClusterThinning
EXOT4Ak4CCThinningTool = DerivationFramework__JetCaloClusterThinning(name = "EXOT4Ak4CCThinningTool",
                                                                    ThinningService         = EXOT4ThinningHelper.ThinningSvc(),
                                                                    SGKey                   = "AntiKt4EMTopoJets",
                                                                    TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                                    SelectionString         = "AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 7*GeV && abs(AntiKt4EMTopoJets.DFCommonJets_Calib_eta) < 3",
                                                                    AdditionalClustersKey   = ["EMOriginTopoClusters","LCOriginTopoClusters","CaloCalTopoClusters"])
ToolSvc += EXOT4Ak4CCThinningTool
thinningTools.append(EXOT4Ak4CCThinningTool)


# Keep topoclusters to which the akt10 trimmed jet is ElementLink'ed to
# Useful for performance studies and to rerun taggers offline
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__JetCaloClusterThinning
EXOT4Ak10CCThinningTool = DerivationFramework__JetCaloClusterThinning(name = "EXOT4Ak10CCThinningTool",
                                                                    ThinningService         = EXOT4ThinningHelper.ThinningSvc(),
                                                                    SGKey                   = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                                                    TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                                    SelectionString         = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_pt > 150*GeV",
                                                                    AdditionalClustersKey   = ["LCOriginTopoClusters"])
ToolSvc += EXOT4Ak10CCThinningTool
thinningTools.append(EXOT4Ak10CCThinningTool)


#Calo cluster thinning for CSSK jets
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__JetCaloClusterThinning
EXOT4A10SoftDropThinningTool = DerivationFramework__JetCaloClusterThinning(name = "EXOT4A10SoftDropThinningTool",
                                                                        ThinningService         = EXOT4ThinningHelper.ThinningSvc(),
                                                                        SGKey                   = "AntiKt10LCTopoCSSKSoftDropBeta100Zcut10Jets",
                                                                        TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                                        SelectionString         = "AntiKt10LCTopoCSSKSoftDropBeta100Zcut10Jets.pt > 150*GeV",
                                                                        AdditionalClustersKey   = ["LCOriginCSSKTopoClusters"])
ToolSvc += EXOT4A10SoftDropThinningTool
thinningTools.append(EXOT4A10SoftDropThinningTool)


#Tracks associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
EXOT4TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name = "EXOT4TauTPThinningTool",
                                                                        ThinningService         = EXOT4ThinningHelper.ThinningSvc(),
                                                                        TauKey                  = "TauJets",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT4TauTPThinningTool
thinningTools.append(EXOT4TauTPThinningTool)


# Set up set of thinning tools for the truth information
# this is done in steps simply to keep the top decay record intact
# most of this will destroy mother-daughter information, but we mostly care about the
# top decay products in the HQT analysis and the hard ME information

# this keeps the tau product and quark hadronization products for us to follow the type of decays
# and associate them with truth jets
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
EXOT4MCThinningTool = DerivationFramework__MenuTruthThinning(name = "EXOT4MCThinningTool",
                                                            ThinningService             = EXOT4ThinningHelper.ThinningSvc(),
                                                            WritePartons                = False,
                                                            WriteHadrons                = True,
                                                            WriteBHadrons               = True,
                                                            WriteGeant                  = False,
                                                            GeantPhotonPtThresh         = -1.0,
                                                            WriteTauHad                 = True,
                                                            PartonPtThresh              = -1.0,
                                                            WriteBSM                    = False,
                                                            WriteBosons                 = False,
                                                            WriteBSMProducts            = False,
                                                            WriteTopAndDecays           = False,
                                                            WriteEverything             = False,
                                                            WriteAllLeptons             = False,
                                                            WriteLeptonsNotFromHadrons  = True,
                                                            WriteStatus3                = False,
                                                            WriteFirstN                 = -1,
                                                            WritettHFHadrons            = True,
                                                            PreserveDescendants         = False)

# this should keep BSM decays and top decays
# this is big in ttbar and in BSM (signal) samples, but it should be minimal otherwise
EXOT4TMCThinningTool = DerivationFramework__MenuTruthThinning(name = "EXOT4TMCThinningTool",
                                                            ThinningService             = EXOT4ThinningHelper.ThinningSvc(),
                                                            WritePartons                = False,
                                                            WriteHadrons                = False,
                                                            WriteBHadrons               = False,
                                                            WriteGeant                  = False,
                                                            GeantPhotonPtThresh         = -1.0,
                                                            WriteTauHad                 = False,
                                                            PartonPtThresh              = -1.0,
                                                            WriteBSM                    = True,
                                                            WriteBosons                 = True,
                                                            WriteBSMProducts            = True,
                                                            WriteTopAndDecays           = True,
                                                            WriteEverything             = False,
                                                            WriteAllLeptons             = False,
                                                            WriteLeptonsNotFromHadrons  = True,
                                                            WriteStatus3                = False,
                                                            WriteFirstN                 = -1,
                                                            PreserveDescendants         = True)

# the hard ME information is stored in the first records, so keep those
# setting the WriteFirstN to 10
# that should be more than necessary for the hard matrix element only
# it also costs very little in size
# also keep top and decays here, to be sure we are saving it, although the previous one
# should also keep it
EXOT4TAMCThinningTool = DerivationFramework__MenuTruthThinning(name = "EXOT4TAMCThinningTool",
                                                            ThinningService             = EXOT4ThinningHelper.ThinningSvc(),
                                                            WritePartons                = False,
                                                            WriteHadrons                = False,
                                                            WriteBHadrons               = False,
                                                            WriteGeant                  = False,
                                                            GeantPhotonPtThresh         = -1.0,
                                                            WriteTauHad                 = False,
                                                            PartonPtThresh              = -1.0,
                                                            WriteBSM                    = False,
                                                            WriteBosons                 = False,
                                                            WriteBSMProducts            = False,
                                                            WriteTopAndDecays           = True,
                                                            WriteEverything             = False,
                                                            WriteAllLeptons             = False,
                                                            WriteLeptonsNotFromHadrons  = True,
                                                            WriteStatus3                = False,
                                                            WriteFirstN                 = 10,
                                                            PreserveDescendants         = False,
                                                            PreserveAncestors           = True)

# also explicitly set this tool to keep BSM signal particles used in the analyses
# that, at the time of writing were not yet included in the set of particles to be considered when
# WriteBSM is activated
# if WriteBSM has been updated in the tool above to include these particles, adding this anyway does not
# hurt
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
EXOT4MCGenThinningTool = DerivationFramework__GenericTruthThinning(name = "EXOT4MCGenThinningTool",
    ThinningService             = EXOT4ThinningHelper.ThinningSvc(),
    ParticleSelectionString     = "abs(TruthParticles.pdgId) ==25 || abs(TruthParticles.pdgId)==39 || abs(TruthParticles.pdgId)==32 || abs(TruthParticles.pdgId)==5100021", 
    PreserveDescendants         = False)

# all truth thinning tools set up, so add them in the sequence now
from AthenaCommon.GlobalFlags import globalflags
if DerivationFrameworkHasTruth:
    ToolSvc += EXOT4MCThinningTool
    thinningTools.append(EXOT4MCThinningTool)
    ToolSvc += EXOT4TMCThinningTool
    thinningTools.append(EXOT4TMCThinningTool)
    ToolSvc += EXOT4TAMCThinningTool
    thinningTools.append(EXOT4TAMCThinningTool)
    ToolSvc += EXOT4MCGenThinningTool
    thinningTools.append(EXOT4MCGenThinningTool)


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
                                    name                    = "EXOT4BkgElectronClassificationTool",
                                    MCTruthClassifierTool   = BkgElectronMCTruthClassifier)
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
    "AntiKt2PV0TrackJets",
    "AntiKt4PV0TrackJets",
    "AntiKt10TruthJets",
    "AntiKt10LCTopoJets"]
replaceAODReducedJets(reducedJetList,exot4Seq,"EXOT4")

#AntiKt10*PtFrac5SmallR20Jets must be scheduled *AFTER* the other collections are replaced
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addDefaultTrimmedJets
addDefaultTrimmedJets(exot4Seq,"EXOT4")

#Add Soft drop jets
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addCSSKSoftDropJets
addCSSKSoftDropJets(exot4Seq,"EXOT4")

#Add VR track jets
addVRJets(exot4Seq)
addVRJets(exot4Seq,training='201903')
#b-tagging alias for VR jets
BTaggingFlags.CalibrationChannelAliases += ["AntiKtVR30Rmax4Rmin02Track->AntiKtVR30Rmax4Rmin02Track,AntiKt4EMTopo"]
BTaggingFlags.CalibrationChannelAliases += ["AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903->AntiKt4EMTopo"]

#Adding Btagging for PFlowJets
from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit
FlavorTagInit(JetCollections  = ['AntiKt4EMPFlowJets'],Sequencer = exot4Seq)


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
applyJetCalibration_xAODColl("AntiKt4EMPFlow", exot4Seq)
applyJetCalibration_CustomColl("AntiKt10LCTopoTrimmedPtFrac5SmallR20", exot4Seq)


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


#=======================================
# SKIMMING, THINNING, AUGMENTATION
#=======================================

# now apply the jet skimming part, since we have all the elements
exot4Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT4Kernel_jet", SkimmingTools = [EXOT4StringSkimmingTool_jet])

# now that we have the skimming, done, do the thinning and augmentation in the next kernel
# the reason it is done here, is only to guarantee that this is only done for the events that pass
# the jet skimming done above
exot4Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT4Kernel", ThinningTools = thinningTools, AugmentationTools = augTools)

#==============================================================================
# Tau truth building/matching
#==============================================================================
if DerivationFrameworkHasTruth:
    from DerivationFrameworkSUSY.SUSYTruthCommon import addTruthTaus
    addTruthTaus(augTools)


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
if DerivationFrameworkHasTruth:
    EXOT4SlimmingHelper.AllVariables += EXOT4AllVariablesTruth

EXOT4SlimmingHelper.StaticContent = EXOT4Content
if DerivationFrameworkHasTruth:
    EXOT4SlimmingHelper.StaticContent.extend(EXOT4TruthContent)

# note that we add the jets outputs, but not
# directly in AllVariables or in StaticContent
# we want to add only the variables of the jets that we use
# there was a large effort to single them out and put them in ExtraVariables
# this way we reduced significantly the amount of disk space used

#addJetOutputs add the collection as a full collection eventough they are defined under smart collection
#So not using addJetOutputs, add the collections and variables explicitly bellow

listJets = ['AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets','AntiKt10LCTopoCSSKSoftDropBeta100Zcut10Jets','AntiKtVR30Rmax4Rmin02TrackJets','AntiKt4EMPFlowJets','AntiKt4EMTopoJets','AntiKt2PV0TrackJets']#FIX #ATLJETMET-744
if DerivationFrameworkHasTruth:
    listJets.extend(['AntiKt10TruthTrimmedPtFrac5SmallR20Jets'])
# need to add the jets that are made on-the-fly into the dictionary, otherwise there is a crash
# for the ones already in the input xAOD, this is done automatically
for i in listJets:
    EXOT4SlimmingHelper.AppendToDictionary[i] = 'xAOD::JetContainer'
    # this line is fundamental! Jets are unreadable without it!
    EXOT4SlimmingHelper.AppendToDictionary[i+'Aux'] = 'xAOD::JetAuxContainer'
    # hand picked list of variables to save -- save disk space
    EXOT4SlimmingHelper.ExtraVariables +=[i,i+'.pt.eta.phi.m.ECF1.ECF2.ECF3.Tau1_wta.Tau2_wta.Tau3_wta.Split12.Split23.NTrimSubjets.Parent.GhostAntiKt2TrackJet"']

listBtag = ['BTagging_AntiKtVR30Rmax4Rmin02Track_201903',
            'BTagging_AntiKt4EMTopo_201810',
            'BTagging_AntiKt4EMPFlow_201810',
            'BTagging_AntiKt4EMPFlow_201903']

for i in listBtag:
    EXOT4SlimmingHelper.AppendToDictionary[i] = 'xAOD::BTaggingContainer'
    EXOT4SlimmingHelper.AppendToDictionary[i+'Aux'] = 'xAOD::BTaggingAuxContainer'


# central JetEtMiss function to save the MET necessary information
addMETOutputs(EXOT4SlimmingHelper, ["Track", "AntiKt4EMTopo", "AntiKt4EMPFlow"], ["AntiKt4EMTopo", "AntiKt4EMPFlow"])
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
