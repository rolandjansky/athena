#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
from AthenaCommon.CFElements import seqAND
from TrigEDMConfig.TriggerEDMRun3 import recordable
from ..Menu.MenuComponents import RecoFragmentsPool

from TrigEFMissingET.TrigEFMissingETMTConfig import getMETMonTool

from TrigT2CaloCommon.CaloDef import clusterFSInputMaker
import GaudiKernel.SystemOfUnits as Units

from .ConfigHelpers import metRecoDictToString, jetRecoDictForMET

import abc

from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)

class AlgConfig(object):
    """ Base class to describe algorithm configurations

    Each individual 'EFrecoAlg' should be described by *one* AlgConfig subclass.
    That subclass must set two data members from its __init__ method:

    - self.inputs should be a *list* of all sequences required to make inputs
      for the algorithm that calculates the MET value
    - self.metAlg should the FEX that calculates the MET value

    The name of metAlg *must* be self.algName and the METContainerKey property
    *must* be set to self.outputKey

    The subclass must also implement the @classmethod 'algType' which returns
    the EFrecoAlg string that it describes.
    """
    __metaclass__ = abc.ABCMeta

    @classmethod
    def algType(cls):
        """ The algorithm that this object configures - this corresponds to the
        EFrecoAlg in the METChainParts dictionary

        Note that no two subclasses of AlgConfig can describe the same algorithm
        (as identified by this string).
        """
        raise NotImplementedError("algType not implemented by subclass!")

    def __init__(self, **recoDict):
        self._suffix = metRecoDictToString(recoDict)

    @property
    def outputKey(self):
        """ The MET container object produced by this algorithm """
        return recordable("HLT_MET_{}".format(self._suffix) )

    @property
    def algName(self):
        """ The name of the algorithm made by this configuration """
        return "EFMET_{}".format(self._suffix)

    def buildSequence(self):
        """ Create the full reco sequence """
        # First, verify that everything was done right
        assert self.metAlg.name() == self.algName, (
            "Incorrect algorithm name '{}' set (should be '{}')".format(
                self.metAlg.name(), self.algName) )
        assert self.metAlg.METContainerKey == self.outputKey, (
            "Incorrect output key '{}' set (should be '{}')".format(
                self.metAlg.METContainerKey, self.outputKey) )

        seq = seqAND("METRecoSequence_{}".format(self._suffix),
                     self.inputs + [self.metAlg])
        return seq

    @classmethod
    def _get_subclasses(cls):
        """ Provides a way to iterate over all subclasses of this class """
        for subcls in cls.__subclasses__():
            for subsubcls in subcls.__subclasses__():
                yield subsubcls
            yield subcls

    @classmethod
    def fromRecoDict(cls, EFrecoAlg, **recoDict):
        for subcls in cls._get_subclasses():
            if subcls.algType() == EFrecoAlg:
                return subcls(EFrecoAlg=EFrecoAlg, **recoDict)

        raise ValueError("Unknown EFrecoAlg '{}' requested".format(EFrecoAlg) )
                    

def metAthSequence(dummyFlags, **recoDict):
    inputSeq = clusterFSInputMaker()
    recoSeq, sequenceOut = RecoFragmentsPool.retrieve(
            metRecoSequence, None, **recoDict)

    seq = seqAND("METAthSeq_{}".format(metRecoDictToString(recoDict) ),
                 [inputSeq, recoSeq])
    return seq, inputSeq, sequenceOut

def metRecoSequence(dummyFlags, **recoDict):
    conf = AlgConfig.fromRecoDict(**recoDict)
    return conf.buildSequence(), conf.outputKey

class CellConfig(AlgConfig):
    @classmethod
    def algType(cls):
        return "cell"
    
    def __init__(self, **recoDict):
        super(CellConfig, self).__init__(**recoDict)
        from TrigT2CaloCommon.CaloDef import HLTFSCellMakerRecoSequence
        from TrigEFMissingET.TrigEFMissingETConf import HLT__MET__CellFex
        (cellMakerSeq, cellName) = HLTFSCellMakerRecoSequence()

        self.inputs = [cellMakerSeq]
        self.metAlg = HLT__MET__CellFex(
                name            = self.algName,
                CellName        = cellName,
                METContainerKey = self.outputKey,
                MonTool         = getMETMonTool() )

class TCConfig(AlgConfig):
    @classmethod
    def algType(cls):
        return "tc"

    def __init__(self, **recoDict):
        super(TCConfig, self).__init__(**recoDict)
        from TrigT2CaloCommon.CaloDef import HLTFSTopoRecoSequence
        from TrigEFMissingET.TrigEFMissingETConf import HLT__MET__TCFex
        # TODO - cluster calibration
        tcSeq, clusterName = RecoFragmentsPool.retrieve(
                HLTFSTopoRecoSequence, "FSJETRoI")

        self.inputs = [tcSeq]
        self.metAlg = HLT__MET__TCFex(
                name            = self.algName,
                ClusterName     = clusterName,
                METContainerKey = self.outputKey,
                MonTool = getMETMonTool() )

class TCPufitConfig(AlgConfig):
    @classmethod
    def algType(cls):
        return "tcpufit"

    def __init__(self, **recoDict):
        super(TCPufitConfig, self).__init__(**recoDict)
        from TrigT2CaloCommon.CaloDef import HLTFSTopoRecoSequence
        from TrigEFMissingET.TrigEFMissingETConf import HLT__MET__TCPufitFex
        tcSeq, clusterName = RecoFragmentsPool.retrieve(
                HLTFSTopoRecoSequence, "FSJETRoI")

        self.inputs = [tcSeq]
        self.metAlg = HLT__MET__TCPufitFex(
                name            = self.algName,
                ClusterName     = clusterName,
                METContainerKey = self.outputKey,
                MonTool = getMETMonTool() )

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
        self.metAlg = HLT__MET__MHTFex(
                name            = self.algName,
                JetName         = jetName,
                METContainerKey = self.outputKey,
                MonTool         = getMETMonTool() )

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
        tracks = "HLT_xAODTracks_FS"
        vertices = "HLT_EFHistoPrmVtx"
        tva = "JetTrackVtxAssoc_{trkopt}".format(**jetRecoDict)
        track_links = "GhostTrack_{trkopt}".format(**jetRecoDict)

        self.inputs = [jetSeq]
        self.metAlg = HLT__MET__TrkMHTFex(
                name            = self.algName,
                METContainerKey = self.outputKey,
                MonTool         = getMETMonTool(),
                JetName         = jetName,
                TrackName       = tracks,
                VertexName      = vertices,
                TVAName         = tva,
                TrackLinkName   = track_links)
        self.metAlg.TrackSelTool.CutLevel         = "Loose"
        self.metAlg.TrackSelTool.maxZ0SinTheta    = 1.5
        self.metAlg.TrackSelTool.maxD0overSigmaD0 = 3
        self.metAlg.TrackSelTool.minPt            = 1 * Units.GeV
