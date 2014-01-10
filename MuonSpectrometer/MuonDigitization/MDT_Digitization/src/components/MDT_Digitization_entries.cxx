#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MDT_Digitization/MdtDigitizationTool.h"
#include "MDT_Digitization/MDT_Digitizer.h"
#include "MDT_Digitization/MDT_Response_DigiTool.h"
#include "MDT_Digitization/RT_Relation_DigiTool.h"
#include "MDT_Digitization/RT_Relation_DB_DigiTool.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

DECLARE_ALGORITHM_FACTORY ( MDT_Digitizer )

DECLARE_TOOL_FACTORY( MDT_Response_DigiTool )
DECLARE_TOOL_FACTORY( RT_Relation_DigiTool )
DECLARE_TOOL_FACTORY( RT_Relation_DB_DigiTool )
DECLARE_TOOL_FACTORY( MdtDigitizationTool )

DECLARE_FACTORY_ENTRIES(MDT_Digitization) {
  DECLARE_ALGORITHM( MDT_Digitizer )

  DECLARE_TOOL(  MDT_Response_DigiTool )
  DECLARE_TOOL(  RT_Relation_DigiTool )
  DECLARE_TOOL(  RT_Relation_DB_DigiTool )
  DECLARE_TOOL(  MdtDigitizationTool )

}
