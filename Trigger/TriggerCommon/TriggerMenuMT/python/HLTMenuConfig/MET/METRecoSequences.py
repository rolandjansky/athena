#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
from AthenaCommon.CFElements import seqAND
from TrigEDMConfig.TriggerEDMRun3 import recordable
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import RecoFragmentsPool

from TrigEFMissingET.TrigEFMissingETConf import EFMissingETAlgMT, EFMissingETFlagsMT
from TrigEFMissingET.TrigEFMissingETMTConfig import getMETMonTool

from TrigT2CaloCommon.CaloDef import clusterFSInputMaker

def metCellAthSequence(ConfigFlags):
    InputMakerAlg= clusterFSInputMaker()
    (recoSequence, sequenceOut) = metCellRecoSequence()

    MetAthSequence =  seqAND("MetCellAthSequence",[InputMakerAlg, recoSequence ])
    return (MetAthSequence, InputMakerAlg, sequenceOut)

    
def metCellRecoSequence():

    from TrigT2CaloCommon.CaloDef import HLTFSCellMakerRecoSequence
    (cellMakerSeq, CellsName) = HLTFSCellMakerRecoSequence()

    #################################################
    # Add EFMissingETAlg and associated tools
    #################################################
    metAlg = EFMissingETAlgMT( name="EFMET_cell" )
    flagsTool = EFMissingETFlagsMT("theFlagsTool")
    metAlg.METContainerKey = recordable("HLT_MET_cell")
    metAlg.MonTool = getMETMonTool()

    #///////////////////////////////////////////
    # Add EFMissingETFromCells tool
    #///////////////////////////////////////////
    from TrigEFMissingET.TrigEFMissingETConf import EFMissingETFromCellsMT
    cellTool = EFMissingETFromCellsMT( name="METFromCellsTool" )
    cellTool.CellsCollection = CellsName
    metAlg.METTools = [cellTool, flagsTool]

    met_recoSequence = seqAND("metCellRecoSequence", [cellMakerSeq, metAlg])

    seqOut = metAlg.METContainerKey
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


    #################################################
    # Add EFMissingETAlg and associated tools
    #################################################
    metAlg = EFMissingETAlgMT( name="EFMET_mht" )
    metAlg.METContainerKey = recordable("HLT_MET_mht")
    metAlg.MonTool = getMETMonTool()

    #///////////////////////////////////////////
    # Add EFMissingETFromCells tool
    #///////////////////////////////////////////
    from TrigEFMissingET.TrigEFMissingETConf import EFMissingETFromJetsMT
    mhtTool = EFMissingETFromJetsMT( name="METFromJetsTool" )

    mhtTool.JetsCollection=sequenceOut
    
    metAlg.METTools = [mhtTool]

    metJetRecoSequence = seqAND("metJetRecoSequence", [jetSeq, metAlg])

    seqOut = metAlg.METContainerKey
    return (metJetRecoSequence, seqOut)

