#include "GaudiKernel/DeclareFactoryEntries.h"

#include "TrigFTKByteStream/TrigFTKByteStreamCnv.h"
#include "TrigFTKByteStream/TrigFTKByteStreamTool.h"
#include "../FTKByteStreamDecoderEncoder.h"
#include "../FTKDump.h"

DECLARE_NAMESPACE_CONVERTER_FACTORY( FTK, TrigFTKByteStreamCnv )
DECLARE_NAMESPACE_TOOL_FACTORY( FTK, TrigFTKByteStreamTool )
DECLARE_NAMESPACE_TOOL_FACTORY( FTK, FTKByteStreamDecoderEncoderTool )
DECLARE_ALGORITHM_FACTORY(FTKDump)

DECLARE_FACTORY_ENTRIES( TrigFTKByteStream ) {
  DECLARE_NAMESPACE_CONVERTER(FTK, TrigFTKByteStreamCnv)
  DECLARE_NAMESPACE_TOOL(FTK, TrigFTKByteStreamTool)
  DECLARE_NAMESPACE_TOOL(FTK, FTKByteStreamDecoderEncoderTool)
  DECLARE_ALGORITHM(FTKDump)
}

