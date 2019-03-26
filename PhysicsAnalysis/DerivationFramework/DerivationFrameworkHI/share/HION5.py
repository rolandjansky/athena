#====================================================================
# HION5.py
# author: Aaron Angerami <angerami@cern.ch>, Andrzej Zemla <azemla@cern.ch>
#	  Jakub Kremer <jakub.kremer@cern.ch>, Piotr Janus <piotr.janus.cern.ch>
# Application: W, Z, gamma (including W/Z/gamma-jet)
# Triggers: Single high pT leptons and photons
# Selection: loose single lepton/photon selection
# Content: Electrons, Muons w/ associated tracks, Photons, limited jet info and ES summary
#====================================================================

# Set up common services and job object.
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *

#for CP common tools
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *

#HI derivation tools
from DerivationFrameworkHI.HIJetDerivationTools import *
from DerivationFrameworkHI.HISkimmingTools import *
from DerivationFrameworkHI.HIDerivationFlags import HIDerivationFlags


#====================================================================
#Read and set conditions
#====================================================================

GetConditionsFromMetaData()

#====================================================================
# SKIMMING
#====================================================================
#removing events
#==========================================================
#Trigger selection
#==========================================================

triggers = []
if HIDerivationFlags.isPPb():
    triggers += ['HLT_e15_lhloose_nod0']
    triggers += ['HLT_mu15']
    triggers += ['HLT_g10_loose']
    triggers += ['HLT_g15_loose']
    triggers += ['HLT_g20_loose']
    triggers += ['HLT_g25_loose']
    triggers += ['HLT_g30_loose']
    triggers += ['HLT_g35_loose']
    triggers += ['HLT_2g10_loose_L12EM5']
elif not HIDerivationFlags.isPP(): 
    if not HIDerivationFlags.doMinBiasSelection():
        triggers += ['HLT_e15_loose_ion_L1EM12']
        triggers += ['HLT_mu8']
        triggers += ['HLT_g20_loose_ion']
        triggers += ['HLT_e15_lhloose_ion_L1EM12']
        triggers += ['HLT_e15_loose_ion']
    else :
        triggers += ['HLT_noalg_mb_L1TE50']
        triggers += ['HLT_mb_sptrk_ion_L1ZDC_A_C_VTE50']    

    #remove egamma tools scheduled for ForwardElectrons, which are missing in Pb+Pb reconstruction
    toolsToRemove = []
    for tool in topSequence.EGammaCommonKernel.AugmentationTools:
        if 'ForwardElectron' in tool.getName():
            toolsToRemove.append(tool)

    for tool in toolsToRemove:
        topSequence.EGammaCommonKernel.AugmentationTools.remove(tool)
else:
    triggers += ['HLT_e15_lhloose_L1EM13VH']
    triggers += ['HLT_e15_lhloose_L1EM12']
    triggers += ['HLT_e15_lhloose_nod0_L1EM12']
    triggers += ['HLT_mu14']
    triggers += ['HLT_g30_loose']    
    triggers += ['HLT_g35_loose_L1EM15']    


#==========================================================
#Event Selection
#==========================================================
#DFCommonMuonsSelectorPreselection.TrtCutOff = True
req_electrons = 'count( Electrons.DFCommonElectronsLHLoose && ( Electrons.pt > 15*GeV ))>0'
req_muons     = 'count( Muons.DFCommonMuonsPreselection && (Muons.pt > 15*GeV) && ( abs(Muons.eta) < 2.7))>0'
if HIDerivationFlags.isPPb(): req_photons = 'count( Photons.DFCommonPhotonsIsEMLoose && (Photons.pt > 10*GeV) ) > 0'
else : req_photons = 'count( Photons.DFCommonPhotonsIsEMLoose && (Photons.pt > 40*GeV) ) > 0'

if not HIDerivationFlags.isSimulation(): req_total = '(' + ' || '.join(triggers) + ') && (' + req_electrons + ' || ' + req_muons + ' || ' + req_photons + ')'
else : req_total = '(' + req_electrons + ' || ' + req_muons + ' || ' + req_photons + ')'

