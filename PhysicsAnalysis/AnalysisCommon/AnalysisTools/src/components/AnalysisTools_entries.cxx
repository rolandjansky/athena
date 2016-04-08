#include "Python.h"
#include "AnalysisTools/AANTupleStream.h"
#include "AnalysisTools/AANTEventSelector.h"
#include "AnalysisTools/AnalysisTools.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
 
DECLARE_TOOL_FACTORY( AnalysisTools )
DECLARE_SERVICE_FACTORY( AANTEventSelector )
DECLARE_ALGORITHM_FACTORY( AANTupleStream )

 
DECLARE_FACTORY_ENTRIES( AnalysisTools )
{
  DECLARE_TOOL( AnalysisTools )
  DECLARE_ALGORITHM( AANTupleStream )
  DECLARE_SERVICE( AANTEventSelector )   
}
