# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import Chain, ChainStep, MenuSequenceCA, SelectionCA, InViewRecoCA, EmptyMenuSequence
from AthenaConfiguration.ComponentFactory import CompFactory
from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import getChainMultFromDict

def generateChains( flags, chainDict ):
    flags = flags.cloneAndReplace('Tau', 'Trigger.Offline.Tau') # use from now on trigger variant of flags

    def __calo():
        recoAcc = InViewRecoCA('CaloTauReco')
        from TrigCaloRec.TrigCaloRecConfig import hltCaloTopoClusteringCfg
        recoAcc.addRecoAlgo(CompFactory.AthViews.ViewDataVerifier(name='VDV'+recoAcc.name,
                                                                  DataObjects=[('TrigRoiDescriptorCollection', recoAcc.inputMaker().InViewRoIs),
                                                                               ( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+HLT_TAURoI'),
                                                                               ('CaloBCIDAverage', 'StoreGateSvc+CaloBCIDAverage'),
                                                                               ( 'SG::AuxElement' , 'StoreGateSvc+EventInfo.averageInteractionsPerCrossing'),
                                                                               ( 'SG::AuxElement' , 'StoreGateSvc+EventInfo.actualInteractionsPerCrossing') ]))
        recoAcc.mergeReco(hltCaloTopoClusteringCfg(flags,
                                                    namePrefix="Tau",
                                                    roisKey=recoAcc.inputMaker().InViewRoIs,
                                                    clustersKey='HLT_TopoCaloClustersLC'))

        from TrigTauRec.TrigTauRecConfigMT import TrigTauRecMergedOnlyMVACfg
        recoAcc.addRecoAlgo(CompFactory.TrigTauCaloRoiUpdater("TauCaloRoiUpdater",
                                                                CaloClustersKey = 'HLT_TopoCaloClustersLC',
                                                                RoIInputKey = 'HLT_TAURoI',
                                                                RoIOutputKey = 'UpdatedCaloRoI'))
        recoAcc.mergeReco(TrigTauRecMergedOnlyMVACfg(flags))

        selAcc = SelectionCA("CaloTau")
        selAcc.mergeReco(recoAcc)
        hypoAlg = CompFactory.TrigTauCaloHypoAlg("HL2CaloTauHypo",
                                                    taujets = "HLT_TrigTauRecMerged_CaloOnly" )
        selAcc.addHypoAlgo(hypoAlg)
        from TrigTauHypo.TrigTauHypoTool import TrigL2TauHypoToolFromDict
        menuCA = MenuSequenceCA(selAcc, HypoToolGen=TrigL2TauHypoToolFromDict)
        return ChainStep(name=selAcc.name, Sequences=[menuCA], chainDicts=[chainDict], multiplicity=getChainMultFromDict(chainDict))

    def __ftfTau():
        selAcc=SelectionCA('tauFTF')
        name = 'FTFTau'
        newRoITool   = CompFactory.ViewCreatorFetchFromViewROITool(RoisWriteHandleKey = 'HLT_Roi_Tau', 
                                                                           InViewRoIs = 'UpdatedCaloRoI')

        evtViewMaker = CompFactory.EventViewCreatorAlgorithm('IM'+name,
                                                            ViewFallThrough   = True,
                                                            RoIsLink          = 'UpdatedCaloRoI',
                                                            RoITool           = newRoITool,
                                                            InViewRoIs        = 'Tau'+name+'RoIs',
                                                            Views             = 'Tau'+name+'Views',
                                                            ViewNodeName      = 'Tau'+name+'InView',
                                                            RequireParentView = True,
                                                            mergeUsingFeature = True   )

        from TrigInDetConfig.TrigInDetConfig import trigInDetFastTrackingCfg
        idTracking = trigInDetFastTrackingCfg(flags, roisKey=evtViewMaker.InViewRoIs, signatureName="Tau")
        fastInDetReco = InViewRecoCA('FastTau', viewMaker=evtViewMaker)
        fastInDetReco.mergeReco(idTracking)
        fastInDetReco.addRecoAlgo(CompFactory.AthViews.ViewDataVerifier(name='VDVFastTau',
                                DataObjects=[( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+%s' % evtViewMaker.InViewRoIs ),
                               ( 'xAOD::TauJetContainer' , 'StoreGateSvc+HLT_TrigTauRecMerged_CaloOnly')]) )
        selAcc.mergeReco(fastInDetReco)
        hypoAlg = CompFactory.TrigTrackPreSelHypoAlg("TrackPreSelHypoAlg_PassByTau",
                                                    trackcollection = flags.Trigger.InDetTracking.Tau.trkTracks_FTF, RoIForIDReadHandleKey="" )
        selAcc.addHypoAlgo(hypoAlg)
        from TrigTauHypo.TrigTauHypoTool import TrigTauTrackHypoToolFromDict
        menuCA = MenuSequenceCA(selAcc, HypoToolGen=TrigTauTrackHypoToolFromDict)
        return ChainStep(name=selAcc.name, Sequences=[menuCA], chainDicts=[chainDict], multiplicity=getChainMultFromDict(chainDict))


    thresholds = [p["L1threshold"] for p in chainDict['chainParts'] if p['signature'] == 'Tau' ]
    chain = Chain( name=chainDict['chainName'], L1Thresholds=thresholds, ChainSteps=[ __calo(), __ftfTau() ] )
    return chain


if __name__ == "__main__":
    # run with: python -m TriggerMenuMT.HLTMenuConfig.Tau.generateTau
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.lock()
    from ..Menu.DictFromChainName import dictFromChainName
    chain = generateChains(ConfigFlags, dictFromChainName('HLT_tau0_perf_ptonly_L1TAU8'))
    for step in chain.steps:
        for s in step.sequences:
            if not isinstance(s, EmptyMenuSequence):
                s.ca.printConfig(withDetails=True, summariseProps=True, printDefaults=True) # flip the last arg to see all settings
                s.ca.wasMerged() # to silence check for orphanted CAs
