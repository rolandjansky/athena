#include "TRT_CalibAlgs/TRTCalibrationMgr.h"
#include "TRT_CalibAlgs/TRT_StrawStatus.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( TRTCalibrationMgr )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( InDet, TRT_StrawStatus )
  
DECLARE_FACTORY_ENTRIES( TRT_CalibAlgs ) 
{
  DECLARE_ALGORITHM( TRTCalibrationMgr ) ;
  DECLARE_NAMESPACE_ALGORITHM( InDet, TRT_StrawStatus );
}




