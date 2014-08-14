// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigBPhysHypo/TrigEFDiMuXFex.cxx
 **
 **   Description: EF hypothesis algorithms for B+ -> K+ mu+ mu-
 **
 **   Author: Cristina Adorisio (Cristina.Adorisio@cern.ch)
 **
 **   Created:   27.10.2008
 **
 **   Modified:
 **
 **
 **************************************************************************/
#include "TrigBphysHypo/TrigEFDiMuXFex.h"
#include "TrigBphysHypo/BtrigUtils.h"

#include "StoreGate/StoreGateSvc.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

// additions of xAOD objects
#include "xAODEventInfo/EventInfo.h"

/*------------------------------------------------------------------------------------*/
TrigEFDiMuXFex::TrigEFDiMuXFex(const std::string & name, ISvcLocator* pSvcLocator):
HLT::FexAlgo(name, pSvcLocator)
/*------------------------------------------------------------------------------------*/
{
    
    // Read cuts
    declareProperty("AcceptAll", m_acceptAll  = false );
    declareProperty("LowerMuMuMassCut", m_lowerMuMuMassCut = 200.0 );  //default=200.0
    declareProperty("UpperMuMuMassCut", m_upperMuMuMassCut = 5000.0 );  //default=5000.0
    declareProperty("LowerKplusMuMuMassCut", m_lowerKplusMuMuMassCut = 5000.0 );  //default=5000.0
    declareProperty("UpperKplusMuMuMassCut", m_upperKplusMuMuMassCut = 5600.0 );  //default=5600.0
    
    // variables for monitoring histograms
    declareMonitoredStdContainer("MuMuMass",      mon_dimumass    , AutoClear);
    declareMonitoredStdContainer("KplusMuMuMass", mon_kdimumass   , AutoClear);
    
    m_trigBphysColl = new TrigEFBphysContainer();
    
}

/*-------------------------------------------*/
TrigEFDiMuXFex::~TrigEFDiMuXFex()
/*-------------------------------------------*/
{
    delete m_trigBphysColl;
}

/*-------------------------------------------*/
HLT::ErrorCode TrigEFDiMuXFex::hltInitialize()
/*-------------------------------------------*/
{
    if ( msgLvl() <= MSG::DEBUG) {
        msg() << MSG::DEBUG << "In EFDiMuXFex hltInitialize" << endreq;
    }
    
    msg() << MSG::INFO << "|----------------------- INFO FROM TrigEFDiMuXFex --------------------|" << endreq;
    msg() << MSG::INFO << "Initializing TrigEFDiMuXFex" << endreq;
    msg() << MSG::INFO << "AcceptAll            = "
	<< (m_acceptAll==true ? "True" : "False") << endreq;
    msg() << MSG::INFO << "LowerMuMuMassCut      = " << m_lowerMuMuMassCut << endreq;
    msg() << MSG::INFO << "UpperMuMuMassCut      = " << m_upperMuMuMassCut << endreq;
    msg() << MSG::INFO << "LowerKplusMuMuMassCut = " << m_lowerKplusMuMuMassCut << endreq;
    msg() << MSG::INFO << "UpperKplusMuMuMassCut = " << m_upperKplusMuMuMassCut << endreq;
    msg() << MSG::INFO << "|---------------------------------------------------------------------|" << endreq;
    
    m_lastEvent      = -1;
    m_lastEvent_DiMu = -1;
    
    m_lastEventPassedMuMu  = -1;
    m_lastEventPassedKMuMu = -1;
    
    m_countTotalEvents      = 0;
    m_countTotalEvents_DiMu = 0;
    m_countTotalRoI         = 0;
    m_countTotalRoI_DiMu    = 0;
    
    m_countPassedEvents      = 0;
    m_countPassedRoIs        = 0;
    m_countPassedEventsMuMu  = 0;
    m_countPassedEventsKMuMu = 0;
    
    m_countPassedMuMuMassCut  = 0;
    m_countPassedKMuMuMassCut = 0;
    
    return HLT::OK;
}


