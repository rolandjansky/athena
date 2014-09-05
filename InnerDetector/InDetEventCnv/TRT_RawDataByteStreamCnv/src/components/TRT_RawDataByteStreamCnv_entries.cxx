#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../TRTRawContByteStreamCnv.h"
DECLARE_CONVERTER_FACTORY( TRTRawContByteStreamCnv )

#include "../TRTRawContByteStreamTool.h"
DECLARE_TOOL_FACTORY( TRTRawContByteStreamTool )  

#include "../TRTRawDataProvider.h"
DECLARE_ALGORITHM_FACTORY( TRTRawDataProvider )

#include "../TRTRawDataProviderTool.h"
DECLARE_TOOL_FACTORY( TRTRawDataProviderTool)  

#include "../TRT_RodDecoder.h"
DECLARE_TOOL_FACTORY( TRT_RodDecoder )

DECLARE_FACTORY_ENTRIES(TRT_RawDataByteStreamCnv) {
  DECLARE_CONVERTER( TRTRawContByteStreamCnv )
  DECLARE_TOOL     ( TRTRawContByteStreamTool )
  DECLARE_ALGORITHM( TRTRawDataProvider )
  DECLARE_TOOL     ( TRTRawDataProviderTool )
  DECLARE_TOOL     ( TRT_RodDecoder ) 
}
