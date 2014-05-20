/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* ===============================================================================
			RefittedTrackPair.cxx  -  description
			-------------------------------------
begin                : Friday 30th March 2006
author               : Tom Atkinson
email                : t.atkinson@physics.unimelb.edu.au
decription           : Implementation code for the RefittedTrackPair class
================================================================================== */

#include "TrkTrack/RefittedTrackPair.h"

Trk::RefittedTrackPair::RefittedTrackPair(){}

Trk::RefittedTrackPair::RefittedTrackPair( ElementLink< DataVector< Trk::Track > > originalTrack, Trk::Track* refittedTrack )
  :
  std::pair< ElementLink< DataVector< Trk::Track > >, Trk::Track* >( originalTrack, refittedTrack )
{}

Trk::RefittedTrackPair::~RefittedTrackPair(){}


