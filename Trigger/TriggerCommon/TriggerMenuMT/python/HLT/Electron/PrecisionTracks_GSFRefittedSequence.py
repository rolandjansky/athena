#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.CFElements import parOR
import AthenaCommon.CfgMgr as CfgMgr

#logging
from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)

def precisionTracks_GSFRefitted(RoIs, ion=False, variant=''):
    """
    Takes precision Tracks as input and applies GSF refits on top
    """
    log.debug('precisionTracks_GSFRefitted(RoIs = %s, variant = %s)',RoIs,variant)

    tag = '_ion' if ion is True else ''
    tag+=variant

    # precision Tracking related data dependencies
    from TriggerMenuMT.HLT.Egamma.TrigEgammaKeys import  getTrigEgammaKeys

    TrigEgammaKeys = getTrigEgammaKeys(variant, ion=ion)

    trackParticles = TrigEgammaKeys.precisionTrackingContainer

    ViewVerifyPrecisionTrk   = CfgMgr.AthViews__ViewDataVerifier("PrecisionTrackViewDataVerifier_forGSFRefit"+tag)

    from TrigInDetConfig.InDetTrigCollectionKeys import TrigTRTKeys, TrigPixelKeys
    ViewVerifyPrecisionTrk.DataObjects = [( 'xAOD::TrackParticleContainer','StoreGateSvc+%s' % trackParticles),
                                 # verifier object needed by GSF
                                 ( 'SG::AuxElement' , 'StoreGateSvc+EventInfo.averageInteractionsPerCrossing' ), 
                                 ( 'InDet::PixelGangedClusterAmbiguities' , 'StoreGateSvc+%s' % TrigPixelKeys.PixelClusterAmbiguitiesMap ),
                                 ( 'InDet::TRT_DriftCircleContainer' , 'StoreGateSvc+%s' % TrigTRTKeys.DriftCircles ),
                                 ( 'SG::AuxElement' , 'StoreGateSvc+EventInfo.AveIntPerXDecor' )]

     # These objects must be loaded from SGIL if not from CondInputLoader
 
    from AthenaCommon.GlobalFlags import globalflags
    if (globalflags.InputFormat.is_bytestream()):
      ViewVerifyPrecisionTrk.DataObjects += [( 'IDCInDetBSErrContainer' , 'StoreGateSvc+PixelByteStreamErrs' )]
      ViewVerifyPrecisionTrk.DataObjects += [( 'TRT_RDO_Container' , 'StoreGateSvc+%s' % TrigTRTKeys.RDOs )]
    else:
      ViewVerifyPrecisionTrk.DataObjects += [( 'TRT_RDO_Container' , 'StoreGateSvc+%s' % "TRT_RDOs" )]


    from TriggerMenuMT.HLT.Electron.TrigEMBremCollectionBuilder import TrigEMBremCollectionBuilderCfg
    
    thesequence_GSF = parOR( "precisionTracking_GSF%s" % RoIs)
    thesequence_GSF += ViewVerifyPrecisionTrk
    
    ## TrigEMBremCollectionBuilder ##
    
    track_GSF = TrigEMBremCollectionBuilderCfg(variant,TrigEgammaKeys)
    thesequence_GSF += track_GSF

     ## TrigEMTrackMatchBuilder_GSF ##
    trackParticles_GSF= track_GSF.OutputTrkPartContainerName
    return thesequence_GSF, trackParticles_GSF
