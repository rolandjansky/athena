#include "TrkMeasurementUpdator_xk/KalmanUpdator_xk.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

using namespace Trk;

DECLARE_TOOL_FACTORY( KalmanUpdator_xk )

DECLARE_FACTORY_ENTRIES( TrkMeasurementUpdator_xk )
{ 
  DECLARE_TOOL( KalmanUpdator_xk )
}

