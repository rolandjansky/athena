#ifndef PFTRACKFILLER_H
#define PFTRACKFILLER_H

#include "eflowRec/eflowRecTrack.h"
#include "eflowRec/PFData.h"

class PFTrackFiller {

public:
  PFTrackFiller(){};
  ~PFTrackFiller(){};

  void fillTracksToRecover(PFData &data) const;
  void fillTracksToConsider(PFData &data, eflowRecTrackContainer &recTrackContainer) const;

};
#endif