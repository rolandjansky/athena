#include "TrigMuonHypo/MuonRoiFex.h"
#include "TrigMuonHypo/MufastHypo.h"
#include "TrigMuonHypo/MufastStauHypo.h"
#include "TrigMuonHypo/MufastPEBHypo.h"
#include "TrigMuonHypo/MufastOTRHypo.h"
#include "TrigMuonHypo/MufastCALHypo.h"
#include "TrigMuonHypo/MucombHypo.h"
#include "TrigMuonHypo/MucombStauHypo.h"
#include "TrigMuonHypo/MuisoHypo.h"
#include "TrigMuonHypo/TrigMooreHypo.h"
#include "TrigMuonHypo/TrigMuonEFSegmentFinderHypo.h"
#include "TrigMuonHypo/TrigMuonEFTrackBuilderHypo.h"
#include "TrigMuonHypo/TrigMuonEFTrackBuilderMultiHypo.h"
#include "TrigMuonHypo/TrigMuonEFExtrapolatorHypo.h"
#include "TrigMuonHypo/TrigMuonEFExtrapolatorMultiHypo.h"
#include "TrigMuonHypo/TrigMuonEFCombinerHypo.h"
#include "TrigMuonHypo/TrigMuonEFCombinerMultiHypo.h"
#include "TrigMuonHypo/TrigMuonEFSegmentHypo.h"
#include "TrigMuonHypo/TrigMuonEFTrackIsolationHypo.h"
#include "TrigMuonHypo/TrigMuonEFCaloIsolationHypo.h"
#include "TrigMuonHypo/TrigL2DimuonMassHypo.h"
#include "TrigMuonHypo/StauHypo.h"
#include "TrigMuonHypo/TileMuHypo.h"
#include "TrigMuonHypo/TrigMuGirlHypo.h"
#include "TrigMuonHypo/TrigMuGirlStauHypo.h"
#include "TrigMuonHypo/TrigMuTagIMOHypo.h"
#include "TrigMuonHypo/TrigL2MuonOverlapRemover.h"
#include "TrigMuonHypo/TrigEFMuonOverlapRemover.h"
#include "TrigMuonHypo/MufastNSWHypo.h"
#include "TrigMuonHypo/TrigMuonEFExtrapolatorNSWHypo.h"
#include "TrigMuonHypo/TrigMuonEFCombinerDiMuonMassHypo.h"
#include "TrigMuonHypo/TrigMuonEFCombinerDiMuonMassPtImpactsHypo.h"
#include "TrigMuonHypo/TrigMuonCaloTagHypo.h"
#include "TrigMuonHypo/TrigMuonIDTrackMultiHypo.h"

DECLARE_COMPONENT( MuonRoiFex )
DECLARE_COMPONENT( MufastHypo )
DECLARE_COMPONENT( MufastStauHypo )
DECLARE_COMPONENT( MufastPEBHypo )
DECLARE_COMPONENT( MufastOTRHypo )
DECLARE_COMPONENT( MufastCALHypo )
DECLARE_COMPONENT( MucombHypo )
DECLARE_COMPONENT( MucombStauHypo )
DECLARE_COMPONENT( MuisoHypo )
DECLARE_COMPONENT( TrigMooreHypo )
DECLARE_COMPONENT( TrigMuonEFSegmentFinderHypo )
DECLARE_COMPONENT( TrigMuonEFTrackBuilderHypo )
DECLARE_COMPONENT( TrigMuonEFTrackBuilderMultiHypo )
DECLARE_COMPONENT( TrigMuonEFExtrapolatorHypo )
DECLARE_COMPONENT( TrigMuonEFExtrapolatorMultiHypo )
DECLARE_COMPONENT( TrigMuonEFCombinerHypo )
DECLARE_COMPONENT( TrigMuonEFCombinerMultiHypo )
DECLARE_COMPONENT( TrigMuonEFSegmentHypo )
DECLARE_COMPONENT( TrigMuonEFTrackIsolationHypo )
DECLARE_COMPONENT( TrigMuonEFCaloIsolationHypo )
DECLARE_COMPONENT( TrigL2DimuonMassHypo )
DECLARE_COMPONENT( StauHypo )
DECLARE_COMPONENT( TileMuHypo )
DECLARE_COMPONENT( TrigMuGirlHypo )
DECLARE_COMPONENT( TrigMuGirlStauHypo )
DECLARE_COMPONENT( TrigMuTagIMOHypo )
DECLARE_COMPONENT( TrigL2MuonOverlapRemover )
DECLARE_COMPONENT( TrigEFMuonOverlapRemover )
DECLARE_COMPONENT( MufastNSWHypo )
DECLARE_COMPONENT( TrigMuonEFExtrapolatorNSWHypo )
DECLARE_COMPONENT( TrigMuonEFCombinerDiMuonMassHypo )
DECLARE_COMPONENT( TrigMuonEFCombinerDiMuonMassPtImpactsHypo )
DECLARE_COMPONENT( TrigMuonCaloTagHypo )
DECLARE_COMPONENT( TrigMuonIDTrackMultiHypo )

