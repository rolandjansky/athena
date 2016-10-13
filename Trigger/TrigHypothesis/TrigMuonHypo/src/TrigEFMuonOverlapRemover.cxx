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

#include "TrigMuonHypo/TrigEFMuonOverlapRemover.h"

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigEFMuonOverlapRemover::TrigEFMuonOverlapRemover(const std::string& name, ISvcLocator* pSvcLocator)
   : HLT::AllTEAlgo(name, pSvcLocator)
{
   //
   m_doMonitor = false;
   m_doDebug   = false;


   // MuExtr based cuts
   declareProperty("DoMuExtrBasedRemoval",   m_doMuExtrBasedRemoval   = true);
   declareProperty("MuExtrRequireDR",        m_muExtrRequireDR        = true);
   declareProperty("MuExtrRequireMufastDR",  m_muExtrRequireMufastDR  = true);
   declareProperty("MuExtrRequireMass",      m_muExtrRequireMass      = true);
   declareProperty("MuExtrRequireSameSign",  m_muExtrRequireSameSign  = true);
   std::vector<double> def_muExtrEtaBins;
   def_muExtrEtaBins.push_back(0);
   def_muExtrEtaBins.push_back(9.9);
   declareProperty("MuExtrEtaBins",          m_muExtrEtaBins=def_muExtrEtaBins);
   std::vector<double> def_muExtrDRThres;
   def_muExtrDRThres.push_back(1e-3);
   declareProperty("MuExtrDRThres",          m_muExtrDRThres=def_muExtrDRThres);
   std::vector<double> def_muExtrMassThres;
   def_muExtrMassThres.push_back(1e-3);
   declareProperty("MuExtrMassThres",        m_muExtrMassThres=def_muExtrMassThres);

   // Monitor choice
   declareProperty("MonitorChoice",  m_monitorChoice = TrigEFMuonOverlapRemoverConsts::monitorChoice_all);

   // monitoring variables
   declareMonitoredVariable("MuExtrNrAllTEs",           m_mnt_muextrNrAllTEs);
   declareMonitoredVariable("MuExtrNrOverlapped",       m_mnt_muextrNrOverlapped);
   declareMonitoredVariable("MuExtrNrActiveTEs",        m_mnt_muextrNrActiveTEs);
   declareMonitoredStdContainer("MuExtrOverlappedPt",   m_mnt_muextrOverlappedPt);
   declareMonitoredStdContainer("MuExtrOverlappedEta",  m_mnt_muextrOverlappedEta);
   declareMonitoredStdContainer("MuExtrOverlappedPhi",  m_mnt_muextrOverlappedPhi);
   declareMonitoredStdContainer("MuExtrError",          m_mnt_muextrError);
   declareMonitoredStdContainer("MuExtrDR",             m_mnt_muextrDR);
   declareMonitoredStdContainer("MuextrMass",           m_mnt_muextrMass);
   declareMonitoredStdContainer("MuExtrDRLog10",        m_mnt_muextrDRLog10);
   declareMonitoredStdContainer("MuextrMassLog10",      m_mnt_muextrMassLog10);
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

HLT::ErrorCode TrigEFMuonOverlapRemover::hltBeginRun()
{
   msg() << MSG::INFO << "in hltBeginRun : " << name() << endreq;

   //
   m_doDebug   =  msgLvl() <= MSG::DEBUG;
   msg() << MSG::INFO << "m_doDebug=" << m_doDebug << endreq;   

   //
   if( m_doMuExtrBasedRemoval ) {
      msg() << MSG::INFO << "--- muExtr based overlap removal as: ---"     << endreq;
      if( m_muExtrRequireDR ) {
	 msg() << MSG::INFO << "+ dR cut:" << endreq;
	 if( (m_muExtrEtaBins.size()-1) != m_muExtrDRThres.size() ) {
	    msg() << MSG::INFO << "bad thresholds setup .... exiting!" << endreq;
	    return HLT::BAD_JOB_SETUP;
	 }
	 for(unsigned int i=0; i<m_muExtrDRThres.size(); i++) {
	    msg() << MSG::INFO << "     EtaBin " << m_muExtrEtaBins[i] << " - " << m_muExtrEtaBins[i+1]
		  << " : dR < " << m_muExtrDRThres[i] << endreq;
	 }
      }
      if( m_muExtrRequireMass ) {
	 msg() << MSG::INFO << "+ Mass cut:" << endreq;
	 if( (m_muExtrEtaBins.size()-1) != m_muExtrMassThres.size() ) {
	    msg() << MSG::INFO << "bad thresholds setup .... exiting!" << endreq;
	    return HLT::BAD_JOB_SETUP;
	 }
	 for(unsigned int i=0; i<m_muExtrMassThres.size(); i++) {
	    msg() << MSG::INFO << "     EtaBin " << m_muExtrEtaBins[i] << " - " << m_muExtrEtaBins[i+1]
		  << " : Mass < " << m_muExtrMassThres[i] << endreq;
	 }
      }
      if( m_muExtrRequireSameSign ) msg() << MSG::INFO << "+ Same charge sign" << endreq;
   }
   
   //
   msg() << MSG::INFO << "Initialization completed successfully"  << endreq; 

   return HLT::OK;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
bool TrigEFMuonOverlapRemover::isOverlap(std::vector<const xAOD::MuonContainer*> extrMfLink1,
					 std::vector<const xAOD::MuonContainer*> extrMfLink2)
{


   //first TE
   std::vector<double> pt1; 
   std::vector<double> eta1; 
   std::vector<double> phi1; 
   std::vector<int> charge1; 
   if (m_doDebug) msg() << MSG::DEBUG << "+++++++++++++ FIRST TE" << endreq;
   for (unsigned int i=0; i<extrMfLink1.size(); i++){
     if (m_doDebug) msg() << MSG::DEBUG << "Element " << i << " of vector of TrigMuonEFInfo containers" << endreq;
     // Get first (and only) RoI:
     const xAOD::MuonContainer* trigMuon = extrMfLink1[i];
     if(!trigMuon){
       msg() << MSG::ERROR
             << "Retrieval of TrigMuonEFInfoContainer from vector failed"
             << endreq;
             return HLT::NAV_ERROR;
     } else {
       if(m_doDebug) msg() << MSG::DEBUG << "TrigMuonEFInfoContainer OK with size " << trigMuon->size() << endreq;
     }
     // loop on the muons within the RoI
     xAOD::MuonContainer::const_iterator MuonItr  = trigMuon->begin();
     xAOD::MuonContainer::const_iterator MuonItrE = trigMuon->end();
     for(int j=0; MuonItr != MuonItrE; ++MuonItr, ++j ) {
        msg() << MSG::DEBUG << "Looking at xAOD::Muon " << j << endreq;
	const xAOD::Muon* muon = (*MuonItr);
        if (!muon) {
          if (m_doDebug) msg() << MSG::DEBUG << "No xAOD::Muon found." << endreq;
          continue;
        }
	else {
          if (muon->trackParticle(xAOD::Muon::TrackParticleType::ExtrapolatedMuonSpectrometerTrackParticle)) { 
	      const xAOD::TrackParticle *tr = muon->trackParticle(xAOD::Muon::TrackParticleType::ExtrapolatedMuonSpectrometerTrackParticle); 
	      if (!tr) {
                if (m_doDebug) msg() << MSG::DEBUG << "Extrapolated track not initialized." << endreq;
                continue;
	      }
	      else {
		if (m_doDebug) msg() << MSG::DEBUG
				     << "Retrieved extrapolated track with abs pt "
				     << (*tr).pt()/CLHEP::GeV << " GeV " << endreq;
                pt1.push_back((*tr).pt());
                eta1.push_back((*tr).eta());
                phi1.push_back((*tr).phi());
                charge1.push_back((*tr).charge());
              }
            }
	}
     }
   }
	 
	  
	    

   //second TE

   std::vector<double> pt2;
   std::vector<double> eta2;
   std::vector<double> phi2;
   std::vector<int> charge2; 
   if (m_doDebug) msg() << MSG::DEBUG << "+++++++++++++ SECOND TE" << endreq;
   for (unsigned int i=0; i<extrMfLink2.size(); i++){
     if (m_doDebug) msg() << MSG::DEBUG << "Element " << i << " of vector of TrigMuonEFInfo containers" << endreq;
     // Get first (and only) RoI:
     const xAOD::MuonContainer* trigMuon = extrMfLink2[i];
     if(!trigMuon){
       msg() << MSG::ERROR
             << "Retrieval of xAOD::MuonContainer from vector failed"
             << endreq;
             return HLT::NAV_ERROR;
     } else {
       if(m_doDebug) msg() << MSG::DEBUG << "xAOD::MuonContainer OK with size " << trigMuon->size() << endreq;
     }
     // loop on the muons within the RoI
     xAOD::MuonContainer::const_iterator MuonItr  = trigMuon->begin();
     xAOD::MuonContainer::const_iterator MuonItrE = trigMuon->end();
     for(int j=0; MuonItr != MuonItrE; ++MuonItr, ++j ) {
        msg() << MSG::DEBUG << "Looking at xAOD::Muon " << j << endreq;
	const xAOD::Muon* muon = (*MuonItr);
        if (!muon) {
          if (m_doDebug) msg() << MSG::DEBUG << "No TrigMuonEFInfo found." << endreq;
          continue;
        } 
	else {
          if (muon->trackParticle(xAOD::Muon::TrackParticleType::ExtrapolatedMuonSpectrometerTrackParticle)) { // was there a muon in this RoI ?
	    const xAOD::TrackParticle* tr = muon->trackParticle(xAOD::Muon::TrackParticleType::ExtrapolatedMuonSpectrometerTrackParticle);
	    if (!tr) {
	      if (m_doDebug) msg() << MSG::DEBUG << "No TrigMuonEFTrack found." << endreq;
	      continue;
	    } 
	    else {
	      if (m_doDebug) msg() << MSG::DEBUG
				   << "Retrieved extrapolated track with abs pt "
				   << (*tr).pt()/CLHEP::GeV << " GeV " << endreq;
	      pt2.push_back((*tr).pt());
	      eta2.push_back((*tr).eta());
	      phi2.push_back((*tr).phi());
	      charge2.push_back((*tr).charge());
	    }	  
          }
        }
     }
   }

   //overlap check... just say that the extrapolated track collections are identical
   bool sametrackcollection = false;
   if(pt1.size()!=pt2.size() || pt1.size()==0 || pt2.size()==0 || pt1.size()>1 || pt2.size()>1) {
     if(m_doDebug) msg() << MSG::DEBUG << "   not same size of extrapolated track collection or not tracks found or size > 1.... cannot judge overlap -> return with false" << endreq;
   } else {
     for(uint i=0; i<pt1.size(); i++) { //to be closed
       if(m_doDebug) msg() << MSG::DEBUG << "   ...extrM1: pt/eta/phi/charge=" << pt1[i]/CLHEP::GeV << " / " << eta1[i] << " / " << phi1[i] << " / " << charge1[i] << endreq;
       if(m_doDebug) msg() << MSG::DEBUG << "   ...extrM2: pt/eta/phi/charge=" << pt2[i]/CLHEP::GeV << " / " << eta2[i] << " / " << phi2[i] << " / " << charge2[i] << endreq;

       // if dR or invMass is necessary but (eta,phi) info is not avaiable
       // (i.e. eta,phi=0,0; rec failed)
       const double ZERO_LIMIT_FOR_ETAPHI = 1e-4;
       if( (fabs(eta1[i]) <ZERO_LIMIT_FOR_ETAPHI && fabs(phi1[i]) < ZERO_LIMIT_FOR_ETAPHI) ||
       (fabs(eta2[i]) <ZERO_LIMIT_FOR_ETAPHI && fabs(phi2[i]) < ZERO_LIMIT_FOR_ETAPHI) ) {
         if(m_doDebug) msg() << MSG::DEBUG << "   ...-> (eta,phi) info not available (rec at (eta,phi)=(0,0))" << endreq;
         if( m_muExtrRequireDR || m_muExtrRequireMass ) {
	   if(m_doDebug) msg() << MSG::DEBUG << "   ...-> but dR of invMass check is required. cannot judge overlap -> return with false" << endreq;
	   return false;
         }
       }

       // if charge or invMass is necessary but charge(pT) info is not avaiable
       const double ZERO_LIMIT_FOR_PT = 1e-4;
       if( (fabs(pt1[i]) <ZERO_LIMIT_FOR_PT) || (fabs(pt2[i]) < ZERO_LIMIT_FOR_PT) ) {
         if(m_doDebug) msg() << MSG::DEBUG << "   ...-> pT info not available (rec at pT=0)" << endreq;
         if( m_muExtrRequireMass ) {
	   if(m_doDebug) msg() << MSG::DEBUG << "   ...-> but invMass check is required. cannot judge overlap -> return with false" << endreq;
	   return false;
         }
       }
       const double ZERO_LIMIT_FOR_CHARGE = 1e-4;
       if( std::abs(charge1[i])<ZERO_LIMIT_FOR_CHARGE || std::abs(charge2[i])<ZERO_LIMIT_FOR_CHARGE ) {
         if(m_doDebug) msg() << MSG::DEBUG << "   ...-> charge info not available" << endreq;
         if( m_muExtrRequireSameSign ) {
           if(m_doDebug) msg() << MSG::DEBUG << "   ...-> but same sign check is required. cannot judge overlap -> return with false" << endreq;
           return false;
         }
       }

       // determine etabin and thresholds
       double absEta = (fabs(pt1[i]) > fabs(pt2[i])) ? fabs(eta1[i]) : fabs(eta2[i]);
       unsigned int iThres = 0;
       for(unsigned int j=0; j<(m_muExtrEtaBins.size()-1); j++) {
         if ( m_muExtrEtaBins[j] <= absEta && absEta < m_muExtrEtaBins[j+1] ) iThres = j;
       }
       double dRThres     = m_muExtrDRThres[iThres];
       double massThres   = m_muExtrMassThres[iThres];
       if(m_doDebug)  {
         msg() << MSG::DEBUG << "   ...iThres=" << iThres << endreq;
         if(m_muExtrRequireDR)        msg() << MSG::DEBUG << "   ...dR       threshold=" << dRThres     << endreq;
         if(m_muExtrRequireMass)      msg() << MSG::DEBUG << "   ...mass     threshold=" << massThres   << endreq;
       }

        // same sign cut
        bool sameSign = false;
        if( m_muExtrRequireSameSign ) {
          sameSign = ((charge1[i])*(charge2[i]) > 0) ? true : false;
          if(m_doDebug) msg() << MSG::DEBUG << "   ...-> sameSign=" << sameSign << endreq;
        }

        // dR cut
        bool dRisClose = false;
        double dr = dR(eta1[i],phi1[i],eta2[i],phi2[i]);
        if(m_doMonitor) {
          m_mnt_muextrDR.push_back(dr);
          const double monitor_limit = 1e-4;
          double dr_mon = (dr>=monitor_limit) ? dr : monitor_limit;
          m_mnt_muextrDRLog10.push_back(log10(dr_mon));
        }
        if( m_muExtrRequireDR ) {
         if( dr < dRThres ) dRisClose = true;
         if(m_doDebug) msg() << MSG::DEBUG << "   ...-> dR=" << dr << " : dRisClose=" << dRisClose << endreq;
       }


       // mass cut
       const double TRACK_MASS = 0;  // just assume zero mass
       bool massIsClose = false;
       double mass = invMass(TRACK_MASS,pt1[i]/CLHEP::GeV,eta1[i],phi1[i],TRACK_MASS,pt2[i]/CLHEP::GeV,eta2[i],phi2[i]);
       if(m_doMonitor) {
         m_mnt_muextrMass.push_back(mass);
         const double monitor_limit = 1e-4;
         double mass_mon = (mass>=monitor_limit) ? mass : monitor_limit;
         m_mnt_muextrMassLog10.push_back(log10(mass_mon));
       }
       if( m_muExtrRequireMass ) {
         if( mass < massThres ) massIsClose = true;
         if(m_doDebug) msg() << MSG::DEBUG << "   ...-> mass=" << mass << " : massIsClose=" << massIsClose << endreq;
       }

       // total judge on extrapolated track
       if( ((m_muExtrRequireSameSign &&   sameSign)     || (! m_muExtrRequireSameSign)) &&
          ((m_muExtrRequireDR       &&  dRisClose)     || (! m_muExtrRequireDR))       &&
          ((m_muExtrRequireMass     &&  massIsClose)   || (! m_muExtrRequireMass)) ) {
         sametrackcollection = true; 
       } else {
         sametrackcollection = false; 
       }

       if(m_doDebug) msg() << MSG::DEBUG << "   ...=> same estrapolated track = " << sametrackcollection << endreq;

     }
   }

   // total judge on TE
   bool overlap = false;
   if( sametrackcollection) {
     overlap = true;
     if(m_doDebug) msg() << MSG::DEBUG << "   ...=> isOverlap=" << overlap << endreq;
   }

   return overlap;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

double TrigEFMuonOverlapRemover::dR(double eta1, double phi1, double eta2, double phi2)
{
   double deta = eta1 - eta2;
   double dphi = fabs(phi1 - phi2);
   if( dphi > CLHEP::pi ) dphi = CLHEP::twopi - dphi;
   double dR = pow( (deta*deta + dphi*dphi), 0.5 );
   return dR;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

double TrigEFMuonOverlapRemover::invMass(double m1, double pt1, double eta1, double phi1,
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

HLT::ErrorCode TrigEFMuonOverlapRemover::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& tes_in,
						    unsigned int type_out)
{
   if(m_doDebug) msg() << MSG::DEBUG << "in hltExecute : " << name() << endreq;

   m_doMonitor = (m_monitorChoice==TrigEFMuonOverlapRemoverConsts::monitorChoice_all) ? true : false; 

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
	 if( m_monitorChoice == TrigEFMuonOverlapRemoverConsts::monitorChoice_mu6_only || m_doDebug ) {
	    std::string label;
	    TrigConf::HLTTriggerElement::getLabel (te->getId(), label );
	    if( m_monitorChoice == TrigEFMuonOverlapRemoverConsts::monitorChoice_mu6_only && label.find("mu6") != std::string::npos ) m_doMonitor = true;
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
      if( m_doMuExtrBasedRemoval ) {
	 m_mnt_muextrNrAllTEs     = 0;
	 m_mnt_muextrNrOverlapped = 0;
	 m_mnt_muextrNrActiveTEs  = 0;
	 m_mnt_muextrOverlappedPt.clear();
	 m_mnt_muextrOverlappedEta.clear();
	 m_mnt_muextrOverlappedPhi.clear();
	 m_mnt_muextrError.clear();
	 m_mnt_muextrDR.clear();
	 m_mnt_muextrMass.clear();
	 m_mnt_muextrDRLog10.clear();
	 m_mnt_muextrMassLog10.clear();
      }
      //
      beforeExecMonitors().ignore();
      //
      if( m_doMuExtrBasedRemoval ) {
	 m_mnt_muextrNrAllTEs     = n_allTEs;
	 m_mnt_muextrNrOverlapped = 0;
	 m_mnt_muextrNrActiveTEs  = n_allTEs;
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
   // get all xAOD::MuonContainers 
   // ---

   bool errorWhenGettingELs = false;
  
   std::vector<std::vector<const xAOD::MuonContainer*> > EFMuonELvec; 

   for(i_te=0; i_te<n_allTEs; i_te++) {
     std::vector<const xAOD::MuonContainer*> EFMuonEL;
      HLT::ErrorCode isMuExtrOK = (m_doMuExtrBasedRemoval) ? getFeatures(vec_allTEs[i_te], EFMuonEL) : HLT::OK;
      if( (m_doMuExtrBasedRemoval && (isMuExtrOK != HLT::OK)) ) {
	 errorWhenGettingELs = true;
	 if( isMuExtrOK != HLT::OK ) {
	    if(m_doMonitor) m_mnt_muextrError.push_back(TrigEFMuonOverlapRemoverConsts::errorCode_cannot_get_EL);
	    msg() << MSG::WARNING << "i_te=" << i_te << ": fails to find EL for: EFMuon" << endreq;
	 }
	 continue;
      }
      bool isMuExtrELOK = (m_doMuExtrBasedRemoval) ? (!EFMuonEL.empty()) : true;
      if( (m_doMuExtrBasedRemoval && ! isMuExtrELOK) ) {
	 errorWhenGettingELs = true;
	 if( ! isMuExtrELOK ) {
	    if(m_doMonitor) m_mnt_muextrError.push_back(TrigEFMuonOverlapRemoverConsts::errorCode_EL_not_valid);
	    msg() << MSG::WARNING << "i_te=" << i_te << ": EL not valid for: EFMuon" << endreq;
	 }
	 continue;
      }
      if( m_doMuExtrBasedRemoval ) EFMuonELvec.push_back(EFMuonEL);
   }

   if( errorWhenGettingELs ) {
      msg() << MSG::WARNING << "error when getting ELs. exitting with all TEs active..." << endreq;
      if(m_doMonitor) afterExecMonitors().ignore();
      return HLT::ErrorCode(HLT::Action::CONTINUE,HLT::Reason::NAV_ERROR);
   }


   // ---
   // identify overlap
   // ---

   std::vector<unsigned int> muextrResult;

   bool errorWhenIdentifyingOverlap = false;

   if( m_doMuExtrBasedRemoval ) {
      if(m_doDebug) msg() << MSG::DEBUG << "--- muExtr based overlap identification ---" << endreq;
      for(i_te=0; i_te<n_allTEs; i_te++) { muextrResult.push_back(i_te); }
      for(i_te=0; i_te<n_allTEs-1; i_te++) {
	 for(j_te=i_te+1; j_te<n_allTEs; j_te++) {
	    if(m_doDebug) msg() << MSG::DEBUG << "++ i_te=" << i_te << " vs j_te=" << j_te << endreq;
	    bool overlapped = isOverlap(EFMuonELvec[i_te],EFMuonELvec[j_te]);
	    if( ! overlapped ) { // judged as different
	       if(m_doDebug) msg() << MSG::DEBUG << "   judged as: different objects" << endreq; 
	       if( muextrResult[i_te] == muextrResult[j_te] ) { // but marked as same by someone
		  msg() << MSG::INFO << "inconsistent in muExtr based overlap removal for more than two objects" << endreq;
		  msg() << MSG::INFO << "judged as different objects but both are already marked as identical by someone else as: " << endreq;
		  msg() << MSG::INFO << "i_te/j_te/result[i_te]/result[j_te]=" << i_te << " / " << j_te << " / " << muextrResult[i_te] << " / "  << muextrResult[j_te] << endreq;
		  errorWhenIdentifyingOverlap = true;
		  if(m_doMonitor) m_mnt_muextrError.push_back(TrigEFMuonOverlapRemoverConsts::errorCode_inconsistent_overlap1);
	       }
	    }
	    else { // judged as overlap
	       if( (muextrResult[j_te] != j_te && muextrResult[i_te] != muextrResult[j_te]) ||
		   (muextrResult[j_te] == j_te && muextrResult[i_te] != i_te) ) {
		  msg() << MSG::INFO << "inconsistent in muExtr based overlap removal for more than two objects" << endreq;
		  msg() << MSG::INFO << "judged as overlap but only either is already marked as overlap to someone else: " << endreq;
		  msg() << MSG::INFO << "i_te/j_te/result[i_te]/result[j_te]=" << i_te << " / " << j_te << " / " << muextrResult[i_te] << " / "  << muextrResult[j_te] << endreq;
		  errorWhenIdentifyingOverlap = true;
		  if(m_doMonitor) m_mnt_muextrError.push_back(TrigEFMuonOverlapRemoverConsts::errorCode_inconsistent_overlap2);
	       }
	       else {
		  if(m_doDebug) msg() << MSG::DEBUG << "   judged as: overlapped objects" << endreq;
		  if( muextrResult[i_te] == i_te ) {
		     if(m_doDebug) {
			msg() << MSG::DEBUG << "   i_te is not yet marked as overlap. so, it is a newly found overlap" << endreq;
			msg() << MSG::DEBUG << "   -> marking muextrResult[j_te] as i_te..." << endreq;
		     }
		     muextrResult[j_te] = i_te;
		  }
		  else {
		     if(m_doDebug) {
			msg() << MSG::DEBUG << "   both i_te/j_te already marked as overlap by: muextrResult[i_te]=" << muextrResult[i_te] << endreq;
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

   bool muextrNoOverlap = true;

   if( m_doMuExtrBasedRemoval ) {
      unsigned int n_uniqueTEs_muextr = 0;
      for(i_te=0; i_te<n_allTEs; i_te++) {
	 if(m_doDebug) msg() << MSG::DEBUG << "muExtr based results: i_te=" << i_te << ": ";
	 if( muextrResult[i_te] != i_te ) { msg() << MSG::DEBUG << "overlap to j_te=" << muextrResult[i_te] << endreq; }
	 else { n_uniqueTEs_muextr++; msg() << MSG::DEBUG << "unique" << endreq; }
      }
      if(m_doDebug) msg() << MSG::DEBUG << "nr of unique TEs after muExtr-based removal=" << n_uniqueTEs_muextr << endreq;
      if( n_allTEs != n_uniqueTEs_muextr ) muextrNoOverlap = false;
   }

   if( muextrNoOverlap ) {
      if(m_doDebug) msg() << MSG::DEBUG << "no overlap identified. exitting with all TEs active" << endreq;
      if(m_doMonitor) afterExecMonitors().ignore();
      return HLT::OK;
   }


   // ---
   // Choose one among identical objects and disable the TEs of the others
   // ---

   //
   if( m_doMuExtrBasedRemoval ) {
     if(m_doDebug) msg() << MSG::DEBUG << "--- disable TEs of the identical object (muExtr based) ---" << endreq;
     for(i_te=0; i_te<n_allTEs; i_te++) {
       if(m_doDebug) msg() << MSG::DEBUG << "++ i_te=" << i_te << ": result=" << muextrResult[i_te] << endreq;
       if( muextrResult[i_te] != i_te ) {
	 if(m_doDebug) msg() << MSG::DEBUG << "   overlap to some one. skip." << endreq;
	 continue;
       }
       std::vector<unsigned int> others;
       for(j_te=0; j_te<n_allTEs; j_te++) {
	 if( muextrResult[j_te] == muextrResult[i_te] ) others.push_back(j_te);
       }
       if( others.size() == 1 ) {
	 if(m_doDebug) msg() << MSG::DEBUG << "   unique object. keep it active." << endreq;
	 continue;
       } else { // must choose one... the first as they are identical
	 if(m_doDebug) msg() << MSG::DEBUG << "   overlap objects among: " << others << endreq;
	 for(unsigned int j=0; j<others.size(); j++) {
	   j_te=others[j];
	   if( j_te != i_te ) {
	     if(m_doDebug) msg() << MSG::DEBUG << "      setting activeState=false for j_te=" << j_te << endreq;
	     vec_outputTEs[j_te]->setActiveState(false);
             // monitoring
	     if(m_doMonitor) {
	       if(m_doDebug) msg() << MSG::DEBUG << "      monitoring of the deactivated j_te=" << j_te << endreq;
	       m_mnt_muextrNrOverlapped++;
               for (unsigned int i=0; i<EFMuonELvec[j_te].size(); i++){
                 if (m_doDebug) msg() << MSG::DEBUG << "Element " << i << " of vector of TrigMuonEFInfo containers" << endreq;
                 // Get first (and only) RoI:
                 const xAOD::MuonContainer* trigMuon = EFMuonELvec[j_te].at(i);
                 if(!trigMuon){
                   msg() << MSG::ERROR
                         << "Retrieval of xAOD::MuonContainer from vector failed"
                         << endreq;
                   return HLT::NAV_ERROR;
                 } else {
                   if(m_doDebug) msg() << MSG::DEBUG << "xAOD::MuonContainer OK with size " << trigMuon->size() << endreq;
                 }
                 // loop on the muons within the RoI
		 xAOD::MuonContainer::const_iterator MuonItr  = trigMuon->begin();
		 xAOD::MuonContainer::const_iterator MuonItrE = trigMuon->end();
                 for(int j=0; MuonItr != MuonItrE; ++MuonItr, ++j ) {
                   if (m_doDebug) msg() << MSG::DEBUG << "Looking at xAOD::Muon " << j << endreq;
		   const xAOD::Muon* muon = (*MuonItr);
                   if (!muon) {
                     if (m_doDebug) msg() << MSG::DEBUG << "No xAOD::Muon found." << endreq;
                     continue;
                   } 
		   else {
                     if (muon->trackParticle(xAOD::Muon::TrackParticleType::ExtrapolatedMuonSpectrometerTrackParticle)) { // was there a muon in this RoI ?
		       const xAOD::TrackParticle* tr = muon->trackParticle(xAOD::Muon::TrackParticleType::ExtrapolatedMuonSpectrometerTrackParticle);
		       if (!tr) {
			 if (m_doDebug) msg() << MSG::DEBUG << "No TrigMuonEFTrack found." << endreq;
			 continue;
		       } 
		       else {
			 if (m_doDebug) msg() << MSG::DEBUG
					      << "Retrieved extrapolated track with abs pt "
					      << (*tr).pt()/CLHEP::GeV << " GeV " << endreq;
			 m_mnt_muextrOverlappedPt.push_back(((*tr).pt())/CLHEP::GeV);
			 m_mnt_muextrOverlappedEta.push_back((*tr).eta());
			 m_mnt_muextrOverlappedPhi.push_back((*tr).phi());
		       }
		     }
                   }
                 }
               }
  	     }
           }
         }
       }
     }
   }

   if(m_doMonitor) m_mnt_muextrNrActiveTEs = n_allTEs - m_mnt_muextrNrOverlapped;

   // ---
   // successful end
   // ---

   if(m_doMonitor) afterExecMonitors().ignore();
   return HLT::OK;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
