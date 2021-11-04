#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from egammaAlgs import egammaAlgsConf
from egammaRec.Factories import AlgFactory
from AthenaCommon.CFElements import parOR
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import RecoFragmentsPool
from .PrecisionCaloMenuSequences import precisionCaloMenuDefs
from .PrecisionCaloMenuSequences_LRT import precisionCaloMenuDefs_LRT
from .PrecisionCaloMenuSequences_FWD import precisionCaloMenuDefs_FWD
from TriggerMenuMT.HLTMenuConfig.Egamma.TrigEgammaKeys import getTrigEgammaKeys
from TriggerMenuMT.HLTMenuConfig.Egamma.TrigEgammaFactories import TrigEgammaRec, TrigEgammaSuperClusterBuilder 
from AthenaCommon.Logging import logging

TrigEgammaKeys = getTrigEgammaKeys()

log = logging.getLogger(__name__)



# Dummy flag arg needed so that each reco sequence is held separately
# in the RecoFragmentsPool -- only the RoIs are used to distinguish
# different sequences. New convention is just to pass "None" for flags
# taken from Jet/JetRecoSequences.py
def precisionCaloRecoSequence(DummyFlag, RoIs, ion=False, is_photon=False):
    log.debug('DummyFlag = %s',str(DummyFlag))
    log.debug('RoIs = %s',RoIs)

    from TrigT2CaloCommon.CaloDef import HLTRoITopoRecoSequence, HLTHIRoITopoRecoSequence
    topoRecoSequence = HLTHIRoITopoRecoSequence if ion is True else HLTRoITopoRecoSequence
    (caloRecoSequence, caloclusters) = RecoFragmentsPool.retrieve(topoRecoSequence, None, RoIs=RoIs, algSuffix='')

    tag = 'HI' if ion is True else '' 
    outputCaloClusters = precisionCaloMenuDefs.caloClusters(ion)
    log.debug('precisionOutputCaloClusters = %s',outputCaloClusters)

    egammaTopoClusterCopier = AlgFactory( egammaAlgsConf.egammaTopoClusterCopier,
                                          name = ('g' if is_photon else 'e') + 'TrigEgammaTopoClusterCopier' + tag + RoIs ,
                                          InputTopoCollection = caloclusters,
                                          OutputTopoCollection = TrigEgammaKeys.outputTopoCollection,
                                          OutputTopoCollectionShallow = "tmp_" + TrigEgammaKeys.outputTopoCollection,
                                          doAdd = False )

    algo = egammaTopoClusterCopier()
    precisionRecoSequence = parOR( ('photon' if is_photon else 'electron')+"RoITopoRecoSequence"+tag)
    precisionRecoSequence += caloRecoSequence
    precisionRecoSequence += algo
    _trigEgammaRec = TrigEgammaRec(name = ('g' if is_photon else 'e') + 'TrigEgammaRec%s' % RoIs)
    precisionRecoSequence += _trigEgammaRec
    _trigEgammaSuperClusterBuilder = TrigEgammaSuperClusterBuilder(name = ('g' if is_photon else 'e') + 'TrigEgammaSuperClusterBuilder%s' % RoIs)
    _trigEgammaSuperClusterBuilder.SuperClusterCollectionName = outputCaloClusters
    _trigEgammaSuperClusterBuilder.CalibrationType = 'photon' if is_photon else 'electron'
    precisionRecoSequence +=  _trigEgammaSuperClusterBuilder
    sequenceOut = outputCaloClusters

    return (precisionRecoSequence, sequenceOut)


