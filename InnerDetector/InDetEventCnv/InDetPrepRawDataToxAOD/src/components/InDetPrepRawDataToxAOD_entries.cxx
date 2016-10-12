#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../TRT_PrepDataToxAOD.h"
#include "../SCT_PrepDataToxAOD.h"
#include "../SCT_RawDataToxAOD.h"
#include "../PixelPrepDataToxAOD.h"


//using namespace InDet;

DECLARE_ALGORITHM_FACTORY( TRT_PrepDataToxAOD )
DECLARE_ALGORITHM_FACTORY( SCT_PrepDataToxAOD )
DECLARE_ALGORITHM_FACTORY( SCT_RawDataToxAOD )
DECLARE_ALGORITHM_FACTORY( PixelPrepDataToxAOD )

DECLARE_FACTORY_ENTRIES( InDetPrepRawDataToxAOD ) 
{
  DECLARE_ALGORITHM( TRT_PrepDataToxAOD )
  DECLARE_ALGORITHM( SCT_PrepDataToxAOD )
  DECLARE_ALGORITHM( SCT_RawDataToxAOD )
  DECLARE_ALGORITHM( PixelPrepDataToxAOD )
}
