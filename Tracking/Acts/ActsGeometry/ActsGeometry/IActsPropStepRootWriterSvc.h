/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRY_IACTSPROPSTEPROOTWRITERSVC_H
#define ACTSGEOMETRY_IACTSPROPSTEPROOTWRITERSVC_H

#include "GaudiKernel/IInterface.h"
#include "Acts/EventData/TrackParameters.hpp"

namespace Acts {
  namespace detail {
    struct Step;
  }
}


class IActsPropStepRootWriterSvc : virtual public IInterface {
public:
    
  DeclareInterfaceID(IActsPropStepRootWriterSvc, 1, 0);

  IActsPropStepRootWriterSvc() {;}
    
  void
  virtual
  write(const std::vector<Acts::detail::Step>& steps) = 0;

};


#endif 
