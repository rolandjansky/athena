/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../L1CaloDecoder.h"
#include "../FakeRoI.h"
#include "../FakeRoIView.h"
//#include "../RoIGraph.h"
#include "../L1Decoder.h"
#include "../FakeCTP.h"
#include "../CTPUnpackingTool.h"
#include "../CTPUnpackingEmulationTool.h"
#include "../EMRoIsUnpackingTool.h"
#include "../RoIsUnpackingEmulationTool.h"
#include "../MURoIsUnpackingTool.h"

DECLARE_ALGORITHM_FACTORY(L1CaloDecoder)
DECLARE_ALGORITHM_FACTORY(FakeRoI)
//DECLARE_ALGORITHM_FACTORY(RoIGraph)
DECLARE_ALGORITHM_FACTORY(FakeCTP)
DECLARE_ALGORITHM_FACTORY(L1Decoder)
DECLARE_NAMESPACE_ALGORITHM_FACTORY( AthViews, FakeRoIView )
DECLARE_TOOL_FACTORY(CTPUnpackingTool)
DECLARE_TOOL_FACTORY(CTPUnpackingEmulationTool)
DECLARE_TOOL_FACTORY(EMRoIsUnpackingTool)
DECLARE_TOOL_FACTORY(RoIsUnpackingEmulationTool)
DECLARE_TOOL_FACTORY(MURoIsUnpackingTool)

DECLARE_FACTORY_ENTRIES( L1Decoder )
{
    DECLARE_ALGORITHM(L1CaloDecoder)
    DECLARE_ALGORITHM(FakeRoI)
    DECLARE_ALGORITHM(FakeCTP)
    DECLARE_ALGORITHM(L1Decoder)
    DECLARE_NAMESPACE_ALGORITHM( AthViews, FakeRoIView )
    DECLARE_TOOL(CTPUnpackingTool)
    DECLARE_TOOL(CTPUnpackingEmultionTool)
    DECLARE_TOOL(EMRoIsUnpackingTool)
    DECLARE_TOOL(RoIsUnpackingEmulationTool)
    DECLARE_TOOL(MURoIsUnpackingTool)
}
