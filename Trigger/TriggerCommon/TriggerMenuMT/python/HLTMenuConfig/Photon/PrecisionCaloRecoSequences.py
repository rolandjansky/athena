#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.Logging    import logging
from egammaAlgs              import egammaAlgsConf
from egammaRec.Factories     import AlgFactory
from AthenaCommon.CFElements import parOR
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents        import RecoFragmentsPool
from TriggerMenuMT.HLTMenuConfig.Egamma.TrigEgammaKeys      import getTrigEgammaKeys
from TriggerMenuMT.HLTMenuConfig.Egamma.TrigEgammaFactories import TrigEgammaRec, TrigEgammaSuperClusterBuilder 


log = logging.getLogger(__name__)


def precisionCaloRecoSequence(DummyFlag, RoIs, ion=False):

    TrigEgammaKeys = getTrigEgammaKeys(ion=ion)

    log.debug('DummyFlag = %s',str(DummyFlag))
    log.debug('RoIs = %s',RoIs)

    from TrigT2CaloCommon.CaloDef import HLTRoITopoRecoSequence, HLTHIRoITopoRecoSequence
    topoRecoSequence = HLTHIRoITopoRecoSequence if ion is True else HLTRoITopoRecoSequence
    (caloRecoSequence, caloclusters) = RecoFragmentsPool.retrieve(topoRecoSequence, None, RoIs=RoIs, algSuffix='')

    tag = 'HI' if ion is True else '' 
    outputCaloClusters = TrigEgammaKeys.precisionCaloClusterContainer
    log.debug('precisionOutputCaloClusters = %s',outputCaloClusters)

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
    precisionRecoSequence += trigEgammaRec
    trigEgammaSuperClusterBuilder = TrigEgammaSuperClusterBuilder(name = 'gTrigEgammaSuperClusterBuilder' + tag + RoIs )
    trigEgammaSuperClusterBuilder.SuperClusterCollectionName = outputCaloClusters
    trigEgammaSuperClusterBuilder.CalibrationType = 'photon'
    precisionRecoSequence +=  trigEgammaSuperClusterBuilder
    sequenceOut = outputCaloClusters

    return (precisionRecoSequence, sequenceOut)



