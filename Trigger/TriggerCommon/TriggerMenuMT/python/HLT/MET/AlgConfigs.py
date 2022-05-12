#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

import errno
import json
import os

from .ConfigHelpers import AlgConfig
from .HLTInputConfig import HLTInputConfigRegistry
from ..Menu.SignatureDicts import METChainParts
import GaudiKernel.SystemOfUnits as Units
import TrigEFMissingET.PUClassification as PUClassification
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.Utils.unixtools import find_datafile


from AthenaCommon.Logging import logging

log = logging.getLogger(__name__)


def test_configs():
    """Make sure that all algorithms defined in the METChainParts have
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
    assert (
        len(unknown_algs) == 0
    ), "The following EFrecoAlgs do not have AlgConfig classes: " "{}".format(
        unknown_algs
    )


class CellConfig(AlgConfig):
    @classmethod
    def algType(cls):
        return "cell"

    def __init__(self, **recoDict):
        super(CellConfig, self).__init__(inputs=["Cells"], **recoDict)

    def make_fex_accumulator(self, flags, name, inputs):
        return CompFactory.getComp("HLT::MET::CellFex")(name, CellName=inputs["Cells"])


class TCConfig(AlgConfig):
    @classmethod
    def algType(cls):
        return "tc"

    def __init__(self, calib, **recoDict):
        super(TCConfig, self).__init__(inputs=["Clusters"], calib=calib, **recoDict)

    def make_fex_accumulator(self, flags, name, inputs):
        return CompFactory.getComp("HLT::MET::TCFex")(
            name, ClusterName=inputs["Clusters"]
        )


class TCPufitConfig(AlgConfig):
    @classmethod
    def algType(cls):
        return "tcpufit"

    def __init__(self, calib, nSigma, **recoDict):
        super(TCPufitConfig, self).__init__(
            inputs=["Clusters"], calib=calib, nSigma=nSigma, **recoDict
        )
        if nSigma == "default":
            nSigma = "sig50"
        # Strip off the 'sig' part of the string, convert the end to a float, then divide by 10
        self.n_sigma = float(nSigma[3:]) / 10.0

    def make_fex_accumulator(self, flags, name, inputs):
        return CompFactory.getComp("HLT::MET::TCPufitFex")(
            name, ClusterName=inputs["Clusters"], NSigma=self.n_sigma
        )


class MHTConfig(AlgConfig):
    @classmethod
    def algType(cls):
        return "mht"

    def __init__(self, **recoDict):
        super(MHTConfig, self).__init__(inputs=["Jets"], **recoDict)

    def make_fex_accumulator(self, flags, name, inputs):
        return CompFactory.getComp("HLT::MET::MHTFex")(name, JetName=inputs["Jets"])


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

    def make_fex_accumulator(self, flags, name, inputs):
        return CompFactory.getComp("HLT::MET::TrkMHTFex")(
            name,
            JetName=inputs["Jets"],
            TrackName=inputs["Tracks"],
            VertexName=inputs["Vertices"],
            TVAName=inputs["TVA"],
            TrackLinkName=inputs["GhostTracksLabel"],
            TrackSelTool=CompFactory.getComp("InDet::InDetTrackSelectionTool")(
                CutLevel="Loose",
                maxZ0SinTheta=1.5,
                maxD0overSigmaD0=3,
                minPt=1 * Units.GeV,
            ),
        )


class PFSumConfig(AlgConfig):
    @classmethod
    def algType(cls):
        return "pfsum"

    def __init__(self, **recoDict):
        super(PFSumConfig, self).__init__(inputs=["cPFOs", "nPFOs"], **recoDict)

    def make_fex_accumulator(self, flags, name, inputs):
        return CompFactory.getComp("HLT::MET::PFSumFex")(
            name,
            NeutralPFOName=inputs["nPFOs"],
            ChargedPFOName=inputs["cPFOs"],
        )


class PFOPufitConfig(AlgConfig):
    @classmethod
    def algType(cls):
        return "pfopufit"

    def __init__(self, nSigma, **recoDict):
        super(PFOPufitConfig, self).__init__(
            inputs=["MergedPFOs", "PFOPUCategory"], nSigma=nSigma, **recoDict
        )
        if nSigma == "default":
            nSigma = "sig50"
        # Strip off the 'sig' part of the string, convert the end to a float, then divide by 10
        self.n_sigma = float(nSigma[3:]) / 10.0

    def make_fex_accumulator(self, flags, name, inputs):
        return CompFactory.getComp("HLT::MET::PUSplitPufitFex")(
            name,
            InputName=inputs["MergedPFOs"],
            InputCategoryName=inputs["PFOPUCategory"],
            NeutralThresholdMode=PUClassification.NeutralForward,
            NSigma=self.n_sigma,
        )


class CVFPufitConfig(AlgConfig):
    @classmethod
    def algType(cls):
        return "cvfpufit"

    def __init__(self, nSigma, **recoDict):
        super(CVFPufitConfig, self).__init__(
            inputs=["Clusters", "CVFPUCategory"], nSigma=nSigma, **recoDict
        )
        if nSigma == "default":
            nSigma = "sig50"
        # Strip off the 'sig' part of the string, convert the end to and int, then divide by 10
        self.n_sigma = int(nSigma[3:]) / 10.0

    def make_fex_accumulator(self, flags, name, inputs):
        return CompFactory.getComp("HLT::MET::PUSplitPufitFex")(
            name,
            InputName=inputs["Clusters"],
            InputCategoryName=inputs["CVFPUCategory"],
            NeutralThresholdMode=PUClassification.All,
            NSigma=self.n_sigma,
        )


class MHTPufitConfig(AlgConfig):
    @classmethod
    def algType(cls):
        return "mhtpufit"

    def __init__(self, nSigma, **recoDict):
        inputs = ["Jets", "JetDef"]
        if recoDict["constitType"] == "pf":
            inputs += ["MergedPFOs"]
        else:
            inputs += ["Clusters"]
        super(MHTPufitConfig, self).__init__(
            inputs=inputs, forceTracks=True, nSigma=nSigma, **recoDict
        )
        if nSigma == "default":
            nSigma = "sig50"
        # Strip off the 'sig' part of the string, convert the end to and int, then divide by 10
        self.n_sigma = int(nSigma[3:]) / 10.0

    def make_fex_accumulator(self, flags, name, inputs):
        calibHasAreaSub = "sub" in self.recoDict
        if calibHasAreaSub:
            from JetRecConfig.JetRecConfig import instantiateAliases
            from JetRecConfig.JetInputConfig import getEventShapeName

            instantiateAliases(inputs["JetDef"])
            rhoKey = getEventShapeName(inputs["JetDef"], "HLT_")
        else:
            rhoKey = ""
        return CompFactory.getComp("HLT::MET::MHTPufitFex")(
            name,
            InputJetsName=inputs["Jets"],
            InputName=inputs[
                "MergedPFOs" if self.recoDict["constitType"] == "pf" else "Clusters"
            ],
            JetCalibIncludesAreaSub=calibHasAreaSub,
            JetEventShapeName=rhoKey,
            NSigma=self.n_sigma,
        )

class NNHLTConfig(AlgConfig):

    @classmethod
    def algType(cls):
        return "nn"

    def __init__(self, **recoDict):
        self.file_name = "TrigEFMissingET/20220429/NNsingleLayerRed.json"
        # Locate the file on the calib path
        full_name = find_datafile(self.file_name, os.environ["CALIBPATH"].split(os.pathsep))
        if full_name is None:
            raise FileNotFoundError(errno.ENOENT, "File not found on CALIBPATH", self.file_name)
        with open(full_name, 'r') as fp:
            network = json.load(fp)
        # Read the names of the algorithms used from the network file. The network file contains
        # the container+aux read from it, e.g. HLT_MET_cell.met so we strip off the HLT_MET_ prefix
        # and the .XX suffix
        self.inputs = {
                dct2["name"].removeprefix("HLT_MET_").partition(".")[0]
                for dct in network["inputs"]
                for dct2 in dct["variables"]
        }
        super().__init__(inputs=self.inputs, inputRegistry=HLTInputConfigRegistry(), **recoDict)

    def make_fex_accumulator(self, flags, name, inputs):
        return CompFactory.getComp("HLT::MET::NNHLTFex")(
            name,
            TriggerMETs=[inputs[k] for k in self.inputs],
            InputFileName=self.file_name,
        )

