#include "TRT_ConditionsAlgs/TRTCondWrite.h"
#include "TRT_ConditionsAlgs/TRTStrawAlign.h"
#include "TRT_ConditionsAlgs/TRTStrawStatusWrite.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( TRTCondWrite )
DECLARE_ALGORITHM_FACTORY( TRTStrawAlign )
DECLARE_ALGORITHM_FACTORY( TRTStrawStatusWrite )

DECLARE_FACTORY_ENTRIES( TRT_ConditionsAlgs ) 
{
  DECLARE_ALGORITHM( TRTCondWrite )
  DECLARE_ALGORITHM( TRTStrawAlign )
  DECLARE_ALGORITHM( TRTStrawStatusWrite )
}



