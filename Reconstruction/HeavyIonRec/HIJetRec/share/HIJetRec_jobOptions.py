from HIJetRec.HIJetRecFlags import HIJetFlags
from HIGlobal.HIGlobalFlags import jobproperties
from JetRec.JetRecFlags import jetFlags
jetFlags.debug.set_Value_and_Lock(4);
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

#equip basic tools
from  HIJetRec.HIJetTools import *


#
#
#begin alg scheduling
#
#

###
#initial jet algorithm
#track jets
if jetFlags.useTracks():
    jtm.addJetFinder("AntiKt4HITrackJets", "AntiKt", 0.4, "pv0track", ptmin= 2000)

#truth jets

#initial unsubtracted jets
hasTruthR4=False
from RecExConfig.ObjKeyStore import objKeyStore
inputcontent = objKeyStore['inputFile'].list()

rtools=[]
if jetFlags.useCells():
    rtools += [jtm.missingcells]
if jetFlags.useTracks:
    rtools += [jtm.tracksel]
    rtools += [jtm.tvassoc]

for R in HIJetFlags.AntiKtRValues() : 
    AddHIJetFinder(R)
    tname="AntiKt%dTruthJets" % int(10*R)
    if jetFlags.useTruth(): 
        copyOnly=False
        for o in inputcontent : 
            if tname in o : 
                copyOnly=True
                break
        if copyOnly and tname not in jtm.jetcons : jtm.jetcons+=[tname]
        else : jtm.addJetFinder(tname,"AntiKt", R,"truth", ptmin= 5000)
rtools+=jtm.jetrecs
JetAlgFromTools(rtools,suffix="Unsubtr",persistify=False)

subtr0=MakeSubtractionTool(EventShapeKey, moment_name="subtr0") #for now, no iteration
#
#Iteration steps
#
#make some seeds and discriminate using moments
seeds0=jtm.addJetCopier("AntiKt2HIJets_seeds0","AntiKt2HIJets_Unsubtracted",[discrim]);
HIJetFinderDict[seeds0.OutputContainer]=seeds0
JetAlgFromTools([seeds0],suffix="seeds0",persistify=False)

from HIJetRec.HIJetRecConf import HIEventShapeJetIteration
iter0=HIEventShapeJetIteration("iter0")
iter0.InputEventShapeKey=EventShapeKey
iter0.OutputEventShapeKey=EventShapeKey+"_iter0"
iter0.AssociationKey=assoc_name
iter0.SeedContainerKeys=[seeds0.OutputContainer]
topSequence += iter0

subtr1=MakeSubtractionTool(iter0.OutputEventShapeKey,moment_name="subtr1")
seeds1=jtm.addJetCopier("AntiKt2HIJets_seeds1","AntiKt2HIJets_Unsubtracted",[subtr0,jtm.jetfil20]) #add calib tool
HIJetFinderDict[seeds1.OutputContainer]=seeds1
JetAlgFromTools([seeds1],suffix="seeds1",persistify=False)

iter1=HIEventShapeJetIteration("iter1")
iter1.InputEventShapeKey=EventShapeKey
iter1.OutputEventShapeKey=EventShapeKey+"_iter1"
iter1.SeedContainerKeys=[seeds1.OutputContainer]#,jtm.AntiKt4HITrackJets.OutputContainer]#eventually add track jets
iter1.AssociationKey=assoc_name
topSequence += iter1
subtr2=MakeSubtractionTool(iter1.OutputEventShapeKey,moment_name="subtr2")
#subtr2.SetMomentOnly=False
#store key of canonical background


#put subtraction tool at the FRONT of the jet modifiers list


hi_tools=[subtr0,subtr1,subtr2]
hi_tools += jtm.modifiersMap['HI']
jtm.modifiersMap['HI']=hi_tools

###
#subtracted algorithms
#make main jets from unsubtr collections w/ same R, add modifiers for subtraction
rtools = []
for k in HIJetFinderDict.keys() :
    if 'Unsubtr' in k :
        in_name=k
        out_name=in_name.replace("_%s" % unsubtr_suffix,"")
        copier=jtm.addJetCopier(out_name,in_name,"HI")
        HIJetFinderDict[out_name]=copier
        rtools+=[copier]

JetAlgFromTools(rtools,suffix="HI",persistify=True)

#eventually pass iterated event shape, event_shape_key=iter1.OutputEventShapeKey
ApplySubtractionToClusters(event_shape_key=EventShapeKey, cluster_key=ClusterKey)
