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
from DerivationFrameworkHI.HIAugmentationTools import *
from DerivationFrameworkHI.HIDerivationFlags import HIDerivationFlags
from HIJetRec.HIJetRecUtils import HasCollection
from HIRecExample.HIRecExampleFlags import jobproperties

#====================================================================
#Read and set conditions
#====================================================================

GetConditionsFromMetaData()

#====================================================================
#Check to see type of data
#====================================================================
from PyUtils import AthFile
af = AthFile.fopen(svcMgr.EventSelector.InputCollections[0])
if 'project_name' in af.fileinfos['metadata']['/TagInfo']: project_tag = af.fileinfos['metadata']['/TagInfo']['project_name']
else: project_tag = ""
print '+++++++++++++++++++++++++++++++ project tag: ',project_tag,' +++++++++++++++++++++++++++++++'

#====================================================================
#Now do the skimming/thinning
#====================================================================

streamName = derivationFlags.WriteDAOD_HION7Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HION7Stream )
DerivationName=streamName.split('_')[-1]
TrackThinningThreshold=900 #in MeV

#Book DF jets only if they are in xAOD or they are made in AODFix
BookDFJetCollection = (jobproperties.HIRecExampleFlags.doHIAODFix or HasCollection("DFAntiKt4HI"))
MainJetCollection = ""
if BookDFJetCollection : MainJetCollection = "DF"

#Trigger selection
expression=''
if not HIDerivationFlags.isSimulation():    
    TriggerDict = GetTriggers(project_tag, HIDerivationFlags.doMinBiasSelection(), DerivationName)
    for i, key in enumerate(TriggerDict):
	    #Event selection based on DF jets for HI
	    expression = expression + '(' + key + ' && count(' + MainJetCollection + 'AntiKt4HIJets.pt >' + str(TriggerDict[key]) + '*GeV) >=1 ) ' + '|| (' + key + ' && count(' + MainJetCollection + 'AntiKt2HIJets.pt >' + str(TriggerDict[key]) + '*GeV) >=1 ) ' 
	    #Event selection based also on non-DF jets for pp
	    if HIDerivationFlags.isPP and BookDFJetCollection: expression = expression + '|| (' + key + ' && count(AntiKt4HIJets.pt >' + str(TriggerDict[key]) + '*GeV) >=1 ) ' + '|| (' + key + ' && count(AntiKt2HIJets.pt >' + str(TriggerDict[key]) + '*GeV) >=1 ) '
	    if not i == len(TriggerDict) - 1:
		    expression = expression + ' || ' 
	    
    print "==========Event filtering expression=========="
    print expression
    print "=============================================="

#Thinning threshods dictionary for jets is applied only in data
JetThinningThreshold = {'AntiKt2HIJets': 20, 'AntiKt4HIJets': 20,'DFAntiKt2HIJets': 20, 'DFAntiKt4HIJets': 20} #in GeV


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
CollectionList=[MainJetCollection+'AntiKt2HIJets',MainJetCollection+'AntiKt4HIJets']
if HIDerivationFlags.isPP() and BookDFJetCollection :CollectionList.extend(['AntiKt2HIJets','AntiKt4HIJets']) 
BtaggedCollectionList=['BTagging_'+ MainJetCollection +'AntiKt4HI']
if HIDerivationFlags.isPP() and ( HasCollection("BTagging_AntiKt4HI") and jobproperties.HIRecExampleFlags.doHIAODFix): BtaggedCollectionList.append('BTagging_AntiKt4HI')

#Jet thinning only for PbPb data
if not HIDerivationFlags.isSimulation() and not HIDerivationFlags.doMinBiasSelection() and not HIDerivationFlags.isPP() : 
    for collection in BtaggedCollectionList :thinningTools.append(addBtaggThinningTool(collection, collection.split("_",1)[1]+"Jets", DerivationName, JetThinningThreshold[collection.split("_",1)[1]+"Jets"]))
    for collection in CollectionList :
        if "Seed" not in collection:
            thinningTools.append(addJetThinningTool(collection,DerivationName,JetThinningThreshold[collection]))    
            thinningTools.append(addJetClusterThinningTool(collection,DerivationName,20))

