#include "GaudiKernel/DeclareFactoryEntries.h"

#include "LongLivedParticleDPDMaker/RpvElectronD0Tool.h"
#include "LongLivedParticleDPDMaker/RpvMuonD0Tool.h"
#include "LongLivedParticleDPDMaker/RpvEgammaIDTool.h"
#include "LongLivedParticleDPDMaker/DiLepFilters.h"
#include "LongLivedParticleDPDMaker/DiLepSkimTools.h"
#include "LongLivedParticleDPDMaker/HnlSkimmingTool.h"
#include "LongLivedParticleDPDMaker/DVMissingETFilterTool.h"
#include "LongLivedParticleDPDMaker/DVTracklessJetFilterTool.h"
#include "LongLivedParticleDPDMaker/HVJetMETFilterTool.h"
#include "LongLivedParticleDPDMaker/DVMeffFilterTool.h"
#include "LongLivedParticleDPDMaker/KinkTrkSingleJetMetFilterTool.h"
#include "LongLivedParticleDPDMaker/KinkTrkZmumuTagTool.h"
#include "LongLivedParticleDPDMaker/KinkTrkZeeTagTool.h"
#include "LongLivedParticleDPDMaker/EmergingJetsFilterTool.h"
#include "LongLivedParticleDPDMaker/EmergingJetsHighPtFilterTool.h"
#include "LongLivedParticleDPDMaker/VHLowTrackJetFilterTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( DerivationFramework, RpvElectronD0Tool )
DECLARE_NAMESPACE_TOOL_FACTORY( DerivationFramework, RpvMuonD0Tool )
DECLARE_NAMESPACE_TOOL_FACTORY( DerivationFramework, RpvEgammaIDTool )
DECLARE_NAMESPACE_TOOL_FACTORY( DerivationFramework, DiLepFilters )
DECLARE_NAMESPACE_TOOL_FACTORY( DerivationFramework, DiLepSkim )
DECLARE_NAMESPACE_TOOL_FACTORY( DerivationFramework, HnlSkimmingTool )
DECLARE_NAMESPACE_TOOL_FACTORY( DerivationFramework, DVMissingETFilterTool )
DECLARE_NAMESPACE_TOOL_FACTORY( DerivationFramework, DVMeffFilterTool )
DECLARE_NAMESPACE_TOOL_FACTORY( DerivationFramework, DVTracklessJetFilterTool )
DECLARE_NAMESPACE_TOOL_FACTORY( DerivationFramework, KinkTrkSingleJetMetFilterTool )
DECLARE_NAMESPACE_TOOL_FACTORY( DerivationFramework, KinkTrkZmumuTagTool )
DECLARE_NAMESPACE_TOOL_FACTORY( DerivationFramework, KinkTrkZeeTagTool )
DECLARE_NAMESPACE_TOOL_FACTORY( DerivationFramework, HVJetMETFilterTool )
DECLARE_NAMESPACE_TOOL_FACTORY( DerivationFramework, EmergingJetsFilterTool )
DECLARE_NAMESPACE_TOOL_FACTORY( DerivationFramework, EmergingJetsHighPtFilterTool )
DECLARE_NAMESPACE_TOOL_FACTORY( DerivationFramework, VHLowTrackJetFilterTool )

DECLARE_FACTORY_ENTRIES( LongLivedParticleDPDMaker ) {

  DECLARE_NAMESPACE_TOOL( DerivationFramework, RpvElectronD0Tool  );
  DECLARE_NAMESPACE_TOOL( DerivationFramework, RpvMuonD0Tool  );
  DECLARE_NAMESPACE_TOOL( DerivationFramework, RpvEgammaIDTool  );
  DECLARE_NAMESPACE_TOOL( DerivationFramework, DiLepFilters );
  DECLARE_NAMESPACE_TOOL( DerivationFramework, DiLepSkim );
  DECLARE_NAMESPACE_TOOL( DerivationFramework, HnlSkimmingTool  );
  DECLARE_NAMESPACE_TOOL( DerivationFramework, DVMissingETFilterTool  );
  DECLARE_NAMESPACE_TOOL( DerivationFramework, DVMeffFilterTool  );
  DECLARE_NAMESPACE_TOOL( DerivationFramework, DVTracklessJetFilterTool  );
  DECLARE_NAMESPACE_TOOL( DerivationFramework, KinkTrkSingleJetMetFilterTool );
  DECLARE_NAMESPACE_TOOL( DerivationFramework, KinkTrkZmumuTagTool );
  DECLARE_NAMESPACE_TOOL( DerivationFramework, KinkTrkZmeeagTool );
  DECLARE_NAMESPACE_TOOL( DerivationFramework, HVJetMETFilterTool  );
  DECLARE_NAMESPACE_TOOL( DerivationFramework, EmergingJetsFilterTool  );
  DECLARE_NAMESPACE_TOOL( DerivationFramework, EmergingJetsHighPtFilterTool  );
  DECLARE_NAMESPACE_TOOL( DerivationFramework, VHLowTrackJetFilterTool  );

}

