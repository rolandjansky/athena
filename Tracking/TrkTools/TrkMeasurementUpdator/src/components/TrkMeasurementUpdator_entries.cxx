#include "TrkMeasurementUpdator/KalmanUpdator.h"
#include "TrkMeasurementUpdator/KalmanUpdatorSMatrix.h"
#include "TrkMeasurementUpdator/KalmanUpdatorAmg.h"
#include "TrkMeasurementUpdator/KalmanWeightUpdator.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

// using namespace Trk;

DECLARE_NAMESPACE_TOOL_FACTORY( Trk, KalmanUpdator )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, KalmanUpdatorSMatrix )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, KalmanUpdatorAmg )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, KalmanWeightUpdator )

DECLARE_FACTORY_ENTRIES(TrkMeasurementUpdator)
{ // in this package name these methods are registered 
  DECLARE_NAMESPACE_TOOL(Trk, KalmanUpdator)
  DECLARE_NAMESPACE_TOOL(Trk, KalmanUpdatorSMatrix)
  DECLARE_NAMESPACE_TOOL(Trk, KalmanUpdatorAmg)
  DECLARE_NAMESPACE_TOOL(Trk, KalmanWeightUpdator)
}

