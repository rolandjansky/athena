// $Id: xAODCreatorAlgs_entries.cxx 298282 2013-12-03 13:05:16Z emoyse $

// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "../MuonCnvAlg.h"
#include "../MuonSegmentCnvAlg.h"
#include "../MuonAODFixAlg.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, MuonCnvAlg )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, MuonSegmentCnvAlg )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAOD, MuonAODFixAlg )

DECLARE_FACTORY_ENTRIES( xAODCreatorAlgs ) {
   DECLARE_NAMESPACE_ALGORITHM( xAODMaker, MuonCnvAlg )
   DECLARE_NAMESPACE_ALGORITHM( xAODMaker, MuonSegmentCnvAlg )
   DECLARE_NAMESPACE_ALGORITHM( xAOD, MuonAODFixAlg )
}
