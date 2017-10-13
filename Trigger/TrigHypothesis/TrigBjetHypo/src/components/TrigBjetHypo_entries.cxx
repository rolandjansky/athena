#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrigBjetHypo/TrigBjetHypo.h"
#include "TrigBjetHypo/TrigBjetFex.h"
#include "TrigBjetHypo/TrigBtagFex.h"
#include "TrigBjetHypo/TrigGSCFex.h"
#include "TrigBjetHypo/TrigLeptonJetFexAllTE.h"
#include "TrigBjetHypo/TrigLeptonJetMatchAllTE.h"
#include "TrigBjetHypo/TrigEFBjetSequenceAllTE.h"
#include "TrigBjetHypo/TrigJetSplitterAllTE.h"
#include "TrigBjetHypo/TrigSuperRoiBuilderAllTE.h"
#include "TrigBjetHypo/TrigBjetEtHypo.h"
#include "TrigBjetHypo/TrigFarawayJetFinderAllTE.h"

DECLARE_ALGORITHM_FACTORY( TrigBjetHypo )
DECLARE_ALGORITHM_FACTORY( TrigBjetFex )
DECLARE_ALGORITHM_FACTORY( TrigBtagFex )
DECLARE_ALGORITHM_FACTORY( TrigGSCFex )
DECLARE_ALGORITHM_FACTORY( TrigLeptonJetFexAllTE )
DECLARE_ALGORITHM_FACTORY( TrigLeptonJetMatchAllTE )
DECLARE_ALGORITHM_FACTORY( TrigEFBjetSequenceAllTE )
DECLARE_ALGORITHM_FACTORY( TrigJetSplitterAllTE )
DECLARE_ALGORITHM_FACTORY( TrigSuperRoiBuilderAllTE )
DECLARE_ALGORITHM_FACTORY( TrigBjetEtHypo )
DECLARE_ALGORITHM_FACTORY( TrigFarawayJetFinderAllTE )

DECLARE_FACTORY_ENTRIES( TrigBjetHypo ) { 

    DECLARE_ALGORITHM( TrigBjetHypo )
    DECLARE_ALGORITHM( TrigBjetFex )
    DECLARE_ALGORITHM( TrigBtagFex )
    DECLARE_ALGORITHM( TrigGSCFex )
    DECLARE_ALGORITHM( TrigLeptonJetFexAllTE )
    DECLARE_ALGORITHM( TrigLeptonJetMatchAllTE )
    DECLARE_ALGORITHM( TrigEFBjetSequenceAllTE )
    DECLARE_ALGORITHM( TrigJetSplitterAllTE )
    DECLARE_ALGORITHM( TrigSuperRoiBuilderAllTE )
    DECLARE_ALGORITHM( TrigBjetEtHypo )
    DECLARE_ALGORITHM( TrigFarawayJetFinderAllTE )

}
