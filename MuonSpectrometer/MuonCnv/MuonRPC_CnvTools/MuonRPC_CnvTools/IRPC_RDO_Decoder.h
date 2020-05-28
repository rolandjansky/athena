/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRPC_CNVTOOL_IRPC_RDO_Decoder_H
#define MUONRPC_CNVTOOL_IRPC_RDO_Decoder_H

#include "GaudiKernel/IAlgTool.h"

class Identifier;
class RpcDigit;
class RpcFiredChannel;
class RpcCablingCondData;

static const InterfaceID IID_IRPC_RDO_Decoder("Muon::IRPC_RDO_Decoder", 1, 0);

namespace Muon {

/** This class provides conversion from RDO data to RPC RDO
   */

class IRPC_RDO_Decoder : virtual public IAlgTool {

public: 
  static const InterfaceID& interfaceID( ) { return IID_IRPC_RDO_Decoder; };
  

  virtual std::vector<RpcDigit*>* getDigit(const RpcFiredChannel* fChan,
				   uint16_t& sectorID, uint16_t& padId,
				   uint16_t& cmaId, const RpcCablingCondData* rpcCab) const = 0;


  virtual std::vector<Identifier>* getOfflineData(const RpcFiredChannel* fChan,
					  uint16_t& sectorID, uint16_t& padId,
					  uint16_t& cmaId, double& time, const RpcCablingCondData* rpcCab) const = 0;


}; 

}

#endif