/*-------------------------------------------*/
HLT::ErrorCode TrigEFDiMuXFex::hltFinalize()
/*-------------------------------------------*/
{
    if ( msgLvl() <= MSG::DEBUG) {
        msg() << MSG::DEBUG << "In EFDiMuXFex hltFinalize" << endreq;
    }
    
    msg() << MSG::INFO << "|----------------------- SUMMARY FROM TrigEFDiMuXFex -----------------|" << endreq;
    msg() << MSG::INFO << "Run on events / RoIs " << m_countTotalEvents       << " / " << m_countTotalRoI <<  endreq;
    msg() << MSG::INFO << "Run on events / RoIs " << m_countTotalEvents_DiMu  << " / " << m_countTotalRoI_DiMu << " from EFDiMuFex algo" <<  endreq;
    msg() << MSG::INFO << "Passed events / RoIs " << m_countPassedEvents      << " / " << m_countPassedRoIs <<  endreq;
    
    msg() << MSG::INFO << "Passed MuMu Invariant Mass Cut ( events / RoIs )      : "      << m_countPassedEventsMuMu  << " / " << m_countPassedMuMuMassCut << endreq;
    msg() << MSG::INFO << "Passed KplusMuMu Invariant Mass Cut ( events / RoIs ) : " << m_countPassedEventsKMuMu << " / " << m_countPassedKMuMuMassCut << endreq;
    msg() << MSG::INFO << "|---------------------------------------------------------------------|" << endreq;
    
    return HLT::OK;
}


