#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../DefaultSimSelector.h"
#include "../KinematicSimSelector.h"
#include "../TruthAssocSimSelector.h"
#include "../HistorySimSelector.h"
#include "../ConeSimSelector.h"
#include "../KinematicPileupSimSelector.h"

DECLARE_NAMESPACE_TOOL_FACTORY( ISF , DefaultSimSelector         )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , KinematicSimSelector       )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , TruthAssocSimSelector      )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , HistorySimSelector         )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , ConeSimSelector            )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , KinematicPileupSimSelector        )

DECLARE_FACTORY_ENTRIES( ISF_SimulationSelectors ) {
  DECLARE_NAMESPACE_TOOL( ISF ,  DefaultSimSelector         )
  DECLARE_NAMESPACE_TOOL( ISF ,  KinematicSimSelector       )
  DECLARE_NAMESPACE_TOOL( ISF ,  TruthAssocSimSelector      )
  DECLARE_NAMESPACE_TOOL( ISF ,  HistorySimSelector         )
  DECLARE_NAMESPACE_TOOL( ISF ,  ConeSimSelector            )
  DECLARE_NAMESPACE_TOOL( ISF ,  KinematicPileupSimSelector        )
}

