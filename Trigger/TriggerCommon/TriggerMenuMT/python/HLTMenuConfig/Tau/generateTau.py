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

    def __ftfCore():                                                                                                                                                                 
        selAcc=SelectionCA('tauCoreFTF')                                                                                                                                      
        name = 'FTFCore'
        newRoITool   = CompFactory.ViewCreatorFetchFromViewROITool(RoisWriteHandleKey = 'HLT_Roi_TauCore',
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
        idTracking = trigInDetFastTrackingCfg(flags, roisKey=evtViewMaker.InViewRoIs, signatureName="TauCore")                                                                       
        fastInDetReco = InViewRecoCA('FastTauCore', viewMaker=evtViewMaker)                                                                                             
        fastInDetReco.mergeReco(idTracking)                                                                                         
        fastInDetReco.addRecoAlgo(CompFactory.AthViews.ViewDataVerifier(name='VDVFastTauCore',                                                 
                                DataObjects=[( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+%s' % evtViewMaker.InViewRoIs ),                                                        
                               ( 'xAOD::TauJetContainer' , 'StoreGateSvc+HLT_TrigTauRecMerged_CaloOnly')]) )                                                                         

        fastInDetReco.addRecoAlgo(CompFactory.TrigTauTrackRoiUpdater('TrackRoiUpdater',                                                                                                       
                                                               RoIInputKey                  = evtViewMaker.InViewRoIs,                                                               
                                                               RoIOutputKey                 = 'UpdatedTrackRoI',                                                                     
                                                               fastTracksKey                = flags.Trigger.InDetTracking.TauCore.trkTracks_FTF,                                                            
                                                               Key_trigTauJetInputContainer = "" ))                                                                            
        fastInDetReco.addRecoAlgo(CompFactory.TrigTauTrackRoiUpdater("TrackRoiUpdaterBDT",                                                                                                   
                                                               RoIInputKey                  = evtViewMaker.InViewRoIs,                                                               
                                                               RoIOutputKey                 = "UpdatedTrackBDTRoI",                                                              
                                                               fastTracksKey                = flags.Trigger.InDetTracking.TauCore.trkTracks_FTF,                                                            
                                                               useBDT                       = True,                                                                                  
                                                               Key_trigTauJetInputContainer = "HLT_TrigTauRecMerged_CaloOnly" ))                                                   
        selAcc.mergeReco(fastInDetReco)                                                                                                                     
        hypoAlg = CompFactory.TrigTrackPreSelHypoAlg("TrackPreSelHypoAlg_RejectEmpty",                                                                                             
                                                    trackcollection = flags.Trigger.InDetTracking.TauCore.trkTracks_FTF )                                                            
        selAcc.addHypoAlgo(hypoAlg)                                                                                                                                            
        from TrigTauHypo.TrigTauHypoTool import TrigTauTrackHypoToolFromDict                                                                                                          
        menuCA = MenuSequenceCA(selAcc, HypoToolGen=TrigTauTrackHypoToolFromDict)                                                                                                     
        return ChainStep(name=selAcc.name, Sequences=[menuCA], chainDicts=[chainDict], multiplicity=getChainMultFromDict(chainDict))   

    def __ftfIso():
        selAcc=SelectionCA('tauIsoFTF')
        name = 'FTFIso'
        newRoITool   = CompFactory.ViewCreatorFetchFromViewROITool(RoisWriteHandleKey = 'HLT_Roi_TauIso',
                                                                           InViewRoIs = 'UpdatedTrackRoI')                                                                                                                        
        evtViewMaker = CompFactory.EventViewCreatorAlgorithm('IM'+name,
                                                            ViewFallThrough   = True,
                                                            RoIsLink          = 'roi',
                                                            RoITool           = newRoITool,
                                                            InViewRoIs        = 'Tau'+name+'RoIs',
                                                            Views             = 'Tau'+name+'Views',
                                                            ViewNodeName      = 'Tau'+name+'InView',
                                                            RequireParentView = True,
                                                            mergeUsingFeature = True   )

        from TrigInDetConfig.TrigInDetConfig import trigInDetFastTrackingCfg
        idTracking = trigInDetFastTrackingCfg(flags, roisKey=evtViewMaker.InViewRoIs, signatureName="TauIso")
        fastInDetReco = InViewRecoCA('FastTauIso', viewMaker=evtViewMaker)
        fastInDetReco.mergeReco(idTracking)
        fastInDetReco.addRecoAlgo(CompFactory.AthViews.ViewDataVerifier(name='VDVFastTauIso',
                                DataObjects=[( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+%s' % evtViewMaker.InViewRoIs ),
                               ( 'xAOD::TauJetContainer' , 'StoreGateSvc+HLT_TrigTauRecMerged_CaloOnly')]) )

        selAcc.mergeReco(fastInDetReco)
        hypoAlg = CompFactory.TrigTrackPreSelHypoAlg("TrackPreSelHypoAlg_PassByIso",
                                                    trackcollection = flags.Trigger.InDetTracking.TauIso.trkTracks_FTF )

        from TrigTauHypo.TrigTauHypoConf import TrigTrackPreSelHypoTool
        TrigTrackPreSelHypoTool.AcceptAll = True

        selAcc.addHypoAlgo(hypoAlg)

        from TrigTauHypo.TrigTauHypoTool import TrigTauTrackHypoToolFromDict
        menuCA = MenuSequenceCA(selAcc, HypoToolGen=TrigTauTrackHypoToolFromDict)
        return ChainStep(name=selAcc.name, Sequences=[menuCA], chainDicts=[chainDict], multiplicity=getChainMultFromDict(chainDict))

    def __ftfIsoBDT():
        selAcc=SelectionCA('tauIsoBDTFTF')
        name = 'FTFIsoBDT'
        newRoITool   = CompFactory.ViewCreatorFetchFromViewROITool(RoisWriteHandleKey = 'HLT_Roi_TauIsoBDT',
                                                                           InViewRoIs = 'UpdatedTrackBDTRoI')                                                                                                 

        evtViewMaker = CompFactory.EventViewCreatorAlgorithm('IM'+name,
                                                            ViewFallThrough   = True,
                                                            RoIsLink          = 'roi',
                                                            RoITool           = newRoITool,
                                                            InViewRoIs        = 'Tau'+name+'RoIs',
                                                            Views             = 'Tau'+name+'Views',
                                                            ViewNodeName      = 'Tau'+name+'InView',
                                                            RequireParentView = True,
                                                            mergeUsingFeature = True   )

        from TrigInDetConfig.TrigInDetConfig import trigInDetFastTrackingCfg
        idTracking = trigInDetFastTrackingCfg(flags, roisKey=evtViewMaker.InViewRoIs, signatureName="TauIsoBDT")
        fastInDetReco = InViewRecoCA('FastTauIsoBDT', viewMaker=evtViewMaker)
        fastInDetReco.mergeReco(idTracking)
        fastInDetReco.addRecoAlgo(CompFactory.AthViews.ViewDataVerifier(name='VDVFastTauIsoBDT',
                                DataObjects=[( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+%s' % evtViewMaker.InViewRoIs ),
                               ( 'xAOD::TauJetContainer' , 'StoreGateSvc+HLT_TrigTauRecMerged_CaloOnly')]) )

        selAcc.mergeReco(fastInDetReco)
        hypoAlg = CompFactory.TrigTrackPreSelHypoAlg("TrackPreSelHypoAlg_PassByIsoBDT",
                                                    trackcollection = flags.Trigger.InDetTracking.TauIsoBDT.trkTracks_FTF )

        from TrigTauHypo.TrigTauHypoConf import TrigTrackPreSelHypoTool
        TrigTrackPreSelHypoTool.AcceptAll = True

        selAcc.addHypoAlgo(hypoAlg)

        from TrigTauHypo.TrigTauHypoTool import TrigTauTrackHypoToolFromDict
        menuCA = MenuSequenceCA(selAcc, HypoToolGen=TrigTauTrackHypoToolFromDict)
        return ChainStep(name=selAcc.name, Sequences=[menuCA], chainDicts=[chainDict], multiplicity=getChainMultFromDict(chainDict))


    thresholds = [p["L1threshold"] for p in chainDict['chainParts'] if p['signature'] == 'Tau' ]
    chain = Chain( name=chainDict['chainName'], L1Thresholds=thresholds, ChainSteps=[ __calo(), __ftfTau(), __ftfCore(), __ftfIso(), __ftfIsoBDT() ] )
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
