#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrkAssociationTools/PRD_AssociationTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Trk, PRD_AssociationTool )

DECLARE_FACTORY_ENTRIES( TrkAssociationTools )
{
	DECLARE_NAMESPACE_TOOL(Trk, PRD_AssociationTool)
}




