# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from TriggerMenuMT.HLTMenuConfig.Electron.ElectronRecoSequences import l2CaloRecoCfg, l2CaloHypoCfg
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import CAMenuSequence, \
    ChainStep, Chain, createStepView, EmptyMenuSequence, InViewReco

from TrigEgammaHypo.TrigEgammaFastCaloHypoTool import TrigEgammaFastCaloHypoToolFromDict
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from TrigEDMConfig.TriggerEDMRun3 import recordable
from AthenaConfiguration.ComponentFactory import CompFactory
from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import getChainMultFromDict



def generateChains( flags,  chainDict ):
    import pprint
    pprint.pprint( chainDict )

    firstStepName = 'FastCaloElectron'
    stepReco, stepView = createStepView(firstStepName)

    accCalo = ComponentAccumulator()
    accCalo.addSequence(stepView)

    l2CaloReco = l2CaloRecoCfg(flags)
    accCalo.merge(l2CaloReco, sequenceName=stepReco.getName())

    # this alg needs EventInfo decorated with the  pileup info
    from LumiBlockComps.LumiBlockMuWriterConfig import LumiBlockMuWriterCfg
    accCalo.merge( LumiBlockMuWriterCfg(flags) )

    l2CaloHypo =  l2CaloHypoCfg( flags, name = 'L2ElectronCaloHypo',
                                 CaloClusters = recordable('HLT_FastCaloEMClusters'))

    accCalo.addEventAlgo(l2CaloHypo, sequenceName=stepView.getName())

    fastCaloSequence = CAMenuSequence(accCalo,
                                     HypoToolGen = TrigEgammaFastCaloHypoToolFromDict)

    accCalo.printConfig()
    # this cannot work for asymmetric combined chains....FP
    fastCaloStep = ChainStep(name=firstStepName, Sequences=[fastCaloSequence], chainDicts=[chainDict], multiplicity=getChainMultFromDict(chainDict))
    


    secondStepName = 'ElectronFTF'
    stepReco, stepView = createStepView(secondStepName)

    accTrk = ComponentAccumulator()
    accTrk.addSequence(stepView)

    # # # fast ID (need to be customised because require secialised configuration of the views maker - i.e. parent has to be linked)
    name = "IMFastElectron"
    evtViewMaker = CompFactory.EventViewCreatorAlgorithm(name,
                                                          ViewFallThrough = True,
                                                          RoIsLink        = 'initialRoI',
                                                          RoITool         = CompFactory.ViewCreatorInitialROITool(),
                                                          InViewRoIs      = name+'RoIs',
                                                          Views           = name+'Views',
                                                          ViewNodeName    = name+"InView",
                                                          RequireParentView = True)
    del name

    from TrigInDetConfig.TrigInDetConfig import trigInDetCfg
    idTracking = trigInDetCfg(flags, roisKey=evtViewMaker.InViewRoIs, signatureName="Electron")

    fastInDetReco = InViewReco("FastElectron", viewMaker=evtViewMaker)
    fastInDetReco.mergeReco(idTracking)
    fastInDetReco.addRecoAlgo(CompFactory.AthViews.ViewDataVerifier( name = 'VDVElectronFastCalo',
                              DataObjects=[('xAOD::TrigEMClusterContainer' , 'StoreGateSvc+HLT_FastCaloEMClusters')]) )

    from TrigEgammaHypo.TrigEgammaFastElectronFexMTConfig import fastElectronFexAlgCfg
    fastInDetReco.mergeReco(fastElectronFexAlgCfg(flags, rois=evtViewMaker.InViewRoIs))

    accTrk.merge(fastInDetReco, sequenceName=stepReco.getName())


    fastElectronHypoAlg = CompFactory.TrigEgammaFastElectronHypoAlgMT()
    fastElectronHypoAlg.Electrons = "HLT_FastElectrons"
    fastElectronHypoAlg.RunInView = True
    accTrk.addEventAlgo(fastElectronHypoAlg, sequenceName=stepView.getName())

    from TrigEgammaHypo.TrigEgammaFastElectronHypoTool import TrigEgammaFastElectronHypoToolFromDict
    fastInDetSequence = CAMenuSequence(accTrk, 
                                       HypoToolGen = TrigEgammaFastElectronHypoToolFromDict)

    fastInDetStep = ChainStep( name=secondStepName, Sequences=[fastInDetSequence], chainDicts=[chainDict], multiplicity=getChainMultFromDict(chainDict))

    l1Thresholds=[]
    for part in chainDict['chainParts']:
        l1Thresholds.append(part['L1threshold'])
    
    # # # EF calo

    # # # EF ID
    
    # # # offline egamma
    emptyStep = ChainStep(name="EmptyElStep", Sequences=[EmptyMenuSequence("EmptyElStep")], chainDicts=[chainDict])
    chain = Chain(chainDict['chainName'], L1Thresholds=l1Thresholds, ChainSteps=[fastCaloStep, fastInDetStep, emptyStep])
    
    return chain
