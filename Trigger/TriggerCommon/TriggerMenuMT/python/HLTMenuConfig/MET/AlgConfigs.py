#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from .ConfigHelpers import AlgConfig, jetRecoDictForMET
from ..Menu.MenuComponents import RecoFragmentsPool
from ..Menu.SignatureDicts import METChainParts
import GaudiKernel.SystemOfUnits as Units
import TrigEFMissingET.PUClassification as PUClassification

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
        super(CellConfig, self).__init__(**recoDict)
        from TriggerMenuMT.HLTMenuConfig.CommonSequences.CaloSequenceSetup import (
            cellRecoSequence,
        )
        from TrigEFMissingET.TrigEFMissingETConf import HLT__MET__CellFex

        cellMakerSeq, cellName = RecoFragmentsPool.retrieve(
            cellRecoSequence, flags=None, RoIs=self.inputMaker.RoIs
        )

        self.inputs = [cellMakerSeq]
        self.fexAlg = self._make_fex_alg(HLT__MET__CellFex, CellName=cellName)


class TCConfig(AlgConfig):
    @classmethod
    def algType(cls):
        return "tc"

    def __init__(self, calib, **recoDict):
        super(TCConfig, self).__init__(calib=calib, **recoDict)
        from TriggerMenuMT.HLTMenuConfig.CommonSequences.CaloSequenceSetup import (
            caloClusterRecoSequence,
            LCCaloClusterRecoSequence,
        )
        from TrigEFMissingET.TrigEFMissingETConf import HLT__MET__TCFex

        RoIs = self.inputMaker.RoIs
        if calib == "em":
            tcSeq, clusterName = RecoFragmentsPool.retrieve(
                caloClusterRecoSequence, flags=None, RoIs=RoIs
            )
        elif calib == "lcw":
            tcSeq, clusterName = RecoFragmentsPool.retrieve(
                LCCaloClusterRecoSequence, flag=None, RoIs=RoIs
            )

        self.inputs = [tcSeq]
        self.fexAlg = self._make_fex_alg(HLT__MET__TCFex, ClusterName=clusterName)


class TCPufitConfig(AlgConfig):
    @classmethod
    def algType(cls):
        return "tcpufit"

    def __init__(self, calib, **recoDict):
        super(TCPufitConfig, self).__init__(calib=calib, **recoDict)
        from TriggerMenuMT.HLTMenuConfig.CommonSequences.CaloSequenceSetup import (
            caloClusterRecoSequence,
            LCCaloClusterRecoSequence,
        )
        from TrigEFMissingET.TrigEFMissingETConf import HLT__MET__TCPufitFex

        RoIs = self.inputMaker.RoIs
        if calib == "em":
            tcSeq, clusterName = RecoFragmentsPool.retrieve(
                caloClusterRecoSequence, flags=None, RoIs=RoIs
            )
        elif calib == "lcw":
            tcSeq, clusterName = RecoFragmentsPool.retrieve(
                LCCaloClusterRecoSequence, flags=None, RoIs=RoIs
            )

        self.inputs = [tcSeq]
        self.fexAlg = self._make_fex_alg(HLT__MET__TCPufitFex, ClusterName=clusterName)


class MHTConfig(AlgConfig):
    @classmethod
    def algType(cls):
        return "mht"

    def __init__(self, **recoDict):
        super(MHTConfig, self).__init__(**recoDict)
        from ..Jet.JetRecoSequences import jetRecoSequence
        from TrigEFMissingET.TrigEFMissingETConf import HLT__MET__MHTFex

        jetRecoDict = jetRecoDictForMET(**recoDict)
        # TODO - right now jet calibration is hardcoded to EM
        jetRecoDict["calib"] = "em"
        jetRecoDict["jetCalib"] = "subjes"
        jetSeq, jetName = RecoFragmentsPool.retrieve(
            jetRecoSequence, None, dataSource="data", **jetRecoDict
        )

        self.inputs = [jetSeq]
        self.fexAlg = self._make_fex_alg(HLT__MET__MHTFex, JetName=jetName)


