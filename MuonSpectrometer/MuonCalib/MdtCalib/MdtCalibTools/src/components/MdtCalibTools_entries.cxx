#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MdtCalibTools/DCSLFitterTool.h"
#include "MdtCalibTools/QuasianalyticLineReconstructionTool.h"
#include "MdtCalibTools/MdtCalibTool.h"
#include "MdtCalibTools/SimpleMdtSegmentSelectorTool.h"
#include "MdtCalibTools/SimplePatternSelectorTool.h"
#include "MdtCalibTools/MdtCalibNtupleMakerTool.h"


using namespace MuonCalib;

DECLARE_TOOL_FACTORY( DCSLFitterTool )
DECLARE_TOOL_FACTORY( QuasianalyticLineReconstructionTool )
DECLARE_TOOL_FACTORY( MdtCalibTool )
DECLARE_TOOL_FACTORY( SimpleMdtSegmentSelectorTool )
DECLARE_TOOL_FACTORY( SimplePatternSelectorTool )
DECLARE_TOOL_FACTORY( MdtCalibNtupleMakerTool )

/** factory entries need to have the name of the package */
DECLARE_FACTORY_ENTRIES( MdtCalibTools )
{
  DECLARE_TOOL( DCSLFitterTool )
  DECLARE_TOOL( QuasianalyticLineReconstructionTool )
  DECLARE_TOOL( MdtCalibTool )
  DECLARE_TOOL( SimpleMdtSegmentSelectorTool )
  DECLARE_TOOL( SimplePatternSelectorTool )
  DECLARE_TOOL( MdtCalibNtupleMakerTool )
}
