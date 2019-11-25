#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
from AthenaCommon.CFElements import seqAND
from TrigEDMConfig.TriggerEDMRun3 import recordable
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import RecoFragmentsPool

from TrigEFMissingET.TrigEFMissingETConf import (
        EFMissingETAlgMT, EFMissingETFlagsMT, HLT__MET__TrkMHTFex,
        HLT__MET__CellFex, HLT__MET__MHTFex)
from TrigEFMissingET.TrigEFMissingETMTConfig import getMETMonTool

from TrigT2CaloCommon.CaloDef import clusterFSInputMaker
import GaudiKernel.SystemOfUnits as Units
from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)

def metCellAthSequence(ConfigFlags):
    InputMakerAlg= clusterFSInputMaker()
    (recoSequence, sequenceOut) = metCellRecoSequence()

    MetAthSequence =  seqAND("MetCellAthSequence",[InputMakerAlg, recoSequence ])
    return (MetAthSequence, InputMakerAlg, sequenceOut)

    
def metCellRecoSequence():

    from TrigT2CaloCommon.CaloDef import HLTFSCellMakerRecoSequence
    (cellMakerSeq, CellsName) = HLTFSCellMakerRecoSequence()
    alg = HLT__MET__CellFex(
            name="EFMET_cell",
            CellName = CellsName,
            METContainerKey = recordable("HLT_MET_cell"),
            MonTool = getMETMonTool() )

    met_recoSequence = seqAND("metCellRecoSequence", [cellMakerSeq, alg])
    seqOut = alg.METContainerKey
    return (met_recoSequence, seqOut)


def metClusterAthSequence(ConfigFlags):
    InputMakerAlg= clusterFSInputMaker()
    (recoSequence, sequenceOut) = metClusterRecoSequence()

    MetClusterAthSequence =  seqAND("MetClusterAthSequence",[InputMakerAlg, recoSequence ])
    return (MetClusterAthSequence, InputMakerAlg, sequenceOut)

    
def metClusterRecoSequence():

    from TrigT2CaloCommon.CaloDef import HLTFSTopoRecoSequence
    (tcSeq, ClustersName) = HLTFSTopoRecoSequence()

    #################################################
    # Add EFMissingETAlg and associated tools
    #################################################
    metAlg = EFMissingETAlgMT( name="EFMET_tc" )
    metAlg.METContainerKey = recordable("HLT_MET_tc")
    metAlg.MonTool = getMETMonTool()

    #///////////////////////////////////////////
    # Add EFMissingETFromClusters tool
    #///////////////////////////////////////////
    from TrigEFMissingET.TrigEFMissingETConf import EFMissingETFromClustersMT
    clusterTool = EFMissingETFromClustersMT( name="METFromClustersTool" )
    clusterTool.ClustersCollection = ClustersName

    metAlg.METTools = [clusterTool]

    metClusterRecoSequence = seqAND("metClusterRecoSequence", [tcSeq, metAlg])

    seqOut = metAlg.METContainerKey
    return (metClusterRecoSequence, seqOut)


def metClusterPufitAthSequence(ConfigFlags):
    InputMakerAlg= clusterFSInputMaker()
    (recoSequence, sequenceOut) = metClusterPufitRecoSequence()

    MetClusterPufitAthSequence =  seqAND("MetClusterPufitAthSequence",[InputMakerAlg, recoSequence ])
    return (MetClusterPufitAthSequence, InputMakerAlg, sequenceOut)

    
