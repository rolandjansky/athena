#include "TrkDistributedKalmanFilter/DistributedKalmanFilter.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Trk, DistributedKalmanFilter )

DECLARE_FACTORY_ENTRIES(TrkDistributedKalmanFilter)
{ // in this package name these methods are registered 
  DECLARE_NAMESPACE_TOOL(Trk, DistributedKalmanFilter)
}

