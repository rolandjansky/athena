// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigBPhysHypo/TrigMultiTrkHypoToolMT.cxx
 **
 **   Description:  Multi-track hypothesis tool for bphys triggers
 **
 **   Author: H. Russell
 **
 **************************************************************************/

#include "TrigMultiTrkHypoToolMT.h"

#include <math.h>

// additions of xAOD objects
#include "xAODTracking/TrackParticle.h"

#include "xAODTrigBphys/TrigBphys.h"
#include "xAODTrigBphys/TrigBphysContainer.h"

#include "AthenaMonitoringKernel/Monitored.h"

#include "DecisionHandling/TrigCompositeUtils.h"

class ISvcLocator;


TrigMultiTrkHypoToolMT::TrigMultiTrkHypoToolMT(const std::string & type,
                                     const std::string & name,
                                     const IInterface* parent )
   : AthAlgTool( type, name, parent ),
     m_decisionId( HLT::Identifier::fromToolName( name ) )
{}

TrigMultiTrkHypoToolMT::~TrigMultiTrkHypoToolMT()
{ }

StatusCode TrigMultiTrkHypoToolMT::initialize()
{
  ATH_MSG_DEBUG("AcceptAll            = " << (m_acceptAll==true ? "True" : "False") );
  ATH_MSG_DEBUG("OppositeCharge       = " << (m_oppositeCharge==true ? "True" : "False") );
  ATH_MSG_DEBUG("LowerMassCut         = " << m_lowerMassCut );
  ATH_MSG_DEBUG("UpperMassCut         = " << m_upperMassCut );
  ATH_MSG_DEBUG("ApplyUpperMassCut    = " << m_applyUpperMassCut );
  ATH_MSG_DEBUG("ApplyChi2Cut         = " << m_applyChi2Cut );
  ATH_MSG_DEBUG("Chi2VtxCut           = " << m_chi2VtxCut );
	
  if ( not m_monTool.name().empty() ) {
        ATH_CHECK( m_monTool.retrieve() );
        ATH_MSG_DEBUG("m_monTool name: " << m_monTool);
  }
  if(static_cast<int>(m_ptTrkMin.size()) != m_nTrk){
      ATH_MSG_ERROR("Requested " << m_nTrk << " tracks per vertex, but only provided "
            << m_ptTrkMin.size() << " track pTs!");        
      return StatusCode::FAILURE;
  } else if(msgLvl(MSG::DEBUG)){
      msg() << MSG::DEBUG <<  "requiring " << m_nTrk << " tracks with pT: ";
      for(float pt :  m_ptTrkMin) msg() << MSG::INFO << pt <<", ";
      msg() << MSG::DEBUG << endmsg;  
  }

  ATH_MSG_DEBUG("Initialization completed successfully");

  return StatusCode::SUCCESS;
}


