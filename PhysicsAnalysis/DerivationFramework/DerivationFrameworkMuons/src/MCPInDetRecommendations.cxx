/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// MCPInDetRecommendations.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: James Catmore (James.Catmore@cern.ch)
// Implements the MCP 2012 recommendations on associated inner detector 
// tracks
// Implemented for NTUP-to-NTUP

#include "DerivationFrameworkMuons/MCPInDetRecommendations.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::MCPInDetRecommendations::MCPInDetRecommendations( const std::string& t,
                                                 const std::string& n,
                                                 const IInterface* p ) : 
  AthAlgTool(t,n,p),
  m_pixAndDead(0),
  m_sctAndDead(4),
  m_pixAndSCTHoles(3),
  m_trtEtaMin(0.1),
  m_trtEtaMax(1.9),
  m_trtHitsPlusOutliers(5),
  m_trtOutliersFraction(0.9),
  m_sgKey(""),
  m_collName("muons")
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("PixelHitsPlusCrossedDeadPixel",m_pixAndDead);
    declareProperty("SCTHitsPlusCrossedDeadSCT",m_sctAndDead);
    declareProperty("PixelHolesPlusSCTHoles",m_pixAndSCTHoles);
    declareProperty("TRTSelectionEtaMin",m_trtEtaMin);
    declareProperty("TRTSelectionEtaMax",m_trtEtaMax);
    declareProperty("TRTHitsPlusOutliers",m_trtHitsPlusOutliers);
    declareProperty("TRTOutliersFraction",m_trtOutliersFraction); 
    declareProperty("StoreGateEntryName",m_sgKey);
    declareProperty("CollectionNTUPName", m_collName);
  }
  
// Destructor
DerivationFramework::MCPInDetRecommendations::~MCPInDetRecommendations() {
}  

// Athena initialize and finalize
StatusCode DerivationFramework::MCPInDetRecommendations::initialize()
{
     if (m_sgKey=="") {
	ATH_MSG_ERROR("No output SG name provided for the MCP inner detector selection tool!");
	return StatusCode::FAILURE;
     }
     ATH_MSG_VERBOSE("initialize() ...");
     return StatusCode::SUCCESS;
}
StatusCode DerivationFramework::MCPInDetRecommendations::finalize()
{
     ATH_MSG_VERBOSE("finalize() ...");
     return StatusCode::SUCCESS;
}

// Augmentation 
StatusCode DerivationFramework::MCPInDetRecommendations::addBranches() const
{

     // Retrieve data
     std::vector<int>* mu_nPixHits;
     std::vector<int>* mu_nPixelDeadSensors;
     std::vector<int>* mu_nSCTHits;
     std::vector<int>* mu_nSCTDeadSensors;
     std::vector<int>* mu_nPixHoles;
     std::vector<int>* mu_nSCTHoles;
     std::vector<int>* mu_nTRTHits;
     std::vector<int>* mu_nTRTOutliers;
     std::vector<float>* mu_eta;
     CHECK(retrieveFromStoreGate(	mu_nPixHits,
				mu_nPixelDeadSensors,
				mu_nSCTHits,
				mu_nSCTDeadSensors,
				mu_nPixHoles,
				mu_nSCTHoles,
				mu_nTRTHits,
				mu_nTRTOutliers,
				mu_eta ));

     // Loop over muons, count up and set decision
     unsigned int nMu = mu_eta->size(); 
     std::vector<int> *passMCP = new std::vector<int>();	
     for (unsigned int muItr=0; muItr < nMu; ++muItr) {
	if ( muonPassesSelection( 	(*mu_nPixHits)[muItr],
					(*mu_nPixelDeadSensors)[muItr],
					(*mu_nSCTHits)[muItr],
					(*mu_nSCTDeadSensors)[muItr],
					(*mu_nPixHoles)[muItr],
					(*mu_nSCTHoles)[muItr],
					(*mu_nTRTHits)[muItr],
					(*mu_nTRTOutliers)[muItr],
					(*mu_eta)[muItr] )
				 ) {passMCP->push_back(1);}
	else {passMCP->push_back(0);}
     }
     
     // Write decision to SG for access by downstream algs	
     if (evtStore()->contains<std::vector<int> >(m_sgKey)) {
	ATH_MSG_ERROR("Tool is attempting to write a StoreGate key " << m_sgKey << " which already exists. Please use a different key");
	return StatusCode::FAILURE;
     } 

     CHECK(evtStore()->record(passMCP, m_sgKey));	

     return StatusCode::SUCCESS;

}


