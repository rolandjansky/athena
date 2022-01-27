#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CFElements import parOR
from TriggerMenuMT.HLT.Egamma.TrigEgammaKeys import getTrigEgammaKeys

#logging
from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)



def precisionPhotonRecoSequence(RoIs, ion=False):
    """ With this function we will setup the sequence of offline EgammaAlgorithms so to make a photon for TrigEgamma 

    Sequence of algorithms is the following:
      - egammaRecBuilder/TrigEgammaRecPhoton creates egammaObjects out of clusters and tracks. Here, at HLT photons we will only use clusters. 
      - photonSuperClusterBuilder algorithm will create superclusters out of the toposlusters and tracks in egammaRec under the photon hypothesis
          https://gitlab.cern.ch/atlas/athena/blob/master/Reconstruction/egamma/egammaAlgs/python/egammaSuperClusterBuilder.py#L26 
      - TopoEgammBuilder will create photons and electrons out of trakcs and SuperClusters. Here at HLT photons the aim is to ignore electrons and not use tracks at all.
          https://gitlab.cern.ch/atlas/athena/blob/master/Reconstruction/egamma/egammaAlgs/src/topoEgammaBuilder.cxx
    """

    TrigEgammaKeys = getTrigEgammaKeys(ion=ion)

    log.debug('precisionPhotonRecoSequence(RoIs = %s)',RoIs)

    tag = '_ion' if ion is True else ''

    # First the data verifiers:
    # Here we define the data dependencies. What input needs to be available for the Fexs (i.e. TopoClusters from precisionCalo) in order to run
    import AthenaCommon.CfgMgr as CfgMgr

    caloClusters = TrigEgammaKeys.precisionCaloClusterContainer 

    ViewVerify = CfgMgr.AthViews__ViewDataVerifier("PrecisionPhotonPhotonViewDataVerifier" + tag)
    ViewVerify.DataObjects = [( 'xAOD::CaloClusterContainer' , 'StoreGateSvc+%s' % caloClusters ),
                              ( 'xAOD::CaloClusterContainer' , 'StoreGateSvc+%s' % TrigEgammaKeys.precisionTopoClusterContainer), # this is for the calo isolation tool 
                              ( 'EgammaRecContainer', 'StoreGateSvc+%s' % TrigEgammaKeys.precisionPhotonSuperClusterCollection),
                              ( 'CaloCellContainer' , 'StoreGateSvc+CaloCells' ),
                              ( 'xAOD::EventInfo' , 'StoreGateSvc+EventInfo' )]
                              
    if ion is True: ViewVerify.DataObjects.append(( 'CaloCellContainer' , 'StoreGateSvc+CorrectedRoICaloCells' ))

    # Retrieve the factories now
    from TriggerMenuMT.HLT.Photon.TrigPhotonFactories import TrigTopoEgammaPhotons_HI, TrigTopoEgammaPhotons

    log.debug('retrieve(precisionPhotonRecoSequence,None,RoIs = %s)',RoIs)


    # The sequence of these algorithms
    thesequence = parOR( "precisionPhotonAlgs" + tag) # This thing creates the sequence with name precisionPhotonAlgs
    thesequence += ViewVerify


    egammaPhotonAlgo = TrigTopoEgammaPhotons_HI if ion is True else TrigTopoEgammaPhotons
    trigTopoEgammaAlgo = egammaPhotonAlgo('TrigTopoEgammaPhotons' +tag)
    trigTopoEgammaAlgo.InputPhotonRecCollectionName = TrigEgammaKeys.precisionPhotonSuperClusterCollection # input from precision calo
    trigTopoEgammaAlgo.PhotonOutputName             = TrigEgammaKeys.precisionPhotonContainer # output
    thesequence += trigTopoEgammaAlgo

    collectionOut = trigTopoEgammaAlgo.PhotonOutputName

    # Add CaloIsolationTool
    from TriggerMenuMT.HLT.Egamma.TrigEgammaFactories import TrigPhotonIsoBuilderCfg, TrigPhotonIsoBuilderHICfg
    isoBuilder = TrigPhotonIsoBuilderCfg('TrigPhotonIsolationBuilder' + tag)
    isoBuilderHI = TrigPhotonIsoBuilderHICfg('TrigPhotonIsolationBuilderHI' + tag)
    thesequence += isoBuilderHI if ion is True else isoBuilder

    #online monitoring for topoEgammaBuilder
    from TriggerMenuMT.HLT.Photon.TrigPhotonFactories import PrecisionPhotonTopoMonitorCfg
    PrecisionPhotonRecoMonAlgo = PrecisionPhotonTopoMonitorCfg('PrecisionPhotonTopoEgammaBuilder' + tag)
    PrecisionPhotonRecoMonAlgo.PhotonKey = trigTopoEgammaAlgo.PhotonOutputName
    PrecisionPhotonRecoMonAlgo.IsoVarKeys = [ '%s.topoetcone20' % trigTopoEgammaAlgo.PhotonOutputName, '%s.topoetcone40' % trigTopoEgammaAlgo.PhotonOutputName]
    thesequence += PrecisionPhotonRecoMonAlgo

    #online monitoring for TrigPhotonSuperClusterBuilder
    from TriggerMenuMT.HLT.Photon.TrigPhotonFactories import PrecisionPhotonSuperClusterMonitorCfg
    PrecisionPhotonSuperClusterMonAlgo = PrecisionPhotonSuperClusterMonitorCfg('PrecisionPhotonSuperClusterBuilder' + tag)
    PrecisionPhotonSuperClusterMonAlgo.InputEgammaRecContainerName = TrigEgammaKeys.precisionPhotonSuperClusterCollection
    thesequence += PrecisionPhotonSuperClusterMonAlgo

    return (thesequence, collectionOut)





