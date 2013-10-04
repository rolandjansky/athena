/***************************************************************************
 iPatGeometryTools
 ATLAS Collaboration
 ***************************************************************************/

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "iPatGeometryTools/DetectorSelection.h"
#include "iPatGeometryTools/LayerAllocator.h"
#include "iPatGeometryTools/LayerNumberAllocator.h"
#include "iPatGeometryTools/SiDetectorLocator.h"

DECLARE_TOOL_FACTORY( DetectorSelection )
DECLARE_TOOL_FACTORY( LayerAllocator )
DECLARE_TOOL_FACTORY( LayerNumberAllocator )
DECLARE_TOOL_FACTORY( SiDetectorLocator )

DECLARE_FACTORY_ENTRIES( iPatGeometryTools ) 
{
    DECLARE_TOOL( DetectorSelection )	
    DECLARE_TOOL( LayerAllocator )
    DECLARE_TOOL( LayerNumberAllocator )
    DECLARE_TOOL( SiDetectorLocator )
}

