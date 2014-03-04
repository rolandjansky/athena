/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// ExoticsTruthThinning.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: James Catmore (James.Catmore@cern.ch)

#include "DerivationFrameworkExotics/ExoticsTruthThinning.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::ExoticsTruthThinning::ExoticsTruthThinning( 	const std::string& t,
                     		                            		const std::string& n,
                                	                 		const IInterface* p ) : 
  AthAlgTool(t,n,p)
  {
    declareInterface<DerivationFramework::IThinningTool>(this);
  }
  
// Destructor
DerivationFramework::ExoticsTruthThinning::~ExoticsTruthThinning() {
}  

// Athena initialize and finalize
StatusCode DerivationFramework::ExoticsTruthThinning::initialize()
{
     ATH_MSG_VERBOSE("initialize() ...");
     return StatusCode::SUCCESS;
}
StatusCode DerivationFramework::ExoticsTruthThinning::finalize()
{
     ATH_MSG_VERBOSE("finalize() ...");
     return StatusCode::SUCCESS;
}

// The thinning itself
StatusCode DerivationFramework::ExoticsTruthThinning::doThinning() const
{

     // Get the relevant branches from StoreGate
     unsigned int mc_n;
     std::vector<float>* mc_pt; 
     CHECK(evtStore()->retrieve((const std::vector<float>*&)mc_pt,"mc_pt"));
     std::vector<float>* mc_m;                                                                                                           
     CHECK(evtStore()->retrieve((const std::vector<float>*&)mc_m,"mc_m"));
     std::vector<float>* mc_eta;                                                                                                          
     CHECK(evtStore()->retrieve((const std::vector<float>*&)mc_eta,"mc_eta"));
     std::vector<float>* mc_phi; 
     CHECK(evtStore()->retrieve((const std::vector<float>*&)mc_phi,"mc_phi"));
     std::vector<int> *mc_status;
     CHECK(evtStore()->retrieve((const std::vector<int>*&)mc_status,"mc_status"));
     std::vector<int> *mc_barcode; 
     CHECK(evtStore()->retrieve((const std::vector<int>*&)mc_barcode,"mc_barcode"));
     std::vector<int> *mc_pdgId;
     CHECK(evtStore()->retrieve((const std::vector<int>*&)mc_pdgId,"mc_pdgId"));
     std::vector<std::vector<int> >* mc_parents;
     CHECK(evtStore()->retrieve((const std::vector<std::vector<int> >*&)mc_parents,"mc_parents"));
     mc_n = mc_pt->size();

     // Decide which elements to keep
     std::vector<unsigned int> mask;
     for (unsigned int i = 0; i<mc_n; ++i) {
	if ( 	((*mc_status)[i]==1 && (*mc_barcode)[i]<200000 ) || 
		( 	((*mc_parents)[i].size()==2) &&
			( ( ((*mc_parents)[i])[0]==3 && ((*mc_parents)[i])[1]==4 ) || 
			  ( ((*mc_parents)[i])[1]==3 && ((*mc_parents)[i])[0]==4 )
			)
		) 
	) {mask.push_back(1);} else {mask.push_back(0);}
     }

     // Write the new vectors
     std::vector<float>* mc_pt_thin = new std::vector<float>;                                                                                                          
     std::vector<float>* mc_m_thin = new std::vector<float>;                                                                                                            
     std::vector<float>* mc_eta_thin = new std::vector<float>;                                                                                                          
     std::vector<float>* mc_phi_thin = new std::vector<float>;                                                                                                          
     std::vector<int>* mc_status_thin = new std::vector<int>;                                                                                                      
     std::vector<int>* mc_barcode_thin = new std::vector<int>;                                                                                                      
     std::vector<int>* mc_pdgId_thin = new std::vector<int>;                                                                                                       
     std::vector<std::vector<int> >* mc_parents_thin = new std::vector<std::vector<int> >;

     for (unsigned int i = 0; i<mc_n; ++i) {
	if (mask[i]==1) {
	   mc_pt_thin->push_back( (*mc_pt)[i] );
	   mc_m_thin->push_back( (*mc_m)[i] );	
	   mc_eta_thin->push_back( (*mc_eta)[i] );	
	   mc_phi_thin->push_back( (*mc_phi)[i] );
	   mc_status_thin->push_back( (*mc_status)[i] );
	   mc_barcode_thin->push_back( (*mc_barcode)[i] );
	   mc_pdgId_thin->push_back( (*mc_pdgId)[i] );
	   mc_parents_thin->push_back( (*mc_parents)[i] );
	}
     }

     return StatusCode::SUCCESS; 

}  
  
