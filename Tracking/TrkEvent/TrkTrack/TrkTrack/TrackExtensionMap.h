/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKPRIM_TRACKEXTENSION_H
#define TRKPRIM_TRACKEXTENSION_H

#include <vector>
#include <map>
#include "TrkTrack/Track.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "AthenaKernel/CLASS_DEF.h"

 /** @brief AUTO - An Undocumented Tracking Object */

class TrackExtensionMap : public std::map<const Trk::Track*, std::vector<const Trk::MeasurementBase*> > {
  
 public:
  // custom destructor
  ~TrackExtensionMap();

};

// this is a hack, inline the destructor
inline TrackExtensionMap::~TrackExtensionMap() {
  for (auto& p : *this) {
    for (const Trk::MeasurementBase* m : p.second) {
      delete m;
    }
  }
}

// do the storegate stuff
CLASS_DEF( TrackExtensionMap , 177089779 , 1 )

#endif
