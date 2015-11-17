#include "GaudiKernel/DeclareFactoryEntries.h"

#include "ZdcByteStream/ZdcByteStreamCnv.h"
#include "ZdcByteStream/ZdcByteStreamTool.h"
#include "ZdcByteStream/ZdcDataAccessV2.h"
#include "ZdcByteStream/ZdcSrcIdMap.h"
#include "ZdcByteStream/ZdcUserHeader.h"
#include "ZdcByteStream/ZdcSubBlock.h"
#include "ZdcByteStream/ZdcPpmSubBlock.h"
#include "ZdcByteStream/ZdcPpmCompression.h"
#include "ZdcByteStream/ZdcDefs.h"
#include "ZdcByteStream/ZdcByteStreamTester.h"
#include "ZdcByteStream/ZdcByteStreamRawData.h"
#include "ZdcByteStream/ZdcByteStreamRawDataV2.h"
#include "ZdcByteStream/ZdcByteStreamReadV1V2Tool.h"
#include "ZdcByteStream/ZdcL1CaloErrorByteStreamTool.h"


DECLARE_TOOL_FACTORY      (ZdcByteStreamTool)
DECLARE_TOOL_FACTORY      (ZdcByteStreamReadV1V2Tool)
DECLARE_TOOL_FACTORY      (ZdcL1CaloErrorByteStreamTool)
DECLARE_TOOL_FACTORY (ZdcDataAccessV2)
DECLARE_CONVERTER_FACTORY (ZdcByteStreamCnv)
DECLARE_ALGORITHM_FACTORY (ZdcByteStreamTester)
DECLARE_ALGORITHM_FACTORY (ZdcByteStreamRawData)
DECLARE_ALGORITHM_FACTORY (ZdcByteStreamRawDataV2)

DECLARE_FACTORY_ENTRIES (ZdcByteStream) {
	 DECLARE_CONVERTER (ZdcByteStreamCnv)
	 DECLARE_TOOL      (ZdcByteStreamTool)
	 DECLARE_TOOL      (ZdcByteStreamReadV1V2Tool)
	 DECLARE_TOOL    (ZdcL1CaloErrorByteStreamTool)
	 DECLARE_TOOL (ZdcDataAccessV2)
	 DECLARE_ALGORITHM (ZdcByteStreamTester)
	 DECLARE_ALGORITHM (ZdcByteStreamRawData)
	 DECLARE_ALGORITHM (ZdcByteStreamRawDataV2)
}

