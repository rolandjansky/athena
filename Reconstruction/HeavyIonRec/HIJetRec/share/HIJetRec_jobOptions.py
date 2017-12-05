from HIJetRec.HIJetRecFlags import HIJetFlags
from HIGlobal.HIGlobalFlags import jobproperties
from JetRec.JetRecFlags import jetFlags
jetFlags.useCells.set_Value(True);


from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from CaloRec.CaloTowerCmbGetter import CaloTowerCmbGetter
CaloTowerCmbGetter()


#check for event shape
#if hasattr(topSequence,"HIEventShapeAlgorithm")
EventShapeKey=jobproperties.HIGlobalFlags.EventShapeKey()
ClusterKey=HIJetFlags.HIClusterKey()

#import utility functions
from  HIJetRec.HIJetRecUtils import *
HIClusterGetter()


#equip basic tools
from  HIJetRec.HIJetRecTools import jtm

###
##truth jets
if jetFlags.useTruth(): 
    for R in HIJetFlags.AntiKtRValues() : 
        tname="AntiKt%dTruthJets" % int(10*R)
        collExists=False
        if tname in jtm.tools : continue
        if rec.readESD() :
            from RecExConfig.ObjKeyStore import objKeyStore
            inputcontent = objKeyStore['inputFile'].list()
            for t in inputcontent :
                if tname in t:
                    print 'Truth collection %s already exists, no need to rebuild it' % tname
                    collExists=True
                    break
        if collExists: continue
        f=jtm.addJetFinder(tname,"AntiKt", R,"truth", ptmin= HIJetFlags.TruthJetPtMin())
        print 'Adding %s' %tname
        AddToOutputList(tname)
        jtm.HIJetRecs+=[f]

#track jets
if jetFlags.useTracks():
    tname="AntiKt4HITrackJets"
    hi_trk_modifiers=jtm.modifiersMap["HITrack"]
    hi_trk_modifiers += AddPtAssociationTools(0.4,False)
    f=jtm.addJetFinder(tname, "AntiKt", 0.4, "HITrack", hi_trk_modifiers, ptmin= HIJetFlags.TrackJetPtMin())
    HIJetFlags.TrackJetContainerName=tname
    AddToOutputList(tname)
    jtm.HIJetRecs+=[f]


#initial unsubtracted jets
for R in HIJetFlags.AntiKtRValues() :  AddHIJetFinder(R)

#initial seeds
if HIJetFlags.SeedRValue() not in HIJetFlags.AntiKtRValues() :AddHIJetFinder(HIJetFlags.SeedRValue())
seed_prefix='AntiKt%dHIJets' % int(10*HIJetFlags.SeedRValue())
seeds0=jtm.addJetCopier("%s_%s0" % (seed_prefix, HIJetFlags.SeedSuffix()),"%s_Unsubtracted" % seed_prefix,[jtm.discrim],shallow=False)
jtm.HIJetRecs+=[seeds0]
iter0=AddIteration(seed_container=seeds0.OutputContainer,shape_name=EventShapeKey,suffix="iter0")
modulator0=iter0.Modulator
subtr1=MakeSubtractionTool(iter0.OutputEventShapeKey,modulator=modulator0)

#now iterate
seeds1=jtm.addJetCopier("%s_%s1" % (seed_prefix,HIJetFlags.SeedSuffix()),"%s_Unsubtracted" % seed_prefix,[subtr1,jtm.HICalibMap[seed_prefix],jtm.jetfilHISeeds],shallow=False)
jtm.HIJetRecs+=[seeds1]
iteration_dict=dict(suffix="iter1")
if jetFlags.useTracks() and HIJetFlags.TrackJetSeeds() : iteration_dict['track_jet_seeds']=HIJetFlags.TrackJetContainerName()
iter1=AddIteration(seed_container=seeds1.OutputContainer,shape_name=EventShapeKey,**iteration_dict)

HIJetFlags.IteratedEventShapeKey=iter1.OutputEventShapeKey
modulator1=iter1.Modulator
jtm.modulator=modulator1

#apply subtraction to cluster constituents
ApplySubtractionToClusters(event_shape_key=HIJetFlags.IteratedEventShapeKey(), cluster_key=ClusterKey, modulator=modulator1,CalculateMoments=True)

#subtraction BEFORE iteration for moment
subtr1=MakeSubtractionTool(iter0.OutputEventShapeKey,moment_name="NoIteration",momentOnly=True,modulator=modulator0)
#main subtractor
subtr2=MakeSubtractionTool(HIJetFlags.IteratedEventShapeKey(),modulator=modulator1)

#put subtraction tool at the FRONT of the jet modifiers list
hi_tools=[subtr1,subtr2]
hi_tools+=GetFlowMomentTools(iter1.OutputEventShapeKey,iter1.ModulationEventShapeKey)

###
#subtracted algorithms
#make main jets from unsubtr collections w/ same R, add modifiers for subtraction

unsubtr_suffix=HIJetFlags.UnsubtractedSuffix()
for k in jtm.jetrecs :
    if unsubtr_suffix in k.name() :
        in_name=k.OutputContainer
        out_name=in_name.replace("_%s" % unsubtr_suffix,"")
        copier=jtm.addJetCopier(out_name,in_name,GetHIModifierList(out_name,hi_tools),shallow=False)
        AddToOutputList(out_name)
        jtm.HIJetRecs+=[copier]


AppendOutputList(jetFlags.jetAODList)
JetAlgFromTools(jtm.HIJetRecs,suffix="HI",persistify=True)