def metClusterPufitRecoSequence(RoIs = 'FSJETRoI'):

    from TrigT2CaloCommon.CaloDef import HLTFSTopoRecoSequence
    (tcSeq, ClustersName) = RecoFragmentsPool.retrieve(HLTFSTopoRecoSequence, RoIs)

    #################################################
    # Add EFMissingETAlg and associated tools
    #################################################
    metAlg = EFMissingETAlgMT( name="EFMET_tcPufit" )
    metAlg.METContainerKey = recordable("HLT_MET_tcPufit")
    metAlg.MonTool = getMETMonTool()
    
        #///////////////////////////////////////////
        # Add EFMissingETFromClustersPufit tool
        #///////////////////////////////////////////
    from TrigEFMissingET.TrigEFMissingETConf import EFMissingETFromClustersPufitMT
    clusterPufitTool = EFMissingETFromClustersPufitMT( name="METFromClustersPufitTool" )
    clusterPufitTool.ClustersCollection = ClustersName

    metAlg.METTools = [clusterPufitTool]

    metClusterPufitRecoSequence = seqAND("metClusterPufitRecoSequence", [tcSeq ,metAlg])

    seqOut = metAlg.METContainerKey
    return (metClusterPufitRecoSequence, seqOut)


def metJetAthSequence(ConfigFlags):
    InputMakerAlg= clusterFSInputMaker()
    (recoSequence, sequenceOut) = metJetRecoSequence()

    MetAthSequence =  seqAND("MetJetAthSequence",[InputMakerAlg, recoSequence ])
    return (MetAthSequence, InputMakerAlg, sequenceOut)


def metJetRecoSequence(RoIs = 'FSJETRoI'):

    from TriggerMenuMT.HLTMenuConfig.Jet.JetRecoSequences import jetRecoSequence
    jetRecoDict={"recoAlg":"a4", "dataType": "tc", "calib": "em", "jetCalib": "subjes", "trkopt": "notrk"}
    (jetSeq, sequenceOut) = RecoFragmentsPool.retrieve( jetRecoSequence, None, dataSource="data", **jetRecoDict )

    alg = HLT__MET__MHTFex(
            name="EFMET_mht",
            JetName = sequenceOut,
            METContainerKey = recordable("HLT_MET_mht"),
            MonTool = getMETMonTool() )

    metJetRecoSequence = seqAND("metJetRecoSequence", [jetSeq, alg])

    seqOut = alg.METContainerKey
    return (metJetRecoSequence, seqOut)

def metTrkMHTAthSequence(ConfigFlags):
    InputMakerAlg = clusterFSInputMaker()
    reco_seq, seq_out = metTrkMHTRecoSequence()
    ath_seq = seqAND("MetTrkMHTAthSequence", [InputMakerAlg, reco_seq])
    return ath_seq, InputMakerAlg, seq_out

def metTrkMHTRecoSequence():

    # Prepare the inputs from the jet slice
    from TriggerMenuMT.HLTMenuConfig.Jet.JetRecoSequences import jetRecoSequence
    jetRecoDict={"recoAlg":"a4", "dataType": "tc", "calib": "em", "jetCalib": "subjesIS", "trkopt": "ftf", "cleaning": "noCleaning"}
    (jetSeq, jetOut) = RecoFragmentsPool.retrieve( jetRecoSequence, None, dataSource="data", **jetRecoDict )

    # These are the names set by the downstream algorithms. Unfortunately these
    # aren't passed to us - we just have to 'know' them
    tracks = "HLT_xAODTracks_FS"
    vertices = "HLT_EFHistoPrmVtx"
    tva = "JetTrackVtxAssoc_{trkopt}".format(**jetRecoDict)
    track_links = "GhostTrack_{trkopt}".format(**jetRecoDict)

    alg = HLT__MET__TrkMHTFex(
            name="EFMET_trkmht",
            METContainerKey = recordable("HLT_MET_trkmht"),
            MonTool = getMETMonTool(),
            JetName = jetOut,
            TrackName = tracks,
            VertexName = vertices,
            TVAName = tva,
            TrackLinkName = track_links)
    alg.TrackSelTool.CutLevel = "Loose"
    alg.TrackSelTool.maxZ0SinTheta = 1.5
    alg.TrackSelTool.maxD0overSigmaD0 = 3
    alg.TrackSelTool.minPt = 1 * Units.GeV

    reco_seq = seqAND("metTrkMHTRecoSequence", [jetSeq, alg])
    return (reco_seq, alg.METContainerKey)

