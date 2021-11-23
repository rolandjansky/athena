#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CFElements import parOR
from TriggerMenuMT.HLTMenuConfig.Egamma.TrigEgammaKeys import getTrigEgammaKeys

#logging
from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)

TrigEgammaKeys = getTrigEgammaKeys()


def precisionPhotonRecoSequence(RoIs, ion=False):
    """ With this function we will setup the sequence of offline EgammaAlgorithms so to make a photon for TrigEgamma 

    Sequence of algorithms is the following:
      - egammaRecBuilder/TrigEgammaRecPhoton creates egammaObjects out of clusters and tracks. Here, at HLT photons we will only use clusters. 
      - photonSuperClusterBuilder algorithm will create superclusters out of the toposlusters and tracks in egammaRec under the photon hypothesis
          https://gitlab.cern.ch/atlas/athena/blob/master/Reconstruction/egamma/egammaAlgs/python/egammaSuperClusterBuilder.py#L26 
      - TopoEgammBuilder will create photons and electrons out of trakcs and SuperClusters. Here at HLT photons the aim is to ignore electrons and not use tracks at all.
          https://gitlab.cern.ch/atlas/athena/blob/master/Reconstruction/egamma/egammaAlgs/src/topoEgammaBuilder.cxx
    """


    log.debug('precisionPhotonRecoSequence(RoIs = %s)',RoIs)

    tag = '_ion' if ion is True else ''

    # First the data verifiers:
    # Here we define the data dependencies. What input needs to be available for the Fexs (i.e. TopoClusters from precisionCalo) in order to run
    import AthenaCommon.CfgMgr as CfgMgr

    caloClusters = TrigEgammaKeys.precisionCaloClusterContainer if not ion else TrigEgammaKeys.precisionHICaloClusterContainer

    ViewVerify = CfgMgr.AthViews__ViewDataVerifier("PrecisionPhotonPhotonViewDataVerifier" + tag)
    ViewVerify.DataObjects = [( 'xAOD::CaloClusterContainer' , 'StoreGateSvc+%s' % caloClusters ),
                              ( 'CaloCellContainer' , 'StoreGateSvc+CaloCells' )]
    if ion is True: ViewVerify.DataObjects.append(( 'CaloCellContainer' , 'StoreGateSvc+CorrectedRoICaloCells' ))

    # Retrieve the factories now
    from TriggerMenuMT.HLTMenuConfig.Photon.TrigPhotonFactories import TrigEgammaRecPhoton, TrigPhotonSuperClusterBuilder, TrigTopoEgammaPhotons_HI, TrigTopoEgammaPhotons

    log.debug('retrieve(precisionPhotonRecoSequence,None,RoIs = %s)',RoIs)


    # The sequence of these algorithms
    thesequence = parOR( "precisionPhotonAlgs" + tag) # This thing creates the sequence with name precisionPhotonAlgs
    thesequence += ViewVerify

    # Add to the sequence the three steps:
    #  - TrigEgammaBuilder, TrigPhotonSuperClusters, TrigTopoEgammaPhotons
    TrigEgammaAlgo = TrigEgammaRecPhoton('TrigEgammaRecPhoton'+tag)
    TrigEgammaAlgo.InputClusterContainerName = caloClusters
    thesequence += TrigEgammaAlgo

    trigPhotonAlgo = TrigPhotonSuperClusterBuilder('TrigPhotonSuperClusterBuilder'+tag)
    trigPhotonAlgo.InputEgammaRecContainerName = TrigEgammaAlgo.egammaRecContainer
    thesequence += trigPhotonAlgo

    egammaPhotonAlgo = TrigTopoEgammaPhotons_HI if ion is True else TrigTopoEgammaPhotons

    trigTopoEgammaAlgo = egammaPhotonAlgo('TrigTopoEgammaPhotons' +tag)
    trigTopoEgammaAlgo.InputPhotonRecCollectionName = trigPhotonAlgo.SuperPhotonRecCollectionName
    collectionOut = trigTopoEgammaAlgo.PhotonOutputName
    thesequence += trigTopoEgammaAlgo

    # Add CaloIsolationTool
    from TriggerMenuMT.HLTMenuConfig.Egamma.TrigEgammaFactories import TrigPhotonIsoBuilderCfg
    isoBuilder = TrigPhotonIsoBuilderCfg('TrigPhotonIsolationBuilder' + tag)
    thesequence += isoBuilder

    #online monitoring for topoEgammaBuilder
    from TriggerMenuMT.HLTMenuConfig.Photon.TrigPhotonFactories import PrecisionPhotonTopoMonitorCfg
    PrecisionPhotonRecoMonAlgo = PrecisionPhotonTopoMonitorCfg('PrecisionPhotonTopoEgammaBuilder' + tag)
    PrecisionPhotonRecoMonAlgo.PhotonKey = trigTopoEgammaAlgo.PhotonOutputName
    PrecisionPhotonRecoMonAlgo.IsoVarKeys = [ '%s.topoetcone20' % trigTopoEgammaAlgo.PhotonOutputName, '%s.topoetcone40' % trigTopoEgammaAlgo.PhotonOutputName]
    thesequence += PrecisionPhotonRecoMonAlgo

    #online monitoring for TrigPhotonSuperClusterBuilder
    from TriggerMenuMT.HLTMenuConfig.Photon.TrigPhotonFactories import PrecisionPhotonSuperClusterMonitorCfg
    PrecisionPhotonSuperClusterMonAlgo = PrecisionPhotonSuperClusterMonitorCfg('PrecisionPhotonSuperClusterBuilder' + tag)
    PrecisionPhotonSuperClusterMonAlgo.InputEgammaRecContainerName = trigPhotonAlgo.SuperPhotonRecCollectionName
    thesequence += PrecisionPhotonSuperClusterMonAlgo

    return (thesequence, collectionOut)





