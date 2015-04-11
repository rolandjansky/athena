from HIJetRec.HIJetRecFlags import HIJetFlags
from HIGlobal.HIGlobalFlags import jobproperties
from JetRec.JetRecFlags import jetFlags
jetFlags.debug=2
jetFlags.timeJetRecTool=2
jetFlags.timeJetToolRunner=2
jetFlags.useCells.set_Value(True);


from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#is getter tool okay instead?
if not hasattr(topSequence,"CmbTowerBldr") :
    include("CaloRec/CaloCombinedTower_jobOptions.py")

#check for event shape
#if hasattr(topSequence,"HIEventShapeAlgorithm")
EventShapeKey=jobproperties.HIGlobalFlags.EventShapeKey()
ClusterKey=HIJetFlags.HIClusterKey()

#import utility functions
from  HIJetRec.HIJetRecUtils import *
HIClusterGetter()


used_JetRec=hasattr(topSequence,"jetalg") 

#equip basic tools
from  HIJetRec.HIJetRecTools import jtm

###
##truth jets
if jetFlags.useTruth(): 
    if not used_JetRec :
        from JetRec.JetFlavorAlgs import scheduleCopyTruthParticles
        scheduleCopyTruthParticles(topSequence)


    for R in HIJetFlags.AntiKtRValues() : 
        tname="AntiKt%dTruthJets" % int(10*R)
        if tname in jtm.tools : continue
        if rec.readESD() :
            from RecExConfig.ObjKeyStore import objKeyStore
            inputcontent = objKeyStore['inputFile'].list()
            if tname in inputcontent : continue
        jtm.addJetFinder(tname,"AntiKt", R,"truth", ptmin= HIJetFlags.TruthJetPtMin())
        AddToOutputList(tname)

#track jets
if jetFlags.useTracks():
    tname="AntiKt4HITrackJets"
    hi_trk_modifiers=jtm.modifiersMap["HITrack"]
    hi_trk_modifiers += AddPtAssociationTools(0.4,False)
    jtm.addJetFinder(tname, "AntiKt", 0.4, "HITrack", hi_trk_modifiers, ptmin= HIJetFlags.TrackJetPtMin())
    HIJetFlags.TrackJetContainerName=tname
    AddToOutputList(tname)


#initial unsubtracted jets
for R in HIJetFlags.AntiKtRValues() :  AddHIJetFinder(R)

seeds0=jtm.addJetCopier("AntiKt2HIJets_%s0" % HIJetFlags.SeedSuffix(),"AntiKt2HIJets_Unsubtracted",[jtm.discrim],shallow=False)
iter0=AddIteration(suffix="iter0",seed_container=seeds0.OutputContainer,shape_name=EventShapeKey)

subtr1=MakeSubtractionTool(iter0.OutputEventShapeKey)
seeds1=jtm.addJetCopier("AntiKt2HIJets_%s1" % HIJetFlags.SeedSuffix(),"AntiKt2HIJets_Unsubtracted",[subtr1,jtm.jetfil20],shallow=False) #add calib tool

# if jetFlags.useTracks() :
#     seeds1a=jtm.addJetCopier("AntiKt2HIJets_seeds1","AntiKt2HIJets_Unsubtracted",[subtr1,jtm.jetfil20],shallow=False) #add calib tool
#     seeds1= copy operation (seeds1a.OutputContainer,HIJetFlags.TrackJetContainerName())


iter1=AddIteration(suffix="iter1",seed_container=seeds1.OutputContainer,shape_name=EventShapeKey)
HIJetFlags.IteratedEventShapeKey=iter1.OutputEventShapeKey
subtr2=MakeSubtractionTool(HIJetFlags.IteratedEventShapeKey())

ApplySubtractionToClusters(event_shape_key=HIJetFlags.IteratedEventShapeKey(), cluster_key=ClusterKey)
subtr1=MakeSubtractionTool(iter0.OutputEventShapeKey,moment_name="subtr1",momentOnly=True)
#put subtraction tool at the FRONT of the jet modifiers list
hi_tools=[subtr1,subtr2]
hi_tools += jtm.modifiersMap['HI']
jtm.modifiersMap['HI']=hi_tools

###
#subtracted algorithms
#make main jets from unsubtr collections w/ same R, add modifiers for subtraction

unsubtr_suffix=HIJetFlags.UnsubtractedSuffix()
for k in jtm.jetrecs :
    if unsubtr_suffix in k.name() :
        in_name=k.OutputContainer
        out_name=in_name.replace("_%s" % unsubtr_suffix,"")
        copier=jtm.addJetCopier(out_name,in_name,"HI",shallow=False)
        AddToOutputList(out_name)



rtools=[]
if hasattr(jtm,"jetrun") :
    for t in jtm.jetrecs :
        if t not in jtm.jetrun.Tools : rtools+=[t]
    AppendOutputList(jetFlags.jetAODList)
else :  rtools=jtm.jetrecs
JetAlgFromTools(rtools,suffix="HI",persistify=True)



