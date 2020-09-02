/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraTreeEvent/MuonCalibEvent_E.h"
#include "MuonCalibExtraTreeEvent/MuonCalibTrack_E.h"
#include "MuonCalibExtraTreeEvent/MuonCalibCaloHit.h"
#include "MuonCalibExtraTreeEvent/MuonCalibTriggerInfo.h"
#include "MuonCalibExtraTreeEvent/MuonCalibMUCTPI.h"
#include "MuonCalibExtraTreeEvent/MuonCalibRpcSectorLogic.h"
#include "MuonCalibEventBase/MuonCalibRawRpcCoin.h"
#include "MuonCalibEventBase/MuonCalibRawRpcTriggerHit.h"

#include "MuonCalibEventBase/MuonCalibEvent.h"
#include "MuonCalibStl/DeleteObject.h"

namespace MuonCalib {
  
  MuonCalibEvent_E::~MuonCalibEvent_E() {    
    std::for_each( beginPhiPat(), endPhiPat(), DeleteObject() ) ;
    m_phiPats.clear();
    std::for_each( beginTrack(), endTrack(), DeleteObject() ) ;
    m_tracks.clear();

    std::for_each( beginMBTS(), endMBTS(), DeleteObject() ) ;
    m_mbts.clear();
    std::for_each( beginL1Calo(), endL1Calo(), DeleteObject() ) ;
    m_l1calo.clear();
    std::for_each( beginCtp(), endCtp(), DeleteObject() ) ;
    m_ctp.clear();
    std::for_each( beginMUCTPI(), endMUCTPI(), DeleteObject() ) ;
    m_mctpi.clear();
    std::for_each( beginRpcSL(), endRpcSL(), DeleteObject() ) ;
    m_rpcsl.clear();
    std::for_each( beginRpcCoin(), endRpcCoin(), DeleteObject() ) ;
    m_rpcco.clear();
    std::for_each( beginRawRpcTrig(), endRawRpcTrig(), DeleteObject() ) ;
    m_rpctr.clear();
  }

  MuonCalibEvent_E::MuonCalibEvent_E(const MuonCalibEvent& event) : MuonCalibEvent(event), m_phiPats(0), m_tracks(0), m_ctp(0), m_mctpi(0), m_rpcsl(0), m_rpcco(0), m_rpctr(0), m_mbts(0), m_l1calo(0) {
  }
  
  
  MuonCalibEvent_E::MuonCalibEvent_E(const MuonCalibEvent_E& event) : MuonCalibEvent(event) {
    PhiPatVec::const_iterator phi = event.beginPhiPat();
    PhiPatVec::const_iterator phi_end = event.endPhiPat();
    for(; phi!=phi_end; ++phi){
      m_phiPats.push_back( new MuonCalibTrack_E(**phi) );
    }

    TrackVec::const_iterator trk = event.beginTrack();
    TrackVec::const_iterator trk_end = event.endTrack();
    for(; trk!=trk_end; ++trk){
      m_tracks.push_back( new MuonCalibTrack_E(**trk) );
    }

    CtpVec::const_iterator tit = event.beginCtp();
    CtpVec::const_iterator tit_end = event.endCtp();
    for(; tit!=tit_end; ++tit){
      m_ctp.push_back( new MuonCalibTriggerInfo(**tit) );
    }

    MUCTPIVec::const_iterator mit = event.beginMUCTPI();
    MUCTPIVec::const_iterator mit_end = event.endMUCTPI();
    for(; mit!=mit_end; ++mit){
      m_mctpi.push_back( new MuonCalibMUCTPI(**mit) );
    }

    RpcSLVec::const_iterator rit = event.beginRpcSL();
    RpcSLVec::const_iterator rit_end = event.endRpcSL();
    for(; rit!=rit_end; ++rit){
      m_rpcsl.push_back( new MuonCalibRpcSectorLogic(**rit) );
    }

    RpcCoinVec::const_iterator cit = event.beginRpcCoin();
    RpcCoinVec::const_iterator cit_end = event.endRpcCoin();
    for(; cit!=cit_end; ++cit){
      m_rpcco.push_back( new MuonCalibRawRpcCoin(**cit) );
    }

    RawRpcTrigVec::const_iterator trit = event.beginRawRpcTrig();
    RawRpcTrigVec::const_iterator trit_end = event.endRawRpcTrig();
    for(; trit!=trit_end; ++trit){
      m_rpctr.push_back( new MuonCalibRawRpcTriggerHit(**trit) );
    }
    
    MBTSVec::const_iterator chit = event.beginMBTS();
    MBTSVec::const_iterator chit_end = event.endMBTS();
    for(; chit!=chit_end; ++chit){
      m_mbts.push_back( new MuonCalibCaloHit(**chit) );
    }

    L1CaloVec::const_iterator lhit = event.beginL1Calo();
    L1CaloVec::const_iterator lhit_end = event.endL1Calo();
    for(; lhit!=lhit_end; ++lhit){
      m_l1calo.push_back( new MuonCalibCaloHit(**lhit) );
    }

  }

  MuonCalibEvent_E& MuonCalibEvent_E::operator=(const MuonCalibEvent_E &right) {
    if(this!=&right) {
      m_phiPats = right.m_phiPats;
      m_tracks = right.m_tracks;
      m_ctp = right.m_ctp;
      m_mctpi = right.m_mctpi;
      m_rpcsl = right.m_rpcsl;
      m_rpcco = right.m_rpcco;
      m_rpctr = right.m_rpctr;
      m_mbts = right.m_mbts;
      m_l1calo = right.m_l1calo;
    }
    return *this;
  }

  void MuonCalibEvent_E::addPattern(  MuonCalibTrack_E* pat) {
    if( pat ){
      m_phiPats.push_back(pat) ;
    }
  }

  void MuonCalibEvent_E::addTrack(  MuonCalibTrack_E* trk) {
    if( trk ){
      m_tracks.push_back(trk) ;
    }
  }

  void MuonCalibEvent_E::addCtp(  MuonCalibTriggerInfo* trig) {
    if( trig ){
      m_ctp.push_back(trig) ;
    }
  }

  void MuonCalibEvent_E::addMUCTPI(  MuonCalibMUCTPI* mctpi) {
    if( mctpi ){
      m_mctpi.push_back(mctpi) ;
    }
  }

  void MuonCalibEvent_E::addRpcSL(  MuonCalibRpcSectorLogic* rpcsl) {
    if( rpcsl ){
      m_rpcsl.push_back(rpcsl) ;
    }
  }

  void MuonCalibEvent_E::addRpcCoin(  MuonCalibRawRpcCoin* rpcco) {
    if( rpcco ){
      m_rpcco.push_back(rpcco) ;
    }
  }

  void MuonCalibEvent_E::addRawRpcTrigHit(  MuonCalibRawRpcTriggerHit* rpctr) {
    if( rpctr ){
      m_rpctr.push_back(rpctr) ;
    }
  }

  void MuonCalibEvent_E::addMBTS(  MuonCalibCaloHit* hit) {
    if( hit ){
      m_mbts.push_back(hit) ;
    }
  }

  void MuonCalibEvent_E::addL1Calo(  MuonCalibCaloHit* hit) {
    if( hit ){
      m_l1calo.push_back(hit) ;
    }
  }

} //namespace MuonCalib

