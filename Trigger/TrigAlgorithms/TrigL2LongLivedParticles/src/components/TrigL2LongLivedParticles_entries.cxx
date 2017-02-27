#include "TrigL2LongLivedParticles/MuonCluster.h"
#include "TrigL2LongLivedParticles/MuonClusterIsolation.h"
#include "TrigL2LongLivedParticles/TrigMuonJetFex.h"
#include "TrigL2LongLivedParticles/TrigJetSplitter.h"
#include "TrigL2LongLivedParticles/TrigBHremoval.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( MuonCluster )
DECLARE_ALGORITHM_FACTORY( MuonClusterIsolation )
DECLARE_ALGORITHM_FACTORY( TrigMuonJetFex )
DECLARE_ALGORITHM_FACTORY( TrigJetSplitter )
DECLARE_ALGORITHM_FACTORY( TrigBHremoval )

DECLARE_FACTORY_ENTRIES( TrigL2LongLivedParticles ) {
	DECLARE_ALGORITHM( MuonCluster )
	DECLARE_ALGORITHM( MuonClusterIsolation )
	DECLARE_ALGORITHM( TrigMuonJetFex )
	DECLARE_ALGORITHM( TrigJetSplitter )
	DECLARE_ALGORITHM( TrigBHremoval )
}
