// JetRec_entries.cxx
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "JetMomentTools/JetCaloEnergies.h"
#include "JetMomentTools/JetCaloQualityTool.h"
#include "../JetCaloCellQualityTool.h"
#include "JetMomentTools/JetWidthTool.h"
#include "JetMomentTools/JetBadChanCorrTool.h"
#include "JetMomentTools/JetVertexFractionTool.h"
#include "JetMomentTools/JetVertexTaggerTool.h"
#include "JetMomentTools/JetTrackMomentsTool.h"
#include "JetMomentTools/JetMuonSegmentMomentsTool.h"
#include "JetMomentTools/JetPtAssociationTool.h"
#include "JetMomentTools/JetIsolationTool.h"
#include "JetMomentTools/JetLArHVTool.h"
#include "JetMomentTools/JetOriginCorrectionTool.h"

DECLARE_TOOL_FACTORY(JetCaloEnergies)
DECLARE_TOOL_FACTORY(JetCaloQualityTool)
DECLARE_TOOL_FACTORY(JetCaloCellQualityTool)
DECLARE_TOOL_FACTORY(JetWidthTool)
DECLARE_TOOL_FACTORY(JetBadChanCorrTool)
DECLARE_TOOL_FACTORY(JetVertexFractionTool)
DECLARE_TOOL_FACTORY(JetVertexTaggerTool)
DECLARE_TOOL_FACTORY(JetTrackMomentsTool)
DECLARE_TOOL_FACTORY(JetMuonSegmentMomentsTool)
DECLARE_TOOL_FACTORY(JetPtAssociationTool)
DECLARE_TOOL_FACTORY(JetIsolationTool)
DECLARE_TOOL_FACTORY(JetLArHVTool)
DECLARE_TOOL_FACTORY(JetOriginCorrectionTool)

DECLARE_FACTORY_ENTRIES(JetRec) {
  DECLARE_TOOL(JetCaloEnergies)
  DECLARE_TOOL(JetCaloQualityTool)
  DECLARE_TOOL(JetCaloCellQualityTool)
  DECLARE_TOOL(JetWidthTool)
  DECLARE_TOOL(JetBadChanCorrTool)
  DECLARE_TOOL(JetMuonSegmentMomentsTool)
  DECLARE_TOOL(JetTrackMomentsTool)
  DECLARE_TOOL(JetVertexFractionTool)
  DECLARE_TOOL(JetVertexTaggerTool)
  DECLARE_TOOL(JetPtAssociationTool)
  DECLARE_TOOL(JetIsolationTool)
  DECLARE_TOOL(JetLArHVTool)
    DECLARE_TOOL(JetOriginCorrectionTool)
}

