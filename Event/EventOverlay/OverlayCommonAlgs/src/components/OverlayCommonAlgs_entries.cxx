#include "GaudiKernel/DeclareFactoryEntries.h"
#include "OverlayCommonAlgs/CopyMcEventCollection.h"
#include "OverlayCommonAlgs/CopyObjects.h"
#include "OverlayCommonAlgs/SaveInDetObjects.h"
#include "OverlayCommonAlgs/RemoveObjects.h"
#include "OverlayCommonAlgs/UpdateEventInfo.h"
#include "OverlayCommonAlgs/BSFilter.h"
#include "OverlayCommonAlgs/G4HitMerger.h"
#include "OverlayCommonAlgs/ByteStreamMultipleOutputStreamCopyTool.h"
#include "OverlayCommonAlgs/DeepCopyObjects.h"
#include "OverlayCommonAlgs/RemoveTempBkgObjects.h"

DECLARE_ALGORITHM_FACTORY( CopyMcEventCollection )
DECLARE_ALGORITHM_FACTORY( CopyObjects )
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
  DECLARE_ALGORITHM( SaveInDetObjects )
  DECLARE_ALGORITHM( RemoveObjects )
  DECLARE_ALGORITHM( UpdateEventInfo )
  DECLARE_ALGORITHM( BSFilter )
  DECLARE_ALGORITHM( G4HitMerger )
  DECLARE_ALGORITHM( DeepCopyObjects )  
  DECLARE_ALGORITHM( RemoveTempBkgObjects )

  DECLARE_TOOL( ByteStreamMultipleOutputStreamCopyTool )
}

