// JetRec_entries.cxx

#include "../JetAlgorithm.h"
#include "JetRec/JetToolRunner.h"
#include "JetRec/JetRecTool.h"
#include "JetRec/JetDumper.h"
#include "JetRec/PseudoJetGetter.h"
#include "JetRec/MuonSegmentPseudoJetGetter.h"
#include "JetRec/JetFromPseudojet.h"
#include "JetRec/JetFinder.h"
#include "JetRec/JetByVertexFinder.h"
#include "JetRec/JetSplitter.h"
#include "JetRec/JetTrimmer.h"
#include "JetRec/JetPruner.h"
#include "JetRec/JetSoftDrop.h"
#include "JetRec/JetReclusterer.h"
#include "JetRec/FastJetInterfaceTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "JetRec/JetSorter.h"
#include "JetRec/JetPseudojetRetriever.h"
#include "JetRec/JetConstituentsRetriever.h"
#include "JetRec/JetPseudojetCopier.h"
#include "JetRec/JetFilterTool.h"
#include "JetRec/JetConstitRemover.h"
#include "JetRec/JetSJCalibrationGroomer.h"

DECLARE_TOOL_FACTORY(JetToolRunner)
DECLARE_TOOL_FACTORY(JetRecTool)
DECLARE_TOOL_FACTORY(JetDumper)
DECLARE_TOOL_FACTORY(PseudoJetGetter)
DECLARE_TOOL_FACTORY(MuonSegmentPseudoJetGetter)
DECLARE_TOOL_FACTORY(JetFromPseudojet)
DECLARE_TOOL_FACTORY(JetFinder)
DECLARE_TOOL_FACTORY(JetByVertexFinder)
DECLARE_TOOL_FACTORY(JetSorter)
DECLARE_TOOL_FACTORY(JetFilterTool)
DECLARE_TOOL_FACTORY(JetSplitter)
DECLARE_TOOL_FACTORY(JetTrimmer)
DECLARE_TOOL_FACTORY(JetPruner)
DECLARE_TOOL_FACTORY(JetSoftDrop)
DECLARE_TOOL_FACTORY(JetReclusterer)
DECLARE_TOOL_FACTORY(FastJetInterfaceTool)
DECLARE_TOOL_FACTORY(JetPseudojetRetriever)
DECLARE_TOOL_FACTORY(JetConstituentsRetriever)
DECLARE_TOOL_FACTORY(JetPseudojetCopier)
DECLARE_TOOL_FACTORY(JetConstitRemover)
DECLARE_TOOL_FACTORY(JetSJCalibrationGroomer)

DECLARE_ALGORITHM_FACTORY(JetAlgorithm)

DECLARE_FACTORY_ENTRIES(JetRec) {
  DECLARE_TOOL(JetToolRunner)
  DECLARE_TOOL(JetRecTool)
  DECLARE_TOOL(JetDumper)
  DECLARE_TOOL(PseudoJetGetter)
  DECLARE_TOOL(MuonSegmentPseudoJetGetter)
  DECLARE_TOOL(JetFinder)
  DECLARE_TOOL(JetByVertexFinder)
  DECLARE_TOOL(JetFromPseudojet)
  DECLARE_TOOL(JetSorter)
  DECLARE_TOOL(JetFilterTool)
  DECLARE_TOOL(JetSplitter)
  DECLARE_TOOL(JetTrimmer)
  DECLARE_TOOL(JetPruner)
  DECLARE_TOOL(JetSoftDrop)
  DECLARE_TOOL(JetReclusterer)
  DECLARE_TOOL(FastJetInterfaceTool)
  DECLARE_TOOL(JetPseudojetRetriever)
  DECLARE_TOOL(JetConstituentsRetriever)
  DECLARE_TOOL(JetPseudojetCopier)
  DECLARE_TOOL(JetConstitRemover)
  DECLARE_TOOL(JetSJCalibrationGroomer)

  DECLARE_ALGORITHM(JetAlgorithm)
}

