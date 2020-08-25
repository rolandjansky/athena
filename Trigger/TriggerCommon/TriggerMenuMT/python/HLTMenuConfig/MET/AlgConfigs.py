#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from .ConfigHelpers import AlgConfig, jetRecoDictForMET
from ..Menu.MenuComponents import RecoFragmentsPool
from ..Menu.SignatureDicts import METChainParts
import GaudiKernel.SystemOfUnits as Units

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
             "{}".format(unknown_algs) )
    

class CellConfig(AlgConfig):
    @classmethod
    def algType(cls):
        return "cell"
    
    def __init__(self, **recoDict):
        super(CellConfig, self).__init__(**recoDict)
        from TriggerMenuMT.HLTMenuConfig.CommonSequences.CaloSequenceSetup import cellRecoSequence
        from TrigEFMissingET.TrigEFMissingETConf import HLT__MET__CellFex
        cellMakerSeq, cellName = RecoFragmentsPool.retrieve(
                cellRecoSequence, flags=None, RoIs=self.inputMaker.RoIs)

        self.inputs = [cellMakerSeq]
        self.fexAlg = self._make_fex_alg(
                HLT__MET__CellFex,
                CellName = cellName)

class TCConfig(AlgConfig):
    @classmethod
    def algType(cls):
        return "tc"

    def __init__(self, calib, **recoDict):
        super(TCConfig, self).__init__(calib=calib, **recoDict)
        from TriggerMenuMT.HLTMenuConfig.CommonSequences.CaloSequenceSetup import (
                caloClusterRecoSequence, LCCaloClusterRecoSequence)
        from TrigEFMissingET.TrigEFMissingETConf import HLT__MET__TCFex
        RoIs = self.inputMaker.RoIs
        if calib == "em": 
            tcSeq, clusterName = RecoFragmentsPool.retrieve(
                    caloClusterRecoSequence, flags = None, RoIs=RoIs)
        elif calib == "lcw":
            tcSeq, clusterName = RecoFragmentsPool.retrieve(
                    LCCaloClusterRecoSequence, flag = None, RoIs=RoIs)

        self.inputs = [tcSeq]
        self.fexAlg = self._make_fex_alg(
                HLT__MET__TCFex,
                ClusterName = clusterName)

class TCPufitConfig(AlgConfig):
    @classmethod
    def algType(cls):
        return "tcpufit"

    def __init__(self, calib, **recoDict):
        super(TCPufitConfig, self).__init__(calib=calib, **recoDict)
        from TriggerMenuMT.HLTMenuConfig.CommonSequences.CaloSequenceSetup import (
                caloClusterRecoSequence, LCCaloClusterRecoSequence)
        from TrigEFMissingET.TrigEFMissingETConf import HLT__MET__TCPufitFex
        RoIs = self.inputMaker.RoIs
        if calib == "em": 
            tcSeq, clusterName = RecoFragmentsPool.retrieve(
                    caloClusterRecoSequence, flags=None, RoIs=RoIs)
        elif calib == "lcw":
            tcSeq, clusterName = RecoFragmentsPool.retrieve(
                    LCCaloClusterRecoSequence, flags=None, RoIs=RoIs)

        self.inputs = [tcSeq]
        self.fexAlg = self._make_fex_alg(
                HLT__MET__TCPufitFex,
                ClusterName = clusterName)

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
                jetRecoSequence, None, dataSource="data", **jetRecoDict)

        self.inputs = [jetSeq]
        self.fexAlg = self._make_fex_alg(
                HLT__MET__MHTFex,
                JetName = jetName)

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
                jetRecoSequence, None, dataSource="data", **jetRecoDict)

        # These are the names set by the upstream algorithms. Unfortunately
        # these aren't passed to us - we just have to 'know' them
        tracks = "HLT_IDTrack_FS_FTF"
        vertices = "HLT_IDVertex_FS"
        tva = "JetTrackVtxAssoc_{trkopt}".format(**jetRecoDict)
        track_links = "GhostTrack_{trkopt}".format(**jetRecoDict)

        self.inputs = [jetSeq]
        self.fexAlg = self._make_fex_alg(
                HLT__MET__TrkMHTFex,
                JetName       = jetName,
                TrackName     = tracks,
                VertexName    = vertices,
                TVAName       = tva,
                TrackLinkName = track_links)
        self.fexAlg.TrackSelTool.CutLevel         = "Loose"
        self.fexAlg.TrackSelTool.maxZ0SinTheta    = 1.5
        self.fexAlg.TrackSelTool.maxD0overSigmaD0 = 3
        self.fexAlg.TrackSelTool.minPt            = 1 * Units.GeV

class PFSumConfig(AlgConfig):
    @classmethod
    def algType(cls):
        return "pfsum"

    def __init__(self, **recoDict):
        super(PFSumConfig, self).__init__(**recoDict)

        from TriggerMenuMT.HLTMenuConfig.CommonSequences.CaloSequenceSetup import (
                caloClusterRecoSequence)
        from eflowRec.PFHLTSequence import PFHLTSequence
        from ..Jet.JetRecoConfiguration import defineJetConstit
        from TrigEFMissingET.TrigEFMissingETConf import HLT__MET__PFSumFex
        jetRecoDict = jetRecoDictForMET(trkopt="ftf", **recoDict)
        jetRecoDict["calib"] = "em"
        jetRecoDict["dataType"] = "pf"

        RoIs = self.inputMaker.RoIs
        tcSeq, clusterName = RecoFragmentsPool.retrieve(
                caloClusterRecoSequence, flags=None, RoIs=RoIs)
        pfseq, pfoPrefix = RecoFragmentsPool.retrieve(
                PFHLTSequence, None, clustersin = clusterName, tracktype="ftf")
        constit = defineJetConstit(jetRecoDict, pfoPrefix=pfoPrefix)
        from JetRecConfig.ConstModHelpers import getConstitModAlg
        constit_mod_seq = getConstitModAlg(
                constit, "HLT",
                tvaKey="JetTrackVtxAssoc_{trkopt}".format(**jetRecoDict),
                vtxKey="HLT_IDVertex_FS")
        self.inputs = [tcSeq, pfseq, constit_mod_seq]
        self.fexAlg = self._make_fex_alg(
                HLT__MET__PFSumFex,
                NeutralPFOName = pfoPrefix+"CHSNeutralParticleFlowObjects",
                ChargedPFOName = pfoPrefix+"CHSChargedParticleFlowObjects")
