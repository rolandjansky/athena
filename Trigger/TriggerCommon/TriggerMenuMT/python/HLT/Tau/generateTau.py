# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from TriggerMenuMT.HLT.Menu.MenuComponents import Chain, ChainStep, MenuSequenceCA, SelectionCA, InViewRecoCA, EmptyMenuSequence
from AthenaConfiguration.ComponentFactory import CompFactory
from TriggerMenuMT.HLT.Menu.DictFromChainName import getChainMultFromDict

from AthenaConfiguration.AccumulatorCache import AccumulatorCache

def generateChains( flags, chainDict ):
    flags = flags.cloneAndReplace('Tau', 'Trigger.Offline.Tau') # use from now on trigger variant of flags
    
    @AccumulatorCache
    def __caloSeq(flags):
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

        from TrigTauRec.TrigTauRecConfig import TrigTauRecMergedOnlyMVACfg
        recoAcc.addRecoAlgo(CompFactory.TrigTauCaloRoiUpdater("TauCaloRoiUpdater",
                                                                CaloClustersKey = 'HLT_TopoCaloClustersLC',
                                                                RoIInputKey = 'HLT_TAURoI',
                                                                RoIOutputKey = 'UpdatedCaloRoI'))
        recoAcc.mergeReco(TrigTauRecMergedOnlyMVACfg(flags))

        selAcc = SelectionCA("CaloTau")
        selAcc.mergeReco(recoAcc)
        hypoAlg = CompFactory.TrigTauCaloHypoAlg("HL2CaloTauHypo",
                                                    taujets = "HLT_TrigTauRecMerged_CaloMVAOnly" )
        selAcc.addHypoAlgo(hypoAlg)
        from TrigTauHypo.TrigTauHypoTool import TrigL2TauHypoToolFromDict
        menuCA = MenuSequenceCA(selAcc, HypoToolGen=TrigL2TauHypoToolFromDict)
        return (selAcc , menuCA)

    def __calo():
        (selAcc , menuCA) = __caloSeq(flags)
        return ChainStep(name=selAcc.name, Sequences=[menuCA], chainDicts=[chainDict], multiplicity=getChainMultFromDict(chainDict))

    @AccumulatorCache
    def __ftfTauSeq(flags):
        selAcc=SelectionCA('tauFTF')
        newRoITool   = CompFactory.ViewCreatorFetchFromViewROITool(RoisWriteHandleKey = 'HLT_Roi_Tau', 
                                                                           InViewRoIs = 'UpdatedCaloRoI')

        from TrigInDetConfig.TrigInDetConfig import trigInDetFastTrackingCfg
        fastInDetReco = InViewRecoCA('FastTau',
                                        RoIsLink          = 'UpdatedCaloRoI',
                                        RoITool           = newRoITool,
                                        RequireParentView = True,
                                        mergeUsingFeature = True
         #viewMaker=evtViewMaker
         )
        idTracking = trigInDetFastTrackingCfg(flags, roisKey=fastInDetReco.inputMaker().InViewRoIs, signatureName="Tau")
        fastInDetReco.mergeReco(idTracking)
        fastInDetReco.addRecoAlgo(CompFactory.AthViews.ViewDataVerifier(name='VDVFastTau',
                                DataObjects=[( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+{}'.format(fastInDetReco.inputMaker().InViewRoIs) ),
                               ( 'xAOD::TauJetContainer' , 'StoreGateSvc+HLT_TrigTauRecMerged_CaloMVAOnly')]) )
        selAcc.mergeReco(fastInDetReco)
        hypoAlg = CompFactory.TrigTrackPreSelHypoAlg("TrackPreSelHypoAlg_PassByTau",
                                                    trackcollection = flags.Trigger.InDetTracking.Tau.trkTracks_FTF, RoIForIDReadHandleKey="" )
        selAcc.addHypoAlgo(hypoAlg)
        from TrigTauHypo.TrigTauHypoTool import TrigTauTrackHypoToolFromDict
        menuCA = MenuSequenceCA(selAcc, HypoToolGen=TrigTauTrackHypoToolFromDict)
        return (selAcc , menuCA)

    def __ftfTau():
        (selAcc , menuCA) = __ftfTauSeq(flags)
        return ChainStep(name=selAcc.name, Sequences=[menuCA], chainDicts=[chainDict], multiplicity=getChainMultFromDict(chainDict))

    @AccumulatorCache
    def __ftfCoreSeq(flags):                                                                                                                                                                 
        selAcc=SelectionCA('tauCoreFTF')                                                                                                                                      
        newRoITool   = CompFactory.ViewCreatorFetchFromViewROITool(RoisWriteHandleKey = 'HLT_Roi_TauCore',
                                                                           InViewRoIs = 'UpdatedCaloRoI')                                                                                                                                                      

        from TrigInDetConfig.TrigInDetConfig import trigInDetFastTrackingCfg                                                                                                   
        fastInDetReco = InViewRecoCA('FastTauCore', RoIsLink          = 'UpdatedCaloRoI',                                                                                    
                                                    RoITool           = newRoITool,
                                                    RequireParentView = True,                                                                                                
                                                    mergeUsingFeature = True) 

        idTracking = trigInDetFastTrackingCfg(flags, roisKey=fastInDetReco.inputMaker().InViewRoIs, signatureName="TauCore")                                                                       
        fastInDetReco.mergeReco(idTracking)                                                                                         
        fastInDetReco.addRecoAlgo(CompFactory.AthViews.ViewDataVerifier(name='VDVFastTauCore',                                                 
                                DataObjects=[( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+{}'.format(fastInDetReco.inputMaker().InViewRoIs) ),
                               ( 'xAOD::TauJetContainer' , 'StoreGateSvc+HLT_TrigTauRecMerged_CaloMVAOnly')]) )                                                                         

        fastInDetReco.addRecoAlgo(CompFactory.TrigTauTrackRoiUpdater('TrackRoiUpdater',                                                                                                       
                                                               RoIInputKey                  = fastInDetReco.inputMaker().InViewRoIs,                                                               
                                                               RoIOutputKey                 = 'UpdatedTrackRoI',                                                                     
                                                               fastTracksKey                = flags.Trigger.InDetTracking.TauCore.trkTracks_FTF,                                                            
                                                               Key_trigTauJetInputContainer = "" ))                                                                            
        fastInDetReco.addRecoAlgo(CompFactory.TrigTauTrackRoiUpdater("TrackRoiUpdaterBDT",                                                                                                   
                                                               RoIInputKey                  = fastInDetReco.inputMaker().InViewRoIs,                                                               
                                                               RoIOutputKey                 = "UpdatedTrackBDTRoI",                                                              
                                                               fastTracksKey                = flags.Trigger.InDetTracking.TauCore.trkTracks_FTF,                                                            
                                                               useBDT                       = True,                                                                                  
                                                               Key_trigTauJetInputContainer = "HLT_TrigTauRecMerged_CaloMVAOnly" ))                                                   
        selAcc.mergeReco(fastInDetReco)                                                                                                                     
        hypoAlg = CompFactory.TrigTrackPreSelHypoAlg("TrackPreSelHypoAlg_RejectEmpty",                                                                                             
                                                    trackcollection = flags.Trigger.InDetTracking.TauCore.trkTracks_FTF )                                                            
        selAcc.addHypoAlgo(hypoAlg)                                                                                                                                            
        from TrigTauHypo.TrigTauHypoTool import TrigTauTrackHypoToolFromDict                                                                                                          
        menuCA = MenuSequenceCA(selAcc, HypoToolGen=TrigTauTrackHypoToolFromDict)                                                                                                     
        return (selAcc , menuCA)
    
    def __ftfCore():                                                                                                                                                                 
        (selAcc , menuCA) = __ftfCoreSeq(flags)
        return ChainStep(name=selAcc.name, Sequences=[menuCA], chainDicts=[chainDict], multiplicity=getChainMultFromDict(chainDict))   

    @AccumulatorCache
    def __ftfIsoSeq(flags):
        selAcc=SelectionCA('tauIsoFTF')
        newRoITool   = CompFactory.ViewCreatorFetchFromViewROITool(RoisWriteHandleKey = 'HLT_Roi_TauIso',
                                                                           InViewRoIs = 'UpdatedTrackRoI')                                                                                                                        

        from TrigInDetConfig.TrigInDetConfig import trigInDetFastTrackingCfg
        fastInDetReco = InViewRecoCA('FastTauIso', RoITool           = newRoITool,
                                                   RequireParentView = True,
                                                   mergeUsingFeature = True )

        idTracking = trigInDetFastTrackingCfg(flags, roisKey=fastInDetReco.inputMaker().InViewRoIs, signatureName="TauIso")
        fastInDetReco.mergeReco(idTracking)
        fastInDetReco.addRecoAlgo(CompFactory.AthViews.ViewDataVerifier(name='VDVFastTauIso',
                                DataObjects=[( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+{}'.format(fastInDetReco.inputMaker().InViewRoIs) ),
                               ( 'xAOD::TauJetContainer' , 'StoreGateSvc+HLT_TrigTauRecMerged_CaloMVAOnly')]) )

        selAcc.mergeReco(fastInDetReco)
        hypoAlg = CompFactory.TrigTrackPreSelHypoAlg("TrackPreSelHypoAlg_PassByIso",
                                                    trackcollection = flags.Trigger.InDetTracking.TauIso.trkTracks_FTF )

        from TrigTauHypo.TrigTauHypoConf import TrigTrackPreSelHypoTool
        TrigTrackPreSelHypoTool.AcceptAll = True

        selAcc.addHypoAlgo(hypoAlg)

        from TrigTauHypo.TrigTauHypoTool import TrigTauTrackHypoToolFromDict
        menuCA = MenuSequenceCA(selAcc, HypoToolGen=TrigTauTrackHypoToolFromDict)
        return (selAcc , menuCA)
    
    def __ftfIso():
        (selAcc , menuCA) = __ftfIsoSeq(flags)
        return ChainStep(name=selAcc.name, Sequences=[menuCA], chainDicts=[chainDict], multiplicity=getChainMultFromDict(chainDict))

    @AccumulatorCache
    def __ftfIsoBDTSeq(flags):
        selAcc=SelectionCA('tauIsoBDTFTF')
        newRoITool   = CompFactory.ViewCreatorFetchFromViewROITool(RoisWriteHandleKey = 'HLT_Roi_TauIsoBDT',
                                                                           InViewRoIs = 'UpdatedTrackBDTRoI')                                                                                                 

        from TrigInDetConfig.TrigInDetConfig import trigInDetFastTrackingCfg
        fastInDetReco = InViewRecoCA('FastTauIsoBDT',   RoITool           = newRoITool,
                                                        RequireParentView = True,
                                                        mergeUsingFeature = True )
        idTracking = trigInDetFastTrackingCfg(flags, roisKey=fastInDetReco.inputMaker().InViewRoIs, signatureName="TauIsoBDT")
        fastInDetReco.mergeReco(idTracking)
        fastInDetReco.addRecoAlgo(CompFactory.AthViews.ViewDataVerifier(name='VDVFastTauIsoBDT',
                                DataObjects=[( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+{}'.format(fastInDetReco.inputMaker().InViewRoIs) ),
                               ( 'xAOD::TauJetContainer' , 'StoreGateSvc+HLT_TrigTauRecMerged_CaloMVAOnly')]) )

        selAcc.mergeReco(fastInDetReco)
        hypoAlg = CompFactory.TrigTrackPreSelHypoAlg("TrackPreSelHypoAlg_PassByIsoBDT",
                                                    trackcollection = flags.Trigger.InDetTracking.TauIsoBDT.trkTracks_FTF )

        from TrigTauHypo.TrigTauHypoConf import TrigTrackPreSelHypoTool
        TrigTrackPreSelHypoTool.AcceptAll = True

        selAcc.addHypoAlgo(hypoAlg)

        from TrigTauHypo.TrigTauHypoTool import TrigTauTrackHypoToolFromDict
        menuCA = MenuSequenceCA(selAcc, HypoToolGen=TrigTauTrackHypoToolFromDict)
        return (selAcc , menuCA)
    
    def __ftfIsoBDT():
        (selAcc , menuCA) = __ftfIsoBDTSeq(flags)
        return ChainStep(name=selAcc.name, Sequences=[menuCA], chainDicts=[chainDict], multiplicity=getChainMultFromDict(chainDict))


    thresholds = [p["L1threshold"] for p in chainDict['chainParts'] if p['signature'] == 'Tau' ]
    chain = Chain( name=chainDict['chainName'], L1Thresholds=thresholds, ChainSteps=[ __calo(), __ftfTau(), __ftfCore(), __ftfIso(), __ftfIsoBDT() ] )
    return chain



if __name__ == "__main__":
    # run with: python -m TriggerMenuMT.HLT.Tau.generateTau
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
