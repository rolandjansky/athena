/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* ===============================================================================
			RefittedTrackPairCollection.h  -  description
			---------------------------------------------
begin                : Friday 30th March 2006
author               : Tom Atkinson 
email                : t.atkinson@physics.unimelb.edu.au
decription           : Collection of refitted track pairs
================================================================================== */

#ifndef TrkRefittedTrackPairCollection_H
#define TrkRefittedTrackPairCollection_H

#include "TrkTrack/RefittedTrackPair.h"

#include "DataModel/DataVector.h"

typedef DataVector< Trk::RefittedTrackPair > RefittedTrackPairCollection;

CLASS_DEF(RefittedTrackPairCollection, 1098918547, 1)

#endif
