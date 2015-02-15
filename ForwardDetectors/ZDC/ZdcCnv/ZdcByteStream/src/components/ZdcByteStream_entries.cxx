#include "GaudiKernel/DeclareFactoryEntries.h"

#include "ZdcByteStream/ZdcByteStreamCnv.h"
#include "ZdcByteStream/ZdcByteStreamTool.h"
#include "ZdcByteStream/ZdcSrcIdMap.h"
#include "ZdcByteStream/ZdcUserHeader.h"
#include "ZdcByteStream/ZdcSubBlock.h"
#include "ZdcByteStream/ZdcPpmSubBlock.h"
#include "ZdcByteStream/ZdcPpmCompression.h"
#include "ZdcByteStream/ZdcDefs.h"
#include "ZdcByteStream/ZdcByteStreamTester.h"
#include "ZdcByteStream/ZdcByteStreamRawData.h"



DECLARE_TOOL_FACTORY      (ZdcByteStreamTool)
DECLARE_CONVERTER_FACTORY (ZdcByteStreamCnv)
DECLARE_ALGORITHM_FACTORY (ZdcByteStreamTester)
DECLARE_ALGORITHM_FACTORY (ZdcByteStreamRawData)

DECLARE_FACTORY_ENTRIES (ZdcByteStream) {
	 DECLARE_CONVERTER (ZdcByteStreamCnv)
	 DECLARE_TOOL      (ZdcByteStreamTool)
	 DECLARE_ALGORITHM (ZdcByteStreamTester)
	 DECLARE_ALGORITHM (ZdcByteStreamRawData)
}

