#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#


from .AlgInputConfig import AlgInputConfig, InputConfigRegistry
from AthenaCommon.Logging import logging
from TriggerMenuMT.HLTMenuConfig.CommonSequences.CaloSequenceSetup import (
    cellRecoSequence,
    caloClusterRecoSequence,
    LCCaloClusterRecoSequence,
)
from eflowRec.PFHLTSequence import PFHLTSequence
from ..Jet.JetRecoConfiguration import defineJetConstit, interpretJetCalibDefault
from ..Jet.JetRecoConfiguration import recoKeys as jetRecoKeys
from ..Jet.JetTrackingConfig import JetTrackingSequence, trkcollskeys
from ..Jet.JetRecoSequences import jetRecoSequence, getTrkColls
from JetRecConfig.ConstModHelpers import getConstitModAlg, aliasToInputDef
from ..Menu.MenuComponents import RecoFragmentsPool
from TrigEFMissingET.TrigEFMissingETConf import (
    HLT__MET__CVFAlg,
    HLT__MET__CVFPrepAlg,
    HLT__MET__PFOPrepAlg,
    ApproximateTrackToLayerTool,
)
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from ..Menu.SignatureDicts import JetChainParts_Default
from InDetTrackSelectionTool.InDetTrackSelectionToolConf import (
    InDet__InDetTrackSelectionTool,
)
from TrackVertexAssociationTool.TrackVertexAssociationToolConf import (
    CP__TrackVertexAssociationTool,
)
import copy

log = logging.getLogger(__name__)


def jetRecoDictForMET(**recoDict):
    """ Get a jet reco dict that's usable for the MET slice """
    jrd = {k: recoDict.get(k, JetChainParts_Default[k]) for k in jetRecoKeys}
    # Rename the cluster calibration
    try:
        jrd["clusterCalib"] = recoDict["calib"]
    except KeyError:
        pass
    # Fill constitMod
    jrd["constitMod"] = recoDict.get("constitmod", "")
    # We only use em calibration for PFOs
    if jrd["constitType"] == "pf":
        jrd["clusterCalib"] = "em"
    # Interpret jet calibration
    if jrd["jetCalib"] == "default":
        jrd["jetCalib"] = interpretJetCalibDefault(jrd)
    return jrd


default_inputs = InputConfigRegistry()


class CellInputConfig(AlgInputConfig):
    def __init__(self):
        super().__init__(produces=["Cells"], step=0)

    def dependencies(self, recoDict):
        # Cells have no input dependencies
        return []

    def create_sequence(self, inputs, RoIs, recoDict):
        cellSeq, cellName = RecoFragmentsPool.retrieve(
            cellRecoSequence, flags=None, RoIs=RoIs
        )
        return cellSeq, {"Cells": cellName}


default_inputs.add_input(CellInputConfig())


class ClusterInputConfig(AlgInputConfig):
    def __init__(self):
        super().__init__(produces=["Clusters"], step=0)

    def dependencies(self, recoDict):
        # Clusters have no input dependencies
        return []

    def create_sequence(self, inputs, RoIs, recoDict):
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
            raise ValueError(f"Invalid value for cluster calibration: {calib}")
        sequences = [tcSeq]
        if recoDict.get("constitmod"):
            # Force the datatype to topoclusters
            recoDict = copy.copy(recoDict)
            recoDict["constitType"] = "tc"
            jetRecoDict = jetRecoDictForMET(**recoDict)
            constit = aliasToInputDef(
                defineJetConstit(jetRecoDict, clustersKey=clusterName)
            )
            constit_mod_seq = getConstitModAlg(constit)
            sequences.append(constit_mod_seq)
            # Update the name to the modified container name
            clusterName = constit.containername

        return [tcSeq], {"Clusters": clusterName}


default_inputs.add_input(ClusterInputConfig())


