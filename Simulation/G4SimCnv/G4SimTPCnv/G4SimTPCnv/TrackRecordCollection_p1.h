/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4SIM_TRACKRECORDCOLLECTION_P1_H
#define G4SIM_TRACKRECORDCOLLECTION_P1_H

/*

Persistent represenation of a TrackRecordCollection,
Author: Davide Costanzo

*/

#include <vector>
#include "G4SimTPCnv/TrackRecord_p1.h"


class TrackRecordCollection_p1
   : public std::vector<TrackRecord_p1>
{
public:
  // Default constructor
  TrackRecordCollection_p1 () {}
};

#endif

