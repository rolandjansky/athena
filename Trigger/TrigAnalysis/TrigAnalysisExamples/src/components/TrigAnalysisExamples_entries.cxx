// $Id: TrigAnalysisExamples_entries.cxx 326080 2010-10-08 13:50:09Z krasznaa $

// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// derived algorithm classes
#include "../TDTExample.h"
#include "../TagAndProbeExAlg.h"
#include "../TrigMatchExAlg.h"
#include "../TriggerAnalysisTutorial.h"
#include "../TrigExpressStreamAlg.h"
#include "../BunchCrossingExample.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY(Trig, TDTExample )
DECLARE_ALGORITHM_FACTORY(TagAndProbeExAlg)
DECLARE_ALGORITHM_FACTORY(TrigMatchExAlg)
DECLARE_ALGORITHM_FACTORY(TriggerAnalysisTutorial)
DECLARE_ALGORITHM_FACTORY(TrigExpressStreamAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY( Trig, BunchCrossingExample )

DECLARE_FACTORY_ENTRIES( TrigAnalysisExamples ) {
  DECLARE_NAMESPACE_ALGORITHM(Trig, TDTExample)
  DECLARE_ALGORITHM(TagAndProbeExAlg)
  DECLARE_ALGORITHM(TrigMatchExAlg)
  DECLARE_ALGORITHM(TriggerAnalysisTutorial)
  DECLARE_NAMESPACE_ALGORITHM( Trig, BunchCrossingExample )
}