class EMClusterInputConfig(AlgInputConfig):
    """ Input config that forces the clusters produced to be at the EM scale
    
    We have this so that we can force PFOs to be produced at the EM scale,
    however a better solution would probably be to add 'em' to the trigger name
    or to change the 'calib' default to be "default" and then have the algorithm
    choose.
    """

    def __init__(self):
        super().__init__(produces=["EMClusters"], step=0)

    def dependencies(self, recoDict):
        # Clusters have no input dependencies
        return []

    def create_sequence(self, inputs, RoIs, recoDict):
        tcSeq, clusterName = RecoFragmentsPool.retrieve(
            caloClusterRecoSequence, flags=None, RoIs=RoIs
        )
        return [tcSeq], {"EMClusters": clusterName}


default_inputs.add_input(EMClusterInputConfig())


class TrackingInputConfig(AlgInputConfig):
    def __init__(self):
        super().__init__(
            produces=copy.copy(trkcollskeys),
            step=1,
        )

    def dependencies(self, recoDict):
        # Tracks have no input dependencies
        return []

    def create_sequence(self, inputs, RoIs, recoDict):
        trkSeq, trkColls = RecoFragmentsPool.retrieve(
            JetTrackingSequence, flags=None, trkopt="ftf", RoIs=RoIs
        )
        return [trkSeq], trkColls


default_inputs.add_input(TrackingInputConfig())


class PFOInputConfig(AlgInputConfig):
    def __init__(self):
        super().__init__(produces=["PFOPrefix", "cPFOs", "nPFOs"])

    def _input_clusters(self, recoDict):
        # Only take the config that forces 'EMClusters' if the calibration
        # isn't set to 'em'. If it's set to em then we want to make sure that
        # we only create those sequences once
        return "Clusters" if recoDict["calib"] == "em" else "EMClusters"

    def dependencies(self, recoDict):
        return [self._input_clusters(recoDict), "Tracks", "Vertices", "TVA"]

    def create_sequence(self, inputs, RoIs, recoDict):
        pfSeq, pfoPrefix = RecoFragmentsPool.retrieve(
            PFHLTSequence,
            flags=None,
            clustersin=inputs[self._input_clusters(recoDict)],
            tracktype="ftf",
        )
        # The jet constituent modifier sequence here is to apply the correct weights
        # and decorate the PV matching decoration. If we've specified constituent
        # modifiers those are also applied.
        recoDict = copy.copy(recoDict)
        # Force the jet data type to the correct thing
        recoDict["constitType"] = "pf"
        jetRecoDict = jetRecoDictForMET(trkopt="ftf", **recoDict)
        constit = aliasToInputDef(defineJetConstit(jetRecoDict, pfoPrefix=pfoPrefix))
        constit_mod_seq = getConstitModAlg(constit)
        # Update the PFO prefix
        pfoPrefix = constit.containername
        if pfoPrefix.endswith("ParticleFlowObjects"):
            pfoPrefix = pfoPrefix[:-19]
        allSeqs = [pfSeq]
        if constit_mod_seq:
            allSeqs.append(constit_mod_seq)
        return (
            allSeqs,
            {
                "PFOPrefix": pfoPrefix,
                "cPFOs": pfoPrefix + "ChargedParticleFlowObjects",
                "nPFOs": pfoPrefix + "NeutralParticleFlowObjects",
            },
        )


default_inputs.add_input(PFOInputConfig())


class MergedPFOInputConfig(AlgInputConfig):
    """ Input config for the PFOs merged into a single container """

    def __init__(self):
        super().__init__(produces=["MergedPFOs", "PFOPUCategory"])

    def dependencies(self, recoDict):
        return ["PFOPrefix", "cPFOs", "nPFOs"]

    def create_sequence(self, inputs, RoIs, recoDict):
        prepAlg = RecoFragmentsPool.retrieve(
            HLT__MET__PFOPrepAlg,
            f"{inputs['PFOPrefix']}METTrigPFOPrepAlg",
            InputNeutralKey=inputs["nPFOs"],
            InputChargedKey=inputs["cPFOs"],
            OutputKey=f"{inputs['PFOPrefix']}METTrigCombinedParticleFlowObjects",
            OutputCategoryKey="PUClassification",
        )
        return (
            [prepAlg],
            {
                "MergedPFOs": prepAlg.OutputKey,
                "PFOPUCategory": prepAlg.OutputCategoryKey,
            },
        )


