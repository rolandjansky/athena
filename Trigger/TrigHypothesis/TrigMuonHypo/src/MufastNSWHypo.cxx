/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>

#include <vector>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "TrigMuonEvent/MuonFeature.h"
#include "TrigMuonEvent/MuonFeatureDetails.h"
#include "TrigMuonHypo/MufastNSWHypo.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

class ISvcLocator;

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

MufastNSWHypo::MufastNSWHypo(const std::string & name, ISvcLocator* pSvcLocator):
   HLT::HypoAlgo(name, pSvcLocator)
{
   declareProperty("AcceptAll",    m_acceptAll=false);

   declareMonitoredVariable("Pt",        m_fex_pt);
   declareMonitoredVariable("Eta",       m_fex_eta);
   declareMonitoredVariable("Phi",       m_fex_phi);
   declareMonitoredVariable("SegTgcInn", m_fex_tgcinn);
   declareMonitoredVariable("SegInn",    m_fex_inn);
   declareMonitoredVariable("SegMid",    m_fex_mid);
   declareMonitoredVariable("SegOut",    m_fex_out);
   declareMonitoredVariable("dTheta",    m_fex_dtheta);
   declareMonitoredVariable("dLEta",     m_fex_dleta);
   declareMonitoredVariable("dLPhi",     m_fex_dlphi);

   m_bins = 0;
}

