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
from DerivationFrameworkHI.HIAugmentationTools import *
from DerivationFrameworkHI.HIDerivationFlags import HIDerivationFlags
from HIRecExample.HIRecExampleFlags import jobproperties
from HIJetRec.HIJetRecUtils import HasCollection

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
#Now do the skimming/thining
#====================================================================

streamName = derivationFlags.WriteDAOD_HION9Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HION9Stream )
DerivationName=streamName.split('_')[-1]
TrackThinningThreshold=4000 #in MeV


ptThreshDict = {'DFAntiKt10HIJets': 200,'AntiKt10HIJets': 200, 'DFAntiKt8HIJets': 150,'AntiKt8HIJets': 150, 'DFAntiKt6HIJets':100,'AntiKt6HIJets':100, 'DFAntiKt4HIJets':80,'AntiKt4HIJets':80} 

#Book DF jets only if they are in xAOD or they are made in AODFix
BookDFJetCollection = (jobproperties.HIRecExampleFlags.doHIAODFix or HasCollection("DFAntiKt10HI"))
MainJetCollection = "DF" if BookDFJetCollection else ""

expression=''
if not HIDerivationFlags.isSimulation():
    #Trigger selection
    TriggerDict = GetTriggers(project_tag, HIDerivationFlags.doMinBiasSelection(), DerivationName)
    for i, key in enumerate(TriggerDict):
            #Event selection based on DF jets for HI
            expression = expression + '(' + key + ' && count(' + MainJetCollection + 'AntiKt10HIJets.pt >' + str(ptThreshDict.get("AntiKt10HIJets")) + '*GeV) >=1 ) ' + '|| (' + key + ' && count(' + MainJetCollection + 'AntiKt6HIJets.pt >' + str(ptThreshDict.get("AntiKt6HIJets")) + '*GeV) >=1 ) '+ '|| (' + key + ' && count(' + MainJetCollection + 'AntiKt4HIJets.pt >' + str(ptThreshDict.get("AntiKt4HIJets")) + '*GeV) >=1 ) ' 
            if jobproperties.HIRecExampleFlags.doHIAODFix:  expression = expression + '|| (' + key + ' && count(' + MainJetCollection + 'AntiKt8HIJets.pt >' + str(ptThreshDict.get("AntiKt8HIJets")) + '*GeV) >=1 ) ' 
            if HIDerivationFlags.isPP() and HasCollection("AntiKt6HIJets") and BookDFJetCollection: expression + '|| (' + key + ' && count(AntiKt6HIJets.pt >' + str(ptThreshDict.get("AntiKt6HIJets")) + '*GeV) >=1 ) ' 
            if HIDerivationFlags.isPP() and HasCollection("AntiKt8HIJets") and BookDFJetCollection: expression + '|| (' + key + ' && count(AntiKt8HIJets.pt >' + str(ptThreshDict.get("AntiKt8HIJets")) + '*GeV) >=1 ) ' 
            if HIDerivationFlags.isPP() and HasCollection("AntiKt10HIJets") and BookDFJetCollection: expression + '|| (' + key + ' && count(AntiKt10HIJets.pt >' + str(ptThreshDict.get("AntiKt10HIJets")) + '*GeV) >=1 ) ' 
            #Event selection based also on non-DF jets for pp
            if not i == len(TriggerDict) - 1:
                expression = expression + ' || ' 
	    
    print "==========Event filtering expression=========="
    print expression
    print "=============================================="
    
#########Skimming#########
skimmingTools=[]

#String selection
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
skimmingTools.append(DerivationFramework__xAODStringSkimmingTool(name = "%sStringSkimmingTool" % DerivationName,expression=expression))

if HIDerivationFlags.isSimulation() : skimmingTools=[]
for t in skimmingTools : ToolSvc+=t


#########Thinning#########

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

#Jet thinning only for PbPb data
#largeRcollections =  [MainJetCollection +"AntiKt10HIJets"]
largeRcollections = []

for collection_number in [4, 6, 8, 10]:
    collection_name = "AntiKt{}HIJets".format(collection_number)
    if jobproperties.HIRecExampleFlags.doHIAODFix:
        largeRcollections.append(MainJetCollection + collection_name)
        
    
    if HIDerivationFlags.isPP() and MainJetCollection != "":
        if HasCollection(collection_name): largeRcollections.append(collection_name)

for collection in largeRcollections :
   
    if collection in ptThreshDict:
        ptThrsh = ptThreshDict.get(collection)
        #Only clusters associated with jets (high pT)
        thinningTools.append(addJetClusterThinningTool(collection,DerivationName,ptThrsh))


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

AllVarContent=["AntiKt4HITrackJets"]
AllVarContent+=largeRcollections
AllVarContent+=HIGlobalVars

extra_jets=[]
if jobproperties.HIRecExampleFlags.doHIAODFix: extra_jets+=["DFAntiKt4HIJets","DFAntiKt6HIJets","DFAntiKt8HIJets","DFAntiKt10HIJets","AntiKt2HIJets_Seed1"] 
if HIDerivationFlags.isSimulation() : extra_jets+=["AntiKt4TruthJets","AntiKt6TruthJets","AntiKt8TruthJets","AntiKt10TruthJets"] 
for item in extra_jets:
    if not SlimmingHelper.AppendToDictionary.has_key(item):
        SlimmingHelper.AppendToDictionary[item]='xAOD::JetContainer'
        SlimmingHelper.AppendToDictionary[item+"Aux"]='xAOD::JetAuxContainer'
        
    if "Truth" in item:
        continue

    if item in ptThreshDict:
        ptNum = ptThreshDict.get(item)
        thinningTools.append(addJetThinningTool(item,DerivationName,ptNum))
        
##SmartVariables
SlimmingHelper.SmartCollections = [ "InDetTrackParticles",
                                    "Electrons",
                                    "Muons",
                                    "Photons",
                                    "PrimaryVertices"]
##AllVariables
if HIDerivationFlags.isSimulation() : 
    AllVarContent+=["AntiKt2TruthJets","AntiKt4TruthJets","TruthEvents","TruthParticles","AntiKt6TruthJets","AntiKt8TruthJets","AntiKt10TruthJets"] 
	
SlimmingHelper.AllVariables=AllVarContent


ExtraVars=[]
if not HIDerivationFlags.isSimulation(): ExtraVars+=makeHITriggerJetBasicBranchList(HIDerivationFlags.isPP,DerivationName)

#Only basic kinematics for small jets
for collection in [MainJetCollection + "AntiKt2HIJets",MainJetCollection + "AntiKt4HIJets","AntiKt2HIJets_Seed1"] :  
    if "Seed" not in collection:    
        for j in HIJetBasicBranches: 
            ExtraVars.append(collection+'.'+j)
    else:
        for j in HISeedBranches: 
            ExtraVars.append(collection+'.'+j)    


ExtraVars.append("InDetTrackParticles.truthMatchProbability")
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

