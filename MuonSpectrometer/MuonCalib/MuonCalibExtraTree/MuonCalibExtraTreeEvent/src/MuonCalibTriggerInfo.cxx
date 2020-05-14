/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraTreeEvent/MuonCalibTriggerInfo.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include <iostream>

namespace MuonCalib{
  
  std::vector< std::pair< std::vector<int>,std::string> > MuonCalibTriggerTypeHelper::m_typeToBitAndStringMapping;


  std::vector<int> MuonCalibTriggerTypeHelper::triggerBits( MuonCalibTriggerType type ) {
    initMap();
    if( !validType(type) ) type = NUMBEROFKNOWNTRIGGERS;
    return m_typeToBitAndStringMapping[type].first;
  }

  std::string MuonCalibTriggerTypeHelper::itemName( MuonCalibTriggerType type ) {
    initMap();
    if( !validType(type) ) type = NUMBEROFKNOWNTRIGGERS;
    return m_typeToBitAndStringMapping[type].second;
  }

  bool MuonCalibTriggerTypeHelper::validType( MuonCalibTriggerType type ) {
    return ( type >= L1_EM3 && type < NUMBEROFKNOWNTRIGGERS );
  }

  bool MuonCalibTriggerTypeHelper::isRpcTrigger( MuonCalibTriggerType type ) {
    if( type == L1_MU0_LOW_RPC || type == L1_MU6_RPC || type == L1_MU0_HIGH_RPC ) return true;
    return false;
  }
  
  bool MuonCalibTriggerTypeHelper::isTgcTrigger( MuonCalibTriggerType type ){
    if( type == L1_MU0_TGC_HALO || type == L1_MU0_TGC || type == L1_MU6_TGC ) return true;
    return false;
  }
  
  bool MuonCalibTriggerTypeHelper::isMbtsTrigger( MuonCalibTriggerType type ) {
    if( type == L1_MBTS_1_COMM || type == L1_MBTS_2_COMM ) return true;
    return false;
  }
  
  bool MuonCalibTriggerTypeHelper::isLVL1CaloTrigger( MuonCalibTriggerType type ) {
    if( type == L1_EM3 || type == L1_TAU5 || type == L1_J5 || type == L1_XE20 ) return true;
    return false;
  }
  
  void MuonCalibTriggerTypeHelper::addEntry( MuonCalibTriggerType type, int bit, std::string name ){
    if( !validType(type) ){
      MsgStream log(Athena::getMessageSvc(),"MuonCalibTriggerTypeHelper");
      log<<MSG::WARNING<<"MuonCalibTriggerTypeHelper::addEntry -> invalid trigger type"<<endmsg;
      return;
    }
    std::vector<int> bits;
    bits.push_back(bit);
    m_typeToBitAndStringMapping[type] = std::make_pair(bits,name);
  }

  void MuonCalibTriggerTypeHelper::addEntry( MuonCalibTriggerType type, std::vector<int>& bits, std::string name ){
    if( !validType(type) ){
      MsgStream log(Athena::getMessageSvc(),"MuonCalibTriggerTypeHelper");
      log<<MSG::WARNING<<"MuonCalibTriggerTypeHelper::addEntry -> invalid trigger type"<<endmsg;
      return;
    }
    m_typeToBitAndStringMapping[type] = std::make_pair(bits,name);
  }
        
  void MuonCalibTriggerTypeHelper::initMap() {
    if( !m_typeToBitAndStringMapping.empty() ) return;
    m_typeToBitAndStringMapping.resize( NUMBEROFKNOWNTRIGGERS + 1);

    addEntry(L1_EM3,0,"L1_EM3");
    addEntry(L1_TAU5,64,"L1_TAU5");
    addEntry(L1_J5,96,"L1_J5");
    addEntry(L1_XE20,129,"L1_XE20");
    
    std::vector<int> bits;
    bits.push_back(23);
    for(int i=30;i<=45;++i)     bits.push_back(i);
    addEntry(L1_MBTS_1_COMM,bits,"L1_MBTS_1_COMM");

    bits.clear();
    bits.push_back(24);
    for(int i=46;i<=61;++i)     bits.push_back(i);
    addEntry(L1_MBTS_2_COMM,bits,"L1_MBTS_2_COMM");
    addEntry(L1_MU0_LOW_RPC,89,"L1_MU0_LOW_RPC");
    addEntry(L1_MU6_RPC,90,"L1_MU6_RPC");
    addEntry(L1_MU0_HIGH_RPC,93,"L1_MU0_HIGH_RPC");
    addEntry(L1_MU0_TGC_HALO,88,"L1_MU0_TGC_HALO");
    addEntry(L1_MU0_TGC,91,"L1_MU0_TGC");
    addEntry(L1_MU6_TGC,92,"L1_MU6_TGC");
    addEntry(NUMBEROFKNOWNTRIGGERS,-1,"UNKNOWN");
  }

  MuonCalibTriggerInfo::MuonCalibTriggerInfo() :
    m_type(UNKNOWNTRIGGERTYPE), m_tbpbit(0), m_tapbit(0), m_tavbit(0), m_delay(0.), m_bcIndex(0) {
  }

  MuonCalibTriggerInfo::MuonCalibTriggerInfo( MuonCalibTriggerType type, double delay) : 
    m_type(type), m_tbpbit(0), m_tapbit(0), m_tavbit(0), m_delay(delay), m_bcIndex(0) {
  }

  MuonCalibTriggerInfo::MuonCalibTriggerInfo( int tbpbit, int tapbit, int tavbit, int bcIndex) : 
    m_type(UNKNOWNTRIGGERTYPE), m_tbpbit(tbpbit), m_tapbit(tapbit), m_tavbit(tavbit), m_delay(0.), m_bcIndex(bcIndex) {
  }

  void MuonCalibTriggerTypeHelper::dumpMapping() {
    initMap();
    MsgStream log(Athena::getMessageSvc(),"MuonCalibTriggerTypeHelper");
    log<<MSG::INFO<<"MuonCalibTriggerTypeHelper::dumpMapping, map size " << m_typeToBitAndStringMapping.size()<<endmsg;
    std::vector< std::pair< std::vector<int>,std::string> >::const_iterator it = m_typeToBitAndStringMapping.begin(); 
    std::vector< std::pair< std::vector<int>,std::string> >::const_iterator it_end = m_typeToBitAndStringMapping.end();
    int index = 0;
    for( ;it!=it_end; ++it ){
      log<<MSG::INFO<<"index " << index++ << "  type " << it->second << " nbits " << it->first.size() << " bits "<<endmsg;
      for( unsigned int i=0;i<it->first.size();++i ){
        log<<MSG::INFO<<"    " << it->first[i]<<endmsg;
      }
    }
  }

}//namespace MuonCalib
