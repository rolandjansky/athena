
#********************************************************************
# HION12.py - derivations intended for analyses of
#              production of jets in UPC, either photonuclear or gamma+gamma
#             
# reductionConf flag HION12 in Reco_tf.py
# author: begilber@cern.ch
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit
from DerivationFrameworkHI.HIJetDerivationTools import *
from DerivationFrameworkHI.HISkimmingTools import *
from DerivationFrameworkHI.HIDerivationFlags import HIDerivationFlags
from DerivationFrameworkHI.HIAugmentationTools import *


streamName = derivationFlags.WriteDAOD_HION12Stream.StreamName # Get the name of the stream from derivation flags
fileName   = buildFileName( derivationFlags.WriteDAOD_HION12Stream ) # Get the file name from the derivation flags
DerivationName=streamName.split('_')[-1] # Get the derivation name from the first part of the stream name

#====================================================================
# AUTO-CONFIGURE CONDITIONS -> set HIDerivationFlags
#====================================================================

GetConditionsFromMetaData()

#====================================================================
# SKIMMING TOOLS
#====================================================================

skimmingTools=[] # Initialize a list of skimming tools
augToolList=[] # Initialize a list of augmentation tools

# SELECTION FOR BACKGROUND ESTIMATES

#====================================================================
# prescaled _etcut triggers
# prescaled _loose triggers
# prescaled _lhloose triggers
#====================================================================

#requirement_object = 'Electrons.pt > 2.0*GeV'
#objectSelection = 'count('+requirement_object+') >= 1'


objectSelection = '(count(PrimaryVertices.z < 1000) < 10)' # An additional cut requiring less than 10 primary vertices to throw out splash events
nJetCuts = ['(count(AntiKt4EMTopoJets.pt > 7.0*GeV) > 0)',  
			'(count(AntiKt4LCTopoJets.pt > 7.0*GeV) > 0)', 
			'(count(AntiKt4EMPFlowJets.pt > 7.0*GeV) > 0)',
			'(count(AntiKt4HIJets.pt > 7.0*GeV) > 0)', 
			'(count(AntiKt4HITrackJets.pt > 7.0*GeV) > 0)', 
			'(count(AntiKt10HIJets.pt > 7.0*GeV) > 0)', 
			'(count(AntiKt10EMTopoJets.pt > 7.0*GeV) > 0)', 
			'(count(AntiKt10LCTopoJets.pt > 7.0*GeV) > 0)', 
			'(count(AntiKt10EMPFlowJets.pt > 7.0*GeV) > 0)']
FCal_Cut = '(count(CaloSums.et < 100*GeV) >= 1)' # An FCal ET cut used to initially filter min-bias and get UPC events

triggers = []
MB_triggers = []

#2018 triggers

# Photo-nuclear dijet physics triggers (j40 not included since j30 is un-prescaled for the entire run)

triggers += ['HLT_j10_L1ZDC_XOR_TE5_VTE200']  # j10 trigger for first half of 2018 run
triggers += ['HLT_j10_0eta490_L1ZDC_XOR_TE5_VTE200']  # j10 trigger for second half of 2018 run
triggers += ['HLT_j15_L1ZDC_XOR_TE5_VTE200']  # j15 trigger for first half of 2018 run
triggers += ['HLT_j15_0eta490_L1ZDC_XOR_TE5_VTE200']  # j15 trigger for second half of 2018 run
triggers += ['HLT_j20_L1ZDC_XOR_TE5_VTE200']  # j20 trigger for first half of 2018 run
triggers += ['HLT_j20_0eta490_L1ZDC_XOR_TE5_VTE200']  # j20 trigger for second half of 2018 run
triggers += ['HLT_j30_L1ZDC_XOR_TE20_VTE200']  # j30 trigger for first half of 2018 run
triggers += ['HLT_j30_0eta490_L1ZDC_XOR_TE20_VTE200']  # j30 trigger for second half of 2018 run
triggers += ['HLT_j10_rcu4_0eta490_L1ZDC_XOR_TE5_VTE200']  # j10 rcu4 trigger for second half of 2018 run (Lower un-calibrated min-pT cut)

# Photoproduction dijet R = 0.4 physics triggers (j40 not included because j30 is un-prescaled for the entire run)
# Actually, all of these triggers (except the rcu4) are un-prescaled. Yay!

