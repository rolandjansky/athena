#include "GaudiKernel/DeclareFactoryEntries.h"

#include "TrigHLTResultByteStream/HLTResultByteStreamCnv.h"
#include "TrigHLTResultByteStream/HLTResultByteStreamTool.h"

DECLARE_NAMESPACE_CONVERTER_FACTORY( HLT, HLTResultByteStreamCnv )
DECLARE_NAMESPACE_TOOL_FACTORY( HLT, HLTResultByteStreamTool )

DECLARE_FACTORY_ENTRIES( TrigHLTResultByteStream ) {
  //DECLARE_CONVERTER(  HLTResultByteStreamCnv )
  DECLARE_NAMESPACE_CONVERTER(HLT, HLTResultByteStreamCnv)
  DECLARE_NAMESPACE_TOOL(HLT, HLTResultByteStreamTool)
}

