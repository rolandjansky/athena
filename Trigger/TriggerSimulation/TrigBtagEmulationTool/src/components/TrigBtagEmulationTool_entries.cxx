/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrigBtagEmulationTool/TrigBtagEmulationTool.h"
#include "../TrigBtagEmulationToolTest.h"
#include "../TrigBtagValidationTest.h"

DECLARE_NAMESPACE_TOOL_FACTORY(Trig,TrigBtagEmulationTool)   
DECLARE_NAMESPACE_ALGORITHM_FACTORY(Trig,TrigBtagEmulationToolTest) 
DECLARE_NAMESPACE_ALGORITHM_FACTORY(Trig,TrigBtagValidationTest)

DECLARE_FACTORY_ENTRIES(TrigBtagEmulationTool) {
  DECLARE_NAMESPACE_TOOL(Trig,TrigBtagEmulationTool)
  DECLARE_NAMESPACE_ALGORITHM(Trig,TrigBtagMatchingToolTest)
  DECLARE_NAMESPACE_ALGORITHM(Trig,TrigBtagValidationTest)
}

