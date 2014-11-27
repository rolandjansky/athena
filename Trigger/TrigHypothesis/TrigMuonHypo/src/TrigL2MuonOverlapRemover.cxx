/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <vector>
#include <string>
#include <cmath>

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include "TrigConfHLTData/HLTTriggerElement.h"
#include "TrigT1Interfaces/RecMuonRoI.h"

#include "TrigMuonHypo/TrigL2MuonOverlapRemover.h"

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonOverlapRemover::TrigL2MuonOverlapRemover(const std::string& name, ISvcLocator* pSvcLocator)
   : HLT::AllTEAlgo(name, pSvcLocator)
{
   //
   m_doMonitor = false;
   m_doDebug   = false;

   // Mufast based cuts
   declareProperty("DoMufastBasedRemoval",   m_doMufastBasedRemoval   = true);
   declareProperty("MufastRequireDR",        m_muFastRequireDR        = true);
   declareProperty("MufastRequireMass",      m_muFastRequireMass      = true);
   declareProperty("MufastRequireSameSign",  m_muFastRequireSameSign  = true);
   // BB
   declareProperty("MufastDRThresBB",        m_muFastDRThresBB  =1e-3);
   declareProperty("MufastMassThresBB",      m_muFastMassThresBB=1e-3);
   // BE
   declareProperty("MufastDRThresBE",        m_muFastDRThresBE  =1e-3);
   declareProperty("MufastMassThresBE",      m_muFastMassThresBE=1e-3);
   // EC
   std::vector<double> def_muFastEtaBinsEC;
   def_muFastEtaBinsEC.push_back(0);
   def_muFastEtaBinsEC.push_back(9.9);
   declareProperty("MufastEtaBinsEC",        m_muFastEtaBinsEC=def_muFastEtaBinsEC);
   std::vector<double> def_muFastDRThresEC;
   def_muFastDRThresEC.push_back(1e-3);
   declareProperty("MufastDRThresEC",        m_muFastDRThresEC=def_muFastDRThresEC);
   std::vector<double> def_muFastMassThresEC;
   def_muFastMassThresEC.push_back(1e-3);
   declareProperty("MufastMassThresEC",      m_muFastMassThresEC=def_muFastMassThresEC);

   // Mucomb based cuts
   declareProperty("DoMucombBasedRemoval",   m_doMucombBasedRemoval   = true);
   declareProperty("MucombRequireDR",        m_muCombRequireDR        = true);
   declareProperty("MucombRequireMufastDR",  m_muCombRequireMufastDR  = true);
   declareProperty("MucombRequireMass",      m_muCombRequireMass      = true);
   declareProperty("MucombRequireSameSign",  m_muCombRequireSameSign  = true);
   std::vector<double> def_muCombEtaBins;
   def_muCombEtaBins.push_back(0);
   def_muCombEtaBins.push_back(9.9);
   declareProperty("MucombEtaBins",          m_muCombEtaBins=def_muCombEtaBins);
   std::vector<double> def_muCombDRThres;
   def_muCombDRThres.push_back(1e-3);
   declareProperty("MucombDRThres",          m_muCombDRThres=def_muCombDRThres);
   std::vector<double> def_muCombMufastDRThres;
   def_muCombMufastDRThres.push_back(0.5);
   declareProperty("MucombMufastDRThres",    m_muCombMufastDRThres=def_muCombMufastDRThres);
   std::vector<double> def_muCombMassThres;
   def_muCombMassThres.push_back(1e-3);
   declareProperty("MucombMassThres",        m_muCombMassThres=def_muCombMassThres);

   // Monitor choice
   // declareProperty("MonitorChoice",  m_monitorChoice = TrigL2MuonOverlapRemoverConsts::monitorChoice_mu10_only);
   declareProperty("MonitorChoice",  m_monitorChoice = TrigL2MuonOverlapRemoverConsts::monitorChoice_all);

   // monitoring variables
   declareMonitoredVariable("MufastNrAllTEs",           m_mnt_mufastNrAllTEs);
   declareMonitoredVariable("MufastNrOverlapped",       m_mnt_mufastNrOverlapped);
   declareMonitoredVariable("MufastNrActiveTEs",        m_mnt_mufastNrActiveTEs);
   declareMonitoredStdContainer("MufastOverlappedPt",   m_mnt_mufastOverlappedPt);
   declareMonitoredStdContainer("MufastOverlappedEta",  m_mnt_mufastOverlappedEta);
   declareMonitoredStdContainer("MufastOverlappedPhi",  m_mnt_mufastOverlappedPhi);
   declareMonitoredStdContainer("MufastError",          m_mnt_mufastError);
   declareMonitoredStdContainer("MufastDR",             m_mnt_mufastDR);
   declareMonitoredStdContainer("MufastMass",           m_mnt_mufastMass);
   declareMonitoredStdContainer("MufastDRLog10",        m_mnt_mufastDRLog10);
   declareMonitoredStdContainer("MufastMassLog10",      m_mnt_mufastMassLog10);

   declareMonitoredVariable("MucombNrAllTEs",           m_mnt_mucombNrAllTEs);
   declareMonitoredVariable("MucombNrOverlapped",       m_mnt_mucombNrOverlapped);
   declareMonitoredVariable("MucombNrActiveTEs",        m_mnt_mucombNrActiveTEs);
   declareMonitoredStdContainer("MucombOverlappedPt",   m_mnt_mucombOverlappedPt);
   declareMonitoredStdContainer("MucombOverlappedEta",  m_mnt_mucombOverlappedEta);
   declareMonitoredStdContainer("MucombOverlappedPhi",  m_mnt_mucombOverlappedPhi);
   declareMonitoredStdContainer("MucombError",          m_mnt_mucombError);
   declareMonitoredStdContainer("MucombDR",             m_mnt_mucombDR);
   declareMonitoredStdContainer("MucombMass",           m_mnt_mucombMass);
   declareMonitoredStdContainer("MucombDRLog10",        m_mnt_mucombDRLog10);
   declareMonitoredStdContainer("MucombMassLog10",      m_mnt_mucombMassLog10);
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

HLT::ErrorCode TrigL2MuonOverlapRemover::hltBeginRun()
{
   msg() << MSG::INFO << "in hltBeginRun : " << name() << endreq;

   //
   m_doDebug   =  msgLvl() <= MSG::DEBUG;
   msg() << MSG::INFO << "m_doDebug=" << m_doDebug << endreq;   

   //
   if( m_doMufastBasedRemoval ) {
      msg() << MSG::INFO << "--- muFast based overlap removal as: ---"     << endreq;
      if( m_muFastRequireDR ) {
	 msg() << MSG::INFO << "+ dR cut:" << endreq;
	 if( (m_muFastEtaBinsEC.size()-1) != m_muFastDRThresEC.size() ) {
	    msg() << MSG::INFO << "bad thresholds setup .... exiting!" << endreq;
	    return HLT::BAD_JOB_SETUP;
	 }
	 msg() << MSG::INFO << "     B-B : dR < " << m_muFastDRThresBB << endreq;
	 msg() << MSG::INFO << "     B-E : dR < " << m_muFastDRThresBE << endreq;
	 msg() << MSG::INFO << "     E-E : " << endreq; 
	 for(unsigned int i=0; i<m_muFastDRThresEC.size(); i++) {
	    msg() << MSG::INFO << "        EtaBin " << m_muFastEtaBinsEC[i] << " - " << m_muFastEtaBinsEC[i+1]
		  << " : dR < " << m_muFastDRThresEC[i] << endreq;
	 }
      }
      if( m_muFastRequireMass ) {
	 msg() << MSG::INFO << "+ Mass cut:" << endreq;
	 if( (m_muFastEtaBinsEC.size()-1) != m_muFastMassThresEC.size() ) {
	    msg() << MSG::INFO << "bad thresholds setup .... exiting!" << endreq;
	    return HLT::BAD_JOB_SETUP;
	 }
	 msg() << MSG::INFO << "     B-B : Mass < " << m_muFastMassThresBB << endreq;
	 msg() << MSG::INFO << "     B-E : Mass < " << m_muFastMassThresBE << endreq;
	 msg() << MSG::INFO << "     E-E : " << endreq; 
	 for(unsigned int i=0; i<m_muFastMassThresEC.size(); i++) {
	    msg() << MSG::INFO << "        EtaBin " << m_muFastEtaBinsEC[i] << " - " << m_muFastEtaBinsEC[i+1]
		  << " : Mass < " << m_muFastMassThresEC[i] << endreq;
	 }
      }
      if( m_muFastRequireSameSign ) msg() << MSG::INFO << "+ Same charge sign" << endreq;
   }

   //
   if( m_doMucombBasedRemoval ) {
      msg() << MSG::INFO << "--- muComb based overlap removal as: ---"     << endreq;
      if( m_muCombRequireDR ) {
	 msg() << MSG::INFO << "+ dR cut:" << endreq;
	 if( (m_muCombEtaBins.size()-1) != m_muCombDRThres.size() ) {
	    msg() << MSG::INFO << "bad thresholds setup .... exiting!" << endreq;
	    return HLT::BAD_JOB_SETUP;
	 }
	 for(unsigned int i=0; i<m_muCombDRThres.size(); i++) {
	    msg() << MSG::INFO << "     EtaBin " << m_muCombEtaBins[i] << " - " << m_muCombEtaBins[i+1]
		  << " : dR < " << m_muCombDRThres[i] << endreq;
	 }
      }
      if( m_muCombRequireMufastDR ) {
	 msg() << MSG::INFO << "+ dr(by mF) cut:" << endreq;
	 if( (m_muCombEtaBins.size()-1) != m_muCombMufastDRThres.size() ) {
	    msg() << MSG::INFO << "bad thresholds setup .... exiting!" << endreq;
	    return HLT::BAD_JOB_SETUP;
	 }
	 for(unsigned int i=0; i<m_muCombMufastDRThres.size(); i++) {
	    msg() << MSG::INFO << "     EtaBin " << m_muCombEtaBins[i] << " - " << m_muCombEtaBins[i+1]
		  << " : dR(mF) < " << m_muCombMufastDRThres[i] << endreq;
	 }
      }
      if( m_muCombRequireMass ) {
	 msg() << MSG::INFO << "+ Mass cut:" << endreq;
	 if( (m_muCombEtaBins.size()-1) != m_muCombMassThres.size() ) {
	    msg() << MSG::INFO << "bad thresholds setup .... exiting!" << endreq;
	    return HLT::BAD_JOB_SETUP;
	 }
	 for(unsigned int i=0; i<m_muCombMassThres.size(); i++) {
	    msg() << MSG::INFO << "     EtaBin " << m_muCombEtaBins[i] << " - " << m_muCombEtaBins[i+1]
		  << " : Mass < " << m_muCombMassThres[i] << endreq;
	 }
      }
      if( m_muCombRequireSameSign ) msg() << MSG::INFO << "+ Same charge sign" << endreq;
   }
   
   //
   msg() << MSG::INFO << "Initialization completed successfully"  << endreq; 

   return HLT::OK;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

bool TrigL2MuonOverlapRemover::isOverlap(const ElementLink<xAOD::L2StandAloneMuonContainer>& mfLink1,
					 const ElementLink<xAOD::L2StandAloneMuonContainer>& mfLink2)
{

   if( ! mfLink1.isValid() ) {
      msg() << MSG::INFO << "L2StandAloneMuonContainer does not contain object (#1)" << endreq;
      if(m_doDebug) msg() << MSG::DEBUG << "   ...-> mF link (#1) broken. cannot judge overlap -> return with false" << endreq;
      if(m_doMonitor) m_mnt_mufastError.push_back(TrigL2MuonOverlapRemoverConsts::errorCode_EL_not_valid);
      return false;
   }
   if( ! mfLink2.isValid() ) {
      msg() << MSG::INFO << "L2StandAloneMuonContainer does not contain object (#2)" << endreq;
      if(m_doDebug) msg() << MSG::DEBUG << "   ...-> mF link (#2) broken. cannot judge overlap -> return with false" << endreq;
      if(m_doMonitor) m_mnt_mufastError.push_back(TrigL2MuonOverlapRemoverConsts::errorCode_EL_not_valid);
      return false;
   }

   const xAOD::L2StandAloneMuon *mf1 = *mfLink1;
   const xAOD::L2StandAloneMuon *mf2 = *mfLink2;

   if(m_doDebug) msg() << MSG::DEBUG << "   ...mF1: pt/eta/phi=" << mf1->pt() << " / " << mf1->etaMS() << " / " << mf1->phiMS() << endreq;
   if(m_doDebug) msg() << MSG::DEBUG << "   ...mF2: pt/eta/phi=" << mf2->pt() << " / " << mf2->etaMS() << " / " << mf2->phiMS() << endreq;

   // if dR or invMass is necessary but (eta,phi) info is not avaiable
   // (i.e. eta,phi=0,0; rec failed)
   const double ZERO_LIMIT_FOR_ETAPHI = 1e-4;
   if( (fabs(mf1->etaMS()) <ZERO_LIMIT_FOR_ETAPHI && fabs(mf1->phiMS()) < ZERO_LIMIT_FOR_ETAPHI) ||
       (fabs(mf2->etaMS()) <ZERO_LIMIT_FOR_ETAPHI && fabs(mf2->phiMS()) < ZERO_LIMIT_FOR_ETAPHI) ) {
      if(m_doDebug) msg() << MSG::DEBUG << "   ...-> (eta,phi) info not available (rec at (eta,phi)=(0,0))" << endreq;
      if( m_muFastRequireDR || m_muFastRequireMass ) {
	 if(m_doDebug) msg() << MSG::DEBUG << "   ...-> but dR of invMass check is required. cannot judge overlap -> return with false" << endreq;
	 return false;
      }
   }

   // if charge or invMass is necessary but charge(=pT) info is not avaiable
   const double ZERO_LIMIT_FOR_PT = 1e-4;
   if( (fabs(mf1->pt()) <ZERO_LIMIT_FOR_PT) || (fabs(mf2->pt()) < ZERO_LIMIT_FOR_PT) ) {
      if(m_doDebug) msg() << MSG::DEBUG << "   ...-> pT info not available (rec at pT=0)" << endreq;
      if( m_muFastRequireSameSign || m_muFastRequireMass ) {
	 if(m_doDebug) msg() << MSG::DEBUG << "   ...-> but same sign or invMass check is required. cannot judge overlap -> return with false" << endreq;
	 return false;
      }
   }

   // determine dR, mass threshold separately for: BB, BE, EE
   double dRThres   = 9999;
   double massThres = 9999;

   if( m_muFastRequireDR || m_muFastRequireMass ) {
      const int SADDRESS_EC = -1;
      bool isBarrel1 = (mf1->sAddress() != SADDRESS_EC ) ? true : false;
      bool isBarrel2 = (mf2->sAddress() != SADDRESS_EC ) ? true : false;
      if(  isBarrel1 && isBarrel2 ) { // BB
	 if(m_doDebug)  msg() << MSG::DEBUG << "   ...B-B" << endreq;
	 dRThres  =m_muFastDRThresBB; 
	 massThres=m_muFastMassThresBB; 
      }
      else if( (isBarrel1 && ! isBarrel2) || (!isBarrel1 && isBarrel2) ) { // BE
	 if(m_doDebug)  msg() << MSG::DEBUG << "   ...B-E" << endreq;
	 dRThres  =m_muFastDRThresBE;
	 massThres=m_muFastMassThresBE;
      }
      else { // EE
	 if(m_doDebug)  msg() << MSG::DEBUG << "   ...E-E" << endreq;
	 double absEta = (fabs(mf1->pt()) > fabs(mf2->pt())) ? fabs(mf1->etaMS()) : fabs(mf2->etaMS());
	 unsigned int iThres=0;
	 for(unsigned int i=0; i<(m_muFastEtaBinsEC.size()-1); i++) {
	    if ( m_muFastEtaBinsEC[i] <= absEta && absEta < m_muFastEtaBinsEC[i+1] ) iThres = i;
	 }
	 if(m_doDebug)  msg() << MSG::DEBUG << "   ...iThres=" << iThres << endreq;
	 dRThres   = m_muFastDRThresEC[iThres];
	 massThres = m_muFastMassThresEC[iThres];
      }
      if(m_doDebug && m_muFastRequireDR)   msg() << MSG::DEBUG << "   ...dR   threshold=" << dRThres << endreq;
      if(m_doDebug && m_muFastRequireMass) msg() << MSG::DEBUG << "   ...mass threshold=" << massThres << endreq;
   }
 
   // same sign cut
   bool sameSign = false;
   if( m_muFastRequireSameSign ) {
      sameSign = ((mf1->pt()*mf2->pt()) > 0) ? true : false;
      if(m_doDebug) msg() << MSG::DEBUG << "   ...-> sameSign=" << sameSign << endreq;
   }

   // dR cut
   bool dRisClose = false;
   double dr = dR(mf1->etaMS(),mf1->phiMS(),mf2->etaMS(),mf2->phiMS());
   if(m_doMonitor) {
      m_mnt_mufastDR.push_back(dr);
      const double monitor_limit = 1e-4;
      double dr_mon = (dr>=monitor_limit) ? dr : monitor_limit;
      m_mnt_mufastDRLog10.push_back(log10(dr_mon));
   }
   if( m_muFastRequireDR ) {
      if( dr < dRThres ) dRisClose = true;
      if(m_doDebug) msg() << MSG::DEBUG << "   ...-> dR=" << dr << " : dRisClose=" << dRisClose << endreq;
   }

   // mass cut
   const double TRACK_MASS = 0;  // just assume zero mass
   bool massIsClose = false;
   double mass = invMass(TRACK_MASS,mf1->pt(),mf1->etaMS(),mf1->phiMS(),TRACK_MASS,mf2->pt(),mf2->etaMS(),mf2->phiMS());
   if(m_doMonitor) {
      m_mnt_mufastMass.push_back(mass);
      const double monitor_limit = 1e-4;
      double mass_mon = (mass>=monitor_limit) ? mass : monitor_limit;
      m_mnt_mufastMassLog10.push_back(log10(mass_mon));
   }
   if( m_muFastRequireMass ) {
      if( mass < massThres ) massIsClose = true;
      if(m_doDebug) msg() << MSG::DEBUG << "   ...-> mass=" << mass << " : massIsClose=" << massIsClose << endreq;
  }

   // total judge
   bool overlap = false;
   if( ((m_muFastRequireSameSign &&   sameSign)   || (! m_muFastRequireSameSign)) &&
       ((m_muFastRequireDR       &&  dRisClose)   || (! m_muFastRequireDR))       &&
       ((m_muFastRequireMass     &&  massIsClose) || (! m_muFastRequireMass)) ) {
      overlap = true;
   }

   if(m_doDebug) msg() << MSG::DEBUG << "   ...=> isOverlap=" << overlap << endreq;

   return overlap;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

bool TrigL2MuonOverlapRemover::isOverlap(const ElementLink<xAOD::L2CombinedMuonContainer>& combMfLink1,
					 const ElementLink<xAOD::L2CombinedMuonContainer>& combMfLink2)
{
   if( combMfLink1.isValid() == 0) {
      msg() << MSG::INFO << "L2SConminedMuonContainer does not contain object (#1)" << endreq;
      if(m_doDebug) msg() << MSG::DEBUG << "   ...-> mF link (#1) broken. cannot judge overlap -> return with false" << endreq;
      if(m_doMonitor) m_mnt_mufastError.push_back(TrigL2MuonOverlapRemoverConsts::errorCode_EL_not_valid);
      return false;
   }
   if( combMfLink2.isValid() == 0) {
      msg() << MSG::INFO << "L2CombinedMuonContainer does not contain object (#2)" << endreq;
      if(m_doDebug) msg() << MSG::DEBUG << "   ...-> mF link (#2) broken. cannot judge overlap -> return with false" << endreq;
      if(m_doMonitor) m_mnt_mufastError.push_back(TrigL2MuonOverlapRemoverConsts::errorCode_EL_not_valid);
      return false;
   }

   const xAOD::L2CombinedMuon *combMf1 = *combMfLink1;
   const xAOD::L2CombinedMuon *combMf2 = *combMfLink2;

   if(m_doDebug) msg() << MSG::DEBUG << "   ...combMF1: pt/eta/phi=" << combMf1->pt()/CLHEP::GeV << " / " << combMf1->eta() << " / " << combMf1->phi() << endreq;
   if(m_doDebug) msg() << MSG::DEBUG << "   ...combMF2: pt/eta/phi=" << combMf2->pt()/CLHEP::GeV << " / " << combMf2->eta() << " / " << combMf2->phi() << endreq;

   // if dR or invMass is necessary but (eta,phi) info is not avaiable
   // (i.e. eta,phi=0,0; rec failed)
   const double ZERO_LIMIT_FOR_ETAPHI = 1e-4;
   if( (fabs(combMf1->eta()) <ZERO_LIMIT_FOR_ETAPHI && fabs(combMf1->phi()) < ZERO_LIMIT_FOR_ETAPHI) ||
       (fabs(combMf2->eta()) <ZERO_LIMIT_FOR_ETAPHI && fabs(combMf2->phi()) < ZERO_LIMIT_FOR_ETAPHI) ) {
      if(m_doDebug) msg() << MSG::DEBUG << "   ...-> (eta,phi) info not available (rec at (eta,phi)=(0,0))" << endreq;
      if( m_muCombRequireDR || m_muCombRequireMass ) {
	 if(m_doDebug) msg() << MSG::DEBUG << "   ...-> but dR of invMass check is required. cannot judge overlap -> return with false" << endreq;
	 return false;
      }
   }

   // if charge or invMass is necessary but charge(=pT) info is not avaiable
   const double ZERO_LIMIT_FOR_PT = 1e-4;
   if( (fabs(combMf1->pt()) <ZERO_LIMIT_FOR_PT) || (fabs(combMf2->pt()) < ZERO_LIMIT_FOR_PT) ) {
      if(m_doDebug) msg() << MSG::DEBUG << "   ...-> pT info not available (rec at pT=0)" << endreq;
      if( m_muCombRequireSameSign || m_muCombRequireMass ) {
	 if(m_doDebug) msg() << MSG::DEBUG << "   ...-> but same sign or invMass check is required. cannot judge overlap -> return with false" << endreq;
	 return false;
      }
   }

   // determine etabin and thresholds
   double absEta = (fabs(combMf1->pt()) > fabs(combMf2->pt())) ? fabs(combMf1->eta()) : fabs(combMf2->eta());
   unsigned int iThres = 0;
   for(unsigned int i=0; i<(m_muCombEtaBins.size()-1); i++) {
      if ( m_muCombEtaBins[i] <= absEta && absEta < m_muCombEtaBins[i+1] ) iThres = i;
   }
   double dRThres     = m_muCombDRThres[iThres];
   double dRbyMFThres = m_muCombMufastDRThres[iThres];
   double massThres   = m_muCombMassThres[iThres];
   if(m_doDebug)  {
      msg() << MSG::DEBUG << "   ...iThres=" << iThres << endreq;
      if(m_muCombRequireDR)        msg() << MSG::DEBUG << "   ...dR       threshold=" << dRThres     << endreq;
      if(m_muCombRequireMufastDR)  msg() << MSG::DEBUG << "   ...dR(byMF) threshold=" << dRbyMFThres << endreq;
      if(m_muCombRequireMass)      msg() << MSG::DEBUG << "   ...mass     threshold=" << massThres   << endreq;
   }

   // same sign cut
   bool sameSign = false;
   if( m_muCombRequireSameSign ) {
      sameSign = ((combMf1->pt()*combMf2->pt()) > 0) ? true : false;
      if(m_doDebug) msg() << MSG::DEBUG << "   ...-> sameSign=" << sameSign << endreq;
   }

   // dR cut
   bool dRisClose = false;
   double dr = dR(combMf1->eta(),combMf1->phi(),combMf2->eta(),combMf2->phi());
   if(m_doMonitor) {
      m_mnt_mucombDR.push_back(dr);
      const double monitor_limit = 1e-4;
      double dr_mon = (dr>=monitor_limit) ? dr : monitor_limit;
      m_mnt_mucombDRLog10.push_back(log10(dr_mon));
   }
   if( m_muCombRequireDR ) {
      if( dr < dRThres ) dRisClose = true;
      if(m_doDebug) msg() << MSG::DEBUG << "   ...-> dR=" << dr << " : dRisClose=" << dRisClose << endreq;
   }

   // dR(by MF) cut
   bool dRbyMFisClose = false;
   if( m_muCombRequireMufastDR ) {
     const xAOD::L2StandAloneMuon* mf1 = combMf1->muSATrack();
     const xAOD::L2StandAloneMuon* mf2 = combMf2->muSATrack();
     if( mf1 == 0 || mf2 == 0 ) {
       msg() << MSG::INFO << "mF link from combinedMF broken" << endreq;
       if(m_doDebug) msg() << MSG::DEBUG << "   ...-> mF dR is required but mF link broken. cannot judge overlap -> return with false" << endreq;
       return false;
     }
     else {
       // here, we do not check (eta,phi) of mF is not (0,0)
       // (i.e. we apply muComb based cut even if muFast rec is failed)
       double dRByMF = dR(mf1->etaMS(),mf1->phiMS(),mf2->etaMS(),mf2->phiMS());
       if( dRByMF < dRbyMFThres ) dRbyMFisClose = true;
       if(m_doDebug) msg() << MSG::DEBUG << "   ...-> dR(by MF)=" << dRByMF << " : dRbyMFisClose=" << dRbyMFisClose << endreq;
     }
   }
   
   // mass cut
   const double TRACK_MASS = 0;  // just assume zero mass
   bool massIsClose = false;
   double mass = invMass(TRACK_MASS,combMf1->pt()/CLHEP::GeV,combMf1->eta(),combMf1->phi(),TRACK_MASS,combMf2->pt()/CLHEP::GeV,combMf2->eta(),combMf2->phi());
   if(m_doMonitor) {
      m_mnt_mucombMass.push_back(mass);
      const double monitor_limit = 1e-4;
      double mass_mon = (mass>=monitor_limit) ? mass : monitor_limit;
      m_mnt_mucombMassLog10.push_back(log10(mass_mon));
   }
   if( m_muCombRequireMass ) {
      if( mass < massThres ) massIsClose = true;
      if(m_doDebug) msg() << MSG::DEBUG << "   ...-> mass=" << mass << " : massIsClose=" << massIsClose << endreq;
  }

   // total judge
   bool overlap = false;
   if( ((m_muCombRequireSameSign &&   sameSign)     || (! m_muCombRequireSameSign)) &&
       ((m_muCombRequireDR       &&  dRisClose)     || (! m_muCombRequireDR))       &&
       ((m_muCombRequireMufastDR &&  dRbyMFisClose) || (! m_muCombRequireMufastDR)) &&
       ((m_muCombRequireMass     &&  massIsClose)   || (! m_muCombRequireMass)) ) {
      overlap = true; 
   }

   if(m_doDebug) msg() << MSG::DEBUG << "   ...=> isOverlap=" << overlap << endreq;

   return overlap;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

double TrigL2MuonOverlapRemover::dR(double eta1, double phi1, double eta2, double phi2)
{
   double deta = eta1 - eta2;
   double dphi = fabs(phi1 - phi2);
   if( dphi > CLHEP::pi ) dphi = CLHEP::twopi - dphi;
   double dR = pow( (deta*deta + dphi*dphi), 0.5 );
   return dR;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

double TrigL2MuonOverlapRemover::invMass(double m1, double pt1, double eta1, double phi1,
					 double m2, double pt2, double eta2, double phi2)
{
   const double ZERO_LIMIT = 1e-12;

   double theta1 = 2*atan2((double)exp(-eta1),1.);
   double theta2 = 2*atan2((double)exp(-eta2),1.);

   double fpt1   = fabs(pt1);
   double fpt2   = fabs(pt2);

   double px1    = fpt1*cos(phi1);
   double py1    = fpt1*sin(phi1);
   double pz1    = fpt1/tan(theta1);
   double  e1    = sqrt(px1*px1+py1*py1+pz1*pz1+m1*m1);

   double px2    = fpt2*cos(phi2);
   double py2    = fpt2*sin(phi2);
   double pz2    = fpt2/tan(theta2); 
   double  e2    = sqrt(px2*px2+py2*py2+pz2*pz2+m2*m2); 

   double pxsum  = px1 + px2;
   double pysum  = py1 + py2;
   double pzsum  = pz1 + pz2;
   double esum   =  e1 +  e2; 

   double mass  = 0;
   double mass2 = esum*esum - pxsum*pxsum - pysum*pysum - pzsum*pzsum;
   if( mass2 > ZERO_LIMIT ) mass = sqrt(mass2);

   return mass;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

HLT::ErrorCode TrigL2MuonOverlapRemover::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& tes_in,
						    unsigned int type_out)
{
   if(m_doDebug) msg() << MSG::DEBUG << "in hltExecute : " << name() << endreq;

   m_doMonitor = (m_monitorChoice==TrigL2MuonOverlapRemoverConsts::monitorChoice_all) ? true : false; 

   // ---
   // collect all TEs and create output TEs with ActiveState=true
   // ---

   std::vector<HLT::TriggerElement*> vec_allTEs;
   std::vector<HLT::TriggerElement*> vec_outputTEs;

   unsigned int tes_in_size = tes_in.size();

   unsigned int i_te;
   unsigned int j_te;
   
   if(m_doDebug) msg() << MSG::DEBUG << "tes_in size=" << tes_in_size << endreq;
   for(unsigned int i_vec=0; i_vec<tes_in_size; i_vec++) {
      unsigned int n_te = tes_in[i_vec].size();
      if(m_doDebug) msg() << MSG::DEBUG << "i_vec=" << i_vec << " : n TEs=" << n_te << endreq;
      for(i_te=0; i_te<n_te; i_te++) {
	 HLT::TriggerElement* te = tes_in[i_vec][i_te];
	 if( m_monitorChoice == TrigL2MuonOverlapRemoverConsts::monitorChoice_mu10_only || m_doDebug ) {
	    std::string label;
	    TrigConf::HLTTriggerElement::getLabel (te->getId(), label );
	    if( m_monitorChoice == TrigL2MuonOverlapRemoverConsts::monitorChoice_mu10_only && label.find("mu10") != std::string::npos ) m_doMonitor = true;
	    if(m_doDebug) msg() << MSG::DEBUG << "input TE ID(): " << te->getId() << ", Label=" << label << endreq;
	 }	
	 bool alreadyThere = false;
	 for(unsigned int j_te=0; j_te<vec_allTEs.size(); j_te++) {
	    if( vec_allTEs[j_te] == te ) {
	       if(m_doDebug) msg() << MSG::DEBUG << "at i_vec=" << i_vec << ", i_te=" << i_te <<
		  ": same TE already there at j_te=" << j_te << ", so, skip this TE." << endreq;
	       alreadyThere = true;
	       break;
	    }
	 }
	 if( ! alreadyThere) vec_allTEs.push_back(te);
      }
   }

   unsigned int n_allTEs = vec_allTEs.size(); 
   if(m_doDebug) msg() << MSG::DEBUG << "size of vec_allTEs=" << n_allTEs << endreq;

   for(i_te=0; i_te<n_allTEs; i_te++) {
      HLT::TEVec tes;
      tes.push_back(vec_allTEs[i_te]);
      HLT::TriggerElement* outputTE = config()->getNavigation()->addNode(tes, type_out);
      if(m_doDebug) {
	 std::string label;
	 TrigConf::HLTTriggerElement::getLabel (outputTE->getId(), label);
	 msg() << MSG::DEBUG << "creating outputTE ID(): " << outputTE->getId() << ", Label=" << label << endreq;
      }
      outputTE->setActiveState(true);
      vec_outputTEs.push_back(outputTE);
   }

   // ---
   // start monitoring
   // ---

   if(m_doDebug) msg() << MSG::DEBUG << "m_doMonitor=" << m_doMonitor << endreq;   

   if(m_doMonitor) {
      // reset
      if( m_doMufastBasedRemoval ) {
	 m_mnt_mufastNrAllTEs     = 0;
	 m_mnt_mufastNrOverlapped = 0;
	 m_mnt_mufastNrActiveTEs  = 0;
	 m_mnt_mufastOverlappedPt.clear();
	 m_mnt_mufastOverlappedEta.clear();
	 m_mnt_mufastOverlappedPhi.clear();
	 m_mnt_mufastError.clear();
	 m_mnt_mufastDR.clear();
	 m_mnt_mufastMass.clear();
	 m_mnt_mufastDRLog10.clear();
	 m_mnt_mufastMassLog10.clear();
      }
      if( m_doMucombBasedRemoval ) {
	 m_mnt_mucombNrAllTEs     = 0;
	 m_mnt_mucombNrOverlapped = 0;
	 m_mnt_mucombNrActiveTEs  = 0;
	 m_mnt_mucombOverlappedPt.clear();
	 m_mnt_mucombOverlappedEta.clear();
	 m_mnt_mucombOverlappedPhi.clear();
	 m_mnt_mucombError.clear();
	 m_mnt_mucombDR.clear();
	 m_mnt_mucombMass.clear();
	 m_mnt_mucombDRLog10.clear();
	 m_mnt_mucombMassLog10.clear();
      }
      //
      beforeExecMonitors().ignore();
      //
      if( m_doMufastBasedRemoval ) {
	 m_mnt_mufastNrAllTEs     = n_allTEs;
	 m_mnt_mufastNrOverlapped = 0;
	 m_mnt_mufastNrActiveTEs  = n_allTEs;
      }
      if( m_doMucombBasedRemoval ) {
	 m_mnt_mucombNrAllTEs     = n_allTEs;
	 m_mnt_mucombNrOverlapped = 0;
	 m_mnt_mucombNrActiveTEs  = n_allTEs;
      }
   }

   // ---
   // if nr of TEs <= 1, nothing to worry on overlaps
   // ---

   if( n_allTEs <= 1 ) {
      if(m_doDebug) msg() << MSG::DEBUG << "nr of TEs <= 1, no overlap removal necessary. exitting with all TEs active" << endreq;
      if(m_doMonitor) afterExecMonitors().ignore();
      return HLT::OK;
   }

   // ---
   // get all L2StandAloneMuon and L2CombinedMuon elementLinks
   // ---

   bool errorWhenGettingELs = false;
   
   typedef  ElementLinkVector<xAOD::L2StandAloneMuonContainer>  ELVMuonsSA;
   typedef  ElementLinkVector<xAOD::L2CombinedMuonContainer>  ELVMuonsComb;

   std::vector<ELVMuonsSA>    standaloneMuonELVvec;
   std::vector<ELVMuonsComb>  combinedMuonELVvec;

   for(i_te=0; i_te<n_allTEs; i_te++) {

     ELVMuonsSA    standaloneMuonELV;
     ELVMuonsComb  combinedMuonELV;

     HLT::ErrorCode isMufastOK = (m_doMufastBasedRemoval) ?
       getFeaturesLinks<xAOD::L2StandAloneMuonContainer,  xAOD::L2StandAloneMuonContainer>(vec_allTEs[i_te], standaloneMuonELV) : HLT::OK;

     HLT::ErrorCode isMucombOK = (m_doMucombBasedRemoval) ?
       getFeaturesLinks<xAOD::L2CombinedMuonContainer, xAOD::L2CombinedMuonContainer>(vec_allTEs[i_te], combinedMuonELV) : HLT::OK;

      if( (m_doMufastBasedRemoval && (isMufastOK != HLT::OK)) || (m_doMucombBasedRemoval && (isMucombOK != HLT::OK)) ) {
	 errorWhenGettingELs = true;
	 if( isMufastOK != HLT::OK ) {
	    if(m_doMonitor) m_mnt_mufastError.push_back(TrigL2MuonOverlapRemoverConsts::errorCode_cannot_get_EL);
	    msg() << MSG::WARNING << "i_te=" << i_te << ": fails to find EL for: L2StandAloneMuon" << endreq;
	 }
	 if( isMucombOK != HLT::OK ) {
	    if(m_doMonitor) m_mnt_mucombError.push_back(TrigL2MuonOverlapRemoverConsts::errorCode_cannot_get_EL);
	    msg() << MSG::WARNING << "i_te=" << i_te << ": fails to find EL for: L2CombinedMuon" << endreq;
	 }
	 continue;
      }
      bool isMufastELOK = (m_doMufastBasedRemoval && standaloneMuonELV.size()==0) ? false : true;
      bool isMucombELOK = (m_doMucombBasedRemoval && combinedMuonELV.size()==0) ? false : true;
      if( (m_doMufastBasedRemoval && ! isMufastELOK) || (m_doMucombBasedRemoval && ! isMucombELOK) ) {
	errorWhenGettingELs = true;
	if( ! isMufastELOK ) {
	  if(m_doMonitor) m_mnt_mufastError.push_back(TrigL2MuonOverlapRemoverConsts::errorCode_EL_not_valid);
	  msg() << MSG::WARNING << "i_te=" << i_te << ": EL not valid for: L2StandAloneMuon" << endreq;
	}
	if( ! isMucombELOK ) {
	  if(m_doMonitor) m_mnt_mucombError.push_back(TrigL2MuonOverlapRemoverConsts::errorCode_EL_not_valid);
	  msg() << MSG::WARNING << "i_te=" << i_te << ": EL not valid for: L2CombinedMuon" << endreq;
	}
	continue;
      }
      if( m_doMufastBasedRemoval ) standaloneMuonELVvec.push_back(standaloneMuonELV);
      if( m_doMucombBasedRemoval ) combinedMuonELVvec.push_back(combinedMuonELV);
   }

   if( errorWhenGettingELs ) {
      msg() << MSG::WARNING << "error when getting ELs. exitting with all TEs active..." << endreq;
      if(m_doMonitor) afterExecMonitors().ignore();
      return HLT::ErrorCode(HLT::Action::CONTINUE,HLT::Reason::NAV_ERROR);
   }

   // ---
   // identify overlap
   // ---

   std::vector<unsigned int> mufastResult;
   std::vector<unsigned int> mucombResult;

   bool errorWhenIdentifyingOverlap = false;

   if( m_doMufastBasedRemoval ) {
     if(m_doDebug) msg() << MSG::DEBUG << "--- muFast based overlap identification ---" << endreq;
     for(i_te=0; i_te<n_allTEs; i_te++) { mufastResult.push_back(i_te); }
     for(i_te=0; i_te<n_allTEs-1; i_te++) {
       for(j_te=i_te+1; j_te<n_allTEs; j_te++) {
	 if(m_doDebug) msg() << MSG::DEBUG << "++ i_te=" << i_te << " vs j_te=" << j_te << endreq;
	 bool overlapped = (standaloneMuonELVvec[i_te].size() > 0 && standaloneMuonELVvec[j_te].size() > 0) ?
	   isOverlap((standaloneMuonELVvec[i_te])[0], (standaloneMuonELVvec[j_te])[0]) : false;
	 if( ! overlapped ) { // judged as different
	   if(m_doDebug) msg() << MSG::DEBUG << "   judged as: different objects" << endreq; 
	   if( mufastResult[i_te] == mufastResult[j_te] ) { // but marked as same by someone
	     msg() << MSG::INFO << "inconsistentency in muFast based overlap removal for more than two objects" << endreq;
	     msg() << MSG::INFO << "two objects are judged as different but both were already marked as identical by someone else as: " << endreq;
	     msg() << MSG::INFO << "i_te/j_te/result[i_te]/result[j_te]=" << i_te << " / " << j_te << " / " << mufastResult[i_te] << " / "  << mufastResult[j_te] << endreq;
	     errorWhenIdentifyingOverlap = true;
	     if(m_doMonitor) m_mnt_mufastError.push_back(TrigL2MuonOverlapRemoverConsts::errorCode_inconsistent_overlap1);
	   }
	 }
	 else { // judged as overlap
	   if( (mufastResult[j_te] != j_te && mufastResult[i_te] != mufastResult[j_te]) ||
	       (mufastResult[j_te] == j_te && mufastResult[i_te] != i_te) ) {
	     msg() << MSG::INFO << "inconsistentency in muFast based overlap removal for more than two objects" << endreq;
	     msg() << MSG::INFO << "two objects are judged as overlap but only either was already marked as overlap to someone else: " << endreq;
	     msg() << MSG::INFO << "i_te/j_te/result[i_te]/result[j_te]=" << i_te << " / " << j_te << " / " << mufastResult[i_te] << " / "  << mufastResult[j_te] << endreq;
	     errorWhenIdentifyingOverlap = true;
	     if(m_doMonitor) m_mnt_mufastError.push_back(TrigL2MuonOverlapRemoverConsts::errorCode_inconsistent_overlap2);
	   }
	   else {
	     if(m_doDebug) msg() << MSG::DEBUG << "   judged as: overlapped objects" << endreq;
	     if( mufastResult[i_te] == i_te ) {
	       if(m_doDebug) {
		 msg() << MSG::DEBUG << "   i_te is not yet marked as overlap. so, it is a newly found overlap" << endreq;
		 msg() << MSG::DEBUG << "   -> marking mufastResult[j_te] as i_te..." << endreq;
	       }
	       mufastResult[j_te] = i_te;
	     }
	     else {
	       if(m_doDebug) {
		 msg() << MSG::DEBUG << "   both i_te/j_te already marked as overlap by: mufastResult[i_te]=" << mufastResult[i_te] << endreq;
		 msg() << MSG::DEBUG << "   -> do nothing..." << endreq;
	       }
	     }
	   }
	 }
       }
     }
   }
   
   if( m_doMucombBasedRemoval ) {
     if(m_doDebug) msg() << MSG::DEBUG << "--- muComb based overlap identification ---" << endreq;
     for(i_te=0; i_te<n_allTEs; i_te++) { mucombResult.push_back(i_te); }
     for(i_te=0; i_te<n_allTEs-1; i_te++) {
       for(j_te=i_te+1; j_te<n_allTEs; j_te++) {
	 if(m_doDebug) msg() << MSG::DEBUG << "++ i_te=" << i_te << " vs j_te=" << j_te << endreq;
	 bool overlapped = (combinedMuonELVvec[i_te].size() > 0 && combinedMuonELVvec[j_te].size() > 0) ?
	   isOverlap((combinedMuonELVvec[i_te])[0], (combinedMuonELVvec[j_te])[0]) : false;
	 if( ! overlapped ) { // judged as different
	   if(m_doDebug) msg() << MSG::DEBUG << "   judged as: different objects" << endreq; 
	   if( mucombResult[i_te] == mucombResult[j_te] ) { // but marked as same by someone
	     msg() << MSG::INFO << "inconsistent in muComb based overlap removal for more than two objects" << endreq;
	     msg() << MSG::INFO << "judged as different objects but both are already marked as identical by someone else as: " << endreq;
	     msg() << MSG::INFO << "i_te/j_te/result[i_te]/result[j_te]=" << i_te << " / " << j_te << " / " << mucombResult[i_te] << " / "  << mucombResult[j_te] << endreq;
	     errorWhenIdentifyingOverlap = true;
	     if(m_doMonitor) m_mnt_mucombError.push_back(TrigL2MuonOverlapRemoverConsts::errorCode_inconsistent_overlap1);
	   }
	 }
	 else { // judged as overlap
	   if( (mucombResult[j_te] != j_te && mucombResult[i_te] != mucombResult[j_te]) ||
	       (mucombResult[j_te] == j_te && mucombResult[i_te] != i_te) ) {
	     msg() << MSG::INFO << "inconsistent in muComb based overlap removal for more than two objects" << endreq;
	     msg() << MSG::INFO << "judged as overlap but only either is already marked as overlap to someone else: " << endreq;
	     msg() << MSG::INFO << "i_te/j_te/result[i_te]/result[j_te]=" << i_te << " / " << j_te << " / " << mucombResult[i_te] << " / "  << mucombResult[j_te] << endreq;
	     errorWhenIdentifyingOverlap = true;
	     if(m_doMonitor) m_mnt_mucombError.push_back(TrigL2MuonOverlapRemoverConsts::errorCode_inconsistent_overlap2);
	       }
	   else {
	     if(m_doDebug) msg() << MSG::DEBUG << "   judged as: overlapped objects" << endreq;
	     if( mucombResult[i_te] == i_te ) {
	       if(m_doDebug) {
		 msg() << MSG::DEBUG << "   i_te is not yet marked as overlap. so, it is a newly found overlap" << endreq;
		 msg() << MSG::DEBUG << "   -> marking mucombResult[j_te] as i_te..." << endreq;
	       }
	       mucombResult[j_te] = i_te;
	     }
	     else {
	       if(m_doDebug) {
		 msg() << MSG::DEBUG << "   both i_te/j_te already marked as overlap by: mucombResult[i_te]=" << mucombResult[i_te] << endreq;
		 msg() << MSG::DEBUG << "   -> do nothing..." << endreq;
	       }
	     }
	   }
	 }
       }
     }
   }
   
   if( errorWhenIdentifyingOverlap ) {
      msg() << MSG::WARNING << "error when resolving overlap. exitting with all TEs active..." << endreq;
      if(m_doMonitor) afterExecMonitors().ignore();
      return HLT::ErrorCode(HLT::Action::CONTINUE,HLT::Reason::USERDEF_1);
   }

   // ---
   // if no overlap identified, just quit
   // ---

   bool mufastNoOverlap = true;
   bool mucombNoOverlap = true;

   if( m_doMufastBasedRemoval ) {
      unsigned int n_uniqueTEs_mufast = 0;
      for(i_te=0; i_te<n_allTEs; i_te++) {
	 if(m_doDebug) msg() << MSG::DEBUG << "muFast based results: i_te=" << i_te << ": ";
	 if( mufastResult[i_te] != i_te ) { msg() << MSG::DEBUG << "overlap to j_te=" << mufastResult[i_te] << endreq; }
	 else { n_uniqueTEs_mufast++; msg() << MSG::DEBUG << "unique" << endreq; }
      }
      if(m_doDebug) msg() << MSG::DEBUG << "nr of unique TEs after muFast-based removal=" << n_uniqueTEs_mufast << endreq;
      if( n_allTEs != n_uniqueTEs_mufast ) mufastNoOverlap = false;
   }

   if( m_doMucombBasedRemoval ) {
      unsigned int n_uniqueTEs_mucomb = 0;
      for(i_te=0; i_te<n_allTEs; i_te++) {
	 if(m_doDebug) msg() << MSG::DEBUG << "muComb based results: i_te=" << i_te << ": ";
	 if( mucombResult[i_te] != i_te ) { msg() << MSG::DEBUG << "overlap to j_te=" << mucombResult[i_te] << endreq; }
	 else { n_uniqueTEs_mucomb++; msg() << MSG::DEBUG << "unique" << endreq; }
      }
      if(m_doDebug) msg() << MSG::DEBUG << "nr of unique TEs after muComb-based removal=" << n_uniqueTEs_mucomb << endreq;
      if( n_allTEs != n_uniqueTEs_mucomb ) mucombNoOverlap = false;
   }

   if( mufastNoOverlap && mucombNoOverlap ) {
      if(m_doDebug) msg() << MSG::DEBUG << "no overlap identified. exitting with all TEs active" << endreq;
      if(m_doMonitor) afterExecMonitors().ignore();
      return HLT::OK;
   }

   // ---
   // Choose the best among identical objects and disable the TEs of the others than the best
   // ---

   bool errorWhenGettingRoI = false;

   if( m_doMufastBasedRemoval ) {
      if(m_doDebug) msg() << MSG::DEBUG << "--- choose best among overlaps & disable TEs (muFast based) ---" << endreq;
      for(i_te=0; i_te<n_allTEs; i_te++) {
	 if(m_doDebug) msg() << MSG::DEBUG << "++ i_te=" << i_te << ": result=" << mufastResult[i_te] << endreq;
	 if( mufastResult[i_te] != i_te ) {
	    if(m_doDebug) msg() << MSG::DEBUG << "   overlap to some one. skip." << endreq;
	    continue;
	 }
	 std::vector<unsigned int> others;
	 for(j_te=0; j_te<n_allTEs; j_te++) {
	    if( mufastResult[j_te] == mufastResult[i_te] ) others.push_back(j_te);
	 }
	 if( others.size() == 1 ) {
	    if(m_doDebug) msg() << MSG::DEBUG << "   unique object. keep it active." << endreq;
	    continue;
	 }
	 else { // must choose one best
	    if(m_doDebug) msg() << MSG::DEBUG << "   overlapped objects among: " << others << endreq;
	    unsigned int best_te = 0;
	    float maxPtMf  = 0;
	    float maxPtRoI = 0;
	    for(unsigned int j=0; j<others.size(); j++) {
	       j_te=others[j];
	       const LVL1::RecMuonRoI* muonRoI = 0;
	       if ( HLT::OK != getFeature(vec_allTEs[j_te], muonRoI, "") ) {
		  msg() << MSG::WARNING << "i_te=" << j_te << ": fails to find RecMuonRoI." << endreq;
		  errorWhenGettingRoI = true;
		  if(m_doMonitor) m_mnt_mufastError.push_back(TrigL2MuonOverlapRemoverConsts::errorCode_cannot_get_RoI);
		  continue;
	       }
	       float ptRoI = muonRoI->getThresholdValue();
	       float ptMf  = 0.;
	       if (standaloneMuonELVvec[j_te].size() > 0) {
		 const xAOD::L2StandAloneMuon* mf = *((standaloneMuonELVvec[j_te])[0]);
		 ptMf  = fabs(mf->pt());
	       }
	       if( (ptRoI-maxPtRoI) > 0.1 ) {
		  maxPtRoI = ptRoI;
		  maxPtMf  = ptMf;
		  best_te  = j_te;
	       }
	       else if( fabs(ptRoI-maxPtRoI) < 0.1 ) {
		  if( ptMf > maxPtMf ) {
		     maxPtRoI = ptRoI;
		     maxPtMf  = ptMf;
		     best_te  = j_te;
		  }
	       }
	    }
	    if(m_doDebug) msg() << MSG::DEBUG << "      best is: best_te/maxPtRoI/maxPtMf=" << best_te << " / " << maxPtRoI << " / " << maxPtMf << endreq;
	    for(unsigned int j=0; j<others.size(); j++) {
	       j_te=others[j];
	       if( j_te != best_te ) {
		  if(m_doDebug) msg() << MSG::DEBUG << "      setting activeState=false for j_te=" << j_te << endreq;
		  vec_outputTEs[j_te]->setActiveState(false);
		  // monitoring
		  if(m_doMonitor) {
		    if (standaloneMuonELVvec[j_te].size() > 0) {
		      const xAOD::L2StandAloneMuon* mf = *((standaloneMuonELVvec[j_te])[0]);
		      m_mnt_mufastNrOverlapped++;
		      m_mnt_mufastOverlappedPt.push_back(mf->pt());
		      m_mnt_mufastOverlappedEta.push_back(mf->etaMS());
		      m_mnt_mufastOverlappedPhi.push_back(mf->phiMS());
		    }
		  }
	       }
	    }
	 }
      }
   }

   if(m_doMonitor) m_mnt_mufastNrActiveTEs = n_allTEs - m_mnt_mufastNrOverlapped;

   if( errorWhenGettingRoI ) {
      msg() << MSG::WARNING << "error when getting muonRoI (muFast based). exitting with all TEs active..." << endreq;
      for(i_te=0; i_te<n_allTEs; i_te++) { vec_outputTEs[i_te]->setActiveState(true); }
      if(m_doMonitor) afterExecMonitors().ignore();
      return HLT::ErrorCode(HLT::Action::CONTINUE,HLT::Reason::USERDEF_2);
   }

   //
   if( m_doMucombBasedRemoval ) {
      if(m_doDebug) msg() << MSG::DEBUG << "--- choose best among overlaps & disable TEs (muComb based) ---" << endreq;
      for(i_te=0; i_te<n_allTEs; i_te++) {
	 if(m_doDebug) msg() << MSG::DEBUG << "++ i_te=" << i_te << ": result=" << mucombResult[i_te] << endreq;
	 if( mucombResult[i_te] != i_te ) {
	    if(m_doDebug) msg() << MSG::DEBUG << "   overlap to some one. skip." << endreq;
	    continue;
	 }
	 std::vector<unsigned int> others;
	 for(j_te=0; j_te<n_allTEs; j_te++) {
	    if( mucombResult[j_te] == mucombResult[i_te] ) others.push_back(j_te);
	 }
	 if( others.size() == 1 ) {
	    if(m_doDebug) msg() << MSG::DEBUG << "   unique object. keep it active." << endreq;
	    continue;
	 }
	 else { // must choose one best
	    if(m_doDebug) msg() << MSG::DEBUG << "   overlap objects among: " << others << endreq;
	    unsigned int best_te = 0;
	    float maxPtCombMf = 0;
	    float maxPtRoI    = 0;
	    for(unsigned int j=0; j<others.size(); j++) {
	       j_te=others[j];
	       const LVL1::RecMuonRoI* muonRoI = 0;
	       if ( HLT::OK != getFeature(vec_allTEs[j_te], muonRoI, "") ) {
		  msg() << MSG::WARNING << "i_te=" << j_te << ": fails to find RecMuonRoI." << endreq;
		  errorWhenGettingRoI = true;
		  if(m_doMonitor) m_mnt_mucombError.push_back(TrigL2MuonOverlapRemoverConsts::errorCode_cannot_get_RoI);
		  continue;
	       }
	       float ptRoI    = muonRoI->getThresholdValue();
	       float ptCombMf = 0.;
	       if (combinedMuonELVvec[j_te].size() > 0) {
		 const xAOD::L2CombinedMuon* combMf = *((combinedMuonELVvec[j_te])[0]);
		 ptCombMf = fabs(combMf->pt()/CLHEP::GeV);
	       }
	       if( (ptRoI-maxPtRoI) > 0.1 ) {
		 maxPtRoI    = ptRoI;
		 maxPtCombMf = ptCombMf;
		 best_te     = j_te;
	       }
	       else if( fabs(ptRoI-maxPtRoI) < 0.1 ) {
		 if( ptCombMf > maxPtCombMf ) {
		   maxPtRoI    = ptRoI;
		   maxPtCombMf = ptCombMf;
		   best_te     = j_te;
		 }
	       }
	    }
	    if(m_doDebug) msg() << MSG::DEBUG << "      best is: best_te/maxPtRoI/maxPtCombMf=" << best_te << " / " << maxPtRoI << " / " << maxPtCombMf << endreq;
	    for(unsigned int j=0; j<others.size(); j++) {
	       j_te=others[j];
	       if( j_te != best_te ) {
		  if(m_doDebug) msg() << MSG::DEBUG << "      setting activeState=false for j_te=" << j_te << endreq;
		  vec_outputTEs[j_te]->setActiveState(false);
		  // monitoring
		  if(m_doMonitor) {
		     if (combinedMuonELVvec[j_te].size() > 0) {
		       const xAOD::L2CombinedMuon* combMf = *((combinedMuonELVvec[j_te])[0]);
		       m_mnt_mucombNrOverlapped++;
		       m_mnt_mucombOverlappedPt.push_back(combMf->pt()/CLHEP::GeV);
		       m_mnt_mucombOverlappedEta.push_back(combMf->eta());
		       m_mnt_mucombOverlappedPhi.push_back(combMf->phi());
		     }
		  }
	       }
	    }
	 }
      }
   }

   if(m_doMonitor) m_mnt_mucombNrActiveTEs = n_allTEs - m_mnt_mucombNrOverlapped;

   if( errorWhenGettingRoI ) {
      msg() << MSG::WARNING << "error when getting muonRoI (muComb based). exitting with all TEs active..." << endreq;
      for(i_te=0; i_te<n_allTEs; i_te++) { vec_outputTEs[i_te]->setActiveState(true); }
      if(m_doMonitor) afterExecMonitors().ignore();
      return HLT::ErrorCode(HLT::Action::CONTINUE,HLT::Reason::USERDEF_2);
   }

   // ---
   // successful end
   // ---

   if(m_doMonitor) afterExecMonitors().ignore();
   return HLT::OK;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
