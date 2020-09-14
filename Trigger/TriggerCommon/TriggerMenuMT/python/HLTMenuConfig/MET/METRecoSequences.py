#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#


from .ConfigHelpers import AlgConfig, jetRecoDictForMET
from AthenaCommon.Logging import logging
from TriggerMenuMT.HLTMenuConfig.CommonSequences.CaloSequenceSetup import (
    caloClusterRecoSequence,
    LCCaloClusterRecoSequence,
)
from eflowRec.PFHLTSequence import PFHLTSequence
from ..Jet.JetRecoConfiguration import defineJetConstit
from ..Jet.JetTrackingConfig import JetTrackingSequence
from JetRecConfig.ConstModHelpers import getConstitModAlg
from ..Menu.MenuComponents import RecoFragmentsPool
from TrigEFMissingET.TrigEFMissingETConf import (
    HLT__MET__CVFAlg,
    ApproximateTrackToLayerTool,
)
from InDetTrackSelectionTool.InDetTrackSelectionToolConf import (
    InDet__InDetTrackSelectionTool,
)
from TrackVertexAssociationTool.TrackVertexAssociationToolConf import (
    CP__TrackVertexAssociationTool,
)


log = logging.getLogger(__name__)


def metAthSequence(dummyFlags, **recoDict):
    conf = AlgConfig.fromRecoDict(**recoDict)
    return conf.athSequence, conf.inputMaker, conf.outputKey


def metRecoSequence(dummyFlags, **recoDict):
    conf = AlgConfig.fromRecoDict(**recoDict)
    return conf.recoSequence, conf.outputKey


def pfoRecoSequence(dummyFlags, RoIs, **recoDict):
    """ Get PFOs as inputs for the MET algorithms
    
    Returns the list of input sequences and the pfo prefix
    """

    tcSeq, clusterName = RecoFragmentsPool.retrieve(
        caloClusterRecoSequence, flags=None, RoIs=RoIs
    )
    pfSeq, pfoPrefix = RecoFragmentsPool.retrieve(
        PFHLTSequence, flags=None, clustersin=clusterName, tracktype="ftf"
    )
    # The jet constituent modifier sequence here is to apply the correct weights
    # and decorate the PV matching decoration
    jetRecoDict = jetRecoDictForMET(trkopt="ftf", **recoDict)
    jetRecoDict["calib"] = "em"
    jetRecoDict["dataType"] = "pf"
    constit = defineJetConstit(jetRecoDict, pfoPrefix=pfoPrefix)
    constit_mod_seq = getConstitModAlg(
        constit,
        "HLT",
        tvaKey="JetTrackVtxAssoc_{trkopt}".format(**jetRecoDict),
        vtxKey="HLT_IDVertex_FS",
    )
    return [tcSeq, pfSeq, constit_mod_seq], pfoPrefix


def cvfClusterSequence(dummyFlags, RoIs, **recoDict):
    """ Get the clusters with the CVF calculated

    Returns the list of input sequences, the cluster container name and the CVF
    decoration name
    """
    calib = recoDict["calib"]
    if calib == "em":
        tcSeq, clusterName = RecoFragmentsPool.retrieve(
            caloClusterRecoSequence, flags=None, RoIs=RoIs
        )
    elif calib == "lcw":
        tcSeq, clusterName = RecoFragmentsPool.retrieve(
            LCCaloClusterRecoSequence, flags=None, RoIs=RoIs
        )
    else:
        raise ValueError("Unsupported calib state {} requested!".format(calib))

    trkopt = "ftf"
    trackSeq, trackColls = RecoFragmentsPool.retrieve(
        JetTrackingSequence, flags=None, trkopt=trkopt, RoIs=RoIs
    )

    # These are the names set by the upstream algorithms. Unfortunately
    # these aren't passed to us - we just have to 'know' them
    tracks = "HLT_IDTrack_FS_FTF"
    vertices = "HLT_IDVertex_FS"

    cvfAlg = RecoFragmentsPool.retrieve(
        HLT__MET__CVFAlg,
        "{}{}ClusterCVFAlg".format(calib, trkopt),
        InputClusterKey=clusterName,
        InputTrackKey=tracks,
        InputVertexKey=vertices,
        OutputCVFKey="CVF",
        TrackSelectionTool=InDet__InDetTrackSelectionTool(CutLevel="TightPrimary"),
        TVATool=CP__TrackVertexAssociationTool(
            WorkingPoint="Loose", VertexContainer=vertices
        ),
        ExtensionTool=ApproximateTrackToLayerTool(),
    )
    return [tcSeq, trackSeq, cvfAlg], clusterName, "CVF"
