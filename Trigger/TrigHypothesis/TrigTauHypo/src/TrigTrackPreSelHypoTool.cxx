/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"

#include "AthenaMonitoringKernel/Monitored.h"

#include "TrigTrackPreSelHypoTool.h"

//class ISvcLocator;

using namespace TrigCompositeUtils;

TrigTrackPreSelHypoTool::TrigTrackPreSelHypoTool( const std::string& type,
                  const std::string& name, 
                  const IInterface* parent ) 
  : base_class( type, name, parent ),
    m_decisionId( HLT::Identifier::fromToolName( name ) ) 
{
}

TrigTrackPreSelHypoTool::~TrigTrackPreSelHypoTool()
{  
}

StatusCode TrigTrackPreSelHypoTool::initialize()
{
  
  ATH_MSG_INFO( "in initialize()" );
  
  if ( !m_monTool.empty() ) CHECK( m_monTool.retrieve() );

  ATH_MSG_DEBUG( "Tool configured for chain/id: " << m_decisionId  );

  ATH_MSG_INFO( " REGTEST: TrigTrackPreSelHypoTool will cut on "                           );
  ATH_MSG_INFO( " REGTEST: Lower pt cut for track selection: " << m_lowerTrackPtCut   );
  ATH_MSG_INFO( " REGTEST: Tracks in core <= "                 << m_tracksInCoreCut   );  
  ATH_MSG_INFO( " REGTEST: Tracks in outer <= "                << m_tracksInIsoCut    );  
  ATH_MSG_INFO( " REGTEST: Relax High pT: "                    << m_relax_highpt      );
  ATH_MSG_INFO( " REGTEST: Relax High pT Threshold: "           << m_highpt_threshold );
  ATH_MSG_INFO( " REGTEST: ------ "                                                   );


  ATH_MSG_INFO( "Initialization of TrigTrackPreSelHypoTool completed successfully" );
  return StatusCode::SUCCESS;
}

bool TrigTrackPreSelHypoTool::decide( const ITrigTrackPreSelHypoTool::TrackingInfo& input ) const {

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
  auto foundTracks = input.trackcollection;

  if(foundTracks->size()!=0){

    ATH_MSG_DEBUG( " Input track collection has size " << foundTracks->size() );

    const Trk::Track *Ltrack = nullptr;
    const Trk::Perigee *tp = nullptr;
    float trk_pt_max = 0;

    //first, identify highest pT leading track, we may need to add here some track quality requirements...
    for (const Trk::Track* track : *foundTracks){
      tp = track->perigeeParameters();
      if(tp){
	      float trk_pt = tp->pT();
         if(trk_pt < m_lowerTrackPtCut) continue;
	      float trk_eta = tp->eta();
	      float trk_phi = tp->parameters()[Trk::phi];
	      double dR_trk_tau = sqrt((roIEta-trk_eta)*(roIEta-trk_eta) + CxxUtils::wrapToPi(roIPhi-trk_phi)*CxxUtils::wrapToPi(roIPhi-trk_phi));
	      if ((trk_pt > trk_pt_max) && dR_trk_tau < m_deltaRLeadTrkRoI) {
	         Ltrack = track;
	         trk_pt_max = trk_pt;
	      }
      }
    }
    if(Ltrack) {
      ATH_MSG_DEBUG( " leading track pT " << trk_pt_max );
    }
    else ATH_MSG_DEBUG( " no leading track pT, using the RoI " );

    //next, count tracks in core and outer region with respect to leading track (or RoI)  
    float ltrk_eta;
    float ltrk_phi;
    float ltrk_z0;// for pile-up suppression cut
    bool usePileupSuppCut;

    if(Ltrack){
      ltrk_eta = Ltrack->perigeeParameters()->eta();
      ltrk_phi = Ltrack->perigeeParameters()->parameters()[Trk::phi];
      // keep using the RoI direction (updated after topoclustering) as it is done in tauRec
      //ltrk_eta = roIEta;
      //ltrk_phi = roIPhi;
      ltrk_z0 = Ltrack->perigeeParameters()->parameters()[Trk::z0];
      usePileupSuppCut = true;

    }
    else{// if no leading track use the RoI as center, but turn off pileup suppression cut
      ltrk_eta = roIEta;
      ltrk_phi = roIPhi;
      ltrk_z0 = 0.;
      usePileupSuppCut = false;
    }

    for (const Trk::Track* track : *foundTracks){
      tp = track->perigeeParameters();
      if(tp){
	     float trk_eta = tp->eta();
	     float trk_phi = tp->parameters()[Trk::phi];
	     float trk_z0 = tp->parameters()[Trk::z0];
        float trk_pt = tp->pT();
        if(trk_pt < m_lowerTrackPtCut) continue;		
	     float dR_trki_ltrk = sqrt((ltrk_eta-trk_eta)*(ltrk_eta-trk_eta) + CxxUtils::wrapToPi(ltrk_phi-trk_phi)*CxxUtils::wrapToPi(ltrk_phi-trk_phi));
	     float dZ0 = fabs(ltrk_z0 - trk_z0);
	     if((dR_trki_ltrk < m_coreSize) && ((dZ0 < m_deltaZ0Cut)||!usePileupSuppCut)){
	       ++nTracksInCore;
	     }
	     if((dR_trki_ltrk > m_coreSize) && (dR_trki_ltrk < m_outerSize) && ((dZ0 < m_deltaZ0Cut)||!usePileupSuppCut)){
	       ++nTracksInIso;
	     }
      }
    }
  }
 
  if (nTracksInCore > m_tracksInCoreCut || nTracksInIso > m_tracksInIsoCut) 
  {
	ATH_MSG_DEBUG(
	      " REGTEST: Number of tracks in core(isolation) = "
	      << nTracksInCore
	      << " ("
	      << nTracksInIso
	      << ") "
         << "did not pass the number thresholds: " 
 	      << m_tracksInCoreCut
	      << " ("
	      << m_tracksInIsoCut
	      << ")" );
      return pass;
  }

  PassedCuts++;
  pass = true;
  
  ATH_MSG_DEBUG( " REGTEST: TE accepted !! " );
  
  
  return pass;
}


StatusCode TrigTrackPreSelHypoTool::decide(  std::vector<TrackingInfo>& input )  const {

  for ( auto& i: input ) {
    if ( passed ( m_decisionId.numeric(), i.previousDecisionIDs ) ) {
      if ( decide( i ) ) {
   addDecisionID( m_decisionId, i.decision );
      }
    }
  }
  return StatusCode::SUCCESS;
}