//-------------------------------------------------------------------------------------
bool TrigMultiTrkHypoToolMT::decideOnSingleObject( const xAOD::TrigBphys* trigBphys, size_t  ) const{
 
  using namespace Monitored;

  ATH_MSG_DEBUG( "in TrigMultiTrkHypoToolMT::decideOnSingleObject(), looking at TrigBphys object");

  bool thisPassedMassCut = false;
  bool thisPassedChi2Cut = false;
  bool thisPassedTrkPtCut = false;
  
  bool result = false;
  
  std::vector<float> ini_trkPts(0);

  auto mon_cutCounter = Monitored::Scalar<int>("CutCounter",-1);
  auto mon_fitChi2    = Monitored::Scalar<float>("FitChi2",-1.);
  auto mon_vtxMass   = Monitored::Scalar<float>("VertexMass",-1.);
  auto mon_trkPts     = Monitored::Collection("trackPts", ini_trkPts);
  auto mon_totCharge  = Monitored::Scalar<int>("totCharge", -9);
  auto mon = Monitored::Group( m_monTool, mon_cutCounter, mon_fitChi2, mon_vtxMass, mon_trkPts, mon_totCharge);

  mon_cutCounter = 0;
        
  if (trigBphys->particleType() == xAOD::TrigBphys::MULTIMU ) {
  
      ATH_MSG_DEBUG("Got Bphys particle with mass " << trigBphys->mass() << " chi2 :  " << trigBphys->fitchi2() ); 
      float vertexMass = trigBphys->mass();
      thisPassedMassCut = (m_lowerMassCut < vertexMass && ((vertexMass < m_upperMassCut) || (!m_applyUpperMassCut) ));
      thisPassedChi2Cut = ((!m_applyChi2Cut) || (trigBphys->fitchi2() < m_chi2VtxCut && trigBphys->fitchi2() >= -1e-10) );
      if(thisPassedMassCut){ ATH_MSG_DEBUG("Passed vertex mass cut (mass = " << vertexMass << " GeV)");}
      mon_vtxMass = ((vertexMass*0.001));
      if(thisPassedChi2Cut){
            ATH_MSG_DEBUG("Apply chi2 cut : " << m_applyChi2Cut << " chi2 :  " << trigBphys->fitchi2() << " passed Chi2 cut < "<< m_chi2VtxCut );
      }
      mon_fitChi2 = trigBphys->fitchi2();
      unsigned int nTrk =trigBphys->nTrackParticles();
      if( nTrk != m_ptTrkMin.size() ){
        ATH_MSG_ERROR("Vertex has " << nTrk << " tracks trying to pass " << m_ptTrkMin.size() << " track thresholds!");
        return false;
      }
      //std::vector<bool> tracksPass; tracksPass.assign(nTrk, false);
      int totq = 0;
      std::vector<float> trackPts;
      for(unsigned int i=0; i < nTrk; i++ ){
          const auto tp = trigBphys->trackParticle(i);
          ATH_MSG_DEBUG("Track pt/eta/phi/charge: " << 
            trigBphys->trackParticle(i)->pt() << ", " <<
            trigBphys->trackParticle(i)->eta() << ", " << 
            trigBphys->trackParticle(i)->phi() << ", " << 
            trigBphys->trackParticle(i)->charge());
          totq += (tp->qOverP() > 0) ? 1 : ((tp->qOverP() < 0) ? -1 : 0);
          trackPts.push_back(tp->pt());
      }
      mon_totCharge = totq;
      std::sort(trackPts.rbegin(), trackPts.rend()); //reverse sort the list
      unsigned int nTrk_pass = 0;
      for(unsigned int i=0; i < nTrk; i++ ){
          ini_trkPts.push_back(trackPts.at(i)*0.001);
          if(trackPts.at(i) > m_ptTrkMin[i]) nTrk_pass++;
          ATH_MSG_DEBUG("track pt " << trackPts.at(i) << " >? " << m_ptTrkMin[i]);
      }      
      ATH_MSG_DEBUG("Passed " << nTrk_pass << " tracks, out of " << nTrk << " tracks");
      if(nTrk_pass == nTrk){
          thisPassedTrkPtCut = true;
      }
      if(!thisPassedMassCut && !thisPassedChi2Cut && !thisPassedTrkPtCut){
          ATH_MSG_DEBUG("Did not pass mass & chi2 & trk pT cuts ");
      } 
      
  }
  
  if ( thisPassedMassCut ) { 
    mon_cutCounter++; 
    if ( thisPassedChi2Cut ) { 
      mon_cutCounter++; 
      if ( thisPassedTrkPtCut ){
        mon_cutCounter++;
      }
    }
  }
  if ( thisPassedMassCut && thisPassedChi2Cut && thisPassedTrkPtCut ) { 
    result = true; 
    ATH_MSG_DEBUG("accepting event"); 
  }
  
  ATH_MSG_DEBUG("AcceptAll is set to : " << (m_acceptAll ? "True, taking all events " : "False, applying selection" ));
  // Accept-All mode: temporary patch; should be done with force-accept
  if (m_acceptAll) {
      return true;
  }
  
  return result;
}

StatusCode TrigMultiTrkHypoToolMT::inclusiveSelection(std::vector<TrigMultiTrkHypoToolMT::TrigMultiTrkInfo>& toolInput) const{

  for ( auto& i: toolInput ) {
   //should this be configured to do something slightly differently? maybe we don't want the previous decision to pass this way??
   ATH_MSG_DEBUG("what is this? " << m_decisionId.numeric()); //hashing of the chain name

   //Make sure that BOTH tracks come from a previous muon that passed (for this chain)
   //How could this not happen? no idea!
   if ( TrigCompositeUtils::passed( m_decisionId.numeric(), i.previousDecisionIDs0 ) &&
   		TrigCompositeUtils::passed( m_decisionId.numeric(), i.previousDecisionIDs1 ) ) {
         if ( decideOnSingleObject( i.trigBphys, 0 )==true ) {
            ATH_MSG_DEBUG("Passed through selection, decisionID:" << m_decisionId );
            TrigCompositeUtils::addDecisionID(m_decisionId, i.decision);
         } else {
            ATH_MSG_DEBUG("Didn't pass through selection, decisionID:" << m_decisionId );
         }
      } else {
         ATH_MSG_DEBUG("Didn't pass previous decision, decisionID:" << m_decisionId );
      }
  }
  return StatusCode::SUCCESS;
}

StatusCode TrigMultiTrkHypoToolMT::decide( std::vector<TrigMultiTrkHypoToolMT::TrigMultiTrkInfo> &input) const {

  if ( m_nBphysObjects == 1 ) {
    return inclusiveSelection( input );
  } else {
    ATH_MSG_WARNING("TrigMultiTrkHypoToolMT is not configured to do multiple dimuon objects selection.");
    ATH_MSG_WARNING("Clever overlap removal needed to ensure that dimuon objectss do not contain identical RoIs!");
    ATH_MSG_WARNING("I'm returning the inclusive selection result!");
    return inclusiveSelection( input );

  }

}

