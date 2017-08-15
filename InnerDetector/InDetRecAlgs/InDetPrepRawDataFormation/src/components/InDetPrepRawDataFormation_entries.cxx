#include "InDetPrepRawDataFormation/PixelClusterization.h"
#include "InDetPrepRawDataFormation/SCT_Clusterization.h"
#include "InDetPrepRawDataFormation/TRT_RIO_Maker.h"
#include "InDetPrepRawDataFormation/CacheCreator.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

using namespace InDet;

DECLARE_NAMESPACE_ALGORITHM_FACTORY( InDet, PixelClusterization)
DECLARE_NAMESPACE_ALGORITHM_FACTORY( InDet, SCT_Clusterization )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( InDet, TRT_RIO_Maker      )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( InDet, CacheCreator      )

DECLARE_FACTORY_ENTRIES( InDetPrepRawDataFormation ) 
{
  DECLARE_NAMESPACE_ALGORITHM( InDet, PixelClusterization)
  DECLARE_NAMESPACE_ALGORITHM( InDet, SCT_Clusterization )
  DECLARE_NAMESPACE_ALGORITHM( InDet, TRT_RIO_Maker      )
  DECLARE_NAMESPACE_ALGORITHM( InDet, CacheCreator       )
}











