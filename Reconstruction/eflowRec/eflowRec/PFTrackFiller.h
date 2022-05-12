#ifndef PFTRACKFILLER_H
#define PFTRACKFILLER_H

#include "eflowRec/eflowRecTrack.h"
#include "eflowRec/PFData.h"

class PFTrackFiller {

public:
  PFTrackFiller(){};
  ~PFTrackFiller(){};

  static void fillTracksToRecover(PFData &data) ;
  static void fillTracksToConsider(PFData &data, eflowRecTrackContainer &recTrackContainer) ;

};
#endif