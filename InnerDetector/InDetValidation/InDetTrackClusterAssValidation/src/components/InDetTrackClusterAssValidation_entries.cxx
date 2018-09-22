#include "GaudiKernel/DeclareFactoryEntries.h"
#include "InDetTrackClusterAssValidation/TrackClusterAssValidation.h"
#include "InDetTrackClusterAssValidation/TrackClusterAssValidationITk.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY(InDet, TrackClusterAssValidation )
DECLARE_NAMESPACE_ALGORITHM_FACTORY(InDet, TrackClusterAssValidationITk )

DECLARE_FACTORY_ENTRIES( InDetTrackClusterAssValidation )
{
  DECLARE_NAMESPACE_ALGORITHM(InDet, TrackClusterAssValidation )
  DECLARE_NAMESPACE_ALGORITHM(InDet, TrackClusterAssValidationITk )
}


