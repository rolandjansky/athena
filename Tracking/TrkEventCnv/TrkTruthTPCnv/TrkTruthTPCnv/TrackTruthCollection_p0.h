/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

// Andrei.Gaponenko@cern.ch, 2007


#ifndef TRACKTRUTHCOLLECTION_P0_H
#define TRACKTRUTHCOLLECTION_P0_H

#include "DataModel/DataVector.h"
#include "DataModel/ElementLink.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTruthData/TrackTruth.h"

#include <vector>
#include <utility>

class TrackTruthVector : 
  public std::vector<std::pair< ElementLink< DataVector<Trk::Track> >, TrackTruth> > 
{};

#endif/*TRACKTRUTHCOLLECTION_P0_H*/
