/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMuonBackTracker_H
#define IMuonBackTracker_H

#include "GaudiKernel/IAlgTool.h" 

//Can not be forwarded due to typedef
#include "TrkTrack/Track.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyAthToolInterfaces/IMboyAthToolMisc.h"

static const InterfaceID IID_IMuonBackTracker("Muon::IMuonBackTracker", 1, 0);

namespace Muon {

  /**
   @class IMuonBackTracker
   
  @author samusog@cern.ch
  
  */

class IMuonBackTracker:virtual public IAlgTool {

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();

   virtual Trk::Track*           MuonBackTrack(const Trk::Track*           pTrack           ) = 0 ;

};

inline const InterfaceID& Muon::IMuonBackTracker::interfaceID() { return IID_IMuonBackTracker;}

}

#endif