if HIDerivationFlags.isSimulation() :
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
    truth_thinning_tool=DerivationFramework__GenericTruthThinning(name                    = "%sTruthParticleThinning" % DerivationName,
                                                                  ThinningService         = "%sThinningSvc" % DerivationName,
                                                                  ParticlesKey            = "TruthParticles", 
                                                                  ParticleSelectionString = "(TruthParticles.status==1) && (TruthParticles.pt > 0.7*GeV) && (abs(TruthParticles.eta) < 2.7)")
    ToolSvc+=truth_thinning_tool
    thinningTools.append(truth_thinning_tool)

#########Slimming#########
extra_Bjets=[]
if jobproperties.HIRecExampleFlags.doHIAODFix: extra_Bjets=['BTagging_DFAntiKt4HI']

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
AllVarContent=["AntiKt4HITrackJets","BTagging_"+MainJetCollection+"AntiKt4HI",MainJetCollection+"AntiKt4HIJets" ]
if HIDerivationFlags.isPP() and (HasCollection("BTagging_AntiKt4HI") and jobproperties.HIRecExampleFlags.doHIAODFix) : AllVarContent+=["BTagging_AntiKt4HI"]
AllVarContent+=HIGlobalVars
if HIDerivationFlags.isSimulation() : 
    AllVarContent+=["AntiKt2TruthJets","AntiKt4TruthJets","TruthEvents","TruthParticles"]
	
SlimmingHelper.AllVariables=AllVarContent

ExtraVars=[]
#if not HIDerivationFlags.isPP: ExtraVars+=HIJetTriggerVars
#else ExtraVars+=ppJetTriggerVars

for collection in CollectionList :  
    if "Seed" not in collection:    
        for j in HIJetBranches: 
            ExtraVars.append(collection+'.'+j)
    else:
        for j in HISeedBranches: 
            ExtraVars.append(collection+'.'+j)

ExtraVars.append("InDetTrackParticles.truthMatchProbability.x.y.z.vx.vy.vz")
ExtraVars.append("InDetTrackParticles.numberOfInnermostPixelLayerSplitHits.numberOfNextToInnermostPixelLayerSplitHits.numberOfNextToInnermostPixelLayerSharedHits")
ExtraVars.append("InDetTrackParticles.numberOfPixelSplitHits.numberOfInnermostPixelLayerSharedHits.numberOfContribPixelLayers.hitPattern.radiusOfFirstHit")
ExtraVars.append("InDetTrackParticles.is_selected.is_associated.is_svtrk_final.pt_wrtSV.eta_wrtSV.phi_wrtSV.d0_wrtSV.z0_wrtSV.errP_wrtSV.errd0_wrtSV.errz0_wrtSV.chi2_toSV")
ExtraVars.append("PrimaryVertices.neutralWeights.numberDoF.sumPt2.chiSquared.covariance.trackWeights")
ExtraVars.append("PrimaryVertices.x.y.trackParticleLinks.vertexType.neutralParticleLinks")
ExtraVars.append("ExtrapolatedMuonTrackParticles.vx.vy.vz")
ExtraVars.append("MuonSpectrometerTrackParticles.vx.vy.vz")
ExtraVars.append("CombinedMuonTrackParticles.vx.vy.vz")
ExtraVars.append("Muons.EnergyLoss.energyLossType")
SlimmingHelper.ExtraVariables=ExtraVars
for v in HIClusterVars : SlimmingHelper.ExtraVariables.append(v)

###############Augmentation#############
HIGlobalAugmentationTool = addHIGlobalAugmentationTool(DerivationName,3,500)
augToolList=[HIGlobalAugmentationTool]

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

