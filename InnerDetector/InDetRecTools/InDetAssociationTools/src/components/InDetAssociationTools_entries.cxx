
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "InDetAssociationTools/InDetPRD_AssociationToolGangedPixels.h"
#include "InDetAssociationTools/InDetPRD_Provider.h"

DECLARE_NAMESPACE_TOOL_FACTORY( InDet, InDetPRD_AssociationToolGangedPixels )
DECLARE_NAMESPACE_TOOL_FACTORY( InDet, InDetPRD_Provider )

DECLARE_FACTORY_ENTRIES( InDetAssociationTools )
{
	DECLARE_NAMESPACE_TOOL( InDet, InDetPRD_AssociationToolGangedPixels )
	DECLARE_NAMESPACE_TOOL( InDet, InDetPRD_Provider )
}




