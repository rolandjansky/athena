#include "GaudiKernel/DeclareFactoryEntries.h"

#include "RecBackgroundAlgs/BackgroundWordFiller.h"
#include "RecBackgroundAlgs/BackgroundWordTest.h"
#include "RecBackgroundAlgs/BeamBackgroundFiller.h"

DECLARE_ALGORITHM_FACTORY( BackgroundWordFiller )
DECLARE_ALGORITHM_FACTORY( BackgroundWordTest )
DECLARE_ALGORITHM_FACTORY( BeamBackgroundFiller )

DECLARE_FACTORY_ENTRIES( RecBackgroundAlgs ) {
  DECLARE_ALGORITHM( BackgroundWordFiller )
  DECLARE_ALGORITHM( BackgroundWordTest )
  DECLARE_ALGORITHM( BeamBackgroundFiller )
}
