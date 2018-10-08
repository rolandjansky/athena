/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file ToolTestMonitoringPlots.cxx
 * @author shaun roe
 **/

#include "InDetPhysValMonitoring/ToolTestMonitoringPlots.h"

#include <vector>
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODEventInfo/EventInfo.h"

#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticle.h"
#include "InDetTestPlot.h"
#include <limits>
#include <cmath> // to get std::isnan()

// #include <unordered_map>

using std::pair;
using std::make_pair;
using std::cout;
using std::endl;
class TGraph;



///Parametrized constructor
ToolTestMonitoringPlots::ToolTestMonitoringPlots(const std::string& type, const std::string& name,
                                                 const IInterface* parent) :
  ManagedMonitorToolBase(type, name, parent) {
  // declareProperty("TrackParticleContainerName", m_trkParticleName="InDetTrackParticles");
  // declareProperty("TruthParticleContainerName", m_truthParticleName="TruthParticles");

  m_monPlots = std::unique_ptr<InDetTestPlot> (new InDetTestPlot(0, "IDPerformanceMon/"));
}

ToolTestMonitoringPlots::~ToolTestMonitoringPlots() {
}

StatusCode
ToolTestMonitoringPlots::initialize() {
  ATH_MSG_DEBUG("Initializing " << name() << "...");
  ATH_CHECK(ManagedMonitorToolBase::initialize());
  return StatusCode::SUCCESS;
}

StatusCode
ToolTestMonitoringPlots::fillHistograms() {
  ATH_MSG_DEBUG("Filling hists " << name() << "...");
  // retrieve trackParticle container
  // auto ptracks = getContainer<xAOD::TrackParticleContainer>(m_trkParticleName);
  // if ((!ptracks) ) return StatusCode::FAILURE;
  // retrieve truthParticle container
  // auto ptruth = getContainer<xAOD::TruthParticleContainer>(m_truthParticleName);
  // if ((!ptruth) ) return StatusCode::FAILURE;

  // const unsigned int nTracks(ptracks->size());
  // const unsigned int nTruth(ptruth->size());

  // std::string m_truthContainerName = "TruthParticles";
  // const xAOD::TruthParticleContainer* truthParticles =
  // getContainer<xAOD::TruthParticleContainer>(m_truthContainerName);

  // Main track loop, filling Track-only, Track 'n' Truth with good matching probability (meas, res, & pull), and Fakes
  const xAOD::TrackParticle p;
  for (int i(0); i != 200; ++i) {
    m_monPlots->fill(p);                                         // Make all the plots requiring only trackParticle
  }
  return StatusCode::SUCCESS;
}

StatusCode
ToolTestMonitoringPlots::bookHistograms() {
  ATH_MSG_INFO("Booking hists " << name() << "...");
  m_monPlots->setDetailLevel(100); // DEBUG, enable expert histograms
  m_monPlots->initialize();
  std::vector<HistData> hists = m_monPlots->retrieveBookedHistograms();
  for (auto& hist : hists) {
    ATH_CHECK(regHist(hist.first, hist.second, all)); // ??
  }
  
  std::vector<EfficiencyData> effs = m_monPlots->retrieveBookedEfficiencies();
  for (auto& eff : effs) {
    ATH_CHECK(regGraph(reinterpret_cast<TGraph*>(eff.first), eff.second, all)); // ??
  }
  
  return StatusCode::SUCCESS;
}

StatusCode
ToolTestMonitoringPlots::procHistograms() {
  ATH_MSG_INFO("Finalising hists " << name() << "...");
  if (endOfRunFlag()) {
    m_monPlots->finalize();
  }
  return StatusCode::SUCCESS;
}
