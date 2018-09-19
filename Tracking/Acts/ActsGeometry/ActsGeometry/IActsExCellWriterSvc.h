/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRY_IACTSEXCELLWRITERSVC_H
#define ACTSGEOMETRY_IACTSEXCELLWRITERSVC_H

#include "GaudiKernel/IInterface.h"
#include "Acts/EventData/TrackParameters.hpp"

namespace Acts {

template <class>
class ExtrapolationCell;

}


class IActsExCellWriterSvc : virtual public IInterface {
public:
    
  DeclareInterfaceID(IActsExCellWriterSvc, 1, 0);

  IActsExCellWriterSvc() {;}
    
  void
  virtual
  store(std::vector<Acts::ExtrapolationCell<Acts::TrackParameters>>& ecells) = 0;

};


#endif 
