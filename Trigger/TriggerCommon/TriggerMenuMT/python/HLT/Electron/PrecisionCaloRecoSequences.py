#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from egammaAlgs import egammaAlgsConf
from egammaRec.Factories import AlgFactory
from AthenaCommon.CFElements import parOR
from TriggerMenuMT.HLT.Menu.MenuComponents import RecoFragmentsPool
from TriggerMenuMT.HLT.Egamma.TrigEgammaFactories import TrigEgammaRec, TrigEgammaSuperClusterBuilder 
from AthenaCommon.Logging import logging

log = logging.getLogger(__name__)

# Dummy flag arg needed so that each reco sequence is held separately
# in the RecoFragmentsPool -- only the RoIs are used to distinguish
# different sequences. New convention is just to pass "None" for flags
# taken from Jet/JetRecoSequences.py
def precisionCaloRecoSequence(flags, RoIs, ion=False, variant=''):
    log.debug('flags = %s',flags)
    log.debug('RoIs = %s',RoIs)

    from TriggerMenuMT.HLT.Egamma.TrigEgammaKeys import  getTrigEgammaKeys
    TrigEgammaKeys = getTrigEgammaKeys(variant, ion=ion)

    from TrigT2CaloCommon.CaloDef import HLTRoITopoRecoSequence, HLTHIRoITopoRecoSequence
    topoRecoSequence = HLTHIRoITopoRecoSequence if ion is True else HLTRoITopoRecoSequence
    (caloRecoSequence, caloclusters) = RecoFragmentsPool.retrieve(topoRecoSequence, flags, RoIs=RoIs, algSuffix=variant)

    log.debug('topoRecoSequence output conmtainer = %s',caloclusters)
    tag = 'HI' if ion is True else '' 

    egammaTopoClusterCopier = AlgFactory( egammaAlgsConf.egammaTopoClusterCopier,
                                          name = 'eTrigEgammaTopoClusterCopier' + tag + RoIs + variant ,
                                          InputTopoCollection = caloclusters,
                                          OutputTopoCollection = TrigEgammaKeys.precisionCaloTopoCollection,
                                          OutputTopoCollectionShallow = "tmp_" + TrigEgammaKeys.precisionCaloTopoCollection,
                                          doAdd = False )

    outputCaloClusters = TrigEgammaKeys.precisionCaloClusterContainer
    log.debug('precisionOutputCaloClusters = %s',outputCaloClusters)

    algo = egammaTopoClusterCopier()
    precisionRecoSequence = parOR( "electronRoITopoRecoSequence"+tag + variant)
    precisionRecoSequence += caloRecoSequence
    precisionRecoSequence += algo
    trigEgammaRec = TrigEgammaRec(name = 'eTrigEgammaRec' + tag + RoIs + variant)
    precisionRecoSequence += trigEgammaRec
    trigEgammaSuperClusterBuilder = TrigEgammaSuperClusterBuilder(name = 'eTrigEgammaSuperClusterBuilder' + tag + RoIs + variant)
    trigEgammaSuperClusterBuilder.SuperClusterCollectionName = outputCaloClusters
    trigEgammaSuperClusterBuilder.CalibrationType = 'electron'
    precisionRecoSequence +=  trigEgammaSuperClusterBuilder
    sequenceOut = outputCaloClusters

    return (precisionRecoSequence, sequenceOut)

