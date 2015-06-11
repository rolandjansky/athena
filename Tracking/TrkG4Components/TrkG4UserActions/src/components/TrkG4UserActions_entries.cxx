#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrkG4UserActions/GeantFollowerHelper.h"
#include "TrkG4UserActions/GeantFollowerMSHelper.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Trk , GeantFollowerHelper )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk , GeantFollowerMSHelper )

DECLARE_FACTORY_ENTRIES( TrkG4UserActions ) {
  DECLARE_NAMESPACE_TOOL( Trk ,  GeantFollowerHelper )
  DECLARE_NAMESPACE_TOOL( Trk ,  GeantFollowerMSHelper )
}
