/** @file AthenaPoolExampleAlgorithms_entries.cxx
 *  @brief This file is needed for linker to load all the components of the library.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: AthenaPoolExampleAlgorithms_entries.cxx,v 1.16 2008-12-10 21:28:12 gemmeren Exp $
 **/

#include "../WriteData.h"
#include "../ReWriteData.h"
#include "../ReadData.h"
#include "../ReadBs.h"
#include "../WriteCond.h"
#include "../ReadCond.h"
#include "../ReadMeta.h"
#include "../WriteTag.h"
#include "../QueryTag.h"
#include "../PassNoneFilter.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY(AthPoolEx, WriteData)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(AthPoolEx, ReWriteData)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(AthPoolEx, ReadData)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(AthPoolEx, ReadBs)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(AthPoolEx, WriteCond)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(AthPoolEx, ReadCond)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(AthPoolEx, WriteTag)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(AthPoolEx, PassNoneFilter)
DECLARE_NAMESPACE_TOOL_FACTORY(AthPoolEx, ReadMeta)
DECLARE_NAMESPACE_TOOL_FACTORY(AthPoolEx, QueryTag)

DECLARE_FACTORY_ENTRIES(AthenaPoolExampleAlgorithms) {
   DECLARE_NAMESPACE_ALGORITHM(AthPoolEx, WriteData);
   DECLARE_NAMESPACE_ALGORITHM(AthPoolEx, ReWriteData);
   DECLARE_NAMESPACE_ALGORITHM(AthPoolEx, ReadData);
   DECLARE_NAMESPACE_ALGORITHM(AthPoolEx, ReadBs);
   DECLARE_NAMESPACE_ALGORITHM(AthPoolEx, WriteCond);
   DECLARE_NAMESPACE_ALGORITHM(AthPoolEx, ReadCond);
   DECLARE_NAMESPACE_ALGORITHM(AthPoolEx, WriteTag);
   DECLARE_NAMESPACE_ALGORITHM(AthPoolEx, PassNoneFilter);
   DECLARE_NAMESPACE_ALGTOOL(AthPoolEx, ReadMeta);
   DECLARE_NAMESPACE_ALGTOOL(AthPoolEx, QueryTag);
}
