/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKPRIM_TRACKEXTENSION_H
#define TRKPRIM_TRACKEXTENSION_H

#include <vector>
#include <map>
#include "TrkTrack/Track.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "CLIDSvc/CLASS_DEF.h"

 /** @brief AUTO - An Undocumented Tracking Object */

class TrackExtensionMap : public std::map<const Trk::Track*, std::vector<const Trk::MeasurementBase*> > {
  
 public:
  // custom destructor
  ~TrackExtensionMap();

};

// this is a hack, inline the destructor
inline TrackExtensionMap::~TrackExtensionMap() {
  TrackExtensionMap::iterator it    = this->begin();
  TrackExtensionMap::iterator itend = this->end();
  for (;it != itend; it++){
    std::vector<const Trk::MeasurementBase*>::iterator itvec    = it->second.begin();
    std::vector<const Trk::MeasurementBase*>::iterator itvecend = it->second.end();
    for (;itvec < itvecend; itvec++) delete (*itvec);
  }
}

// do the storegate stuff
CLASS_DEF( TrackExtensionMap , 177089779 , 1 )

#endif
