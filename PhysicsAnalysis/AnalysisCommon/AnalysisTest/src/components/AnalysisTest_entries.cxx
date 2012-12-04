#include "GaudiKernel/DeclareFactoryEntries.h"

#include "AnalysisTest/ReadMuon.h"
#include "AnalysisTest/ReadElectron.h"
#include "AnalysisTest/ReadPhoton.h"
#include "AnalysisTest/ReadTauJet.h"
#include "AnalysisTest/ReadTruthParticle.h"
#include "AnalysisTest/ReadTrigger.h"
#include "AnalysisTest/Readegamma.h"
#include "AnalysisTest/ReadVxContainer.h"
#include "AnalysisTest/ReadTrackParticle.h"
#include "AnalysisTest/NavTest.h"

#include "AnalysisTest/WriteINav4MomAssocs.h"
#include "AnalysisTest/ReadINav4MomAssocs.h"


DECLARE_ALGORITHM_FACTORY ( ReadMuon )
DECLARE_ALGORITHM_FACTORY ( ReadElectron )
DECLARE_ALGORITHM_FACTORY ( ReadPhoton )
DECLARE_ALGORITHM_FACTORY ( ReadTauJet )
DECLARE_ALGORITHM_FACTORY ( ReadTruthParticle )
DECLARE_ALGORITHM_FACTORY ( ReadTrigger )
DECLARE_ALGORITHM_FACTORY ( Readegamma )
DECLARE_ALGORITHM_FACTORY ( ReadVxContainer )
DECLARE_ALGORITHM_FACTORY ( ReadTrackParticle )
DECLARE_ALGORITHM_FACTORY ( NavTest )

DECLARE_ALGORITHM_FACTORY ( WriteINav4MomAssocs )
DECLARE_ALGORITHM_FACTORY ( ReadINav4MomAssocs )

DECLARE_FACTORY_ENTRIES (AnalysisTest) {

  DECLARE_ALGORITHM ( ReadMuon )
  DECLARE_ALGORITHM ( ReadElectron )
  DECLARE_ALGORITHM ( ReadPhoton )
  DECLARE_ALGORITHM ( ReadTauJet )
  DECLARE_ALGORITHM ( ReadTruthParticle )
  DECLARE_ALGORITHM ( ReadTrigger )
  DECLARE_ALGORITHM ( ReadTau )
  DECLARE_ALGORITHM ( Readegamma )
  DECLARE_ALGORITHM ( ReadVxContainer )
  DECLARE_ALGORITHM ( ReadTrackParticle )
  DECLARE_ALGORITHM ( NavTest )

  DECLARE_ALGORITHM ( WriteINav4MomAssocs )
  DECLARE_ALGORITHM ( ReadINav4MomAssocs )

}

