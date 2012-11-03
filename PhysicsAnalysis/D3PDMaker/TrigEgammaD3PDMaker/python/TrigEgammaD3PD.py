# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import D3PDMakerCoreComps
from D3PDMakerConfig.D3PDMakerFlags             import D3PDMakerFlags
from TrigEgammaD3PDMaker.TrigElectronD3PDObject import TrigElectronD3PDObject
from TrigEgammaD3PDMaker.TrigPhotonD3PDObject   import TrigPhotonD3PDObject
from TrigEgammaD3PDMaker.EFElectronD3PDObject   import EFElectronD3PDObject
from TrigEgammaD3PDMaker.EFPhotonD3PDObject     import EFPhotonD3PDObject
from TrigCaloD3PDMaker.TrigEMClusterD3PDObject  import TrigEMClusterD3PDObject

from TriggerD3PDMaker.TrigNaviD3PDObject     import *
from TriggerD3PDMaker.TrigDecisionD3PDObject import TrigDecisionD3PDObject

from CaloD3PDMaker.ClusterD3PDObject import ClusterD3PDObject
from CaloD3PDMaker.CellD3PDObject import CellD3PDObject

from egammaD3PDMaker.PhotonD3PDObject import PhotonD3PDObject

from TrigInDetD3PDMaker.TrigInDetTrackD3PDObject import TrigInDetTrackD3PDObject
from TrackD3PDMaker.TrackD3PDObject import TrackParticleD3PDObject
from TrackD3PDMaker.TrackD3PDMakerFlags import TrackD3PDFlags

TrackD3PDFlags.doTruth = False
TrackD3PDFlags.storeDiagonalCovarianceAsErrors = False
TrackD3PDFlags.storeHitTruthMatching = False
TrackD3PDFlags.storeDetailedTruth = False
TrackD3PDFlags.storePixelHitsOnTrack = False
TrackD3PDFlags.storeSCTHitsOnTrack = False
TrackD3PDFlags.storeTRTHitsOnTrack = False
TrackD3PDFlags.storePixelOutliersOnTrack = False
TrackD3PDFlags.storeSCTOutliersOnTrack = False
TrackD3PDFlags.storeTRTOutliersOnTrack = False
TrackD3PDFlags.storePixelHolesOnTrack = False
TrackD3PDFlags.storeSCTHolesOnTrack = False
TrackD3PDFlags.storeTRTHolesOnTrack = False
TrackD3PDFlags.storeTrackPredictionAtBLayer = False
TrackD3PDFlags.storeTrackInfo = False
TrackD3PDFlags.storeTrackFitQuality = True
TrackD3PDFlags.storeTrackSummary = True
TrackD3PDFlags.trackParametersAtGlobalPerigeeLevelOfDetails = 0
TrackD3PDFlags.trackParametersAtPrimaryVertexLevelOfDetails = 0
TrackD3PDFlags.trackParametersAtBeamSpotLevelOfDetails = 0
TrackD3PDFlags.storeTrackMomentum = True
TrackD3PDFlags.vertexPositionLevelOfDetails = 0
TrackD3PDFlags.storeVertexFitQuality = False
TrackD3PDFlags.storeVertexKinematics = False
TrackD3PDFlags.storeVertexPurity = False
TrackD3PDFlags.storeVertexTrackAssociation = False
TrackD3PDFlags.storeVertexTrackIndexAssociation = False

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


# Dummy to get trigger set up.
from D3PDMakerCoreComps.D3PDObject import make_Void_D3PDObject
InitTrigD3PDObject = make_Void_D3PDObject (default_name = 'InitTrigEgamma')
def _initTrigHook (c, **kw):
    from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
    cfg =  TriggerConfigGetter()
    # import TrigEgammaD3PDMaker.Setup_RoILinksCnvTool_IBv2
    import TrigEgammaD3PDMaker.Setup_RoILinksCnvTool_Pppv1
    preseq = AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName())
    if not hasattr (preseq, 'FlatHltNaviMaker'):
        from TriggerMenuAnalysis.TriggerMenuAnalysisConf import FlatHltNaviMaker
        preseq += FlatHltNaviMaker ('FlatHltNaviMaker')
    return
InitTrigD3PDObject.defineHook (_initTrigHook)

