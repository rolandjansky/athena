/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVERTEXSEEDFINDERUTILS_TWOTRACKS_H
#define TRKVERTEXSEEDFINDERUTILS_TWOTRACKS_H

#include "TrkParameters/TrackParameters.h"

namespace Trk 
{
  

  class TwoTracks : public std::pair<const Perigee*,const Perigee*> 
  {
    
  public:
    
    TwoTracks();
    
    TwoTracks(const Perigee & first,const Perigee & second);
    
    TwoTracks(const TwoTracks & same);
    
    ~TwoTracks() {};
    
    const Perigee & getFirstPerigee() const;

    const Perigee & getSecondPerigee() const;
    
    void setFirstPerigee(const Perigee & first);

    void setSecondPerigee(const Perigee & first);
    
  };
  
}

#endif
