#include "GaudiKernel/DeclareFactoryEntries.h"
#include "ISF_Algorithms/SimKernel.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( ISF , SimKernel )

DECLARE_FACTORY_ENTRIES( ISF_Algorithms ) {
  DECLARE_NAMESPACE_ALGORITHM( ISF ,  SimKernel )
}   
