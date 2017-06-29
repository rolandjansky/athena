// JetRecTools_entries.cxx

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "JetRecTools/JetTrackSelectionTool.h"
#include "JetRecTools/SimpleJetTrackSelectionTool.h"
#include "JetRecTools/TrackVertexAssociationTool.h"
#include "JetRecTools/TrackPseudoJetGetter.h"
#include "JetRecTools/PFlowPseudoJetGetter.h"
#include "JetRecTools/JetConstituentModSequence.h"
#include "JetRecTools/JetConstituentModifierBase.h"
#include "JetRecTools/CaloClusterConstituentsOrigin.h"
#include "JetRecTools/SoftKillerWeightTool.h"
#include "JetRecTools/VoronoiWeightTool.h"
#include "JetRecTools/ClusterAtEMScaleTool.h"
#include "JetRecTools/ConstituentSubtractorTool.h"
#include "JetRecTools/JetInputElRemovalTool.h"
#include "JetRecTools/CorrectPFOTool.h"
#include "JetRecTools/ChargedHadronSubtractionTool.h"

DECLARE_TOOL_FACTORY(JetTrackSelectionTool)
DECLARE_TOOL_FACTORY(SimpleJetTrackSelectionTool)
DECLARE_TOOL_FACTORY(TrackVertexAssociationTool)
DECLARE_TOOL_FACTORY(TrackPseudoJetGetter)
DECLARE_TOOL_FACTORY(PFlowPseudoJetGetter)
DECLARE_TOOL_FACTORY(JetConstituentModSequence)
DECLARE_TOOL_FACTORY(JetConstituentModifierBase)
DECLARE_TOOL_FACTORY(CaloClusterConstituentsOrigin)
DECLARE_TOOL_FACTORY(SoftKillerWeightTool)
DECLARE_TOOL_FACTORY( VoronoiWeightTool )
DECLARE_TOOL_FACTORY( ClusterAtEMScaleTool )
DECLARE_TOOL_FACTORY( ConstituentSubtractorTool )
DECLARE_TOOL_FACTORY( JetInputElRemovalTool )
DECLARE_TOOL_FACTORY( CorrectPFOTool )
DECLARE_TOOL_FACTORY( ChargedHadronSubtractionTool )

DECLARE_FACTORY_ENTRIES(JetRecTools) {
  DECLARE_TOOL(JetTrackSelectionTool)
  DECLARE_TOOL(SimpleJetTrackSelectionTool)
  DECLARE_TOOL(TrackVertexAssociationTool)
  DECLARE_TOOL(TrackPseudoJetGetter)
  DECLARE_TOOL(PFlowPseudoJetGetter)
  DECLARE_TOOL(JetConstituentModSequence)
  DECLARE_TOOL(JetConstituentModifierBase)
  DECLARE_TOOL(CaloClusterConstituentsOrigin)
  DECLARE_TOOL(SoftKillerWeightTool)
  DECLARE_TOOL( ConstituentSubtractorTool )
  DECLARE_TOOL( VoronoiWeightTool )
  DECLARE_TOOL( ClusterAtEMScaleTool )
  DECLARE_TOOL( JetInputElRemovalTool )
  DECLARE_TOOL( CorrectPFOTool )
  DECLARE_TOOL( ChargedHadronSubtractionTool )

}
