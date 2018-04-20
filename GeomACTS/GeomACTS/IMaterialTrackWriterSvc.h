#ifndef GEOMACTS_IMATERIALTRACKWRITER_SERVICE
#define GEOMACTS_IMATERIALTRACKWRITER_SERVICE

#include "GaudiKernel/IInterface.h"
#include "ACTS/EventData/TrackParameters.hpp"

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
