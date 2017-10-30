#include "../WriteTruthParticles.h"
#include "../ReadTruthParticles.h"
#include "../TrackExample.h"
#include "../ZeeOnESD.h"
#include "../JetTagAna.h"
#include "../MiscellaneousExamples.h"
#include "../VFitZmmOnAOD.h"
#include "../PileUpTruthExample.h"
#include "../TrigAnalysisExample.h"
#include "../AthExReadD3pdEvtStore.h"
#include "../AthExReadD3pdNtuple.h"


DECLARE_ALGORITHM_FACTORY( MiscellaneousExamples )
DECLARE_ALGORITHM_FACTORY( WriteTruthParticles )
DECLARE_ALGORITHM_FACTORY( ReadTruthParticles )
DECLARE_ALGORITHM_FACTORY( TrackExample )
DECLARE_ALGORITHM_FACTORY( ZeeOnESD )
DECLARE_ALGORITHM_FACTORY( VFitZmmOnAOD )
DECLARE_ALGORITHM_FACTORY( JetTagAna )
DECLARE_ALGORITHM_FACTORY( PileUpTruthExample )
DECLARE_ALGORITHM_FACTORY( TrigAnalysisExample )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( AthEx, ReadD3pdNtuple )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( AthEx, ReadD3pdEvtStore )

