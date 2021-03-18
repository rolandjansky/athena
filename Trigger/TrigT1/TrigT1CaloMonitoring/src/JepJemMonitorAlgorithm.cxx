/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "JepJemMonitorAlgorithm.h"

JepJemMonitorAlgorithm::JepJemMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator)
{
  // probably unecessary
  //  m_inputJELocation=JetElements::xAODJetElementLocation;
}

StatusCode JepJemMonitorAlgorithm::initialize() {

  ATH_MSG_DEBUG("JepJemMonitorAlgorith::initialize");
  ATH_MSG_DEBUG("Package Name "<< m_packageName);

  // container names
  ATH_MSG_DEBUG("m_xAODTriggerTowerContainerName"<< m_xAODTriggerTowerContainerName); 
  ATH_MSG_DEBUG("m_JetElementLocation"<< m_JetElementLocation); 

  // steering parameters
  ATH_MSG_DEBUG("m_maxSlices"<< m_maxSlices);

  // we initialise all the containers that we need
  ATH_CHECK(m_xAODTriggerTowerContainerName.initialize());
  ATH_CHECK(m_JetElementLocation.initialize());

  // retrieve any tools if needed
  // ATH_CHECK(myTool.retrieve());

  return AthMonitorAlgorithm::initialize();
}

StatusCode JepJemMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {
  // ===============================
  // RETRIEVE AND SETUP JET ELEMENTS
  // ===============================
  ATH_MSG_DEBUG("JepJemMonitorAlgorithm::fillHistograms");

  // Retrieve Trigger Towers from SG
  SG::ReadHandle<xAOD::TriggerTowerContainer> triggerTowerTES(m_xAODTriggerTowerContainerName, ctx);
  ATH_CHECK(triggerTowerTES.isValid());

  // Retrieve jet elements from SG
  SG::ReadHandle<xAOD::JetElementContainer> JetElementTES(m_JetElementLocation, ctx);
  ATH_CHECK(JetElementTES.isValid());

  // array of monitored items
  std::vector<std::reference_wrapper<Monitored::IMonitoredVariable>> variables;

  ATH_MSG_DEBUG("Retrieve input TriggerTowers " << m_JetElementLocation.key());
  SG::ReadHandle<xAOD::JetElementContainer> inputJEs(m_JetElementLocation, ctx);
  
  // create vectors of jet elements with quantities to be monitored
  std::vector<MonitorJepJemJE> monJepJemJEs_em;  // EM elements
  std::vector<MonitorJepJemJE> monJepJemJEs_had; // HAD elements
  std::vector<MonitorJepJemJE> monJepJemJEs;     // ALL elements
  ATH_CHECK(fillJepJemJEVectors(JetElementTES, monJepJemJEs_em, monJepJemJEs_had));

  auto run = Monitored::Scalar<int>("run",GetEventInfo(ctx)->runNumber());
  variables.push_back(run);

  // ============
  // For 2D plots
  // ============
  std::vector<MonitorJepJemJE> monJEs;  // for filling cutmasks

  int nSlices = static_cast<int>(m_maxSlices);  // not sure how to properly convert from 'const Gaudi::Property<int>' to 'int'
  auto hitmapVectsEM  = std::make_unique<std::vector<MonitorJepJemJE>[]>(nSlices);  // for filling EM timeslices
  auto hitmapVectsHAD = std::make_unique<std::vector<MonitorJepJemJE>[]>(nSlices);  // for filling HAD timeslices

  xAOD::JetElementContainer::const_iterator jeIterator = (*JetElementTES).begin();
  xAOD::JetElementContainer::const_iterator jeIteratorEnd = (*JetElementTES).end();
  for (; jeIterator != jeIteratorEnd; ++jeIterator) {
    const xAOD::JetElement* je = *jeIterator;
    const double eta = je->eta();
    const double phi = je->phi();
    const uint8_t em  = je->emJetElementET();
    const uint8_t had = je->hadJetElementET();

    // for cutmasks
    if (em > 0)  {
      MonitorJepJemJE monJE;
      monJE.eta = eta;
      monJE.phi = phi;
      monJEs.push_back(monJE);
    }
    if (had > 0) {
      MonitorJepJemJE monJE;
      monJE.eta = eta;
      monJE.phi = phi;
      monJEs.push_back(monJE);
    }

    // for the hitmaps per timeslice
    const std::vector<uint16_t>& emEnergyVec(je->emJetElementETVec());
    const std::vector<uint16_t>& hadEnergyVec(je->hadJetElementETVec());

    const int slicesEm = emEnergyVec.size();
    const int slicesHad = hadEnergyVec.size();

    for (int i=0; i < m_maxSlices; i++) {
      if (i < slicesEm && emEnergyVec[i] > 0) {
        MonitorJepJemJE monJE_slice;
        monJE_slice.eta = eta;
        monJE_slice.phi = phi;
        hitmapVectsEM[i].push_back(monJE_slice);
      }
      if (i < slicesHad && hadEnergyVec[i] > 0) {
        MonitorJepJemJE monJE_slice;
        monJE_slice.eta = eta;
        monJE_slice.phi = phi;
        hitmapVectsHAD[i].push_back(monJE_slice);
      }
    }
  }
  
  // setup cutmasks to select the em and had JEs
  auto mask_em_JE = Monitored::Collection("mask_em_JE", monJEs, []( const auto &je ) { return (je.jelement->emJetElementET() > 0); });
  variables.push_back(mask_em_JE);
  auto mask_had_JE = Monitored::Collection("mask_had_JE", monJEs, []( const auto &je ) { return (je.jelement->hadJetElementET() > 0); });
  variables.push_back(mask_had_JE);

  // monitored eta and phi for slices
  std::string varNameEta, varNamePhi;
  std::stringstream buffer;
  for (int i=0; i < m_maxSlices; i++) {
    buffer.str("");
    buffer << i;

    // em
    auto etaSliceJepJemJE_em = Monitored::Collection("etaSliceJepJemJE_em_num"+buffer.str(), hitmapVectsEM[i], []( const auto &je ) { return (je.jelement->eta()); });
    variables.push_back(etaSliceJepJemJE_em);
    auto phiSliceJepJemJE_em = Monitored::Collection("phiSliceJepJemJE_em_num"+buffer.str(), hitmapVectsHAD[i], []( const auto &je ) { return (je.jelement->phi()); });
    variables.push_back(phiSliceJepJemJE_em);
    // had
    auto etaSliceJepJemJE_had = Monitored::Collection("etaSliceJepJemJE_had_num"+buffer.str(), hitmapVectsHAD[i], []( const auto &je ) { return (je.jelement->eta()); });
    variables.push_back(etaSliceJepJemJE_had);
    auto phiSliceJepJemJE_had = Monitored::Collection("phiSliceJepJemJE_had_num"+buffer.str(), hitmapVectsHAD[i], []( const auto &je ) { return (je.jelement->phi()); });
    variables.push_back(phiSliceJepJemJE_had);
  }
  
  // ===================
  // 1D MONITORING HISTS
  // ===================
  // EM 
  auto etJepJemJE_em = Monitored::Collection("etJepJemJE_em", monJepJemJEs_em, []( const auto &je ){ return je.jelement->emJetElementET(); });
  variables.push_back(etJepJemJE_em);
  auto phiJepJemJE_em = Monitored::Collection("phiJepJemJE_em", monJepJemJEs_em, []( const auto &je ){ return je.jelement->phi(); });
  variables.push_back(phiJepJemJE_em);
  auto etaJepJemJE_em = Monitored::Collection("etaJepJemJE_em", monJepJemJEs_em, []( const auto &je ){ return je.jelement->eta(); });
  variables.push_back(etaJepJemJE_em);
  // HAD
  auto etJepJemJE_had = Monitored::Collection("etJepJemJE_had", monJepJemJEs_had, []( const auto &je ){ return je.jelement->hadJetElementET(); });
  variables.push_back(etJepJemJE_had);
  auto phiJepJemJE_had = Monitored::Collection("phiJepJemJE_had", monJepJemJEs_had, []( const auto &je ){ return je.jelement->phi(); });
  variables.push_back(phiJepJemJE_had);
  auto etaJepJemJE_had = Monitored::Collection("etaJepJemJE_had", monJepJemJEs_had, []( const auto &je ){ return je.jelement->eta(); });
  variables.push_back(etaJepJemJE_had);

  fill(m_packageName,variables); 
  variables.clear();
  return StatusCode::SUCCESS;
}

