/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     TrigEFDsPhiPiFex.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigBphysHypo
// author :  Julie Kirk
// ********************************************************************

#include <math.h> 
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"

#include "StoreGate/StoreGateSvc.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "TrigBphysHypo/TrigEFDsPhiPiFex.h"
#include "TrigBphysHypo/BtrigUtils.h"
#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"


#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"

// additions of xAOD objects
#include "xAODEventInfo/EventInfo.h"

//#include <map.h>

class ISvcLocator;

using namespace std;

/*------------------------------------------------------------------------------------*/
TrigEFDsPhiPiFex::TrigEFDsPhiPiFex(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::FexAlgo(name, pSvcLocator)
/*------------------------------------------------------------------------------------*/
{

  // Read cuts 
  declareProperty( "AcceptAll", m_acceptAll  = false );
  declareProperty("TrackPTthr", m_trackPtthr=1400.);
  declareProperty("dEtaTrackPair", m_dEta_cut=0.2);
  declareProperty("dPhiTrackPair", m_dPhi_cut=0.2);
  declareProperty("dz0TrackPair", m_dz0_cut=3.);
  declareProperty("pTsumTrackPair", m_pTsum_cut=0.);
  declareProperty("EtaBins", m_Etabin);
  declareProperty("PhiMasslow_cut", m_PhiMasslow_cut);
  declareProperty("PhiMasshigh_cut", m_PhiMasshigh_cut);
  declareProperty("DsMasslow_cut", m_DsMasslow_cut);
  declareProperty("DsMasshigh_cut", m_DsMasshigh_cut);

  // variables for monitoring histograms
  declareMonitoredVariable(    "Ntrack",     mon_Ntrack);
  declareMonitoredStdContainer("trackPt",    mon_trackPt           , AutoClear);
  declareMonitoredStdContainer("dEta",       mon_dEta              , AutoClear);
  declareMonitoredStdContainer("dPhi",       mon_dPhi              , AutoClear);
  declareMonitoredStdContainer("dz0",        mon_dz0               , AutoClear);
  declareMonitoredStdContainer("Ptsum",      mon_Ptsum             , AutoClear);
  declareMonitoredStdContainer("PhiMass",    mon_PhiMass           , AutoClear);
  //declareMonitoredStdContainer("PhiFitMass", mon_PhiFitMass        , AutoClear);
  declareMonitoredStdContainer("DsMass",     mon_DsMass            , AutoClear);
  //declareMonitoredStdContainer("DsFitMass",  mon_DsFitMass         , AutoClear);

  // zero counters
  m_lastEvent = 999;
  m_lastEventPassed = 999;
  m_countTotalEvents=0;
  m_countTotalRoI=0;
  m_countPassedEvents=0;
  m_countPassedRoIs=0;
  m_countPassedTrackPair=0;
  m_countPassedPhiMass=0;
  m_countPassedDsMass=0;

  m_trigBphysColl=0;
}

/*-------------------------------------------*/
TrigEFDsPhiPiFex::~TrigEFDsPhiPiFex()
/*-------------------------------------------*/
{
    delete m_trigBphysColl;
}

/*-------------------------------------------*/
HLT::ErrorCode TrigEFDsPhiPiFex::hltInitialize()
/*-------------------------------------------*/
{

  // print out properties, cuts


  msg() << MSG::INFO 
      << "Track pT cut " << m_trackPtthr
      << endreq;
  msg() << MSG::INFO 
      << "Track pair cuts: dEta<" << m_dEta_cut << " dPhi<" << m_dPhi_cut 
      << " dz0<" << m_dz0_cut << " pTsum>" << m_pTsum_cut  
      << endreq;
  msg() << MSG::INFO 
      << "Phi mass cuts: " << m_PhiMasslow_cut << " < Mass(phi) < " << m_PhiMasshigh_cut 
      << endreq;
  msg() << MSG::INFO 
      << "Ds mass cuts: " << m_DsMasslow_cut << " < Mass(Ds) < " << m_DsMasshigh_cut 
      << endreq;
  msg() << MSG::INFO 
      << "Initialization completed successfully" 
      << endreq;
  
  return HLT::OK;
}


/*-------------------------------------------*/
HLT::ErrorCode TrigEFDsPhiPiFex::hltFinalize()
/*-------------------------------------------*/
{

  msg() << MSG::INFO << "in finalize()" << endreq;
  msg() << MSG::INFO << "Run on events/RoIs " << m_countTotalEvents << "/" << m_countTotalRoI <<  endreq;
  msg() << MSG::INFO << "Passed events/RoIs " << m_countPassedEvents << "/" << m_countPassedRoIs <<  endreq;
  msg() << MSG::INFO << "RoIs Passed track pair: "  << m_countPassedTrackPair << endreq;
  msg() << MSG::INFO << "RoIs Passed PhiMass: "  << m_countPassedPhiMass << endreq;
  msg() << MSG::INFO << "RoIs Passed DsMass: "  << m_countPassedDsMass << endreq;
  
  return HLT::OK;
}


/*--------------------------------------------------------------*/
HLT::ErrorCode TrigEFDsPhiPiFex::acceptInput(const HLT::TriggerElement* inputTE, bool& pass)
/*--------------------------------------------------------------*/
{
  bool PassedTrackPair=false;
  bool PassedPhiMass=false;
  bool PassedDsMass=false;
  bool result = false;
  //  const double PIMASS = 139.57;
  //  const double KMASS = 493.677;
  //  const double PHIMASS = 1020.;
  //const double DSMASS = 1968.2;

  // reset monitoring variables
  mon_Ntrack=-99;

    // Retrieve event info
    int IdRun   = 0;
    int IdEvent = 0;
    
    // JW - Try to get the xAOD event info
    const EventInfo* pEventInfo(0);
    const xAOD::EventInfo *evtInfo(0);
    if ( store()->retrieve(evtInfo).isFailure() ) {
        if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "Failed to get xAOD::EventInfo " << endreq;
        // now try the old event ifo
        if ( store()->retrieve(pEventInfo).isFailure() ) {
            if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "Failed to get EventInfo " << endreq;
            //mon_Errors.push_back( ERROR_No_EventInfo );
        } else {
            IdRun   = pEventInfo->event_ID()->run_number();
            IdEvent = pEventInfo->event_ID()->event_number();
            if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Run " << IdRun << " Event " << IdEvent <<  endreq;
        }// found old event info
    }else { // found the xAOD event info
        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Run " << evtInfo->runNumber()
            << " Event " << evtInfo->eventNumber() << endreq;
        IdRun   = evtInfo->runNumber();
        IdEvent = evtInfo->eventNumber();
    } // get event ifo


  // Accept-All mode: temporary patch; should be done with force-accept 
  if (m_acceptAll) {
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "AcceptAll property is set: taking all events" 
	  << endreq;

    pass = true;
  } 
  else {
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "AcceptAll property not set: applying selection" 
	  << endreq;
    pass=false;
  }

// get RoI descriptor
  const TrigRoiDescriptor* roiDescriptor = 0;
  if (getFeature(inputTE, roiDescriptor) != HLT::OK) roiDescriptor = 0;

  if ( !roiDescriptor ) {
    if ( msgLvl() <= MSG::WARNING) {
      msg() <<  MSG::WARNING << "No RoI for this Trigger Element! " << endreq;
    }    
    
    return HLT::NAV_ERROR;
  }

  if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " In EFDsPhiPIFEX acceptInput" << endreq;

  if ( msgLvl() <= MSG::DEBUG ){
    msg() << MSG::DEBUG 
	<< "Using inputTE("<< inputTE <<")->getId(): " << inputTE->getId()
	<< "; RoI ID = "   << roiDescriptor->roiId()
	<< ": Eta = "      << roiDescriptor->eta()
	<< ", Phi = "      << roiDescriptor->phi()
	<< endreq;
  } 


    if (IdEvent != (int) m_lastEvent) {
      m_countTotalEvents++;
      m_lastEvent=IdEvent;
    }
    m_countTotalRoI++;


//  create vector for TrigEFBphys particles
    delete m_trigBphysColl;
  m_trigBphysColl = new TrigEFBphysContainer();
  //  m_VertexColl = new TrigVertexCollection;





    ///////////////// Get vector of tracks /////////////////
  const Rec::TrackParticleContainer* TrkParticleCont;
  
  HLT::ErrorCode status = getFeature(inputTE, TrkParticleCont);
  if(status != HLT::OK or TrkParticleCont==NULL) {
    msg() << MSG::WARNING << "Failed to get TrackParticleContainer's from the trigger element" << endreq;
    return HLT::MISSING_FEATURE;
  } else {
    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Got Track Collections" << endreq;
  }  


    ////////////////////////////////////////////////////////////


    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Now loop over tracks " << endreq;

    Rec::TrackParticleContainer::const_iterator trkIt =  TrkParticleCont->begin();
    Rec::TrackParticleContainer::const_iterator lastTrkIt = TrkParticleCont->end();
    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Found tracks, ntrack= " << TrkParticleCont->size() << endreq;
    mon_Ntrack=TrkParticleCont->size();
    std::vector<bool> GoodTrack(TrkParticleCont->size(), false);
    int nGoodTrack=0;
    mon_trackPt.reserve(mon_Ntrack);
    for (int itrk=0 ; trkIt != lastTrkIt; itrk++, trkIt++) {
      
        // JW EDM const Trk::MeasuredPerigee* trckPerigee =  (*trkIt)->measuredPerigee();
        const Trk::Perigee* trckPerigee =  (*trkIt)->measuredPerigee();

      //      double d0 = trckPerigee->parameters()[Trk::d0];
      double px = trckPerigee->momentum()[Trk::px];
      double py = trckPerigee->momentum()[Trk::py];
      double ptSquare=std::pow(px,2) + std::pow(py,2);
      double pT = sqrt(ptSquare) * trckPerigee->parameters()[Trk::qOverP]/fabs(trckPerigee->parameters()[Trk::qOverP]);      
      mon_trackPt.push_back(fabs(pT)/CLHEP::GeV);

      //double phi = trckPerigee->parameters()[Trk::phi];
      //double theta = trckPerigee->parameters()[Trk::theta];
      //double eta = -std::log(tan(theta/2));

	if( fabs(pT) > m_trackPtthr ) {
            GoodTrack[itrk]=true;
            nGoodTrack++;
            if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG	<< "Good track, track: " << itrk << " pT = "  <<  pT << " phi = " << trckPerigee->parameters()[Trk::phi] << endreq;
	} else {
            if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Bad track, track: " << itrk << " pT = " <<  pT << endreq;
	} // if good pT
     } // end loop over tracks

     if (nGoodTrack<3) return HLT::OK;

    Rec::TrackParticleContainer::const_iterator trk1It =  TrkParticleCont->begin();
    Rec::TrackParticleContainer::const_iterator lastTrk1It = TrkParticleCont->end();
    for (int itrk=0 ; trk1It != lastTrk1It; itrk++, trk1It++) {
       Rec::TrackParticleContainer::const_iterator trk2It =  TrkParticleCont->begin();
       Rec::TrackParticleContainer::const_iterator lastTrk2It = TrkParticleCont->end();
       for (int jtrk=0 ; trk2It != lastTrk2It; jtrk++, trk2It++) {
            if (jtrk<=itrk) continue;
            if ( msgLvl() <= MSG::VERBOSE ) msg() << MSG::VERBOSE << "Check tracks " << itrk << " and " << jtrk << endreq;
            if (!(GoodTrack[itrk] && GoodTrack[jtrk]) ) continue;
            if ( msgLvl() <= MSG::VERBOSE ) msg() << MSG::VERBOSE << "Good tracks " << itrk << " and " << jtrk << " now check sign " << endreq;

           // JW EDM           const Trk::MeasuredPerigee* trckPerigee1 =  (*trk1It)->measuredPerigee();
           //            const Trk::MeasuredPerigee* trckPerigee2 =  (*trk2It)->measuredPerigee();
           const Trk::Perigee* trckPerigee1 =  (*trk1It)->measuredPerigee();
           const Trk::Perigee* trckPerigee2 =  (*trk2It)->measuredPerigee();

            if ((trckPerigee1->parameters()[Trk::qOverP]>0. && trckPerigee2->parameters()[Trk::qOverP]>0.) ||
                (trckPerigee1->parameters()[Trk::qOverP]<0. && trckPerigee2->parameters()[Trk::qOverP]<0.))  continue;

            if ( msgLvl() <= MSG::VERBOSE ) msg() << MSG::VERBOSE << "Tracks are opposite sign, make track pair cuts " << endreq;
            if (! PassTrackPair( trckPerigee1, trckPerigee2)) continue;
            if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Tracks " << itrk << " and " << jtrk  << " pass track pair cuts, now form KK mass " << endreq;
            PassedTrackPair=true;
	    
           //JW EDM           std::vector<const  Trk::MeasuredPerigee*> inputtrks;
           std::vector<const  Trk::Perigee*> inputtrks;
	    std::vector<double> massHypo ;
  	    inputtrks.push_back(trckPerigee1 );
	    inputtrks.push_back(trckPerigee2 );
            massHypo.push_back( KMASS );
            massHypo.push_back( KMASS );
                   
            double PhiMass=InvMass(inputtrks, massHypo);
            mon_PhiMass.push_back(PhiMass);

            int etaBin;
            double theta1 = trckPerigee1->parameters()[Trk::theta];
            double eta1 = -std::log(tan(theta1/2));
            double theta2 = trckPerigee2->parameters()[Trk::theta];
            double eta2 = -std::log(tan(theta2/2));
            for (int iBin = 0; iBin < 3; iBin++)
                 if ( fabs(eta1) > m_Etabin[iBin] ||  fabs(eta2) > m_Etabin[iBin] ) etaBin = iBin; 


            float eta = (fabs(eta1)>fabs(eta2) ? eta1 : eta2 );

	      if ( msgLvl() <= MSG::VERBOSE ) msg()  << MSG::VERBOSE << "track eta: " << eta1 << " " << eta2 << " eta " << eta << " eta Bin : " << etaBin << endreq;

            if ( msgLvl() <= MSG::VERBOSE ) msg() << MSG::VERBOSE << " Phi_mass =  " << PhiMass << endreq;
            if (PhiMass> m_PhiMasslow_cut[etaBin] && PhiMass<m_PhiMasshigh_cut[etaBin]) {
		PassedPhiMass=true;
                TrigEFBphys* trigPartPhi = new TrigEFBphys(roiDescriptor->roiId(), eta , roiDescriptor->phi(), TrigEFBphys::PHIKK, PhiMass);
	        if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "Create EFBphys particle with roIId " << trigPartPhi->roiId()  
                       << " mass " << PhiMass << " phi, eta " << trigPartPhi->phi() << " " << trigPartPhi->eta() <<
		       " vertex type " << trigPartPhi->particleType() << endreq;

		// JK 9-6-08 Add TrackParticles to TrigEFBphys
                ElementLink<Rec::TrackParticleContainer> track1EL(*TrkParticleCont,itrk);
                ElementLink<Rec::TrackParticleContainer> track2EL(*TrkParticleCont,jtrk);
                trigPartPhi->addTrack(track1EL);
                trigPartPhi->addTrack(track2EL);

                m_trigBphysColl->push_back(trigPartPhi);
                int PhiIndex = m_trigBphysColl->size() - 1;
                if ( msgLvl() <= MSG::DEBUG )msg() << MSG::DEBUG << "phi index " << PhiIndex << endreq;
     	        // if pass phi mass then now make Ds mass
                Rec::TrackParticleContainer::const_iterator trk3It =  TrkParticleCont->begin();
                Rec::TrackParticleContainer::const_iterator lastTrk3It = TrkParticleCont->end();
                for (int ktrk=0 ; trk3It != lastTrk3It; ktrk++, trk3It++) {

                    if (!(GoodTrack[ktrk]) ) continue;
                    if (trk3It==trk1It || trk3It==trk2It) continue;
                    if ( msgLvl() <= MSG::VERBOSE ) msg() << MSG::VERBOSE << " try to add track " << ktrk << endreq;
                    //JW EDM const Trk::MeasuredPerigee* trckPerigee3 =  (*trk3It)->measuredPerigee();
                    const Trk::Perigee* trckPerigee3 =  (*trk3It)->measuredPerigee();
        	    inputtrks.push_back(trckPerigee3);
                    massHypo.push_back( PIMASS );
 	            double DsMass = InvMass(inputtrks, massHypo);
                    mon_DsMass.push_back(DsMass);

                    if ( msgLvl() <= MSG::VERBOSE ) msg() << MSG::VERBOSE << " Ds mass =  " << DsMass << endreq;

                    double theta3 = trckPerigee3->parameters()[Trk::theta];
                    double eta3 = -std::log(tan(theta3/2));
                    if (fabs(eta3) > eta) {
                     for (int iBin = 0; iBin < 3; iBin++)
                        if ( fabs(eta3) > m_Etabin[iBin] ) etaBin = iBin; 

                     eta = eta3;
		    }

	            if ( msgLvl() <= MSG::VERBOSE ) msg()  << MSG::VERBOSE << "track eta: " << eta3 << " eta " << eta << " eta Bin : " << etaBin << endreq;
                    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " phi index to create elementlink " << PhiIndex << endreq;
                    ElementLink<TrigEFBphysContainer> trigPartPhiEL(*m_trigBphysColl,PhiIndex);



                    if (DsMass > m_DsMasslow_cut[etaBin] && DsMass < m_DsMasshigh_cut[etaBin]) {
                        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " EVENT PASSES!! " << endreq;
		    TrigEFBphys* trigPartDs = new TrigEFBphys(roiDescriptor->roiId(), eta , roiDescriptor->phi(), TrigEFBphys::DSPHIPI, DsMass, trigPartPhiEL);
	  	    if ( msgLvl() <= MSG::DEBUG )msg()  << MSG::DEBUG << "Create EFBphys particle with roIId " << trigPartDs->roiId()  
                           << " mass " << DsMass << " phi, eta " << trigPartDs->phi() << " " << trigPartDs->eta() <<
			       " vertex type " << trigPartDs->particleType() << endreq;
		        result=true;
                        PassedDsMass=true;
                        ElementLink<Rec::TrackParticleContainer> track3EL(*TrkParticleCont,ktrk);
                        trigPartDs->addTrack(track1EL);
                        trigPartDs->addTrack(track2EL);
                        trigPartDs->addTrack(track3EL);

                        m_trigBphysColl->push_back(trigPartDs);
		    }

		    
                    inputtrks.pop_back();
                    massHypo.pop_back();
		} // loop over third track

	    } // end pass phi mass


	    
       } // end loop over track 2
    } // end loop over track 1


     if (PassedTrackPair) m_countPassedTrackPair++;
     if (PassedPhiMass) m_countPassedPhiMass++;
     if (PassedDsMass) m_countPassedDsMass++;
     if (result) {
        m_countPassedRoIs++;
        if (IdEvent!= (int) m_lastEventPassed) {
          m_countPassedEvents++;
          m_lastEventPassed=IdEvent;
	}
     }              
     // Reach this point successfully
     if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Run: " << IdRun << " Event: " << IdEvent << "  result is " << result << endreq;

//     result=true;
  
  if (!m_acceptAll) pass = result; 

  return HLT::OK;
}


/*--------------------------------------------------------------*/
HLT::ErrorCode TrigEFDsPhiPiFex::hltExecute(const HLT::TriggerElement* /* inputTE */, HLT::TriggerElement* outputTE)
/*--------------------------------------------------------------*/
{
     if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " In EFDsPhiPIFEX hltExecute" << endreq;

// get RoI descriptor
  const TrigRoiDescriptor* roiDescriptor = 0;
  if (getFeature(outputTE, roiDescriptor) != HLT::OK) roiDescriptor = 0;

  if ( !roiDescriptor ) {
    if ( msgLvl() <= MSG::WARNING) {
      msg() <<  MSG::WARNING << "No RoI for this Trigger Element! " << endreq;
    }    
    
    return HLT::NAV_ERROR;
  }

  if ( msgLvl() <= MSG::DEBUG ){
    msg() << MSG::DEBUG 
	<< "Using outputTE("<< outputTE <<")->getId(): " << outputTE->getId()
	<< "; RoI ID = "   << roiDescriptor->roiId()
	<< ": Eta = "      << roiDescriptor->eta()
	<< ", Phi = "      << roiDescriptor->phi()
	<< endreq;
  } 

     if ((m_trigBphysColl!=0) && (m_trigBphysColl->size() > 0)) {
  	if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "REGTEST: Store Bphys Collection size: " << m_trigBphysColl->size() << endreq;
        //std::string bphysKey ="";
	//	HLT::ErrorCode sc = recordAndAttachFeature(outputTE, m_trigBphysColl, bphysKey, "EFDsPhiPiFex" );
	HLT::ErrorCode sc = attachFeature(outputTE, m_trigBphysColl, "EFDsPhiPiFex" );
        if(sc != HLT::OK) {
	   msg()  << MSG::WARNING << "Failed to store trigBphys Collection" << endreq;
           return sc;
	}
     } else {
  	if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "REGTEST: no bphys collection to store "  << endreq;
        delete m_trigBphysColl;

     }
     m_trigBphysColl=0;

     return HLT::OK;
}

/*-------------------------------------------*/
bool TrigEFDsPhiPiFex::PassTrackPair(const Trk::Perigee* trckPerigee1, const Trk::Perigee* trckPerigee2)
/*-------------------------------------------*/
{
  bool passed=false;

  double phi1 = trckPerigee1->parameters()[Trk::phi];
  double theta1 = trckPerigee1->parameters()[Trk::theta];
  double z0_1 = trckPerigee1->parameters()[Trk::z0];
  double px1 = trckPerigee1->momentum()[Trk::px];
  double py1 = trckPerigee1->momentum()[Trk::py];
  double pt1 = sqrt(px1*px1 + py1*py1);
  double eta1 = -std::log(tan(theta1/2));

  double phi2 = trckPerigee2->parameters()[Trk::phi];
  double theta2 = trckPerigee2->parameters()[Trk::theta];
  double z0_2 = trckPerigee2->parameters()[Trk::z0];
  double px2 = trckPerigee2->momentum()[Trk::px];
  double py2 = trckPerigee2->momentum()[Trk::py];
  double pt2 = sqrt(px2*px2 + py2*py2);
  double eta2 = -std::log(tan(theta2/2));


  if ( msgLvl() <= MSG::VERBOSE ){
    msg() << MSG::VERBOSE << "PassTrackPair: track 1 : pt= " << pt1 
    <<  " phi0 = " << phi1 << "  eta = " << eta1 << "   z0 = " << z0_1 << endreq;
    msg() << MSG::VERBOSE << "PassTrackPair: track 2 : pt= " << pt2 
    <<  " phi0 = " << phi2 << "  eta = " << eta2 << "   z0 = " << z0_2 << endreq;
  }
  double deta = eta1-eta2;
  double dphi = phi1-phi2;
  double absdphi = fabs(dphi);
  if (2.* M_PI -absdphi < absdphi) {
     if (dphi > 0) { 
        dphi = dphi - 2.* M_PI;
     } else { 
        dphi = 2* M_PI - absdphi;
     }
  }	
  dphi=fabs(dphi);

  double dz0  = z0_1 - z0_2;
  double ptsum= pt1+pt2;


  if ( msgLvl() <= MSG::VERBOSE )  msg() << MSG::VERBOSE
      << " deta = " << deta << "    dphi = " << dphi << "    dz0 = " << dz0 << "    ptsum = " << ptsum 
      << endreq;

  mon_dEta.push_back(deta);
  mon_dPhi.push_back(dphi);
  mon_dz0.push_back(dz0);
  mon_Ptsum.push_back(ptsum/1000.);


  if (fabs(deta)< m_dEta_cut && fabs(dphi)<m_dPhi_cut && fabs(dz0)<m_dz0_cut && ptsum>m_pTsum_cut) 
     passed=true;



  if ( msgLvl() <= MSG::VERBOSE ) msg() << MSG::VERBOSE << "Track pairs passed: " << passed << endreq;
  
  return passed;
}

/*
double TrigEFDsPhiPiFex::closeTrack(TrigVertex* p_Vtx, TrigInDetTrack* track)
{
  double max_dist = 10.;
  bool answer=true;

  if ( msgLvl() <= MSG::DEBUG ) {
    msg() << MSG::DEBUG << "Check closeness of track to vertex: " << endreq;
    msg() << MSG::DEBUG << "Track parameters: pT: " << track->param()->pT() /1000. <<
    " phi " << track->param()->phi0()  << " eta: " << track->param()->eta()  << endreq;
    msg() << MSG::DEBUG << "vertex position  x= y= z= " << p_Vtx->x() << " " << p_Vtx->y()
    << " " << p_Vtx->z() << endreq;
  }
  double theta = 2. * atan(exp(-1.*(track->param()->eta()) ));
  double track_x = cos(track->param()->phi0()) * sin(theta);
  double track_y = sin(track->param()->phi0()) * sin(theta);
  double track_z = cos(theta);

  double d0 = track->param()->a0();
  // diection between phi-vertex and track perigee
  double d_x =  p_Vtx->x() + d0 * sin(track->param()->phi0());
  double d_y =  p_Vtx->y() - d0 * cos(track->param()->phi0());
  double d_z =  p_Vtx->z() - track->param()->z0();

  // distance between 2
  double dis_x = track_y * d_z - track_z * d_y;
  double dis_y = track_x * d_z - track_z * d_x;
  double dis_z = track_x * d_y - track_y * d_x;

  double distance = sqrt(dis_x*dis_x + dis_y * dis_y + dis_z*dis_z);
  if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "distance is: " << distance << endreq;
 
    if (distance > max_dist) answer=false;

  return(distance);
}
*/
