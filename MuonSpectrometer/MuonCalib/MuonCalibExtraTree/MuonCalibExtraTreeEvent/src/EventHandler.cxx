/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraTreeEvent/EventHandler.h"

#include "MuonCalibExtraTreeEvent/MuonCalibHit_E.h"
#include "MuonCalibExtraTreeEvent/MuonCalibTriggerInfo.h"
#include "MuonCalibExtraTreeEvent/MuonCalibCaloHit.h"
#include "MuonCalibExtraTreeEvent/MuonCalibHole_E.h"
#include "MuonCalibExtraTreeEvent/MuonCalibTrack_E.h"
#include "MuonCalibExtraTreeEvent/MuonCalibEvent_E.h"
#include "MuonCalibExtraTreeEvent/MuonCalibExtendedTrack.h"
#include "MuonCalibExtraTreeEvent/MuonCalibExtendedSegment.h"
#include "MuonCalibEventBase/MuonCalibTruthCollection.h"
#include "MuonCalibEventBase/MuonCalibTruth.h"
#include "MuonCalibEventBase/MuonCalibMdtTruthHit.h"
#include "MuonCalibEventBase/MuonCalibRpcTruthHit.h"
#include "MuonCalibEventBase/MuonCalibTgcTruthHit.h"
#include "MuonCalibEventBase/MuonCalibCscTruthHit.h"
#include "MuonCalibIdentifier/MuonFixedId.h"
#include "MuonCalibEventBase/MuonCalibEvent.h"
#include "MuonCalibEventBase/MuonCalibPattern.h"
#include "MuonCalibEventBase/MuonCalibSegment.h"
#include "MuonCalibEventBase/MuonCalibRawHitCollection.h"
#include "MuonCalibEventBase/MuonCalibTruthCollection.h"
#include "MuonCalibStl/DeleteObject.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include <iostream>
#include <sstream>

namespace MuonCalib{

  EventHandler::EventHandler() : m_event(nullptr),m_mooreIndices(4,0),m_muonboyIndices(3,100),m_debug(false) { 

  }

  EventHandler::~EventHandler(){
    clear();
  }
  
  void EventHandler::setdbg(const bool dbg_on){
    m_debug = dbg_on;
  }

  void EventHandler::clear() {


    // delete all extended tracks
    AuthorTrackVecIt it = m_extendedTracks.begin();
    AuthorTrackVecIt it_end = m_extendedTracks.end();
    for( ;it!=it_end;++it ){
      std::for_each( it->second.begin(), it->second.end(), DeleteObject() );
    }
    m_extendedTracks.clear();

    // delete all extended segments
    AuthorSegmentVecIt sit = m_extendedSegments.begin();
    AuthorSegmentVecIt sit_end = m_extendedSegments.end();
    for( ;sit!=sit_end;++sit ){
      std::for_each( sit->second.begin(), sit->second.end(), DeleteObject() );
    }
    m_extendedSegments.clear();
    
    // reset event
    m_event = 0;
  }

  void EventHandler::setEvent( const MuonCalibEvent_E& event ) {
    // reset previous event and set new event
    clear();
    m_event = &event;
   
    // create extended tracks
    createExtendedTracks();

    // create extended truth tracks
    createExtendedTruthTracks();

    // create extended tracks
    createExtendedSegments();

    // create extended raw hits
    createExtendedRawHits();

    AuthorTrackVecIt mit = m_extendedTracks.begin();
    AuthorTrackVecIt mit_end = m_extendedTracks.end();
    for( ;mit!=mit_end;++mit ){
      if( mit->first < 500 ) linkTracks( mit->first );

      // associate segments with tracks
      if( mit->first < 99 )  associateSegmentsWithTracks(m_extendedSegments[m_mooreIndices.segment],mit->second);
      else                   associateSegmentsWithTracks(m_extendedSegments[m_muonboyIndices.segment],mit->second);

    }
  }

  std::string EventHandler::dumpRawHits() const {
    std::ostringstream sout;
    sout << m_extendedRawHits.dumpMdtHits();
    return sout.str();
  }

  std::string EventHandler::dumpTracks( const EventHandler::TrackVec& tracks ) const {
    std::ostringstream sout;
    TrkCit trk = tracks.begin();
    TrkCit trk_end = tracks.end();
    TrkCit trk_last = trk_end - 1;
    for( ;trk!=trk_end;++trk){
      sout << (*trk)->dump(); 
      if( trk != trk_last ) sout << std::endl;
    }
    return sout.str();
  }

  std::string EventHandler::dumpSegments( const EventHandler::SegmentVec& segments ) const {
    std::ostringstream sout;
    SegCit seg = segments.begin();
    SegCit seg_end = segments.end();
    SegCit seg_last = seg_end - 1;
    for( ;seg!=seg_end;++seg){
      sout << (*seg)->dump(); 
      if( seg != seg_last ) sout << std::endl;
    }
    return sout.str();
  }