/*---------------------------------------------------------------------------------------------------------*/
HLT::ErrorCode TrigEFDiMuXFex::hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE)
/*---------------------------------------------------------------------------------------------------------*/
{
    if ( msgLvl() <= MSG::DEBUG) {
        msg() << MSG::DEBUG << "In EFDiMuXFex hltExecute" << endreq;
    }
    
    
    bool resultMuMu  = false;
    bool resultKMuMu = false;
    
    bool PassedMuMuPair    = false;
    bool PassedKMuMuMass   = false;
    
    std::vector<const Trk::Perigee *> particleOK; // JW from measuredperigee
    std::vector<double> mu_pT;
    
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
        if ( msgLvl() <= MSG::DEBUG ){
            msg() << MSG::DEBUG << "AcceptAll property is set: taking all events"
            << endreq;
        }
    } else {
        if ( msgLvl() <= MSG::DEBUG ){
            msg() << MSG::DEBUG << "AcceptAll property not set: applying selection"
            << endreq;
        }
    }
    
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
    if ( !m_trigBphysColl )
        m_trigBphysColl = new TrigEFBphysContainer();
    else
        m_trigBphysColl->clear();
    
    // Get vector of pointers to all TrigMuonEF objects linked to the outputTE
    // by label "EFMuPair":
    const TrigEFBphysContainer* vectorOfMuPairs=NULL;
    HLT::ErrorCode status = getFeature(outputTE, vectorOfMuPairs, "EFMuPairs");
    
    if ( msgLvl() <= MSG::DEBUG) {
        msg() << MSG::DEBUG << "Returned from getFeature" << endreq;
    }
    if ( status != HLT::OK || vectorOfMuPairs == NULL ) {
        if ( msgLvl() <= MSG::DEBUG) {
            msg() << MSG::DEBUG << "No TrigMuPair Feature found" << endreq;
        }
        return status;
    } else {
        if ( msgLvl() <= MSG::DEBUG) {
            msg() << MSG::DEBUG << "TrigMuPair Feature found  with "  << vectorOfMuPairs->size() << " TrigEFBphys particles" << endreq;
        }
    }
    // to count events from TrigDiMuon
    if ( IdEvent != m_lastEvent_DiMu ) {
        m_countTotalEvents_DiMu++;
        m_lastEvent_DiMu=IdEvent;
    }
    m_countTotalRoI_DiMu++;
    
    for ( TrigEFBphysContainer::const_iterator pairIt = vectorOfMuPairs->begin(); pairIt != vectorOfMuPairs->end(); ++pairIt ){
        const ElementLinkVector<Rec::TrackParticleContainer> trackVector = (*pairIt)->trackVector();
        if (trackVector.size() != 0) {
            if ( msgLvl() <= MSG::DEBUG) {
                msg() << MSG::DEBUG << "Got muon pair track vector size: " << trackVector.size() << endreq;
            }
        } else {
            if ( msgLvl() <= MSG::DEBUG) {
                msg() << MSG::DEBUG << "No track vector!!! "  << endreq;
            }
        }
        
        float mass = (*pairIt)->mass();
        msg() << MSG::VERBOSE << "MuMu Invariant Mass = " << mass << endreq;
        
        // apply mass cut
        if ( mass < m_lowerMuMuMassCut || mass > m_upperMuMuMassCut) {
            if ( msgLvl()<= MSG::DEBUG ){
                msg() << MSG::DEBUG << "Combination discarded by mass cut: " << mass << " MeV" << endreq;
            }
            particleOK.clear(); //to have only the muon pair in the vector
            mu_pT.clear();
            PassedMuMuPair = false;
            continue;
        } else {
            mon_dimumass.push_back((mass*0.001));
            PassedMuMuPair = true;
            resultMuMu = true;
            if( msgLvl() <= MSG::DEBUG ) {
                msg() << MSG::DEBUG << "MuMu combination retained! Invariant mass value: "
                << mass << " MeV" << endreq;
            }
            particleOK.clear(); //to have only the muon pair in the vector
            mu_pT.clear();
            
            ElementLinkVector<Rec::TrackParticleContainer>::const_iterator trkIt = trackVector.begin();
            if( msgLvl() <= MSG::DEBUG ) {
                msg() << MSG::DEBUG << "Number of tracks in vertex:  " << trackVector.size() << endreq;
            }
            if ( trackVector.size() < 2 ) {
                if( msgLvl() <= MSG::DEBUG ) {
                    msg() << MSG::DEBUG << "Mo muon tracks - jump to the next" << trackVector.size() << endreq;
                }
                continue;
            }
            
            for (int itrk=0 ; trkIt!= trackVector.end(); ++itrk, ++trkIt) {
                // JW EDM const Trk::MeasuredPerigee* muTrackPerigee=(*(*trkIt))->measuredPerigee();
                const Trk::Perigee* muTrackPerigee=(*(*trkIt))->measuredPerigee();
                if (!muTrackPerigee) continue;
                msg() << MSG::VERBOSE << "track, iterator, pointer " << itrk << " " << *trkIt << " " << *(*trkIt) << endreq;
                double phi   = muTrackPerigee->parameters()[Trk::phi];
                double theta = muTrackPerigee->parameters()[Trk::theta];
                double eta   = -std::log(tan(theta/2));
                double px    = muTrackPerigee->momentum()[Trk::px];
                double py    = muTrackPerigee->momentum()[Trk::py];
                double ptSquare = std::pow(px,2) + std::pow(py,2);
                double pT       = std::sqrt(ptSquare) * muTrackPerigee->parameters()[Trk::qOverP]/std::fabs(muTrackPerigee->parameters()[Trk::qOverP]);
                
                msg() << MSG::DEBUG << "track " << itrk << " pt phi eta " << pT << " " << phi << " " << eta << endreq;
                
                //	msg() << MSG::VERBOSE << "track " << itrk << " pt phi eta " << pT << " " << phi << " " << eta << endreq;
                
                particleOK.push_back(muTrackPerigee);
                mu_pT.push_back(pT);
            }
            //CRIS DA CANCELLARE
            msg() << MSG::DEBUG << "CRISTINA EF - particleOK.size() = " << particleOK.size() << endreq;
            msg() << MSG::DEBUG << "CRISTINA EF - mu_pT.size() = " << mu_pT.size() << endreq;
            //CRIS DA CANCELLARE
        }
        
        // Now look over track to find the third one
        const Rec::TrackParticleContainer* TrkParticleCont;
        
        status = getFeature(inputTE, TrkParticleCont);
        if ( status != HLT::OK || TrkParticleCont==NULL ) {
            msg() << MSG::DEBUG << "Failed to get TrackParticleContainer's from the trigger element" << endreq;
            return HLT::MISSING_FEATURE;
        } else {
            msg() << MSG::DEBUG << "Got Track Collections with size " << TrkParticleCont->size() << endreq;
        }
        if ( TrkParticleCont->size() <= 2 ){
            msg() << MSG::DEBUG << "Track Collections size <= 2, go to next  TrackParticleContainer" << endreq;
            //continue;
        }
        
        if( msgLvl() <= MSG::DEBUG ) {
            msg() << MSG::DEBUG << "Now loop over tracks " << endreq;
        }
        Rec::TrackParticleContainer::const_iterator trkIt =  TrkParticleCont->begin();
        Rec::TrackParticleContainer::const_iterator lastTrkIt = TrkParticleCont->end();
        if( msgLvl() <= MSG::DEBUG ) {
            msg() << MSG::DEBUG << "Found tracks, ntrack= " << TrkParticleCont->size() << endreq;
        }
        
        int itr1=-99;
        int itr2=-99;
        bool indexFound = false;
        
        for (int itrk=0 ; trkIt != lastTrkIt; ++itrk, ++trkIt) {
            
            //JW EDM const Trk::MeasuredPerigee* trackPerigee =  (*trkIt)->measuredPerigee();
            const Trk::Perigee* trackPerigee =  (*trkIt)->measuredPerigee();
            //double phi   = trackPerigee->parameters()[Trk::phi];
            //double theta = trackPerigee->parameters()[Trk::theta];
            double px    = trackPerigee->momentum()[Trk::px];
            double py    = trackPerigee->momentum()[Trk::py];
            double ptSquare = std::pow(px,2) + std::pow(py,2);
            double pT       = std::sqrt(ptSquare) * trackPerigee->parameters()[Trk::qOverP]/std::fabs(trackPerigee->parameters()[Trk::qOverP]);
            msg() << MSG::VERBOSE << "track " << itrk << " pT = " << pT << endreq;
            
            
            particleOK.push_back(trackPerigee);
            
            std::vector<double> massHypo;
            massHypo.clear();
            massHypo.push_back(MUMASS);
            massHypo.push_back(MUMASS);
            massHypo.push_back(KPLUSMASS);
            
            if ( pT < 0 ){   // only positive tracks (B+ case)
                particleOK.pop_back();
                continue;
            }
            if ( std::fabs(pT - mu_pT[0]) < EPSILON || std::fabs(pT - mu_pT[1]) < EPSILON ){
                particleOK.pop_back();
                continue;
            } else {
                
                double BmassHypo = InvMass(particleOK, massHypo);
                particleOK.pop_back();
                
                // apply mass cut
                if ( BmassHypo < m_lowerKplusMuMuMassCut || BmassHypo > m_upperKplusMuMuMassCut ) {
                    if ( msgLvl()<= MSG::DEBUG ){
                        msg() << MSG::DEBUG << "3 particles combination discarded by mass cut: " << BmassHypo << " MeV" << endreq;
                    }
                    continue;
                } else {
                    mon_kdimumass.push_back((BmassHypo*0.001));
                    PassedKMuMuMass = true;
                    resultKMuMu = true;
                    if( msgLvl() <= MSG::DEBUG ) {
                        msg() << MSG::DEBUG << "3 particles combination retained! Invariant mass value: " << BmassHypo << " MeV" << endreq;
                    }
                    //store new TrigEFBphys collection
                    TrigEFBphys* trigPartBmumuX = new TrigEFBphys(roiDescriptor->roiId(), roiDescriptor->eta(), roiDescriptor->phi(), TrigEFBphys::BMUMUX, BmassHypo);
                    if( msgLvl() <= MSG::DEBUG ) {
                        msg()  << MSG::DEBUG << "Create Bphys particle with roI Id " << trigPartBmumuX->roiId()
                        << " phi = " << trigPartBmumuX->phi() << " eta = " << trigPartBmumuX->eta()
                        << " mass = " << trigPartBmumuX->mass()
                        << " particle type (trigPartBmumuX->particleType()) " << trigPartBmumuX->particleType() << endreq;
                    }
                    
                    ElementLink<Rec::TrackParticleContainer> track3(*TrkParticleCont,itrk);
                    
                    Rec::TrackParticleContainer::const_iterator track =  TrkParticleCont->begin();
                    Rec::TrackParticleContainer::const_iterator lastTrack = TrkParticleCont->end();
                    if (indexFound == false) {
                        bool halfIndex1 = false;
                        bool halfIndex2 = false;
                        for (int it=0 ; track != lastTrack; ++it, ++track) {
                            
                            indexFound = (halfIndex1 && halfIndex2);
                            if (indexFound == true) break;
                            
                            //JW EDM const Trk::MeasuredPerigee* trackPerigee =  (*track)->measuredPerigee();
                            const Trk::Perigee* trackPerigee =  (*track)->measuredPerigee();
                            double px    = trackPerigee->momentum()[Trk::px];
                            double py    = trackPerigee->momentum()[Trk::py];
                            double ptSquare = std::pow(px,2) + std::pow(py,2);
                            double pT       = sqrt(ptSquare) * trackPerigee->parameters()[Trk::qOverP]/fabs(trackPerigee->parameters()[Trk::qOverP]);  
                            
                            if ( fabs(pT - mu_pT[0]) <= EPSILON ) {
                                itr1=it;
                                halfIndex1 = true;
                            } 
                            else if ( fabs(pT - mu_pT[1]) <= EPSILON ){ 
                                itr2=it;
                                halfIndex2 = true;
                            }
                        }
                    }
                    
                    if ( itr1 == -99 || itr2 == -99 ) {
                        msg() << MSG::INFO << "Something is worng, can't find tracks in original container" << endreq;
                    }
                    
                    ElementLink<Rec::TrackParticleContainer> track1(*TrkParticleCont,itr1);	    
                    ElementLink<Rec::TrackParticleContainer> track2(*TrkParticleCont,itr2);
                    
                    trigPartBmumuX->addTrack(track1);
                    trigPartBmumuX->addTrack(track2);
                    trigPartBmumuX->addTrack(track3);
                    
                    
                    m_trigBphysColl->push_back(trigPartBmumuX);
                    if( msgLvl() <= MSG::DEBUG ) {
                        msg() << MSG::DEBUG << "Added particle to Bphys particle collection" << endreq;
                    }
                }
            }
            //}
        } // end loop over tracks
    } // end of loop over pair
    
    if ( ( m_trigBphysColl != 0 ) && ( m_trigBphysColl->size() > 0 ) ) {
        if( msgLvl() <= MSG::DEBUG ) {
            msg()  << MSG::DEBUG << "REGTEST: Store Bphys Collection size: " << m_trigBphysColl->size() << endreq;
        }
        HLT::ErrorCode sc = attachFeature(outputTE, m_trigBphysColl, "EFDiMuXFex" );
        if ( sc != HLT::OK ) {
            msg() << MSG::WARNING << "Failed to store trigBphys Collection" << endreq;
            return sc;
        }
    } else {
        if( msgLvl() <= MSG::DEBUG ) {
            msg() << MSG::DEBUG << "REGTEST: No Bphys Collection to store"  << endreq;
        }
        delete m_trigBphysColl;
    }
    m_trigBphysColl=0;
    
    
    if ( PassedMuMuPair ) m_countPassedMuMuMassCut++;
    if ( PassedKMuMuMass ) m_countPassedKMuMuMassCut++;
    
    if ( resultMuMu == true ){
        if ( IdEvent != (int) m_lastEventPassedMuMu ) {
            m_countPassedEventsMuMu++;
            m_lastEventPassedMuMu=IdEvent;
        }
    }
    if ( resultKMuMu == true ){
        m_countPassedRoIs++;
        if ( IdEvent != (int) m_lastEventPassedKMuMu ) {
            m_countPassedEvents++;
            m_countPassedEventsKMuMu++;
            m_lastEventPassedKMuMu=IdEvent;
        }
    }
    
    return HLT::OK;
}




