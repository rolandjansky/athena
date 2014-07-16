/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAMRPCRDODECODER_H
#define MUONBYTESTREAMRPCRDODECODER_H

#include "MuonRPC_CnvTools/IRPC_RDO_Decoder.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "AthenaKernel/MsgStreamMember.h"

#include <inttypes.h>
#include <vector>

// Decoder class for conversion from RPC RDOs to RPC digits
// Stefano Rosati
// CERN Jan 2004

namespace Muon
{
    

class RpcRDO_Decoder : virtual public IRPC_RDO_Decoder, public AthAlgTool
{

 public:

  RpcRDO_Decoder(const std::string& type, const std::string& name,
                              const IInterface* parent);
  ~RpcRDO_Decoder() { }

  virtual StatusCode initialize();
  virtual StatusCode finalize() { return StatusCode::SUCCESS; }

  virtual std::vector<RpcDigit*>* getDigit(const RpcFiredChannel * fChan, 
				   uint16_t& sectorID, uint16_t& padId, 
				   uint16_t& cmaId) const;

  virtual std::vector<Identifier>* getOfflineData(const RpcFiredChannel * fChan, 
					  uint16_t& sectorID, uint16_t& padId, 
					  uint16_t& cmaId, double& time) const;

	
 private:

  const MuonGM::MuonDetectorManager* m_muonMgr;

  const RpcIdHelper*    m_rpcIdHelper;
  const IRPCcablingSvc* m_cablingSvc;

};


}



#endif


