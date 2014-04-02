#include "GaudiKernel/DeclareFactoryEntries.h"
#include "InDetAlignDBTools/SiTrkAlignDBTool.h"
#include "InDetAlignDBTools/TRTTrkAlignDBTool.h"
#include "InDetAlignDBTools/InDetTrkAlignDBTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( InDet, SiTrkAlignDBTool )
DECLARE_NAMESPACE_TOOL_FACTORY( InDet, TRTTrkAlignDBTool )
DECLARE_NAMESPACE_TOOL_FACTORY( InDet, InDetTrkAlignDBTool )

DECLARE_FACTORY_ENTRIES( InDetAlignDBTools )
{
	DECLARE_NAMESPACE_TOOL( InDet, SiTrkAlignDBTool )
	DECLARE_NAMESPACE_TOOL( InDet, TRTTrkAlignDBTool )
	DECLARE_NAMESPACE_TOOL( InDet, InDetTrkAlignDBTool )
}
