#include "TrigL2LongLivedParticles/MuonCluster.h"
#include "TrigL2LongLivedParticles/TrigMuonJetFex.h"
#include "TrigL2LongLivedParticles/TrigJetSplitter.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( MuonCluster )
DECLARE_ALGORITHM_FACTORY( TrigMuonJetFex )
DECLARE_ALGORITHM_FACTORY( TrigJetSplitter )

DECLARE_FACTORY_ENTRIES( TrigL2LongLivedParticles ) {
	DECLARE_ALGORITHM( MuonCluster )
	DECLARE_ALGORITHM( TrigMuonJetFex )
	DECLARE_ALGORITHM( TrigJetSplitter )
}
