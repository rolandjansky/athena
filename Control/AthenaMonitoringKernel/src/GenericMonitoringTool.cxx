/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <map>
#include <mutex>
#include <algorithm>

#include <TH1.h>
#include <TH2.h>
#include <TProfile.h>
#include <TProfile2D.h>

#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "AthenaMonitoringKernel/HistogramDef.h"
#include "AthenaMonitoringKernel/HistogramFiller.h"
#include "AthenaMonitoringKernel/IMonitoredVariable.h"

#include "HistogramFiller/HistogramFillerFactory.h"

using namespace Monitored;

GenericMonitoringTool::GenericMonitoringTool(const std::string & type, const std::string & name, const IInterface* parent)
  : AthAlgTool(type, name, parent) { }

GenericMonitoringTool::~GenericMonitoringTool() { }

StatusCode GenericMonitoringTool::initialize() {
  ATH_CHECK(m_histSvc.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode GenericMonitoringTool::start() {
  if ( not m_explicitBooking ) {
    ATH_MSG_DEBUG("Proceeding to histogram booking");
    return book();
  }
  return StatusCode::SUCCESS;
}

StatusCode GenericMonitoringTool::stop() {
  m_fillers.clear();
  return StatusCode::SUCCESS;
}

StatusCode GenericMonitoringTool::book() {

  // If no histogram path given use parent or our own name
  if (m_histoPath.empty()) {
    auto named = dynamic_cast<const INamedInterface*>(parent());
    m_histoPath = named ? named->name() : name();
  }

  // Replace dot (e.g. MyAlg.MyTool) with slash to create sub-directory
  std::replace( m_histoPath.begin(), m_histoPath.end(), '.', '/' );

  ATH_MSG_DEBUG("Booking histograms in path: " << m_histoPath.value());

  HistogramFillerFactory factory(this, m_histoPath);

  for (const std::string& item : m_histograms) {
    if (item.empty()) {
      ATH_MSG_DEBUG( "Skipping empty histogram definition" );
      continue;
    }
    ATH_MSG_DEBUG( "Configuring monitoring for: " << item );
    HistogramDef def = HistogramDef::parse(item);

    if (def.ok) {
      std::shared_ptr<HistogramFiller> filler(factory.create(def));

      if (filler) {
	m_fillers.push_back(filler);
      } else {
        ATH_MSG_WARNING( "The histogram filler cannot be instantiated for: " << def.name );
      }
    } else {
      ATH_MSG_ERROR( "Unparsable histogram definition: " << item );
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG( "Monitoring for variable " << def.name << " prepared" );
  }

  if ( m_fillers.empty() && m_failOnEmpty ) {
    std::string hists;
    for (const auto &h : m_histograms) hists += (h+",");
    ATH_MSG_ERROR("No monitored variables created based on histogram definition: [" << hists <<
                  "] Remove this monitoring tool or check its configuration.");
    return StatusCode::FAILURE;
  }


  return StatusCode::SUCCESS;
}

namespace Monitored {
    std::ostream& operator<< ( std::ostream& os, const std::reference_wrapper<Monitored::IMonitoredVariable>& rmv ) {
        std::string s = rmv.get().name();
        return os << s;
    }
}


void GenericMonitoringTool::invokeFillers(const std::vector<std::reference_wrapper<Monitored::IMonitoredVariable>>& monitoredVariables) const {
  std::scoped_lock guard(m_fillMutex);
  for ( auto filler: m_fillers ) {
    m_vars.reset();
    const int fillerCardinality = filler->histogramVariablesNames().size() + (filler->histogramWeightName().empty() ? 0: 1) + (filler->histogramCutMaskName().empty() ? 0 : 1);

    if ( fillerCardinality == 1 ) { // simplest case, optimising this to be super fast
      for ( auto& var: monitoredVariables ) {
        if ( var.get().name().compare( filler->histogramVariablesNames()[0] ) == 0 )  {
	  m_vars.var[0] = &var.get();
          filler->fill( m_vars );
          break;
        }
      }
    } else { // a more complicated case, and cuts or weights
      int matchesCount = 0;
      for ( const auto& var: monitoredVariables ) {
	bool matched = false;
	for ( unsigned fillerVarIndex = 0; fillerVarIndex < filler->histogramVariablesNames().size(); ++fillerVarIndex ) {
	  if ( var.get().name().compare( filler->histogramVariablesNames()[fillerVarIndex] ) == 0 ) {
	    m_vars.set(fillerVarIndex, &var.get());
	    matched = true;
	    matchesCount++;
	    break;
	  }
	}
	if ( matchesCount == fillerCardinality ) break;
	if ( not matched ) { // may be a weight or cut variable still
	  if ( var.get().name().compare( filler->histogramWeightName() ) == 0 )  {
	    m_vars.weight = &var.get();
	    matchesCount ++;
	  } else if ( var.get().name().compare( filler->histogramCutMaskName() ) == 0 )  {
	    m_vars.cut = &var.get();
	    matchesCount++;
	  }
	}
	if ( matchesCount == fillerCardinality ) break;
      }
      if ( matchesCount == fillerCardinality ) {
	filler->fill( m_vars );
      } else if ( ATH_UNLIKELY( matchesCount != 0 ) ) { // something has matched, but not all, worth informing user
	bool reasonFound = false;
	if (ATH_UNLIKELY(!filler->histogramWeightName().empty() && !m_vars.weight)) {
	  reasonFound = true;
	  ATH_MSG_DEBUG("Filler weight not found in monitoredVariables:"
			<< "\n  Filler weight               : " << filler->histogramWeightName()
			<< "\n  Asked to fill from mon. vars: " << monitoredVariables);
	}
	if (ATH_UNLIKELY(!filler->histogramCutMaskName().empty() && !m_vars.cut)) {
	  reasonFound = true;
	  ATH_MSG_DEBUG("Filler cut mask not found in monitoredVariables:"
			<< "\n  Filler cut mask             : " << filler->histogramCutMaskName()
			<< "\n  Asked to fill from mon. vars: " << monitoredVariables);
	}
	if ( not reasonFound ) {
	  ATH_MSG_DEBUG("Filler has different variables than monitoredVariables:"
			<< "\n  Filler variables            : " << filler->histogramVariablesNames()
			<< "\n  Asked to fill from mon. vars: " << monitoredVariables
			<< "\n  Selected monitored variables: " << m_vars.names() );
	}
      }
    }
  }
}

uint32_t GenericMonitoringTool::runNumber() {
  return Gaudi::Hive::currentContext().eventID().run_number();
}

uint32_t GenericMonitoringTool::lumiBlock() {
  return Gaudi::Hive::currentContext().eventID().lumi_block();
}
