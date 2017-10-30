#include "../AnalysisTagBuilder.h" 
#include "../RawInfoSummaryTagBuilder.h"
#include "../ElectronTagBuilder.h"
#include "../GlobalEventTagBuilder.h"
#include "../GlobalTriggerTagBuilder.h"
#include "../MuonTagBuilder.h"
#include "../PhotonTagBuilder.h"
#include "../TauJetTagBuilder.h"
#include "../MissingETTagBuilder.h"
#include "../ParticleJetTagBuilder.h"

#include "EventTagAlgs/EventSplitter.h"


DECLARE_ALGORITHM_FACTORY( AnalysisTagBuilder )
DECLARE_ALGORITHM_FACTORY( RawInfoSummaryTagBuilder )
DECLARE_ALGORITHM_FACTORY( ElectronTagBuilder )
DECLARE_ALGORITHM_FACTORY( GlobalEventTagBuilder )
DECLARE_ALGORITHM_FACTORY( GlobalTriggerTagBuilder )
DECLARE_ALGORITHM_FACTORY( MuonTagBuilder )
DECLARE_ALGORITHM_FACTORY( PhotonTagBuilder )
DECLARE_ALGORITHM_FACTORY( TauJetTagBuilder )
DECLARE_ALGORITHM_FACTORY( ParticleJetTagBuilder )
DECLARE_ALGORITHM_FACTORY( MissingETTagBuilder )


DECLARE_ALGORITHM_FACTORY( EventSplitter )

