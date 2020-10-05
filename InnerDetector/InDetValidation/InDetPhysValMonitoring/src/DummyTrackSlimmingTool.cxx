/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "DummyTrackSlimmingTool.h"
#include "TrkTrack/Track.h"

DummyTrackSlimmingTool::DummyTrackSlimmingTool(const std::string& type, const std::string& name, const IInterface* p)
  : AthAlgTool(type, name, p) {
  declareInterface<Trk::ITrackSlimmingTool>(this);
}

/**This method 'skims' interesting information from the passed track, and creates a new one with cloned copies of this
   information
   @param track A reference to the track to be skimmed. It will not be modified in any way.
   @return A 'slimmed' version of 'track', where exactly what information is copied depends on how the tool is
      configured
 */
Trk::Track*
DummyTrackSlimmingTool::slim(const Trk::Track& track) const {
  return new Trk::Track(track);
}

std::unique_ptr<Trk::Track> 
DummyTrackSlimmingTool::slimCopy(const Trk::Track& track) const{
  return std::make_unique<Trk::Track>(track);
}

void
DummyTrackSlimmingTool::slimTrack(Trk::Track&) const{
}




