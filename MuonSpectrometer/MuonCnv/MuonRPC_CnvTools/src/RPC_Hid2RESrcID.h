/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __RPC_HID2RESRCID__
#define __RPC_HID2RESRCID__


#include "AthenaKernel/MsgStreamMember.h"
// #include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "RPCcablingInterface/IRPCcablingSvc.h"
#include "MuonIdHelpers/RpcIdHelper.h"

#include <stdint.h> 
#include <map>


class RPC_Hid2RESrcID {

public:

  RPC_Hid2RESrcID (); 

  RPC_Hid2RESrcID (int specialROBNumber); 

  void set(const IRPCcablingSvc* p_cabling, const RpcIdHelper* rpcHelper); 

  std::vector<uint32_t> getRodIDFromCollID(const Identifier& offlineId);

  uint32_t getRodID(const Identifier& offlineId);

  uint32_t getRodID  (const int& side, const int& slogic, const int& padId); 
  uint32_t getRodID  (const int& sector); 
  uint32_t getRodID  (const uint16_t& side, const uint16_t& rodIndex);

  uint32_t getRobID  ( uint32_t rod_id); 

  uint32_t getRosID  ( uint32_t rob_id); 

  uint32_t getDetID  ( uint32_t ros_id); 
  
  uint16_t findRODid(uint16_t side, uint16_t slogic, uint16_t padId);

  Identifier findPadOfflineId(uint16_t side, uint16_t slogic, uint16_t padId);

  const RpcIdHelper * getIdHelper () { return m_rpcHelper; }  
   
  MsgStream & msg(MSG::Level lvl) const { return m_msg << lvl;};

  bool msgLevel (MSG::Level lvl) { return m_msg.get().level() <= lvl;};

private: 

  const IRPCcablingSvc* m_cabling;
  const RpcIdHelper*    m_rpcHelper;

  int m_specialROBNumber;

  mutable Athena::MsgStreamMember m_msg;
  
};

#endif 


