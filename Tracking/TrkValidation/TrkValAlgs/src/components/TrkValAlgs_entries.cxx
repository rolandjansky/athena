#include "GaudiKernel/DeclareFactoryEntries.h"


#include "TrkValAlgs/EventDataModelMonitor.h"
#include "TrkValAlgs/TrackValidationNtupleWriter.h"
//#include "TrkValAlgs/RecMomentumQualityValidation.h"
#include "TrkValAlgs/SingleTrackDiffAlg.h"
//#include "TrkValAlgs/MaterialOnTrackValidation.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( Trk, EventDataModelMonitor )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( Trk, TrackValidationNtupleWriter )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( Trk, SingleTrackDiffAlg )
//DECLARE_NAMESPACE_ALGORITHM_FACTORY( Trk, MaterialOnTrackValidation )
//DECLARE_NAMESPACE_ALGORITHM_FACTORY( Trk, RecMomentumQualityValidation )

DECLARE_FACTORY_ENTRIES( TrkValAlgs )
{
  //    DECLARE_NAMESPACE_ALGORITHM( Trk, EventDataModelMonitor )
  DECLARE_NAMESPACE_ALGORITHM( Trk, TrackValidationNtupleWriter )
    DECLARE_NAMESPACE_ALGORITHM( Trk, SingleTrackDiffAlg )
  //  DECLARE_NAMESPACE_ALGORITHM( Trk, MaterialOnTrackValidation )
  //  DECLARE_NAMESPACE_ALGORITHM( Trk, RecMomentumQualityValidation )

}
