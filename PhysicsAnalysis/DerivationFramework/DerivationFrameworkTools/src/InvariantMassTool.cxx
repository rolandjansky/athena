/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InvariantMassTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: James Catmore (james.catmore@cern.ch)
//

#include "DerivationFrameworkTools/InvariantMassTool.h"
#include "xAODBase/IParticleContainer.h"
#include <vector>
#include <string>

namespace DerivationFramework {

  InvariantMassTool::InvariantMassTool(const std::string& t,
      const std::string& n,
      const IInterface* p) : 
    ExpressionParserUser<AthAlgTool,kInvariantMassToolParserNum>(t,n,p),
    m_expression("true"),
    m_expression2(""), 
    m_massHypothesis(0.0),
    m_massHypothesis2(0.0)
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("ObjectRequirements", m_expression);
    declareProperty("SecondObjectRequirements", m_expression2);
    declareProperty("MassHypothesis", m_massHypothesis);
    declareProperty("SecondMassHypothesis", m_massHypothesis2);
  }

  StatusCode InvariantMassTool::initialize()
  {
    if (m_sgName.key()=="") {
      ATH_MSG_ERROR("No SG name provided for the output of invariant mass tool!");
      return StatusCode::FAILURE;
    }
    ATH_CHECK(m_sgName.initialize());

    if (m_expression2.empty()) m_expression2 = m_expression;
    ATH_CHECK(initializeParser({m_expression,m_expression2} ) );

    ATH_CHECK(m_containerName.initialize());
    if (!m_containerName2.key().empty()) {
      ATH_CHECK(m_containerName2.initialize());
    }



    return StatusCode::SUCCESS;
  }

  StatusCode InvariantMassTool::finalize()
  {
    ATH_CHECK( finalizeParser());
    return StatusCode::SUCCESS;
  }

  StatusCode InvariantMassTool::addBranches() const
  {
    // Write masses to SG for access by downstream algs     
    if (evtStore()->contains<std::vector<float> >(m_sgName.key())) {
      ATH_MSG_ERROR("Tool is attempting to write a StoreGate key " << m_sgName << " which already exists. Please use a different key");
      return StatusCode::FAILURE;
    }
    std::unique_ptr<std::vector<float> > masses(new std::vector<float>());
    ATH_CHECK(getInvariantMasses(masses.get()));
    //CHECK(evtStore()->record(std::move(masses), m_sgName));      
    SG::WriteHandle<std::vector<float> > writeHandle(m_sgName);
    ATH_CHECK(writeHandle.record(std::move(masses)));
    return StatusCode::SUCCESS;
  }  

  StatusCode InvariantMassTool::getInvariantMasses(std::vector<float>* masses) const
  {

    // check the relevant information is available
    if (m_containerName.key().empty()) {
      ATH_MSG_WARNING("Input container missing - returning zero");  
      masses->push_back(0.0);
      return StatusCode::FAILURE;
    }

    SG::ReadHandle<xAOD::IParticleContainer> particles{m_containerName};
    
    bool from2Collections(false);
    const xAOD::IParticleContainer* particles2{nullptr}; 
    if (!m_containerName2.key().empty() && m_containerName2.key()!=m_containerName.key()) {
      SG::ReadHandle<xAOD::IParticleContainer> particleHdl2{m_containerName2};
      particles2=particleHdl2.cptr();
      from2Collections = true;
    }


    // get the positions of the elements which pass the requirement
    std::vector<int> entries =  m_parser[kInvariantMassToolParser1]->evaluateAsVector();
    std::vector<int> entries2 =  m_parser[kInvariantMassToolParser2]->evaluateAsVector();
    unsigned int nEntries = entries.size();
    unsigned int nEntries2 = entries2.size();

    // check the sizes are compatible
    if (!from2Collections) {
      if ( (particles->size() != nEntries) || (particles->size() != nEntries2) || (nEntries!=nEntries2) ) { 
        ATH_MSG_ERROR("Branch sizes incompatible - returning zero. Check your selection strings.");
        masses->push_back(0.0);
        return StatusCode::FAILURE;
      } 
    }
    if (from2Collections) {
      if ( (particles->size() != nEntries) || (particles2->size() != nEntries2) ) {
        ATH_MSG_ERROR("Branch sizes incompatible - returning zero. Check your selection strings.");
        masses->push_back(0.0);
        return StatusCode::FAILURE;
      }
    }    

    // Double loop to get all possible index pairs
    unsigned int outerIt, innerIt;
    std::vector<std::vector<int> > pairs;
    // Loop for case where both legs are from the same container
    if (!from2Collections) {
      for (outerIt=0; outerIt<nEntries; ++outerIt) {
        for (innerIt=outerIt+1; innerIt<nEntries; ++innerIt) {
          std::vector<int> tmpPair;
          tmpPair.push_back(outerIt); tmpPair.push_back(innerIt);
          pairs.push_back(tmpPair);
        }
      }    
      // Select the pairs for which the mass should be calculated, and then calculate it	
      std::vector<std::vector<int> >::iterator pairIt;
      for (pairIt=pairs.begin(); pairIt!=pairs.end(); ++pairIt) {
        unsigned int first = (*pairIt)[0];
        unsigned int second = (*pairIt)[1];    
        if ( (entries[first]==1 && entries2[second]==1) || (entries2[first]==1 && entries[second]==1) ) {
         
          const float mass = calculateInvariantMass( ((*particles)[first])->p4().Vect(),
                                                   ((*particles)[second])->p4().Vect(), 
                                                   m_massHypothesis,
                                                   m_massHypothesis);  
          masses->push_back(mass);
        }
      }  
    }
    
    // Loop for case where both legs are from different containers
    if (from2Collections) {
      for (outerIt=0; outerIt<nEntries; ++outerIt) {
        if (entries[outerIt]==0) continue;
        for (innerIt=0; innerIt<nEntries2; ++innerIt) {
          if (entries2[innerIt]==0) continue;
          std::vector<int> tmpPair;
          tmpPair.push_back(outerIt); tmpPair.push_back(innerIt);
          pairs.push_back(tmpPair);
        }
      }
      // Select the pairs for which the mass should be calculated, and then calculate it        
      std::vector<std::vector<int> >::iterator pairIt;
      for (pairIt=pairs.begin(); pairIt!=pairs.end(); ++pairIt) {
        unsigned int first = (*pairIt)[0];
        unsigned int second = (*pairIt)[1];
        const float mass = calculateInvariantMass( ((*particles)[first])->p4().Vect(),
                                                   ((*particles)[second])->p4().Vect(), 
                                                   m_massHypothesis,
                                                   m_massHypothesis2);  
        masses->push_back(mass);
      }
    } 

    return StatusCode::SUCCESS; 

  }
  float InvariantMassTool::calculateInvariantMass(const TVector3& v1, const TVector3&v2,float M1,float M2) const{
      TLorentzVector p1(v1, M1 > 0 ? std::hypot(M1, v1.Mag()) : v1.Mag());
      TLorentzVector p2(v2, M2 > 0 ? std::hypot(M2, v2.Mag()) : v2.Mag());
      return (p1+p2).M();
      
  }
  
}
