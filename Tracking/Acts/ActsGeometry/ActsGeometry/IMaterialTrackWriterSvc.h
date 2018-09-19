/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRY_IMATERIALTRACKWRITERSVC_H
#define ACTSGEOMETRY_IMATERIALTRACKWRITERSVC_H

#include "GaudiKernel/IInterface.h"
#include "Acts/EventData/TrackParameters.hpp"

namespace Acts {

class MaterialTrack;
//class MaterialProperties;

class IMaterialTrackWriterSvc : virtual public IInterface {
public:
    
  DeclareInterfaceID(IMaterialTrackWriterSvc, 1, 0);

  IMaterialTrackWriterSvc() {;}
    
  void
  virtual
  write(const MaterialTrack& mTrack) = 0;

};

}


#endif 
