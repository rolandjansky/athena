#include "InDetTrigRawDataProvider/TrigTRTRawDataProvider.h"
#include "InDetTrigRawDataProvider/TrigPixRawDataProvider.h"
#include "InDetTrigRawDataProvider/TrigSCTRawDataProvider.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

//using namespace InDet;

DECLARE_NAMESPACE_TOOL_FACTORY( InDet, TrigPixRawDataProvider )
DECLARE_NAMESPACE_TOOL_FACTORY( InDet, TrigSCTRawDataProvider )
DECLARE_NAMESPACE_TOOL_FACTORY( InDet, TrigTRTRawDataProvider )

DECLARE_FACTORY_ENTRIES( InDetTrigRawDataProvider )
{
  DECLARE_NAMESPACE_TOOL( InDet, TrigPixRawDataProvider )
  DECLARE_NAMESPACE_TOOL( InDet, TrigSCTRawDataProvider )
  DECLARE_NAMESPACE_TOOL( InDet, TrigTRTRawDataProvider )
}


