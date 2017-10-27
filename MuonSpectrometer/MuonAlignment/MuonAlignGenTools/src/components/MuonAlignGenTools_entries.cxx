#include "MuonAlignGenTools/CommonGeometryManagerTool.h"
#include "MuonAlignGenTools/MuonGeometryManagerTool.h"
#include "MuonAlignGenTools/MdtGeometryManagerTool.h"
#include "MuonAlignGenTools/CscGeometryManagerTool.h"
#include "MuonAlignGenTools/TgcGeometryManagerTool.h"
#include "MuonAlignGenTools/MuonFillNtupleTool.h"
#include "MuonAlignGenTools/MdtAlignDBTool.h"
#include "MuonAlignGenTools/CscAlignDBTool.h"
#include "MuonAlignGenTools/MuonAlignModuleTool.h"
#include "MuonAlignGenTools/MuonTrackPreProcessor.h"
#include "MuonAlignGenTools/MuonTrackCollectionProvider.h"
#include "MuonAlignGenTools/MuonAlignHelperTool.h"
#include "MuonAlignGenTools/MuonAlignRefitTool.h"

DECLARE_COMPONENT( Muon::MuonAlignModuleTool )
DECLARE_COMPONENT( Muon::CommonGeometryManagerTool )
DECLARE_COMPONENT( Muon::MuonGeometryManagerTool )
DECLARE_COMPONENT( Muon::MdtGeometryManagerTool )
DECLARE_COMPONENT( Muon::CscGeometryManagerTool )
DECLARE_COMPONENT( Muon::TgcGeometryManagerTool )
DECLARE_COMPONENT( Muon::MuonFillNtupleTool )
DECLARE_COMPONENT( Muon::MuonAlignDBTool )
DECLARE_COMPONENT( Muon::MdtAlignDBTool )
DECLARE_COMPONENT( Muon::CscAlignDBTool )
DECLARE_COMPONENT( Muon::MuonAlignHelperTool )
DECLARE_COMPONENT( Muon::MuonAlignRefitTool )
DECLARE_COMPONENT( Muon::MuonTrackPreProcessor )
DECLARE_COMPONENT( Muon::MuonTrackCollectionProvider )
//DECLARE_COMPONENT( Muon::UnbiasedResidualTool )
//DECLARE_COMPONENT( Muon::UnbiasedMuonResiduals )

