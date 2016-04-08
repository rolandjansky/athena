#include "InDetSurveyConstraintTool/SurveyConstraint.h"
#include "InDetSurveyConstraintTool/SurveyConstraintTestAlg.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( SurveyConstraintTestAlg )
DECLARE_TOOL_FACTORY( SurveyConstraint )

DECLARE_FACTORY_ENTRIES(InDetSurveyConstraintTool) { 
  DECLARE_TOOL( SurveyConstraint );
  DECLARE_ALGORITHM( SurveyConstraintTestAlg );
}
