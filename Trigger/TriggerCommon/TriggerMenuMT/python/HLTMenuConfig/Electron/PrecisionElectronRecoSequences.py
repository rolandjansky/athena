#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.CFElements import parOR
from AthenaCommon.GlobalFlags import globalflags

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

    # First the data verifiers:
    # Here we define the data dependencies. What input needs to be available for the Fexs (i.e. TopoClusters from precisionCalo) in order to run
    from TriggerMenuMT.HLTMenuConfig.Egamma.PrecisionCaloSequenceSetup import precisionCaloMenuDefs
    import AthenaCommon.CfgMgr as CfgMgr


    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
    IDTrigConfig = getInDetTrigConfig( 'electron' )

    ## Taking Fast Track information computed in 2nd step ##
    #TrackCollection = IDTrigConfig.FT().trkTracksFTF()
    ViewVerifyTrk   = CfgMgr.AthViews__ViewDataVerifier("FastTrackViewDataVerifier")
    
    ViewVerifyTrk.DataObjects = [ #( 'TrackCollection' , 'StoreGateSvc+' + TrackCollection ),
                                 ( 'xAOD::CaloClusterContainer' , 'StoreGateSvc+' + precisionCaloMenuDefs.precisionCaloClusters ),
                                 ( 'CaloCellContainer' , 'StoreGateSvc+CaloCells' ),
                                 ( 'SG::AuxElement' , 'StoreGateSvc+EventInfo.AveIntPerXDecor' ),
                                 ( 'IDCInDetBSErrContainer' , 'StoreGateSvc+SCT_FlaggedCondData_TRIG' ),
                                 ( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+precisionElectron' ),
                                 ( 'IDCInDetBSErrContainer' , 'StoreGateSvc+PixelByteStreamErrs' ),
                                 ( 'IDCInDetBSErrContainer' , 'StoreGateSvc+SCT_ByteStreamErrs' )] # the load below doesn't always work

    # These objects must be loaded from SGIL if not from CondInputLoader
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    if globalflags.InputFormat.is_bytestream():
      ViewVerifyTrk.DataObjects += [( 'IDCInDetBSErrContainer' , 'StoreGateSvc+PixelByteStreamErrs' ),
                                    ( 'IDCInDetBSErrContainer' , 'StoreGateSvc+SCT_ByteStreamErrs' )]
    else:
      topSequence.SGInputLoader.Load += [( 'TRT_RDO_Container' , 'StoreGateSvc+TRT_RDOs' )]
      ViewVerifyTrk.DataObjects += [( 'TRT_RDO_Container' , 'StoreGateSvc+TRT_RDOs' )]

    """ Precision Track Related Setup.... """
    PTAlgs = []
    PTTracks = []
    PTTrackParticles = []

    from TrigInDetConfig.InDetPT import makeInDetPrecisionTracking

    PTTracks, PTTrackParticles, PTAlgs = makeInDetPrecisionTracking( config = IDTrigConfig, verifier = ViewVerifyTrk, rois= RoIs )
    PTSeq = parOR("precisionTrackingInElectrons", PTAlgs)
    trackParticles = PTTrackParticles[-1]
    
    electronPrecisionTrack = parOR("electronPrecisionTrack")
    electronPrecisionTrack += ViewVerifyTrk
    electronPrecisionTrack += PTSeq

    """ Retrieve the factories now """
    from TriggerMenuMT.HLTMenuConfig.Electron.TrigElectronFactories import TrigEgammaRecElectron, TrigElectronSuperClusterBuilder, TrigTopoEgammaElectronCfg
    from TriggerMenuMT.HLTMenuConfig.Egamma.TrigEgammaFactories import  TrigEMTrackMatchBuilder

     
    #The sequence of these algorithms
    thesequence = parOR( "precisionElectron_"+RoIs)
   
    # Create the sequence of three steps:
    #  - TrigEgammaRecElectron, TrigElectronSuperClusterBuilder, TrigTopoEgammaElectron
    trackMatchBuilder = TrigEMTrackMatchBuilder()
    trackMatchBuilder.TrackParticlesName = trackParticles
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

    return (thesequence, electronPrecisionTrack, collectionOut)
