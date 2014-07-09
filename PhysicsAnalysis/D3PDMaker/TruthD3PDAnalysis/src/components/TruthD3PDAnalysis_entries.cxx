// $Id: TruthD3PDAnalysis_entries.cxx 551562 2013-06-18 13:13:00Z zmarshal $

// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "../GenObjectsFilterTool.h"
#include "../TruthParticleFilterTool.h"
#include "../SimpleTruthParticleFilterTool.h"
#include "../D3PDMCTruthClassifier.h"

DECLARE_NAMESPACE_TOOL_FACTORY (D3PD, GenObjectsFilterTool)
DECLARE_NAMESPACE_TOOL_FACTORY (D3PD, TruthParticleFilterTool)
DECLARE_NAMESPACE_TOOL_FACTORY (D3PD, SimpleTruthParticleFilterTool)
DECLARE_NAMESPACE_TOOL_FACTORY (D3PD, D3PDMCTruthClassifier)

DECLARE_FACTORY_ENTRIES( TruthD3PDAnalysis ) {

  DECLARE_NAMESPACE_TOOL (D3PD, GenObjectsFilterTool)
  DECLARE_NAMESPACE_TOOL (D3PD, TruthParticleFilterTool)
  DECLARE_NAMESPACE_TOOL (D3PD, SimpleTruthParticleFilterTool)
  DECLARE_NAMESPACE_TOOL (D3PD, D3PDMCTruthClassifier)

}
