#include "CSC_Digitization/CscDigitBuilder.h"
#include "CSC_Digitization/CscDigitizationTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

DECLARE_ALGORITHM_FACTORY( CscDigitBuilder )

DECLARE_TOOL_FACTORY( CscDigitizationTool )

DECLARE_FACTORY_ENTRIES(CSC_Digitization) {
  DECLARE_ALGORITHM(CscDigitBuilder)
  DECLARE_TOOL( CscDigitizationTool )
}

