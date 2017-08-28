/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrigMuonEvent_page TrigMuonEvent
@author alexander.oh@cern.ch

@section TrigMuonEvent_TrigMuonEventIntro Introduction


EDM package implementing transient objects to be used by FEX algorithms to
fill information for later processing by Hypo's or other tools and
storage.

@section TrigMuonEvent_MyPackageOverview Class Overview
  The TrigMuonEvent package contains of following classes:

  - CombinedMuonFeature          : used by muComb
  - MuonFeature                  : used by muFast
  - TileMuFeature                : muon candidates from TileCal and the combined tracks from Inner-Detector.
  - TileTrackMuFeature           : used by tile track muon trigger
  - TrigDiMuon                   : used by di muon trigger
  - TrigMuonClusterFeature       : used by muon ROI cluster trigger
  - IsoMuonFeature               : used by muon isolation trigger
  - TrigMuonEF                   : used by muon EF, legacy. Use TrigMuonEFInfo instead.
  - TrigMuonEFContainer          : container for TrigMuonEF
  - TrigMuonEFInfo               : used by muon EF
  - TrigMuonEFInfoContainer      : container for TrigMuonEFInfo
  - TrigMuonEFTrack              : track information, part of TrigMuonEFInfo
  - TrigMuonEFCbTrack            : combined track information, part of TrigMuonEFInfo
  - TrigMuonEFInfoTrack          : container object to hold track information, part of TrigMuonEFInfo
  - TrigMuonEFInfoTrackContainer : container for TrigMuonEFInfo




*/
