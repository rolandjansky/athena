#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../TrigMuonEFTrackIsolationTool.h"
#include "../TrigMuonEFTrackIsolation.h"
#include "../TrigMuonEFStandaloneTrackTool.h"
#include "../TrigMuonEFRoiAggregator.h"
#include "../TrigMuonEFFSRoiMaker.h"
#include "../InDetTrkRoiMaker.h"
#include "../TrigMuonEFCaloIsolation.h"
#include "../TrigMuonEFIDTrackRoiMaker.h"

DECLARE_TOOL_FACTORY(TrigMuonEFTrackIsolationTool)
DECLARE_TOOL_FACTORY(TrigMuonEFStandaloneTrackTool)

DECLARE_ALGORITHM_FACTORY(TrigMuonEFTrackIsolation)
DECLARE_ALGORITHM_FACTORY(TrigMuonEFRoiAggregator)
DECLARE_ALGORITHM_FACTORY(InDetTrkRoiMaker)
DECLARE_ALGORITHM_FACTORY(TrigMuonEFCaloIsolation)

DECLARE_ALGORITHM_FACTORY(TrigMuonEFFSRoiMaker)

DECLARE_ALGORITHM_FACTORY(TrigMuonEFIDTrackRoiMaker)

DECLARE_FACTORY_ENTRIES(TrigMuonEF) {
  DECLARE_TOOL( TrigMuonEFTrackIsolationTool )
  DECLARE_TOOL( TrigMuonEFStandaloneTrackTool )

  DECLARE_ALGORITHM( TrigMuonEFTrackIsolation )
  DECLARE_ALGORITHM( TrigMuonEFRoiAggregator )
  DECLARE_ALGORITHM( InDetTrkRoiMaker )
  DECLARE_ALGORITHM( TrigMuonEFFSRoiMaker )
  DECLARE_ALGORITHM( TrigMuonEFCaloIsolation )
  DECLARE_ALGORITHM( TrigMuonEFIDTrackRoiMaker )
}

