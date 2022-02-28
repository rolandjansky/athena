/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCSDOVARIABLES_H
#define RPCSDOVARIABLES_H

#include "ValAlgVariables.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include <vector>

class RpcSDOVariables : public ValAlgVariables
{
 public:
  RpcSDOVariables(StoreGateSvc* evtStore, const MuonGM::MuonDetectorManager* detManager, const MuonIdHelper* idhelper,
                  TTree* tree, const std::string & containername, MSG::Level msglvl) :
    ValAlgVariables(evtStore, detManager, tree, containername, msglvl),
    m_rpcIdHelper(nullptr),
    m_rpc_nsdo(0), 
    m_rpc_sdo_stationName(),
    m_rpc_sdo_stationEta(),
    m_rpc_sdo_stationPhi(),
    m_rpc_sdo_doubletR(),
    m_rpc_sdo_doubletZ(),
    m_rpc_sdo_doubletPhi(),
    m_rpc_sdo_gas_gap(),
    m_rpc_sdo_measuresPhi(),
    m_rpc_sdo_strip(),
    m_rpc_sdo_channel(),
    m_rpc_sdo_word(),
    m_rpc_sdo_barcode(),
    m_rpc_sdo_globalPosX(),
    m_rpc_sdo_globalPosY(),
    m_rpc_sdo_globalPosZ(),
    m_rpc_sdo_globaltime(),
    m_rpc_sdo_localPosX(),
    m_rpc_sdo_localPosY()
  {
    setHelper(idhelper);
  }

  ~RpcSDOVariables()=default;

  StatusCode initializeVariables();
  StatusCode fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr);

 private:
  const RpcIdHelper* m_rpcIdHelper{};

  void setHelper(const MuonIdHelper* idhelper){
    m_rpcIdHelper = dynamic_cast<const RpcIdHelper*>(idhelper);
    if(!m_rpcIdHelper) {
       ATH_MSG_ERROR("casting IdHelper to RpcIdHelper failed");
       throw std::runtime_error("Casting error in RpcSDOVariables::setHelper");
    }
  }

  StatusCode clearVariables();
  void deleteVariables(){};

  int m_rpc_nsdo;
  std::vector<std::string> m_rpc_sdo_stationName;
  std::vector<int> m_rpc_sdo_stationEta;
  std::vector<int> m_rpc_sdo_stationPhi;
  std::vector<int> m_rpc_sdo_doubletR;
  std::vector<int> m_rpc_sdo_doubletZ;
  std::vector<int> m_rpc_sdo_doubletPhi;
  std::vector<int> m_rpc_sdo_gas_gap;
  std::vector<int> m_rpc_sdo_measuresPhi;
  std::vector<int> m_rpc_sdo_strip;
  std::vector<int> m_rpc_sdo_channel;

  std::vector<int>    m_rpc_sdo_word;
  std::vector<int>    m_rpc_sdo_barcode;
  std::vector<double> m_rpc_sdo_globalPosX;
  std::vector<double> m_rpc_sdo_globalPosY;
  std::vector<double> m_rpc_sdo_globalPosZ;
  std::vector<double> m_rpc_sdo_globaltime;

  std::vector<double> m_rpc_sdo_localPosX;
  std::vector<double> m_rpc_sdo_localPosY;

};

#endif // RPCSDOVARIABLES_H