StatusCode JepJemMonitorAlgorithm::fillJepJemJEVectors(
                SG::ReadHandle<xAOD::JetElementContainer> &jetElements,
						    std::vector<MonitorJepJemJE>              &monJepJemJEs_em,
                std::vector<MonitorJepJemJE>              &monJepJemJEs_had
						    ) const
{
  //   
  xAOD::JetElementContainer::const_iterator jeIterator = (*jetElements).begin();
  xAOD::JetElementContainer::const_iterator jeIteratorEnd = (*jetElements).end();
  for (; jeIterator != jeIteratorEnd; ++jeIterator) {
    const xAOD::JetElement* je = *jeIterator;
    const uint8_t em  = je->emJetElementET();
    const uint8_t had = je->hadJetElementET();
    const double eta = je->eta();
    const double phi = je->phi();
    // const double phi_scaled je->phi_scaled;

    // fill jet element vector for plots
    MonitorJepJemJE monJE;
    monJE.jelement=je;
    monJE.eta = eta;
    monJE.phi = phi;
    // monJE.phi_scaled = phi_scaled
    
    if (em) {
      monJepJemJEs_em.push_back(monJE);
    }
    if (had) {
      monJepJemJEs_had.push_back(monJE);
    }
  } // iterator

  return StatusCode::SUCCESS;
}
