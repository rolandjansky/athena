/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// vim: ts=2 sw=2
#include "TrigTauEmulation/Level1TopoSelectionTool.h"
#include "TrigTauEmulation/Utils.h"
#include "TrigTauEmulation/MsgStream.h"

const double DELTA_OVERLAP_J = round(100*((0.1)*(0.1)));
const double DELTA_OVERLAP_EMTAU = round(100*((0.0)*(0.0)));

Level1TopoSelectionTool::Level1TopoSelectionTool(const std::string& name) : Level1SelectionTool(name) {
  declareProperty("DeltaR", m_delta_r=2.8, "DeltaR matching requirement");
  declareProperty("DeltaPhi", m_delta_phi = 0.1, "DeltaPhi matching requirement");

  declareProperty("UseBox", m_use_box=false, "BOX instead of dR (default is dR)");
  declareProperty("BoxDeltaPhi", m_box_delta_phi=2.0, "BOX Delta Phi requirement");
  declareProperty("BoxDeltaEta", m_box_delta_eta=2.0, "BOX Delta Eta requirement");

  // m_topo_decisions = new std::map<std::string, bool>();
}

Level1TopoSelectionTool::Level1TopoSelectionTool(const Level1TopoSelectionTool& other) : Level1SelectionTool(other.name() + "_copy") {
  m_delta_r = other.m_delta_r;
  m_delta_phi = other.m_delta_phi;
  m_use_box = other.m_use_box;
  m_box_delta_phi = other.m_box_delta_phi;
  m_box_delta_eta = other.m_box_delta_eta;
}

StatusCode Level1TopoSelectionTool::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode Level1TopoSelectionTool::execute(const xAOD::EmTauRoIContainer * c1,
    const xAOD::EmTauRoIContainer * c2,
    const std::string & decor,
    const std::string & sel1,
    const std::string & sel2)
{
  // std::map<std::string, bool> blurp;
  // const std::string decor = "DR-" + sel1 + sel2;
  // blurp[decor] = false;
  m_topo_decisions[decor] = false;
  for(const auto p1 : *c1){
    p1->auxdecor<bool>(decor) = false;
    // Only consider selected objects
    if ( sel1 == "" || (sel1 != "" && p1->auxdataConst<bool>(sel1) == true) ) {
      for(const auto p2 : *c2){
        p2->auxdecor<bool>(decor) = false;
        // Only consider selected objects
        if ( sel2 == "" || (sel2 != "" && p2->auxdataConst<bool>(sel2) == true) ) {
          double delta_r = Utils::L1_DeltaR_Square(p1->eta(), p1->phi(), p2->eta(), p2->phi());
          bool is_topo = topological_criteria(p1->eta(), p1->phi(), p2->eta(), p2->phi(), m_use_box);
          if (is_topo && delta_r > DELTA_OVERLAP_EMTAU) {
            m_topo_decisions[decor] = true;
            p1->auxdecor<bool>(decor) = true;
            p2->auxdecor<bool>(decor) = true;
          }
        }
      }
    }
  }
  return StatusCode::SUCCESS;
}

// Tool execute
StatusCode Level1TopoSelectionTool::execute(const xAOD::EmTauRoIContainer * c1,
    const xAOD::JetRoIContainer * c2,
    const std::string & decor,
    const std::string & sel1,
    const std::string & sel2)
{
  // std::string decor = "DR-" + sel1 + sel2;
  m_topo_decisions[decor] = false;
  for(const auto p1 : *c1){
    p1->auxdecor<bool>(decor) = false;
    // Only consider selected objects
    if ( sel1 == "" || (sel1 != "" && p1->auxdataConst<bool>(sel1) == true) ) {
      for(const auto p2 : *c2){
        p2->auxdecor<bool>(decor) = false;
        // Only consider selected objects
        if ( sel2 == "" || (sel2 != "" && p2->auxdataConst<bool>(sel2) == true) ) {
          double delta_r = Utils::L1_DeltaR_Square(p1->eta(), p1->phi(), p2->eta(), p2->phi());
          bool is_topo = topological_criteria(p1->eta(), p1->phi(), p2->eta(), p2->phi(), m_use_box);
          if (is_topo && delta_r > DELTA_OVERLAP_J) {
            m_topo_decisions[decor] = true;
            p1->auxdecor<bool>(decor) = true;
            p2->auxdecor<bool>(decor) = true;
          }
        }
      }
    }
  }
  return StatusCode::SUCCESS;
}

