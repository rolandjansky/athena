#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TRT_TrackExtensionAlg/TRT_TrackExtensionAlg.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY(InDet, TRT_TrackExtensionAlg )

DECLARE_FACTORY_ENTRIES( TRT_TrackExtensionAlg )
{
  DECLARE_NAMESPACE_ALGORITHM(InDet,  TRT_TrackExtensionAlg )
}

