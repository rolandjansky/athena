#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrkGlobalChi2AlignTools/GlobalChi2AlignTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Trk, GlobalChi2AlignTool )

DECLARE_FACTORY_ENTRIES( TrkGlobalChi2AlignTools )
{
    DECLARE_NAMESPACE_ALGORITHM( Trk, GlobalChi2AlignTool );
}
