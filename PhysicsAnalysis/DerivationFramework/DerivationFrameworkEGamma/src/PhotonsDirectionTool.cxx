/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// PhotonsDirectionTool.cxx, (c) ATLAS Detector software
// compute photon et = E(cluster)/cosh(eta of 2nd sampling)
// eventually E will be after recalibration
///////////////////////////////////////////////////////////////////
// Author: Giovanni Marchiori (giovanni.marchiori@cern.ch)
//

#include "DerivationFrameworkEGamma/PhotonsDirectionTool.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include <vector>
#include <string>

namespace DerivationFramework {
    
    PhotonsDirectionTool::PhotonsDirectionTool(const std::string& t,
					       const std::string& n,
					       const IInterface* p) :
    AthAlgTool(t,n,p),
    m_sgEta(""),
    m_sgPhi(""),
    m_sgEt(""),
    m_sgE(""),
    m_collName("Photons")
    {
        declareInterface<DerivationFramework::IAugmentationTool>(this);
	declareProperty("EtaSGEntry",m_sgEta);
	declareProperty("PhiSGEntry",m_sgPhi);
	declareProperty("EtSGEntry",m_sgEt);
	declareProperty("ESGEntry",m_sgE);
	declareProperty("PhotonContainer",m_collName);
    }
    
    StatusCode PhotonsDirectionTool::initialize()
    {
	if (m_sgEta=="" && 
	    m_sgPhi=="" && 
	    m_sgE=="" && 
	    m_sgEt=="" ) {
	  ATH_MSG_ERROR("You are requesting the PhotonsDirectionTool but have provided no SG names for the results");
	  return StatusCode::FAILURE;
	}
        return StatusCode::SUCCESS;
    }
    
    StatusCode PhotonsDirectionTool::finalize()
    {
        return StatusCode::SUCCESS;
    }
    
    StatusCode PhotonsDirectionTool::addBranches() const
    {
        // Retrieve photon container
	const xAOD::PhotonContainer* photons =  evtStore()->retrieve< const xAOD::PhotonContainer >(m_collName);
	if( !photons ) {
	  ATH_MSG_ERROR("Couldn't retrieve photon container with key: " << m_collName);
	  return StatusCode::FAILURE;
     	}

	// define the pointers which will hold the additional containers
	std::vector<float> *recEta(0);
	std::vector<float> *recPhi(0);
	std::vector<float> *recEt(0);
	std::vector<float> *recE(0);
	
	// check that key we want to write does not already exist in SG
	// if not, create the containers and register them to SG
	if (m_sgEta!="" && evtStore()->contains<std::vector<float> >(m_sgEta)) {
	  ATH_MSG_ERROR("Tool is attempting to write a StoreGate key " << m_sgEta << " which already exists. Please use a different key");
	  return StatusCode::FAILURE;
	}
	else{
	  recEta = new std::vector<float>();
	  CHECK(evtStore()->record(recEta, m_sgEta));
	}
	
        if (m_sgPhi!="" && evtStore()->contains<std::vector<float> >(m_sgPhi)) {
	  ATH_MSG_ERROR("Tool is attempting to write a StoreGate key " << m_sgPhi << " which already exists. Please use a different key");
	  return StatusCode::FAILURE;
	}
	else{
	  recPhi = new std::vector<float>();
	  CHECK(evtStore()->record(recPhi, m_sgPhi));
	}
	
	if (m_sgEt!="" && evtStore()->contains<std::vector<float> >(m_sgEt)) {
	  ATH_MSG_ERROR("Tool is attempting to write a StoreGate key " << m_sgEt << " which already exists. Please use a different key");
	  return StatusCode::FAILURE;
	}
	else{
	  recEt = new std::vector<float>();
	  CHECK(evtStore()->record(recEt, m_sgEt));
	}

	if (m_sgE!="" && evtStore()->contains<std::vector<float> >(m_sgE)) {
	  ATH_MSG_ERROR("Tool is attempting to write a StoreGate key " << m_sgE << " which already exists. Please use a different key");
	  return StatusCode::FAILURE;
	}
	else{
	  recE = new std::vector<float>();
	  CHECK(evtStore()->record(recE, m_sgE));
	}

        // LOOP OVER PHOTONS
	for (xAOD::PhotonContainer::const_iterator gIt = photons->begin(); gIt!=photons->end(); ++gIt) {           
 
            // Prepare variables
	    float eta(0.0), phi(0.0), e(0.0), et(0.0); 

	    if ( (*gIt)->nCaloClusters() > 0) {
	      const xAOD::CaloCluster* gCluster = (*gIt)->caloCluster(0);
	      eta = gCluster->etaBE(2);
	      phi = gCluster->phi();
	      e = gCluster->e();
	      et = e / cosh( eta );	    
	    }
	    else {
	      ATH_MSG_ERROR("Couldn't retrieve photon cluster, will use photon 4-momentum");
	      eta = (*gIt)->eta();
	      phi = (*gIt)->phi();
	      e = (*gIt)->e();
	      et = (*gIt)->pt();
	    }

	    if (m_sgEta!="") recEta->push_back(eta);
            if (m_sgPhi!="") recPhi->push_back(phi);
            if (m_sgEt!="") recEt->push_back(et);
            if (m_sgE!="") recE->push_back(e);       
        } // end of loop over photons
       
        // Write SG for access by downstream algs

        return StatusCode::SUCCESS;
        
    }
    
} // end of namespace
