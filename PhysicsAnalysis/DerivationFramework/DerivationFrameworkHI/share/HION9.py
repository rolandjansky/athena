#====================================================================
# HION9.py
# author: Aaron Angerami <angerami@cern.ch>
# Application: jet sub-structure
# Triggers: High pT jets
# Content: large R jets including their constituents
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkHI.HIJetDerivationTools import *
from DerivationFrameworkHI.HISkimmingTools import *
from DerivationFrameworkHI.HIDerivationFlags import HIDerivationFlags

#====================================================================
#Read and set conditions
#====================================================================

GetConditionsFromMetaData()

#====================================================================
#Now do the skimming/thinning
#====================================================================

streamName = derivationFlags.WriteDAOD_HION9Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HION9Stream )
DerivationName=streamName.split('_')[-1]
TrackThinningThreshold=4000 #in MeV
#Thinning threshods for jets is applied only in data
largeRThreshold=150
expression='HLT_j75_ion_L1TE50 && count(DFAntiKt10HIJets.pt > %d*GeV) >=1' % largeRThreshold
if HIDerivationFlags.isPP() : expression='HLT_j85 && count(DFAntiKt10HIJets.pt > %d*GeV) >=1' % largeRThreshold
if HIDerivationFlags.doMinBiasSelection() : expression = '(HLT_noalg_mb_L1TE50 || HLT_mb_sptrk_ion_L1ZDC_A_C_VTE50) && count(DFAntiKt10HIJets.pt > %d*GeV) >=1' % largeRThreshold

#########Skimming#########
skimmingTools=[]

#String selection
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
skimmingTools.append(DerivationFramework__xAODStringSkimmingTool(name = "%sStringSkimmingTool" % DerivationName,expression=expression))


if HIDerivationFlags.isSimulation() : skimmingTools=[]
for t in skimmingTools : ToolSvc+=t


#########Thinning#########

thinningTools=[]
thinningTools.append(addJetClusterThinningTool('DFAntiKt10HIJets',DerivationName,largeRThreshold))


#track thinning
#Version without track selector: 
#thinningTools.append(addTrackThinningToolTight(DerivationName,TrackThinningThreshold))
#Track selector based
from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
HITrackSelector=InDet__InDetTrackSelectionTool("InDetTrackSelectionTool_%s" % DerivationName) 
HITrackSelector.CutLevel = "Loose"
HITrackSelector.maxNSiSharedModules  = 100
HITrackSelector.minPt=TrackThinningThreshold
ToolSvc+=HITrackSelector

from DerivationFrameworkHI.DerivationFrameworkHIConf import DerivationFramework__HITrackParticleThinningTool
TPThinningTool=DerivationFramework__HITrackParticleThinningTool(name='%sTPThinningTool' % DerivationName,
                                                                ThinningService="%sThinningSvc" % DerivationName,
                                                                InDetTrackParticlesKey="InDetTrackParticles",
                                                                PrimaryVertexKey="PrimaryVertices",
                                                                PrimaryVertexSelection="sumPt2",
                                                                TrackSelectionTool=HITrackSelector)


ToolSvc+=TPThinningTool
thinningTools=[TPThinningTool]

#Jet thinning
# if not HIDerivationFlags.isSimulation() : 
#     for collection in ["DFAntiKt2HIJets","DFAntiKt4HIJets"] : thinningTools.append(addJetThinningTool(collection,DerivationName,100.)

if HIDerivationFlags.isSimulation() :
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
    truth_thinning_tool=DerivationFramework__GenericTruthThinning(name                    = "%sTruthParticleThinning" % DerivationName,
                                                                  ThinningService         = "%sThinningSvc" % DerivationName,
                                                                  ParticlesKey            = "TruthParticles", 
                                                                  ParticleSelectionString = "(TruthParticles.status==1) && (TruthParticles.pt > 0.7*GeV) && (abs(TruthParticles.eta) < 2.7)")
    ToolSvc+=truth_thinning_tool
    thinningTools.append(truth_thinning_tool)

#########Slimming#########


from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
SlimmingHelper = SlimmingHelper("%sSlimmingHelper" % DerivationName)

AllVarContent=["AntiKt4HITrackJets","DFAntiKt10HIJets"]
AllVarContent+=HIGlobalVars

extra_jets=["DFAntiKt2HIJets","DFAntiKt4HIJets","DFAntiKt10HIJets"]
if HIDerivationFlags.isSimulation() : extra_jets+=["AntiKt10TruthJets"]
for item in extra_jets:
    if not SlimmingHelper.AppendToDictionary.has_key(item):
        SlimmingHelper.AppendToDictionary[item]='xAOD::JetContainer'
        SlimmingHelper.AppendToDictionary[item+"Aux"]='xAOD::JetAuxContainer'


##SmartVariables
SlimmingHelper.SmartCollections = [ "InDetTrackParticles",
                                    "Electrons",
                                    "Muons",
                                    "Photons",
                                    "PrimaryVertices"]
##AllVariables
if HIDerivationFlags.isSimulation() : 
    AllVarContent+=["AntiKt2TruthJets","AntiKt4TruthJets","TruthEvents","TruthParticles","AntiKt10TruthJets"]
	
SlimmingHelper.AllVariables=AllVarContent

##ExtraVaraibles
###ExtraVars=HIJetTriggerVars+HIClusterVars
ExtraVars=[]
ExtraVars+=HIJetTriggerVars
for collection in ["AntiKt2HIJets","AntiKt4HIJets"] :  
    for j in HIJetBranches: 
        #ExtraVars.append(collection+'.'+j) #!!!removed non-DF
        ExtraVars.append('DF' + collection+'.'+j)


ExtraVars.append("InDetTrackParticles.truthMatchProbability")
SlimmingHelper.ExtraVariables=ExtraVars
for v in HIClusterVars : SlimmingHelper.ExtraVariables.append(v)


augToolList=[]
#########Kernel#########
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("%sKernel" % DerivationName,
                                                                       AugmentationTools = augToolList,
                                                                       SkimmingTools = skimmingTools,
                                                                       ThinningTools = thinningTools)

#########Stream#########
TheStream = MSMgr.NewPoolRootStream( streamName, fileName )
TheStream.AcceptAlgs(["%sKernel" % DerivationName])

#########Connect the pieces#########
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="%sThinningSvc" % DerivationName, outStreams=[evtStream] )
SlimmingHelper.AppendContentToStream(TheStream) 
TheStream.AddItem("xAOD::EventInfoContainer#*")
TheStream.AddItem("xAOD::EventInfoAuxContainer#*")
TheStream.AddItem("xAOD::EventInfo#*")
TheStream.AddItem("xAOD::EventAuxInfo#*")
#EventShapeWeighted (5kb/event) removed
for es in ["EventShapeWeighted_iter0_Modulate","EventShapeWeighted_iter1_Modulate"] :
    TheStream.AddItem("xAOD::HIEventShapeContainer#" + es);
    TheStream.AddItem("xAOD::HIEventShapeAuxContainer#" + es + "Aux.");


#    TheStream.AddItem("xAOD::JetContainer#DF"+collection)
#    TheStream.AddItem("xAOD::JetAuxContainer#DF"+collection+"Aux.")
