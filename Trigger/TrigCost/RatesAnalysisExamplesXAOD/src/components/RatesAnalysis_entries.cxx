
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../ExampleRatesEmulation.h"
#include "../ExampleRatesFullMenu.h"

DECLARE_ALGORITHM_FACTORY( ExampleRatesEmulation )
DECLARE_ALGORITHM_FACTORY( ExampleRatesFullMenu )

DECLARE_FACTORY_ENTRIES( RatesAnalysisExamplesXAOD ) 
{
  DECLARE_ALGORITHM( ExampleRatesEmulation );
  DECLARE_ALGORITHM( ExampleRatesFullMenu );
}
