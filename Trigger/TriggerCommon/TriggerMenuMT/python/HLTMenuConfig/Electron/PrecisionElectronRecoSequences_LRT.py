#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.CFElements import parOR

#logging
from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)

def precisionElectronRecoSequence_LRT(RoIs):
    """ With this function we will setup the sequence of offline EgammaAlgorithms so to make a electron for TrigEgamma 

    Sequence of algorithms is the following:
      - egammaRecBuilder/TrigEgammaRecElectron creates egammaObjects out of clusters and tracks. 
      - electronSuperClusterBuilder algorithm will create superclusters out of the topoclusters and tracks in egammaRec under the electron hypothesis
          https://gitlab.cern.ch/atlas/athena/blob/master/Reconstruction/egamma/egammaAlgs/python/egammaSuperClusterBuilder.py#L26 
      - TopoEgammBuilder will create photons and electrons out of trakcs and SuperClusters. Here at HLT electrons the aim is to ignore photons.
          https://gitlab.cern.ch/atlas/athena/blob/master/Reconstruction/egamma/egammaAlgs/src/topoEgammaBuilder.cxx
    """

    log.debug('precisionElectronRecoSequence_LRT(RoIs = %s)',RoIs)
    
    import AthenaCommon.CfgMgr as CfgMgr
    # First the data verifiers:
    # Here we define the data dependencies. What input needs to be available for the Fexs (i.e. TopoClusters from precisionCalo) in order to run
    from TriggerMenuMT.HLTMenuConfig.Egamma.PrecisionCaloMenuSequences_LRT import precisionCaloMenuDefs_LRT
       
    # precision Tracking related data dependencies
    from TriggerMenuMT.HLTMenuConfig.Egamma.EgammaDefs import TrigEgammaKeys_LRT

    ViewVerifyTrk   = CfgMgr.AthViews__ViewDataVerifier("PrecisionTrackViewDataVerifier_LRT")

    ViewVerifyTrk.DataObjects = [( 'CaloCellContainer' , 'StoreGateSvc+CaloCells' ),
                                 ( 'xAOD::CaloClusterContainer' , 'StoreGateSvc+%s' % precisionCaloMenuDefs_LRT.precisionCaloClusters ),
                                 ( 'xAOD::TrackParticleContainer','StoreGateSvc+%s' % TrigEgammaKeys_LRT.TrigElectronTracksCollectionName_LRT)]


    """ Retrieve the factories now """
    from TriggerMenuMT.HLTMenuConfig.Electron.TrigElectronFactories import TrigEgammaRecElectron, TrigElectronSuperClusterBuilder, TrigTopoEgammaElectronCfg
    from TriggerMenuMT.HLTMenuConfig.Egamma.TrigEgammaFactories import  TrigEMTrackMatchBuilder, TrigElectronIsoBuilderCfg_LRT

    # Create the sequence of three steps:
    #  - TrigEgammaRecElectron, TrigElectronSuperClusterBuilder, TrigTopoEgammaElectron

    # Create the sequence of three steps:
    #  - TrigEgammaRecElectron, TrigElectronSuperClusterBuilder, TrigTopoEgammaElectron
    #The sequence of these algorithms
    thesequence = parOR( "precisionElectron_LRT%s" % RoIs)
    thesequence += ViewVerifyTrk
    
    ## TrigEMTrackMatchBuilder_LRT ##
    TrigEMTrackMatchBuilder = TrigEMTrackMatchBuilder("TrigEMTrackMatchBuilder_LRT")
    TrigEMTrackMatchBuilder.TrackParticlesName =  TrigEgammaKeys_LRT.TrigElectronTracksCollectionName_LRT

    ## TrigEgammaRecElectron_LRT ##
    TrigEgammaRecAlgo = TrigEgammaRecElectron("TrigEgammaRecElectron_LRT")
    thesequence += TrigEgammaRecAlgo
    TrigEgammaRecAlgo.TrackMatchBuilderTool = TrigEMTrackMatchBuilder
    TrigEgammaRecAlgo.InputTopoClusterContainerName = precisionCaloMenuDefs_LRT.precisionCaloClusters

    ## TrigElectronSuperClusterBuilder_LRT ##
    TrigSuperElectronAlgo = TrigElectronSuperClusterBuilder("TrigElectronSuperClusterBuilder_LRT")
    thesequence += TrigSuperElectronAlgo
    TrigSuperElectronAlgo.InputEgammaRecContainerName =  TrigEgammaRecAlgo.egammaRecContainer
    TrigSuperElectronAlgo.TrackMatchBuilderTool = TrigEMTrackMatchBuilder

    ## TrigTopoEgammaElectronCfg_LRT ##
    TrigTopoEgammaAlgo = TrigTopoEgammaElectronCfg("TrigTopoEgammaElectronCfg_LRT")
    thesequence += TrigTopoEgammaAlgo
    TrigTopoEgammaAlgo.SuperElectronRecCollectionName = TrigSuperElectronAlgo.SuperElectronRecCollectionName
    TrigTopoEgammaAlgo.ElectronOutputName = TrigEgammaKeys_LRT.outputElectronKey_LRT
    collectionOut = TrigTopoEgammaAlgo.ElectronOutputName

    ## TrigElectronIsoBuilderCfg_LRT ##
    isoBuilder = TrigElectronIsoBuilderCfg_LRT("TrigElectronIsoBuilderCfg_LRT")
    thesequence += isoBuilder

    #online monitoring for topoEgammaBuilder_LRT
    from TriggerMenuMT.HLTMenuConfig.Electron.TrigElectronFactories import PrecisionElectronTopoMonitorCfg
    PrecisionElectronRecoMonAlgo_LRT = PrecisionElectronTopoMonitorCfg("PrecisionElectronTopoEgammaBuilder_LRT")
    PrecisionElectronRecoMonAlgo_LRT.ElectronKey = TrigTopoEgammaAlgo.ElectronOutputName
    thesequence += PrecisionElectronRecoMonAlgo_LRT

    #online monitoring for TrigElectronSuperClusterBuilder_LRT
    from TriggerMenuMT.HLTMenuConfig.Electron.TrigElectronFactories import PrecisionElectronSuperClusterMonitorCfg
    PrecisionElectronSuperClusterMonAlgo_LRT = PrecisionElectronSuperClusterMonitorCfg("PrecisionElectronSuperClusterBuilder_LRT")
    PrecisionElectronSuperClusterMonAlgo_LRT.InputEgammaRecContainerName = TrigSuperElectronAlgo.SuperElectronRecCollectionName
    thesequence += PrecisionElectronSuperClusterMonAlgo_LRT
    
    return (thesequence, collectionOut)
