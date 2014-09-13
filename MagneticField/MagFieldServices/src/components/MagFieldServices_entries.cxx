#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MagFieldServices/AtlasFieldSvc.h"
#include "MagFieldServices/H8FieldSvc.h"

DECLARE_NAMESPACE_SERVICE_FACTORY( MagField , AtlasFieldSvc    )
DECLARE_NAMESPACE_SERVICE_FACTORY( MagField , H8FieldSvc       )

DECLARE_FACTORY_ENTRIES( MagFieldServices ) {
  DECLARE_NAMESPACE_SERVICE( MagField ,  AtlasFieldSvc         )
  DECLARE_NAMESPACE_SERVICE( MagField ,  H8FieldSvc            )
}
