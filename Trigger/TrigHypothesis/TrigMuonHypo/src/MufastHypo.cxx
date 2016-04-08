/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "xAODTrigMuon/L2StandAloneMuonContainer.h"
#include "TrigMuonHypo/MufastHypo.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

class ISvcLocator;

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

MufastHypo::MufastHypo(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator)
{
   std::vector<float> def_bins;
   def_bins.push_back(0);
   def_bins.push_back(2.5); 
   std::vector<float> def_thrs;
   def_thrs.push_back(5.49*CLHEP::GeV);

   declareProperty("PtBins",       m_ptBins=def_bins);
   declareProperty("PtThresholds", m_ptThresholds=def_thrs);
   declareProperty("AcceptAll",    m_acceptAll=false);
   declareProperty("SelectPV",     m_selectPV=false);
   declareProperty("Z_PV_Bins",    m_Z_PV=400.);
   declareProperty("R_PV_Bins",    m_R_PV=200.);

   declareProperty("PtThresholdForECWeakBRegionA", m_ptThresholdForECWeakBRegionA=3.);
   declareProperty("PtThresholdForECWeakBRegionB", m_ptThresholdForECWeakBRegionB=3.);

   declareMonitoredVariable("Pt",    m_fex_pt);
   declareMonitoredVariable("Eta",   m_fex_eta);
   declareMonitoredVariable("Phi",   m_fex_phi);

   declareMonitoredVariable("XatSt", m_x_at_station);
   declareMonitoredVariable("YatSt", m_y_at_station);
   declareMonitoredVariable("ZatSt", m_z_at_station);
   declareMonitoredVariable("XatBe", m_x_at_beam);
   declareMonitoredVariable("ZatBe", m_z_at_beam);

   m_bins = 0;
   m_storeGate = 0;
}