  std::string EventHandler::dumpSegments() const {
    std::ostringstream sout;
    if( m_extendedSegments.count(m_muonboyIndices.segment) && !m_extendedSegments[m_muonboyIndices.segment].empty() ) {
      sout << " Muonboy Segments " << m_extendedSegments[m_muonboyIndices.segment].size() << std::endl
	   << dumpSegments(m_extendedSegments[m_muonboyIndices.segment]);
      if( !m_extendedSegments[m_mooreIndices.segment].empty() ) sout << std::endl; 
    }
    if( m_extendedSegments.count(m_mooreIndices.segment) && !m_extendedSegments[m_mooreIndices.segment].empty() ) {
      sout << " Moore Segments " << m_extendedSegments[m_mooreIndices.segment].size() << std::endl
	   << dumpSegments(m_extendedSegments[m_mooreIndices.segment]);
    }
    return sout.str();
  }

  std::string EventHandler::dumpEvent() const {
    std::ostringstream sout;
    sout << dumpRawHits() << std::endl
	 << dumpTracks()
	 << std::endl << dumpSegments();
    return sout.str();
  }

  std::string EventHandler::dumpTracks() const {
    std::ostringstream sout;

    AuthorTrackVecIt it = m_extendedTracks.begin();
    AuthorTrackVecIt it_end = m_extendedTracks.end();
    for( ;it!=it_end; ++it ){

      // skip standalone tracks at IP 
      if( it->first == 1 || it->first == 2 ) continue;

      sout << " Track Author " << it->first << " number of tracks " << it->second.size();
      // only dump Muon tracks
      if(  it->first < 1000 ) sout << std::endl << dumpTracks(it->second);
      AuthorTrackVecIt it_next = it;
      if( ++it_next != it_end ) sout << std::endl;
    }
    return sout.str();
  }

  void EventHandler::createExtendedRawHits() {
    
    if( !m_event || !m_event->rawHitCollection() ) return;

    AuthorTrackVecIt trkIt = m_extendedTracks.find(m_mooreIndices.track);
    AuthorSegmentVecIt segIt =  m_extendedSegments.find(m_mooreIndices.segment);
   
    m_extendedRawHits = MuonCalibExtendedRawHits( *m_event->rawHitCollection(),
						  segIt != m_extendedSegments.end()? segIt->second : SegmentVec(),
						  trkIt != m_extendedTracks.end() ? trkIt->second : TrackVec() );
    
  }

 
  void EventHandler::createExtendedTracks() {
    
    
    // check that event is set
    if( !m_event ) return;

    // hack to get the associations right...
    MuonCalibExtendedTrack* previousMooreTrack = 0; 

    MuonCalibEvent_E::TrackVec::const_iterator track_it = m_event->beginTrack();
    MuonCalibEvent_E::TrackVec::const_iterator track_it_end = m_event->endTrack();
    for ( ; track_it!=track_it_end; track_it++) {
      
      // reference to current track
      const MuonCalibTrack_E& track = **track_it;
      
      // get author Moore/Muonboy tracks author == 0 and corresponding TrackVec
      int author = track.author();
      TrackVec& trackVec = m_extendedTracks[author];

      // create new extended track and add it to TrackVec
      MuonCalibExtendedTrack* extendedTrack = new MuonCalibExtendedTrack(track,0,0);

      // store previous moore track so we can add the back extrapolated ones if any
      if( author == 0 ){
	previousMooreTrack = extendedTrack;
      }else if ( (author == 1 || author == 2 ) && previousMooreTrack ){
	previousMooreTrack->addAssociatedTrack(extendedTrack);
	extendedTrack->addAssociatedTrack(previousMooreTrack);
      } else if (author == 10 &&(*track_it)->nrHits()==0) {
// Associate MuidSA/MooreExtrapolate to IP to author 0 if no hits on track
        TrackVec& trackVec0 = m_extendedTracks[0];
        double dotprodmax = -1.;
        if(trackVec0.size()>0) {
          int trackcount10 = trackVec.size();
          TrkIt it0 = trackVec0.begin();
          TrkIt it0_end = trackVec0.end();
          int trackcount0 = 0;
          const MuonCalibExtendedTrack* associatedMooreTrack = 0;
          for( ;it0!=it0_end; ++it0 ) {
            trackcount0++;
            if(trackcount10>trackcount0) continue; 
            double dotprod =  extendedTrack->direction().dot((*it0)->direction());
            if(extendedTrack->isAssociated(*it0)) continue; 
            if((*it0)->isAssociated(extendedTrack)) continue; 
            if(dotprod>dotprodmax) {
              dotprodmax = dotprod;
              associatedMooreTrack = *it0; 
              if(dotprodmax>0.8) break; 
            }
          } 
	  if(dotprodmax>0.8 && associatedMooreTrack) {
            extendedTrack->addAssociatedTrack(associatedMooreTrack);
            associatedMooreTrack->addAssociatedTrack(extendedTrack);
          }
        }
      }
      
      trackVec.push_back(extendedTrack);
    }
  }

