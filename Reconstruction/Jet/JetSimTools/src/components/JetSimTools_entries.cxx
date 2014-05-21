#include "JetSimTools/JetTruthParticleSelectorTool.h"
#include "JetSimTools/TruthPseudoJetGetter.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY( JetTruthParticleSelectorTool )

DECLARE_TOOL_FACTORY( TruthPseudoJetGetter )

DECLARE_FACTORY_ENTRIES(JetSimTools) {
  DECLARE_TOOL( JetTruthParticleSelectorTool );
  DECLARE_TOOL( TruthPseudoJetGetter );

}



