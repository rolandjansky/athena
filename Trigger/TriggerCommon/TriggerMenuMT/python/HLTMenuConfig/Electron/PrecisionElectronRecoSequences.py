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
      - electronSuperClusterBuilder algorithm will create superclusters out of the topoclusters and tracks in egammaRec under the electron hypothesis
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
                                 ( 'xAOD::CaloClusterContainer' , 'StoreGateSvc+%s' % precisionCaloMenuDefs.precisionCaloClusters ),
                                 ( 'xAOD::TrackParticleContainer','StoreGateSvc+%s' % TrigEgammaKeys.TrigElectronTracksCollectionName)]


    """ Retrieve the factories now """
    from TriggerMenuMT.HLTMenuConfig.Electron.TrigElectronFactories import TrigEgammaRecElectron, TrigElectronSuperClusterBuilder, TrigTopoEgammaElectronCfg
    from TriggerMenuMT.HLTMenuConfig.Egamma.TrigEgammaFactories import  TrigEMTrackMatchBuilder, TrigElectronIsoBuilderCfg

    # Create the sequence of three steps:
    #  - TrigEgammaRecElectron, TrigElectronSuperClusterBuilder, TrigTopoEgammaElectron

    # Create the sequence of three steps:
    #  - TrigEgammaRecElectron, TrigElectronSuperClusterBuilder, TrigTopoEgammaElectron
    #The sequence of these algorithms
    thesequence = parOR( "precisionElectron%s" % RoIs)
    thesequence += ViewVerifyTrk
    
    ## TrigEMTrackMatchBuilder_noGSF ##
    TrigEMTrackMatchBuilder = TrigEMTrackMatchBuilder("TrigEMTrackMatchBuilder_noGSF")
    TrigEMTrackMatchBuilder.TrackParticlesName =  TrigEgammaKeys.TrigElectronTracksCollectionName

    ## TrigEgammaRecElectron_noGSF ##
    TrigEgammaRecAlgo = TrigEgammaRecElectron("TrigEgammaRecElectron_noGSF")
    thesequence += TrigEgammaRecAlgo
    TrigEgammaRecAlgo.TrackMatchBuilderTool = TrigEMTrackMatchBuilder
    TrigEgammaRecAlgo.InputTopoClusterContainerName = precisionCaloMenuDefs.precisionCaloClusters

    ## TrigElectronSuperClusterBuilder_noGSF ##
    TrigSuperElectronAlgo = TrigElectronSuperClusterBuilder("TrigElectronSuperClusterBuilder_noGSF")
    thesequence += TrigSuperElectronAlgo
    TrigSuperElectronAlgo.InputEgammaRecContainerName =  TrigEgammaRecAlgo.egammaRecContainer
    TrigSuperElectronAlgo.TrackMatchBuilderTool = TrigEMTrackMatchBuilder

    ## TrigTopoEgammaElectronCfg_noGSF ##
    TrigTopoEgammaAlgo = TrigTopoEgammaElectronCfg("TrigTopoEgammaElectronCfg_noGSF")
    thesequence += TrigTopoEgammaAlgo
    TrigTopoEgammaAlgo.SuperElectronRecCollectionName = TrigSuperElectronAlgo.SuperElectronRecCollectionName
    collectionOut = TrigTopoEgammaAlgo.ElectronOutputName

    ## TrigElectronIsoBuilderCfg_noGSF ##
    isoBuilder = TrigElectronIsoBuilderCfg("TrigElectronIsoBuilderCfg_noGSF")
    thesequence += isoBuilder

    #online monitoring for topoEgammaBuilder
    from TriggerMenuMT.HLTMenuConfig.Electron.TrigElectronFactories import PrecisionElectronTopoMonitorCfg
    PrecisionElectronRecoMonAlgo = PrecisionElectronTopoMonitorCfg()
    PrecisionElectronRecoMonAlgo.ElectronKey = TrigTopoEgammaAlgo.ElectronOutputName
    thesequence += PrecisionElectronRecoMonAlgo

    #online monitoring for TrigElectronSuperClusterBuilder
    from TriggerMenuMT.HLTMenuConfig.Electron.TrigElectronFactories import PrecisionElectronSuperClusterMonitorCfg
    PrecisionElectronSuperClusterMonAlgo = PrecisionElectronSuperClusterMonitorCfg()
    PrecisionElectronSuperClusterMonAlgo.InputEgammaRecContainerName = TrigSuperElectronAlgo.SuperElectronRecCollectionName
    thesequence += PrecisionElectronSuperClusterMonAlgo
    return (thesequence, collectionOut)

