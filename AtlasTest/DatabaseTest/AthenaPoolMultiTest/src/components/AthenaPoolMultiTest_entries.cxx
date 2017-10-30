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

