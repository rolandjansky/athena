# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from TriggerMenuMT.HLTMenuConfig.Electron.ElectronRecoSequences import l2CaloRecoCfg, l2CaloHypoCfg
from TriggerMenuMT.HLTMenuConfig.Photon.PhotonRecoSequences import l2PhotonRecoCfg, l2PhotonHypoCfg
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequenceCA, SelectionCA, ChainStep, Chain
from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import getChainMultFromDict
from TrigEDMConfig.TriggerEDMRun3 import recordable
from TrigEgammaHypo.TrigEgammaFastCaloHypoTool import TrigEgammaFastCaloHypoToolFromDict
from TrigEgammaHypo.TrigEgammaFastPhotonHypoTool import TrigEgammaFastPhotonHypoToolFromDict

import pprint
from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenuMT.HLTMenuConfig.Photon.generatePhoton' )

# TODO reuse electron calo setup (these two could share all algorithms)
def _fastCalo(flags, chainDict):
    selAcc=SelectionCA('FastCaloPhoton')
    selAcc.mergeReco(l2CaloRecoCfg(flags))

     # this alg needs EventInfo decorated with the  pileup info
    from LumiBlockComps.LumiBlockMuWriterConfig import LumiBlockMuWriterCfg
    selAcc.merge(LumiBlockMuWriterCfg(flags))

    l2CaloHypo = l2CaloHypoCfg(flags,
                               name='L2PhotonCaloHypo',
                               CaloClusters=recordable('HLT_FastCaloEMClusters'))

    selAcc.addHypoAlgo(l2CaloHypo)

    fastCaloSequence = MenuSequenceCA(selAcc,
                                      HypoToolGen=TrigEgammaFastCaloHypoToolFromDict)

    return ChainStep(name=selAcc.name, Sequences=[fastCaloSequence], chainDicts=[chainDict], multiplicity=getChainMultFromDict(chainDict))


def _fastPhoton(flags, chainDict):
    selAcc=SelectionCA('FastPhoton')
    selAcc.mergeReco(l2PhotonRecoCfg(flags))

    l2PhotonHypo = l2PhotonHypoCfg( flags,
                                    Photons = 'HLT_FastPhotons',
                                    RunInView = True )
    selAcc.addHypoAlgo(l2PhotonHypo)

    l2PhotonSequence = MenuSequenceCA(selAcc,
                                      HypoToolGen = TrigEgammaFastPhotonHypoToolFromDict)

    return ChainStep(selAcc.name, Sequences=[l2PhotonSequence], chainDicts=[chainDict],  multiplicity=getChainMultFromDict(chainDict) )


def generateChains(flags, chainDict):

    l1Thresholds=[]
    for part in chainDict['chainParts']:
        l1Thresholds.append(part['L1threshold'])

    log.debug('dictionary is: %s\n', pprint.pformat(chainDict))

    chain = Chain(chainDict['chainName'], L1Thresholds=l1Thresholds, ChainSteps=[_fastCalo(flags, chainDict), _fastPhoton(flags, chainDict)])
    return chain
