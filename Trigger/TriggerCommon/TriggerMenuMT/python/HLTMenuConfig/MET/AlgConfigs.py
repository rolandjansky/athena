#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from .ConfigHelpers import AlgConfig
from ..Menu.SignatureDicts import METChainParts
import GaudiKernel.SystemOfUnits as Units
import TrigEFMissingET.PUClassification as PUClassification
from TrigEFMissingET.TrigEFMissingETConf import (
    HLT__MET__CellFex,
    HLT__MET__TCFex,
    HLT__MET__TCPufitFex,
    HLT__MET__MHTFex,
    HLT__MET__TrkMHTFex,
    HLT__MET__PFSumFex,
    HLT__MET__MHTPufitFex,
    HLT__MET__PUSplitPufitFex,
)


from AthenaCommon.Logging import logging

log = logging.getLogger(__name__)


def test_configs():
    """ Make sure that all algorithms defined in the METChainParts have
    configurations

    Really, this is mainly to have something sensible to call in the
    ConfigHelpers file to succeed the ctest :(
    """
    unknown_algs = []
    for alg in METChainParts["EFrecoAlg"]:
        for subcls in AlgConfig._get_subclasses():
            if subcls.algType() == alg:
                break
        else:
            unknown_algs.append(alg)
    assert len(unknown_algs) == 0, (
        "The following EFrecoAlgs do not have AlgConfig classes: "
        "{}".format(unknown_algs)
    )


class CellConfig(AlgConfig):
    @classmethod
    def algType(cls):
        return "cell"

    def __init__(self, **recoDict):
        super(CellConfig, self).__init__(inputs=["Cells"], **recoDict)

    def make_fex(self, name, inputs):
        return HLT__MET__CellFex(name, CellName=inputs["Cells"])


class TCConfig(AlgConfig):
    @classmethod
    def algType(cls):
        return "tc"

    def __init__(self, calib, **recoDict):
        super(TCConfig, self).__init__(inputs=["Clusters"], calib=calib, **recoDict)

    def make_fex(self, name, inputs):
        return HLT__MET__TCFex(name, ClusterName=inputs["Clusters"])


class TCPufitConfig(AlgConfig):
    @classmethod
    def algType(cls):
        return "tcpufit"

    def __init__(self, calib, **recoDict):
        super(TCPufitConfig, self).__init__(
            inputs=["Clusters"], calib=calib, **recoDict
        )

    def make_fex(self, name, inputs):
        return HLT__MET__TCPufitFex(name, ClusterName=inputs["Clusters"])


class MHTConfig(AlgConfig):
    @classmethod
    def algType(cls):
        return "mht"

    def __init__(self, **recoDict):
        super(MHTConfig, self).__init__(inputs=["Jets"], **recoDict)

    def make_fex(self, name, inputs):
        return HLT__MET__MHTFex(name, JetName=inputs["Jets"])


# NB: TrkMHT isn't ready to run with PF jets yet - for that we need to add an
# option for cPFOs
class TrkMHTConfig(AlgConfig):
    @classmethod
    def algType(cls):
        return "trkmht"

    def __init__(self, **recoDict):
        super(TrkMHTConfig, self).__init__(
            inputs=["Jets", "Tracks", "Vertices", "TVA", "GhostTracksLabel"],
            forceTracks=True,
            **recoDict
        )

    def make_fex(self, name, inputs):
        fex = HLT__MET__TrkMHTFex(
            name,
            JetName=inputs["Jets"],
            TrackName=inputs["Tracks"],
            VertexName=inputs["Vertices"],
            TVAName=inputs["TVA"],
            TrackLinkName=inputs["GhostTracksLabel"],
        )
        fex.TrackSelTool.CutLevel = "Loose"
        fex.TrackSelTool.maxZ0SinTheta = 1.5
        fex.TrackSelTool.maxD0overSigmaD0 = 3
        fex.TrackSelTool.minPt = 1 * Units.GeV
        return fex


class PFSumConfig(AlgConfig):
    @classmethod
    def algType(cls):
        return "pfsum"

    def __init__(self, **recoDict):
        super(PFSumConfig, self).__init__(inputs=["cPFOs", "nPFOs"], **recoDict)

    def make_fex(self, name, inputs):
        return HLT__MET__PFSumFex(
            name, NeutralPFOName=inputs["nPFOs"], ChargedPFOName=inputs["cPFOs"],
        )


class PFOPufitConfig(AlgConfig):
    @classmethod
    def algType(cls):
        return "pfopufit"

    def __init__(self, **recoDict):
        super(PFOPufitConfig, self).__init__(
            inputs=["MergedPFOs", "PFOPUCategory"], **recoDict
        )

    def make_fex(self, name, inputs):
        return HLT__MET__PUSplitPufitFex(
            name,
            InputName=inputs["MergedPFOs"],
            InputCategoryName=inputs["PFOPUCategory"],
            NeutralThresholdMode=PUClassification.NeutralForward,
        )


class CVFPufitConfig(AlgConfig):
    @classmethod
    def algType(cls):
        return "cvfpufit"

    def __init__(self, **recoDict):
        super(CVFPufitConfig, self).__init__(
            inputs=["Clusters", "CVFPUCategory"], **recoDict
        )

    def make_fex(self, name, inputs):
        return HLT__MET__PUSplitPufitFex(
            name,
            InputName=inputs["Clusters"],
            InputCategoryName=inputs["CVFPUCategory"],
            NeutralThresholdMode=PUClassification.All,
        )


class MHTPufitConfig(AlgConfig):
    @classmethod
    def algType(cls):
        return "mhtpufit"

    def __init__(self, **recoDict):
        inputs = ["Jets", "JetDef"]
        if recoDict["constitType"] == "pf":
            inputs += ["MergedPFOs"]
        else:
            inputs += ["Clusters"]
        super(MHTPufitConfig, self).__init__(
            inputs=inputs, forceTracks=True, **recoDict
        )

    def make_fex(self, name, inputs):
        calibHasAreaSub = "sub" in self.recoDict
        if calibHasAreaSub:
            from JetRecConfig.JetRecConfig import instantiateAliases
            from JetRecConfig.JetInputConfig import buildEventShapeAlg

            instantiateAliases(inputs["JetDef"])
            evtShapeAlg = buildEventShapeAlg(inputs["JetDef"], "HLT_")

            rhoKey = evtShapeAlg.EventDensityTool.OutputContainer
        else:
            rhoKey = ""
        if self.recoDict["constitType"] == "pf":
            inputName = inputs["MergedPFOs"]
        else:
            inputName = inputs["Clusters"]
        return HLT__MET__MHTPufitFex(
            name,
            InputJetsName=inputs["Jets"],
            InputName=inputName,
            JetCalibIncludesAreaSub=calibHasAreaSub,
            JetEventShapeName=rhoKey,
        )
