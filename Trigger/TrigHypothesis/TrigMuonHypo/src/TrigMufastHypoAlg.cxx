/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "xAODTrigMuon/L2StandAloneMuonContainer.h"
#include "TrigMuonHypo/TrigMufastHypoAlg.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

using namespace TrigCompositeUtils; 

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigMufastHypoAlg::TrigMufastHypoAlg( const std::string& name,
				      ISvcLocator* pSvcLocator ) :
  ::AthReentrantAlgorithm( name, pSvcLocator )
{
   declareProperty("ViewRoIs", m_viewsCollectionKey = std::string("ViewRoIs"), "MUViewRoIs to read in");
   declareProperty("MuFastDecisions", m_muFastCollectionKey = std::string("MufastDecisions"), "xAOD::L2StandAloneMuonContainer to read in");
   declareProperty("Decisions", m_decisionsKey = std::string("TrigMufastHypoAlgDesicions"), "Decisions");
   declareProperty("L1Decisions", m_previousDecisionsKey = std::string("L1Decisions"), "Key for L1 decisions per RoI");
} 

TrigMufastHypoAlg::~TrigMufastHypoAlg() 
{}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigMufastHypoAlg::initialize()
{
//  ATH_MSG_INFO ( "Initializing " << name() << "..." );

  ATH_CHECK(m_viewsCollectionKey.initialize());
  ATH_CHECK(m_muFastCollectionKey.initialize());
  ATH_CHECK(m_decisionsKey.initialize());
  ATH_CHECK(m_previousDecisionsKey.initialize());
 
  ATH_MSG_INFO( "Initialization completed successfully" );
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigMufastHypoAlg::finalize() 
{   
//  ATH_MSG_INFO( "Finalizing " << name() << "..." );
  ATH_MSG_INFO( "Finalization completed successfully" );
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigMufastHypoAlg::execute_r( const EventContext& context ) const
{

  ATH_MSG_DEBUG("StatusCode TrigMufastHypoAlg::execute_r start");

  std::map<const TrigRoiDescriptor*, const TrigCompositeUtils::Decision* > roiToDecision;
  auto previousDecisionsHandle = SG::makeHandle( m_previousDecisionsKey, context );
  for ( auto previousDecision: *previousDecisionsHandle ) { 
    auto roiEL = previousDecision->objectLink<TrigRoiDescriptorCollection>( "initialRoI" );
    ATH_CHECK( roiEL.isValid() );
    const TrigRoiDescriptor* roi = *roiEL;
    roiToDecision.insert( std::make_pair( roi, previousDecision ) );
  }   
  ATH_MSG_DEBUG("REGTEST: RoI to decisions map size: " << roiToDecision.size() );

  auto viewsHandle = SG::makeHandle( m_viewsCollectionKey, context );
  if (!viewsHandle.isValid()){
    ATH_MSG_ERROR("ReadHandle for MUViewRoIs isn't Valid");
    return StatusCode::FAILURE;
  }

  for ( auto view: *viewsHandle ) {

    ATH_MSG_DEBUG("REGTEST: MUViewRoIs = " << view);

    // retrieve MuFastDecisions with ReadHandle
    auto muFastCollectionHandle = SG::makeHandle( m_muFastCollectionKey );
    const xAOD::L2StandAloneMuonContainer *muFastCollection = muFastCollectionHandle.cptr();
    if (!muFastCollectionHandle.isValid()){
      ATH_MSG_ERROR("ReadHandle for xAOD::L2StandAloneMuonContainer isn't Valid");
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("REGTEST: xAOD::L2StansAloneMuoonContainer size = " << muFastCollection->size());
    xAOD::L2StandAloneMuonContainer::const_iterator p_muon = (*muFastCollection).begin();
    xAOD::L2StandAloneMuonContainer::const_iterator p_muonEn = (*muFastCollection).end();
    for (;p_muon != p_muonEn; ++p_muon) {
      ATH_MSG_DEBUG("REGTEST: xAOD::L2StandAloneMuonContainer pt = " << (*p_muon)->pt());
      ATH_MSG_DEBUG("REGTEST: xAOD::L2StansAloneMuonContainer  eta/phi = " << (*p_muon)->eta() << "/" << (*p_muon)->phi());  
    }
  }

  auto decisions = std::make_unique<DecisionContainer>();
  auto aux = std::make_unique<DecisionAuxContainer>();
  decisions->setStore( aux.get() );

  //SG::WriteHandle<xAOD::L2StandAloneMuonContainer> muFastCollection(m_muFastCollectionKey);
  //ATH_CHECK(muFastCollection.record(std::make_unique<xAOD::L2StandAloneMuonContainer>()));

  auto handle =  SG::makeHandle( m_decisionsKey, context );     
  ATH_CHECK( handle.record( std::move( decisions ), std::move( aux ) ) );

  ATH_MSG_DEBUG("StatusCode TrihTrigMufastHypoAlgAlg::execute_r success");
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