triggers += ['HLT_j10_L1VZDC_A_C_TE5_VTE200']  # j10 trigger for the first half of the 2018 run
triggers += ['HLT_j15_L1VZDC_A_C_TE5_VTE200']  # j15 trigger for the first half of the 2018 run
triggers += ['HLT_j20_L1VZDC_A_C_TE5_VTE200']  # j20 trigger for the first half of the 2018 run
triggers += ['HLT_j30_L1VZDC_A_C_TE20_VTE200']  # j30 trigger for the first half of the 2018 run
triggers += ['HLT_j10_0eta490_L1VZDC_A_C_TE5_VTE200']  # j10 trigger for the second half of the 2018 run
triggers += ['HLT_j15_0eta490_L1VZDC_A_C_TE5_VTE200']  # j15 trigger for the second half of the 2018 run
triggers += ['HLT_j20_0eta490_L1VZDC_A_C_TE5_VTE200']  # j20 trigger for the second half of the 2018 run
triggers += ['HLT_j30_0eta490_L1VZDC_A_C_TE20_VTE200']  # j30 trigger for the second half of the 2018 run
triggers += ['HLT_j10_rcu4_0eta490_L1VZDC_A_C_TE5_VTE200']  # j10 rcu4 trigger for second half of 2018 run (Lower un-calibrated min-pT cut)

# Photoproduction dijet R = 1.0 physics triggers

triggers += ['HLT_j10_a10_lcw_subjes_L1VZDC_A_C_TE5_VTE200'] # j10 R=1.0 trigger for second half of the 2018 run. Same pre-scale as j15.
triggers += ['HLT_j15_a10_lcw_subjes_L1VZDC_A_C_TE5_VTE200'] # j15 R=1.0 trigger for second half of the 2018 run. Same pre-scale as j10.
triggers += ['HLT_j20_a10_lcw_subjes_L1VZDC_A_C_TE5_VTE200'] # j20 R=1.0 trigger for second half of the 2018 run. Same pre-scale as j30.
triggers += ['HLT_j30_a10_lcw_subjes_L1VZDC_A_C_TE5_VTE200'] # j30 R=1.0 trigger for second half of the 2018 run. Same pre-scale as j20.

# Photo-nuclear min-bias triggers

MB_triggers += ['HLT_mb_sptrk_L1ZDC_XOR_TE5_VTE200']  # Main min-bias stream for photo-nuclear dijets
MB_triggers += ['HLT_mb_sptrk_L1ZDC_XOR_VTE200']  # Min-bias stream to check the impact of the TE5 cut. Very pre-scaled.
MB_triggers += ['HLT_noalg_L1ZDC_XOR_TE5_VTE200']  # Stream to deal with events where no tracks were re-constructed due to the rapidity gap
MB_triggers += ['HLT_noalg_L1ZDC_XOR']  # Stream to deal with overall effects of TE5 and VTE200 cuts. Extremely pre-scaled.
MB_triggers += ['HLT_noalg_L1TE5_VTE200'] # Stream to examine the impact of the ZDC cuts. Prescale is 276.6.

# Here are all of he 2015 photo-nuclear triggers which do not have an identical version in 2018.

triggers += ['HLT_j10_320eta490_ion_L1TE5_VTE200'] # Forward trigger on HI jets in 2015 without ZDC requirement
triggers += ['HLT_j10_ion_L1TE5_VTE200']  # Mid-rapidity j10 trigger on HI jets in 2015 without ZDC requirement
triggers += ['HLT_j15_ion_L1TE5_VTE200']  # Mid-rapidity j15 trigger on HI jets in 2015 without ZDC requirement
triggers += ['HLT_j20_ion_L1TE5_VTE200']  # Mid-rapidity j20 trigger on HI jets in 2015 without ZDC requirement
triggers += ['HLT_j20_ion_L1ZDC_XOR_TE5_VTE200']  # Mid-rapidity j20 trigger on HI jets in 2015 with ZDC requirement
triggers += ['HLT_j10_ion_L1ZDC_XOR_TE5_VTE200']  # Mid-rapidity j10 trigger on HI jets in 2015 with ZDC requirement
triggers += ['HLT_j15_ion_L1ZDC_XOR_TE5_VTE200']  # Mid-rapidity j15 trigger on HI jets in 2015 with ZDC requirement
triggers += ['HLT_j10_ion_mb_mbts_vetombts1side2in_L1ZDC_XOR_TE5_VTE200']  # j10 trigger on HI jets in 2015 with an asymmetric MBTS and ZDC XOR requirement