print "================AND====BEGIN=================="
print req_total
print "================AND====END===================="

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool


HION5SkimmingLeptonsAndPhotonsTool = DerivationFramework__xAODStringSkimmingTool( name = "HION5SkimmingLeptonsAndPhotonsTool",
                                                                          expression = req_total )

ToolSvc+=HION5SkimmingLeptonsAndPhotonsTool

#====================================================================
# SET UP STREAM
#====================================================================
streamName = derivationFlags.WriteDAOD_HION5Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HION5Stream )
HION5Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HION5Stream.AcceptAlgs(["HION5Kernel"])

#====================================================================
# THINNING
#====================================================================
# Establish the thinning helper (which will set up the services behind the scenes)
# Plug in the required triggers

thinningTools = []
#Jet constituents for SEB
thinningTools.append(addJetClusterThinningTool('DFAntiKt4HIJets','HION5',20))

from DerivationFrameworkCore.ThinningHelper import ThinningHelper
HION5ThinningHelper = ThinningHelper( "HION5ThinningHelper" )
HION5ThinningHelper.AppendToStream( HION5Stream )

from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
HITrackSelector = InDet__InDetTrackSelectionTool("HION5InDetTrackSelectionTool") 
HITrackSelector.CutLevel = "Loose"
HITrackSelector.maxNSiSharedModules = 100
HITrackSelector.minPt = 900
ToolSvc += HITrackSelector

from DerivationFrameworkHI.DerivationFrameworkHIConf import DerivationFramework__HITrackParticleThinningTool
HION5ThinningTracksTool = DerivationFramework__HITrackParticleThinningTool( name = 'HION5ThinningTracksTool',
                                                                            ThinningService = HION5ThinningHelper.ThinningSvc(),
                                                                            InDetTrackParticlesKey = "InDetTrackParticles",
                                                                            PrimaryVertexKey = "PrimaryVertices",
                                                                            PrimaryVertexSelection = "sumPt2",
                                                                            TrackSelectionTool = HITrackSelector
                                                                          )

ToolSvc += HION5ThinningTracksTool
thinningTools.append(HION5ThinningTracksTool)


if HIDerivationFlags.isSimulation() :
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning

    truth_req_boson = "((abs(TruthParticles.pdgId) == 23) || (abs(TruthParticles.pdgId) == 24))"
    truth_req_lepton = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 14) && (TruthParticles.pt > 1*GeV) && (TruthParticles.status == 1) && (TruthParticles.barcode < 200000))"
    truth_req_general = "(TruthParticles.status == 1) && (TruthParticles.pt > 0.7*GeV) && (abs(TruthParticles.eta) < 2.7)"

    HION5TruthLepTool = DerivationFramework__GenericTruthThinning(name                         = "HION5TruthLepTool",
                                                                  ThinningService              = HION5ThinningHelper.ThinningSvc(),
                                                                  ParticleSelectionString      = truth_req_lepton,
                                                                  PreserveDescendants          = False,
                                                                  PreserveGeneratorDescendants = False,
                                                                  PreserveAncestors            = True)

    HION5TruthBosTool = DerivationFramework__GenericTruthThinning(name                         = "HION5TruthBosTool",
                                                                  ThinningService              = HION5ThinningHelper.ThinningSvc(),
                                                                  ParticleSelectionString      = truth_req_boson,
                                                                  PreserveDescendants          = False,
                                                                  PreserveGeneratorDescendants = True,
                                                                  PreserveAncestors            = False)

    HION5TruthGenTool = DerivationFramework__GenericTruthThinning(name                         = "HION5TruthGenTool",
                                                                  ThinningService              = HION5ThinningHelper.ThinningSvc(),
                                                                  ParticleSelectionString      = truth_req_general)

    ToolSvc += HION5TruthLepTool
    ToolSvc += HION5TruthBosTool
    ToolSvc += HION5TruthGenTool
    thinningTools.append(HION5TruthLepTool)
    thinningTools.append(HION5TruthBosTool)
    thinningTools.append(HION5TruthGenTool)


