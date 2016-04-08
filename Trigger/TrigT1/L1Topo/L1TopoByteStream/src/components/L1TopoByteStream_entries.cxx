#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../L1TopoByteStreamCnv.h"
#include "../L1TopoByteStreamxAODCnv.h"
#include "../L1TopoByteStreamAuxCnv.h"
#include "../L1TopoByteStreamTool.h"

DECLARE_CONVERTER_FACTORY(L1TopoByteStreamCnv)
DECLARE_NAMESPACE_CONVERTER_FACTORY(LVL1BS, L1TopoByteStreamxAODCnv)
DECLARE_NAMESPACE_CONVERTER_FACTORY(LVL1BS, L1TopoByteStreamAuxCnv)
DECLARE_TOOL_FACTORY(L1TopoByteStreamTool)

DECLARE_FACTORY_ENTRIES(L1TopoByteStream) {
  DECLARE_CONVERTER(L1TopoByteStreamCnv)
  DECLARE_TOOL(L1TopoByteStreamTool)

  DECLARE_NAMESPACE_CONVERTER(LVL1BS, L1TopoByteStreamxAODCnv)
  DECLARE_NAMESPACE_CONVERTER(LVL1BS, L1TopoByteStreamAuxCnv)
}
