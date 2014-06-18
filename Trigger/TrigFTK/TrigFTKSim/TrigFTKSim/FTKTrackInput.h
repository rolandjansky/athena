/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKTRACKINPUT_H
#define FTKTRACKINPUT_H

#include "FTKTrackStream.h"
#include "FTKTrack.h"
#include "FTKObjectInput.h"

class FTKTrackInput : public FTKObjectInput<FTKTrackStream,FTKTrack> {
 public:
  int nextEvent(); // read the next event
  const FTKTrack* nextTrack(int); // move to the next track and return the pointer
  const FTKTrack* getTrack(int region,int trackid,int bankid) const; // retrieve track by its indices
};
#endif // FTKTRACKINPUT_H
