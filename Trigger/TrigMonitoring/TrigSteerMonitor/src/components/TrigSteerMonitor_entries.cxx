#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrigSteerMonitor/TrigSteerMoni.h"
#include "TrigSteerMonitor/TrigChainMoni.h"
#include "TrigSteerMonitor/TrigSignatureMoni.h"
#include "TrigSteerMonitor/TrigTEMoni.h"
#include "TrigSteerMonitor/TrigRoIMoni.h"
#include "TrigSteerMonitor/TrigErrorMon.h"
#include "TrigSteerMonitor/TrigRateMoni.h"
#include "TrigSteerMonitor/TrigROBMoni.h"
#include "TrigSteerMonitor/TrigMemMoni.h"
#include "TrigSteerMonitor/TrigOpMoni.h"
#include "TrigSteerMonitor/TrigCorMoni.h"
#include "TrigSteerMonitor/TrigMemAuditor.h"

DECLARE_FACTORY_ENTRIES(TrigSteerMonitor) {

  DECLARE_TOOL( TrigSteerMoni)
  DECLARE_TOOL( TrigChainMoni)
  DECLARE_TOOL( TrigSignatureMoni)
  DECLARE_TOOL( TrigTEMoni )
  DECLARE_TOOL( TrigRoIMoni )
  DECLARE_TOOL ( TrigErrorMon )
  DECLARE_TOOL ( TrigRateMoni )
  DECLARE_TOOL ( TrigROBMoni )
  DECLARE_TOOL ( TrigOpMoni )
  DECLARE_TOOL ( TrigMemMoni )
  DECLARE_TOOL ( TrigCorMoni )
  DECLARE_AUDITOR (TrigMemAuditor )
}

DECLARE_TOOL_FACTORY( TrigSteerMoni )
DECLARE_TOOL_FACTORY( TrigChainMoni )
DECLARE_TOOL_FACTORY( TrigSignatureMoni )
DECLARE_TOOL_FACTORY( TrigTEMoni )
DECLARE_TOOL_FACTORY( TrigRoIMoni )
DECLARE_TOOL_FACTORY( TrigErrorMon )
DECLARE_TOOL_FACTORY( TrigRateMoni )
DECLARE_TOOL_FACTORY( TrigROBMoni )
DECLARE_TOOL_FACTORY( TrigOpMoni )
DECLARE_TOOL_FACTORY( TrigMemMoni )
DECLARE_TOOL_FACTORY( TrigCorMoni )
DECLARE_AUDITOR_FACTORY( TrigMemAuditor )

