#include "GaudiKernel/DeclareFactoryEntries.h"
#include "PixelToTPIDTool/PixelToTPIDTool.h"
#include "PixelToTPIDTool/PixeldEdxTestAlg.h"
#include "PixelToTPIDTool/PixeldEdxAODFix.h"

DECLARE_NAMESPACE_TOOL_FACTORY( InDet, PixelToTPIDTool )
DECLARE_ALGORITHM_FACTORY(PixeldEdxTestAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(InDet,PixeldEdxAODFix)
 
DECLARE_FACTORY_ENTRIES( PixelToTPIDTools ) 
{ 
       DECLARE_NAMESPACE_TOOL( InDet, PixelToTPIDTool ) 
       DECLARE_ALGORITHM(PixeldEdxTestAlg)
       DECLARE_NAMESPACE_ALGORITHM(InDet, PixeldEdxAODFix)

} 

