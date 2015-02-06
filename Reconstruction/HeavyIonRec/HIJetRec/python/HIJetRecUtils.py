# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from HIJetRec.HIJetRecFlags import HIJetFlags
from JetRec.JetRecFlags import jetFlags
from JetRec.JetRecStandard import jtm

HIJetFinderDict={}
HIJetFinderDefaults=dict(ghostArea=0.01, ptminFilter= 5000)

EventShapeDict={}

from HIJetRec.HIJetRecFlags import HIJetFlags
unsubtr_suffix=HIJetFlags.UnsubtractedSuffix()


def AppendOutputList(HIAODItemList) :
    """Adds HIJetAODOutputList to the list passed in as an argument"""
    #need to add procedure for straight copy of input
    from HIJetRec.HIJetRecFlags import HIJetFlags
    HIAODItemList+=HIJetFlags.HIJetAODItemList()

def HIClusterGetter(tower_key="CombinedTower", cell_key="AllCalo", cluster_key="", pers=True) :
    """Function to equip HI cluster builder from towers and cells, adds to output AOD stream"""

    if cluster_key == "" :
        from HIJetRec.HIJetRecFlags import HIJetFlags
        cluster_key=HIJetFlags.HIClusterKey()

    from HIJetRec.HIJetRecConf import HIClusterMaker
    theAlg=HIClusterMaker()
    theAlg.OutputLevel=0
    theAlg.InputTowerKey=tower_key
    theAlg.CaloCellContainerKey=cell_key
    theAlg.OutputContainerKey=cluster_key

    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    topSequence += theAlg

    from HIJetRec.HIJetRecFlags import HIJetFlags
    if pers:
        HIJetFlags.HIJetAODItemList+=["xAOD::CaloClusterContainer_v1#"+cluster_key]
        HIJetFlags.HIJetAODItemList+=["xAOD::CaloClusterAuxContainer_v1#"+cluster_key+"Aux."]
        HIJetFlags.HIJetAODItemList+=["CaloClusterCellLinkContainer#"+cluster_key+"_links"]
    return theAlg


def AddHIJetFinder(R=0.4) :
    cname="AntiKt%dHIJets_%s" % (int(10*R),unsubtr_suffix)
    if cname not in HIJetFinderDict :
        finder=jtm.addJetFinder(cname, "AntiKt", R, "HI","HI_Unsubtr",HIJetFinderDefaults)
        HIJetFinderDict[cname]=finder


def MakeSubtractionTool(shapeKey, moment_name='subtracted') :
    from HIJetRec.HIJetRecConf import HIJetConstituentSubtractionTool
    subtr=HIJetConstituentSubtractionTool("HIConstituentSubtractor_%s" % shapeKey)
    subtr.EventShapeKey=shapeKey
    subtr.MomentName=moment_name
    if not hasattr(jtm,"HIJetSubtractor") : 
        from HIJetRec.HIJetRecConf import HIJetCellSubtractorTool
        cell_subtr=HIJetCellSubtractorTool("HIJetSubtractor")
        jtm.add(cell_subtr)

    subtr.Subtractor=jtm.HIJetSubtractor
    jtm.add(subtr)
    return subtr

def ApplySubtractionToClusters(event_shape_key="", cluster_key="") :
    if event_shape_key == "":
        from HIGlobal.HIGlobalFlags import jobproperties
        event_shape_key=jobproperties.HIGlobalFlags.EventShapeKey()

    if cluster_key == "" :
        from HIJetRec.HIJetRecFlags import HIJetFlags
        cluster_key=HIJetFlags.HIClusterKey()

    if not hasattr(jtm,"HIJetSubtractor") : 
        from HIJetRec.HIJetRecConf import HIJetCellSubtractorTool
        cell_subtr=HIJetCellSubtractorTool("HIJetSubtractor")
        jtm.add(cell_subtr)

    from HIJetRec.HIJetRecConf import HIClusterSubtraction
    theAlg=HIClusterSubtraction()
    theAlg.ClusterKey=cluster_key
    theAlg.EventShapeKey=event_shape_key
    theAlg.Subtractor=jtm.HIJetSubtractor
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    topSequence += theAlg

