#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
from AthenaCommon.CFElements import seqAND
from TrigEDMConfig.TriggerEDMRun3 import recordable

from TrigEFMissingET.TrigEFMissingETConf import EFMissingETAlgMT, EFMissingETFromHelperMT, EFMissingETFlagsMT
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
    metAlg = EFMissingETAlgMT( name="EFMET" )
    helperTool = EFMissingETFromHelperMT("theHelperTool")
    flagsTool = EFMissingETFlagsMT("theFlagsTool")
    metAlg.METContainerKey = recordable("HLT_MET")
    metAlg.MonTool = getMETMonTool()

    #///////////////////////////////////////////
    # Add EFMissingETFromCells tool
    #///////////////////////////////////////////
    from TrigEFMissingET.TrigEFMissingETConf import EFMissingETFromCellsMT
    cellTool = EFMissingETFromCellsMT( name="METFromCellsTool" )

    ### WARNING: this setting does not work for the scheduler: the variable is set, but the scheduler retrieves the default one
    cellTool.CellsCollection = CellsName

    
    metAlg.METTools.append(cellTool)
    metAlg.METTools.append(helperTool)
    metAlg.METTools.append(flagsTool)

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
    metAlg = EFMissingETAlgMT( name="EFMET" )
    helperTool = EFMissingETFromHelperMT("theHelperTool")
    metAlg.METContainerKey = "HLT_MET"
    metAlg.METContainerKey = recordable("HLT_MET")
    metAlg.MonTool = getMETMonTool()

    #///////////////////////////////////////////
    # Add EFMissingETFromClusters tool
    #///////////////////////////////////////////
    from TrigEFMissingET.TrigEFMissingETConf import EFMissingETFromClustersMT
    clusterTool = EFMissingETFromClustersMT( name="METFromClustersTool" )

    ### WARNING: this setting does not work for the scheduler: the variable is set, but the scheduler retrieves the default one
    clusterTool.ClustersCollection = ClustersName

    metAlg.METTools.append(clusterTool)
    metAlg.METTools.append(helperTool)

    metClusterRecoSequence += metAlg

    seqOut = metAlg.METContainerKey
    return (metClusterRecoSequence, seqOut)

def metJetAthSequence(ConfigFlags):
    InputMakerAlg= clusterFSInputMaker()
    (recoSequence, sequenceOut) = metJetRecoSequence()

    MetAthSequence =  seqAND("MetJetAthSequence",[InputMakerAlg, recoSequence ])
    return (MetAthSequence, InputMakerAlg, sequenceOut)


def metJetRecoSequence(RoIs = 'FSJetRoI'):

    from TrigUpgradeTest.jetDefs import jetRecoSequence
    (recoSequence, JetsName) = jetRecoSequence(RoIs)


    #################################################
    # Add EFMissingETAlg and associated tools
    #################################################
    metAlg = EFMissingETAlgMT( name="EFMET" )
    helperTool = EFMissingETFromHelperMT("theHelperTool")
    metAlg.METContainerKey = recordable("HLT_MET_mht")
    metAlg.MonTool = getMETMonTool()

    #///////////////////////////////////////////
    # Add EFMissingETFromCells tool
    #///////////////////////////////////////////
    from TrigEFMissingET.TrigEFMissingETConf import EFMissingETFromJetsMT
    mhtTool = EFMissingETFromJetsMT( name="METFromJetsTool" )

    ### This warning was copied from metCellRecoSequence(). Does this apply here? - @ggallard
    ### WARNING: this setting does not work for the scheduler: the variable is set, but the scheduler retrieves the default one
    mhtTool.JetsCollection=JetsName
    
    metAlg.METTools.append(mhtTool)
    metAlg.METTools.append(helperTool)

    recoSequence += metAlg

    seqOut = metAlg.METContainerKey
    return (recoSequence, seqOut)