MB_triggers += ['HLT_noalg_mb_L1TE50'] # 2015 MB trigger with high pre-scale on events with low total energy
MB_triggers += ['HLT_mb_sptrk_ion_L1ZDC_A_C_VTE50'] # 2015 MB trigger with full ZDC activation for inefficiency

# Photoproduction min-bias triggers
# Need to apply a ZDC veto in order to assess actual efficiencies for comparison.

MB_triggers += ['HLT_mb_sp_L1VTE50']  # Min-bias stream for dijet photoproduction up to 50 GeV. No minimum cut so high pre-scale.
MB_triggers += ['HLT_noalg_pc_L1TE50_VTE600.0ETA490']  # Part of the main heavy ion min-bias stream from 50 to 600 GeV. In the PC stream so requires special attention.

expression = '( (' + ' || '.join(triggers+MB_triggers) + ') && '+objectSelection+ ' && ' + '(' + ' || '.join(nJetCuts) + ')' + ')'
# if HIDerivationFlags.doMinBiasSelection() : expression = FCal_Cut
print expression

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
HION12SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "HION12SkimmingTool",
																   expression = expression)
skimmingTools = [HION12SkimmingTool]
print "HION12 skimming tool:", HION12SkimmingTool

if HIDerivationFlags.isSimulation(): # When using MC, we don't used a trigger to select events, so we won't do this trigger-based skim.
	skimmingTools = []

ToolSvc += HION12SkimmingTool # Add the skimming tool to the list of tools used. Allows a chance for it to be deleted if this is MC and we are not skimming.

#================
# THINNING
#================

from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool 
HITrackSelector=InDet__InDetTrackSelectionTool("InDetTrackSelectionTool_UPC") # Make an in-detector track selection tool for UPC, define track requirements
HITrackSelector.CutLevel = "HILoose"
HITrackSelector.maxD0=1e9
HITrackSelector.maxZ0=1e9
HITrackSelector.maxZ0SinTheta=1e9
HITrackSelector.minPt=200.
ToolSvc+=HITrackSelector # Add to ToolSvc=

# In particular, this tool wil thin out track particles which we don't need.
from DerivationFrameworkHI.DerivationFrameworkHIConf import DerivationFramework__HITrackParticleThinningTool
HION12ThinningTool=DerivationFramework__HITrackParticleThinningTool(name                    = "HION12ThinningTool",
																   ThinningService         = "HION12ThinningSvc",
																   InDetTrackParticlesKey  = "InDetTrackParticles",
																   PrimaryVertexKey        = "PrimaryVertices",
																   PrimaryVertexSelection  = "sumPt2",
																   TrackSelectionTool      = HITrackSelector)

ToolSvc+=HION12ThinningTool
thinningTools=[HION12ThinningTool]

#================
# SLIMMING
#================

from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
SlimmingHelper = SlimmingHelper("HION12SlimmingHelper")

SlimmingHelper.SmartCollections = [ "Electrons",              # Smart collection for electrons
									"Muons",                  # Smart collection for muons
									"Photons",                # Smart collection for photons
									"InDetTrackParticles"]    # Smart collection for tracks
			  
