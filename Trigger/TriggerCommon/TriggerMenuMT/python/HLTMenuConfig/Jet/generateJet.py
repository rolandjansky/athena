# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, ChainStep, Chain, InEventReco, getChainStepName, createStepView
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def generateChains( flags, chainDict ):

    stepName = getChainStepName('Jet', 1)
    stepReco, stepView = createStepView(stepName)

    acc = ComponentAccumulator()
    acc.addSequence(stepView)

    # All this should be some common FS cell module?
    from TrigT2CaloCommon.TrigCaloDataAccessConfig import trigCaloDataAccessSvcCfg
    acc.merge( trigCaloDataAccessSvcCfg(flags) )
    cdaSvc = acc.getService("TrigCaloDataAccessSvc") # should be made primary component

    acc.printConfig()

    from TrigT2CaloCommon.CaloDef import clusterFSInputMaker
    inEventReco = InEventReco("JetReco",inputMaker=clusterFSInputMaker())

    cellsname = "CaloCellsFS"
    clustersname = "HLT_CaloTopoClustersFS"

    from TrigCaloRec.TrigCaloRecConfig import HLTCaloCellMaker
    cellmaker=HLTCaloCellMaker("HLTCaloCellMaker_FS")
    cellmaker.RoIs="FSJETRoI"
    cellmaker.TrigDataAccessMT=cdaSvc
    cellmaker.CellsName=cellsname

    inEventReco.addRecoAlg(cellmaker)

    from CaloRec.CaloTopoClusterConfig import CaloTopoClusterCfg
    inEventReco.mergeReco( CaloTopoClusterCfg(flags,cellsname=cellsname,clustersname=clustersname,doLCCalib=False,sequenceName=inEventReco.recoSeq.name()) )

    #sequencing of actual jet reconstruction
    from JetRecConfig import JetRecConfig
    from JetRecConfig.JetDefinition import JetConstit, JetDefinition, xAODType
   
    #hardcoded jet collection for now 
    clustermods = ["ECPSFrac","ClusterMoments"]
    trigMinPt = 7e3
    TrigEMTopo = JetConstit( xAODType.CaloCluster, ["EM"])
    TrigEMTopo.rawname = clustersname
    TrigEMTopo.inputname = clustersname
    TrigAntiKt4EMTopoSubJES = JetDefinition( "AntiKt", 0.4, TrigEMTopo, ptmin=trigMinPt,ptminfilter=trigMinPt)
    TrigAntiKt4EMTopoSubJES.modifiers = ["Calib:TrigRun2:data:JetArea_EtaJES_GSC_Insitu","Sort"] + clustermods 

    jetprefix="Trig"
    jetsuffix="subjesIS"
    # May need a switch to disable automatic modifier prerequisite generation
    jetRecoComps = JetRecConfig.JetRecCfg(TrigAntiKt4EMTopoSubJES, flags, jetprefix, jetsuffix)
    inEventReco.mergeReco(jetRecoComps)    

    acc.merge(inEventReco,stepReco.getName())

    #hypo
    from TrigHLTJetHypo.TrigHLTJetHypoConf import TrigJetHypoAlgMT
    from TrigHLTJetHypo.TrigJetHypoToolConfig import trigJetHypoToolFromDict
    hypo = TrigJetHypoAlgMT("TrigJetHypoAlgMT_a4tcem_subjesIS")
    jetsfullname = jetprefix+TrigAntiKt4EMTopoSubJES.basename+jetsuffix+"Jets"
    hypo.Jets = jetsfullname
    acc.addEventAlgo(hypo)

    jetSequence = MenuSequence( Sequence    = inEventReco.sequence(),
                                Maker       = inEventReco.inputMaker(),
                                Hypo        = hypo,
                                HypoToolGen = trigJetHypoToolFromDict,
                                CA = acc)

    jetStep = ChainStep(stepName, [jetSequence])

    l1Thresholds=[]
    for part in chainDict['chainParts']:
        l1Thresholds.append(part['L1threshold'])

    import pprint
    pprint.pprint(chainDict)

    acc.printConfig()

    chain = Chain( chainDict['chainName'], chainDict['L1item'], L1Thresholds=l1Thresholds, ChainSteps=[ jetStep ] )

    return chain
