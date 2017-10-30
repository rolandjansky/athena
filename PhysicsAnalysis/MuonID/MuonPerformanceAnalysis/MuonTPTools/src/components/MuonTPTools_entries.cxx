// MuonTPTools_entries.cxx

#include "MuonTPTools/BadMuonVetoHelperTool.h"
#include "MuonTPTools/MuonTPTool.h"
#include "MuonTPTools/MuonTPExtrapolationTool.h"
#include "MuonTPTools/IDTrackIsolationDecoratorTool.h"
#include "MuonTPTools/IDTrackCaloDepositsDecoratorTool.h"
#include "MuonTPTools/MuonTPTrigUtils.h"

#include "MuonTPTools/MuonTPSelectionTool.h"
#include "MuonTPTools/DiMuonTPSelectionTool.h"
#include "MuonTPTools/HighPtSingleMuonSelectionTool.h"

#include "MuonTPTools/MuonTPEfficiencyTool.h"
#include "MuonTPTools/MuonRecoTPEfficiencyTool.h"
#include "MuonTPTools/MuonTrigTPEfficiencyTool.h"
#include "MuonTPTools/MuonIsolTPEfficiencyTool.h"
#include "MuonTPTools/MuonTPPlotTool.h"
#include "MuonTPTools/DiMuonTPPlotTool.h"

#include "MuonTPTools/MuonTPTreeTool.h"
#include "MuonTPTools/DiMuonTPTreeTool.h"
#include "MuonTPTools/MuonTPVertexHelper.h"


DECLARE_COMPONENT( MuonTPTool )
DECLARE_COMPONENT( MuonTPExtrapolationTool )
DECLARE_COMPONENT( IDTrackIsolationDecoratorTool )
DECLARE_COMPONENT( IDTrackCaloDepositsDecoratorTool )
DECLARE_COMPONENT( BadMuonVetoHelperTool )
DECLARE_COMPONENT( MuonTPTrigUtils )
DECLARE_COMPONENT( MuonTPVertexHelper )

// DECLARE_COMPONENT( MuonTPSelectionTool )
DECLARE_COMPONENT( DiMuonTPSelectionTool )
DECLARE_COMPONENT( HighPtSingleMuonSelectionTool )

// DECLARE_COMPONENT( MuonTPEfficiencyTool )
DECLARE_COMPONENT( MuonRecoTPEfficiencyTool )
DECLARE_COMPONENT( MuonTrigTPEfficiencyTool )
DECLARE_COMPONENT( MuonIsolTPEfficiencyTool )

// DECLARE_COMPONENT( MuonTPPlotTool )
DECLARE_COMPONENT( DiMuonTPPlotTool )
// DECLARE_COMPONENT( JPsiMuonTPPlotTool )

// DECLARE_COMPONENT( MuonTPTreeTool )
DECLARE_COMPONENT( DiMuonTPTreeTool )

  
  
  
  
  

