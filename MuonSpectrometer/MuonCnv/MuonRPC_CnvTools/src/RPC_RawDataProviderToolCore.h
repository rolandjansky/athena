/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRPCRAWDATAPROVIDERTOOLCORE_H
#define MUONRPCRAWDATAPROVIDERTOOLCORE_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonRDO/RpcPadContainer.h"
#include "MuonRDO/RpcSectorLogicContainer.h"
#include "RPC_CondCabling/RpcCablingCondData.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "RPCcablingInterface/CablingRPCBase.h"
#include "MuonRPC_CnvTools/IRpcROD_Decoder.h"
#include "ByteStreamData/RawEvent.h"

class IROBDataProviderSvc;

namespace Muon
{
    /**
     * Base class for tools to decode RPC raw data.
     * 
     * This contains all the logic to decode a vector of ROB fragments into the relevent containers.
     * The derived classes should do the management of the containers and then call the convertIntoContainers
     * from the convert functions required by the interface.
     */
    class RPC_RawDataProviderToolCore : public AthAlgTool {
      
    public:
      RPC_RawDataProviderToolCore(const std::string& t, 
                                  const std::string& n, 
                                  const IInterface* p);
      
      virtual ~RPC_RawDataProviderToolCore()=default;
      
      virtual StatusCode initialize();
     
    protected:

      // This function does all the actual work of decoding the data
      StatusCode convertIntoContainers(const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vecRobs,  const std::vector<IdentifierHash>& collections,
                                       RpcPadContainer* pad, RpcSectorLogicContainer* logic) const;
      
      std::vector<IdentifierHash> to_be_converted(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag,
                                                  const std::vector<IdentifierHash>& coll) const;

      SG::ReadCondHandleKey<RpcCablingCondData> m_readKey{this, "ReadKey", "RpcCablingCondData", "Key of RpcCablingCondData"};

      // Rob Data Provider handle 
      ServiceHandle<IROBDataProviderSvc> m_robDataProvider;

      // ROD decoding tool
      ToolHandle<IRpcROD_Decoder> m_decoder{this,"Decoder","Muon::RpcROD_Decoder/RpcROD_Decoder"};
      
      // WriteHandleKey for RPC PAD container
      SG::WriteHandleKey<RpcPadContainer>            m_containerKey {
	this, "RdoLocation", "RPCPAD", "Name of the RPCPAD produced by RawDataProvider"};
      // WriteHandleKey for RPC sector logic container
      SG::WriteHandleKey<RpcSectorLogicContainer>    m_sec{
	this, "RPCSec", "RPC_SECTORLOGIC", "Name of the RPC_SECTORLOGIC produced by RawDataProvider"};
      
    };//class RPC_RawDataProviderToolCore
}//namespace Muon

#endif //MUONRPCRAWDATAPROVIDERTOOLCORE_H