default_inputs.add_input(MergedPFOInputConfig())


class CVFClusterInputConfig(AlgInputConfig):
    def __init__(self):
        super().__init__(produces=["CVF", "CVFPUCategory"])

    def dependencies(self, recoDict):
        return ["Clusters", "Tracks", "Vertices"]

    def create_sequence(self, inputs, RoIs, recoDict):
        trkopt = "ftf"
        cvfAlg = RecoFragmentsPool.retrieve(
            HLT__MET__CVFAlg,
            f"{recoDict['calib']}{trkopt}ClusterCVFAlg",
            InputClusterKey=inputs["Clusters"],
            InputTrackKey=inputs["Tracks"],
            InputVertexKey=inputs["Vertices"],
            OutputCVFKey="CVF",
            TrackSelectionTool=InDet__InDetTrackSelectionTool(CutLevel="TightPrimary"),
            TVATool=CP__TrackVertexAssociationTool(
                WorkingPoint="Loose", VertexContainer=inputs["Vertices"]
            ),
            ExtensionTool=ApproximateTrackToLayerTool(),
        )
        prepAlg = RecoFragmentsPool.retrieve(
            HLT__MET__CVFPrepAlg,
            f"{recoDict['calib']}{trkopt}ClusterCVFPrepAlg",
            InputClusterKey=inputs["Clusters"],
            InputCVFKey=cvfAlg.OutputCVFKey,
            OutputCategoryKey="PUClassification",
        )
        return (
            [cvfAlg, prepAlg],
            {"CVF": cvfAlg.OutputCVFKey, "CVFPUCategory": prepAlg.OutputCategoryKey},
        )


default_inputs.add_input(CVFClusterInputConfig())


class JetInputConfig(AlgInputConfig):
    """ Helper input config for jets

    Note that if the jets require JVT but are topo jets there is nothing in the
    recoDict (unless the calibration is gsc) to force tracking to run. Therefore
    for this config, you can add an extra key 'forceTracks' to the recoDict
    """

    def __init__(self):
        super().__init__(produces=["Jets", "JetDef"])

    def _input_clusters(self, recoDict):
        # Only take the config that forces 'EMClusters' if the calibration
        # isn't set to 'em'. If it's set to em then we want to make sure that
        # we only create those sequences once
        # NB: for the jets here this is hopefully temporary - we're only forcing
        # the clusters to be EM as we haven't added the lcw jets to the EDM
        return "Clusters" if recoDict["calib"] == "em" else "EMClusters"

    def _use_tracks(self, recoDict):
        """ Whether or not this reco configuration requires tracks """
        if recoDict.get("forceTracks", False):
            return True
        if recoDict["constitType"] == "pf":
            return True
        elif recoDict["constitType"] == "tc":
            return "gsc" in recoDict["jetCalib"]
        else:
            raise ValueError(f"Unexpected constitType {recoDict['constitType']}")

    def dependencies(self, recoDict):
        deps = [self._input_clusters(recoDict)]
        if self._use_tracks(recoDict):
            deps += ["Tracks", "Vertices"]
        return deps

    def create_sequence(self, inputs, RoIs, recoDict):
        trkopt = "ftf" if self._use_tracks(recoDict) else "notrk"
        recoDict = {k: v for k, v in recoDict.items() if k != "forceTracks"}
        jetRecoDict = jetRecoDictForMET(trkopt=trkopt, **recoDict)
        # hard code to em (for now) - there are no LC jets in EDM
        jetRecoDict["clusterCalib"] = "em"

        # Extract the track collections part from our input dict
        trkcolls = {} if trkopt == "notrk" else getTrkColls(inputs)

        jetSeq, jetName, jetDef = RecoFragmentsPool.retrieve(
            jetRecoSequence,
            ConfigFlags,
            clustersKey=inputs[self._input_clusters(recoDict)],
            **trkcolls,
            **jetRecoDict,
        )
        return [jetSeq], {"Jets": jetName, "JetDef": jetDef}


default_inputs.add_input(JetInputConfig())
