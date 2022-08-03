#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
from AthenaCommon.CFElements import parOR
#logging
from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)

def precisionElectronRecoSequence(RoIs, ion=False, doGSF=True, doLRT=False):
    """ With this function we will setup the sequence of offline EgammaAlgorithms so to make a electron for TrigEgamma 

    Sequence of algorithms is the following:
      - egammaRecBuilder/TrigEgammaRecElectron creates egammaObjects out of clusters and tracks. 
      - electronSuperClusterBuilder algorithm will create superclusters out of the topoclusters and tracks in egammaRec under the electron hypothesis
          https://gitlab.cern.ch/atlas/athena/blob/master/Reconstruction/egamma/egammaAlgs/python/egammaSuperClusterBuilder.py#L26 
      - TopoEgammBuilder will create photons and electrons out of trakcs and SuperClusters. Here at HLT electrons the aim is to ignore photons.
          https://gitlab.cern.ch/atlas/athena/blob/master/Reconstruction/egamma/egammaAlgs/src/topoEgammaBuilder.cxx
    """

    log.debug('precisionElectronRecoSequence(RoIs = %s, ion = %s, doGSF = %s, doLRT = %s)',RoIs,ion,doGSF,doLRT)

    tag = '_ion' if ion is True else ''

    # create a Variant string out of the options above
    variant = '_'
    if doLRT:
        variant+='LRT'
    if doGSF:
        variant+='GSF'

    if not doLRT and not doGSF:
        variant+='noGSF'

    tag += variant


 
    import AthenaCommon.CfgMgr as CfgMgr
    from TriggerMenuMT.HLT.Egamma.TrigEgammaFactories import  TrigEMClusterTool       
    # precision Tracking related data dependencies
    from TriggerMenuMT.HLT.Egamma.TrigEgammaKeys import  getTrigEgammaKeys
    if doLRT:
        TrigEgammaKeys_noGSF = getTrigEgammaKeys('_LRT')
    else:
        TrigEgammaKeys_noGSF = getTrigEgammaKeys()
    TrigEgammaKeys = getTrigEgammaKeys(variant, ion=ion)
    caloClusters = TrigEgammaKeys.precisionElectronCaloClusterContainer
    trackParticles_noGSF = TrigEgammaKeys_noGSF.precisionTrackingContainer
    if doGSF:
        trackParticles = TrigEgammaKeys.precisionElectronTrackParticleContainerGSF
    else:
        trackParticles = TrigEgammaKeys_noGSF.precisionTrackingContainer


    ViewVerifyTrk_GSF   = CfgMgr.AthViews__ViewDataVerifier("PrecisionTrackViewDataVerifier"+tag)

    from TrigInDetConfig.InDetTrigCollectionKeys import TrigTRTKeys, TrigPixelKeys
    ViewVerifyTrk_GSF.DataObjects = [( 'CaloCellContainer' , 'StoreGateSvc+CaloCells' ),
                                 ( 'xAOD::CaloClusterContainer' , 'StoreGateSvc+%s' % caloClusters ),
                                 ( 'xAOD::TrackParticleContainer','StoreGateSvc+%s' % trackParticles_noGSF)]
    if doGSF:
        ViewVerifyTrk_GSF.DataObjects += [
                                 # verifier object needed by GSF
                                 ( 'xAOD::TrackParticleContainer','StoreGateSvc+%s' % trackParticles),
                                 ( 'SG::AuxElement' , 'StoreGateSvc+EventInfo.averageInteractionsPerCrossing' ), 
                                 ( 'InDet::PixelGangedClusterAmbiguities' , 'StoreGateSvc+%s' % TrigPixelKeys.PixelClusterAmbiguitiesMap ),
                                 ( 'InDet::TRT_DriftCircleContainer' , 'StoreGateSvc+%s' % TrigTRTKeys.DriftCircles ),
                                 ( 'SG::AuxElement' , 'StoreGateSvc+EventInfo.AveIntPerXDecor' )]

        from AthenaCommon.GlobalFlags import globalflags
        if (globalflags.InputFormat.is_bytestream()):
          ViewVerifyTrk_GSF.DataObjects += [( 'IDCInDetBSErrContainer' , 'StoreGateSvc+PixelByteStreamErrs' )]
          ViewVerifyTrk_GSF.DataObjects += [( 'TRT_RDO_Container' , 'StoreGateSvc+%s' % TrigTRTKeys.RDOs )]
        else:
          ViewVerifyTrk_GSF.DataObjects += [( 'TRT_RDO_Container' , 'StoreGateSvc+%s' % "TRT_RDOs" )]

    """ Retrieve the factories now """
    from TriggerMenuMT.HLT.Electron.TrigElectronFactories import TrigEgammaRecElectron, TrigElectronSuperClusterBuilder, TrigTopoEgammaElectronCfg
    from TriggerMenuMT.HLT.Egamma.TrigEgammaFactories import  TrigEMTrackMatchBuilder, TrigElectronIsoBuilderCfg, TrigElectronIsoBuilderCfg_LRT
   
    # Create the sequence of three steps:
    #  - TrigEgammaRecElectron, TrigElectronSuperClusterBuilder, TrigTopoEgammaElectron

    # Create the sequence of steps:
    #  - TrigEgammaRecElectron, TrigElectronSuperClusterBuilder, TrigTopoEgammaElectron
    #The sequence of these algorithms
    thesequence_GSF = parOR( "precisionElectron"+ RoIs + tag)
    thesequence_GSF += ViewVerifyTrk_GSF

    ## TrigEMTrackMatchBuilder_GSF ##
    TrigEMTrackMatchBuilder_GSF = TrigEMTrackMatchBuilder("TrigEMTrackMatchBuilder"+tag)
    ## if gsf then use trackparticles here, otherwise match with trackparticles_noGSF
    if doGSF:
        TrigEMTrackMatchBuilder_GSF.TrackParticlesName = trackParticles
    else:
        TrigEMTrackMatchBuilder_GSF.TrackParticlesName = trackParticles_noGSF
    
    ## TrigEgammaRecElectron_GSF ##
    TrigEgammaRecAlgo_GSF = TrigEgammaRecElectron("TrigEgammaRecElectron"+tag)
    thesequence_GSF += TrigEgammaRecAlgo_GSF
    if doGSF:
        TrigEgammaRecAlgo_GSF.TrackMatchBuilderTool = TrigEMTrackMatchBuilder_GSF
    TrigEgammaRecAlgo_GSF.InputClusterContainerName = caloClusters

    ## TrigElectronSuperClusterBuilder_GSF ##
    TrigSuperElectronAlgo_GSF = TrigElectronSuperClusterBuilder("TrigElectronSuperClusterBuilder"+tag)
    thesequence_GSF += TrigSuperElectronAlgo_GSF
    TrigSuperElectronAlgo_GSF.TrackMatchBuilderTool = TrigEMTrackMatchBuilder_GSF

    ## TrigTopoEgammaElectronCfg_GSF ##
    TrigTopoEgammaAlgo_GSF = TrigTopoEgammaElectronCfg("TrigTopoEgammaElectronCfg"+tag) 
    thesequence_GSF += TrigTopoEgammaAlgo_GSF
    TrigTopoEgammaAlgo_GSF.EMClusterTool = TrigEMClusterTool("electron",variant)
    if doGSF:
        TrigTopoEgammaAlgo_GSF.ElectronOutputName = TrigEgammaKeys.precisionElectronContainer
    else:
        TrigTopoEgammaAlgo_GSF.ElectronOutputName = TrigEgammaKeys_noGSF.precisionElectronContainer
    collectionOut_GSF = TrigTopoEgammaAlgo_GSF.ElectronOutputName
    TrigTopoEgammaAlgo_GSF.DummyElectronOutputName = "HLT_PrecisionDummyElectron"
    collectionOut_GSF_dummy = TrigTopoEgammaAlgo_GSF.DummyElectronOutputName

    ## TrigElectronIsoBuilderCfg_GSF ##
    if doLRT:
        isoBuilder_GSF = TrigElectronIsoBuilderCfg_LRT("TrigElectronIsoBuilderCfg" + tag)
    else: # standard sequence GSF
        isoBuilder_GSF = TrigElectronIsoBuilderCfg("TrigElectronIsoBuilderCfg" + tag)
   

    if doGSF:
        isoBuilder_GSF.ElectronCollectionContainerName = TrigEgammaKeys.precisionElectronContainer
    else:
        isoBuilder_GSF.ElectronCollectionContainerName = TrigEgammaKeys_noGSF.precisionElectronContainer
        isoBuilder_GSF.useBremAssoc = False # Yhis line needs to be checked

    thesequence_GSF += isoBuilder_GSF

    
    #online monitoring for topoEgammaBuilder_GSF
    from TriggerMenuMT.HLT.Electron.TrigElectronFactories import PrecisionElectronTopoMonitorCfg
    PrecisionElectronRecoMonAlgo_GSF = PrecisionElectronTopoMonitorCfg("PrecisionElectronTopoMonitoring"+tag)
    PrecisionElectronRecoMonAlgo_GSF.ElectronKey = TrigTopoEgammaAlgo_GSF.ElectronOutputName
    PrecisionElectronRecoMonAlgo_GSF.IsoVarKeys = [ '%s.ptcone20' % TrigTopoEgammaAlgo_GSF.ElectronOutputName,
                                                    '%s.ptvarcone20' % TrigTopoEgammaAlgo_GSF.ElectronOutputName,
                                                    '%s.ptcone30' % TrigTopoEgammaAlgo_GSF.ElectronOutputName,
                                                    '%s.ptvarcone30' % TrigTopoEgammaAlgo_GSF.ElectronOutputName ]
    thesequence_GSF += PrecisionElectronRecoMonAlgo_GSF

    #online monitoring for TrigElectronSuperClusterBuilder_GSF
    from TriggerMenuMT.HLT.Electron.TrigElectronFactories import PrecisionElectronSuperClusterMonitorCfg
    PrecisionElectronSuperClusterMonAlgo_GSF = PrecisionElectronSuperClusterMonitorCfg("PrecisionElectronSuperClusterBuilder"+tag)
    PrecisionElectronSuperClusterMonAlgo_GSF.InputEgammaRecContainerName = TrigSuperElectronAlgo_GSF.SuperElectronRecCollectionName
    thesequence_GSF += PrecisionElectronSuperClusterMonAlgo_GSF

    return (thesequence_GSF, collectionOut_GSF, collectionOut_GSF_dummy)

