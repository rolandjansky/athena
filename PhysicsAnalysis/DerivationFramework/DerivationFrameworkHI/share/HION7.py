#====================================================================
# HION7.py
# author: Aaron Angerami <angerami@cern.ch>
# Application: Fragmentation study, jet-track correlations
# Triggers: High pT jets
# Selection: Offline MB event selection, offline jet threshold, track and jet skimming anf thinning
# Content: ES summary, jets, extended tracking
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
#Check to see type of data
#====================================================================
from PyUtils import AthFile
af = AthFile.fopen(svcMgr.EventSelector.InputCollections[0])
project_tag = af.fileinfos['metadata']['/TagInfo']['project_name']
beam_energy = af.fileinfos['metadata']['/TagInfo']['beam_energy']
print '+++++++++++++++++++++++++++++++ project tag: ',project_tag,' +++++++++++++++++++++++++++++++'

#====================================================================
#Now do the skimming/thinning
#====================================================================

streamName = derivationFlags.WriteDAOD_HION7Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HION7Stream )
DerivationName=streamName.split('_')[-1]
TrackThinningThreshold=900 #in MeV

#Trigger selection
expression=''
if not HIDerivationFlags.isSimulation():    
    TriggerDict = GetTriggers(project_tag, HIDerivationFlags.doMinBiasSelection(), DerivationName)
    for i, key in enumerate(TriggerDict):
	    #Event selection based on DF jets for HI
	    expression = expression + '(' + key + ' && count(DFAntiKt4HIJets.pt >' + str(TriggerDict[key]) + '*GeV) >=1 ) '
	    #Event selection based also on non-DF jets for pp
	    if HIDerivationFlags.isPP: expression = expression + '|| (' + key + ' && count(AntiKt4HIJets.pt >' + str(TriggerDict[key]) + '*GeV) >=1 ) '
	    if not i == len(TriggerDict) - 1:
		    expression = expression + ' || ' 
	    
    print "==========Event filtering expression=========="
    print expression
    print "=============================================="

#Thinning threshods for jets is applied only in data
JetThinningThreshold = {'AntiKt2HIJets': 35, 'AntiKt4HIJets': 35,'DFAntiKt2HIJets': 35, 'DFAntiKt4HIJets': 35} #in GeV


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


if HIDerivationFlags.isSimulation() : skimmingTools=[]
for t in skimmingTools : ToolSvc+=t


#########Thinning#########

thinningTools=[]

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

#Jet collections to be stored
CollectionList=['DFAntiKt2HIJets','DFAntiKt4HIJets']
if HIDerivationFlags.isPP() : CollectionList=['AntiKt2HIJets','AntiKt4HIJets','DFAntiKt2HIJets','DFAntiKt4HIJets'] 

#Jet thinning only for PbPb data
if not HIDerivationFlags.isSimulation() and not HIDerivationFlags.doMinBiasSelection() and not HIDerivationFlags.isPP() : 
    for collection in CollectionList : thinningTools.append(addJetThinningTool(collection,DerivationName,JetThinningThreshold[collection]))

if HIDerivationFlags.isSimulation() :
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
    truth_thinning_tool=DerivationFramework__GenericTruthThinning(name                    = "%sTruthParticleThinning" % DerivationName,
                                                                  ThinningService         = "%sThinningSvc" % DerivationName,
                                                                  ParticlesKey            = "TruthParticles", 
                                                                  ParticleSelectionString = "(TruthParticles.status==1) && (TruthParticles.pt > 0.7*GeV) && (abs(TruthParticles.eta) < 2.7)")
    ToolSvc+=truth_thinning_tool
    thinningTools.append(truth_thinning_tool)

#########Slimming#########
extra_Bjets=['BTagging_DFAntiKt4HI']

from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
SlimmingHelper = SlimmingHelper("%sSlimmingHelper" % DerivationName)
for item in CollectionList :  
    if not SlimmingHelper.AppendToDictionary.has_key(item):
        SlimmingHelper.AppendToDictionary[item]='xAOD::JetContainer'
        SlimmingHelper.AppendToDictionary[item+"Aux"]='xAOD::JetAuxContainer'
for item in extra_Bjets :  
    if not SlimmingHelper.AppendToDictionary.has_key(item):
        SlimmingHelper.AppendToDictionary[item]='xAOD::BTaggingContainer'
        SlimmingHelper.AppendToDictionary[item+"Aux"]='xAOD::BTaggingAuxContainer'

##SmartVariables
SlimmingHelper.SmartCollections = [ "InDetTrackParticles",
                                    "Electrons",
                                    "Muons",
                                    "Photons",
                                    "PrimaryVertices"]
##AllVariables
AllVarContent=["AntiKt4HITrackJets","BTagging_DFAntiKt4HI"]
AllVarContent+=HIGlobalVars
if HIDerivationFlags.isSimulation() : 
    AllVarContent+=["AntiKt2TruthJets","AntiKt4TruthJets","TruthEvents","TruthParticles"]
	
SlimmingHelper.AllVariables=AllVarContent

ExtraVars=[]
#if not HIDerivationFlags.isPP: ExtraVars+=HIJetTriggerVars
#else ExtraVars+=ppJetTriggerVars

for collection in CollectionList :  
    for j in HIJetBranches: 
        ExtraVars.append(collection+'.'+j)

ExtraVars.append("InDetTrackParticles.truthMatchProbability")
SlimmingHelper.ExtraVariables=ExtraVars


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

