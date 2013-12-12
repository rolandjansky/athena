#include "GaudiKernel/DeclareFactoryEntries.h"
#include "InDetAlignNtupleTools/SimpleIDNtupleTool.h"
#include "InDetAlignNtupleTools/DetailedIDNtupleTool.h"


DECLARE_NAMESPACE_TOOL_FACTORY( InDet, SimpleIDNtupleTool )
DECLARE_NAMESPACE_TOOL_FACTORY( InDet, DetailedIDNtupleTool )


DECLARE_FACTORY_ENTRIES( InDetAlignNtupleTools )
{
	DECLARE_NAMESPACE_TOOL( InDet, SimpleIDNtupleTool )
        DECLARE_NAMESPACE_TOOL( InDet, DetailedIDNtupleTool )
}
