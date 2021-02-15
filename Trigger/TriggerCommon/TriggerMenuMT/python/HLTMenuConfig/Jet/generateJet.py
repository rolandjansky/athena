# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import CAMenuSequence, ChainStep, Chain, InEventReco, createStepView
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
import pprint
from AthenaCommon.Logging import logging
from ..CommonSequences.FullScanDefs import caloFSRoI
log = logging.getLogger( 'TriggerMenuMT.HLTMenuConfig.Jet.generateJet' )

def HLTCaloCellMakerCfg( flags, cellsname, cdaSvc ):
    result = ComponentAccumulator()
    from TrigT2CaloCommon.TrigCaloDataAccessConfig import trigCaloDataAccessSvcCfg, CaloDataAccessSvcDependencies
    
    result.merge(trigCaloDataAccessSvcCfg(flags))
    verifier = CompFactory.AthViews.ViewDataVerifier( name = 'VDVFSCaloJet',
                                                    DataObjects = [('TrigRoiDescriptorCollection', f"StoreGateSvc+{caloFSRoI}"),
                                                                  ('CaloBCIDAverage', 'StoreGateSvc+CaloBCIDAverage') ])
    result.addEventAlgo( verifier )
    cellmaker = CompFactory.HLTCaloCellMaker("HLTCaloCellMaker_FS")

    cellmaker.RoIs = caloFSRoI
    cellmaker.TrigDataAccessMT = cdaSvc
    cellmaker.CellsName = cellsname
    cellmaker.ExtraInputs = CaloDataAccessSvcDependencies

    result.addEventAlgo(cellmaker)
    return result

def generateChains( flags, chainDict ):

    stepName = 'Jet'
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
    
    cellmakerCfg = HLTCaloCellMakerCfg(flags, cellsname, cdaSvc)

    inEventReco.mergeReco( cellmakerCfg )

    from CaloRec.CaloTopoClusterConfig import CaloTopoClusterCfg
    inEventReco.mergeReco( CaloTopoClusterCfg( flags,
                                    cellsname = cellsname,
                                    clustersname = clustersname,
                                    doLCCalib = False) )

    #sequencing of actual jet reconstruction
    from JetRecConfig import JetRecConfig
    from JetRecConfig.JetDefinition import JetConstitSeq, JetDefinition, xAODType, JetInputDef
    from JetRecConfig.StandardJetConstits import jetinputdefdic

    # declare a jet source for the HLT clusters if needed :
    jetinputdefdic.setdefault(clustersname , JetInputDef(clustersname, xAODType.CaloCluster) )
    #hardcoded jet collection for now 
    clustermods = ["ECPSFrac","ClusterMoments"]
    trigMinPt = 7000
    HLT_EMTopo = JetConstitSeq( "HLT_EMTopo",xAODType.CaloCluster, ["EM"], clustersname, clustersname,label="EMTopo")
    
    HLT_AntiKt4EMTopo_subjesIS = JetDefinition( "AntiKt", 0.4, HLT_EMTopo, ptmin=trigMinPt,
                                                prefix="HLT_",
                                                suffix = "_subjesIS",
                                               )
    HLT_AntiKt4EMTopo_subjesIS.modifiers = ["Calib:TrigRun2:data:JetArea_EtaJES_GSC_Insitu:HLT_Kt4EMTopoEventShape","Sort", "Filter:"+str(trigMinPt)] + clustermods 

    # May need a switch to disable automatic modifier prerequisite generation
    jetRecoComps = JetRecConfig.JetRecCfg(HLT_AntiKt4EMTopo_subjesIS, flags) 
    inEventReco.mergeReco(jetRecoComps)    

    acc.merge(inEventReco,stepReco.getName())

    #hypo
    from TrigHLTJetHypo.TrigJetHypoToolConfig import trigJetHypoToolFromDict
    hypo = CompFactory.TrigJetHypoAlgMT("TrigJetHypoAlgMT_a4tcem_subjesIS")
    jetsfullname = HLT_AntiKt4EMTopo_subjesIS.fullname()
    hypo.Jets = jetsfullname
    acc.addEventAlgo(hypo, sequenceName=stepView.getName() )

    jetSequence = CAMenuSequence(acc,
                                 HypoToolGen = trigJetHypoToolFromDict)

    jetStep = ChainStep(name=stepName, Sequences=[jetSequence], chainDicts=[chainDict])
    
    l1Thresholds=[]
    for part in chainDict['chainParts']:
        l1Thresholds.append(part['L1threshold'])

    log.debug('dictionary is: %s\n', pprint.pformat(chainDict))

    acc.printConfig()

    chain = Chain( chainDict['chainName'], L1Thresholds=l1Thresholds, ChainSteps=[jetStep] )

    return chain
