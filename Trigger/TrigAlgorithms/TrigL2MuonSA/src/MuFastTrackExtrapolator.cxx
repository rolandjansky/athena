/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL2MuonSA/MuFastTrackExtrapolator.h"

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

#include "CLHEP/Units/PhysicalConstants.h"

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

static const InterfaceID IID_MuFastTrackExtrapolator("IID_MuFastTrackExtrapolator", 1, 0);

const InterfaceID& TrigL2MuonSA::MuFastTrackExtrapolator::interfaceID() { return IID_MuFastTrackExtrapolator; }

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::MuFastTrackExtrapolator::MuFastTrackExtrapolator(const std::string& type, 
							       const std::string& name,
							       const IInterface*  parent): 
  AlgTool(type,name,parent),
  m_msg(0),
  m_storeGateSvc( "StoreGateSvc", name )
{
  declareInterface<TrigL2MuonSA::MuFastTrackExtrapolator>(this);
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::MuFastTrackExtrapolator::~MuFastTrackExtrapolator() 
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MuFastTrackExtrapolator::initialize()
{
   // Get a message stream instance
   m_msg = new MsgStream( msgSvc(), name() );
   msg() << MSG::DEBUG << "Initializing MuFastTrackExtrapolator - package version " << PACKAGE_VERSION << endreq ;
   
   StatusCode sc;
   sc = AlgTool::initialize();
   if (!sc.isSuccess()) {
      msg() << MSG::ERROR << "Could not initialize the AlgTool base class." << endreq;
      return sc;
   }
   
   // Locate the StoreGateSvc
   sc =  m_storeGateSvc.retrieve();
   if (!sc.isSuccess()) {
      msg() << MSG::ERROR << "Could not find StoreGateSvc" << endreq;
      return sc;
   }

   // 
   return StatusCode::SUCCESS; 
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
void TrigL2MuonSA::MuFastTrackExtrapolator::setExtrapolatorTool(ToolHandle<ITrigMuonBackExtrapolator>* backExtrapolator)
{
  m_backExtrapolatorTool = backExtrapolator;
  return;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MuFastTrackExtrapolator::extrapolateTrack(std::vector<TrigL2MuonSA::TrackPattern>& v_trackPatterns,
								   double winPt)
{
  msg() << MSG::DEBUG << "in extrapolateTrack" << endreq;
  
  StatusCode sc = StatusCode::SUCCESS;
  
  std::vector<TrigL2MuonSA::TrackPattern>::iterator itTrack;
  for (itTrack=v_trackPatterns.begin(); itTrack!=v_trackPatterns.end(); itTrack++) {

    MuonFeature* feature = new MuonFeature(itTrack->s_address,
					   (itTrack->pt)*(itTrack->charge),
					   itTrack->superPoints[0].R,
					   itTrack->etaMap,
					   itTrack->phi,
					   itTrack->phiDir,
					   itTrack->superPoints[0].Z,
					   itTrack->superPoints[0].Alin,
					   1.0);
    
    double etaVtx = 0.;
    double phiVtx = 0.;
    double sigEta = 0.;
    double sigPhi = 0.;
    
    sc = (*m_backExtrapolatorTool)->give_eta_phi_at_vertex(feature, etaVtx, sigEta, phiVtx, sigPhi, winPt);
    
    if (sc.isFailure()) {
      msg() << MSG::DEBUG  << "BackExtrapolator problem: "
	    << "Pt of Muon Feature out of BackExtrapolator range."
	    << endreq;
      msg() << MSG::DEBUG  << "Use Muon Feature position to fill the "
	    << "TrigRoiDescriptor for IDSCAN." << endreq;
      etaVtx = itTrack->etaMap;
      phiVtx = itTrack->phi;
    }

    itTrack->etaVtx = etaVtx;
    itTrack->phiVtx = phiVtx;
    
    if (feature) delete feature;
  }
  
   return sc; 
 }

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MuFastTrackExtrapolator::finalize()
{
  msg() << MSG::DEBUG << "Finalizing MuFastTrackExtrapolator - package version " << PACKAGE_VERSION << endreq;
  
  // delete message stream
  if ( m_msg ) delete m_msg;
  
  StatusCode sc = AlgTool::finalize(); 
  return sc;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
