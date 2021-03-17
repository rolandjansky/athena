/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DeltaRTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: James Catmore (james.catmore@cern.ch)
//

#include "DerivationFrameworkTools/DeltaRTool.h"
#include "xAODBase/IParticleContainer.h"
#include <vector>
#include <string>

namespace DerivationFramework {

  DeltaRTool::DeltaRTool(const std::string& t,
      const std::string& n,
      const IInterface* p) : 
    ExpressionParserUser<AthAlgTool,kDeltaRToolParserNum>(t,n,p),
    m_expression(""),
    m_2ndExpression("")
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("ObjectRequirements", m_expression);
    declareProperty("SecondObjectRequirements", m_2ndExpression); 
  }
  
  StatusCode DeltaRTool::initialize()
  {
    if (m_sgName.key()=="") {
      ATH_MSG_ERROR("No SG name provided for the output of invariant mass tool!");
      return StatusCode::FAILURE;
    }
    ATH_CHECK(m_sgName.initialize());
    ATH_CHECK(m_containerName.initialize());

    if (!m_containerName2.key().empty()) {
      ATH_CHECK(m_containerName2.initialize());
    }

    ATH_CHECK(initializeParser( {m_expression, m_2ndExpression} ));
    return StatusCode::SUCCESS;
  }

  StatusCode DeltaRTool::finalize()
  {
    ATH_CHECK(finalizeParser());
    return StatusCode::SUCCESS;
  }

  StatusCode DeltaRTool::addBranches() const
  {
    // Write deltaRs to SG for access by downstream algs     
    if (evtStore()->contains<std::vector<float> >(m_sgName.key())) {
      ATH_MSG_ERROR("Tool is attempting to write a StoreGate key " << m_sgName << " which already exists. Please use a different key");
      return StatusCode::FAILURE;
    }
    std::unique_ptr<std::vector<float> > deltaRs(new std::vector<float>());
    ATH_CHECK(getDeltaRs(deltaRs.get()));

    SG::WriteHandle<std::vector<float> > writeHandle(m_sgName);
    ATH_CHECK(writeHandle.record(std::move(deltaRs)));

    return StatusCode::SUCCESS;
  }  

  StatusCode DeltaRTool::getDeltaRs(std::vector<float>* deltaRs) const
  {

    // check the relevant information is available
    if (m_containerName.key()=="") {
      ATH_MSG_WARNING("Input container missing - returning zero");  
      deltaRs->push_back(0.0);
      return StatusCode::FAILURE;
    }
    bool secondContainer(false);
    if (m_containerName2.key()!="") secondContainer=true;

    // get the relevant branches
    SG::ReadHandle<xAOD::IParticleContainer> particles{m_containerName};

    const xAOD::IParticleContainer* secondParticles(NULL);
    if (secondContainer) {
       SG::ReadHandle<xAOD::IParticleContainer> particleHdl2{m_containerName2};
       secondParticles=particleHdl2.cptr();
    }

    // get the positions of the elements which pass the requirement
    std::vector<int> entries, entries2;
    if (!m_expression.empty()) {entries = m_parser[kDeltaRToolParser1]->evaluateAsVector();}
    else {entries.assign(particles->size(),1);} // default: include all elements 
    unsigned int nEntries = entries.size();
    // check the sizes are compatible
    if (particles->size() != nEntries ) {
      ATH_MSG_FATAL("Branch sizes incompatible");
      return StatusCode::FAILURE;
    }
    unsigned int nEntries2(0);
    if (secondContainer) {
        if (!m_2ndExpression.empty()) {entries2 =  m_parser[kDeltaRToolParser2]->evaluateAsVector();}
	else {entries2.assign(secondParticles->size(),1);} // default: include all elements
	nEntries2 = entries2.size();
	// check the sizes are compatible
	if (secondParticles->size() != nEntries2 ) {
           ATH_MSG_FATAL("Branch sizes incompatible - returning zero");
           return StatusCode::FAILURE;
        }
    }

    // Double loop to get the pairs for which the mass should be calculated	
    std::vector<std::vector<int> > pairs;
    if (!secondContainer) {
      unsigned int outerIt, innerIt;
      for (outerIt=0; outerIt<nEntries; ++outerIt) {
        for (innerIt=outerIt+1; innerIt<nEntries; ++innerIt) {
          std::vector<int> tmpPair;
          if (entries[outerIt]==1 && entries[innerIt]==1) {
            tmpPair.push_back(outerIt); tmpPair.push_back(innerIt);
            pairs.push_back(tmpPair);
          }
        }
      }
    }

    if (secondContainer) {
      unsigned int coll1It, coll2It;
      for (coll1It=0; coll1It<nEntries; ++coll1It) {
        for (coll2It=0; coll2It<nEntries2; ++coll2It) {
          std::vector<int> tmpPair;
          if (entries[coll1It]==1 && entries2[coll2It]==1) {
            tmpPair.push_back(coll1It); tmpPair.push_back(coll2It);
            pairs.push_back(tmpPair);
          }
        }
      }
    }

    // Loop over the pairs; calculate the mass; put into vector and return
    std::vector<std::vector<int> >::iterator pairIt;
    for (pairIt=pairs.begin(); pairIt!=pairs.end(); ++pairIt) {
      unsigned int first = (*pairIt)[0];
      unsigned int second = (*pairIt)[1];
      if (!secondContainer) {
	float phi1f = ((*particles)[first])->p4().Phi(); float phi2f = ((*particles)[second])->p4().Phi();
        float eta1f = ((*particles)[first])->p4().Eta(); float eta2f = ((*particles)[second])->p4().Eta();
        float deltaR = calculateDeltaR(phi1f,phi2f,eta1f,eta2f);
        deltaRs->push_back(deltaR);
      }
      if (secondContainer) {
        float phi1f = ((*particles)[first])->p4().Phi(); float phi2f = ((*secondParticles)[second])->p4().Phi();
        float eta1f = ((*particles)[first])->p4().Eta(); float eta2f = ((*secondParticles)[second])->p4().Eta();
        float deltaR = calculateDeltaR(phi1f,phi2f,eta1f,eta2f);
        deltaRs->push_back(deltaR);
      }	
    }

    return StatusCode::SUCCESS; 

  }
  
  float DeltaRTool::calculateDeltaR(float phi1, float phi2, float eta1, float eta2) const
  {
    float deltaPhi = fabs(phi1-phi2);
    if (deltaPhi>TMath::Pi()) deltaPhi = 2.0*TMath::Pi() - deltaPhi;
    float deltaPhiSq = deltaPhi*deltaPhi;
    float deltaEtaSq = (eta1-eta2)*(eta1-eta2);
    float deltaR = sqrt(deltaPhiSq+deltaEtaSq);
    return deltaR;
  }
}
