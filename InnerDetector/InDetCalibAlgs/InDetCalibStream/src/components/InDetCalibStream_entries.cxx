#include "InDetCalibStream/CalibStream.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

using namespace InDet;

DECLARE_NAMESPACE_ALGORITHM_FACTORY( InDet, CalibStream )

DECLARE_FACTORY_ENTRIES(InDetCalibStream){ 
  DECLARE_NAMESPACE_ALGORITHM( InDet, CalibStream );
}
