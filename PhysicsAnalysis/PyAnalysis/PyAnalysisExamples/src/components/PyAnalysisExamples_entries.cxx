#include "GaudiKernel/DeclareFactoryEntries.h"

#include "PyAnalysisExamples/MyTool.h"
#include "PyAnalysisExamples/MySelectionAlg.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY(AthPyEx, MySelectionAlg)
DECLARE_NAMESPACE_TOOL_FACTORY(AthPyEx, MyTool )

DECLARE_FACTORY_ENTRIES(PyAnalysisExamples)
{
  DECLARE_NAMESPACE_ALGORITHM(AthPyEx, MySelectionAlg)
  DECLARE_NAMESPACE_TOOL(AthPyEx, MyTool)
}
