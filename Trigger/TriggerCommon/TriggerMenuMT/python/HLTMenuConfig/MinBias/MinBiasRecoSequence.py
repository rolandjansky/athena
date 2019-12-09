#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
from AthenaCommon.CFElements import seqAND
from TrigT2CaloCommon.CaloDef import clusterFSInputMaker

def minbiasSpacePointAthSequence(ConfigFlags):
    InputMakerAlg= clusterFSInputMaker()
    (recoSequence, sequenceOut) = MinBiasRecoSequences()

    minbiasSpacePointAthSequence =  seqAND("minbiasSpacePointAthSequence",[InputMakerAlg, recoSequence ])
    return (minbiasSpacePointAthSequence, InputMakerAlg, sequenceOut)

    def minbiasTrackAthSequence(ConfigFlags):
        InputMakerAlg= clusterFSInputMaker()
        (recoSequence, sequenceOut) = MinBiasRecoSequences()

        minbiasTrackAthSequence =  seqAND("minbiasTrackAthSequence",[InputMakerAlg, recoSequence ])
        return (minbiasTrackAthSequence, InputMakerAlg, sequenceOut)


def MinBiasRecoSequences():

    from TrigT2CaloCommon.CaloDef import HLTFSMinBiasRecoSequences
    (cellMakerSeq, CellsName) = HLTFSMinBiasRecoSequences()

    #################################################
    # Add SPCountHypoAlg and associated tools
    #################################################
