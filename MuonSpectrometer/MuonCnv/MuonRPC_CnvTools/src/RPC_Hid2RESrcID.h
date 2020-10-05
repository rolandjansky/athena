/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __RPC_HID2RESRCID__
#define __RPC_HID2RESRCID__

#include "AthenaKernel/MsgStreamMember.h"
#include "GaudiKernel/StatusCode.h"

#include "RPC_CondCabling/RpcCablingCondData.h"
#include "MuonIdHelpers/RpcIdHelper.h"

#include <stdint.h> 
#include <map>

#include "CxxUtils/checker_macros.h"

class RPC_Hid2RESrcID {

public:

  RPC_Hid2RESrcID();

  RPC_Hid2RESrcID (int specialROBNumber);

  void set(const RpcIdHelper* rpdId);

  uint32_t getRodID(const Identifier& offlineId, const RpcCablingCondData* readCdo);
  uint32_t getRodID(const int& side, const int& slogic, const int& padId, const RpcCablingCondData* readCdo);
  uint32_t getRodID(const int& sector);
  uint32_t getRodID(const uint16_t& side, const uint16_t& rodIndex);

  uint32_t getRobID(const uint32_t rod_id);

  uint32_t getRosID(const uint32_t rob_id);

  uint32_t getDetID(const uint32_t ros_id);
   
  MsgStream& msg(MSG::Level lvl) const { return m_msg << lvl;};

  bool msgLevel (MSG::Level lvl) { return m_msg.get().level() <= lvl;};

private:
  const RpcIdHelper* m_rpcIdHelper;

  int m_specialROBNumber;

  mutable Athena::MsgStreamMember m_msg ATLAS_THREAD_SAFE;
  
};

#endif 


