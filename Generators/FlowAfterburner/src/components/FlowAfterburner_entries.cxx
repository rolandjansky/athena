//====================================================================
//  FlowAfterburner_entries.cxx
//--------------------------------------------------------------------
//
//  Package    : Generators/FlowAfterburner
//
//  Description: Declaration of the components (factory entries)
//               specified by this component library.
//
//====================================================================

#include "FlowAfterburner/AddFlowByShifting.h"
#include "FlowAfterburner/CheckFlow.h"
#include "FlowAfterburner/CheckFlow_New.h"
#include "FlowAfterburner/CheckFlow_New_Minbias.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( AddFlowByShifting )
DECLARE_ALGORITHM_FACTORY( CheckFlow )
DECLARE_ALGORITHM_FACTORY( CheckFlow_New )
DECLARE_ALGORITHM_FACTORY( CheckFlow_New_Minbias)

DECLARE_FACTORY_ENTRIES(FlowAfterburner) {
  DECLARE_ALGORITHM( AddFlowByShifting )
  DECLARE_ALGORITHM( CheckFlow )
  DECLARE_ALGORITHM( CheckFlow_New )
  DECLARE_ALGORITHM( CheckFlow_New_Minbias )
}
