#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CFElements import parOR

#logging
from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)

def precisionPhotonRecoSequence(RoIs):
    """ With this function we will setup the sequence of offline EgammaAlgorithms so to make a photon for TrigEgamma 

    Sequence of algorithms is the following:
      - egammaRecBuilder/TrigEgammaRecPhoton creates egammaObjects out of clusters and tracks. Here, at HLT photons we will only use clusters. 
      - photonSuperClusterBuilder algorithm will create superclusters out of the toposlusters and tracks in egammaRec under the photon hypothesis
          https://gitlab.cern.ch/atlas/athena/blob/master/Reconstruction/egamma/egammaAlgs/python/egammaSuperClusterBuilder.py#L26 
      - TopoEgammBuilder will create photons and electrons out of trakcs and SuperClusters. Here at HLT photons the aim is to ignore electrons and not use tracks at all.
          https://gitlab.cern.ch/atlas/athena/blob/master/Reconstruction/egamma/egammaAlgs/src/topoEgammaBuilder.cxx
    """


    log.debug('precisionPhotonRecoSequence(RoIs = %s)',RoIs)

    # First the data verifiers:
    # Here we define the data dependencies. What input needs to be available for the Fexs (i.e. TopoClusters from precisionCalo) in order to run
    from TriggerMenuMT.HLTMenuConfig.Egamma.PrecisionCaloMenuSequences import precisionCaloMenuDefs
    import AthenaCommon.CfgMgr as CfgMgr
    ViewVerify = CfgMgr.AthViews__ViewDataVerifier("PrecisionPhotonPhotonViewDataVerifier")
    ViewVerify.DataObjects = [( 'xAOD::CaloClusterContainer' , 'StoreGateSvc+%s' % precisionCaloMenuDefs.precisionCaloClusters ),
                              ( 'CaloCellContainer' , 'StoreGateSvc+CaloCells' )]


    # Retrieve the factories now
    from TriggerMenuMT.HLTMenuConfig.Photon.TrigPhotonFactories import TrigEgammaRecPhoton, TrigPhotonSuperClusterBuilder, TrigTopoEgammaPhotons

    log.debug('retrieve(precisionPhotonRecoSequence,None,RoIs = %s)',RoIs)

    # The sequence of these algorithms
    thesequence = parOR( "precisionPhotonAlgs") # This thing creates the sequence with name precisionPhotonAlgs
    thesequence += ViewVerify

    # Add to the sequence the three steps:
    #  - TrigEgammaBuilder, TrigPhotonSuperClusters, TrigTopoEgammaPhotons
    TrigEgammaAlgo = TrigEgammaRecPhoton()
    TrigEgammaAlgo.InputClusterContainerName = precisionCaloMenuDefs.precisionCaloClusters
    thesequence += TrigEgammaAlgo

    trigPhotonAlgo = TrigPhotonSuperClusterBuilder()
    trigPhotonAlgo.InputEgammaRecContainerName = TrigEgammaAlgo.egammaRecContainer
    thesequence += trigPhotonAlgo

    trigTopoEgammaAlgo = TrigTopoEgammaPhotons()
    trigTopoEgammaAlgo.SuperPhotonRecCollectionName = trigPhotonAlgo.SuperPhotonRecCollectionName
    collectionOut = trigTopoEgammaAlgo.PhotonOutputName
    thesequence += trigTopoEgammaAlgo

    # Add CaloIsolationTool
    from TriggerMenuMT.HLTMenuConfig.Egamma.TrigEgammaFactories import TrigPhotonIsoBuilderCfg
    isoBuilder = TrigPhotonIsoBuilderCfg()
    thesequence += isoBuilder

    #online monitoring for topoEgammaBuilder
    from TriggerMenuMT.HLTMenuConfig.Photon.TrigPhotonFactories import PrecisionPhotonTopoMonitorCfg
    PrecisionPhotonRecoMonAlgo = PrecisionPhotonTopoMonitorCfg()
    PrecisionPhotonRecoMonAlgo.PhotonKey = trigTopoEgammaAlgo.PhotonOutputName
    PrecisionPhotonRecoMonAlgo.IsoVarKeys = [ '%s.topoetcone20' % trigTopoEgammaAlgo.PhotonOutputName, '%s.topoetcone40' % trigTopoEgammaAlgo.PhotonOutputName]
    thesequence += PrecisionPhotonRecoMonAlgo

    #online monitoring for TrigPhotonSuperClusterBuilder
    from TriggerMenuMT.HLTMenuConfig.Photon.TrigPhotonFactories import PrecisionPhotonSuperClusterMonitorCfg
    PrecisionPhotonSuperClusterMonAlgo = PrecisionPhotonSuperClusterMonitorCfg()
    PrecisionPhotonSuperClusterMonAlgo.InputEgammaRecContainerName = trigPhotonAlgo.SuperPhotonRecCollectionName
    thesequence += PrecisionPhotonSuperClusterMonAlgo

    return (thesequence, collectionOut)





