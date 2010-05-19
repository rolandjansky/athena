#include "GaudiKernel/DeclareFactoryEntries.h"

#include "BCM_RawDataByteStreamCnv/BCM_RawContByteStreamCnv.h"
DECLARE_CONVERTER_FACTORY( BCM_RawContByteStreamCnv )

#include "BCM_RawDataByteStreamCnv/BCM_RawContByteStreamTool.h"
DECLARE_TOOL_FACTORY( BCM_RawContByteStreamTool )

#include "BCM_RawDataByteStreamCnv/BCM_RawDataProvider.h"
DECLARE_ALGORITHM_FACTORY( BCM_RawDataProvider )

#include "BCM_RawDataByteStreamCnv/BCM_RawDataProviderTool.h"
DECLARE_TOOL_FACTORY( BCM_RawDataProviderTool)  

#include "BCM_RawDataByteStreamCnv/BCM_RodDecoder.h"
DECLARE_TOOL_FACTORY( BCM_RodDecoder )

DECLARE_FACTORY_ENTRIES(BCM_RawDataByteStreamCnv) {
  DECLARE_CONVERTER( BCM_RawContByteStreamCnv )
  DECLARE_TOOL     ( BCM_RawContByteStreamTool )
  DECLARE_ALGORITHM( BCM_RawDataProvider )
  DECLARE_TOOL     ( BCM_RawDataProviderTool )
  DECLARE_TOOL     ( BCM_RodDecoder )
}



