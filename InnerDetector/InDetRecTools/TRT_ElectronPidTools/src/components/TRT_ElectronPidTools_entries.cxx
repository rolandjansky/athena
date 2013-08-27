#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TRT_ElectronPidTools/TRT_ElectronPidTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( InDet, TRT_ElectronPidTool )

DECLARE_FACTORY_ENTRIES( TRT_ElectronPidTools )
{
	DECLARE_NAMESPACE_TOOL( InDet, TRT_ElectronPidTool )
}