AllVarContent=["AntiKt4EMPFlowJets",                          # Include R = 0.4 anti-kt EM Particle Flow jets
			   "AntiKt4EMTopoJets",                           # Include R = 0.4 anti-kt EM topo-jets
			   "AntiKt4LCTopoJets",                           # Include R = 0.4 anti-kt local calibration topo-jets
			   "CaloCalTopoClusters",                         # Include topocluster information
			   "PrimaryVertices",                             # Include a list of all primary vertices
			   "NCB_MuonSegments",                            # Include the non-collision background muons to handle punch-throughs.
			   "JetETMissChargedParticleFlowObjects",         # Include the charged particle flow objects from the Jet/ET Miss group
			   "JetETMissNeutralParticleFlowObjects",         # Include the neutral particle flow objects from the Jet/ET Miss group
			   "TauChargedParticleFlowObjects",               # Include the charged particle flow objects used for Tau reconstruction
			   "TauNeutralParticleFlowObjects",               # Include the neutral particle flow objects used for Tau reconstruction
			   "TauShotParticleFlowObjects",                  # Include the "shot" particle flow objects used for Tau reconstruction
			   "Kt4EMPFlowEventShape",                        # The event shape specifically for R=0.4 EM PFlow jets
			   "Kt4EMTopoOriginEventShape",                   # The event shape specifically for R=0.4 EM Topo jets
			   "Kt4LCTopoOriginEventShape",                   # The event shape specifically for R=0.4 LC Topo jets
			   "TopoClusterIsoCentralEventShape",             # Part of the event shape for topo-jets
			   "TopoClusterIsoVeryForwardEventShape",         # Part of the event shape for topo-jets
			   "TopoClusterIsoForwardEventShape",             # Part of the event shape for topo-jets
			   "NeutralParticleFlowIsoCentralEventShape",     # Part of the event shape for PFlow jets
			   "ParticleFlowIsoCentralEventShape",            # Part of the event shape for PFlow jets
			   "NeutralParticleFlowIsoForwardEventShape",     # Part of the event shape for PFlow jets
			   "ParticleFlowIsoForwardEventShape",            # Part of the event shape for PFlow jets
			   "HLT_xAOD__JetContainer_a4tcemsubjesISFS",     # Include the HLT R = 0.4 EM Topo trigger jets (2018 HI Run)
			   "HLT_xAOD__JetContainer_a4ionemsubjesISFS",    # Include the HLT R = 0.4 heavy ion trigger jets (2015 HI Run)
			   "HLT_xAOD__JetContainer_a10tclcwsubjesFS"]     # Include the HLT R = 1.0 LCW Topo trigger jets (2018 HI Run)

# Here, we are adding information about the MC truth-level to our derivation. Most of this works for all UPC derivations, but parts to change are marked.
# HepMC Status codes which are used are taken from page 13 of: http://hepmc.web.cern.ch/hepmc/releases/HepMC2_user_manual.pdf
# For generator-dependent status codes, 
if HIDerivationFlags.isSimulation():
	TruthPhotonContainer='InitialTruthPhotons' # For all UPC analyses, we add a dictionary entry where we specifically store the initial-state truth photons for easy access
	# Here, we are making the tool which actually isolates those photons. Note the selection condition: Particles with PDG ID 22 (photons) and status -13 (Beam-within-a-beam)
	from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthCollectionMaker
	UPCTruthPhotonTool = DerivationFramework__TruthCollectionMaker(name                    = "HION12TruthPhotonTool",
																   NewCollectionName       = TruthPhotonContainer,
																   ParticleSelectionString = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.status == 13))")

	ToolSvc+=UPCTruthPhotonTool # Add the tool which makes that collection to our execution
	augToolList.append(UPCTruthPhotonTool) # Count this tool as an augmentation tool
	SlimmingHelper.AppendToDictionary[TruthPhotonContainer]='xAOD::TruthParticleContainer' # List the output of this tool in the dictionary as a container of truth particles
	SlimmingHelper.AppendToDictionary[TruthPhotonContainer+"Aux"]='xAOD::TruthParticleAuxContainer' # Also includes its aux information

	# Next, let's add a tool which will grab just the other truth particles of interest. We include all photons, all final-state particles (status == 1), and all intermediate particles which are part of the beam or participate in the hardest sub-process (9 < status < 30)
	# The string also caps the barcode (effectively particle index) at 200000. This portion stops the derivation from including events with way more final state particles than is reasonable. 200k is a fine cap for UPC.
	from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
	truth_thin = DerivationFramework__GenericTruthThinning(name                    = "HION12TruthThinningTool",
														   ThinningService         = "HION12ThinningSvc",
														   ParticleSelectionString = "((abs(TruthParticles.pdgId) == 22) || (TruthParticles.status == 1) || ((TruthParticles.status < 30) && (TruthParticles.status > 9)) ) && (TruthParticles.barcode < 200000)")
	ToolSvc+=truth_thin # Add this thinning tool to our execution
	thinningTools.append(truth_thin) # Include this tool in our list of thinning tools
	# The next line can be changed depending on your needs. In particular, it currently includes jets, which is probably unnecessary.
	AllVarContent+=["AntiKt4TruthJets", "AntiKt10TruthJets", "TruthEvents", "TruthParticles", TruthPhotonContainer] # The list of additional variable content when we are looking at MC

AllVarContent += HIGlobalVars # Other useful variables we will need like MBTS information and ZDC output
AllVarContent += ["ZdcSums"] # We also want the option to look at the ZDC sums prior to re-processing, which is not in HIGlobalVars.

