
#include "JetMonitoring/JetKinematicHistos.h"
#include "JetMonitoring/JetAttributeHisto.h"
#include "JetMonitoring/HistoDefinitionTool.h"
#include "JetMonitoring/HistosForJetSelection.h"
#include "JetMonitoring/LeadingJetsRelations.h"
#include "JetMonitoring/EfficiencyResponseHistos.h"
#include "JetMonitoring/JetContainerHistoFiller.h"

#include "../JetMonitoringTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY( JetKinematicHistos )
DECLARE_TOOL_FACTORY( JetMonitoringTool )
DECLARE_TOOL_FACTORY( HistoDefinitionTool )
DECLARE_TOOL_FACTORY( HistosForJetSelection )
DECLARE_TOOL_FACTORY( LeadingJetsRelations )
DECLARE_TOOL_FACTORY( EfficiencyResponseHistos )
DECLARE_TOOL_FACTORY( JetContainerHistoFiller )


DECLARE_TOOL_FACTORY( JetAttributeHisto )


DECLARE_FACTORY_ENTRIES(JetMonitoring) {
  DECLARE_TOOL( JetKinematicHistos );
  DECLARE_TOOL( JetMonitoringTool );
  DECLARE_TOOL( JetAttributeHisto );
  DECLARE_TOOL( LeadingJetsRelations );
  DECLARE_TOOL( EfficiencyResponseHistos );

  DECLARE_TOOL( JetContainerHistoFiller );

  DECLARE_TOOL( HistoDefinitionTool );
  DECLARE_TOOL( HistosForJetSelection );
 
}



