/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <map>
#include <mutex>
#include <TH1.h>
#include <TH2.h>
#include <TProfile.h>
#include <TProfile2D.h>

#include "AthenaMonitoring/GenericMonitoringTool.h"
#include "AthenaMonitoring/HistogramDef.h"

using namespace std;
using namespace Monitored;

const InterfaceID& GenericMonitoringTool::interfaceID() {
	static InterfaceID GenericMonitoringTool_ID("GenericMonitoringTool", 1, 0);

	return GenericMonitoringTool_ID;
}

GenericMonitoringTool::GenericMonitoringTool(const std::string & type, const std::string & name, const IInterface* parent)
  : AthAlgTool(type, name, parent), m_histSvc("THistSvc", name) { 
  declareProperty("Histograms", m_histograms, "Definitions of histograms");
  declareProperty("HistogramsGroupName", m_histogramsGroupName, "Name of group to which histograms would be generated");
  declareInterface<GenericMonitoringTool>(this);
}

GenericMonitoringTool::~GenericMonitoringTool() { }

StatusCode GenericMonitoringTool::initialize() {
  ATH_CHECK(m_histSvc.retrieve());
  ATH_CHECK(!m_histogramsGroupName.empty());
  
  HistogramFillerFactory factory(m_histSvc, m_histogramsGroupName);

  for (const string& item : m_histograms) {
    ATH_MSG_DEBUG( "Configuring monitoring from: " << item );

    HistogramDef def = HistogramDef::parse(item);

    if (def.ok) {
        ATH_MSG_DEBUG( "Definition parsed correctly" );
        HistogramFiller* filler = factory.create(def);
        
        if (filler != nullptr) {

            m_fillers.push_back(filler);
	    ATH_MSG_DEBUG( "Intermediate structures created" );
        } else {
	  ATH_MSG_WARNING( "The histogram filler can not be instantiated for: " << def.name );
	}
    } else {
      ATH_MSG_ERROR( "Unparsable histogram definition: " << item );
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG( "Monitoring for varaible " << def.name << " prepared" );
  }

  if ( m_fillers.empty() ) {
    ATH_MSG_ERROR("No variables to be monitored, detach this tool, it will save time");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

vector<HistogramFiller*> GenericMonitoringTool::getHistogramsFillers(vector<reference_wrapper<Monitored::IMonitoredVariable>> monitoredVariables) {
  vector<HistogramFiller*> result;

  for (auto filler : m_fillers) {
    auto fillerVariables = filler->histogramVariablesNames();
    vector<reference_wrapper<Monitored::IMonitoredVariable>> variables;

    for (auto fillerVariable : fillerVariables) {
      for (auto monValue : monitoredVariables) {
        if (fillerVariable.compare(monValue.get().name()) == 0) {
          variables.push_back(monValue);
          break;
        }
      }
    }

    if (fillerVariables.size() != variables.size()) {
      ATH_MSG_DEBUG("Filler has different variables than monitoredVariables");
      continue;
    }

    HistogramFiller* fillerCopy = filler->clone();
    fillerCopy->setMonitoredVariables(variables);
    result.push_back(fillerCopy);
  }

  return result;
}
