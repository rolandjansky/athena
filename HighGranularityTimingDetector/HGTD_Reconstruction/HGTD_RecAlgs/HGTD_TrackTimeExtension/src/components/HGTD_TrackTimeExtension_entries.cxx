#include "GaudiKernel/DeclareFactoryEntries.h"
#include "HGTD_TrackTimeExtension/TrackTimeExtensionAlg.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY(HGTD, TrackTimeExtensionAlg)

DECLARE_FACTORY_ENTRIES(TrackTimeExtensionAlg) {
  DECLARE_NAMESPACE_ALGORITHM(HGTD, TrackTimeExtensionAlg)
}
