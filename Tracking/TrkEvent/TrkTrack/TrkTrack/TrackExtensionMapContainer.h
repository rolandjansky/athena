/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKPRIM_TRACKEXTENSION_CNT_H
#define TRKPRIM_TRACKEXTENSION_CNT_H

#include "DataModel/DataVector.h"
#include "TrkTrack/TrackExtensionMap.h"
#include "CLIDSvc/CLASS_DEF.h"

 /** @brief Container of TrackExtensionMap for the trigger navigation */

typedef DataVector<TrackExtensionMap> TrackExtensionMapContainer;
CLASS_DEF( TrackExtensionMapContainer , 1280217485 , 1 )

#endif
