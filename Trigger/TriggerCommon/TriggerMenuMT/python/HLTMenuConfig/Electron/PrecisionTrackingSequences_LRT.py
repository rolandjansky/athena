#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.CFElements import parOR
from AthenaCommon.GlobalFlags import globalflags
import AthenaCommon.CfgMgr as CfgMgr

#logging
from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)

from TriggerMenuMT.HLTMenuConfig.Egamma.TrigEgammaDefs import TrigEgammaKeys_LRT

def precisionTracking_LRT(RoIs):
## Taking Fast Track information computed in 2nd step ##
    
    IDTrigConfig = TrigEgammaKeys_LRT.IDTrigConfig_LRT

    # TrackCollection="TrigFastTrackFinder_Tracks_Electron"
    ViewVerifyTrk = CfgMgr.AthViews__ViewDataVerifier("FastTrackViewDataVerifier_LRT")
    
    ViewVerifyTrk.DataObjects = [( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+' + RoIs ),
                                 ( 'CaloCellContainer' , 'StoreGateSvc+CaloCells' ),
                                 ( 'SG::AuxElement' , 'StoreGateSvc+EventInfo.averageInteractionsPerCrossing' ),
                                 ( 'IDCInDetBSErrContainer' , 'StoreGateSvc+SCT_FlaggedCondData_TRIG' )]

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
    
    from TrigInDetConfig.InDetTrigPrecisionTracking import makeInDetTrigPrecisionTracking

    PTTracks, PTTrackParticles, PTAlgs = makeInDetTrigPrecisionTracking( config = IDTrigConfig, verifier = ViewVerifyTrk, rois= RoIs )
    PTSeq = parOR("precisionTrackingInElectrons_LRT", PTAlgs)
    #trackParticles = PTTrackParticles[-1]    
    trackParticles = TrigEgammaKeys_LRT.TrigElectronTracksCollectionName_LRT   

    electronPrecisionTrack = parOR("electronPrecisionTrack_LRT")
    electronPrecisionTrack += ViewVerifyTrk
    electronPrecisionTrack += PTSeq

    return electronPrecisionTrack, trackParticles