class TrkMHTConfig(AlgConfig):
    @classmethod
    def algType(cls):
        return "trkmht"

    def __init__(self, **recoDict):
        super(TrkMHTConfig, self).__init__(**recoDict)
        from ..Jet.JetRecoSequences import jetRecoSequence
        from TrigEFMissingET.TrigEFMissingETConf import HLT__MET__TrkMHTFex

        jetRecoDict = jetRecoDictForMET(trkopt="ftf", **recoDict)
        # TODO - right now jet calibration is hardcoded to EM
        jetRecoDict["calib"] = "em"
        jetSeq, jetName = RecoFragmentsPool.retrieve(
            jetRecoSequence, None, dataSource="data", **jetRecoDict
        )

        # These are the names set by the upstream algorithms. Unfortunately
        # these aren't passed to us - we just have to 'know' them
        tracks = "HLT_IDTrack_FS_FTF"
        vertices = "HLT_IDVertex_FS"
        tva = "JetTrackVtxAssoc_{trkopt}".format(**jetRecoDict)
        track_links = "GhostTrack_{trkopt}".format(**jetRecoDict)

        self.inputs = [jetSeq]
        self.fexAlg = self._make_fex_alg(
            HLT__MET__TrkMHTFex,
            JetName=jetName,
            TrackName=tracks,
            VertexName=vertices,
            TVAName=tva,
            TrackLinkName=track_links,
        )
        self.fexAlg.TrackSelTool.CutLevel = "Loose"
        self.fexAlg.TrackSelTool.maxZ0SinTheta = 1.5
        self.fexAlg.TrackSelTool.maxD0overSigmaD0 = 3
        self.fexAlg.TrackSelTool.minPt = 1 * Units.GeV


class PFSumConfig(AlgConfig):
    @classmethod
    def algType(cls):
        return "pfsum"

    def __init__(self, **recoDict):
        super(PFSumConfig, self).__init__(**recoDict)

        from TrigEFMissingET.TrigEFMissingETConf import HLT__MET__PFSumFex
        from .METRecoSequences import pfoRecoSequence

        self.inputs, pfoPrefix = RecoFragmentsPool.retrieve(
            pfoRecoSequence, flags=None, RoIs=self.inputMaker.RoIs
        )
        self.fexAlg = self._make_fex_alg(
            HLT__MET__PFSumFex,
            NeutralPFOName=pfoPrefix + "CHSNeutralParticleFlowObjects",
            ChargedPFOName=pfoPrefix + "CHSChargedParticleFlowObjects",
        )


class PFOPufitConfig(AlgConfig):
    @classmethod
    def algType(cls):
        return "pfopufit"

    def __init__(self, **recoDict):
        super(PFOPufitConfig, self).__init__(**recoDict)

        from TrigEFMissingET.TrigEFMissingETConf import (
            HLT__MET__PUSplitPufitFex,
            HLT__MET__PFOPrepAlg,
        )
        from .METRecoSequences import pfoRecoSequence

        pfoInputs, pfoPrefix = RecoFragmentsPool.retrieve(
            pfoRecoSequence, flags=None, RoIs=self.inputMaker.RoIs
        )
        # NB for this, we might be slightly misusing the 'flags' parameter in
        # the reco fragments pool. Here, we let it just pass the name parameter
        # through to the underlying alg config class parameter
        prepAlg = RecoFragmentsPool.retrieve(
            HLT__MET__PFOPrepAlg,
            f"{pfoPrefix}PFOPufitPrepAlg",
            InputNeutralKey=f"{pfoPrefix}CHSNeutralParticleFlowObjects",
            InputChargedKey=f"{pfoPrefix}CHSChargedParticleFlowObjects",
            OutputKey=f"{pfoPrefix}METTrigCombinedParticleFlowObjects",
            OutputCategoryKey="PUClassification",
        )
        self.inputs = pfoInputs + [prepAlg]
        # TODO - make the neutral threshold mode settable in the chain name?
        self.fexAlg = self._make_fex_alg(
            HLT__MET__PUSplitPufitFex,
            InputName=prepAlg.OutputKey,
            InputCategoryName=prepAlg.OutputCategoryKey,
            NeutralThresholdMode=PUClassification.NeutralForward,
        )


