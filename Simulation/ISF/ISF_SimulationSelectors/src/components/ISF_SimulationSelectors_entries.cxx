#include "GaudiKernel/DeclareFactoryEntries.h"
#include "ISF_SimulationSelectors/DefaultSimSelector.h"
#include "ISF_SimulationSelectors/KinematicSimSelector.h"
#include "ISF_SimulationSelectors/TruthAssocSimSelector.h"
#include "ISF_SimulationSelectors/HistorySimSelector.h"
#include "ISF_SimulationSelectors/ConeSimSelector.h"
#include "ISF_SimulationSelectors/PileupSimSelector.h"
#include "ISF_SimulationSelectors/RandomSimSelector.h"

DECLARE_NAMESPACE_TOOL_FACTORY( ISF , DefaultSimSelector         )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , KinematicSimSelector       )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , TruthAssocSimSelector      )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , HistorySimSelector         )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , ConeSimSelector            )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , PileupSimSelector        )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , RandomSimSelector        )

DECLARE_FACTORY_ENTRIES( ISF_SimulationSelectors ) {
  DECLARE_NAMESPACE_TOOL( ISF ,  DefaultSimSelector         )
  DECLARE_NAMESPACE_TOOL( ISF ,  KinematicSimSelector       )
  DECLARE_NAMESPACE_TOOL( ISF ,  TruthAssocSimSelector      )
  DECLARE_NAMESPACE_TOOL( ISF ,  HistorySimSelector         )
  DECLARE_NAMESPACE_TOOL( ISF ,  ConeSimSelector            )
  DECLARE_NAMESPACE_TOOL( ISF ,  PileupSimSelector        )
  DECLARE_NAMESPACE_TOOL( ISF ,  RandomSimSelector        )
}

