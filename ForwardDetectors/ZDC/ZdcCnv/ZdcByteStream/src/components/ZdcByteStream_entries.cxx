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

