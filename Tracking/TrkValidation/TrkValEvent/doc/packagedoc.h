/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrkValEvent_page The TrkValEvent package

This package is part of the software for checking
and validating the tracking algorithms. It hosts data classes which
form part of abstract interfaces (e.g. TrkValInterfaces) and
internal data structures in the tracking validation realm
(e.g. TrkValTools).

They are specific to validation work and not intended for
general use in the EDM. If a class is needed for additional
purposes other than validation it should be moved out of
this package.

@author <a href="http://consult.cern.ch/xwho/people/485812">Wolfgang Liebig</a>

@section TrkValEvent_OverviewTrkValEventw Class Overview
  The TrkValEvent package contains:

  - classes describing track-based jets: Trk::GenParticleJet and Trk::TrackJet.
    They are aimed at investigating tracking performance in or with jets.
  - definitions for track categories at MC truth level. They are enums in
    TruthClassificationDefs.h
  - Trk::TrackStateData which is a helper class for the Trk::TrackDiff algorithm.

The package follows the installed_library pattern.
      
@section TrkValEvent_ExtrasTrkValEvent Extra Pages

*/
