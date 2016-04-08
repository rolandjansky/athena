#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TRT_TrigTrackExtensionAlg/TRT_TrigTrackExtensionAlg.h"
#include "InDetRecToolInterfaces/ITRT_TrackExtensionTool.h"


DECLARE_NAMESPACE_ALGORITHM_FACTORY( InDet, TRT_TrigTrackExtensionAlg )

DECLARE_FACTORY_ENTRIES( TRT_TrigTrackExtensionAlg )
{
  DECLARE_NAMESPACE_ALGORITHM( InDet, TRT_TrigTrackExtensionAlg )
}

