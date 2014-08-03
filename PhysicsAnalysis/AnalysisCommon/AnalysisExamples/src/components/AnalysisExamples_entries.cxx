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

#include "GaudiKernel/DeclareFactoryEntries.h"

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

DECLARE_FACTORY_ENTRIES( AnalysisExamples ) {
  DECLARE_ALGORITHM( MiscellaneousExamples )
  DECLARE_ALGORITHM( WriteTruthParticles )
  DECLARE_ALGORITHM( ReadTruthParticles )
  DECLARE_ALGORITHM( TrackExample )
  DECLARE_ALGORITHM( ZeeOnESD )
  DECLARE_ALGORITHM( VFitZmmOnAOD )
  DECLARE_ALGORITHM( JetTagAna )
  DECLARE_ALGORITHM( PileUpTruthExample )
  DECLARE_ALGORITHM( TrigAnalysisExample )
  DECLARE_NAMESPACE_ALGORITHM( AthEx, ReadD3pdNtuple )
  DECLARE_NAMESPACE_ALGORITHM( AthEx, ReadD3pdEvtStore )

}