bool DerivationFramework::MCPInDetRecommendations::muonPassesSelection(	int nPixHits, 
									int nPixelDeadSensors,
									int nSCTHits,
									int nSCTDeadSensors,
									int nPixHoles,
									int nSCTHoles,
									int nTRTHits,
									int nTRTOutliers,
									float muonEta) const {
	bool itemPassesHitSelection =  ((nPixHits+nPixelDeadSensors > m_pixAndDead) &&
					(nSCTHits+nSCTDeadSensors > m_sctAndDead) &&
					(nPixHoles+nSCTHoles < m_pixAndSCTHoles));

	bool itemPassesTRTCuts(false);
	int n = nTRTHits+nTRTOutliers;
	float outlierFraction = m_trtOutliersFraction * (float)n;
	if ( (fabs(muonEta)>m_trtEtaMin) && (fabs(muonEta)<m_trtEtaMax) ) {
		if ( (n>5) && (nTRTOutliers < outlierFraction) ) itemPassesTRTCuts=true;
	} else {itemPassesTRTCuts=true;}

	bool passesMCP = (itemPassesHitSelection && itemPassesTRTCuts); 
 	return passesMCP;

}

StatusCode DerivationFramework::MCPInDetRecommendations::retrieveFromStoreGate(	
										std::vector<int> *&mu_nPixHits,
										std::vector<int> *&mu_nPixDeadSensors,
										std::vector<int> *&mu_nSCTHits,
										std::vector<int> *&mu_nSCTDeadSensors,
										std::vector<int> *&mu_nPixHoles,
										std::vector<int> *&mu_nSCTHoles,
										std::vector<int> *&mu_nTRTHits,
										std::vector<int> *&mu_nTRTOutliers,
										std::vector<float> *&mu_eta)  const {

	// Build branch names
	// Gotcha: in NTUP_COMMON branch names go as:
	// mu_muid_blah for muid
	// mu_staco_blah for staco
	// mu_blah for 3rd chain
	std::string mu_nPixHitsName = "mu_"+m_collName+"nPixHits";
	std::string mu_nPixDeadSensorsName = "mu_"+m_collName+"nPixelDeadSensors";
	std::string mu_nSCTHitsName = "mu_"+m_collName+"nSCTHits";
   	std::string mu_nSCTDeadSensorsName = "mu_"+m_collName+"nSCTDeadSensors";
	std::string mu_nPixHolesName = "mu_"+m_collName+"nPixHoles";
	std::string mu_nSCTHolesName = "mu_"+m_collName+"nSCTHoles";
	std::string mu_nTRTHitsName = "mu_"+m_collName+"nTRTHits";
	std::string mu_nTRTOutliersName = "mu_"+m_collName+"nTRTOutliers";
	std::string mu_etaName = "mu_"+m_collName+"eta";

 	// Retrieves
        CHECK(evtStore()->retrieve((const std::vector<int>*&)mu_nPixHits,mu_nPixHitsName));
	CHECK(evtStore()->retrieve((const std::vector<int>*&)mu_nPixDeadSensors,mu_nPixDeadSensorsName));
	CHECK(evtStore()->retrieve((const std::vector<int>*&)mu_nSCTHits,mu_nSCTHitsName));
	CHECK(evtStore()->retrieve((const std::vector<int>*&)mu_nSCTDeadSensors,mu_nSCTDeadSensorsName));
	CHECK(evtStore()->retrieve((const std::vector<int>*&)mu_nPixHoles,mu_nPixHolesName));
	CHECK(evtStore()->retrieve((const std::vector<int>*&)mu_nSCTHoles,mu_nSCTHolesName));
	CHECK(evtStore()->retrieve((const std::vector<int>*&)mu_nTRTHits,mu_nTRTHitsName));
	CHECK(evtStore()->retrieve((const std::vector<int>*&)mu_nTRTOutliers,mu_nTRTOutliersName));
	CHECK(evtStore()->retrieve((const std::vector<float>*&)mu_eta,mu_etaName));
										
	return StatusCode::SUCCESS;

}	  
