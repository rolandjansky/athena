/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <iterator>
#include <sstream>

//
#include "TLorentzVector.h"

#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ListItem.h"
//

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "CxxUtils/phihelper.h"

#include "xAODTau/TauJetContainer.h"
#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

#include "xAODTrigger/TrigPassBits.h"

#include "xAODTracking/TrackParticleContainer.h"

#include "AthenaMonitoringKernel/Monitored.h"

#include "TrigTrkPrecHypoTool.h"

//class ISvcLocator;

using namespace TrigCompositeUtils;

TrigTrkPrecHypoTool::TrigTrkPrecHypoTool( const std::string& type,
                  const std::string& name, 
                  const IInterface* parent ) 
  : base_class( type, name, parent ),
    m_decisionId( HLT::Identifier::fromToolName( name ) ) 
{
}

TrigTrkPrecHypoTool::~TrigTrkPrecHypoTool()
{  
}

StatusCode TrigTrkPrecHypoTool::initialize()
{
  
  ATH_MSG_DEBUG( "in initialize()" );
  
  if ( !m_monTool.empty() ) CHECK( m_monTool.retrieve() );

  ATH_MSG_DEBUG( "Tool configured for chain/id: " << m_decisionId  );

  ATH_MSG_DEBUG( " REGTEST: TrigTrkPrecHypoTool will cut on "                           );
  ATH_MSG_DEBUG( " REGTEST: Lower pt cut for track selection: " << m_lowerTrackPtCut   );
  ATH_MSG_DEBUG( " REGTEST: Tracks in core <= "                 << m_tracksInCoreCut   );  
  ATH_MSG_DEBUG( " REGTEST: Tracks in outer <= "                << m_tracksInIsoCut    );  
  ATH_MSG_DEBUG( " REGTEST: Relax High pT: "                    << m_relax_highpt      );
  ATH_MSG_DEBUG( " REGTEST: Relax High pT Threshold: "           << m_highpt_threshold );
  ATH_MSG_DEBUG( " REGTEST: ------ "                                                   );


  ATH_MSG_DEBUG( "Initialization of TrigTrkPrecHypoTool completed successfully" );
  return StatusCode::SUCCESS;
}

bool TrigTrkPrecHypoTool::decide( const ITrigTrkPrecHypoTool::TrackingInfo& input ) const {

  // Get the messaging service, print where you are
  ATH_MSG_DEBUG( "REGTEST:"<< name() << ": in execute()" );

  bool pass = false;

  using namespace Monitored;

  auto nTracksInCore     = Monitored::Scalar<int>( "nTracksInCore", -1);
  auto nTracksInIso      = Monitored::Scalar<int>( "nTracksInIso", -1);
  auto PassedCuts        = Monitored::Scalar<int>( "CutCounter", -1 );

  // general reset
  PassedCuts = 0;

  //get RoI descriptor
  auto roiDescriptor = input.roi;
  float roIEta = roiDescriptor->eta();
  float roIPhi = roiDescriptor->phi();
  float roIZ   = roiDescriptor->zed();

  ATH_MSG_DEBUG( "Input RoI eta: " << roIEta << " Input RoI phi: " << roIPhi << " Input RoI z: " << roIZ);

  // get the tracks collection
  // Retrieve Input TrackCollection
  auto foundTracks = input.trackparticles;

  if(foundTracks->size()!=0){

    ATH_MSG_DEBUG( " Input track collection has size " << foundTracks->size() );

  }

  PassedCuts++;
  pass = true;
  
  ATH_MSG_DEBUG( " REGTEST: TE accepted !! " );
  
  
  return pass;
}


StatusCode TrigTrkPrecHypoTool::decide(  std::vector<TrackingInfo>& input )  const {

  for ( auto& i: input ) {
    if ( passed ( m_decisionId.numeric(), i.previousDecisionIDs ) ) {
      if ( decide( i ) ) {
   addDecisionID( m_decisionId, i.decision );
      }
    }
  }
  return StatusCode::SUCCESS;
}
