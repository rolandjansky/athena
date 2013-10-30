/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVertexSeedFinderUtils/TwoTracks.h"

namespace Trk {

  TwoTracks::TwoTracks() : std::pair<const Perigee*,const Perigee*>(0,0) {}

  TwoTracks::TwoTracks(const Perigee & first,const Perigee & second): 
          std::pair<const Perigee*,const Perigee*>(&first,&second) {}

  TwoTracks::TwoTracks(const TwoTracks & same) : 
    std::pair<const Perigee*,const Perigee*>(same.first,same.second) {}

  const Perigee & TwoTracks::getFirstPerigee() const {
    return *(this->first);
  }

  const Perigee & TwoTracks::getSecondPerigee() const {
    return *(this->second);
  }

  void TwoTracks::setFirstPerigee(const Perigee & first)
  {
    this->first=&first;
  }

  void TwoTracks::setSecondPerigee(const Perigee & second)
  {
    this->second=&second;
  }
  
}
