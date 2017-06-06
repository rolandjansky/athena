/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopExamples/MiniToFlatAnalysis.h"

#include <string>

#include "TopConfiguration/TopConfig.h"

#include "TopEvent/EventTools.h"
#include "TopEvent/TopEventMaker.h"
#include "TopEvent/Event.h"

#include "TopEventSelectionTools/PlotManager.h"
#include "TH1.h"
#include "TH1D.h"
#include "TFile.h"
#include "TDirectoryFile.h"

namespace top {
  MiniToFlatAnalysis::MiniToFlatAnalysis(const std::string& name) :
    asg::AsgTool(name),
    m_config(nullptr),

    m_topTools(nullptr),
    m_sfCalc(nullptr),
    m_evtMaker(nullptr),

    m_outputFileName("SetMe"),
    m_outputFile(nullptr) {
    declareProperty("config" , m_config);
    declareProperty("outputFileName" , m_outputFileName = "miniout.root");
  }

  StatusCode MiniToFlatAnalysis::initialize() {
    m_topTools = std::unique_ptr<top::TopToolStore> (new top::TopToolStore("top::TopToolStore"));
    top::check(m_topTools->setProperty("config", m_config) ,
               "Failed to set config for topTools");
    top::check(m_topTools->initialize() , "Failed to initialize");

    m_sfCalc = std::unique_ptr<top::ScaleFactorCalculator> (new top::ScaleFactorCalculator("top::ScaleFactorCalculator"));
    top::check(m_sfCalc->setProperty("config", m_config) ,
               "Failed to set config for sfCalc");
    top::check(m_sfCalc->initialize() , "Failed to initialize");

    m_evtMaker = std::unique_ptr<top::TopEventMaker>(new top::TopEventMaker("top::TopEventMaker"));
    top::check(m_evtMaker->setProperty("config", m_config),
               "Failed to set config for evtMaker");
    top::check(m_evtMaker->initialize(), "Failed to initialize");

    m_outputFile = new TFile(m_outputFileName.c_str(), "RECREATE");

    m_event_saver = std::unique_ptr<top::EventSaverFlatNtuple>(new top::EventSaverFlatNtuple());
    m_event_saver->initialize(m_config, m_outputFile, *(m_config->allSelectionNames().get()));

    return StatusCode::SUCCESS;
  }

  StatusCode MiniToFlatAnalysis::execute() {
    // Re-do Scale Factors - You cannot do this with a flat ntuple
    top::check(m_sfCalc->execute() , "Failed to calculate scale factors");

    const xAOD::SystematicEventContainer* allSystematics = m_evtMaker->systematicEvents(m_config->sgKeyTopSystematicEvents());
    // ATH_MSG_INFO(" Total number of SystematicEvents (inc Nominal) = "<<allSystematics->size());

    for (auto currentSystematic : *allSystematics) {
      // Make a top::Event
      top::Event topEvent = m_evtMaker->makeTopEvent(*currentSystematic);
      topEvent.m_saveEvent = false;

      // Reset the ttree indicies (probably should be done somewhere else)
      if (topEvent.m_isLoose) {
        topEvent.m_ttreeIndex = m_config->ttreeIndexLoose(topEvent.m_hashValue);
      } else {
        topEvent.m_ttreeIndex = m_config->ttreeIndex(topEvent.m_hashValue);
      }

      for (auto sel : *m_config->allSelectionNames()) {
        if (currentSystematic->auxdataConst<int>(sel) == 1) {
          topEvent.m_saveEvent = true;
          break;
        }
      }

      m_event_saver->saveEvent(topEvent);
    }

    return StatusCode::SUCCESS;
  }


  StatusCode MiniToFlatAnalysis::finalize() {
    m_outputFile->cd();
    m_event_saver->finalize();

    return StatusCode::SUCCESS;
  }

}  // namespace top
