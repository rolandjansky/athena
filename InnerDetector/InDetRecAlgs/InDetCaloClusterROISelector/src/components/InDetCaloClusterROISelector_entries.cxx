
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "InDetCaloClusterROISelector/CaloClusterROI_Selector.h"

using namespace InDet;

DECLARE_ALGORITHM_FACTORY( CaloClusterROI_Selector  )
	
DECLARE_FACTORY_ENTRIES( InDetCaloClusterROISelector )
{
  DECLARE_ALGORITHM( CaloClusterROI_Selector  )
}
