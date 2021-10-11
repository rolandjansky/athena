/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAMRPCRDODECODER_H
#define MUONBYTESTREAMRPCRDODECODER_H

#include "MuonRPC_CnvTools/IRPC_RDO_Decoder.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "RPC_CondCabling/RpcCablingCondData.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

#include <inttypes.h>
#include <vector>
#include <string>

// Decoder class for conversion from RPC RDOs to RPC digits
namespace Muon {

class RpcRDO_Decoder : public extends<AthAlgTool, IRPC_RDO_Decoder> {

 public:

  RpcRDO_Decoder(const std::string& type, const std::string& name,
                              const IInterface* parent);
  ~RpcRDO_Decoder()=default;

  virtual StatusCode initialize() override;

  virtual std::vector<RpcDigit*>* getDigit(const RpcFiredChannel* fChan,
				   uint16_t& sectorID, uint16_t& padId,
				   uint16_t& cmaId, const RpcCablingCondData* rpcCab) const override;

  virtual std::vector<Identifier>* getOfflineData(const RpcFiredChannel* fChan,
					  uint16_t& sectorID, uint16_t& padId,
					  uint16_t& cmaId, double& time, const RpcCablingCondData* rpcCab) const override;

 private:
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
};


}

#endif // MUONBYTESTREAMRPCRDODECODER_H
