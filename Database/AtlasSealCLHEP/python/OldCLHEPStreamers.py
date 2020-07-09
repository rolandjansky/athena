# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


def loadOldCLHEPStreamers():
    """
    Helper method to load and configure the AthenaRootStreamerSvc to be able
    to read Old CLHEP classes (v1.8)
    """
    from AthenaCommon.Logging import logging
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    from AthenaCommon import CfgMgr
    
    msg = logging.getLogger( 'loadOldCLHEPStreamers' )
    msg.debug( "Loading Old CLHEP streamers..." )
    
    # ROOT streaming service
    if not hasattr (svcMgr, 'AthenaRootStreamerSvc'):
        svcMgr += CfgMgr.AthenaRootStreamerSvc()
    streamerSvc = svcMgr.AthenaRootStreamerSvc
    
    # active ROOT streamers list
    streamerSvc.Streamers  += [ "CLHEPVec3dStreamer" ]      
    streamerSvc.Streamers  += [ "CLHEPPoint3dStreamer" ]      
    streamerSvc.Streamers  += [ "CLHEPRotationStreamer" ]      
    streamerSvc.Streamers  += [ "CLHEPGenMatrixStreamer" ]      
    streamerSvc.Streamers  += [ "CLHEPMatrixStreamer" ]      
    streamerSvc.Streamers  += [ "CLHEPLorVecStreamer" ]      
    streamerSvc.Streamers  += [ "CLHEPTransform3DStreamer" ]      
    streamerSvc.Streamers  += [ "CLHEP3VectorStreamer" ]      
    streamerSvc.Streamers  += [ "CLHEPBasicVectorStreamer" ]      

    msg.debug( "Loading Old CLHEP streamers... [DONE]" )
    return

## load streamer configuration at module import
loadOldCLHEPStreamers()

## clean-up: avoid running multiple times this method
del loadOldCLHEPStreamers
