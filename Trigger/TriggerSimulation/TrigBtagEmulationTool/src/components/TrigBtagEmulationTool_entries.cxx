/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrigBtagEmulationTool/TrigBtagEmulationTool.h"
#include "src/TrigBtagEmulationToolTest.h"
#include "src/TrigBtagValidationTest.h"

DECLARE_NAMESPACE_TOOL_FACTORY(Trig,TrigBtagEmulationTool)   
DECLARE_NAMESPACE_ALGORITHM_FACTORY(Trig,TrigBtagEmulationToolTest) 
DECLARE_NAMESPACE_ALGORITHM_FACTORY(Trig,TrigBtagValidationTest)

DECLARE_FACTORY_ENTRIES(TrigBtagEmulationTool) {
  DECLARE_NAMESPACE_TOOL(Trig,TrigBtagEmulationTool)
  DECLARE_NAMESPACE_ALGORITHM(Trig,TrigBtagEmulationToolTest)
  DECLARE_NAMESPACE_ALGORITHM(Trig,TrigBtagValidationTest)
}

