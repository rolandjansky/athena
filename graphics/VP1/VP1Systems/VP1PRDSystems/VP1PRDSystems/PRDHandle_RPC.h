/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PRDHANDLE_RPC_H
#define PRDHANDLE_RPC_H

#include "VP1PRDSystems/PRDHandleBase.h"
#include "VP1PRDSystems/PRDCollHandle_RPC.h"
#include "MuonPrepRawData/RpcPrepData.h"

class PRDHandle_RPC : public PRDHandleBase {
public:

  PRDHandle_RPC(PRDCollHandle_RPC*,const Muon::RpcPrepData*);
  virtual ~PRDHandle_RPC() {};

  void buildShapes(SoNode*&shape_simple, SoNode*&shape_detailed);
  int regionIndex();
  QStringList clicked() const;

  const Muon::RpcPrepData * rpc() const { return m_rpc; }
  const Trk::PrepRawData * getPRD() const { return m_rpc; }

  bool inMuonChamber() const { return true; }
  GeoPVConstLink parentMuonChamberPV() const { return m_rpc->detectorElement()->parentStationPV(); }

private:
  const Muon::RpcPrepData* m_rpc;

};

#endif