def JetAlgFromTools(rtools, suffix="HI",persistify=True) :
    from JetRec.JetRecConf import JetToolRunner
    runner=JetToolRunner("jetrun"+suffix, Tools=rtools, Timer=jetFlags.timeJetToolRunner())
    jtm.add(runner)

    from JetRec.JetRecConf import JetAlgorithm
    theAlg=JetAlgorithm("jetalg"+suffix)
    theAlg.Tools = [runner]
    from AthenaCommon.AlgSequence import AlgSequence
    topsequence = AlgSequence()
    topsequence += theAlg

    from GaudiKernel.Constants import DEBUG
    if jetFlags.debug > 0:

      jtm.setOutputLevel(runner, DEBUG)
      theAlg.OutputLevel = DEBUG

    if jetFlags.debug > 1:
      for tool in jtm.jetrecs:
        jtm.setOutputLevel(tool, DEBUG)

    if jetFlags.debug > 2:
      for tool in jtm.finders:
        jtm.setOutputLevel(tool, DEBUG)

    if jetFlags.debug > 3:
      jtm.setOutputLevel(jtm.jetBuilder, DEBUG)
         
    if persistify :
        for t in rtools:
            HIJetFlags.HIJetAODItemList += [ "xAOD::JetContainer_v1#" + t.name() ]
            HIJetFlags.HIJetAODItemList += [ "xAOD::JetAuxContainer_v1#" + t.name() + "Aux." ]
    return theAlg


# def makeEventShapeUsingSeeds(inkey="", outkey="", seeds1=[], seeds2=[]) :
#     from HIJetRec.HIJetRecConf import HIJetEventShapeModifier
#     from HIGlobal.HIGlobalFlags import jobproperties
#     if inkey == ""
#         inkey=jobproperties.HIGlobalFlags.HIEventShapeKey
#     if outkey== "":
#         pass #do default naming
#     if outkey in EventShapeDict.keys() :
#         #print message
#         return EventShapeDict[outkey]

#     theAlg=HIJetEventShapeModifier("HIJetEventShapeModifier_%s" % outkey)
#     theAlg.InputEventShapeKey=inkey
#     theAlg.OutputEventShapeKey=outkey
#     theAlg.CaloJetCollectionKeys=seeds1
#     theAlg.OtherJetCollectionKeys=seeds2
#     theAlg.ExclusionRadius=0.4;
#     topSequence += theAlg
#     EventShapeDict[outkey]=theAlg
#     return theAlg


# def doIteration() :

#     #build seeds
#     from HIJetRec.HIJetRecConf import HIJetDiscriminatorTool
#     discrim=HIJetDiscriminatorTool()
#     discrim.MinTowerEt=3.*GeV #make set by flags
#     discrim.Dcut=4 #make set by flags
#     jtm.add(discrim)

#     #add modifier tool for seedless background
#     #not used in iteration but probably still want subtracted kinematics
#     from HIGlobal.HIGlobalFlags import jobproperties
#     subtr0=makeSubtractionTool(jobproperties.HIGlobalFlags.HIEventShapeKey)
    
#     seed1=jtm.addJetCopier("AntiKt2HI_Seed1","AntiKt2HI_%s" % unsubtr_suffix,[discrim])
#     shapeAlg1=makeEventShapeUsingSeeds(outkey="HIEventShape_%s" % seed1.OutputContainer ,seeds_list_calo=[seed1.OutputContainer])
#     subtr1=makeSubtractionTool(shapeAlg1.OutputEventShapeKey)
    
#     #iteration step, pass subtraction tool from previous step
#     seed2=jtm.addJetCopier("AntiKt2HI_Seed2","AntiKt2HI_unsubtr",[subtr1]) #also add ptmin filter set by flags
#     tj_list=[]
#     #check to see that track jets have been configured
#     tj_list=["AntiKt4TrackJets"]
#     shapeAlg2=makeEventShapeUsingSeeds(outkey="HIEventShape_",seed_list_calo=[seed2.OutputContainer],seed_list_nocalo=tj_list) #fix naming
#     subtr2=makeSubtractionTool(shapeAlg2.OutputEventShapeKey)


#customize quality cuts?
# jtm += JetCaloQualityTool(
#     "caloqual_cluster",
#     DoFracSamplingMax = True,
#     DoN90 = True,
#     DoLArQuality = True,
#     LArQualityCut = 4000,
#     TileQualityCut = 254,
#     DoTiming = True,
#     TimingCellTimeCuts = [],
#     TimingClusterTimeCuts = [5, 10],
#     DoNegativeE = True,
#     DoHECQuality = True,
#     DoAverageLArQF = True,
#     DoJetCentroid = True,
#     ComputeVariableFromCluster = True
#     )