  void EventHandler::createExtendedTruthTracks() {
    
    
    // check that event is set
    if( !m_event ) return;
//
// Truth track collection 
//

    const MuonCalibTruthCollection* truth = m_event->calibTruthCollection() ;
    if (!truth)
    	return;
    MuonCalibTruthCollection::TruthVecCit truth_it     = truth->truthCollectionBegin();
    MuonCalibTruthCollection::TruthVecCit truth_it_end = truth->truthCollectionEnd();

    if(truth_it == truth_it_end) return;
    

    std::map <int, MuonCalibTruth* > truthTrkME;
    std::map <int, MuonCalibTruth* > truthTrkIP;
    std::map <int, MuonCalibTruth* > truthTrkRest;
    std::map <int, std::set <MuonCalibMdtTruthHit*> > truthTrkMdtHitMap;
    std::map <int, std::set <MuonCalibCscTruthHit*> > truthTrkCscHitMap;
    std::map <int, std::set <MuonCalibRpcTruthHit*> > truthTrkRpcHitMap;
    std::map <int, std::set <MuonCalibTgcTruthHit*> > truthTrkTgcHitMap;

    MsgStream log(Athena::getMessageSvc(),"EventHandler");
    for ( ; truth_it!=truth_it_end; truth_it++) {
        int barcode = ((*truth_it)->barCode());
        if(barcode<=0) continue;
        if(log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<"createExtendedTruthTracks() truth track barcode " << barcode << endmsg;
        if ((*truth_it)->position().perp()< 4000 && std::abs((*truth_it)->position().z())<6000.) {
          if ((*truth_it)->position().perp()< 100 && std::abs((*truth_it)->position().z())<500.)  {
// Close to IP
            truthTrkIP[barcode] = *truth_it;
          } else {
            truthTrkRest[barcode] = *truth_it;
          }
        } else {
        // Take track at Muon Entry
            truthTrkME[barcode] = *truth_it;
        }

    }                       

//
//  Mdt truth hit information 
//

    MuonCalibTruthCollection::MdtTruthVecCit mdt_truth_it = truth->mdtTruthCollectionBegin() ;
    MuonCalibTruthCollection::MdtTruthVecCit mdt_truth_it_end = truth->mdtTruthCollectionEnd() ;

    for(; mdt_truth_it != mdt_truth_it_end; ++mdt_truth_it) {
        int barcode = (*mdt_truth_it)->barCode();
        if(barcode == 0) continue; 
        std::map <int, std::set <MuonCalibMdtTruthHit*> >::iterator it = truthTrkMdtHitMap.find(barcode);
        if (it!=truthTrkMdtHitMap.end()) {
          it->second.insert(*mdt_truth_it);
        } else {
          std::set <MuonCalibMdtTruthHit*> mdtHits;
          mdtHits.insert(*mdt_truth_it);
          truthTrkMdtHitMap[barcode]= mdtHits;
        } 
        std::map <int, MuonCalibTruth* >::iterator itt = truthTrkIP.find(barcode);
        if( itt== truthTrkIP.end() ) {
          itt = truthTrkME.find(barcode);
          if( itt== truthTrkME.end() ) {
            itt = truthTrkRest.find(barcode);
            if( itt== truthTrkRest.end() ) {
              if(log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<" Mdt Truth hit not found with barcode: " << barcode<<endmsg; 
            } else { if(log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<" Mdt Truth hit at Rest " << barcode<<endmsg; }
          } else { if(log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<" Mdt Truth hit at ME " << barcode<<endmsg; }
        } else { if(log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<" Mdt Truth hit at IP " << barcode<<endmsg; }
    } 

    if(truthTrkMdtHitMap.size()==0) return;


//
//  Csc truth hit information 
//

    MuonCalibTruthCollection::CscTruthVecCit csc_truth_it = truth->cscTruthCollectionBegin() ;
    MuonCalibTruthCollection::CscTruthVecCit csc_truth_it_end = truth->cscTruthCollectionEnd() ;

    for(; csc_truth_it != csc_truth_it_end; ++csc_truth_it) {
        int barcode = (*csc_truth_it)->barCode();
        if(barcode == 0) continue; 
        std::map <int, std::set <MuonCalibCscTruthHit*> >::iterator it = truthTrkCscHitMap.find(barcode);
        if (it!=truthTrkCscHitMap.end()) {
          it->second.insert(*csc_truth_it);
        } else {
          std::set <MuonCalibCscTruthHit*> cscHits;
          cscHits.insert(*csc_truth_it);
          truthTrkCscHitMap[barcode]= cscHits;
        } 
        std::map <int, MuonCalibTruth* >::iterator itt = truthTrkIP.find(barcode);
        if( itt== truthTrkIP.end() ) {
          itt = truthTrkME.find(barcode);
          if( itt== truthTrkME.end() ) {
            itt = truthTrkRest.find(barcode);
            if( itt== truthTrkRest.end() ) {
              if(log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<" Csc Truth hit not found with barcode: " << barcode<<endmsg; 
            } else { if(log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<" Csc Truth hit at Rest " << barcode<<endmsg; }
          } else { if(log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<" Csc Truth hit at ME " << barcode<<endmsg; }
        } else { if(log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<" Csc Truth hit at IP " << barcode<<endmsg; }
    } 

//
//  Rpc truth hit information 
//

    MuonCalibTruthCollection::RpcTruthVecCit rpc_truth_it = truth->rpcTruthCollectionBegin() ;
    MuonCalibTruthCollection::RpcTruthVecCit rpc_truth_it_end = truth->rpcTruthCollectionEnd() ;

    for(; rpc_truth_it != rpc_truth_it_end; ++rpc_truth_it) {
        int barcode = (*rpc_truth_it)->barCode();
        if(barcode == 0) continue; 
        std::map <int, std::set <MuonCalibRpcTruthHit*> >::iterator it = truthTrkRpcHitMap.find(barcode);
        if (it!=truthTrkRpcHitMap.end()) {
          it->second.insert(*rpc_truth_it);
        } else {
          std::set <MuonCalibRpcTruthHit*> rpcHits;
          rpcHits.insert(*rpc_truth_it);
          truthTrkRpcHitMap[barcode]= rpcHits;
        } 
        std::map <int, MuonCalibTruth* >::iterator itt = truthTrkIP.find(barcode);
        if( itt== truthTrkIP.end() ) {
          itt = truthTrkME.find(barcode);
          if( itt== truthTrkME.end() ) {
            itt = truthTrkRest.find(barcode);
            if( itt== truthTrkRest.end() ) {
              if(log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<" Rpc Truth hit not found with barcode: " << barcode<<endmsg; 
            } else { if(log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<" Rpc Truth hit at Rest " << barcode<<endmsg; }
          } else { if(log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<" Rpc Truth hit at ME " << barcode<<endmsg; }
        } else { if(log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<" Rpc Truth hit at IP " << barcode<<endmsg; }
    } 

//
//  Tgc truth hit information 
//

    MuonCalibTruthCollection::TgcTruthVecCit tgc_truth_it = truth->tgcTruthCollectionBegin() ;
    MuonCalibTruthCollection::TgcTruthVecCit tgc_truth_it_end = truth->tgcTruthCollectionEnd() ;

    for(; tgc_truth_it != tgc_truth_it_end; ++tgc_truth_it) {
        int barcode = (*tgc_truth_it)->barCode();
        if(barcode == 0) continue; 
        std::map <int, std::set <MuonCalibTgcTruthHit*> >::iterator it = truthTrkTgcHitMap.find(barcode);
        if (it!=truthTrkTgcHitMap.end()) {
          it->second.insert(*tgc_truth_it);
        } else {
          std::set <MuonCalibTgcTruthHit*> tgcHits;
          tgcHits.insert(*tgc_truth_it);
          truthTrkTgcHitMap[barcode]= tgcHits;
        } 
        std::map <int, MuonCalibTruth* >::iterator itt = truthTrkIP.find(barcode);
        if( itt== truthTrkIP.end() ) {
          itt = truthTrkME.find(barcode);
          if( itt== truthTrkME.end() ) {
            itt = truthTrkRest.find(barcode);
            if( itt== truthTrkRest.end() ) {
              if(log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<" Tgc Truth hit not found with barcode: " << barcode<<endmsg; 
            } else { if(log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<" Tgc Truth hit at Rest " << barcode<<endmsg; }
          } else { if(log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<" Tgc Truth hit at ME " << barcode<<endmsg; }
        } else { if(log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<" Tgc Truth hit at IP " << barcode<<endmsg; }
    } 



    std::map <int, MuonCalibTruth* >::iterator itIP = truthTrkIP.begin();
    std::map <int, MuonCalibTruth* >::iterator itIP_end = truthTrkIP.end();
    for(; itIP != itIP_end; ++itIP) {
       int barcode = itIP->first;
       int pdgcode = itIP->second->PDGCode();
       std::map <int, std::set <MuonCalibMdtTruthHit*> >::iterator its = truthTrkMdtHitMap.find(barcode);
       if (its!=truthTrkMdtHitMap.end()) {
          if(log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<" Truth track at IP " << barcode << " with Mdt hits " << its->second.size() << " pdg " << pdgcode<<endmsg; 
         double posx = itIP->second->position().x();  
         double posy = itIP->second->position().y();  
         double posz = itIP->second->position().z();  
         double phi = itIP->second->momentum().phi();
         double theta = itIP->second->momentum().theta();
         double qOverP = 1./(itIP->second->momentum().mag());
         if(pdgcode<0) qOverP = - qOverP; 
         int author = -13;
         if(std::abs(pdgcode)!=13) author = -113;
         if(log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<" pdgcode " << pdgcode << " author " << author<<endmsg;
         int ndof = 0;
         double cov00 = -999.;
         double cov01 = -999.;
         double cov02 = -999.;
         double cov03 = -999.;
         double cov04 = -999.;
         double cov11 = -999.;
         double cov12 = -999.;
         double cov13 = -999.;
         double cov14 = -999.;
         double cov22 = -999.;
         double cov23 = -999.;
         double cov24 = -999.;
         double cov33 = -999.;
         double cov34 = -999.;
         double cov44 = -999.;
         double chi2 = -999.;
         MuonCalibTrack_E* truthTrack = new  MuonCalibTrack_E(posx, posy, posz, phi, theta, qOverP, author, cov00, cov01, cov02, cov03, cov04, cov11, cov12, cov13, cov14, cov22, cov23, cov24, cov33, cov34, cov44, chi2, ndof);

         std::set <MuonCalibMdtTruthHit*>::iterator ith = its->second.begin();  
         std::set <MuonCalibMdtTruthHit*>::iterator ith_end = its->second.end();  
         for(; ith !=ith_end; ++ith) {
           MuonFixedId id = (*ith)->identify(); 
           double driftRadius = (*ith)->driftRadius(); 
           double error = 1.; 
           double pull = 0.; 
           double residual = 1.; 
           Amg::Vector3D pos(0.,(*ith)->positionAlongTube(),0.);
           MuonCalibHit_E* muonHit = new MuonCalibHit_E(id, pos, driftRadius, error, residual, pull, 1 );
           truthTrack->addHit(muonHit);   
           if(log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<" Truth track Add Mdt hit "<<endmsg;     
         }

	 std::map <int, std::set <MuonCalibCscTruthHit*> >::iterator itcs = truthTrkCscHitMap.find(barcode);
         if (itcs!=truthTrkCscHitMap.end()) {
	   std::set <MuonCalibCscTruthHit*>::iterator itc = itcs->second.begin();  
	   std::set <MuonCalibCscTruthHit*>::iterator itc_end = itcs->second.end();  
	   for(; itc !=itc_end; ++itc) {
	     MuonFixedId id = (*itc)->identify(); 
	     double driftRadius = (*itc)->time(); 
	     double error = 1.; 
	     double pull = 0.; 
	     double residual = 1.; 
	     Amg::Vector3D pos(0.,0.,0.);
	     MuonCalibHit_E* muonHit = new MuonCalibHit_E(id, pos, driftRadius, error, residual, pull, 1 );
	     truthTrack->addHit(muonHit);   
	     if(log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<" Truth track Add Csc hit "<<endmsg;     
	   }
         }

	 std::map <int, std::set <MuonCalibRpcTruthHit*> >::iterator itrs = truthTrkRpcHitMap.find(barcode);
         if (itrs!=truthTrkRpcHitMap.end()) {
   	   std::set <MuonCalibRpcTruthHit*>::iterator itr = itrs->second.begin();  
	   std::set <MuonCalibRpcTruthHit*>::iterator itr_end = itrs->second.end();  
	   for(; itr !=itr_end; ++itr) {
	     MuonFixedId id = (*itr)->identify(); 
	     double driftRadius = (*itr)->time(); 
	     double error = 1.; 
	     double pull = 0.; 
	     double residual = 1.; 
	     Amg::Vector3D pos(0.,0.,0.);
	     MuonCalibHit_E* muonHit = new MuonCalibHit_E(id, pos, driftRadius, error, residual, pull, 1 );
	     truthTrack->addHit(muonHit);   
	     if(log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<" Truth track Add Rpc hit "<<endmsg;     
	   }
         }

	 std::map <int, std::set <MuonCalibTgcTruthHit*> >::iterator itts = truthTrkTgcHitMap.find(barcode);
           if (itts!=truthTrkTgcHitMap.end()) {
	     std::set <MuonCalibTgcTruthHit*>::iterator itt = itts->second.begin();  
	     std::set <MuonCalibTgcTruthHit*>::iterator itt_end = itts->second.end();  
	     for(; itt !=itt_end; ++itt) {
	       MuonFixedId id = (*itt)->identify(); 
	       double driftRadius = (*itt)->time(); 
	       double error = 1.; 
	       double pull = 0.; 
	       double residual = 1.; 
	       Amg::Vector3D pos(0.,0.,0.);
	       MuonCalibHit_E* muonHit = new MuonCalibHit_E(id, pos, driftRadius, error, residual, pull, 1 );
	       truthTrack->addHit(muonHit);   
	       if(log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<" Truth track Add Tgc hit "<<endmsg;     
	     }
         }

         TrackVec& trackVec = m_extendedTracks[author];
         MuonCalibExtendedTrack* extendedTrack = new MuonCalibExtendedTrack(*truthTrack,pdgcode,barcode);
         trackVec.push_back(extendedTrack);
// dump truth track
         if(m_debug) extendedTrack->dump();

         delete truthTrack;
         truthTrack=nullptr;

       }
    } 

    std::map <int, MuonCalibTruth* >::iterator itME = truthTrkME.begin();
    std::map <int, MuonCalibTruth* >::iterator itME_end = truthTrkME.end();
    for(; itME != itME_end; ++itME) {
       int barcode = itME->first;
       int pdgcode = itME->second->PDGCode();
       std::map <int, std::set <MuonCalibMdtTruthHit*> >::iterator its = truthTrkMdtHitMap.find(barcode);
       if (its!=truthTrkMdtHitMap.end()) {
          if(log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<" Truth track at ME " << barcode << " with Mdt hits " << its->second.size() << " pdg " << pdgcode<<endmsg; 
         double posx = itME->second->position().x();  
         double posy = itME->second->position().y();  
         double posz = itME->second->position().z();  
         double phi = itME->second->momentum().phi();
         double theta = itME->second->momentum().theta();
         double qOverP = 1./(itME->second->momentum().mag());
         if(pdgcode<0) qOverP = - qOverP; 
         int author = -1013;
         if(std::abs(pdgcode)!=13) author = -1113;
         if(log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<" pdgcode " << pdgcode << " author " << author<<endmsg;
         int ndof = 0;
         double cov00 = -999.;
         double cov01 = -999.;
         double cov02 = -999.;
         double cov03 = -999.;
         double cov04 = -999.;
         double cov11 = -999.;
         double cov12 = -999.;
         double cov13 = -999.;
         double cov14 = -999.;
         double cov22 = -999.;
         double cov23 = -999.;
         double cov24 = -999.;
         double cov33 = -999.;
         double cov34 = -999.;
         double cov44 = -999.;
         double chi2 = -999.;
         MuonCalibTrack_E* truthTrack = new  MuonCalibTrack_E(posx, posy, posz, phi, theta, qOverP, author, cov00, cov01, cov02, cov03, cov04, cov11, cov12, cov13, cov14, cov22, cov23, cov24, cov33, cov34, cov44, chi2, ndof);

         std::set <MuonCalibMdtTruthHit*>::iterator ith = its->second.begin();  
         std::set <MuonCalibMdtTruthHit*>::iterator ith_end = its->second.end();  
         for(; ith !=ith_end; ++ith) {
           MuonFixedId id = (*ith)->identify(); 
           double driftRadius = (*ith)->driftRadius(); 
           double error = 1.; 
           double pull = 0.; 
           double residual = 1.; 
           Amg::Vector3D pos(0.,(*ith)->positionAlongTube(),0.);
           MuonCalibHit_E* muonHit = new MuonCalibHit_E(id, pos, driftRadius, error, residual, pull, 1 );
           truthTrack->addHit(muonHit);   
           if(log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<" Truth track Add Mdt hit "<<endmsg;     
         }


	 std::map <int, std::set <MuonCalibCscTruthHit*> >::iterator itcs = truthTrkCscHitMap.find(barcode);
         if (itcs!=truthTrkCscHitMap.end()) {
	   std::set <MuonCalibCscTruthHit*>::iterator itc = itcs->second.begin();  
	   std::set <MuonCalibCscTruthHit*>::iterator itc_end = itcs->second.end();  
	   for(; itc !=itc_end; ++itc) {
	     MuonFixedId id = (*itc)->identify(); 
	     double driftRadius = (*itc)->time(); 
	     double error = 1.; 
	     double pull = 0.; 
	     double residual = 1.; 
	     Amg::Vector3D pos(0.,0.,0.);
	     MuonCalibHit_E* muonHit = new MuonCalibHit_E(id, pos, driftRadius, error, residual, pull, 1 );
	     truthTrack->addHit(muonHit);   
	     if(log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<" Truth track Add Csc hit "<<endmsg;     
	   }
         }
	 std::map <int, std::set <MuonCalibRpcTruthHit*> >::iterator itrs = truthTrkRpcHitMap.find(barcode);
         if (itrs!=truthTrkRpcHitMap.end()) {
	   std::set <MuonCalibRpcTruthHit*>::iterator itr = itrs->second.begin();  
	   std::set <MuonCalibRpcTruthHit*>::iterator itr_end = itrs->second.end();  
	   for(; itr !=itr_end; ++itr) {
	     MuonFixedId id = (*itr)->identify(); 
	     double driftRadius = (*itr)->time(); 
	     double error = 1.; 
	     double pull = 0.; 
	     double residual = 1.; 
	     Amg::Vector3D pos(0.,0.,0.);
	     MuonCalibHit_E* muonHit = new MuonCalibHit_E(id, pos, driftRadius, error, residual, pull, 1 );
	     truthTrack->addHit(muonHit);   
	     if(log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<" Truth track Add Rpc hit "<<endmsg;     
	   }
	 }

	 std::map <int, std::set <MuonCalibTgcTruthHit*> >::iterator itts = truthTrkTgcHitMap.find(barcode);
         if (itts!=truthTrkTgcHitMap.end()) {
	   std::set <MuonCalibTgcTruthHit*>::iterator itt = itts->second.begin();  
	   std::set <MuonCalibTgcTruthHit*>::iterator itt_end = itts->second.end();  
	   for(; itt !=itt_end; ++itt) {
	     MuonFixedId id = (*itt)->identify(); 
	     double driftRadius = (*itt)->time(); 
	     double error = 1.; 
	     double pull = 0.; 
	     double residual = 1.; 
	     Amg::Vector3D pos(0.,0.,0.);
	     MuonCalibHit_E* muonHit = new MuonCalibHit_E(id, pos, driftRadius, error, residual, pull, 1 );
	     truthTrack->addHit(muonHit);   
	     if(log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<" Truth track Add Tgc hit "<<endmsg;     
	   }
         }

         TrackVec& trackVec = m_extendedTracks[author];
         MuonCalibExtendedTrack* extendedTrack = new MuonCalibExtendedTrack(*truthTrack,pdgcode,barcode);
         trackVec.push_back(extendedTrack);
// dump truth track
         if(m_debug) extendedTrack->dump(); 

         delete truthTrack;
         truthTrack=nullptr;

       }
    } 
  }

  void EventHandler::createExtendedSegments() {
    
    // check that event is set
    if( !m_event ) return;

    MuonCalibEvent::MCPVecCit pit = m_event->patternBegin();
    MuonCalibEvent::MCPVecCit pit_end = m_event->patternEnd();
    for( ;pit!=pit_end;++pit ){
      const MuonCalibPattern& pat = **pit;
      MuonCalibPattern::MuonSegCit sit = pat.muonSegBegin();
      MuonCalibPattern::MuonSegCit sit_end = pat.muonSegEnd();
      for( ;sit!=sit_end;++sit ){
	const MuonCalibSegment& seg = **sit;
	
	// get author 
	int author = seg.author();
	SegmentVec& segVec = m_extendedSegments[author];

	// create new extended segment
	MuonCalibExtendedSegment* extendedSegment = new MuonCalibExtendedSegment(seg);
	segVec.push_back(extendedSegment);
      }
    }    
  }
    
  void EventHandler::associateSegmentsWithTracks( SegmentVec& segments, TrackVec& tracks ) {
    TrkIt trkIt = tracks.begin();
    TrkIt trkIt_end = tracks.end();
    for( ;trkIt != trkIt_end; ++trkIt ){
      SegIt sit = segments.begin();
      SegIt sit_end = segments.end();
      std::map <const MuonCalibExtendedSegment*, int>  segmentTrackMatch;
      for( ;sit!=sit_end;++sit ){
        int match = associateSegmentWithTrack( **sit,**trkIt );
	if( match > 200) {
          segmentTrackMatch[*sit] = match;
        }
      }
      std::map <const MuonCalibExtendedSegment*, int>:: iterator mit = segmentTrackMatch.begin();
      std::map <const MuonCalibExtendedSegment*, int>:: iterator mit_end = segmentTrackMatch.end();
      for( ;mit!=mit_end;++mit ){
        std::map <const MuonCalibExtendedSegment*, int>:: iterator mit2 = mit;
        for(;mit2!=mit_end;++mit2){
           if(mit2!=mit) {
             if(segmentOverlap(*(mit->first),*(mit2->first))>1) {
               if(mit->second>mit2->second) {
                 segmentTrackMatch[mit2->first] = -1;
               } else if (mit->second==mit2->second) {
                 if(mit->first->chi2()<mit2->first->chi2()) {
                   segmentTrackMatch[mit2->first] = -1;
                 } else {
                   segmentTrackMatch[mit->first] = -1;
                   break;
                 } 
               } else {
                 segmentTrackMatch[mit->first] = -1;
                 break;
               } 
             }       
           }
        }
        if(mit->second>0) {
          mit->first->addTrack(*trkIt);
	  (*trkIt)->addSegment(mit->first);
	}
      }
    }
  }
  int EventHandler::segmentOverlap( const MuonCalibExtendedSegment& segment1, const MuonCalibExtendedSegment& segment2 ) {

    int overlap = 0;
    if(segment1.mdtHitsOnTrack()>0&&segment2.mdtHitsOnTrack()>0) {

      std::vector <MuonFixedId> segmentIds(segment1.mdtHitsOnTrack());

      MuonCalibSegment::MdtHitCit mdtIt = segment1.mdtHOTBegin();
      MuonCalibSegment::MdtHitCit mdtIt_end = segment1.mdtHOTEnd();
      for( ;mdtIt!=mdtIt_end;++mdtIt ){
        const MdtCalibHitBase& hit = **mdtIt;
        MuonFixedId id = hit.identify();
        if( !id.isValid() ) {
          MsgStream log(Athena::getMessageSvc(),"EventHandler");
          log<<MSG::WARNING<<"segmentOverlap(), invalid MDT id! "<<endmsg;
          continue;
        }
        segmentIds.push_back(id);
      }
      mdtIt = segment2.mdtHOTBegin();
      mdtIt_end = segment2.mdtHOTEnd();
      for( ;mdtIt!=mdtIt_end;++mdtIt ){
        const MdtCalibHitBase& hit = **mdtIt;
        MuonFixedId id = hit.identify();
        if( !id.isValid() ) {
          MsgStream log(Athena::getMessageSvc(),"EventHandler");
          log<<MSG::WARNING<<"segmentOverlap(), invalid MDT id! "<<endmsg;
          continue;
        }
        std::vector <MuonFixedId> ::iterator it = std::find( segmentIds.begin(), segmentIds.end(), id );
        if( it == segmentIds.end() ) {
        } else overlap++;
      }
      return overlap;
    } else if (segment1.cscHitsOnTrack()>0&&segment2.cscHitsOnTrack()>0) {

      std::vector <MuonFixedId> segmentIds(segment1.cscHitsOnTrack());

      MuonCalibSegment::CscHitCit cscIt = segment1.cscHOTBegin();
      MuonCalibSegment::CscHitCit cscIt_end = segment1.cscHOTEnd();
      for( ;cscIt!=cscIt_end;++cscIt ){
        const CscCalibHitBase& hit = **cscIt;
        MuonFixedId id = hit.identify();
        if( !id.isValid() ) {
          MsgStream log(Athena::getMessageSvc(),"EventHandler");
          log<<MSG::WARNING<<"segmentOverlap(), invalid CSC id! "<<endmsg;
          continue;
        }
        segmentIds.push_back(id);
      }

      cscIt = segment2.cscHOTBegin();
      cscIt_end = segment2.cscHOTEnd();
      for( ;cscIt!=cscIt_end;++cscIt ){
        const CscCalibHitBase& hit = **cscIt;
        MuonFixedId id = hit.identify();
        if( !id.isValid() ) {
          MsgStream log(Athena::getMessageSvc(),"EventHandler");
          log<<MSG::WARNING<<"segmentOverlap(), invalid CSC id! "<<endmsg;
          continue;
        }
        std::vector <MuonFixedId> ::iterator it = std::find( segmentIds.begin(), segmentIds.end(), id );
        if( it == segmentIds.end() ) {
        } else overlap++;
      }
      return overlap;

    } else {
      return overlap;
    }
    
  }
  int EventHandler::associateSegmentWithTrack( const MuonCalibExtendedSegment& segment, const MuonCalibExtendedTrack& track ) {

    double minRadius = 1.; // the sign of all hits with a drift radius smaller that minRadius is not taken into account
    unsigned int noppositeSign = 0;
    unsigned int nsameSign = 0;
    unsigned int nnotOnTrack = 0;
    MuonCalibSegment::MdtHitCit mdtIt = segment.mdtHOTBegin();
    MuonCalibSegment::MdtHitCit mdtIt_end = segment.mdtHOTEnd();
    for( ;mdtIt!=mdtIt_end;++mdtIt ){
      const MdtCalibHitBase& hit = **mdtIt;
      MuonFixedId id = hit.identify();
      if( !id.isValid() ) {
        MsgStream log(Athena::getMessageSvc(),"EventHandler");
        log<<MSG::WARNING<<"associateSegmentWithTrack(), invalid MDT id! "<<endmsg;
        continue;
      }
      // look up hit on track
      IdHitMap::const_iterator trkHit = track.idHitMap().find(id);
      if( trkHit != track.idHitMap().end() ){
	// found hit
	double signSeg = hit.driftRadius() < -minRadius ? -1. : 1.;
	double signTrk = trkHit->second->driftRadius() < -minRadius ? -1. : 1.;
	if( signSeg == signTrk ) ++nsameSign;
	else                     ++noppositeSign;
      }else{
	++nnotOnTrack;
      }
    }

    MuonCalibSegment::CscHitCit cscIt = segment.cscHOTBegin();
    MuonCalibSegment::CscHitCit cscIt_end = segment.cscHOTEnd();
    for( ;cscIt!=cscIt_end;++cscIt ){
      const CscCalibHitBase& hit = **cscIt;
      MuonFixedId id = hit.identify();
      if( !id.isValid() ) {
        MsgStream log(Athena::getMessageSvc(),"EventHandler");
        log<<MSG::WARNING<<"associateSegmentWithTrack(), invalid CSC id! "<<endmsg;
        continue;
      }
      // look up hit on track
      IdHitMap::const_iterator trkHit = track.idHitMap().find(id);
      if( trkHit != track.idHitMap().end() ){
        // found hit
        ++nsameSign;
      }else{
        ++nnotOnTrack;
      }
    }

    return nnotOnTrack+10*noppositeSign+100*nsameSign;

  }

  void EventHandler::linkTracks( int referenceIndex ) {

    // check if author found in list
    AuthorTrackVecIt pos = m_extendedTracks.find(referenceIndex);
    if( pos == m_extendedTracks.end() ) return;
    
    TrackVec& refTracks = pos->second;

    // loop over reference tracks and associate other authors
    TrkIt rit = refTracks.begin();
    TrkIt rit_end = refTracks.end();
    for( ;rit!=rit_end;++rit ){
      
      // loop over authors
      AuthorTrackVecIt ait = m_extendedTracks.begin();
      AuthorTrackVecIt ait_end = m_extendedTracks.end();
      for( ;ait!=ait_end;++ait ){
	
	// skip same collection
	if( ait->first == referenceIndex ) continue;

	TrackVec& tracks = ait->second;
	
	match( **rit, tracks );
	
      }
    }
  }

  void EventHandler::match( const MuonCalibExtendedTrack& reference, EventHandler::TrackVec& tracks ) {
    
    const MuonCalibExtendedTrack* bestMatch = 0;
    MuonCalibExtendedTrackOverlap bestOverlap;
    // loop over tracks and calcualte overlap with reference
    TrkIt it = tracks.begin();
    TrkIt it_end = tracks.end();
    for( ;it!=it_end;++it ){

      // check whether the two tracks are not already associated
      if( reference.isAssociated(*it) ) continue;

      MuonCalibExtendedTrackOverlap overlap = reference.calculateHitOverlap(**it);
      if(  overlap.sharedPrecisionHits() > 0 ){
	if( overlap.sharedPrecisionHits() > bestOverlap.sharedPrecisionHits() ) {
	  bestMatch = *it;
	  bestOverlap = overlap;
	} 
      }
    }
    
    if( bestMatch ){
      reference.addAssociatedTrack(bestMatch);
      bestMatch->addAssociatedTrack(&reference);
    }
  } 


} //namespace MuonCalib


