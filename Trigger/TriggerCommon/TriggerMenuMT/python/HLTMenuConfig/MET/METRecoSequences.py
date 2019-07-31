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
    (metCellRecoSequence, CellsName) = HLTFSCellMakerRecoSequence()

    #################################################
    # Add EFMissingETAlg and associated tools
    #################################################
    metAlg = EFMissingETAlgMT( name="EFMET_cell" )
    flagsTool = EFMissingETFlagsMT("theFlagsTool")
    metAlg.METContainerKey = recordable("HLT_MET")
    metAlg.MonTool = getMETMonTool()

    #///////////////////////////////////////////
    # Add EFMissingETFromCells tool
    #///////////////////////////////////////////
    from TrigEFMissingET.TrigEFMissingETConf import EFMissingETFromCellsMT
    cellTool = EFMissingETFromCellsMT( name="METFromCellsTool" )
    cellTool.CellsCollection = CellsName
    metAlg.METTools = [cellTool, flagsTool]

    metCellRecoSequence += metAlg

    seqOut = metAlg.METContainerKey
    return (metCellRecoSequence, seqOut)


def metClusterAthSequence(ConfigFlags):
    InputMakerAlg= clusterFSInputMaker()
    (recoSequence, sequenceOut) = metClusterRecoSequence()

    MetClusterAthSequence =  seqAND("MetClusterAthSequence",[InputMakerAlg, recoSequence ])
    return (MetClusterAthSequence, InputMakerAlg, sequenceOut)

    
def metClusterRecoSequence():

    from TrigT2CaloCommon.CaloDef import HLTFSTopoRecoSequence
    (metClusterRecoSequence, ClustersName) = HLTFSTopoRecoSequence()


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

    ### WARNING: this setting does not work for the scheduler: the variable is set, but the scheduler retrieves the default one
    clusterTool.ClustersCollection = ClustersName

    metAlg.METTools = [clusterTool]

    metClusterRecoSequence += metAlg

    seqOut = metAlg.METContainerKey
    return (metClusterRecoSequence, seqOut)


def metClusterPufitAthSequence(ConfigFlags):
    InputMakerAlg= clusterFSInputMaker()
    (recoSequence, sequenceOut) = metClusterPufitRecoSequence()

    MetClusterPufitAthSequence =  seqAND("MetClusterPufitAthSequence",[InputMakerAlg, recoSequence ])
    return (MetClusterPufitAthSequence, InputMakerAlg, sequenceOut)

    
def metClusterPufitRecoSequence(RoIs = 'FSJETRoI'):

    from TrigT2CaloCommon.CaloDef import HLTFSTopoRecoSequence

    (metClusterPufitRecoSequence, ClustersName) = RecoFragmentsPool.retrieve(HLTFSTopoRecoSequence, RoIs)

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

    metClusterPufitRecoSequence += metAlg

    seqOut = metAlg.METContainerKey
    return (metClusterPufitRecoSequence, seqOut)


def metJetAthSequence(ConfigFlags):
    InputMakerAlg= clusterFSInputMaker()
    (recoSequence, sequenceOut) = metJetRecoSequence()

    MetAthSequence =  seqAND("MetJetAthSequence",[InputMakerAlg, recoSequence ])
    return (MetAthSequence, InputMakerAlg, sequenceOut)


def metJetRecoSequence(RoIs = 'FSJetRoI'):

    from TrigUpgradeTest.jetDefs import jetRecoSequence
    (recoSequence, JetsName) = RecoFragmentsPool.retrieve(jetRecoSequence, RoIs)


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

    mhtTool.JetsCollection=JetsName
    
    metAlg.METTools = [mhtTool]

    recoSequence += metAlg

    seqOut = metAlg.METContainerKey
    return (recoSequence, seqOut)

