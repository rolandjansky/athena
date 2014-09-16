/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL2MuonSA/MuFastTrackFitter.h"

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

#include "CLHEP/Units/PhysicalConstants.h"

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

static const InterfaceID IID_MuFastTrackFitter("IID_MuFastTrackFitter", 1, 0);

const InterfaceID& TrigL2MuonSA::MuFastTrackFitter::interfaceID() { return IID_MuFastTrackFitter; }

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::MuFastTrackFitter::MuFastTrackFitter(const std::string& type, 
						   const std::string& name,
						   const IInterface*  parent): 
  AlgTool(type,name,parent),
  m_msg(0),
  m_storeGateSvc( "StoreGateSvc", name ),
  m_use_mcLUT(true),
  m_alignmentBarrelLUTSvc(0)
{
  declareInterface<TrigL2MuonSA::MuFastTrackFitter>(this);
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::MuFastTrackFitter::~MuFastTrackFitter() 
{
  /*
  delete m_sagittaRadiusEstimate;
  delete m_alphaBetaEstimate;
  delete m_ptFromRadius;
  delete m_ptFromAlphaBeta;
  */
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MuFastTrackFitter::initialize()
{
   // Get a message stream instance
   m_msg = new MsgStream( msgSvc(), name() );
   msg() << MSG::DEBUG << "Initializing MuFastTrackFitter - package version " << PACKAGE_VERSION << endreq ;
   
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

StatusCode TrigL2MuonSA::MuFastTrackFitter::setMCFlag(BooleanProperty use_mcLUT)
{
  m_use_mcLUT = use_mcLUT;

  StatusCode sc = StatusCode::SUCCESS;

  if (m_use_mcLUT) {
    sc = serviceLocator()->service("PtBarrelLUTSvc_MC", m_ptBarrelLUTSvc);
  } else{ 
    sc = serviceLocator()->service("PtBarrelLUTSvc",    m_ptBarrelLUTSvc);
  }
  if (!sc.isSuccess()) {
    msg() << MSG::ERROR << "Could not find PtBarrelLUTSvc" << endreq;
    return sc;
  }
  
  if (m_use_mcLUT) {
    sc = serviceLocator()->service("PtEndcapLUTSvc_MC", m_ptEndcapLUTSvc);
  } else {
    sc = serviceLocator()->service("PtEndcapLUTSvc",    m_ptEndcapLUTSvc);
  }
  if (!sc.isSuccess()) {
    msg() << MSG::ERROR << "Could not find PtEndcapLUTSvc" << endreq;
    return sc;
  }

  if ( !m_use_mcLUT ) {
    sc = serviceLocator()->service("AlignmentBarrelLUTSvc", m_alignmentBarrelLUTSvc);
    if (!sc.isSuccess()) {
      msg() << MSG::ERROR << "Could not find PtBarrelLUTSvc" << endreq;
      return sc;
    }
  }

  // Set MsgStream for utils
  m_sagittaRadiusEstimate = new TrigL2MuonSA::SagittaRadiusEstimate(m_msg, m_use_mcLUT, m_alignmentBarrelLUTSvc);
  m_alphaBetaEstimate     = new TrigL2MuonSA::AlphaBetaEstimate(m_msg, m_ptEndcapLUTSvc);
  m_ptFromRadius          = new TrigL2MuonSA::PtFromRadius(m_msg, m_use_mcLUT, m_ptBarrelLUTSvc);
  m_ptFromAlphaBeta       = new TrigL2MuonSA::PtFromAlphaBeta(m_msg, m_ptEndcapLUTSvc);

  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MuFastTrackFitter::findTracks(const LVL1::RecMuonRoI*     p_roi,
						       TrigL2MuonSA::RpcFitResult& rpcFitResult,
						       std::vector<TrigL2MuonSA::TrackPattern>& v_trackPatterns)
{
   StatusCode sc = StatusCode::SUCCESS;

   std::vector<TrigL2MuonSA::TrackPattern>::iterator itTrack;
   for (itTrack=v_trackPatterns.begin(); itTrack!=v_trackPatterns.end(); itTrack++) {

     sc = m_sagittaRadiusEstimate->setSagittaRadius(p_roi, rpcFitResult, *itTrack);
     if (!sc.isSuccess()) {
       msg() << MSG::WARNING << "Barrel sagitta and radius estimation failed" << endreq;
       return sc;
     }

     sc = m_ptFromRadius->setPt(*itTrack);
     if (!sc.isSuccess()) {
       msg() << MSG::WARNING << "Barrel pT estimation failed" << endreq;
       return sc;
     }
     
   }

   return sc; 
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MuFastTrackFitter::findTracks(const LVL1::RecMuonRoI*     p_roi,
						       TrigL2MuonSA::TgcFitResult& tgcFitResult,
						       std::vector<TrigL2MuonSA::TrackPattern>& v_trackPatterns)
{
   StatusCode sc = StatusCode::SUCCESS;

   std::vector<TrigL2MuonSA::TrackPattern>::iterator itTrack;
   for (itTrack=v_trackPatterns.begin(); itTrack!=v_trackPatterns.end(); itTrack++) {

     sc = m_alphaBetaEstimate->setAlphaBeta(p_roi, tgcFitResult, *itTrack);
     if (!sc.isSuccess()) {
       msg() << MSG::WARNING << "Endcap alpha and beta estimation failed" << endreq;
       return sc;
     }
    
     sc = m_ptFromAlphaBeta->setPt(*itTrack);
     if (!sc.isSuccess()) {
       msg() << MSG::WARNING << "Endcap pT estimation failed" << endreq;
       return sc;
     }
     
   }

   return sc; 
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MuFastTrackFitter::finalize()
{
   msg() << MSG::DEBUG << "Finalizing MuFastTrackFitter - package version " << PACKAGE_VERSION << endreq;
   
   // delete message stream
   if ( m_msg ) delete m_msg;

   if ( m_sagittaRadiusEstimate ) delete m_sagittaRadiusEstimate;
   if ( m_alphaBetaEstimate ) delete m_alphaBetaEstimate;
   if ( m_ptFromRadius ) delete m_ptFromRadius;
   if ( m_ptFromAlphaBeta ) delete m_ptFromAlphaBeta;
   
   StatusCode sc = AlgTool::finalize(); 
   return sc;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

