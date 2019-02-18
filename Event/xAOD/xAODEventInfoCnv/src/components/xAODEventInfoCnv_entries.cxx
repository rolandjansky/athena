//
// Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
//

// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "../EventInfoCnvTool.h"
#include "../EventInfoSelectorTool.h"
#include "../EventInfoCnvAlg.h"
#include "../EventInfoReaderAlg.h"
#include "../EventDuplicateFinderAlg.h"
#include "../EventInfoNonConstCnvAlg.h"
#include "../EventInfoModifierAlg.h"

#include "../EventInfoMCWeightFixAlg.h"
#include "../EventInfoRunNumberFixAlg.h"

using namespace xAOD;

DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker, EventInfoCnvTool )
DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker, EventInfoSelectorTool )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, EventInfoCnvAlg )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODReader, EventInfoReaderAlg )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODReader, EventDuplicateFinderAlg )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, EventInfoNonConstCnvAlg )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODReader, EventInfoModifierAlg )

DECLARE_ALGORITHM_FACTORY( EventInfoMCWeightFixAlg )
DECLARE_ALGORITHM_FACTORY( EventInfoRunNumberFixAlg )

DECLARE_FACTORY_ENTRIES( xAODEventInfoCnv ) {

   DECLARE_NAMESPACE_TOOL( xAODMaker, EventInfoCnvTool )
   DECLARE_NAMESPACE_TOOL( xAODMaker, EventInfoSelectorTool )
   DECLARE_NAMESPACE_ALGORITHM( xAODMaker, EventInfoCnvAlg )
   DECLARE_NAMESPACE_ALGORITHM( xAODReader, EventInfoReaderAlg )
   DECLARE_NAMESPACE_ALGORITHM( xAODReader, EventDuplicateFinderAlg )
   DECLARE_NAMESPACE_ALGORITHM( xAODMaker, EventInfoNonConstCnvAlg )
   DECLARE_NAMESPACE_ALGORITHM( xAODReader, EventInfoModifierAlg )

   DECLARE_ALGORITHM( EventInfoMCWeightFixAlg )
   DECLARE_ALGORITHM( EventInfoRunNumberFixAlg )

}
