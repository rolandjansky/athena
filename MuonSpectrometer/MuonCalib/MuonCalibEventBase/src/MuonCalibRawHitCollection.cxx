/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibEventBase/MuonCalibRawHitCollection.h"
#include "MuonCalibEventBase/MuonCalibRawMdtHit.h"
#include "MuonCalibEventBase/MuonCalibRawRpcHit.h"
#include "MuonCalibEventBase/MuonCalibRawTgcHit.h"
#include "MuonCalibEventBase/MuonCalibRawTgcCoin.h"
#include "MuonCalibEventBase/MuonCalibRawCscHit.h"
#include <algorithm>

#include "MuonCalibStl/DeleteObject.h"

namespace MuonCalib {

  MuonCalibRawHitCollection::MuonCalibRawHitCollection() {
    MuonCalibRawMdtHitVec newMdt(0);
    m_rawMdtHitVec= newMdt;
    MuonCalibRawRpcHitVec newRpc(0);
    m_rawRpcHitVec= newRpc;
    MuonCalibRawTgcHitVec newTgc(0);
    m_rawTgcHitVec= newTgc;
    MuonCalibRawTgcCoinVec newTgcCoin(0);
    m_rawTgcCoinVec= newTgcCoin;
    MuonCalibRawCscHitVec newCsc(0);
    m_rawCscHitVec= newCsc;
  }

  MuonCalibRawHitCollection::~MuonCalibRawHitCollection() {
    std::for_each( rawMdtHitCollectionBegin(), rawMdtHitCollectionEnd(), DeleteObject()  ); 
    m_rawMdtHitVec.clear();
    std::for_each( rawRpcHitCollectionBegin(), rawRpcHitCollectionEnd(), DeleteObject()  ); 
    m_rawRpcHitVec.clear();
    std::for_each( rawTgcHitCollectionBegin(), rawTgcHitCollectionEnd(), DeleteObject()  ); 
    m_rawTgcHitVec.clear();
    std::for_each( rawTgcCoinCollectionBegin(), rawTgcCoinCollectionEnd(), DeleteObject()  ); 
    m_rawTgcCoinVec.clear();
    std::for_each( rawCscHitCollectionBegin(), rawCscHitCollectionEnd(), DeleteObject()  ); 
    m_rawCscHitVec.clear();
  } 
  
  MuonCalibRawHitCollection::MuonCalibRawHitCollection(const MuonCalibRawHitCollection &muonRawHitCollection ) {
    MuonCalibRawMdtHitVecCit mdt_it = muonRawHitCollection.rawMdtHitCollectionBegin();
    MuonCalibRawMdtHitVecCit mdt_it_end = muonRawHitCollection.rawMdtHitCollectionEnd();
    for(; mdt_it!= mdt_it_end; ++mdt_it ){
      m_rawMdtHitVec.push_back( new MuonCalibRawMdtHit(**mdt_it) ); 
    } 
    MuonCalibRawRpcHitVecCit rpc_it = muonRawHitCollection.rawRpcHitCollectionBegin();
    MuonCalibRawRpcHitVecCit rpc_it_end = muonRawHitCollection.rawRpcHitCollectionEnd();
    for(; rpc_it!= rpc_it_end; ++rpc_it ){
      m_rawRpcHitVec.push_back( new MuonCalibRawRpcHit(**rpc_it) ); 
    } 
    MuonCalibRawTgcHitVecCit tgc_it = muonRawHitCollection.rawTgcHitCollectionBegin();
    MuonCalibRawTgcHitVecCit tgc_it_end = muonRawHitCollection.rawTgcHitCollectionEnd();
    for(; tgc_it!= tgc_it_end; ++tgc_it ){
      m_rawTgcHitVec.push_back( new MuonCalibRawTgcHit(**tgc_it) ); 
    } 
    MuonCalibRawTgcCoinVecCit tgcCoin_it = muonRawHitCollection.rawTgcCoinCollectionBegin();
    MuonCalibRawTgcCoinVecCit tgcCoin_it_end = muonRawHitCollection.rawTgcCoinCollectionEnd();
    for(; tgcCoin_it!= tgcCoin_it_end; ++tgcCoin_it ){
      m_rawTgcCoinVec.push_back( new MuonCalibRawTgcCoin(**tgcCoin_it) ); 
    } 
    MuonCalibRawCscHitVecCit csc_it = muonRawHitCollection.rawCscHitCollectionBegin();
    MuonCalibRawCscHitVecCit csc_it_end = muonRawHitCollection.rawCscHitCollectionEnd();
    for(; csc_it!= csc_it_end; ++csc_it ){
      m_rawCscHitVec.push_back( new MuonCalibRawCscHit(**csc_it) ); 
    } 
  }
  
