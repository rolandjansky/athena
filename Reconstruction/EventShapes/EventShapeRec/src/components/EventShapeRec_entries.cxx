
#include "EventShapeRec/EventShapeCalculator.h"
#include "EventShapeRec/EventEtDensityCalculator.h"
#include "EventShapeRec/EventEtDensityTester.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( EventShapeCalculator )
DECLARE_ALGORITHM_FACTORY( EventEtDensityCalculator )
DECLARE_ALGORITHM_FACTORY( EventEtDensityTester )

DECLARE_FACTORY_ENTRIES( EventShapeRec ) {
  DECLARE_ALGORITHM( EventShapeCalculator );
  DECLARE_ALGORITHM( EventEtDensityCalculator );
  DECLARE_ALGORITHM( EventEtDensityTester );
}
