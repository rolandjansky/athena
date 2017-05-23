#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../CopyMcEventCollection.h"
#include "../CopyObjects.h"
#include "../CopyTimings.h"
#include "../CopyTruthInfo.h"
#include "../SaveInDetObjects.h"
#include "../RemoveObjects.h"
#include "../UpdateEventInfo.h"
#include "../BSFilter.h"
#include "../G4HitMerger.h"
#include "../ByteStreamMultipleOutputStreamCopyTool.h"
#include "../DeepCopyObjects.h"
#include "../RemoveTempBkgObjects.h"

DECLARE_ALGORITHM_FACTORY( CopyMcEventCollection )
DECLARE_ALGORITHM_FACTORY( CopyObjects )
DECLARE_ALGORITHM_FACTORY( CopyTimings )
DECLARE_ALGORITHM_FACTORY( CopyTruthInfo )
DECLARE_ALGORITHM_FACTORY( SaveInDetObjects )
DECLARE_ALGORITHM_FACTORY( RemoveObjects )
DECLARE_ALGORITHM_FACTORY( UpdateEventInfo )
DECLARE_ALGORITHM_FACTORY( BSFilter )
DECLARE_ALGORITHM_FACTORY( G4HitMerger )
DECLARE_ALGORITHM_FACTORY( DeepCopyObjects )
DECLARE_ALGORITHM_FACTORY( RemoveTempBkgObjects )

DECLARE_TOOL_FACTORY( ByteStreamMultipleOutputStreamCopyTool )

DECLARE_FACTORY_ENTRIES( OverlayCommonAlgs ) {
  DECLARE_ALGORITHM( CopyMcEventCollection )
  DECLARE_ALGORITHM( CopyObjects )
  DECLARE_ALGORITHM( CopyTimings )
  DECLARE_ALGORITHM( CopyTruthInfo )
  DECLARE_ALGORITHM( SaveInDetObjects )
  DECLARE_ALGORITHM( RemoveObjects )
  DECLARE_ALGORITHM( UpdateEventInfo )
  DECLARE_ALGORITHM( BSFilter )
  DECLARE_ALGORITHM( G4HitMerger )
  DECLARE_ALGORITHM( DeepCopyObjects )  
  DECLARE_ALGORITHM( RemoveTempBkgObjects )

  DECLARE_TOOL( ByteStreamMultipleOutputStreamCopyTool )
}

