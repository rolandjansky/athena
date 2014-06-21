# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AppMgr import ServiceMgr as svcMgr

def makeTrackFilterAlg( TracksName = "TrackParticleCandidate",
                           OutputTracksName="TauSelectedTracks",
                        ptCut=500.,nSCTPix=6
                        
                           ) :

    from TauD3PDMaker.TauD3PDMakerConf import SimpleTrackFilter
    trackFilter = SimpleTrackFilter(OutputTracksName)

    trackFilter.TracksName = TracksName
    trackFilter.OutputTracksName = OutputTracksName 
    trackFilter.ptCut = ptCut
    trackFilter.nSCTPixHitsCut = nSCTPix

    return trackFilter


    



