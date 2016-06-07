/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibEventBase/MuonCalibTruthCollection.h"
#include "MuonCalibEventBase/MuonCalibTruth.h"
#include "MuonCalibEventBase/MuonCalibMdtTruthHit.h"
#include "MuonCalibEventBase/MuonCalibRpcTruthHit.h"
#include "MuonCalibEventBase/MuonCalibTgcTruthHit.h"
#include "MuonCalibEventBase/MuonCalibCscTruthHit.h"
#include "MuonCalibStl/DeleteObject.h"
#include <algorithm>

namespace MuonCalib {

  MuonCalibTruthCollection::MuonCalibTruthCollection() {
    TruthVec newVec(0);
    m_truthVec= newVec;
    MdtTruthVec newMdtVec(0);
    m_mdtTruthVec = newMdtVec;
    RpcTruthVec newRpcVec(0);
    m_rpcTruthVec = newRpcVec;
    TgcTruthVec newTgcVec(0);
    m_tgcTruthVec = newTgcVec;
    CscTruthVec newCscVec(0);
    m_cscTruthVec = newCscVec;
  }

  MuonCalibTruthCollection::~MuonCalibTruthCollection() {
    std::for_each( truthCollectionBegin(), truthCollectionEnd(), DeleteObject()  ) ; 
    m_truthVec.clear();

    std::for_each( mdtTruthCollectionBegin(), mdtTruthCollectionEnd(), DeleteObject() );
    m_mdtTruthVec.clear();

    std::for_each( rpcTruthCollectionBegin(), rpcTruthCollectionEnd(), DeleteObject() );
    m_rpcTruthVec.clear();

    std::for_each( tgcTruthCollectionBegin(), tgcTruthCollectionEnd(), DeleteObject() );
    m_tgcTruthVec.clear();

    std::for_each( cscTruthCollectionBegin(), cscTruthCollectionEnd(), DeleteObject() );
    m_cscTruthVec.clear();

  } 
  
  MuonCalibTruthCollection::MuonCalibTruthCollection(const MuonCalibTruthCollection& truthCollection ) {
    TruthVecCit it = truthCollection.truthCollectionBegin() ;
    TruthVecCit it_end = truthCollection.truthCollectionEnd() ;
    for( ; it!= it_end ; ++it ){
      m_truthVec.push_back( new MuonCalibTruth(**it) ); 

    } 

    MdtTruthVecCit mdt_it = truthCollection.mdtTruthCollectionBegin();
    MdtTruthVecCit mdt_it_end = truthCollection.mdtTruthCollectionEnd();
    for( ; mdt_it!=mdt_it_end; ++mdt_it){
      m_mdtTruthVec.push_back( new MuonCalibMdtTruthHit( **mdt_it ) );
    }
    RpcTruthVecCit rpc_it = truthCollection.rpcTruthCollectionBegin();
    RpcTruthVecCit rpc_it_end = truthCollection.rpcTruthCollectionEnd();
    for( ; rpc_it!=rpc_it_end; ++rpc_it){
      m_rpcTruthVec.push_back( new MuonCalibRpcTruthHit( **rpc_it ) );
    }
    CscTruthVecCit csc_it = truthCollection.cscTruthCollectionBegin();
    CscTruthVecCit csc_it_end = truthCollection.cscTruthCollectionEnd();
    for( ; csc_it!=csc_it_end; ++csc_it){
      m_cscTruthVec.push_back( new MuonCalibCscTruthHit( **csc_it ) );
    }
    TgcTruthVecCit tgc_it = truthCollection.tgcTruthCollectionBegin();
    TgcTruthVecCit tgc_it_end = truthCollection.tgcTruthCollectionEnd();
    for( ; tgc_it!=tgc_it_end; ++tgc_it){
      m_tgcTruthVec.push_back( new MuonCalibTgcTruthHit( **tgc_it ) );
    }

  }
  
  MuonCalibTruthCollection& MuonCalibTruthCollection::operator=( const MuonCalibTruthCollection& rhs ){
    if(this!=&rhs) {
      TruthVec temp_truthVec ;
      TruthVecCit it = rhs.truthCollectionBegin() ;
      TruthVecCit it_end = rhs.truthCollectionEnd() ;
      for( ; it!=it_end ; ++it ){
	temp_truthVec.push_back( new MuonCalibTruth(**it) ) ;
      }
      m_truthVec = temp_truthVec ;

      MdtTruthVec temp_mdtTruthVec ;
      MdtTruthVecCit mdt_it = rhs.mdtTruthCollectionBegin();
      MdtTruthVecCit mdt_it_end = rhs.mdtTruthCollectionEnd();
      for( ; mdt_it!=mdt_it_end; ++mdt_it){
	temp_mdtTruthVec.push_back( new MuonCalibMdtTruthHit( **mdt_it ) );
      }
      m_mdtTruthVec = temp_mdtTruthVec ;
      
      RpcTruthVec temp_rpcTruthVec ;
      RpcTruthVecCit rpc_it = rhs.rpcTruthCollectionBegin();
      RpcTruthVecCit rpc_it_end = rhs.rpcTruthCollectionEnd();
      for( ; rpc_it!=rpc_it_end; ++rpc_it){
	temp_rpcTruthVec.push_back( new MuonCalibRpcTruthHit( **rpc_it ) );
      }
      m_rpcTruthVec = temp_rpcTruthVec ;
      
      TgcTruthVec temp_tgcTruthVec ;
      TgcTruthVecCit tgc_it = rhs.tgcTruthCollectionBegin();
      TgcTruthVecCit tgc_it_end = rhs.tgcTruthCollectionEnd();
      for( ; tgc_it!=tgc_it_end; ++tgc_it){
	temp_tgcTruthVec.push_back( new MuonCalibTgcTruthHit( **tgc_it ) );
      }
      m_tgcTruthVec = temp_tgcTruthVec ;
      
      CscTruthVec temp_cscTruthVec ;
      CscTruthVecCit csc_it = rhs.cscTruthCollectionBegin();
      CscTruthVecCit csc_it_end = rhs.cscTruthCollectionEnd();
      for( ; csc_it!=csc_it_end; ++csc_it){
	temp_cscTruthVec.push_back( new MuonCalibCscTruthHit( **csc_it ) );
      }
      m_cscTruthVec = temp_cscTruthVec ;
      
    }
    return (*this) ;
  }
  
}  //namespace MuonCalib
