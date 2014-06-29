#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../MuonCombinedInDetCandidateAlg.h"
#include "../MuonCombinedMuonCandidateAlg.h"
#include "../MuonCombinedAlg.h"
#include "../MuonCreatorAlg.h"
#include "../MuonCombinedInDetExtensionAlg.h"
#include "../MuonSegmentTagAlg.h"

DECLARE_ALGORITHM_FACTORY( MuonCombinedInDetCandidateAlg )
DECLARE_ALGORITHM_FACTORY( MuonCombinedMuonCandidateAlg )
DECLARE_ALGORITHM_FACTORY( MuonCombinedAlg )
DECLARE_ALGORITHM_FACTORY( MuonCreatorAlg )
DECLARE_ALGORITHM_FACTORY( MuonCombinedInDetExtensionAlg )
DECLARE_ALGORITHM_FACTORY( MuonSegmentTagAlg )

DECLARE_FACTORY_ENTRIES( MuonCombinedAlgs )
{
  DECLARE_ALGORITHM( MuonCombinedInDetCandidateAlg )
  DECLARE_ALGORITHM( MuonCombinedMuonCandidateAlg )
  DECLARE_ALGORITHM( MuonCombinedAlg )
  DECLARE_ALGORITHM( MuonCreatorAlg )
  DECLARE_ALGORITHM( MuonCombinedInDetExtensionAlg )
  DECLARE_ALGORITHM( MuonSegmentTagAlg )
}

