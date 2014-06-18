/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// DeltaRTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: James Catmore (james.catmore@cern.ch)
//

#include "DerivationFrameworkTools/DeltaRTool.h"

#include <vector>
#include <string>

namespace DerivationFramework {

  DeltaRTool::DeltaRTool(const std::string& t,
      const std::string& n,
      const IInterface* p) :
    AthAlgTool(t,n,p),
    m_sgName(""),
    m_phi1BranchName(""),
    m_phi2BranchName(""),
    m_eta1BranchName(""),
    m_eta2BranchName("")
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("StoreGateEntryName", m_sgName);
    declareProperty("Phi1BranchName", m_phi1BranchName);
    declareProperty("Phi2BranchName", m_phi2BranchName);
    declareProperty("Eta1BranchName", m_eta1BranchName);
    declareProperty("Eta2BranchName", m_eta2BranchName);

  }

  StatusCode DeltaRTool::initialize()
  {
    if (m_sgName=="") {
      ATH_MSG_ERROR("No SG name provided for the output of delta R tool!");
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }

  StatusCode DeltaRTool::finalize()
  {
    return StatusCode::SUCCESS;
  }

  StatusCode DeltaRTool::addBranches() const
  {
    std::vector<float> *deltaRs = new std::vector<float>();
    CHECK(getDeltaRs(deltaRs));
    // Write deltaR of pairs to SG for access by downstream algs
    if (evtStore()->contains<std::vector<float> >(m_sgName)) {
      ATH_MSG_ERROR("Tool is attempting to write a StoreGate key " << m_sgName << " which already exists. Please use a different key");
      return StatusCode::FAILURE;
    }
    CHECK(evtStore()->record(deltaRs, m_sgName));
    return StatusCode::SUCCESS;
  }

  StatusCode DeltaRTool::getDeltaRs(std::vector<float>*& deltaRs) const
  {

    // check the relevant information is available
    if (m_phi1BranchName=="" || m_phi2BranchName=="" || m_eta1BranchName=="" || m_eta2BranchName=="") {
      ATH_MSG_ERROR("Phi/eta information missing!");
      return StatusCode::FAILURE;
    }

    // get the relevant branches
    std::vector<float>* phi1;
    std::vector<float>* phi2;
    std::vector<float>* eta1;
    std::vector<float>* eta2;
    CHECK(evtStore()->retrieve((const std::vector<float>*&)phi1,m_phi1BranchName));
    CHECK(evtStore()->retrieve((const std::vector<float>*&)phi2,m_phi2BranchName));
    CHECK(evtStore()->retrieve((const std::vector<float>*&)eta1,m_eta1BranchName));
    CHECK(evtStore()->retrieve((const std::vector<float>*&)eta2,m_eta2BranchName));
    if (phi1->size()!=eta1->size() || phi2->size()!=eta2->size() ) {
      ATH_MSG_ERROR("Branch sizes incompatible - returning zero");
      return StatusCode::FAILURE;
    }

    if (phi1->size()==0 || phi2->size()==0) return StatusCode::SUCCESS; // do nothing

    // Double loop to get the pairs for which the deltaR should be calculated
    unsigned int outerIt, innerIt;
    std::vector<std::vector<int> > pairs;
    for (outerIt=0; outerIt<phi1->size()-1; ++outerIt) {
      for (innerIt=0; innerIt<phi2->size()-1; ++innerIt) {
        float phi1f = (*phi1)[outerIt]; float phi2f = (*phi2)[innerIt];
        float eta1f = (*eta1)[outerIt]; float eta2f = (*eta2)[innerIt];
        float deltaR = calculateDeltaR(phi1f,phi2f,eta1f,eta2f);
        deltaRs->push_back(deltaR);
      }
    }

    return StatusCode::SUCCESS;

  }

  float DeltaRTool::calculateDeltaR(float phi1, float phi2, float eta1, float eta2) const
  {
    float deltaPhiSq = (phi1-phi2)*(phi1-phi2);
    float deltaEtaSq = (eta1-eta2)*(eta1-eta2);
    float deltaR = sqrt(deltaPhiSq+deltaEtaSq);
    return deltaR;
  }        

}
