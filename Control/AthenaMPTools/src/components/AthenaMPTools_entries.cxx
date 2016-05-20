#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../SharedEvtQueueProvider.h"
#include "../SharedEvtQueueConsumer.h"
#include "../SharedReaderTool.h"
#include "../SharedWriterTool.h"
#include "../FileSchedulingTool.h"
#include "../EvtRangeScatterer.h"
#include "../EvtRangeProcessor.h"

DECLARE_TOOL_FACTORY( SharedEvtQueueProvider )
DECLARE_TOOL_FACTORY( SharedEvtQueueConsumer )
DECLARE_TOOL_FACTORY( SharedReaderTool )
DECLARE_TOOL_FACTORY( SharedWriterTool )
DECLARE_TOOL_FACTORY( FileSchedulingTool )
DECLARE_TOOL_FACTORY( EvtRangeScatterer )
DECLARE_TOOL_FACTORY( EvtRangeProcessor )

// Much ugliness to only build SharedHiveEvtQueueConsumer for Hive builds

#ifdef ATHENAHIVE

#include "../SharedHiveEvtQueueConsumer.h"
DECLARE_TOOL_FACTORY( SharedHiveEvtQueueConsumer )

DECLARE_FACTORY_ENTRIES ( AthenaMPTools ) {
  DECLARE_TOOL( SharedEvtQueueProvider );
  DECLARE_TOOL( SharedEvtQueueConsumer );
  DECLARE_TOOL( SharedHiveEvtQueueConsumer );
  DECLARE_TOOL( SharedReaderTool );
  DECLARE_TOOL( SharedWriterTool );
  DECLARE_TOOL( FileSchedulingTool );
  DECLARE_TOOL( EvtRangeScatterer );
  DECLARE_TOOL( EvtRangeProcessor );
}

#else

DECLARE_FACTORY_ENTRIES ( AthenaMPTools ) {
  DECLARE_TOOL( SharedEvtQueueProvider );
  DECLARE_TOOL( SharedEvtQueueConsumer );
  DECLARE_TOOL( SharedReaderTool );
  DECLARE_TOOL( SharedWriterTool );
  DECLARE_TOOL( FileSchedulingTool );
  DECLARE_TOOL( EvtRangeScatterer );
  DECLARE_TOOL( EvtRangeProcessor );
}

#endif


