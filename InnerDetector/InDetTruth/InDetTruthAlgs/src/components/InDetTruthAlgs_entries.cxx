#include "GaudiKernel/DeclareFactoryEntries.h"
#include "InDetTruthAlgs/PRD_MultiTruthMaker.h"
#include "InDetTruthAlgs/InDetDetailedTrackTruthMaker.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( InDet, InDetDetailedTrackTruthMaker )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( InDet, PRD_MultiTruthMaker )

DECLARE_FACTORY_ENTRIES( InDetTruthAlgs )
{
  DECLARE_NAMESPACE_ALGORITHM( InDet, InDetDetailedTrackTruthMaker )
  DECLARE_NAMESPACE_ALGORITHM( InDet, PRD_MultiTruthMaker )
}