class CVFPufitConfig(AlgConfig):
    @classmethod
    def algType(cls):
        return "cvfpufit"

    def __init__(self, **recoDict):
        super().__init__(**recoDict)
        from .METRecoSequences import cvfClusterSequence
        from TrigEFMissingET.TrigEFMissingETConf import (
            HLT__MET__CVFPrepAlg,
            HLT__MET__PUSplitPufitFex,
        )

        RoIs = self.inputMaker.RoIs
        calib = recoDict["calib"]
        inputs, clusterName, cvfName = RecoFragmentsPool.retrieve(
            cvfClusterSequence, flags=None, RoIs=RoIs, **recoDict
        )

        prepAlg = RecoFragmentsPool.retrieve(
            HLT__MET__CVFPrepAlg,
            f"{calib}ClusterCVFPrepAlg",
            InputClusterKey=clusterName,
            InputCVFKey=cvfName,
            OutputCategoryKey="PUClassification",
        )

        self.inputs = inputs + [prepAlg]
        self.fexAlg = self._make_fex_alg(
            HLT__MET__PUSplitPufitFex,
            InputName=clusterName,
            InputCategoryName=prepAlg.OutputCategoryKey,
            NeutralThresholdMode=PUClassification.All,
        )


class MHTPufitConfig(AlgConfig):
    @classmethod
    def algType(cls):
        return "mhtpufit"

    def __init__(self, **recoDict):
        super().__init__(**recoDict)
        from ..Jet.JetRecoSequences import jetRecoSequence
        from ..Jet.JetRecoConfiguration import defineJets
        from TriggerMenuMT.HLTMenuConfig.CommonSequences.CaloSequenceSetup import (
            caloClusterRecoSequence,
        )
        from TrigEFMissingET.TrigEFMissingETConf import HLT__MET__MHTPufitFex

        jetRecoDict = jetRecoDictForMET(trkopt="ftf", **recoDict)
        # If this is PFlow then set the calib type to "em"
        if recoDict["jetDataType"] == "pf":
            jetRecoDict["calib"] = "em"
        jetSeq, jetName = RecoFragmentsPool.retrieve(
            jetRecoSequence, flags=None, dataSource="data", **jetRecoDict
        )

        # We need to get the input name that the jet sequence used
        _, clusterName = RecoFragmentsPool.retrieve(
            caloClusterRecoSequence, flags=None, RoIs=self.inputMaker.RoIs
        )
        if jetRecoDict["dataType"] == "pf":
            from eflowRec.PFHLTSequence import PFHLTSequence

            _, pfoPrefix = RecoFragmentsPool.retrieve(
                PFHLTSequence,
                flags=None,
                clustersin=clusterName,
                tracktype=jetRecoDict["trkopt"],
            )
            jetDef = defineJets(jetRecoDict, pfoPrefix=pfoPrefix)
        elif jetRecoDict["dataType"] == "tc":
            jetDef = defineJets(jetRecoDict, clustersKey=clusterName)
        else:
            raise ValueError(
                "Unexpected jetDataType {}".format(jetRecoDict["dataType"])
            )
        inputName = jetDef.inputdef.inputname
        calibHasAreaSub = "sub" in jetRecoDict["jetCalib"]
        if calibHasAreaSub:
            from JetRecConfig.JetRecConfig import getEventShapeAlg, getConstitPJGAlg

            evtShapeAlg = getEventShapeAlg(
                jetDef.inputdef,
                getConstitPJGAlg(jetDef.inputdef).OutputContainer,
                "HLT_",
            )
            rhoKey = evtShapeAlg.EventDensityTool.OutputContainer
        else:
            rhoKey = ""

        self.inputs = [jetSeq]
        self.fexAlg = self._make_fex_alg(
            HLT__MET__MHTPufitFex,
            InputJetsName=jetName,
            InputName=inputName,
            JetCalibIncludesAreaSub=calibHasAreaSub,
            JetEventShapeName=rhoKey,
        )

