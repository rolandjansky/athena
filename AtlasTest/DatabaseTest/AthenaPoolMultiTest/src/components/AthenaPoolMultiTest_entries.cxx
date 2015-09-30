/**
 * @file AthenaPoolMultiTest_entries.cxx
 * @brief Required by Gaudi class factory
 */
  
#include "../StoreDump.h"
#include "../PassAllFilter.h"
#include "../PassNoneFilter.h"
#include "../AddTrigMap.h"
#include "../EventSplit.h"
#include "../EventTagReader.h"
#include "../EventTagWriter.h"
#include "../TagEventTagWriter.h"
#include "../RunEventTagWriter.h"
#include "../RunEventMetaWriter.h"
#include "../DummyLumirangeTool.h"
#include "../UserMetadataExample.h"
#include "../UserMetadataRestoreExample.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY(StoreDump)
DECLARE_ALGORITHM_FACTORY(PassAllFilter)
DECLARE_ALGORITHM_FACTORY(PassNoneFilter)
DECLARE_ALGORITHM_FACTORY(AddTrigMap)
DECLARE_ALGORITHM_FACTORY(EventSplit)
DECLARE_ALGORITHM_FACTORY(EventTagReader)
DECLARE_ALGORITHM_FACTORY(EventTagWriter)
DECLARE_ALGORITHM_FACTORY(TagEventTagWriter)
DECLARE_ALGORITHM_FACTORY(RunEventTagWriter)
DECLARE_ALGORITHM_FACTORY(RunEventMetaWriter)
DECLARE_ALGORITHM_FACTORY(DummyLumirangeTool)
DECLARE_ALGORITHM_FACTORY(UserMetadataExample)
DECLARE_ALGORITHM_FACTORY(UserMetadataRestoreExample)

DECLARE_FACTORY_ENTRIES(AthenaPoolMultiTest) {
   DECLARE_ALGORITHM(StoreDump)
   DECLARE_ALGORITHM(PassAllFilter)
   DECLARE_ALGORITHM(PassNoneFilter)
   DECLARE_ALGORITHM(AddTrigMap)
   DECLARE_ALGORITHM(EventSplit)
   DECLARE_ALGORITHM(EventTagReader)
   DECLARE_ALGORITHM(EventTagWriter)
   DECLARE_ALGORITHM(TagEventTagWriter)
   DECLARE_ALGORITHM(RunEventTagWriter)
   DECLARE_ALGORITHM(RunEventMetaWriter)
   DECLARE_ALGORITHM(DummyLumirangeTool)
   DECLARE_ALGORITHM(UserMetadataExample)
   DECLARE_ALGORITHM(UserMetadataRestoreExample)
}
