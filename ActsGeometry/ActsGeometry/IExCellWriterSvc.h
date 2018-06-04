#ifndef GEOMACTS_IEXCELLWRITER_SERVICE
#define GEOMACTS_IEXCELLWRITER_SERVICE

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