  MuonCalibRawHitCollection& MuonCalibRawHitCollection::operator=( const MuonCalibRawHitCollection &rhs ){
    if(this!=&rhs) {
      MuonCalibRawMdtHitVec temp_rawMdtHitVec;
      MuonCalibRawMdtHitVecCit mdt_it = rhs.rawMdtHitCollectionBegin();
      MuonCalibRawMdtHitVecCit mdt_it_end = rhs.rawMdtHitCollectionEnd();
      for(; mdt_it!=mdt_it_end; ++mdt_it ){
	temp_rawMdtHitVec.push_back( new MuonCalibRawMdtHit(**mdt_it) );
      }
      m_rawMdtHitVec = temp_rawMdtHitVec;

      MuonCalibRawRpcHitVec temp_rawRpcHitVec;
      MuonCalibRawRpcHitVecCit rpc_it = rhs.rawRpcHitCollectionBegin();
      MuonCalibRawRpcHitVecCit rpc_it_end = rhs.rawRpcHitCollectionEnd();
      for(; rpc_it!=rpc_it_end; ++rpc_it ){
	temp_rawRpcHitVec.push_back( new MuonCalibRawRpcHit(**rpc_it) );
      }
      m_rawRpcHitVec = temp_rawRpcHitVec;

      MuonCalibRawTgcHitVec temp_rawTgcHitVec;
      MuonCalibRawTgcHitVecCit tgc_it = rhs.rawTgcHitCollectionBegin();
      MuonCalibRawTgcHitVecCit tgc_it_end = rhs.rawTgcHitCollectionEnd();
      for(; tgc_it!=tgc_it_end; ++tgc_it ){
	temp_rawTgcHitVec.push_back( new MuonCalibRawTgcHit(**tgc_it) );
      }
      m_rawTgcHitVec = temp_rawTgcHitVec;

      MuonCalibRawTgcCoinVec temp_rawTgcCoinVec;
      MuonCalibRawTgcCoinVecCit tgcCoin_it = rhs.rawTgcCoinCollectionBegin();
      MuonCalibRawTgcCoinVecCit tgcCoin_it_end = rhs.rawTgcCoinCollectionEnd();
      for(; tgcCoin_it!=tgcCoin_it_end; ++tgcCoin_it ){
	temp_rawTgcCoinVec.push_back( new MuonCalibRawTgcCoin(**tgcCoin_it) );
      }
      m_rawTgcCoinVec = temp_rawTgcCoinVec;

      MuonCalibRawCscHitVec temp_rawCscHitVec;
      MuonCalibRawCscHitVecCit csc_it = rhs.rawCscHitCollectionBegin();
      MuonCalibRawCscHitVecCit csc_it_end = rhs.rawCscHitCollectionEnd();
      for(; csc_it!=csc_it_end; ++csc_it ){
	temp_rawCscHitVec.push_back( new MuonCalibRawCscHit(**csc_it) );
      }
      m_rawCscHitVec = temp_rawCscHitVec;
    } 
    return (*this);
  }
  
}  //namespace MuonCalib
