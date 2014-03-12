// $Id: AnalysisTriggerAlgs_entries.cxx 587265 2014-03-12 09:45:45Z krasznaa $

// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "../RoIBResultToAOD.h"
#include "../RoIBResultToxAOD.h"
#include "../MuCTPI_RDOToRoIBResult.h"

//
// Declare the algorithm(s):
//
DECLARE_ALGORITHM_FACTORY( RoIBResultToxAOD )
DECLARE_ALGORITHM_FACTORY( RoIBResultToAOD )
DECLARE_ALGORITHM_FACTORY( MuCTPI_RDOToRoIBResult )


DECLARE_FACTORY_ENTRIES( AnalysisTriggerAlgs ) {

   DECLARE_ALGORITHM( RoIBResultToAOD )
   DECLARE_ALGORITHM( RoIBResultToxAOD )
   DECLARE_ALGORITHM( MuCTPI_RDOToRoIBResult )

}
