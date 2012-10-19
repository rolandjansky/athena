#include "TauTrackTools/TruthTauCreatorAlg.h"
#include "TauTrackTools/TauTrackEfficiencyAlg.h"
#include "TauTrackTools/TauJetTrackBasedTruthMatchTool.h"
#include "TauTrackTools/TauJetTruthMatchTool.h"
#include "TauTrackTools/TauSeedTruthMatchTool.h"
#include "TauTrackTools/TauSeedTrackBasedTruthMatchTool.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( TauID , TruthTauCreatorAlg )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( TauID , TauTrackEfficiencyAlg )
DECLARE_NAMESPACE_TOOL_FACTORY( TauID, TauJetTruthMatchTool )
DECLARE_NAMESPACE_TOOL_FACTORY( TauID, TauJetTrackBasedTruthMatchTool )
DECLARE_NAMESPACE_TOOL_FACTORY( TauID, TauSeedTruthMatchTool )
DECLARE_NAMESPACE_TOOL_FACTORY( TauID, TauSeedTrackBasedTruthMatchTool )

DECLARE_FACTORY_ENTRIES( TauTrackTools ) {

  DECLARE_NAMESPACE_ALGORITHM( TauID ,  TruthTauCreatorAlg )
  DECLARE_NAMESPACE_ALGORITHM( TauID ,  TauTrackEfficiencyAlg )
  DECLARE_NAMESPACE_TOOL( TauID, TauJetTruthMatchTool )
  DECLARE_NAMESPACE_TOOL( TauID, TauJetTrackBasedTruthMatchTool )
  DECLARE_NAMESPACE_TOOL( TauID, TauSeedTruthMatchTool )
  DECLARE_NAMESPACE_TOOL( TauID, TauSeedTrackBasedTruthMatchTool )
}

