/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigGSCFexMT.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypo
//
// ************************************************

#include "src/TrigGSCFexMT.h"

// ONLINE INFRASTRUCTURE
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigOperationalInfo.h" // TO BE REMOVED

// BEAMSPOT
#include "InDetBeamSpotService/IBeamCondSvc.h"

// EDM

#include "xAODBase/IParticle.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"

#include "xAODTracking/TrackParticleContainer.h"

#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "TrigSteeringEvent/PhiHelper.h"

// ----------------------------------------------------------------------------------------------------------------- 


TrigGSCFexMT::TrigGSCFexMT(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator) {}

// ----------------------------------------------------------------------------------------------------------------- 

TrigGSCFexMT::~TrigGSCFexMT() {}

// ----------------------------------------------------------------------------------------------------------------- 

StatusCode TrigGSCFexMT::initialize() {

  // Get message service 
  ATH_MSG_INFO( "Initializing TrigGSCFexMT, version " << PACKAGE_VERSION );

  // declareProperty overview 
  ATH_MSG_DEBUG( "declareProperty review:"                );
  ATH_MSG_DEBUG( "   "     << m_JetContainerKey           );
  ATH_MSG_DEBUG( "   "     << m_VertexContainerKey        );
  ATH_MSG_DEBUG( "   "     << m_TrackParticleContainerKey );
  ATH_MSG_DEBUG( "   "     << m_jetOutputKey              );

  ATH_MSG_DEBUG( "Initializing ReadHandleKeys"        );
  ATH_CHECK( m_roiContainerKey.initialize()           );
  ATH_CHECK( m_JetContainerKey.initialize()           );
  ATH_CHECK( m_VertexContainerKey.initialize()        );
  ATH_CHECK( m_TrackParticleContainerKey.initialize() );
  ATH_CHECK( m_jetOutputKey.initialize()              );

  ATH_MSG_DEBUG( "Retrieving Tools"        );
  ATH_CHECK( m_jetGSCCalib_tool.retrieve() );

  return StatusCode::SUCCESS;
}


// ----------------------------------------------------------------------------------------------------------------- 


