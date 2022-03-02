# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from TriggerMenuMT.HLT.Electron.ElectronRecoSequences import l2CaloRecoCfg
from TriggerMenuMT.HLT.Photon.PhotonRecoSequences import l2PhotonRecoCfg, l2PhotonHypoCfg
from TriggerMenuMT.HLT.Config.MenuComponents import MenuSequenceCA, SelectionCA, ChainStep, Chain
from TriggerMenuMT.HLT.Config.Utility.DictFromChainName import getChainMultFromDict
from TrigEDMConfig.TriggerEDMRun3 import recordable
from TrigEgammaHypo.TrigEgammaFastCaloHypoTool import TrigEgammaFastCaloHypoToolFromDict
from TrigEgammaHypo.TrigEgammaFastPhotonHypoTool import TrigEgammaFastPhotonHypoToolFromDict
from AthenaConfiguration.AccumulatorCache import AccumulatorCache

import pprint
from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)

@AccumulatorCache
def _fastCaloSeq(flags):
    selAcc=SelectionCA('FastCaloPhoton')
    selAcc.mergeReco(l2CaloRecoCfg(flags))

    from TrigEgammaHypo.TrigEgammaFastCaloHypoTool import TrigEgammaFastCaloHypoAlgCfg
    l2CaloHypo = TrigEgammaFastCaloHypoAlgCfg(flags,
                                              name='PhotonEgammaFastCaloHypo',
                                              CaloClusters=recordable('HLT_FastCaloEMClusters'))
    selAcc.mergeHypo(l2CaloHypo)

    fastCaloSequence = MenuSequenceCA(selAcc,
                                      HypoToolGen=TrigEgammaFastCaloHypoToolFromDict)

    return (selAcc , fastCaloSequence)

# TODO reuse electron calo setup (these two could share all algorithms)
def _fastCalo(flags, chainDict):
    
    selAcc , fastCaloSequence = _fastCaloSeq(flags)

    return ChainStep(name=selAcc.name, Sequences=[fastCaloSequence], chainDicts=[chainDict], multiplicity=getChainMultFromDict(chainDict))

@AccumulatorCache
def _fastPhotonSeq(flags):
    selAcc=SelectionCA('FastPhoton')
    selAcc.mergeReco(l2PhotonRecoCfg(flags))

    l2PhotonHypo = l2PhotonHypoCfg( flags,
                                    Photons = 'HLT_FastPhotons',
                                    RunInView = True )
    selAcc.addHypoAlgo(l2PhotonHypo)

    l2PhotonSequence = MenuSequenceCA(selAcc,
                                      HypoToolGen = TrigEgammaFastPhotonHypoToolFromDict)

    return (selAcc , l2PhotonSequence)

def _fastPhoton(flags, chainDict):
    
    selAcc , l2PhotonSequence = _fastPhotonSeq(flags)
    
    return ChainStep(selAcc.name, Sequences=[l2PhotonSequence], chainDicts=[chainDict],  multiplicity=getChainMultFromDict(chainDict) )


def generateChains(flags, chainDict):

    l1Thresholds=[]
    for part in chainDict['chainParts']:
        l1Thresholds.append(part['L1threshold'])

    log.debug('dictionary is: %s\n', pprint.pformat(chainDict))

    chain = Chain(chainDict['chainName'], L1Thresholds=l1Thresholds, ChainSteps=[_fastCalo(flags, chainDict), _fastPhoton(flags, chainDict)])
    return chain
