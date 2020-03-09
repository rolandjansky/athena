/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL2MuonSA/MuFastTrackFitter.h"

#include "CLHEP/Units/PhysicalConstants.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

static const InterfaceID IID_MuFastTrackFitter("IID_MuFastTrackFitter", 1, 0);

const InterfaceID& TrigL2MuonSA::MuFastTrackFitter::interfaceID() { return IID_MuFastTrackFitter; }

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::MuFastTrackFitter::MuFastTrackFitter(const std::string& type, 
						   const std::string& name,
						   const IInterface*  parent): 
  AthAlgTool(type,name,parent),
  m_use_mcLUT(true),
  m_use_endcapInnerFromBarrel(false),
  m_sagittaRadiusEstimate("TrigL2MuonSA::SagittaRadiusEstimate"),
  m_alphaBetaEstimate("TrigL2MuonSA::AlphaBetaEstimate"),
  m_ptFromRadius("TrigL2MuonSA::PtFromRadius"),
  m_ptFromAlphaBeta("TrigL2MuonSA::PtFromAlphaBeta")
{
  declareInterface<TrigL2MuonSA::MuFastTrackFitter>(this);
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::MuFastTrackFitter::~MuFastTrackFitter() 
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MuFastTrackFitter::initialize()
{
   StatusCode sc;
   sc = AthAlgTool::initialize();
   if (!sc.isSuccess()) {
     ATH_MSG_ERROR("Could not initialize the AthAlgTool base class.");
      return sc;
   }
   
   ATH_CHECK( m_sagittaRadiusEstimate.retrieve() );
   ATH_CHECK( m_alphaBetaEstimate.retrieve() );
   ATH_CHECK( m_ptFromRadius.retrieve() );
   ATH_CHECK( m_ptFromAlphaBeta.retrieve() );
   
   // 
   return StatusCode::SUCCESS; 
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MuFastTrackFitter::setMCFlag(BooleanProperty use_mcLUT)
{
  m_use_mcLUT = use_mcLUT;

//  StatusCode sc = StatusCode::SUCCESS;

  if (m_use_mcLUT) {
    // Barrel 
    const ServiceHandle<PtBarrelLUTSvc> ptBarrelLUTSvc("PtBarrelLUTSvc_MC", name());
    if ( ptBarrelLUTSvc.retrieve().isFailure() ) {
       ATH_MSG_ERROR("Could not find PtBarrelLUTSvc");
       return StatusCode::FAILURE;
    }
    // Endcap
    const ServiceHandle<PtEndcapLUTSvc> ptEndcapLUTSvc("PtEndcapLUTSvc_MC", name());
    if ( ptEndcapLUTSvc.retrieve().isFailure() ) {
       ATH_MSG_ERROR("Could not find PtEndcaplLUTSvc");
       return StatusCode::FAILURE;
    }

    m_alphaBetaEstimate->setMCFlag(m_use_mcLUT, &*ptEndcapLUTSvc);

    m_ptFromRadius->setMCFlag(m_use_mcLUT, &*ptBarrelLUTSvc);

    m_ptFromAlphaBeta->setMCFlag(m_use_mcLUT, &*ptEndcapLUTSvc);

  } else { 
    // Barrel
    const ServiceHandle<PtBarrelLUTSvc> ptBarrelLUTSvc("PtBarrelLUTSvc", name());
    if ( ptBarrelLUTSvc.retrieve().isFailure() ) {
       ATH_MSG_ERROR("Could not find PtBarrelLUTSvc");
       return StatusCode::FAILURE;
    }
    // Endcap
    const ServiceHandle<PtEndcapLUTSvc> ptEndcapLUTSvc("PtEndcapLUTSvc", name());
    if ( ptEndcapLUTSvc.retrieve().isFailure() ) {
       ATH_MSG_ERROR("Could not find PtEndcaplLUTSvc");
       return StatusCode::FAILURE;
    }

    m_alphaBetaEstimate->setMCFlag(m_use_mcLUT, &*ptEndcapLUTSvc);

    m_ptFromRadius->setMCFlag(m_use_mcLUT, &*ptBarrelLUTSvc);

    m_ptFromAlphaBeta->setMCFlag(m_use_mcLUT, &*ptEndcapLUTSvc);

  }

  const ServiceHandle<AlignmentBarrelLUTSvc> alignmentBarrelLUTSvc("AlignmentBarrelLUTSvc", name());
  if ( alignmentBarrelLUTSvc.retrieve().isFailure() ) {
     ATH_MSG_ERROR("Could not find AlignmentBarrelLUTSvc");
     return StatusCode::FAILURE;
  }

  // Calculation of sagitta and radius
  // sc = m_sagittaRadiusEstimate.retrieve();
  // if ( sc.isFailure() ) {
  //   ATH_MSG_ERROR("Could not retrieve " << m_sagittaRadiusEstimate);
  //   return sc;
  // }
  // ATH_MSG_DEBUG("Retrieved service " << m_sagittaRadiusEstimate);

  m_sagittaRadiusEstimate->setMCFlag(m_use_mcLUT, &*alignmentBarrelLUTSvc);

  // Calculation of alpha and beta
  // sc = m_alphaBetaEstimate.retrieve();
  // if ( sc.isFailure() ) {
  //   ATH_MSG_ERROR("Could not retrieve " << m_alphaBetaEstimate);
  //   return sc;
  // }
  // ATH_MSG_DEBUG("Retrieved service " << m_alphaBetaEstimate);


  // conversion: radius -> pT
  // sc = m_ptFromRadius.retrieve();
  // if ( sc.isFailure() ) {
  //   ATH_MSG_ERROR("Could not retrieve " << m_ptFromRadius);
  //   return sc;
  // }
  // ATH_MSG_DEBUG("Retrieved service " << m_ptFromRadius);


  // conversion: alpha, beta -> pT
  // sc = m_ptFromAlphaBeta.retrieve();
  // if ( sc.isFailure() ) {
  //   ATH_MSG_ERROR("Could not retrieve " << m_ptFromAlphaBeta);
  //   return sc;
  // }
  // ATH_MSG_DEBUG("Retrieved service " << m_ptFromAlphaBeta);

  ATH_MSG_DEBUG( "Completed tp set " << (m_use_mcLUT?"MC":"not MC") << " flag" );    

  return StatusCode::SUCCESS;
}

void TrigL2MuonSA::MuFastTrackFitter::setUseEIFromBarrel( BooleanProperty use_endcapInnerFromBarrel )
{
  m_use_endcapInnerFromBarrel = use_endcapInnerFromBarrel;
  return;
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

     m_sagittaRadiusEstimate -> setUseEndcapInner( m_use_endcapInnerFromBarrel );
     sc = m_sagittaRadiusEstimate->setSagittaRadius(p_roi, rpcFitResult, *itTrack);
     if (!sc.isSuccess()) {
       ATH_MSG_WARNING("Barrel sagitta and radius estimation failed");
       return sc;
     }

     sc = m_ptFromRadius->setPt(*itTrack);
     if (!sc.isSuccess()) {
       ATH_MSG_WARNING("Barrel pT estimation failed");
       return sc;
     }
     
   }

   return sc; 
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MuFastTrackFitter::findTracks(const LVL1::RecMuonRoI*     p_roi,
						       TrigL2MuonSA::TgcFitResult& tgcFitResult,
						       std::vector<TrigL2MuonSA::TrackPattern>& v_trackPatterns,
                                                       const TrigL2MuonSA::MuonRoad& muonRoad)
{
   StatusCode sc = StatusCode::SUCCESS;

   std::vector<TrigL2MuonSA::TrackPattern>::iterator itTrack;
   for (itTrack=v_trackPatterns.begin(); itTrack!=v_trackPatterns.end(); itTrack++) {

     sc = m_alphaBetaEstimate->setAlphaBeta(p_roi, tgcFitResult, *itTrack, muonRoad);
     if (!sc.isSuccess()) {
       ATH_MSG_WARNING("Endcap alpha and beta estimation failed");
       return sc;
     }
    
     sc = m_ptFromAlphaBeta->setPt(*itTrack,tgcFitResult);
     if (!sc.isSuccess()) {
       ATH_MSG_WARNING("Endcap pT estimation failed");
       return sc;
     }
     
   }

   return sc; 
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MuFastTrackFitter::finalize()
{
  ATH_MSG_DEBUG("Finalizing MuFastTrackFitter - package version " << PACKAGE_VERSION);
   
   StatusCode sc = AthAlgTool::finalize(); 
   return sc;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

