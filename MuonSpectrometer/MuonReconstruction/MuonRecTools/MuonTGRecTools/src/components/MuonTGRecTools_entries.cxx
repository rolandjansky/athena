#include "MuonTGRecTools/MuonHolesOnTrackTool.h"
#include "MuonTGRecTools/MuonTGMeasurementTool.h"
#include "MuonTGRecTools/MuonTGHitNtuple.h"
#include "../MuonSystemExtensionTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

using namespace Muon;
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, MuonHolesOnTrackTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, MuonTGMeasurementTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, MuonSystemExtensionTool )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( Muon, MuonTGHitNtuple )

DECLARE_FACTORY_ENTRIES(MuonTGRecTools) {
    DECLARE_NAMESPACE_TOOL( Muon, MuonHolesOnTrackTool)
    DECLARE_NAMESPACE_TOOL( Muon, MuonTGMeasurementTool)
    DECLARE_NAMESPACE_TOOL( Muon, MuonSystemExtensionTool)
    DECLARE_NAMESPACE_ALGORITHM( Muon, MuonTGHitNtuple )
}




