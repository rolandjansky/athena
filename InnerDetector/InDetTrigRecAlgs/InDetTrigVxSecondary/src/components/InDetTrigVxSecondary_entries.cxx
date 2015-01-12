#include "GaudiKernel/DeclareFactoryEntries.h"
#include "InDetTrigVxSecondary/TrigVxSecondary.h"
#include "InDetTrigVxSecondary/TrigVxSecondaryCombo.h"

using namespace InDet;

DECLARE_ALGORITHM_FACTORY( TrigVxSecondary      )
DECLARE_ALGORITHM_FACTORY( TrigVxSecondaryCombo )

  DECLARE_FACTORY_ENTRIES( InDetTrigVxSecondary ) {
  DECLARE_ALGORITHM( TrigVxSecondary      )
  DECLARE_ALGORITHM( TrigVxSecondaryCombo )
    }

