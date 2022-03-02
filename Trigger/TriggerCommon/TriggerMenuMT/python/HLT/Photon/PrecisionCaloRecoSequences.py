#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.Logging    import logging
from egammaAlgs              import egammaAlgsConf
from egammaRec.Factories     import AlgFactory
from AthenaCommon.CFElements import parOR
from TriggerMenuMT.HLT.Config.MenuComponents        import RecoFragmentsPool
from TriggerMenuMT.HLT.Egamma.TrigEgammaKeys      import getTrigEgammaKeys
from TriggerMenuMT.HLT.Egamma.TrigEgammaFactories import TrigEgammaRec, TrigEgammaSuperClusterBuilder 


log = logging.getLogger(__name__)


def precisionCaloRecoSequence(flags, RoIs, ion=False):

    TrigEgammaKeys = getTrigEgammaKeys(ion=ion)

    log.debug('flags = %s',flags)
    log.debug('RoIs = %s',RoIs)

    from TrigT2CaloCommon.CaloDef import HLTRoITopoRecoSequence, HLTHIRoITopoRecoSequence
    topoRecoSequence = HLTHIRoITopoRecoSequence if ion is True else HLTRoITopoRecoSequence
    (caloRecoSequence, caloclusters) = RecoFragmentsPool.retrieve(topoRecoSequence, flags, RoIs=RoIs, algSuffix='')

    log.debug('Photon topoRecoSequence output container = %s',caloclusters)
    tag = 'HI' if ion is True else '' 
    egammaTopoClusterCopier = AlgFactory( egammaAlgsConf.egammaTopoClusterCopier,
                                          name = 'gTrigEgammaTopoClusterCopier' + tag + RoIs ,
                                          InputTopoCollection = caloclusters,
                                          OutputTopoCollection = TrigEgammaKeys.precisionCaloTopoCollection,
                                          OutputTopoCollectionShallow = "tmp_" + TrigEgammaKeys.precisionCaloTopoCollection,
                                          doAdd = False )

    algo = egammaTopoClusterCopier()
    precisionRecoSequence = parOR("photonRoITopoRecoSequence"+tag)
    precisionRecoSequence += caloRecoSequence
    precisionRecoSequence += algo
    trigEgammaRec = TrigEgammaRec(name = 'gTrigEgammaRec' + tag + RoIs)
    trigEgammaRec.InputClusterContainerName = TrigEgammaKeys.precisionCaloTopoCollection # input
    trigEgammaRec.egammaRecContainer        = TrigEgammaKeys.precisionCaloEgammaRecCollection # output
    precisionRecoSequence += trigEgammaRec
    
    outputCaloClusters = TrigEgammaKeys.precisionPhotonCaloClusterContainer
    log.debug('precisionOutputCaloClusters = %s',outputCaloClusters)



    trigEgammaSuperClusterBuilder = TrigEgammaSuperClusterBuilder(name = 'gTrigEgammaSuperClusterBuilder' + tag + RoIs )
    trigEgammaSuperClusterBuilder.InputEgammaRecContainerName  = TrigEgammaKeys.precisionCaloEgammaRecCollection # input
    trigEgammaSuperClusterBuilder.SuperClusterCollectionName   = outputCaloClusters # output
    trigEgammaSuperClusterBuilder.SuperegammaRecCollectionName = TrigEgammaKeys.precisionPhotonSuperClusterCollection # output
    trigEgammaSuperClusterBuilder.CalibrationType = 'photon'
    precisionRecoSequence +=  trigEgammaSuperClusterBuilder

    sequenceOut = outputCaloClusters

    return (precisionRecoSequence, sequenceOut)



