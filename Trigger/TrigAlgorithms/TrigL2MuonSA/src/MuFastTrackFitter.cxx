/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuFastTrackFitter.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::MuFastTrackFitter::MuFastTrackFitter(const std::string& type, 
						   const std::string& name,
						   const IInterface*  parent): 
  AthAlgTool(type,name,parent)
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MuFastTrackFitter::initialize()
{
   
   ATH_CHECK( m_sagittaRadiusEstimate.retrieve() );
   ATH_CHECK( m_alphaBetaEstimate.retrieve() );
   ATH_CHECK( m_ptFromRadius.retrieve() );
   ATH_CHECK( m_ptFromAlphaBeta.retrieve() );

   return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MuFastTrackFitter::setMCFlag(BooleanProperty use_mcLUT)
{
  m_use_mcLUT = use_mcLUT;

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

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MuFastTrackFitter::findTracks(const LVL1::RecMuonRoI*     p_roi,
						       TrigL2MuonSA::RpcFitResult& rpcFitResult,
						       std::vector<TrigL2MuonSA::TrackPattern>& v_trackPatterns)
{

   for (TrigL2MuonSA::TrackPattern& itTrack : v_trackPatterns) {

     m_sagittaRadiusEstimate -> setUseEndcapInner( m_use_endcapInnerFromBarrel );
     ATH_CHECK( m_sagittaRadiusEstimate->setSagittaRadius(p_roi, rpcFitResult, itTrack) );

     ATH_CHECK( m_ptFromRadius->setPt(itTrack) );
     
   }

   return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MuFastTrackFitter::findTracks(const LVL1::RecMuonRoI*     p_roi,
						       TrigL2MuonSA::TgcFitResult& tgcFitResult,
						       std::vector<TrigL2MuonSA::TrackPattern>& v_trackPatterns,
                                                       const TrigL2MuonSA::MuonRoad& muonRoad)
{

   for (TrigL2MuonSA::TrackPattern& itTrack : v_trackPatterns) {

     ATH_CHECK( m_alphaBetaEstimate->setAlphaBeta(p_roi, tgcFitResult, itTrack, muonRoad) );

     if ( itTrack.etaBin < -1 ) {
       itTrack.etaBin = (int)((fabs(muonRoad.extFtfMiddleEta)-1.)/0.05); // eta binning is the same as AlphaBetaEstimate
     }

     ATH_CHECK( m_ptFromAlphaBeta->setPt(itTrack,tgcFitResult) );
     
   }

   return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

