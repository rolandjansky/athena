#define TRIGGERD3PDMAKER_ENTRIES_FILE

// Local include(s):
#include "../TriggerBitFillerTool.h"

// LVL1 getter tools:
#include "../Muon_ROIGetterTool.h"
#include "../Jet_ROIGetterTool.h"
#include "../EmTau_ROIGetterTool.h"
#include "../JetET_ROIGetterTool.h"
#include "../EnergySum_ROIGetterTool.h"

// LVL1 filler tools:
#include "../Muon_ROIFillerTool.h"
#include "../Jet_ROIFillerTool.h"
#include "../EmTau_ROIthrPatternFillerTool.h"
#include "../EmTau_ROIObjectDecisionFillerTool.h"
#include "../JetET_ROIFillerTool.h"
#include "../EnergySum_ROIFillerTool.h"
#include "../MuCTPI_RDOFillerTool.h"
#include "../CTP_RDOFillerTool.h"

// Navigation filler tools:
#include "../ChainEntryFillerTool.h"
#include "../CombLinksFillerTool.h"

// Additional tools:
#include "../TrigConfMetadataTool.h"
#include "../TrigDecisionFillerTool.h"
#include "../TrigRoiDescriptorFillerTool.h"
#include "../TrigDBKeysFillerTool.h"
#include "../TrigOperationalInfoFillerTool.h"
#include "../BunchStructureMetadataTool.h"
#include "../BunchConfigIDFillerTool.h"
#include "TriggerD3PDMaker/TriggerKeyTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TriggerBitFillerTool )

DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, Muon_ROIGetterTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, Jet_ROIGetterTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, EmTau_ROIGetterTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, JetET_ROIGetterTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, EnergySum_ROIGetterTool )

DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, Muon_ROIFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, Jet_ROIFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, EmTau_ROIthrPatternFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, EmTau_ROIObjectDecisionFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, JetET_ROIFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, EnergySum_ROIFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, MuCTPI_RDOFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, CTP_RDOFillerTool )

DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, ChainEntryFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, CombLinksFillerTool)

DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TrigConfMetadataTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TrigDecisionFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TrigRoiDescriptorFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TrigDBKeysFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TrigOperationalInfoFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, BunchStructureMetadataTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, BunchConfigIDFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TriggerKeyTool )

