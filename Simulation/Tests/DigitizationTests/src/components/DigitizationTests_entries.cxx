#include "GaudiKernel/DeclareFactoryEntries.h"

#include "DigitizationTests/DigiTestAlg.h"
//#include "DigitizationTests/PileUpEventInfoTest.h"
#include "../McEventCollectionTestTool.h"
#include "../PileUpEventInfoTestTool.h"
#include "../PixelRDOsTestTool.h"
#include "../SCT_RDOsTestTool.h"
#include "../TRT_RDOsTestTool.h"

DECLARE_FACTORY_ENTRIES(DigitizationTests)
{
  DECLARE_ALGTOOL(McEventCollectionTestTool)
  DECLARE_ALGTOOL(PileUpEventInfoTestTool)
  DECLARE_ALGTOOL(PixelRDOsTestTool)
  DECLARE_ALGTOOL(SCT_RDOsTestTool)
  DECLARE_ALGTOOL(TRT_RDOsTestTool)
  DECLARE_ALGORITHM(DigiTestAlg)
    //DECLARE_ALGORITHM(PileUpEventInfoTest)
}

DECLARE_TOOL_FACTORY(McEventCollectionTestTool)
DECLARE_TOOL_FACTORY(PileUpEventInfoTestTool)
DECLARE_TOOL_FACTORY(PixelRDOsTestTool)
DECLARE_TOOL_FACTORY(SCT_RDOsTestTool)
DECLARE_TOOL_FACTORY(TRT_RDOsTestTool)
DECLARE_ALGORITHM_FACTORY(DigiTestAlg)
//DECLARE_ALGORITHM_FACTORY(PileUpEventInfoTest)
