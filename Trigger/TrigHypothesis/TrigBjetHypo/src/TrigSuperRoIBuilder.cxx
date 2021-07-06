/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//
#include "TrigSuperRoIBuilder.h"
#include "CxxUtils/phihelper.h"

//** ----------------------------------------------------------------------------------------------------------------- **//

TrigSuperRoIBuilder::TrigSuperRoIBuilder(const std::string & name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator) {}

//** ----------------------------------------------------------------------------------------------------------------- **//


StatusCode TrigSuperRoIBuilder::initialize() {

  ATH_MSG_DEBUG( "declareProperty review:"   );
  ATH_MSG_DEBUG( "    " << m_etaHalfWidth    );
  ATH_MSG_DEBUG( "    " << m_phiHalfWidth    );
  ATH_MSG_DEBUG( "    " << m_minJetEt        );
  ATH_MSG_DEBUG( "    " << m_maxJetEta       );

  ATH_MSG_DEBUG( "Initialising HandleKeys" );
  CHECK( m_jetInputKey.initialize()        );
  CHECK( m_roIOutputKey.initialize()  );  

  return StatusCode::SUCCESS;
}


//** ----------------------------------------------------------------------------------------------------------------- **//

StatusCode TrigSuperRoIBuilder::execute() {

  ATH_MSG_DEBUG( "Running "<< name() <<" ... " );
  const EventContext& ctx = getContext();

  // ==============================================================================================================================
  //    ** Retrieve Inputs
  // ==============================================================================================================================

  SG::ReadHandle< xAOD::JetContainer > jetContainerHandle = SG::makeHandle( m_jetInputKey,ctx );
  CHECK( jetContainerHandle.isValid() );
  const xAOD::JetContainer *jetContainer = jetContainerHandle.get();
  ATH_MSG_DEBUG( "Found " << jetContainer->size() << " jets, creating corresponding RoIs ... " );

  // ==============================================================================================================================
  //    ** Prepare Outputs
  // ==============================================================================================================================

  std::unique_ptr< TrigRoiDescriptorCollection > roICollection = std::make_unique< TrigRoiDescriptorCollection >();

  // ==============================================================================================================================
  //    ** Perform the computation
  // ==============================================================================================================================

  // Create Super-RoI
  TrigRoiDescriptor* superRoi = new TrigRoiDescriptor();
  superRoi->setComposite( true );
  
  // Run on Input Jets
  for ( const xAOD::Jet *jet : *jetContainer ) {
    float jetEt  = jet->p4().Et();
    float jetEta = jet->eta();
    float jetPhi = jet->phi();
    
    ATH_MSG_DEBUG( "Jet  Et " << jetEt << "; eta "<< jetEta << "; phi " << jetPhi );
    
    if (jetEt < m_minJetEt) {
      ATH_MSG_DEBUG( "Jet below the " << m_minJetEt << " GeV threshold; Et " << jetEt << "; skipping this jet." );
      continue;
    }
    
    if (fabs(jetEta) > m_maxJetEta) {
      ATH_MSG_DEBUG( "Jet outside the |eta| < " << m_maxJetEta << " requirement; Eta = " << jetEta << "; skipping this jet." );
      continue;
    }
    
    ATH_MSG_DEBUG( "    ** Creating RoI corresponding to Jet" );
    double phiMinus = CxxUtils::wrapToPi(jetPhi-m_phiHalfWidth); 
    double phiPlus  = CxxUtils::wrapToPi(jetPhi+m_phiHalfWidth); 
    double etaMinus = jetEta - m_etaHalfWidth;  
    double etaPlus  = jetEta + m_etaHalfWidth;  
    
    TrigRoiDescriptor* roi =  new TrigRoiDescriptor( jetEta, etaMinus, etaPlus, 
						     jetPhi, phiMinus, phiPlus );
    
    ATH_MSG_DEBUG( "    ** Adding ROI descriptor ROI collection !" );
    ATH_MSG_DEBUG( "         " << (*roi) );

    //    roICollection->push_back( roi ); // TMP
    superRoi->push_back( roi );
  }
  
  ATH_MSG_DEBUG( "Super RoI for fast tracking" );
  ATH_MSG_DEBUG( *superRoi );
  roICollection->push_back( superRoi ); // TMP
  
  // ==============================================================================================================================
  //    ** Store the outputs
  // ==============================================================================================================================
  
  ATH_MSG_DEBUG( "Saving Super RoI to be used as input to Fast Tracking as '" << m_roIOutputKey.key() << "' with Size: " << roICollection->size() );
  SG::WriteHandle< TrigRoiDescriptorCollection > outputRoiHandle = SG::makeHandle( m_roIOutputKey,ctx );
  CHECK( outputRoiHandle.record( std::move( roICollection ) ) );

  return StatusCode::SUCCESS;
}



