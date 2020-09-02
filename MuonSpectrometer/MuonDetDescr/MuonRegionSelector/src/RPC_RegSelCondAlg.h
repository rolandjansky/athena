/** emacs: this is -*- c++ -*- **/

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 **   @file    RPC_RegSelCondAlg.h        
 **                   
 **   @author  sutt
 **   @date    Tue  4 Feb 2020 15:25:00 CET
 **
 **/
 
#ifndef RPC_RegSelCondAlg_h
#define RPC_RegSelCondAlg_h

#include "MuonRegSelCondAlg.h"

#include "RPC_CondCabling/RpcCablingCondData.h"
#include "StoreGate/ReadCondHandleKey.h"

#include <string>

class RPC_RegSelCondAlg : public MuonRegSelCondAlg {

public:

  RPC_RegSelCondAlg( const std::string& name, ISvcLocator* pSvcLocator );

  std::unique_ptr<RegSelSiLUT> createTable( const MuonMDT_CablingMap* mdtCabling ) const override;

  virtual StatusCode initialize() override;
private:
  SG::ReadCondHandleKey<RpcCablingCondData> m_rpcReadKey{this, "RpcCablingKey", "RpcCablingCondData", "Key of RpcCablingCondData"};

};

#endif // RPC_RegSelCondAlg_h
