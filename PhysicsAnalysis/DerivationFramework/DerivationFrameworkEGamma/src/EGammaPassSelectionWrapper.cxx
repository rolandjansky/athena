/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EGammaPassSelectionWrapper.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: James Catmore (James.Catmore@cern.ch)
// Wrapper around the passSelection() method of xAOD egamma
// Writes result to SG for later selection by string parser

#include "DerivationFrameworkEGamma/EGammaPassSelectionWrapper.h"
#include "xAODEgamma/EgammaContainer.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::EGammaPassSelectionWrapper::EGammaPassSelectionWrapper( const std::string& t,
									     const std::string& n,
									     const IInterface* p ) :
  AthAlgTool(t,n,p),
  m_collName("Electrons"),
  m_sgPrefix("")
{
  declareInterface<DerivationFramework::IAugmentationTool>(this);
  declareProperty("SelectionVariables",m_qualFlags);
  declareProperty("CollectionName", m_collName);
  declareProperty("SGPrefix", m_sgPrefix);
}
 
// Destructor
DerivationFramework::EGammaPassSelectionWrapper::~EGammaPassSelectionWrapper() {
} 

// Athena initialize and finalize
StatusCode DerivationFramework::EGammaPassSelectionWrapper::initialize()
{
  if (m_qualFlags.size()==0) {
    ATH_MSG_ERROR("No selection variables for the egamma passSelection wrapper tool!");
    return StatusCode::FAILURE;
  }
  ATH_MSG_VERBOSE("initialize() ...");
  return StatusCode::SUCCESS;
}
StatusCode DerivationFramework::EGammaPassSelectionWrapper::finalize()
{
  ATH_MSG_VERBOSE("finalize() ...");
  return StatusCode::SUCCESS;
}

// Augmentation
StatusCode DerivationFramework::EGammaPassSelectionWrapper::addBranches() const
{

  // Retrieve data
  const xAOD::EgammaContainer* egammas =  evtStore()->retrieve< const xAOD::EgammaContainer >( m_collName );
  if( ! egammas ) {
    ATH_MSG_ERROR("Couldn't retrieve e-gamma container with key: " << m_collName);
    return StatusCode::FAILURE;
  }
	
  // Make vectors for the cut results
  std::vector<std::vector<int>* > allSelectionResults;
  for (std::vector<std::string>::const_iterator strItr = m_qualFlags.begin(); strItr!=m_qualFlags.end(); ++strItr) {
    std::vector<int> *passEgamma = new std::vector<int>();  
    allSelectionResults.push_back(passEgamma);
  }  
  // Loop over egammas, set decisions   
  for (xAOD::EgammaContainer::const_iterator eIt = egammas->begin(); eIt!=egammas->end(); ++eIt) {
    unsigned int itr(0);
    for (std::vector<std::string>::const_iterator strItr = m_qualFlags.begin(); strItr!=m_qualFlags.end(); ++strItr, ++itr) {
      bool val(0);
      if ( (*eIt)->passSelection(val,*strItr) ) {
	if (val) {allSelectionResults[itr]->push_back(1);}
	else {allSelectionResults[itr]->push_back(0);}
      } else {
	ATH_MSG_WARNING("Evident problem with quality flag " << *strItr << " so setting to false!");
	allSelectionResults[itr]->push_back(0); 
      }
    }
  }     

  // Write decision to SG for access by downstream algs 
  unsigned int itr(0);
  for (std::vector<std::string>::const_iterator strItr = m_qualFlags.begin(); strItr!=m_qualFlags.end(); ++strItr, ++itr) {
    std::string sgKey("");
    if (m_sgPrefix=="") {
      sgKey = *strItr;
    } else {
      sgKey = m_sgPrefix+*strItr;
    }   
    if (evtStore()->contains<std::vector<int> >(sgKey)) {
      ATH_MSG_ERROR("Tool is attempting to write a StoreGate key " << sgKey << " which already exists. Please use a different key");
      return StatusCode::FAILURE;
    }
    CHECK(evtStore()->record(allSelectionResults[itr],sgKey));       
  }
  return StatusCode::SUCCESS;
}

