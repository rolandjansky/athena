#include "usingns6_test1.h"

class StoreGateSvc {};


class TileCablingSvc {
public:
  virtual ~TileCablingSvc() {}
  ServiceHandle<StoreGateSvc> m_detStore;
};

namespace CLHEP { class RandGauss; }
using CLHEP::RandGauss;
 
