#include "GaudiKernel/DeclareFactoryEntries.h"
#include "ZMassConstraint/ConstraintFit.h"
#include "../TestAthenaConstraintFit.h"

DECLARE_NAMESPACE_TOOL_FACTORY( ZMassConstraint, ConstraintFit )
DECLARE_ALGORITHM_FACTORY( TestAthenaConstraintFit )


DECLARE_FACTORY_ENTRIES( ElectronPhotonFourMomentumCorrection ){
  DECLARE_NAMESPACE_TOOL( ZMassConstraint, ConstraintFit )
  DECLARE_ALGORITHM( TestAthenaConstraintFit )
}