def precisionCaloRecoSequence_LRT(DummyFlag, RoIs):
    log.info('DummyFlag_LRT = %s',str(DummyFlag))
    log.info('RoIs_LRT = %s',RoIs)

    egammaTopoClusterCopier = AlgFactory( egammaAlgsConf.egammaTopoClusterCopier,
                                          name = 'eTrigEgammaTopoClusterCopier%s' % RoIs ,
                                          InputTopoCollection= "caloclusters",
                                          OutputTopoCollection=TrigEgammaKeys.outputTopoCollection,
                                          OutputTopoCollectionShallow="tmp_"+TrigEgammaKeys.outputTopoCollection,
                                          doAdd = False )

    from TrigT2CaloCommon.CaloDef import HLTRoITopoRecoSequence
    (caloRecoSequence, caloclusters) = RecoFragmentsPool.retrieve(HLTRoITopoRecoSequence, None, RoIs=RoIs,algSuffix='_LRT')
    precisionRecoSequence = parOR( "electronRoITopoRecoSequence_LRT")
    precisionRecoSequence += caloRecoSequence
    algo = egammaTopoClusterCopier()
    algo.InputTopoCollection = caloclusters
    precisionRecoSequence += algo
    _trigEgammaRec = TrigEgammaRec(name ='eTrigEgammaRec%s' % RoIs)
    precisionRecoSequence += _trigEgammaRec
    _trigEgammaSuperClusterBuilder = TrigEgammaSuperClusterBuilder('eTrigEgammaSuperClusterBuilder%s' % RoIs)
    _trigEgammaSuperClusterBuilder.SuperClusterCollectionName = precisionCaloMenuDefs_LRT.precisionCaloClusters
    _trigEgammaSuperClusterBuilder.CalibrationType = 'electron'
    precisionRecoSequence += _trigEgammaSuperClusterBuilder
    sequenceOut = precisionCaloMenuDefs_LRT.precisionCaloClusters

    return (precisionRecoSequence, sequenceOut)




def precisionCaloRecoSequence_FWD(DummyFlag, RoIs):
    log.info('DummyFlag_FWD = %s',str(DummyFlag))
    log.info('RoIs_FWD = %s',RoIs)

    egammaTopoClusterCopier = AlgFactory( egammaAlgsConf.egammaTopoClusterCopier,
                                          name = 'eTrigEgammaTopoClusterCopier%s' % RoIs ,
                                          InputTopoCollection= "caloclusters",
                                          OutputTopoCollection=TrigEgammaKeys.outputTopoCollection,
                                          OutputTopoCollectionShallow="tmp_"+TrigEgammaKeys.outputTopoCollection,
                                          doAdd = False )

    from TrigT2CaloCommon.CaloDef import HLTRoITopoRecoSequence
    (caloRecoSequence, caloclusters) = RecoFragmentsPool.retrieve(HLTRoITopoRecoSequence, None, RoIs=RoIs,algSuffix='_FWD')

    precisionRecoSequence = parOR( "electronRoITopoRecoSequence_FWD")
    precisionRecoSequence += caloRecoSequence
    algo = egammaTopoClusterCopier()
    algo.InputTopoCollection = caloclusters
    precisionRecoSequence += algo
    _trigEgammaRec = TrigEgammaRec(name ='eTrigEgammaRec%s' % RoIs)
    precisionRecoSequence +=  _trigEgammaRec
    _trigEgammaSuperClusterBuilder = TrigEgammaSuperClusterBuilder(name ='eTrigEgammaSuperClusterBuilder%s' % RoIs)
    _trigEgammaSuperClusterBuilder.SuperClusterCollectionName = precisionCaloMenuDefs_FWD.precisionCaloClusters
    _trigEgammaSuperClusterBuilder.CalibrationType = 'electron'
    precisionRecoSequence += _trigEgammaSuperClusterBuilder
    sequenceOut = precisionCaloMenuDefs_FWD.precisionCaloClusters

    return (precisionRecoSequence, sequenceOut)


def egammaFSCaloRecoSequence():
    from TrigT2CaloCommon.CaloDef import setMinimalCaloSetup
    setMinimalCaloSetup()

    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    from HLTSeeding.HLTSeedingConfig import mapThresholdToL1RoICollection
    from TrigCaloRec.TrigCaloRecConfig import HLTCaloCellMaker

    cellMaker = HLTCaloCellMaker('HLTCaloCellMakerEGFS')
    cellMaker.RoIs = mapThresholdToL1RoICollection('FSNOSEED')
    cellMaker.TrigDataAccessMT = svcMgr.TrigCaloDataAccessSvc
    cellMaker.CellsName = 'CaloCellsEGFS'

    from TrigT2CaloCommon.CaloDef import _algoHLTHIEventShape
    eventShapeMaker = _algoHLTHIEventShape(
        name='HLTEventShapeMakerEG',
        inputEDM=cellMaker.CellsName,
        outputEDM=precisionCaloMenuDefs.egEventShape
    )

    return parOR("egammaFSRecoSequence", [cellMaker, eventShapeMaker])
