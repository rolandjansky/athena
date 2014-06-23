#include "GaudiKernel/DeclareFactoryEntries.h"
#include "JetTagCalibration/CalibrationBroker.h"

using namespace Analysis ;

DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, CalibrationBroker )

/** factory entries need to have the name of the package */
DECLARE_FACTORY_ENTRIES( JetTagCalibration )
{
  DECLARE_NAMESPACE_TOOL( Analysis, CalibrationBroker );
}
