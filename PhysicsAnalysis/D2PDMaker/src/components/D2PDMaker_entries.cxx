#include "GaudiKernel/DeclareFactoryEntries.h"

#include "D2PDMaker/D2PDINavigable4MomentumSelector.h"
#include "D2PDMaker/D2PDParticleCombiner.h"
#include "D2PDMaker/D2PDElectronSelector.h"
#include "D2PDMaker/D2PDPhotonSelector.h"
#include "D2PDMaker/D2PDMuonSelector.h"
#include "D2PDMaker/D2PDTauSelector.h"
#include "D2PDMaker/D2PDJetSelector.h"
#include "D2PDMaker/D2PDTrackParticleSelector.h"
#include "D2PDMaker/D2PDVertexSelector.h"
#include "D2PDMaker/D2PDTruthParticleSelector.h"
#include "D2PDMaker/CheckD2PD.h"
#include "D2PDMaker/CreateControlPlots.h"
#include "D2PDMaker/UserDataHelperTool.h"
#include "D2PDMaker/FilterTool.h"

DECLARE_ALGORITHM_FACTORY( D2PDINavigable4MomentumSelector )
DECLARE_ALGORITHM_FACTORY( D2PDParticleCombiner )
DECLARE_ALGORITHM_FACTORY( D2PDElectronSelector )
DECLARE_ALGORITHM_FACTORY( D2PDPhotonSelector )
DECLARE_ALGORITHM_FACTORY( D2PDMuonSelector )
DECLARE_ALGORITHM_FACTORY( D2PDTauSelector )
DECLARE_ALGORITHM_FACTORY( D2PDJetSelector )
DECLARE_ALGORITHM_FACTORY( D2PDTrackParticleSelector )
DECLARE_ALGORITHM_FACTORY( D2PDVertexSelector )
DECLARE_ALGORITHM_FACTORY( D2PDTruthParticleSelector )
DECLARE_ALGORITHM_FACTORY( CheckD2PD )
DECLARE_ALGORITHM_FACTORY( CreateControlPlots )
DECLARE_TOOL_FACTORY( UserDataHelperTool )
DECLARE_TOOL_FACTORY( FilterTool )

DECLARE_FACTORY_ENTRIES( D2PDMaker ) 
{
  DECLARE_ALGORITHM( D2PDINavigable4MomentumSelector );
  DECLARE_ALGORITHM( D2PDParticleCombiner );
  DECLARE_ALGORITHM( D2PDElectronSelector );
  DECLARE_ALGORITHM( D2PDPhotonSelector );
  DECLARE_ALGORITHM( D2PDMuonSelector );
  DECLARE_ALGORITHM( D2PDTauSelector );
  DECLARE_ALGORITHM( D2PDJetSelector );
  DECLARE_ALGORITHM( D2PDTrackParticleSelector );
  DECLARE_ALGORITHM( D2PDVertexSelector );
  DECLARE_ALGORITHM( D2PDTruthParticleSelector );
  DECLARE_ALGORITHM( CheckD2PD );
  DECLARE_ALGORITHM( CreateControlPlots );
  DECLARE_TOOL( UserDataHelperTool );
  DECLARE_TOOL( FilterTool );
}


