/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//
//   @file    TrigBjetRoiBuilderMT.cxx        
//
//            Creates a ROI from a jet collection
//                   


#include "TrigBjetRoiBuilderMT.h"
#include "CxxUtils/phihelper.h"

//** ----------------------------------------------------------------------------------------------------------------- **//

TrigBjetRoiBuilderMT::TrigBjetRoiBuilderMT(const std::string & name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator) {}

//** ----------------------------------------------------------------------------------------------------------------- **//


StatusCode TrigBjetRoiBuilderMT::initialize() {

  ATH_MSG_DEBUG( "declareProperty review:"   );
  ATH_MSG_DEBUG( "    " << m_etaHalfWidth    );
  ATH_MSG_DEBUG( "    " << m_phiHalfWidth    );
  ATH_MSG_DEBUG( "    " << m_minJetEt        );
  ATH_MSG_DEBUG( "    " << m_maxJetEta       );
  ATH_MSG_DEBUG( "    " << m_nJetsMax        );
  ATH_MSG_DEBUG( "    " << m_dynamicMinJetEt );
  ATH_MSG_DEBUG( "    " << m_dynamicNJetsMax );
  ATH_MSG_DEBUG( "    " << m_dynamicEtFactor );

  ATH_MSG_DEBUG( "Initialising HandleKeys" );
  CHECK( m_jetInputKey.initialize()        );
  CHECK( m_roIOutputKey.initialize()  );  

  return StatusCode::SUCCESS;
}


//** ----------------------------------------------------------------------------------------------------------------- **//

StatusCode TrigBjetRoiBuilderMT::execute() {

  ATH_MSG_DEBUG( "Running "<< name() <<" ... " );
  const EventContext& ctx = getContext();

  // Sanity check that we're not running two optimisations at the same time
  // can be removed once a default method is decided upon
  if (m_nJetsMax > 0 && m_dynamicMinJetEt) {
    ATH_MSG_WARNING( "Two incompatible CPU optimisation methods chosen (DynamicMinJetEt and NJetsMax).  Switching off DynamicMinJetEt." );
    m_dynamicMinJetEt = false;
  }

  // Prepare Outputs
  std::unique_ptr< TrigRoiDescriptorCollection > roICollection( new TrigRoiDescriptorCollection() );

  // Retrieve Input Jets
  SG::ReadHandle< xAOD::JetContainer > jetContainerHandle = SG::makeHandle( m_jetInputKey,ctx );
  CHECK( jetContainerHandle.isValid() );
  const xAOD::JetContainer *jetContainer = jetContainerHandle.get();
  ATH_MSG_DEBUG( "Found " << jetContainer->size() << " jets, creating corresponding RoIs ... " );

  // Run on Input Jets
  int i = 0;
  for ( const xAOD::Jet *jet : *jetContainer ) {
    i++;

    float jetEt  = jet->p4().Et();
    float jetEta = jet->eta();
    float jetPhi = jet->phi();
    
    if (jetEt < m_minJetEt) {
      ATH_MSG_DEBUG( "Jet " << i << " below the " << m_minJetEt << " GeV threshold; Et " << jetEt << "; skipping this jet." );
      continue;
    }

    if (fabs(jetEta) > m_maxJetEta) {
      ATH_MSG_DEBUG( "Jet " << i << " outside the |eta| < " << m_maxJetEta << " requirement; Eta = " << jetEta << "; skipping this jet." );
      continue;
    }

    // For high pile-up situations, raise the pT threshold of the jets considered after checking the first N (=m_dynamicNJetsMax) jets
    if (m_dynamicMinJetEt && i > m_dynamicNJetsMax ) {
      float dynamicMinJetEt = m_minJetEt + ((i - m_dynamicNJetsMax) * m_dynamicEtFactor); 
      if (jetEt < dynamicMinJetEt) {
	ATH_MSG_DEBUG( "Jet "<< i << " below the dynamic " << dynamicMinJetEt << " GeV ( = " 
		       << m_minJetEt << " + (" << i << " - " << m_dynamicNJetsMax << ") * " << m_dynamicEtFactor << ")"
		       << " threshold; Et " << jetEt << "; skipping this jet." );
	continue;
      }    
    }

    if (m_nJetsMax > 0 && i > m_nJetsMax) {
      ATH_MSG_DEBUG( "Maximum allowed jet multiplicity = "<< m_nJetsMax << "; skipping jet " << i << "." );
      continue;
    }

    ATH_MSG_DEBUG( "Jet "<< i << "; Et " << jetEt << "; eta "<< jetEta << "; phi " << jetPhi );

    ATH_MSG_DEBUG( "Creating RoI corresponding to Jet" );
    // create RoI correspondinding to the jet
    double phiMinus = CxxUtils::wrapToPi(jetPhi-m_phiHalfWidth); 
    double phiPlus  = CxxUtils::wrapToPi(jetPhi+m_phiHalfWidth); 
    double etaMinus = jetEta-m_etaHalfWidth;  
    double etaPlus  = jetEta+m_etaHalfWidth;  

    TrigRoiDescriptor* roi =  new TrigRoiDescriptor(jetEta, etaMinus, etaPlus, 
						    jetPhi, phiMinus, phiPlus );

    ATH_MSG_DEBUG( "Adding ROI descriptor ROI collection !" );
    ATH_MSG_DEBUG( "    ** roi : eta=" << roi->eta() <<" phi=" << roi->phi() );
    roICollection->push_back( roi );
  }

  // Save Outputs 
  ATH_MSG_DEBUG( "Saving RoIs to be used as input to Fast Tracking -- TO BE CHANGED -- ::: " << m_roIOutputKey.key() );
  SG::WriteHandle< TrigRoiDescriptorCollection > outputRoiHandle = SG::makeHandle( m_roIOutputKey,ctx );
  CHECK( outputRoiHandle.record( std::move( roICollection ) ) );

  return StatusCode::SUCCESS;
}