// Tool execute
StatusCode Level1TopoSelectionTool::execute(const xAOD::MuonRoIContainer * c1,
    const xAOD::EmTauRoIContainer * c2,
    const std::string & decor,
    const std::string & sel1,
    const std::string & sel2)
{
  // std::string decor = "DR-" + sel1 + sel2;
  m_topo_decisions[decor] = false;
  for(const auto p1 : *c1){
    p1->auxdecor<bool>(decor) = false;
    // Only consider selected objects
    if ( sel1 == "" || (sel1 != "" && p1->auxdataConst<bool>(sel1) == true) ) {
      for(const auto p2 : *c2){
        p2->auxdecor<bool>(decor) = false;
        // Only consider selected objects
        if ( sel2 == "" || (sel2 != "" && p2->auxdataConst<bool>(sel2) == true) ) {
          bool is_topo = topological_criteria(p1->eta(), p1->phi(), p2->eta(), p2->phi(), m_use_box);
          if (is_topo) {
            m_topo_decisions[decor] = true;
            p1->auxdecor<bool>(decor) = true;
            p2->auxdecor<bool>(decor) = true;
          }
        }
      }
    }
  }
  return StatusCode::SUCCESS;
}

// Tool execute
StatusCode Level1TopoSelectionTool::execute(const xAOD::EmTauRoIContainer * c1,
    const xAOD::EnergySumRoI * met,
    const std::string & decor,
    const std::string & sel1,
    const std::string & sel2)
{
  // std::string decor = "DR-" + sel1 + sel2;
  m_topo_decisions[decor] = false;
  met->auxdecor<bool>(decor) = false;
  // Only consider selected objects
  if ( sel2 == "" || (sel2 != "" && met->auxdataConst<bool>(sel2) == true) ) {
    double met_phi = met->energyT() !=0 ? met->eyMiss() / met->energyT() : 0.;
    for(const auto p1 : *c1){
      p1->auxdecor<bool>(decor) = false;
      // Only consider selected objects
      if ( sel1 == "" || (sel1 != "" && p1->auxdataConst<bool>(sel1) == true) ) {
        double delta_phi = Utils::DeltaPhi(p1->phi(), met_phi);
        if (delta_phi < m_delta_phi) {
          m_topo_decisions[decor] = true;
          p1->auxdecor<bool>(decor) = true;
          met->auxdecor<bool>(decor) = true;
        }
      }
    }
  }
  return StatusCode::SUCCESS;
}

// Tool execute
StatusCode Level1TopoSelectionTool::execute(const xAOD::EmTauRoIContainer * c1,
    const xAOD::EmTauRoIContainer * c2,
    const xAOD::JetRoIContainer * c3,
    const std::string & decor,
    const std::string & sel1,
    const std::string & sel2,
    const std::string & sel3)
{
  // Topo criteria with an additional 
  // object (overlap removal)


  // std::string decor = "DR-" + sel1 + sel2 + "_" + sel3;
  m_topo_decisions[decor] = false;
  for (const auto p1 : *c1) {
    p1->auxdecor<bool>(decor) = false;
    // Only consider selected objects
    if (sel1 == "" || (sel1 != "" && p1->auxdataConst<bool>(sel1) == true)) {
      for (const auto p2 : *c2) {
        p2->auxdecor<bool>(decor) = false;
        // Only consider selected objects
        if (sel2 == "" || (sel2 != "" && p2->auxdataConst<bool>(sel2) == true)) {
          double delta_r = Utils::L1_DeltaR_Square(p1->eta(), p1->phi(), p2->eta(), p2->phi());
          bool is_topo = topological_criteria(p1->eta(), p1->phi(), p2->eta(), p2->phi(), m_use_box);
          if (is_topo && delta_r > DELTA_OVERLAP_EMTAU) {
            // Check if we have an additional jet that is not one of the 2 taus
            for (const auto p3 : *c3) {
              p3->auxdecor<bool>(decor) = false;
              // Only consider selected objects
              if (sel3 == "" || (sel3 != "" && p3->auxdataConst<bool>(sel3) == true)) {
                double delta_r_p1_p3 = Utils::L1_DeltaR_Square(p1->eta(), p1->phi(), p3->eta(), p3->phi());
                double delta_r_p2_p3 = Utils::L1_DeltaR_Square(p2->eta(), p2->phi(), p3->eta(), p3->phi());
                if (delta_r_p1_p3 > DELTA_OVERLAP_J && delta_r_p2_p3 > DELTA_OVERLAP_J) {
                  m_topo_decisions[decor] = true;
                  p1->auxdecor<bool>(decor) = true;
                  p2->auxdecor<bool>(decor) = true;
                  p3->auxdecor<bool>(decor) = true;
                }
              }
            }
          }
        }
      }
    }
  }
  return StatusCode::SUCCESS;
}

