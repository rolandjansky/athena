#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.CFElements import parOR
#logging
from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)

def precisionElectronRecoSequence_GSF(RoIs, ion=False, variant = ''):
    """
    Similar reconstruction sequence as ../PrecisionElectronRecoSequences.py, tailored to run GSF and LRTGSF
    """

    log.debug('precisionElectronRecoSequence(RoIs = %s, variant = %s)',RoIs,variant)

    tag = '_ion' if ion is True else ''
    tag+=variant
 
    import AthenaCommon.CfgMgr as CfgMgr
       
    # precision Tracking related data dependencies
    from TriggerMenuMT.HLT.Egamma.TrigEgammaKeys import  getTrigEgammaKeys
    if variant == '_GSF':
        TrigEgammaKeys_noGSF = getTrigEgammaKeys()
    else:
        TrigEgammaKeys_noGSF = getTrigEgammaKeys('_LRT')
    TrigEgammaKeys = getTrigEgammaKeys(variant, ion=ion)
    caloClusters = TrigEgammaKeys.precisionElectronCaloClusterContainer
    trackParticles = TrigEgammaKeys.precisionElectronTrackParticleContainerGSF
    trackParticles_noGSF = TrigEgammaKeys_noGSF.precisionTrackingContainer

    ViewVerifyTrk_GSF   = CfgMgr.AthViews__ViewDataVerifier("PrecisionTrackViewDataVerifier"+tag)

    from TrigInDetConfig.InDetTrigCollectionKeys import TrigTRTKeys, TrigPixelKeys
    ViewVerifyTrk_GSF.DataObjects = [( 'CaloCellContainer' , 'StoreGateSvc+CaloCells' ),
                                 ( 'xAOD::CaloClusterContainer' , 'StoreGateSvc+%s' % caloClusters ),
                                 ( 'xAOD::TrackParticleContainer','StoreGateSvc+%s' % trackParticles),
                                 ( 'xAOD::TrackParticleContainer','StoreGateSvc+%s' % trackParticles_noGSF),
                                 # verifier object needed by GSF
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
    TrigEMTrackMatchBuilder_GSF.TrackParticlesName = trackParticles
    
    ## TrigEgammaRecElectron_GSF ##
    TrigEgammaRecAlgo_GSF = TrigEgammaRecElectron("TrigEgammaRecElectron"+tag)
    thesequence_GSF += TrigEgammaRecAlgo_GSF
    TrigEgammaRecAlgo_GSF.TrackMatchBuilderTool = TrigEMTrackMatchBuilder_GSF
    TrigEgammaRecAlgo_GSF.InputClusterContainerName = caloClusters

    ## TrigElectronSuperClusterBuilder_GSF ##
    TrigSuperElectronAlgo_GSF = TrigElectronSuperClusterBuilder("TrigElectronSuperClusterBuilder"+tag)
    thesequence_GSF += TrigSuperElectronAlgo_GSF
    TrigSuperElectronAlgo_GSF.TrackMatchBuilderTool = TrigEMTrackMatchBuilder_GSF

    ## TrigTopoEgammaElectronCfg_GSF ##
    TrigTopoEgammaAlgo_GSF = TrigTopoEgammaElectronCfg("TrigTopoEgammaElectronCfg"+tag) 
    thesequence_GSF += TrigTopoEgammaAlgo_GSF
    TrigTopoEgammaAlgo_GSF.ElectronOutputName = TrigEgammaKeys.precisionElectronContainer
    collectionOut_GSF = TrigTopoEgammaAlgo_GSF.ElectronOutputName
    TrigTopoEgammaAlgo_GSF.DummyElectronOutputName = "HLT_PrecisionDummyElectron"
    collectionOut_GSF_dummy = TrigTopoEgammaAlgo_GSF.DummyElectronOutputName

    ## TrigElectronIsoBuilderCfg_GSF ##
    if variant == '_LRTGSF': # LRT sequence GSF, NOTE: variant can be: _LRT, _GSF, _LRTGSF or ''
        isoBuilder_GSF = TrigElectronIsoBuilderCfg_LRT("TrigElectronIsoBuilderCfg" + tag)
    else: # standard sequence GSF
        isoBuilder_GSF = TrigElectronIsoBuilderCfg("TrigElectronIsoBuilderCfg" + tag)
    
    isoBuilder_GSF.ElectronCollectionContainerName = TrigEgammaKeys.precisionElectronContainer
    thesequence_GSF += isoBuilder_GSF

    
    #online monitoring for topoEgammaBuilder_GSF
    from TriggerMenuMT.HLT.Electron.TrigElectronFactories import PrecisionElectronTopoMonitorCfg
    PrecisionElectronRecoMonAlgo_GSF = PrecisionElectronTopoMonitorCfg("PrecisionElectronTopoMonitoring"+tag)
    PrecisionElectronRecoMonAlgo_GSF.ElectronKey = TrigTopoEgammaAlgo_GSF.ElectronOutputName
    PrecisionElectronRecoMonAlgo_GSF.IsoVarKeys = [ '%s.ptcone20' % TrigTopoEgammaAlgo_GSF.ElectronOutputName, '%s.ptvarcone20' % TrigTopoEgammaAlgo_GSF.ElectronOutputName]
    thesequence_GSF += PrecisionElectronRecoMonAlgo_GSF

    #online monitoring for TrigElectronSuperClusterBuilder_GSF
    from TriggerMenuMT.HLT.Electron.TrigElectronFactories import PrecisionElectronSuperClusterMonitorCfg
    PrecisionElectronSuperClusterMonAlgo_GSF = PrecisionElectronSuperClusterMonitorCfg("PrecisionElectronSuperClusterBuilder"+tag)
    PrecisionElectronSuperClusterMonAlgo_GSF.InputEgammaRecContainerName = TrigSuperElectronAlgo_GSF.SuperElectronRecCollectionName
    thesequence_GSF += PrecisionElectronSuperClusterMonAlgo_GSF

    return (thesequence_GSF, collectionOut_GSF, collectionOut_GSF_dummy)