JetCollectionList = ['AntiKt2HIJets', 'AntiKt4HIJets', 'DFAntiKt2HIJets', 'DFAntiKt4HIJets', 'AntiKt2HIJets_Seed1']


from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("HION5Kernel",
                                                                           AugmentationTools = [],
                                                                           SkimmingTools = [HION5SkimmingLeptonsAndPhotonsTool],
                                                                           ThinningTools = thinningTools
                                                                      )
HION5Stream.AcceptAlgs(["HION5Kernel"])

#trackMET calculations
met_ptCutList = [1000,2000,3000,4000,5000]
from DerivationFrameworkHI.TrackMET_config import *
configTrackMET(DerivationFrameworkJob,met_ptCutList)

#====================================================================
# CONTENT LIST
#====================================================================
# Add the required contents (offline)
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
HION5SlimmingHelper = SlimmingHelper("HION5SlimmingHelper")

for item in JetCollectionList :  
    if not HION5SlimmingHelper.AppendToDictionary.has_key(item):
        HION5SlimmingHelper.AppendToDictionary[item]='xAOD::JetContainer'
        HION5SlimmingHelper.AppendToDictionary[item+"Aux"]='xAOD::JetAuxContainer'

from DerivationFrameworkHI.HION5ExtraContent import *

HION5SlimmingHelper.SmartCollections = ["InDetTrackParticles",
                                        "PrimaryVertices",
                                        "Electrons",
                                        "Muons",
                                        "Photons"
                                       ]
if HIDerivationFlags.isPPb(): HION5SlimmingHelper.SmartCollections += ["MET_Reference_AntiKt4EMTopo", "AntiKt4EMTopoJets"]

ExtraContentAll=[]
for collection in JetCollectionList :  
    if "Seed" not in collection:    
        for j in HIJetBranches: 
            ExtraContentAll.append(collection+'.'+j)
    else:
        for j in HISeedBranches: 
            ExtraContentAll.append(collection+'.'+j)        
        
HION5SlimmingHelper.ExtraVariables = ExtraContentAll

for v in HIClusterVars : HION5SlimmingHelper.ExtraVariables.append(v)

from DerivationFrameworkEGamma.ElectronsCPDetailedContent import *
HION5SlimmingHelper.ExtraVariables += ElectronsCPDetailedContent
HION5SlimmingHelper.ExtraVariables += GSFTracksCPDetailedContent

from DerivationFrameworkEGamma.PhotonsCPDetailedContent import *
HION5SlimmingHelper.ExtraVariables += PhotonsCPDetailedContent


addMETOutputs(HION5SlimmingHelper,met_ptCutList)
#HION5SlimmingHelper.AllVariables = ["AntiKt2HIJets","AntiKt4HIJets","AntiKt4HITrackJets","HIClusters"]
HION5SlimmingHelper.AllVariables += ["AntiKt4HITrackJets"]
HION5SlimmingHelper.AllVariables += HIGlobalVars
if HIDerivationFlags.isPPb(): HION5SlimmingHelper.AllVariables += ["HIEventShape", "ForwardElectrons", "ForwardElectronClusters"]
if HIDerivationFlags.isSimulation() : HION5SlimmingHelper.AllVariables += ["AntiKt2TruthJets","AntiKt4TruthJets","egammaTruthParticles","TruthEvents","TruthVertices","TruthParticles"]


HION5SlimmingHelper.IncludeEGammaTriggerContent = True
HION5SlimmingHelper.IncludeMuonTriggerContent = True

HION5SlimmingHelper.AppendContentToStream(HION5Stream)

#This enables L1EnergySums
OtherContent = [
    'xAOD::EnergySumRoI#*',
    'xAOD::EnergySumRoIAuxInfo#*'
 ]

for item in OtherContent:
    HION5Stream.AddItem(item)
