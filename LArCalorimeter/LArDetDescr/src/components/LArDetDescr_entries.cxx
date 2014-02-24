//====================================================================

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "LArDetDescr/LArRecoMaterialTool.h"
#include "LArDetDescr/LArRecoSimpleGeomTool.h"

DECLARE_TOOL_FACTORY( LArRecoMaterialTool )
DECLARE_TOOL_FACTORY( LArRecoSimpleGeomTool )

DECLARE_FACTORY_ENTRIES	( LArDetDescr ) {
    DECLARE_TOOL( LArRecoMaterialTool );
    DECLARE_TOOL( LArRecoSimpleGeomTool );
}


