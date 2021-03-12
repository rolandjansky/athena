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
    m_sgName(""),
    m_massHypothesis(0.0),
    m_massHypothesis2(0.0),
    m_containerName(""),
    m_containerName2("")
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("ObjectRequirements", m_expression);
    declareProperty("SecondObjectRequirements", m_expression2);
    declareProperty("StoreGateEntryName", m_sgName);
    declareProperty("MassHypothesis", m_massHypothesis);
    declareProperty("SecondMassHypothesis", m_massHypothesis2);
    declareProperty("ContainerName", m_containerName);
    declareProperty("SecondContainerName", m_containerName2);
  }

  StatusCode InvariantMassTool::initialize()
  {
    if (m_sgName=="") {
      ATH_MSG_ERROR("No SG name provided for the output of invariant mass tool!");
      return StatusCode::FAILURE;
    }

    if (m_expression2.empty()) m_expression2 = m_expression;
    ATH_CHECK(initializeParser({m_expression,m_expression2} ) );
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
    if (evtStore()->contains<std::vector<float> >(m_sgName)) {
      ATH_MSG_ERROR("Tool is attempting to write a StoreGate key " << m_sgName << " which already exists. Please use a different key");
      return StatusCode::FAILURE;
    }
    std::unique_ptr<std::vector<float> > masses(new std::vector<float>());
    CHECK(getInvariantMasses(masses.get()));
    CHECK(evtStore()->record(std::move(masses), m_sgName));      
    return StatusCode::SUCCESS;
  }  

  StatusCode InvariantMassTool::getInvariantMasses(std::vector<float>* masses) const
  {

    // check the relevant information is available
    if (m_containerName=="") {
      ATH_MSG_WARNING("Input container missing - returning zero");  
      masses->push_back(0.0);
      return StatusCode::FAILURE;
    }

    // get the relevant branches
    const xAOD::IParticleContainer* particles = evtStore()->retrieve< const xAOD::IParticleContainer >( m_containerName );
    if( ! particles ) {
        ATH_MSG_ERROR ("Couldn't retrieve IParticles with key: " << m_containerName );
        return StatusCode::FAILURE;
    }
    
    bool from2Collections(false);
    const xAOD::IParticleContainer* particles2(0); 
    if (m_containerName2!="" && m_containerName2!=m_containerName) {
      particles2 = evtStore()->retrieve< const xAOD::IParticleContainer >( m_containerName2 );
      if( ! particles2 ) {
        ATH_MSG_ERROR ("Couldn't retrieve IParticles with key: " << m_containerName2 );
        return StatusCode::FAILURE;
      }
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
          float px1 = ((*particles)[first])->p4().Px(); float px2 = ((*particles)[second])->p4().Px();
          float py1 = ((*particles)[first])->p4().Py(); float py2 = ((*particles)[second])->p4().Py();
          float pz1 = ((*particles)[first])->p4().Pz(); float pz2 = ((*particles)[second])->p4().Pz();
          float mass = calculateInvariantMass(px1,px2,py1,py2,pz1,pz2,m_massHypothesis);
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
        float px1 = ((*particles)[first])->p4().Px(); float px2 = ((*particles2)[second])->p4().Px();
        float py1 = ((*particles)[first])->p4().Py(); float py2 = ((*particles2)[second])->p4().Py();
        float pz1 = ((*particles)[first])->p4().Pz(); float pz2 = ((*particles2)[second])->p4().Pz();
        float mass = calculateInvariantMass(px1,px2,py1,py2,pz1,pz2,m_massHypothesis,m_massHypothesis2);
        masses->push_back(mass);
      }
    } 

    return StatusCode::SUCCESS; 

  }

  float InvariantMassTool::calculateInvariantMass(float px1, float px2, float py1, float py2, float pz1, float pz2, float massH) const 
  {
    float e1 = sqrt(px1*px1 + py1*py1 + pz1*pz1 + massH*massH); 
    float e2 = sqrt(px2*px2 + py2*py2 + pz2*pz2 + massH*massH);
    float eSum = e1+e2;
    float pxSum = px1+px2;
    float pySum = py1+py2;
    float pzSum = pz1+pz2;
    float invariantMass = sqrt( (eSum*eSum)-(pxSum*pxSum)-(pySum*pySum)-(pzSum*pzSum) );   	   
    return invariantMass;
  }        

  float InvariantMassTool::calculateInvariantMass(float px1, float px2, float py1, float py2, float pz1, float pz2, float massH, float massH2) const
  {
    float e1 = sqrt(px1*px1 + py1*py1 + pz1*pz1 + massH*massH);
    float e2 = sqrt(px2*px2 + py2*py2 + pz2*pz2 + massH2*massH2);
    float eSum = e1+e2;
    float pxSum = px1+px2;
    float pySum = py1+py2;
    float pzSum = pz1+pz2;
    float invariantMass = sqrt( (eSum*eSum)-(pxSum*pxSum)-(pySum*pySum)-(pzSum*pzSum) );
    return invariantMass;
  }
}
