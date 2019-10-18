#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from egammaAlgs import egammaAlgsConf
from egammaRec.Factories import AlgFactory
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import RecoFragmentsPool
from PrecisionCaloSequenceSetup import precisionCaloMenuDefs
# logger
from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenuMT.HLTMenuConfig.Egamma.PrecisionCaloRec' )

# Dummy flag arg needed so that each reco sequence is held separately
# in the RecoFragmentsPool -- only the RoIs are used to distinguish
# different sequences. New convention is just to pass "None" for flags
# taken from Jet/JetRecoSequences.py
def precisionCaloRecoSequence(DummyFlag, RoIs):
    log.debug('DummyFlag = %s',str(DummyFlag))
    log.debug('RoIs = %s',RoIs)

    egammaTopoClusterCopier = AlgFactory( egammaAlgsConf.egammaTopoClusterCopier,
                                          name = 'TrigEgammaTopoClusterCopier'+RoIs ,
                                          #InputTopoCollection=jobproperties.egammaRecFlags.inputTopoClusterCollection(),
                                          InputTopoCollection= "caloclusters",
                                          OutputTopoCollection=precisionCaloMenuDefs.precisionCaloClusters,
                                          #OutputTopoCollection="ClustersName",
                                          OutputTopoCollectionShallow="tmp_"+precisionCaloMenuDefs.precisionCaloClusters,
                                          doAdd = False )

    from TrigT2CaloCommon.CaloDef import HLTRoITopoRecoSequence
    (precisionRecoSequence, caloclusters) = RecoFragmentsPool.retrieve(HLTRoITopoRecoSequence, RoIs)

    algo = egammaTopoClusterCopier()
    algo.InputTopoCollection = caloclusters
    precisionRecoSequence += algo
    sequenceOut = algo.OutputTopoCollection

    return (precisionRecoSequence, sequenceOut)


