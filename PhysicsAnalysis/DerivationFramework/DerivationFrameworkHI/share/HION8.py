#====================================================================
# HION8.py
# author: Aaron Angerami <angerami@cern.ch>
# Application: Inlusive jet measuremetns
# Triggers: High pT jets
# Selection: Offline MB event selection, offline jet threshold, track and jet skimming anf thinning
# Content: ES summary, jets, some tracking
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

streamName = derivationFlags.WriteDAOD_HION8Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HION8Stream )
DerivationName=streamName.split('_')[-1]
TrackThinningThreshold=4000 #in MeV
#Thinning threshods for jets is applied only in data and the cut is set to be the 99% point of the lowest trigger threshold
JetThinningThreshold = {'DFAntiKt4HIJets': HITriggerDict['HLT_j50_ion_L1TE20'] , 'DFAntiKt2HIJets': HITriggerDict['HLT_j50_ion_L1TE20'] , 'AntiKt4HIJets': HITriggerDict['HLT_j50_ion_L1TE20'] , 'AntiKt2HIJets': HITriggerDict['HLT_j50_ion_L1TE20']} #in GeV
expression='(HLT_j50_ion_L1TE20 && count(DFAntiKt4HIJets.pt > %d*GeV) >=1) || (HLT_j60_ion_L1TE50 && count(DFAntiKt4HIJets.pt > %d*GeV) >=1) || (HLT_j75_ion_L1TE50 && count(DFAntiKt4HIJets.pt > %d*GeV) >=1)' % (HITriggerDict['HLT_j50_ion_L1TE20'],HITriggerDict['HLT_j60_ion_L1TE50'],HITriggerDict['HLT_j75_ion_L1TE50']) 
#TODO to be changed to DF when their performance is understood
if HIDerivationFlags.isPP() : expression='(HLT_j30_L1TE5 && count(AntiKt4HIJets.pt > %d*GeV) >=1) || (HLT_j40_L1TE10 && count(AntiKt4HIJets.pt > %d*GeV) >=1) || (HLT_j50_L1J12 && count(AntiKt4HIJets.pt > %d*GeV) >=1) || (HLT_j60_L1J15 && count(AntiKt4HIJets.pt > %d*GeV) >=1) || (HLT_j75_L1J20 && count(AntiKt4HIJets.pt > %d*GeV) >=1) || (HLT_j85 && count(AntiKt4HIJets.pt > %d*GeV) >=1) ' % (ppTriggerDict['HLT_j30_L1TE5'],ppTriggerDict['HLT_j40_L1TE10'],ppTriggerDict['HLT_j50_L1J12'],ppTriggerDict['HLT_j60_L1J15'],ppTriggerDict['HLT_j75_L1J20'],ppTriggerDict['HLT_j85'])
if HIDerivationFlags.doMinBiasSelection() : expression = 'HLT_noalg_mb_L1TE50 || HLT_mb_sptrk_ion_L1ZDC_A_C_VTE50'

#########Skimming#########
skimmingTools=[]

#String selection
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
skimmingTools.append(DerivationFramework__xAODStringSkimmingTool(name = "%sStringSkimmingTool" % DerivationName,expression=expression))

#HI Event selection
#from DerivationFrameworkHI.DerivationFrameworkHIConf import DerivationFramework__HIMBSkimmingTool
#EventSkimmingTool = DerivationFramework__HIMBSkimmingTool(name = "%sEventSkimmingTool" % DerivationName)
#EventSkimmingTool.HIEventSelectorTool=getHIEventSelector()
#skimmingTools.append(EventSkimmingTool)

#No skimming for MC
if HIDerivationFlags.isSimulation() : skimmingTools=[]
for t in skimmingTools : ToolSvc+=t


#########Thinning#########

thinningTools=[]
thinningTools.append(addJetClusterThinningTool('DFAntiKt4HIJets',DerivationName,30))

#track thinning
#Original: 
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

CollectionList=[]
CollectionList=['DFAntiKt2HIJets','DFAntiKt4HIJets','AntiKt2HIJets_Seed1']
if HIDerivationFlags.isPP() : CollectionList=['AntiKt2HIJets','AntiKt4HIJets','DFAntiKt2HIJets','DFAntiKt4HIJets','AntiKt2HIJets_Seed1']

#Jet thinning only on PbPb HP streams
if not HIDerivationFlags.isSimulation() and not HIDerivationFlags.doMinBiasSelection() and not HIDerivationFlags.isPP() : 
    for collection in CollectionList : 
        if "Seed" not in collection:
            thinningTools.append(addJetThinningTool(collection,DerivationName,JetThinningThreshold[collection]))

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
for item in CollectionList :  
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
AllVarContent=["AntiKt4HITrackJets"]
AllVarContent+=HIGlobalVars
if HIDerivationFlags.isSimulation() : 
    AllVarContent+=["AntiKt2TruthJets","AntiKt4TruthJets","TruthEvents","TruthParticles"]
	
SlimmingHelper.AllVariables=AllVarContent

##ExtraVaraibles
###ExtraVars=HIJetTriggerVars+HIClusterVars
ExtraVars=[]
ExtraVars+=HIJetTriggerVars

for collection in CollectionList :  
    if "Seed" not in collection:    
        for j in HIJetBranches: 
            ExtraVars.append(collection+'.'+j)
    else:
        for j in HISeedBranches: 
            ExtraVars.append(collection+'.'+j)
                
ExtraVars+=HIJetTriggerVars
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
