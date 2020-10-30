#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.CFElements import parOR

#logging
from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenuMT.HLTMenuConfig.Egamma.PrecisionElectronRecoSequences')

def precisionElectronRecoSequence(RoIs):
    """ With this function we will setup the sequence of offline EgammaAlgorithms so to make a electron for TrigEgamma 

    Sequence of algorithms is the following:
      - egammaRecBuilder/TrigEgammaRecElectron creates egammaObjects out of clusters and tracks. 
      - electronSuperClusterBuilder algorithm will create superclusters out of the toposlusters and tracks in egammaRec under the electron hypothesis
          https://gitlab.cern.ch/atlas/athena/blob/master/Reconstruction/egamma/egammaAlgs/python/egammaSuperClusterBuilder.py#L26 
      - TopoEgammBuilder will create photons and electrons out of trakcs and SuperClusters. Here at HLT electrons the aim is to ignore photons.
          https://gitlab.cern.ch/atlas/athena/blob/master/Reconstruction/egamma/egammaAlgs/src/topoEgammaBuilder.cxx
    """

    log.debug('precisionElectronRecoSequence(RoIs = %s)',RoIs)
    
    import AthenaCommon.CfgMgr as CfgMgr
    # First the data verifiers:
    # Here we define the data dependencies. What input needs to be available for the Fexs (i.e. TopoClusters from precisionCalo) in order to run
    from TriggerMenuMT.HLTMenuConfig.Egamma.PrecisionCaloSequenceSetup import precisionCaloMenuDefs
    
    # precision Tracking related data dependencies
    from TriggerMenuMT.HLTMenuConfig.Egamma.EgammaDefs import TrigEgammaKeys

    ViewVerifyTrk   = CfgMgr.AthViews__ViewDataVerifier("PrecisionTrackViewDataVerifier")

    ViewVerifyTrk.DataObjects = [( 'CaloCellContainer' , 'StoreGateSvc+CaloCells' ),
                                 ( 'xAOD::CaloClusterContainer' , 'StoreGateSvc+' + precisionCaloMenuDefs.precisionCaloClusters ),
                                 ( 'xAOD::TrackParticleContainer','StoreGateSvc+' + TrigEgammaKeys.TrigElectronTracksCollectionName)] 


    """ Retrieve the factories now """
    from TriggerMenuMT.HLTMenuConfig.Electron.TrigElectronFactories import TrigEgammaRecElectron, TrigElectronSuperClusterBuilder, TrigTopoEgammaElectronCfg
    from TriggerMenuMT.HLTMenuConfig.Egamma.TrigEgammaFactories import  TrigEMTrackMatchBuilder
   
    #The sequence of these algorithms
    thesequence = parOR( "precisionElectron_"+RoIs)
   
    thesequence += ViewVerifyTrk
    # Create the sequence of three steps:
    #  - TrigEgammaRecElectron, TrigElectronSuperClusterBuilder, TrigTopoEgammaElectron
    trackMatchBuilder = TrigEMTrackMatchBuilder()
    trackMatchBuilder.TrackParticlesName = TrigEgammaKeys.TrigElectronTracksCollectionName
    TrigEgammaAlgo = TrigEgammaRecElectron()
   
    TrigEgammaAlgo.InputTopoClusterContainerName = precisionCaloMenuDefs.precisionCaloClusters
    TrigEgammaAlgo.TrackMatchBuilderTool = trackMatchBuilder
    thesequence += TrigEgammaAlgo
        
    trigElectronAlgo = TrigElectronSuperClusterBuilder()
    
    trigElectronAlgo.InputEgammaRecContainerName = TrigEgammaAlgo.egammaRecContainer
    thesequence += trigElectronAlgo

    trigTopoEgammaAlgo = TrigTopoEgammaElectronCfg()
    trigTopoEgammaAlgo.SuperElectronRecCollectionName = trigElectronAlgo.SuperElectronRecCollectionName
    collectionOut = trigTopoEgammaAlgo.ElectronOutputName
    thesequence += trigTopoEgammaAlgo
    
    from TriggerMenuMT.HLTMenuConfig.Egamma.TrigEgammaFactories import TrigElectronIsoBuilderCfg
    isoBuilder = TrigElectronIsoBuilderCfg()
    thesequence += isoBuilder

    #online monitoring for topoEgammaBuilder
    from TriggerMenuMT.HLTMenuConfig.Electron.TrigElectronFactories import PrecisionElectronTopoMonitorCfg
    PrecisionElectronRecoMonAlgo = PrecisionElectronTopoMonitorCfg()
    PrecisionElectronRecoMonAlgo.ElectronKey = trigTopoEgammaAlgo.ElectronOutputName
    thesequence += PrecisionElectronRecoMonAlgo

    return (thesequence, collectionOut)