#===================================
# ADD MORE OBJECTS TO THE DERIVATION
#===================================

ExtraVars=['Muons.MuonSpectrometerPt']    # Also include the spectrometer pT of muons
# Here, extra jets are heavy ion jets for which we can get away with keeping fewer of the moments. We don't currently have the machinery for other jets.
ExtraJets=["AntiKt4HITrackJets",          # Include R = 0.4 Heavy Ion anti-kt track jets
		   "AntiKt4HIJets",               # Include R = 0.4 Heavy Ion anti-kt tower jets
		   "AntiKt10HIJets"]              # Include R = 1.0 Heavy Ion anti-kt tower jets

for collection in ExtraJets:
	for branch in HIJetBranches: 
		  ExtraVars.append(collection+'.'+branch)

SlimmingHelper.ExtraVariables=ExtraVars   
SlimmingHelper.AllVariables=AllVarContent # Set the slimming variable content

# Definition of addUPCJets(jetalg, radius, inputtype, sequence, outputlist) is from HIJetDerivationTools
privateSequence = CfgMgr.AthSequencer("HION12Sequence")  # Add a sequence which will include dynamically generated jet collections
DerivationFrameworkJob += privateSequence                # Schedule this sequence with the derivation framework job
OutputJetsUPC[DerivationName] = []                       # Establish a list of new jets to be appended

# In order to produce our R = 1.0 jets, we need to add their constituents to the jet production sequence.
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addCHSPFlowObjects   # The particle flow objects to create the particle flow jets
addCHSPFlowObjects()
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets # The jets which seed the R = 1.0 jets
# Only include those ones that you use. The order in the list is not significant
reducedJetList = ["AntiKt2PV0TrackJets", # This collection will be flavour-tagged automatically
				  "AntiKt4PV0TrackJets",
				  "AntiKt10LCTopoJets"]
replaceAODReducedJets(reducedJetList, privateSequence, "HION12")

addUPCJets("AntiKt",1.0,"EMTopo",privateSequence,DerivationName)   # Include R = 1.0 EM topo-jets
addUPCJets("AntiKt",1.0,"EMPFlow",privateSequence,DerivationName)  # Include R = 1.0 EM particle flow jets
addUPCJets("AntiKt",1.0,"LCTopo",privateSequence,DerivationName)   # Include R = 1.0 local calibration topo-jets

# Loop through the new jet types in the list to add their information to the slimmer
for item in OutputJetsUPC[DerivationName]:
	if not SlimmingHelper.AppendToDictionary.has_key(item):
		SlimmingHelper.AppendToDictionary[item]='xAOD::JetContainer'
		SlimmingHelper.AppendToDictionary[item+"Aux"]='xAOD::JetAuxContainer'
	SlimmingHelper.AllVariables.append(item)

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================

# Here, we add an augmentation tool which includes relevant information from the heavy ion event shape without including the full object.
#   Argument 1: The name of the derivation
#   Argument 2: The highest order v_n to include. We include only v2.
#   Argument 3: The minimum pT cut included in track-count for OOT pile-up. Doesn't matter for us, so set to 200 for consistency.
HIGlobalAugmentationTool = addHIGlobalAugmentationTool("HION12",2,200)
augToolList+=[HIGlobalAugmentationTool]

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
privateSequence += CfgMgr.DerivationFramework__DerivationKernel("HION12Kernel",
																	   AugmentationTools = augToolList,
																	   SkimmingTools = skimmingTools,
																	   ThinningTools = thinningTools)

#====================================================================
# SET UP STREAM
#====================================================================
# These are declared at the top of the file
HION12Stream = MSMgr.NewPoolRootStream( streamName, fileName )
# Only events that pass the filters listed below are written out.
# Name must match that of the kernel above
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
HION12Stream.AcceptAlgs(["HION12Kernel"])


#Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="HION12ThinningSvc", outStreams=[evtStream] )

# This line must come after we have finished configuring HION12SlimmingHelper
SlimmingHelper.AppendContentToStream(HION12Stream)
HION12Stream.AddItem("xAOD::EventInfo#*")
HION12Stream.AddItem("xAOD::EventAuxInfo#*")
HION12Stream.AddItem("xAOD::EnergySumRoI#LVL1EnergySumRoI")
HION12Stream.AddItem("xAOD::EnergySumRoIAuxInfo#LVL1EnergySumRoIAux.")
