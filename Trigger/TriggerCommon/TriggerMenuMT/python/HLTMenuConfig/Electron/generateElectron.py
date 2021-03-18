# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from TriggerMenuMT.HLTMenuConfig.Electron.ElectronRecoSequences import l2CaloRecoCfg, l2CaloHypoCfg
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequenceCA, \
    ChainStep, Chain, EmptyMenuSequence, InViewRecoCA, SelectionCA

from TrigEgammaHypo.TrigEgammaFastCaloHypoTool import TrigEgammaFastCaloHypoToolFromDict
from TrigEDMConfig.TriggerEDMRun3 import recordable
from AthenaConfiguration.ComponentFactory import CompFactory
from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import getChainMultFromDict


def _fastCalo(flags, chainDict):
    selAcc=SelectionCA('FastCaloElectron')
    selAcc.mergeReco(l2CaloRecoCfg(flags))

     # this alg needs EventInfo decorated with the  pileup info
    from LumiBlockComps.LumiBlockMuWriterConfig import LumiBlockMuWriterCfg
    selAcc.merge(LumiBlockMuWriterCfg(flags))

    l2CaloHypo = l2CaloHypoCfg(flags,
                               name='L2ElectronCaloHypo',
                               CaloClusters=recordable('HLT_FastCaloEMClusters'))

    selAcc.addHypoAlgo(l2CaloHypo)

    fastCaloSequence = MenuSequenceCA(selAcc,
                                      HypoToolGen=TrigEgammaFastCaloHypoToolFromDict)

     # this cannot work for asymmetric combined chains....FP
    return ChainStep(name=selAcc.name, Sequences=[fastCaloSequence], chainDicts=[chainDict], multiplicity=getChainMultFromDict(chainDict))

def _ftf(flags, chainDict):
    selAcc=SelectionCA('ElectronFTF')
    # # # fast ID (need to be customised because require secialised configuration of the views maker - i.e. parent has to be linked)
    name = "IMFastElectron"
    evtViewMaker = CompFactory.EventViewCreatorAlgorithm(name,
                                                            ViewFallThrough = True,
                                                            RoIsLink        = 'initialRoI',
                                                            RoITool         = CompFactory.ViewCreatorInitialROITool(),
                                                            InViewRoIs      = name+'RoIs',
                                                            Views           = name+'Views',
                                                            ViewNodeName    = 'FastElectronInView',
                                                            RequireParentView = True)
    del name
    from TrigInDetConfig.TrigInDetConfig import trigInDetFastTrackingCfg
    idTracking = trigInDetFastTrackingCfg(flags, roisKey=evtViewMaker.InViewRoIs, signatureName="Electron")
    fastInDetReco = InViewRecoCA('FastElectron', viewMaker=evtViewMaker)
    fastInDetReco.mergeReco(idTracking)
    fastInDetReco.addRecoAlgo(CompFactory.AthViews.ViewDataVerifier(name='VDVElectronFastCalo',
                                DataObjects=[('xAOD::TrigEMClusterContainer', 'StoreGateSvc+HLT_FastCaloEMClusters')]) )
    from TrigEgammaHypo.TrigEgammaFastElectronFexMTConfig import fastElectronFexAlgCfg
    fastInDetReco.mergeReco(fastElectronFexAlgCfg(flags, rois=evtViewMaker.InViewRoIs))
    selAcc.mergeReco(fastInDetReco)
    fastElectronHypoAlg = CompFactory.TrigEgammaFastElectronHypoAlgMT()
    fastElectronHypoAlg.Electrons = 'HLT_FastElectrons'
    fastElectronHypoAlg.RunInView = True
    selAcc.addHypoAlgo(fastElectronHypoAlg)
    from TrigEgammaHypo.TrigEgammaFastElectronHypoTool import TrigEgammaFastElectronHypoToolFromDict
    fastInDetSequence = MenuSequenceCA(selAcc,
                                        HypoToolGen=TrigEgammaFastElectronHypoToolFromDict)
    return ChainStep( name=selAcc.name, Sequences=[fastInDetSequence], chainDicts=[chainDict], multiplicity=getChainMultFromDict(chainDict))

def _precisonCalo(flags, chainDict):
    recoAcc = InViewRecoCA('ElectronRoITopoClusterReco')
    recoAcc.addRecoAlgo(CompFactory.AthViews.ViewDataVerifier(name='VDV'+recoAcc.name,
                                                              DataObjects=[('TrigRoiDescriptorCollection', recoAcc.inputMaker().InViewRoIs),
                                                                           ('CaloBCIDAverage', 'StoreGateSvc+CaloBCIDAverage')]))

    from TrigCaloRec.TrigCaloRecConfig import hltCaloTopoClusteringCfg
    recoAcc.mergeReco(hltCaloTopoClusteringCfg(flags,
                                               namePrefix="Electron",
                                               roisKey=recoAcc.inputMaker().InViewRoIs)) # RoI

    copier = CompFactory.egammaTopoClusterCopier('TrigEgammaTopoClusterCopierPrecisionCaloRoIs',
                                                 InputTopoCollection='HLT_TopoCaloClustersRoI',
                                                 OutputTopoCollection='HLT_CaloEMClusters',
                                                 OutputTopoCollectionShallow='tmp_HLT_CaloEMClusters')
    recoAcc.addRecoAlgo(copier)

    selAcc = SelectionCA('PrecisionCalo')
    selAcc.mergeReco(recoAcc)
    hypoAlg = CompFactory.TrigEgammaPrecisionCaloHypoAlgMT(name='ElectronPrecisionCaloHypo',
                                                           CaloClusters=recordable('HLT_CaloEMClusters'))
    selAcc.addHypoAlgo(hypoAlg)
    from TrigEgammaHypo.TrigEgammaPrecisionCaloHypoTool import TrigEgammaPrecisionCaloHypoToolFromDict
    menuSequence = MenuSequenceCA(selAcc,
                                  HypoToolGen=TrigEgammaPrecisionCaloHypoToolFromDict)
    return ChainStep(name=selAcc.name, Sequences=[menuSequence], chainDicts=[chainDict], multiplicity=getChainMultFromDict(chainDict))


def generateChains(flags, chainDict):

    l1Thresholds=[]
    for part in chainDict['chainParts']:
        l1Thresholds.append(part['L1threshold'])

    # # # offline egamma
    chain = Chain(chainDict['chainName'], L1Thresholds=l1Thresholds,
                            ChainSteps=[_fastCalo(flags, chainDict), _ftf(flags, chainDict), _precisonCalo(flags, chainDict)])

    return chain

if __name__ == "__main__":
    # run with: python -m TriggerMenuMT.HLTMenuConfig.Electron.generateElectron
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.lock()
    from ..Menu.DictFromChainName import dictFromChainName
    chain = generateChains(ConfigFlags, dictFromChainName('HLT_e26_L1EM15'))
    for step in chain.steps:
        for s in step.sequences:
            if not isinstance(s, EmptyMenuSequence):
                s.ca.printConfig(withDetails=True, summariseProps=False) # flip the last arg to see all settings
                s.ca.wasMerged() # to silence check for orphanted CAs
