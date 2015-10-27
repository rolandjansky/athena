#include "GaudiKernel/DeclareFactoryEntries.h"
#include "CalibrationDataInterface/CalibrationDataInterfaceTool.h"
#include "CalibrationDataInterface/CalibrationDataUpdater.h"
#include "CalibrationDataInterface/CalibrationDataInterfaceTester.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( Analysis, CalibrationDataUpdater )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( Analysis, CalibrationDataInterfaceTester )
DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, CalibrationDataInterfaceTool )

DECLARE_FACTORY_ENTRIES( CalibrationDataInterface )
{
	DECLARE_NAMESPACE_ALGORITHM( Analysis, CalibrationDataUpdater )
	DECLARE_NAMESPACE_ALGORITHM( Analysis, CalibrationDataInterfaceTester )
	DECLARE_NAMESPACE_TOOL( Analysis, CalibrationDataInterfaceTool )
}

