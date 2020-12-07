#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import RecoFragmentsPool, MenuSequence
from AthenaCommon.CFElements import seqAND
from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable
from .JetRecoConfiguration import jetRecoDictToString
from AthenaConfiguration.ComponentFactory import CompFactory
from ..CommonSequences.FullScanDefs import caloFSRoI, trkFSRoI

# For step 1, starting from the basic calo reco and topoclustering
# Used for calo-only chains and preselection for tracking
def getInitialInputMaker():
    from TrigT2CaloCommon.CaloDef import clusterFSInputMaker
    InputMakerAlg = conf2toConfigurable(clusterFSInputMaker())
    return InputMakerAlg

# For later steps, where calo reco should not be run
# The same instance of an algorithm cannot be run in different steps
# Used for chains that use tracking
def getTrackingInputMaker():
    InputMakerAlg = conf2toConfigurable(CompFactory.InputMakerForRoI( "IM_Jet_TrackingStep" ))
    InputMakerAlg.RoITool = conf2toConfigurable(CompFactory.ViewCreatorInitialROITool())
    InputMakerAlg.RoIs = trkFSRoI
    return InputMakerAlg


###############################################################################################
# Complete sequences that set up or specify the inputs to jet finding

# This sets up the reconstruction starting from topoclusters.
# No tracking is permitted.
def jetCaloRecoSequences( configFlags, RoIs, **jetRecoDict ):
    if jetRecoDict["trkopt"] != "notrk":
        raise ValueError("Calorimeter jet reco called with a tracking option!")

    # Get the topocluster reconstruction sequence
    from .JetRecoSequences import jetClusterSequence, jetRecoSequence
    topoClusterSequence, clustersKey = RecoFragmentsPool.retrieve(
        jetClusterSequence, configFlags, RoIs=RoIs, clusterCalib=jetRecoDict["clusterCalib"])

    # Get the jet reconstruction sequence including the jet definition and output collection
    jetRecoSeq, jetsOut, jetDef  = RecoFragmentsPool.retrieve(
        jetRecoSequence, configFlags, clustersKey=clustersKey, **jetRecoDict )

    return [topoClusterSequence,jetRecoSeq], jetsOut, clustersKey

# This sets up the reconstruction where tracks are required.
# Topoclustering will not be scheduled, we just pass in the name of the cluster collection.
def jetTrackingRecoSequences(configFlags, RoIs, clustersKey, **jetRecoDict):
    if jetRecoDict["trkopt"] == "notrk":
        raise ValueError("Jet reco with tracks called without a tracking option!")

    # Get the track reconstruction sequence
    from .JetTrackingConfig import JetTrackingSequence
    (jetTrkSeq, trkcolls) = RecoFragmentsPool.retrieve(
        JetTrackingSequence, configFlags, trkopt=jetRecoDict["trkopt"], RoIs=RoIs)

    # Get the jet reconstruction sequence including the jet definition and output collection
    # Pass in the cluster and track collection names
    from .JetRecoSequences import jetRecoSequence
    jetRecoSeq, jetsOut, jetDef  = RecoFragmentsPool.retrieve(
        jetRecoSequence,
        configFlags, clustersKey=clustersKey, **trkcolls, **jetRecoDict )

    return [jetTrkSeq,jetRecoSeq], jetsOut

###############################################################################################
# Functions defining the MenuSequence that will be placed into ChainSteps

# Generate a menu sequence given a set of jet sequences to schedule.
# The hypo may be set up as a preselection hypo, in which case it will
# record a single DecisionObject, instead of one per jet.
# A hypo may alternatively be configured to passThrough, such that
# the hypo will not retrieve any jets and simply pass.

from enum import Enum
class JetHypoAlgType(Enum):
    STANDARD = 0
    PRESEL = 1
    PASSTHROUGH = 2

