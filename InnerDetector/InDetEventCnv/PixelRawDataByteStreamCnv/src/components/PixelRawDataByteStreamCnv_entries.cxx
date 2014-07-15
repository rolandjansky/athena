#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../PixelRawContByteStreamCnv.h"
DECLARE_CONVERTER_FACTORY( PixelRawContByteStreamCnv )

#include "../PixelRawContByteStreamTool.h"
DECLARE_TOOL_FACTORY( PixelRawContByteStreamTool )

#include "../PixelRawDataProvider.h"
DECLARE_ALGORITHM_FACTORY( PixelRawDataProvider )

#include "../PixelRawDataProviderTool.h"
DECLARE_TOOL_FACTORY( PixelRawDataProviderTool)  

#include "../PixelRodDecoder.h"
DECLARE_TOOL_FACTORY( PixelRodDecoder )


DECLARE_FACTORY_ENTRIES(PixelRawDataByteStreamCnv) {
  DECLARE_CONVERTER( PixelRawContByteStreamCnv )
  DECLARE_TOOL     ( PixelRawContByteStreamTool )
  DECLARE_ALGORITHM( PixelRawDataProvider )
  DECLARE_TOOL     ( PixelRawDataProviderTool )
  DECLARE_TOOL     ( PixelRodDecoder )
}
