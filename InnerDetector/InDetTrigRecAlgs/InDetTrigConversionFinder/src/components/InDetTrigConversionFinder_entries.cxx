#include "InDetTrigConversionFinder/TrigConversionFinder.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

using namespace InDet;

DECLARE_ALGORITHM_FACTORY( TrigConversionFinder )

DECLARE_FACTORY_ENTRIES(InDetTrigConversionFinder) 
{
  DECLARE_ALGORITHM( TrigConversionFinder )
}