// Tool execute
StatusCode Level1TopoSelectionTool::execute(const xAOD::MuonRoIContainer * c1,
    const xAOD::EmTauRoIContainer * c2,
    const xAOD::JetRoIContainer * c3,
    const std::string & decor,
    const std::string & sel1,
    const std::string & sel2,
    const std::string & sel3)
{
  // Topo criteria with an additional 
  // object (overlap removal)


  // std::string decor = "DR-" + sel1 + sel2 + "_" + sel3;
  m_topo_decisions[decor] = false;
  for (const auto p1 : *c1) {
    p1->auxdecor<bool>(decor) = false;
    // Only consider selected objects
    if (sel1 == "" || (sel1 != "" && p1->auxdataConst<bool>(sel1) == true)) {
      for (const auto p2 : *c2) {
        p2->auxdecor<bool>(decor) = false;
        // Only consider selected objects
        if (sel2 == "" || (sel2 != "" && p2->auxdataConst<bool>(sel2) == true)) {
          bool is_topo_p1_p2 = topological_criteria(p1->eta(), p1->phi(), p2->eta(), p2->phi(), m_use_box);
          if (is_topo_p1_p2) {
            // Check if we have an additional jet that is not one of the 2 taus
            for (const auto p3 : *c3) {
              p3->auxdecor<bool>(decor) = false;
              // Only consider selected objects
              if (sel3 == "" || (sel3 != "" && p3->auxdataConst<bool>(sel3) == true)) {
                // Muon and Jet do not overlap so we don't need this check
                // double delta_r_p1_p3 = Utils::L1_DeltaR_Square(p1->eta(), p1->phi(), p3->eta(), p3->phi());
                double delta_r_p2_p3 = Utils::L1_DeltaR_Square(p2->eta(), p2->phi(), p3->eta(), p3->phi());
                if (/*delta_r_p1_p3 > DELTA_OVERLAP && */ delta_r_p2_p3 > DELTA_OVERLAP_J) {
                  m_topo_decisions[decor] = true;
                  p1->auxdecor<bool>(decor) = true;
                  p2->auxdecor<bool>(decor) = true;
                  p3->auxdecor<bool>(decor) = true;
                }
              }
            }
          }
        }
      }
    }
  }
  return StatusCode::SUCCESS;
}


bool Level1TopoSelectionTool::decision(const std::string & item)
{
  if (m_topo_decisions.find(item) != m_topo_decisions.end()) {
    return m_topo_decisions[item];
  } else {
    MY_MSG_ERROR(item<<" is not in the list of known topological requirements");
    return false;
  }
}   


bool Level1TopoSelectionTool::topological_criteria(const double eta1, const double phi1, const double eta2, const double phi2, const bool use_box)
{
  if (not use_box)
    return (Utils::L1_DeltaR_Square(eta1, phi1, eta2, phi2) < round(100*((m_delta_r*m_delta_r))));
  else 
    return ((Utils::L1_DeltaPhi(phi1, phi2) < round(10*m_box_delta_phi)) and (Utils::L1_DeltaEta(eta1, eta2) < round(10*m_box_delta_phi)));

}
