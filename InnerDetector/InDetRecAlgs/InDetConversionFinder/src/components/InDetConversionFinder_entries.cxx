#include "InDetConversionFinder/ConversionFinder.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

using namespace InDet;

DECLARE_ALGORITHM_FACTORY( ConversionFinder )

DECLARE_FACTORY_ENTRIES(InDetConversionFinder) 
{
  DECLARE_ALGORITHM( ConversionFinder )
}