##
# @short Function for passing parameters to individual D3PDObjects
#
# This function helps in making it possible to pass specific options
# to the D3PDObjects added by the TrigEgammaD3PDObjects function.
# The formalist is the same as for all the other D3PDMaker function.
#
# @param level The overall detail level of the D3PDObject
# @param name  An object name to refer to this object later on
# @param kwin  Input keyworded parameters
# @param kw    Extra keywords provided here
# @returns The list of parameters to give to the D3PDObject
#
def _args( level, name, kwin, **kw ):
    result = kw.copy()
    result[ "level" ] = level
    for ( k, v ) in kwin.items():
        if k.startswith( name + "_" ):
            result[ k[ len( name ) + 1 : ] ] = v
            pass
        pass
    return result

def TrigEgammaD3PDObjects( alg, level = 10, **kw ):

    alg += InitTrigD3PDObject( level )

    # L1 trigger info
    if not hasattr( alg, "trig_L1_emtau_Filler" ):
        from TriggerD3PDMaker.EmTauROID3PDObject import EmTauROID3PDObject
        alg += EmTauROID3PDObject( **_args( level, "EmTauROI", kw ) )
    else:
        from AthenaCommon.Logging import logging
        _mlog = logging.getLogger( "TrigEgammaD3PDObjects" )
        _mlog.info( "EmTauROID3PDObject already in the D3PD; skipping." )

    # L2 trigger info
    alg += TrigEMClusterD3PDObject( **_args( level, "TrigEMCluster", kw,
                                             prefix = 'trig_L2_emcl_',
                                             allowMissing = True ) )
    alg += TrigInDetTrackD3PDObject( **_args( level, "TrigInDetTrackIDSCAN", kw,
                                              sgkey = 'HLT_TrigIDSCAN_eGamma',
                                              prefix = 'trig_L2_trk_idscan_eGamma_',
                                              allowMissing = True ) )
    alg += TrigInDetTrackD3PDObject( **_args( level, "TrigInDetTrackSiTrack", kw,
                                              sgkey = 'HLT_TrigSiTrack_eGamma',
                                              prefix = 'trig_L2_trk_sitrack_eGamma_',
                                              allowMissing = True ) )
    alg += TrigElectronD3PDObject( **_args( level, "TrigElectron", kw,
                                            prefix = 'trig_L2_el_',
                                            sgkey = 'HLT_L2ElectronFex,HLT_L2IDCaloFex',
                                            allowMissing = True ) )
    alg += TrigPhotonD3PDObject( **_args( level, "TrigPhoton", kw,
                                          prefix='trig_L2_ph_' ) )

    # EF trigger info
    alg += ClusterD3PDObject( **_args( level, "TrigCaloCluster", kw,
                                       sgkey = 'HLT_TrigCaloClusterMaker',
                                       prefix = 'trig_EF_emcl_',
                                       allowMissing = True ) )
    alg += ClusterD3PDObject( **_args( level, "TrigCaloClusterSLW", kw,
                                       sgkey = 'HLT_TrigCaloClusterMaker_slw',
                                       prefix = 'trig_EF_emcl_slw_',
                                       allowMissing = True ) )
    alg += EFElectronD3PDObject( **_args( level, "EFElectron", kw,
                                          sgkey = 'HLT_egamma_Electrons,HLT_egamma',
                                          prefix = 'trig_EF_el_',
                                          allowMissing = True ) )
    alg += EFPhotonD3PDObject( **_args( level, "EFPhoton", kw, 
                                        sgkey = 'HLT_egamma_Photons,HLT_egamma',
                                        prefix = 'trig_EF_ph_',
                                        allowMissing = True ) )
    
    # Trigger navigation info
    if not hasattr( alg, "trig_Nav_Filler" ):
        alg += ChainEntryD3PDObject( **_args( level, "ChainEntry", kw ) )
        pass
    alg += CombLinksD3PDObjects[ 'L2_e' ]( **_args( level, "CombLinksL2", kw ) )
    alg += CombLinksD3PDObjects[ 'EF_e' ]( **_args( level, "CombLinksEF", kw ) )

    # Add the trigger configuration metadata:
    from TriggerD3PDMaker.TrigConfMetadata import addTrigConfMetadata
    addTrigConfMetadata( alg )

    if not hasattr(alg, 'TrigDecisionFiller'):
        alg += TrigDecisionD3PDObject( **_args( level, "TrigDecision", kw ) )
        pass
    
    return alg


def TrigEgammaD3PD(fname, level=10, tname='t', seq=topSequence):
    alg = D3PDMakerCoreComps.MakerAlg(tname, seq=seq, file=fname)
    TrigEgammaD3PDObjects (alg, level)
    return alg
