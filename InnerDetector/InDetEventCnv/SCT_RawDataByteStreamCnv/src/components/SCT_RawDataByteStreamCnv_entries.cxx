#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../SCTRawContByteStreamCnv.h"
DECLARE_CONVERTER_FACTORY( SCTRawContByteStreamCnv )

#include "../SCTRawContByteStreamService.h"
DECLARE_SERVICE_FACTORY( SCTRawContByteStreamService )  

#include "../SCTRawDataProvider.h"
DECLARE_ALGORITHM_FACTORY( SCTRawDataProvider )

#include "../SCTRawDataProviderTool.h"
DECLARE_TOOL_FACTORY( SCTRawDataProviderTool)  

#include "../SCT_RodDecoder.h"
DECLARE_TOOL_FACTORY( SCT_RodDecoder )

#include "../SCT_RodEncoder.h"
DECLARE_TOOL_FACTORY( SCT_RodEncoder )

DECLARE_FACTORY_ENTRIES(SCT_RawDataByteStreamCnv) {
  DECLARE_CONVERTER( SCTRawContByteStreamCnv )
  DECLARE_SERVICE  ( SCTRawContByteStreamService )
  DECLARE_ALGORITHM( SCTRawDataProvider )
  DECLARE_TOOL     ( SCTRawDataProviderTool )
  DECLARE_TOOL     ( SCT_RodDecoder ) 
  DECLARE_TOOL     ( SCT_RodEncoder ) 
}
