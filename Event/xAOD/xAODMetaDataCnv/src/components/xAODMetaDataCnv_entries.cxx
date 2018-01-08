// $Id: xAODMetaDataCnv_entries.cxx 670279 2015-05-27 15:18:27Z krasznaa $

// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "xAODMetaDataCnv/FileMetaDataTool.h"
#include "../FileMetaDataCreatorTool.h"
#include "../FileMetaDataMarkUpTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker, FileMetaDataTool )
DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker, FileMetaDataCreatorTool )
DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker, FileMetaDataMarkUpTool )

DECLARE_FACTORY_ENTRIES( xAODMetaDataCnv ) {

   DECLARE_NAMESPACE_TOOL( xAODMaker, FileMetaDataTool )
   DECLARE_NAMESPACE_TOOL( xAODMaker, FileMetaDataCreatorTool )
   DECLARE_NAMESPACE_TOOL( xAODMaker, FileMetaDataMarkUpTool )

}
