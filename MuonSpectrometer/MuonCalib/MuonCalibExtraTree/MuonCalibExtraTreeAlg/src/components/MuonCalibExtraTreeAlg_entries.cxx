#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MuonCalibExtraTreeAlg/MuonCalibExtraTreeAlg.h"
#include "MuonCalibExtraTreeAlg/MuonCalibExtraTreeTriggerAlg.h"
#include "MuonCalibExtraTreeAlg/SegmentOnTrackSelector.h"
#include "MuonCalibExtraTreeAlg/ExtraTreeTrackFillerTool.h"
#include "../ExtraTreeMuonFillerTool.h"

using namespace MuonCalib;

DECLARE_ALGORITHM_FACTORY ( MuonCalibExtraTreeAlg )
DECLARE_ALGORITHM_FACTORY ( MuonCalibExtraTreeTriggerAlg )
DECLARE_TOOL_FACTORY ( SegmentOnTrackSelector )
DECLARE_TOOL_FACTORY ( ExtraTreeTrackFillerTool )
DECLARE_TOOL_FACTORY (  ExtraTreeMuonFillerTool )

DECLARE_FACTORY_ENTRIES(MuonCalibExtraTreeAlg) {
  DECLARE_ALGORITHM(MuonCalibExtraTreeAlg)
  DECLARE_ALGORITHM(MuonCalibExtraTreeTriggerAlg)
  DECLARE_TOOL ( ExtraTreeTrackFillerTool )
  DECLARE_TOOL ( ExtraTreeMuonFillerTool )
  DECLARE_TOOL ( SegmentOnTrackSelector )
}
