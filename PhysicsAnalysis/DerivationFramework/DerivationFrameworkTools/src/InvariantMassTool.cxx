/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InvariantMassTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: James Catmore (james.catmore@cern.ch)
//

#include "DerivationFrameworkTools/InvariantMassTool.h"
#include "xAODBase/IParticleContainer.h"
#include "ExpressionEvaluation/ExpressionParser.h"
#include "ExpressionEvaluation/SGxAODProxyLoader.h"
#include "ExpressionEvaluation/MultipleProxyLoader.h"
#include "ExpressionEvaluation/SGNTUPProxyLoader.h"
#include <vector>
#include <string>

namespace DerivationFramework {

  InvariantMassTool::InvariantMassTool(const std::string& t,
      const std::string& n,
      const IInterface* p) : 
    AthAlgTool(t,n,p),
    m_expression("true"),
    m_parser(0),
    m_sgName(""),
    m_massHypothesis(0.0),
    m_containerName("")
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("ObjectRequirements", m_expression);
    declareProperty("StoreGateEntryName", m_sgName);
    declareProperty("MassHypothesis", m_massHypothesis);
    declareProperty("ContainerName", m_containerName);
  }

  StatusCode InvariantMassTool::initialize()
  {
    if (m_sgName=="") {
      ATH_MSG_ERROR("No SG name provided for the output of invariant mass tool!");
      return StatusCode::FAILURE;
    }

    ExpressionParsing::MultipleProxyLoader *proxyLoaders = new ExpressionParsing::MultipleProxyLoader();
    proxyLoaders->push_back(new ExpressionParsing::SGxAODProxyLoader(evtStore()));
    proxyLoaders->push_back(new ExpressionParsing::SGNTUPProxyLoader(evtStore()));
    m_parser = new ExpressionParsing::ExpressionParser(proxyLoaders);
    m_parser->loadExpression(m_expression);

    return StatusCode::SUCCESS;
  }

  StatusCode InvariantMassTool::finalize()
  {
    if (m_parser) {
      delete m_parser;
      m_parser = 0;
    }
    return StatusCode::SUCCESS;
  }

  StatusCode InvariantMassTool::addBranches() const
  {
    // Write masses to SG for access by downstream algs     
    if (evtStore()->contains<std::vector<float> >(m_sgName)) {
      ATH_MSG_ERROR("Tool is attempting to write a StoreGate key " << m_sgName << " which already exists. Please use a different key");
      return StatusCode::FAILURE;
    }
    std::vector<float> *masses = new std::vector<float>();
    CHECK(getInvariantMasses(masses));
    CHECK(evtStore()->record(masses, m_sgName));      
    return StatusCode::SUCCESS;
  }  

  StatusCode InvariantMassTool::getInvariantMasses(std::vector<float>*& masses) const
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
 
    // get the positions of the elements which pass the requirement
    std::vector<int> entries =  m_parser->evaluateAsVector();
    unsigned int nEntries = entries.size();

    // check the sizes are compatible
    if (particles->size() != nEntries ) { 
      ATH_MSG_WARNING("Branch sizes incompatible - returning zero");
      masses->push_back(0.0);
      return StatusCode::FAILURE;
    } 

    // Double loop to get the pairs for which the mass should be calculated	
    unsigned int outerIt, innerIt;
    std::vector<std::vector<int> > pairs;
    for (outerIt=0; outerIt<nEntries; ++outerIt) {
      for (innerIt=outerIt+1; innerIt<nEntries; ++innerIt) {
        std::vector<int> tmpPair;
        if (entries[outerIt]==1 && entries[innerIt]==1) {
          tmpPair.push_back(outerIt); tmpPair.push_back(innerIt);
          pairs.push_back(tmpPair);
        }
      }
    }

    // Loop over the pairs; calculate the mass; put into vector and return
    std::vector<std::vector<int> >::iterator pairIt;
    for (pairIt=pairs.begin(); pairIt!=pairs.end(); ++pairIt) {
      unsigned int first = (*pairIt)[0];
      unsigned int second = (*pairIt)[1];
      float px1 = ((*particles)[first])->p4().Px(); float px2 = ((*particles)[second])->p4().Px();
      float py1 = ((*particles)[first])->p4().Py(); float py2 = ((*particles)[second])->p4().Py();
      float pz1 = ((*particles)[first])->p4().Pz(); float pz2 = ((*particles)[second])->p4().Pz();
      float mass = calculateInvariantMass(px1,px2,py1,py2,pz1,pz2,m_massHypothesis);
      masses->push_back(mass);	
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

}