MufastHypo::~MufastHypo(){
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

HLT::ErrorCode MufastHypo::hltInitialize()
{
   msg() << MSG::INFO << "Initializing " << name() << " - package version " 
	 << PACKAGE_VERSION << endreq;

  if(m_acceptAll) {
      msg() << MSG::INFO << "Accepting all the events with not cut!" << endreq;
  }
  else {
     m_bins = m_ptBins.size() - 1;
     if (m_bins != m_ptThresholds.size()) {
	msg() << MSG::INFO << "bad thresholds setup .... exiting!" << endreq;
	return HLT::BAD_JOB_SETUP;
     }

      for (std::vector<float>::size_type i=0; i<m_bins;++i) {
	 char buf1[256];
	 char buf2[256];
	 sprintf(buf1,"%f5.2",m_ptBins[i]);
	 sprintf(buf2,"%f5.2",m_ptBins[i+1]);
	 msg() << MSG::INFO << "EtaBin " << buf1 << " - " <<  buf2
	       << ": with Pt Threshold of " << (m_ptThresholds[i])/CLHEP::GeV 
	       << " GeV" << endreq;
      }
      
      msg() << MSG::INFO << "Endcap WeakBField A: pT threshold of " << m_ptThresholdForECWeakBRegionA / CLHEP::GeV << " GeV" << endreq;
      msg() << MSG::INFO << "Endcap WeakBField B: pT threshold of " << m_ptThresholdForECWeakBRegionB / CLHEP::GeV << " GeV" << endreq;
  }
 
  msg() << MSG::INFO << "Initialization completed successfully" << endreq;

  return HLT::OK;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

HLT::ErrorCode MufastHypo::hltFinalize()
{
  msg() << MSG::INFO << "in finalize()" << endreq;
  return HLT::OK;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

HLT::ErrorCode MufastHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass)
{
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
  
   bool result = false;
  
   // Some debug output:
   if(doDebug) msg() << MSG::DEBUG << "outputTE->ID(): " << outputTE->getId() << endreq;
  
   // Get vector of pointers to L2StandAloneMuon linked to the outputTE 
   const xAOD::L2StandAloneMuonContainer* vectorOfMuons(0);
   HLT::ErrorCode status = getFeature(outputTE, vectorOfMuons);
   if (status!=HLT::OK) {
      msg() << MSG::DEBUG << "no L2StandAloneMuon found" << endreq;
      return status;
   }

   // Check that there is only one L2StandAloneMuon
   if (vectorOfMuons->size() != 1){
      msg() << MSG::ERROR << "Size of vector is " << vectorOfMuons->size() << endreq;
      return HLT::ErrorCode(HLT::Action::CONTINUE,HLT::Reason::NAV_ERROR);
   }

   // Get first (and only) RoI:
   const xAOD::L2StandAloneMuon* pMuon = vectorOfMuons->front();
   if(!pMuon){
      msg() << MSG::ERROR << "Retrieval of L2StandAloneMuon from vector failed" << endreq;
      return HLT::ErrorCode(HLT::Action::CONTINUE,HLT::Reason::NAV_ERROR);
   }

   // fill Monitoring histos
   m_fex_pt  = (pMuon->pt())?  pMuon->pt()  : -9999.;
   m_fex_eta = (pMuon->etaMS())? pMuon->etaMS() : -9999.;
   m_fex_phi = (pMuon->etaMS())? pMuon->phiMS() : -9999.;

   if( pMuon->etaMS() ) {
      float localPhi = getLocalPhi(pMuon->etaMS(),pMuon->phiMS(),pMuon->rMS());
      float radius = pMuon->rMS()/cos(fabs(localPhi));
      float DirZ = (pMuon->dirZMS())? pMuon->dirZMS() : .000001;
      float DirF = (pMuon->dirPhiMS())?  pMuon->dirPhiMS()  : .000001;
      m_x_at_station = radius * cos(pMuon->phiMS());
      m_y_at_station = radius * sin(pMuon->phiMS());
      m_z_at_station = pMuon->zMS();
      float xb = m_x_at_station - m_y_at_station/DirF;
      float de = m_x_at_station - xb;
      float ds = sqrt(m_y_at_station*m_y_at_station+de*de);
      m_x_at_beam = xb;
      m_z_at_beam = m_z_at_station - ds*DirZ;
   } else {
      m_x_at_station = -9999.;
      m_y_at_station = -9999.;
      m_z_at_station = -9999.;
      m_x_at_beam = -9999.;
      m_z_at_beam = -9999.;
   }

   //Get the Pt cut for that eta bin
   float threshold = 0;
   float absEta = fabs(m_fex_eta);
   for (std::vector<float>::size_type i=0; i<m_bins; ++i)
      if ( absEta > m_ptBins[i] && absEta < m_ptBins[i+1] ) threshold = m_ptThresholds[i]; 

   // if in the weak Bfield regions at endcap, set special threshold
   MufastHypoConsts::ECRegions ecRegion = whichECRegion( m_fex_eta, m_fex_phi );
   if( ecRegion == MufastHypoConsts::WeakBFieldA ) {
      if(doDebug) msg() << MSG::DEBUG << "threshold is set for EC WeakBField A" << endreq;
      threshold = m_ptThresholdForECWeakBRegionA;
   }
   if( ecRegion == MufastHypoConsts::WeakBFieldB ) {
      if(doDebug) msg() << MSG::DEBUG << "threshold is set for EC WeakBField B" << endreq;
      threshold = m_ptThresholdForECWeakBRegionB;
   }

   if(doDebug) msg() << MSG::DEBUG << "threshold value is set as:" << threshold/CLHEP::GeV << " GeV" << endreq;

   // Check pt threshold for hypothesis, 
   // convert units since Muonfeature is in GeV
   if ( std::abs(pMuon->pt()) > (threshold/CLHEP::GeV)){
      // selects only tracks coming from a region around PV
      if( m_selectPV ){
	 if((fabs(m_x_at_beam)<m_R_PV) && (fabs(m_z_at_beam)<m_Z_PV))
	    result = true;
      } else {
	 result = true;
      }
   }

   if (doDebug) {
      msg() << MSG::DEBUG << " REGTEST muon pt is " << pMuon->pt() << " GeV" 
            << " and threshold cut is " << threshold/CLHEP::GeV << " GeV" 
            << " so hypothesis is " << (result?"true":"false") << endreq;
   }
  
   //store the result
   pass = result;
  
   return HLT::OK;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

MufastHypoConsts::ECRegions MufastHypo::whichECRegion( const float eta, const float phi ) const
{
   float absEta = fabs(eta);

   if(      ( 1.3 <= absEta && absEta < 1.45) &&
            ( (0                 <= fabs(phi) && fabs(phi) < CLHEP::pi/48. )     ||
	      (CLHEP::pi*11./48. <= fabs(phi) && fabs(phi) < CLHEP::pi*13./48. ) ||
	      (CLHEP::pi*23./48. <= fabs(phi) && fabs(phi) < CLHEP::pi*25./48. ) ||
	      (CLHEP::pi*35./48. <= fabs(phi) && fabs(phi) < CLHEP::pi*37./48. ) ||
	      (CLHEP::pi*47./48. <= fabs(phi) && fabs(phi) < CLHEP::pi )
	  )
      ) return MufastHypoConsts::WeakBFieldA;
   
   else if( ( 1.5 <= absEta && absEta < 1.65 ) &&
	    ( (CLHEP::pi*3./32.  <= fabs(phi) && fabs(phi) < CLHEP::pi*5./32. ) ||
	      (CLHEP::pi*11./32. <= fabs(phi) && fabs(phi) < CLHEP::pi*13./32.) ||
	      (CLHEP::pi*19./32. <= fabs(phi) && fabs(phi) < CLHEP::pi*21./32.) ||
	      (CLHEP::pi*27./32. <= fabs(phi) && fabs(phi) < CLHEP::pi*29./32.)
	       )
      ) return MufastHypoConsts::WeakBFieldB;
   
   else return MufastHypoConsts::Bulk;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

float MufastHypo::getLocalPhi(float eta, float phi, float rad) const
{
   if(phi<0.) phi += 2*3.14159265;
   float step = 0.78539816;
   float offs = 0.39269908;
   if(fabs(eta) <= 1.1)
   {
      float Dphi = 999999.;
      float sign = 0.;
      const float ZERO_LIMIT = 1e-6;
      if(rad < 800.)
      {
         for(int i=0;i<8;++i) if(fabs(i*step-phi)<=Dphi)
	 { 
	    Dphi=fabs(i*step-phi);
	    sign = (fabs(Dphi) > ZERO_LIMIT) ? (i*step-phi)/fabs(i*step-phi) : 0;
	 }
	 return sign*Dphi;
      }else
      {
         for(int i=1;i<8;++i) if(fabs(i*step+offs-phi)<=Dphi)
	 { 
	    Dphi=fabs(i*step+offs-phi);
	    sign = (fabs(Dphi) > ZERO_LIMIT) ? (i*step+offs-phi)/fabs(i*step+offs-phi) : 0;
	 }     
	 return sign*Dphi;
      }
   }else
   {
      return 0.;
   }
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
