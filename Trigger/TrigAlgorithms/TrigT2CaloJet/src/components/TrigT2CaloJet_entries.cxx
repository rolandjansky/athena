#include "TrigT2CaloJet/T2CaloJet.h"
#include "TrigT2CaloJet/T2L1CaloJet.h"
#include "TrigT2CaloJet/T2L1CaloFullScanJet.h"
#include "TrigT2CaloJet/T2L1Unpacking.h"
#include "TrigT2CaloJet/Trig3MomentumMerger.h"
#include "TrigT2CaloJet/T2AllRoiUnpacking.h"
#include "TrigT2CaloJet/T2CaloFastJet.h"
#include "TrigT2CaloJet/T2CaloJetBaseTool.h"
#include "TrigT2CaloJet/T2CaloJetConeTool.h"
#include "TrigT2CaloJet/T2CaloJetCalibTool.h"
#include "TrigT2CaloJet/T2CaloJetGridFromCells.h"
#include "TrigT2CaloJet/T2CaloJetGridFromFEBHeader.h"
#include "TrigT2CaloJet/T2CaloJetGridFromLvl1Ppr.h"
#include "TrigT2CaloJet/T2L1CaloJetFullScanBaseTool.h"
#include "TrigT2CaloJet/T2L1CaloJetFullScanFastJetTool.h"
#include "TrigT2CaloJet/T2L1TowerCalibTool.h"
#include "TrigT2CaloJet/T2L1CaloJetCalibTool.h"

DECLARE_ALGORITHM_FACTORY( T2CaloJet )
DECLARE_ALGORITHM_FACTORY( T2L1CaloJet )
DECLARE_ALGORITHM_FACTORY( T2L1CaloFullScanJet )
DECLARE_ALGORITHM_FACTORY( T2CaloFastJet )
DECLARE_ALGORITHM_FACTORY( T2L1Unpacking )
DECLARE_ALGORITHM_FACTORY( T2AllRoiUnpacking )
DECLARE_TOOL_FACTORY( T2CaloJetBaseTool )
DECLARE_TOOL_FACTORY( T2CaloJetConeTool )
DECLARE_TOOL_FACTORY( T2CaloJetCalibTool )
DECLARE_TOOL_FACTORY( T2CaloJetGridFromCells )
DECLARE_TOOL_FACTORY( T2CaloJetGridFromFEBHeader )
DECLARE_TOOL_FACTORY( T2CaloJetGridFromLvl1Ppr )
DECLARE_TOOL_FACTORY( T2L1CaloJetFullScanBaseTool )
DECLARE_TOOL_FACTORY( T2L1CaloJetFullScanFastJetTool )
DECLARE_TOOL_FACTORY( T2L1TowerCalibTool )
DECLARE_TOOL_FACTORY( T2L1CaloJetCalibTool )

