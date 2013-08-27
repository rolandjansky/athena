#include "InDetRecStatistics/InDetRecStatisticsAlg.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( InDet, InDetRecStatisticsAlg)

DECLARE_FACTORY_ENTRIES(InDetRecStatistics) {
  DECLARE_NAMESPACE_ALGORITHM( InDet, InDetRecStatisticsAlg )
}
