/** @file OutputStreamAthenaPool_entries.cxx
 *  @brief This file is needed for linker to load all the components of the library.
 *  @author Peter van Gemmeren <gemmeren@bnl.gov>
 **/

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../AthenaPoolOutputStreamTool.h"
#include "../MakeInputDataHeader.h"
#include "../MakeEventStreamInfo.h"
#include "../CopyEventStreamInfo.h"

DECLARE_TOOL_FACTORY(AthenaPoolOutputStreamTool)
DECLARE_TOOL_FACTORY(MakeEventStreamInfo)
DECLARE_TOOL_FACTORY(CopyEventStreamInfo)
DECLARE_ALGORITHM_FACTORY(MakeInputDataHeader)

DECLARE_FACTORY_ENTRIES(OutputStreamAthenaPool) {
   DECLARE_TOOL(AthenaPoolOutputStreamTool)
   DECLARE_TOOL(MakeEventStreamInfo)
   DECLARE_TOOL(CopyEventStreamInfo)
   DECLARE_ALGORITHM(MakeInputDataHeader)
}