MufastNSWHypo::~MufastNSWHypo(){
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

HLT::ErrorCode MufastNSWHypo::hltInitialize()
{
   msg() << MSG::INFO << "Initializing " << name() << " - package version " 
	 << PACKAGE_VERSION << endreq;

   if(m_acceptAll) {
      msg() << MSG::INFO << "Accepting all the events with not cut!" << endreq;
   }
 
   msg() << MSG::INFO << "Initialization completed successfully" << endreq;

   return HLT::OK;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

HLT::ErrorCode MufastNSWHypo::hltFinalize()
{
   msg() << MSG::INFO << "in finalize()" << endreq;
   return HLT::OK;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

HLT::ErrorCode MufastNSWHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass)
{
   pass = false;

   m_storeGate = store();

   bool doDebug =  msgLvl() <= MSG::DEBUG;
   
   if(doDebug) msg() << MSG::DEBUG << "in hltExecute : " << name() << endreq;
   
   // if accept All flag is on, just pass it
   if(m_acceptAll) {
      pass = true;
      if(doDebug) msg() << MSG::DEBUG << "Accept property is set: taking all the events" << endreq;
      return HLT::OK;
   }
  
   if(doDebug) msg() << MSG::DEBUG << "Accept property not set: applying selection!" << endreq;
  
   // Some debug output:
   if(doDebug) msg() << MSG::DEBUG << "outputTE->ID(): " << outputTE->getId() << endreq;
  
   // Get MuonFeature
   std::vector<const MuonFeature*> vectorOfMuons; 
   HLT::ErrorCode status = getFeatures(outputTE, vectorOfMuons);
   if(status!=HLT::OK) {
      msg() << MSG::DEBUG << "no MuonFeatures found" << endreq;
      return status;
   }
   if (vectorOfMuons.size() != 1){
      msg() << MSG::ERROR << "Size of vector is " << vectorOfMuons.size() << endreq;
      return HLT::ErrorCode(HLT::Action::CONTINUE,HLT::Reason::NAV_ERROR);
   }
   const MuonFeature* pMuon = vectorOfMuons.front();
   if(!pMuon){
      msg() << MSG::ERROR << "Retrieval of MuonFeature from vector failed" << endreq;
      return HLT::ErrorCode(HLT::Action::CONTINUE,HLT::Reason::NAV_ERROR);
   }

   // Get MuonFeatureDetails
   std::vector<const MuonFeatureDetails*> vectorOfMuonDetails; 
   status = getFeatures(outputTE, vectorOfMuonDetails);
   if(status!=HLT::OK) {
      msg() << MSG::DEBUG << "no MuonFeatureDetailss found" << endreq;
      return status;
   }
   if (vectorOfMuonDetails.size() != 1){
      msg() << MSG::ERROR << "Size of vector is " << vectorOfMuonDetails.size() << endreq;
      return HLT::ErrorCode(HLT::Action::CONTINUE,HLT::Reason::NAV_ERROR);
   }
   const MuonFeatureDetails* pMuonDetail = vectorOfMuonDetails.front();
   if(!pMuonDetail){
      msg() << MSG::ERROR << "Retrieval of MuonFeatureDetails from vector failed" << endreq;
      return HLT::ErrorCode(HLT::Action::CONTINUE,HLT::Reason::NAV_ERROR);
   }

   // 
   float pt_mf   = pMuon->pt();
   float eta_mf  = pMuon->eta();
   float phi_mf  = pMuon->phi();
   float eta_roi = pMuonDetail->roi_eta();
   float phi_roi = pMuonDetail->roi_phi();

   // fill Monitoring histos
   m_fex_pt  = pt_mf;
   m_fex_eta = eta_mf;
   m_fex_phi = phi_mf;

   // if outside NSW no cut at all
   const float ETA_NSW       = 1.3;
   const float ETA_MDT_INN   = 2.0;
   const float PT_ZERO_LIMIT = 0.1;
   float eta_switch = (fabs(pt_mf) < PT_ZERO_LIMIT) ? eta_roi : eta_mf;
   if( fabs(eta_switch) < ETA_NSW || fabs(eta_switch) > ETA_MDT_INN ) {
      pass = true;
      if (doDebug)
	 msg() << MSG::DEBUG << " REGTEST outside NSW: eta=" << eta_switch << " -> hypo=" << (pass?"true":"false") << endreq;
      return HLT::OK;
   }

   // reject if muFast reconstruction fails
   if( fabs(pt_mf) < PT_ZERO_LIMIT ) {
      pass = false;
      if (doDebug)
	 msg() << MSG::DEBUG << " REGTEST reconstruction failed: pt=" << pMuon->pt() << " -> hypo=" << (pass?"true":"false") << endreq;
      return HLT::OK;
   }

   // --------------
   // apply NSW cut
   // --------------
   
   float sp_r[4];
   float sp_z[4];
   float sp_slope[4];
   sp_r[0] = pMuonDetail->tgc_Inn_r(); sp_z[0] = pMuonDetail->tgc_Inn_z(); sp_slope[0] = 0;
   //
   sp_r[1] = pMuon->sp1_r(); sp_z[1] = pMuon->sp1_z(); sp_slope[1] = pMuon->sp1_slope();
   sp_r[2] = pMuon->sp2_r(); sp_z[2] = pMuon->sp2_z(); sp_slope[2] = pMuon->sp2_slope();
   sp_r[3] = pMuon->sp3_r(); sp_z[3] = pMuon->sp3_z(); sp_slope[3] = pMuon->sp3_slope();

   // msg() << MSG::DEBUG << " REGTEST sp_r/z[0]=" << sp_r[0] << "/" << sp_z[0] << endreq;
   // const std::vector<float>& tgc_inn_rho_hit_r = pMuonDetail->tgc_Inn_rho_hit_r();
   // msg() << MSG::DEBUG << " REGTEST size TgcInnRhoHitR=" << tgc_inn_rho_hit_r.size() << endreq;
   // msg() << MSG::DEBUG << " REGTEST mdt_Inner_R/Z=" << pMuonDetail->mdt_Inner_R() << "/" << pMuonDetail->mdt_Inner_Z() << endreq;
   // msg() << MSG::DEBUG << " REGTEST sp_r/z[1]=" << sp_r[1] << "/" << sp_z[1] << endreq;

   // 1. segment cut
   bool hasTgcInner = false;
   bool hasInner    = false;
   bool hasMiddle   = false;
   bool hasOuter    = false;
   const float RZ_ZERO_LIMIT = 0.01;
   const float INNER_Z_CUT   = 10000;
   const float MIDDLE_Z_CUT  = 16000;
   for(unsigned int i=0; i<4; i++) {
      float r = sp_r[i];
      float z = sp_z[i];
      if( r > RZ_ZERO_LIMIT && fabs(z) > RZ_ZERO_LIMIT ) {
	 if     ( fabs(z) < INNER_Z_CUT ) { (i==0) ? hasTgcInner=true : hasInner=true; }
	 else if( fabs(z) < MIDDLE_Z_CUT ){ hasMiddle=true; }
	 else { hasOuter=true; }
      }
   } 
   m_fex_tgcinn =  ( hasTgcInner ) ? 1.5 : 0.5;
   m_fex_inn    =  ( hasInner )    ? 1.5 : 0.5;
   m_fex_mid    =  ( hasMiddle )   ? 1.5 : 0.5;
   m_fex_out    =  ( hasOuter )    ? 1.5 : 0.5;

   if( ! hasInner && ! hasTgcInner ) {
      pass = false;
      if (doDebug)
	 msg() << MSG::DEBUG << " REGTEST no Inner hit: pt/eta/sp_z[0/1]=" << pt_mf << "/" << eta_mf << "/" << sp_z[0] << "/" << sp_z[1] << " -> hypo=" << (pass?"true":"false") << endreq;
      return HLT::OK;
   }

   // 2. dTheta cut
   double theta_slope = (hasInner) ? atan2(sp_slope[1],1) : 2*atan(exp(-pMuonDetail->tgc_Inn_eta()));
   if( theta_slope < 0 ) theta_slope += CLHEP::pi;
   double theta_point = (hasInner) ? atan2(sp_r[1],sp_z[1]) : atan2(pMuonDetail->tgc_Inn_r(),pMuonDetail->tgc_Inn_z());
   double dTheta = theta_slope - theta_point;
   m_fex_dtheta = dTheta;
   // msg() << MSG::DEBUG << " REGTEST sp_slope[1]=" << sp_slope[1] << endreq;
   // msg() << MSG::DEBUG << " REGTEST sp_r[1]=" << sp_r[1] << endreq;
   // msg() << MSG::DEBUG << " REGTEST sp_z[1]=" << sp_z[1] << endreq;
   // msg() << MSG::DEBUG << " REGTEST theta_slope=" << theta_slope << endreq;
   // msg() << MSG::DEBUG << " REGTEST theta_point=" << theta_point << endreq;
   if( ! dThetaCut(pMuon->eta(),dTheta) ) {
      pass = false;
      if (doDebug)
	 msg() << MSG::DEBUG << " REGTEST failed dTheta cut: dTheta=" << dTheta << " -> hypo=" << (pass?"true":"false") << endreq;
      return HLT::OK;
   }

   // 3. dL cut
   float eta_nsw = -log(tan(theta_point/2));
   float phi_nsw = (hasTgcInner) ? pMuonDetail->tgc_Inn_phi() : phi_roi;
   double dLeta  = fabs(eta_nsw - eta_roi);
   double dLphi  = fabs(phi_nsw - phi_roi);
   if( dLphi > CLHEP::pi ) dLphi = CLHEP::twopi - dLphi;
   m_fex_dleta = dLeta;
   m_fex_dlphi = dLphi;
   if( ! dLCut(pMuon->eta(),dLeta, dLphi) ) {
      pass = false;
      if (doDebug)
	 msg() << MSG::DEBUG << " REGTEST failed dL cut: dLeta/phi=" << dLeta << "/" << dLphi << " -> hypo=" << (pass?"true":"false") << endreq;
      return HLT::OK;
   }

   // passed all
   pass = true;
   if (doDebug) msg() << MSG::DEBUG << " REGTEST passed all NSW cuts" << endreq;

  
   return HLT::OK;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

bool MufastNSWHypo::dThetaCut(double eta, double dTheta)
{
   double feta    = fabs(eta);
   double fdtheta = fabs(dTheta);

   if( feta < 1.2 ) return true;

   bool retval = false;
   if( feta < 1.4 ) {
      if( fdtheta < 0.015 ) retval = true;
   }
   else if( feta < 1.6 ) {
      if( fdtheta < 0.015 ) retval = true;
   }
   else if( feta < 1.8 ) {
      if( fdtheta < 0.010 ) retval = true;
   }
   else if( feta < 2.0 ) {
      if( fdtheta < 0.025 ) retval = true;
   }
   else if( feta < 2.2 ) {
      if( fdtheta < 0.070 ) retval = true;
   }
   else {
      if( fdtheta < 0.070 ) retval = true;
   }
   
   return retval;
}

bool MufastNSWHypo::dLCut(double eta, double dLeta, double dLphi)
{
   double feta   = fabs(eta);
   double fdleta = fabs(dLeta);
   double fdlphi = fabs(dLphi);

   if( fdlphi > 0.06 ) return false;

   bool retval = false;
   if( feta < 2.0 ) {
      if( fdleta < 0.05 ) retval = true;
   }
   else {
      if( fdleta < 0.07 ) retval = true;
   }

   return retval;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
