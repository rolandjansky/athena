#include "../../ZeeValidation/ZeeValidationMonitoringTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_NAMESPACE_TOOL_FACTORY( ZeeValidation, ZeeValidationMonitoringTool )

DECLARE_FACTORY_ENTRIES( ZeeValidation ) {
    DECLARE_NAMESPACE_TOOL( ZeeValidation, ZeeValidationMonitoringTool )
}