StatusCode TrigGSCFexMT::execute() {
  ATH_MSG_DEBUG( "Executing TrigGSCFexMT" );

  // ==============================================================================================================================
  //    ** Retrieve Ingredients
  // ==============================================================================================================================

  const EventContext& ctx = getContext();
  /*
  // RoIs
  ATH_MSG_DEBUG( "Ready to retrieve rois : " << m_roiContainerKey.key() );
  auto roiContainerHandle = SG::makeHandle( m_roiContainerKey,ctx );
  CHECK( roiContainerHandle.isValid() );

  auto roiContainer = roiContainerHandle.get();
  ATH_MSG_DEBUG( "Retrieved " << roiContainer->size() <<" input RoIs" );
  for ( auto el : *roiContainer ) {
    ATH_MSG_DEBUG( "   ** RoI : eta=" << el->eta() <<" phi=" << el->phi() );
  }
  */
  // Jets
  ATH_MSG_DEBUG( "Ready to retrieve jets : " << m_JetContainerKey.key() );
  SG::ReadHandle< xAOD::JetContainer > jetContainerHandle = SG::makeHandle( m_JetContainerKey,ctx );
  CHECK( jetContainerHandle.isValid() );

  const xAOD::JetContainer *jetContainer = jetContainerHandle.get();
  ATH_MSG_DEBUG( "Retrieved " << jetContainer->size() << " input Jets for GSC correction : " << m_JetContainerKey );
  for ( const xAOD::Jet *jet : *jetContainer )
    ATH_MSG_DEBUG( "  ** Jet pt=" << jet->p4().Et() <<" eta="<< jet->eta()<< " phi="<< jet->phi() );

  //  SG::ReadHandle< xAOD::VertexContainer > prmVtxContainerHandle = SG::makeHandle( m_VertexContainerKey,ctx );
  //  SG::ReadHandle< xAOD::TrackParticleContainer > trkParticlesHandle = SG::makeHandle( m_TrackParticleContainerKey,ctx );


  // ==============================================================================================================================
  //    ** Prepare Output
  // ==============================================================================================================================

  std::unique_ptr< xAOD::JetContainer > calibrateJets( new xAOD::JetContainer() );
  std::unique_ptr< xAOD::JetAuxContainer > calibratedJetsAux( new xAOD::JetAuxContainer() );
  calibrateJets->setStore( calibratedJetsAux.get() );

  // ==============================================================================================================================
  //    ** Calibrate Jets
  // ==============================================================================================================================

  /*
  unsigned int nTrk(0);
  double       width(0);
  double       ptsum(0);

  //  xAOD::TrackParticleContainer::const_iterator trkParticlesIter = trkParticlesHandle->begin();
  for ( const xAOD::TrackParticle* trk : *trkParticlesHandle ) {

    //std::cout << "\tTrigGSCFex: track"
    //	      << " pt: "  << trk->p4().Pt()
    //	      << " eta: " << trk->p4().Eta()
    //	      << " phi: " << trk->p4().Phi()
    //	      << " m: "   << trk->p4().M()
    //	      << std::endl;
    
    //
    // trkIsGood() defined below
    //
    if( trkIsGood(trk) && 
	fabs( (trk->z0() + trk->vz() - primaryVertex->z()) * sin(trk->theta()) ) <= 1.0 ){

      float dEta = (trk->eta() - jet.p4().Eta());
      float dPhi =  HLT::deltaPhi(trk->phi(), jet.p4().Phi());
      width += trk->pt() * sqrt( dEta*dEta + dPhi*dPhi);
      ptsum += trk->pt();

      ++nTrk;
    }

  }

  if ( ptsum > 0 ) width = width / ptsum;
  else width = -1.;

  //
  // Set moments in the jet
  //
  std::vector<float> widthVector;
  widthVector.push_back(width);
  jet.setAttribute("TrackWidthPt1000", widthVector);

  std::vector<int> ntrkVector;
  ntrkVector.push_back(nTrk);
  jet.setAttribute("NumTrkPt1000", ntrkVector);  

  //
  // Muon segments must also be set for calibration tool to work
  // fill w/ 0 as a dummy
  int gmsc = 0;
  jet.setAttribute("GhostMuonSegmentCount", gmsc);


  // EXECUTE OFFLINE TOOLS

  // calJet is a pointer to the new, calibrated jet
  xAOD::Jet* calJet = nullptr;
  m_jetGSCCalib_tool->calibratedCopy(jet,calJet);
  // Now we have a new fully calibrated jet!

  //std::cout << "TrigGSCFex: New jet"
  //	    << " pt: "  << calJet->p4().Pt()
  //	    << " eta: " << calJet->p4().Eta()
  //	    << " phi: " << calJet->p4().Phi()
  //	    << " m: "   << calJet->p4().M()
  //	    << std::endl;

  */
  ATH_MSG_DEBUG( "Ready to perform calibration" );
  for ( const xAOD::Jet *inJet : *jetContainer ) {
    /*
    xAOD::Jet *outJet = nullptr;
    m_jetGSCCalib_tool->calibratedCopy( inJet,outJet ); 
    */
    xAOD::Jet *outJet = new xAOD::Jet();
    calibrateJets->push_back( outJet );
    *outJet = *inJet;
  }
  ATH_MSG_DEBUG( "  ** Calibration performed" );
  // ==============================================================================================================================
  //    ** Store Output
  // ==============================================================================================================================

  ATH_MSG_DEBUG( "Saving Output" );
  SG::WriteHandle< xAOD::JetContainer > outputJetContainerHandle = SG::makeHandle( m_jetOutputKey,ctx );
  ATH_CHECK( outputJetContainerHandle.record( std::move(calibrateJets),std::move(calibratedJetsAux) ) ); 

  return StatusCode::SUCCESS;
}


// ----------------------------------------------------------------------------------------------------------------- 

StatusCode TrigGSCFexMT::finalize() {
  return StatusCode::SUCCESS;
}


bool TrigGSCFexMT::trkIsGood(const xAOD::TrackParticle *trk){

  uint8_t numberOfPixelHits=0;
  if( ! trk->summaryValue (numberOfPixelHits, xAOD::SummaryType::numberOfPixelHits) )
    std::cout<<"numberOfPixelHits not available"<<std::endl;
  uint8_t numberOfPixelHoles=0;
  if( ! trk->summaryValue (numberOfPixelHoles, xAOD::SummaryType::numberOfPixelHoles) )
    std::cout<<"numberOfPixelHoles not available"<<std::endl;
  uint8_t numberOfSCTHits=0;
  if( ! trk->summaryValue (numberOfSCTHits, xAOD::SummaryType::numberOfSCTHits) )
    std::cout<<"numberOfSCTHits not available"<<std::endl;
  uint8_t numberOfSCTHoles=0;
  if( ! trk->summaryValue (numberOfSCTHoles, xAOD::SummaryType::numberOfSCTHoles) )
    std::cout<<"numberOfSCTHoles not available"<<std::endl;
  uint8_t numberOfPixelSharedHits=0;
  if( ! trk->summaryValue (numberOfPixelSharedHits, xAOD::SummaryType::numberOfPixelSharedHits) )
    std::cout<<"numberOfPixelSharedHits not available"<<std::endl;

  if(trk->pt() >= 1000. //not 400 MeV                                                                                                         
     && fabs(trk->eta()) <= 2.5
     && (numberOfPixelHits + numberOfSCTHits) >= 7
     && (numberOfPixelHoles + numberOfSCTHoles) <= 2
     && numberOfPixelHoles <= 1
     && numberOfPixelSharedHits <= 1) //this should be shared modules, but including SCT doesn't seem straightforward           
    return true;

  return false;
}
