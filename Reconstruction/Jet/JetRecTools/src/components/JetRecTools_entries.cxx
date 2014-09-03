


#include "GaudiKernel/DeclareFactoryEntries.h"
#include "JetRecTools/JetDetailedTrackSelectorTool.h"
#include "JetRecTools/JetTrackSelectionTool.h"
#include "JetRecTools/TrackVertexAssociationTool.h"
#include "JetRecTools/TrackPseudoJetGetter.h"
#include "JetRecTools/MissingCellListTool.h"

#include "JetRecTools/PFlowPseudoJetGetter.h"

DECLARE_TOOL_FACTORY(JetDetailedTrackSelectorTool)
DECLARE_TOOL_FACTORY(JetTrackSelectionTool)
DECLARE_TOOL_FACTORY(TrackVertexAssociationTool)
DECLARE_TOOL_FACTORY(TrackPseudoJetGetter)
DECLARE_TOOL_FACTORY(MissingCellListTool)
DECLARE_TOOL_FACTORY(PFlowPseudoJetGetter)

DECLARE_FACTORY_ENTRIES(JetRecTools) {

  DECLARE_TOOL(JetDetailedTrackSelectorTool)
  DECLARE_TOOL(JetTrackSelectionTool)
  DECLARE_TOOL(TrackVertexAssociationTool)
  DECLARE_TOOL(TrackPseudoJetGetter)
  DECLARE_TOOL(MissingCellListTool)
  DECLARE_TOOL(PFlowPseudoJetGetter)


}

