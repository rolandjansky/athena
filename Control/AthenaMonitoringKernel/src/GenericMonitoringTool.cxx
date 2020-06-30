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
  m_fillerMap.clear();
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

  // First, make a vector of fillers. Then move the pointers to an unordered_map en masse.
  std::vector<std::shared_ptr<Monitored::HistogramFiller>> fillers;
  fillers.reserve(m_histograms.size());
  m_fillerMap.reserve(m_histograms.size());
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
        fillers.push_back(filler);
      } else {
        ATH_MSG_WARNING( "The histogram filler cannot be instantiated for: " << def.name );
      }
    } else {
      ATH_MSG_ERROR( "Unparsable histogram definition: " << item );
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG( "Monitoring for variable " << def.name << " prepared" );
  }

  if ( fillers.empty() && m_failOnEmpty ) {
    std::string hists;
    for (const auto &h : m_histograms) hists += (h+",");
    ATH_MSG_ERROR("No monitored variables created based on histogram definition: [" << hists <<
                  "] Remove this monitoring tool or check its configuration.");
    return StatusCode::FAILURE;
  }

  for (const auto& filler : fillers ) {
    const auto& fillerVariables = filler->histogramVariablesNames();
    for (const auto& fillerVariable : fillerVariables) {
      m_fillerMap[fillerVariable].push_back(filler);
    }
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

  // stage 1: get candidate fillers (assume generally we get only a few variables)
  std::vector<HistogramFiller*> candidates;
  for (const auto& monValue : monitoredVariables) {
    const auto& match = m_fillerMap.find(monValue.get().name());
    if (match != m_fillerMap.end()) {
      candidates.reserve(candidates.size() + match->second.size());
      for (const auto& i : match->second) {
        candidates.push_back(i.get());
      }
    }
  }
  // dedup vector (yes, this is faster than using std::set above)
  std::sort(candidates.begin(), candidates.end());
  candidates.erase(std::unique(candidates.begin(), candidates.end()), candidates.end());
  // stage 2: refine for fillers that have all variables set
  std::vector<std::reference_wrapper<IMonitoredVariable>> variables;
  variables.reserve(3); // enough for all current fillers

  for (const auto& filler : candidates) {
    // Find the associated monitored variable for each histogram's variable(s)
    const auto& fillerVariables = filler->histogramVariablesNames();

    variables.clear();
    for (const auto& fillerVariable : fillerVariables) {
      for (const auto& monValue : monitoredVariables) {
        if (fillerVariable.compare(monValue.get().name()) == 0) {
          variables.push_back(monValue);
          break;
        }
      }
    }

    // Find the weight variable in the list of monitored variables
    const auto& fillerWeight = filler->histogramWeightName();
    Monitored::IMonitoredVariable* weight(nullptr);
    if ( not fillerWeight.empty() ) {
      for (const auto& monValue : monitoredVariables) {
        if (fillerWeight.compare(monValue.get().name()) == 0) {
          weight = &monValue.get();
          break;
        }
      }
    }

    // Find the cutMask variable in the list of monitored variables
    const auto& fillerCutMask = filler->histogramCutMaskName();
    Monitored::IMonitoredVariable* cutmask(nullptr);
    if ( not fillerCutMask.empty() ) {
      for (const auto& monValue : monitoredVariables) {
        if (fillerCutMask.compare(monValue.get().name()) == 0) {
          cutmask = &monValue.get();
          break;
        }
      }
    }

    if (fillerVariables.size() != variables.size()) {
      ATH_MSG_DEBUG("Filler has different variables than monitoredVariables:"
                    << "\n  Filler variables            : " << fillerVariables
                    << "\n  Asked to fill from mon. vars: " << monitoredVariables
                    << "\n  Selected monitored variables: " << variables);
      continue;
    }
    if (ATH_UNLIKELY(!fillerWeight.empty() && !weight)) {
      ATH_MSG_DEBUG("Filler weight not found in monitoredVariables:"
                    << "\n  Filler weight               : " << fillerWeight
                    << "\n  Asked to fill from mon. vars: " << monitoredVariables);
      continue;
    }
    if (ATH_UNLIKELY(!fillerCutMask.empty() && !cutmask)) {
      ATH_MSG_DEBUG("Filler cut mask not found in monitoredVariables:"
                    << "\n  Filler cut mask             : " << fillerCutMask
                    << "\n  Asked to fill from mon. vars: " << monitoredVariables);
      continue;
    }
    
    std::scoped_lock guard(m_fillMutex);
    filler->setMonitoredVariables(std::move(variables));
    filler->setMonitoredWeight(weight);
    filler->setMonitoredCutMask(cutmask);
    filler->fill();    
  }  
}

uint32_t GenericMonitoringTool::runNumber() {
  return Gaudi::Hive::currentContext().eventID().run_number();
}

uint32_t GenericMonitoringTool::lumiBlock() {
  return Gaudi::Hive::currentContext().eventID().lumi_block();
}
