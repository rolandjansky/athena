/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRY_IEXCELLWRITERSVC_H
#define ACTSGEOMETRY_IEXCELLWRITERSVC_H

#include "GaudiKernel/IInterface.h"
#include "Acts/EventData/TrackParameters.hpp"

namespace Acts {

template <class>
class ExtrapolationCell;


class IExCellWriterSvc : virtual public IInterface {
public:
    
  DeclareInterfaceID(IExCellWriterSvc, 1, 0);

  IExCellWriterSvc() {;}
    
  void
  virtual
  store(std::vector<Acts::ExtrapolationCell<Acts::TrackParameters>>& ecells) = 0;

};

}


#endif 