def makeMenuSequence(jetSeq,IMAlg,jetsIn,jetDefString,hypoType=JetHypoAlgType.STANDARD):

    #hypo
    from TrigHLTJetHypo.TrigJetHypoToolConfig import trigJetHypoToolFromDict
    def trigStreamerHypoTool(chain_dict):
        return conf2toConfigurable(CompFactory.TrigStreamerHypoToolMT(chain_dict["chainName"]))

    hyponame = "TrigJetHypoAlgMT_"+jetDefString
    trigHypoToolGen = trigJetHypoToolFromDict
    if hypoType==JetHypoAlgType.PASSTHROUGH:
        hyponame = "TrigStreamerHypoAlgMT_caloReco"
        hypo = conf2toConfigurable(CompFactory.TrigStreamerHypoAlgMT(hyponame,SetInitialRoIAsFeature=True))
        trigHypoToolGen = trigStreamerHypoTool
    elif hypoType==JetHypoAlgType.PRESEL:
        hyponame += "_presel"
        hypo = conf2toConfigurable(CompFactory.TrigJetHypoAlgMT(hyponame, Jets=jetsIn, DoPresel=True))
    else:
        hypo = conf2toConfigurable(CompFactory.TrigJetHypoAlgMT(hyponame, Jets=jetsIn))

    return  MenuSequence(   Sequence    = jetSeq,
                            Maker       = IMAlg,
                            Hypo        = hypo,
                            HypoToolGen = trigHypoToolGen )

# For the preselection step before running tracking (step 1)
def jetCaloPreselMenuSequence(configFlags, **jetRecoDict):
    InputMakerAlg = getInitialInputMaker()
    jetSequences, jetsIn, clustersKey = RecoFragmentsPool.retrieve(
        jetCaloRecoSequences,
        configFlags, RoIs=caloFSRoI, **jetRecoDict)

    jetDefString = jetRecoDictToString(jetRecoDict)
    jetAthSeq = seqAND("jetSeqCaloPresel_"+jetDefString,[InputMakerAlg]+jetSequences)

    return makeMenuSequence(jetAthSeq,InputMakerAlg,jetsIn,jetDefString,
                            hypoType=JetHypoAlgType.PRESEL), clustersKey

# A null preselection, which will only run the cluster making (step 1)
def jetCaloRecoMenuSequence(configFlags, clusterCalib):
    InputMakerAlg = getInitialInputMaker()
    jetsIn = ""
    from .JetRecoSequences import jetClusterSequence
    topoClusterSequence, clustersKey = RecoFragmentsPool.retrieve(
        jetClusterSequence, configFlags, RoIs=caloFSRoI, clusterCalib=clusterCalib)

    jetAthSeq = seqAND("jetSeqCaloRecoPassThrough_"+clusterCalib,[InputMakerAlg,topoClusterSequence])

    return makeMenuSequence(jetAthSeq,InputMakerAlg,jetsIn,"caloreco",
                            hypoType=JetHypoAlgType.PASSTHROUGH), clustersKey

# A full hypo selecting only on calo jets (step 1)
def jetCaloHypoMenuSequence(configFlags, **jetRecoDict):
    InputMakerAlg = getInitialInputMaker()
    jetSequences, jetsIn, clustersKey = RecoFragmentsPool.retrieve(
        jetCaloRecoSequences,
        configFlags, RoIs=caloFSRoI, **jetRecoDict)
    jetDefString = jetRecoDictToString(jetRecoDict)
    jetAthSeq = seqAND("jetSeqCaloHypo_"+jetDefString,[InputMakerAlg]+jetSequences)

    return makeMenuSequence(jetAthSeq,InputMakerAlg,jetsIn,jetDefString)

# A full hypo selecting on jets with track reco (step 2)
# To combine either with a presel or a passthrough sequence
# As this does not run topoclustering, the cluster collection
# name needs to be passed in
def jetTrackingHypoMenuSequence(configFlags, clustersKey, **jetRecoDict):
    InputMakerAlg = getTrackingInputMaker()
    jetSequences, jetsIn = RecoFragmentsPool.retrieve(
        jetTrackingRecoSequences,
        configFlags, RoIs=trkFSRoI, clustersKey=clustersKey, **jetRecoDict)

    jetDefString = jetRecoDictToString(jetRecoDict)
    jetAthSeq = seqAND("jetSeqTrkHypo_"+jetDefString,[InputMakerAlg]+jetSequences)

    return makeMenuSequence(jetAthSeq,InputMakerAlg,jetsIn,jetDefString)
