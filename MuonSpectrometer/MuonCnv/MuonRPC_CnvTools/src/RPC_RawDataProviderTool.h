/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRPCRAWDATAPROVIDERTOOL_H
#define MUONRPCRAWDATAPROVIDERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"
#include "MuonRDO/RpcPadContainer.h"
#include "MuonRDO/RpcPad_Cache.h"
#include "MuonRDO/RpcSectorLogicContainer.h"

class StoreGateSvc;
class RpcPadIdHash;
class ActiveStoreSvc;
class IROBDataProviderSvc;
class IRPCcablingSvc;

namespace Muon
{
    class IRpcROD_Decoder;


class RPC_RawDataProviderTool : virtual public IMuonRawDataProviderTool, 
                                        public AthAlgTool
{
    public:
    
    RPC_RawDataProviderTool(const std::string& t, 
                            const std::string& n, 
                            const IInterface* p);


    virtual ~RPC_RawDataProviderTool();
    
    virtual StatusCode initialize();
    virtual StatusCode finalize();

    /** Decoding method. - current methods: let's keep them! */
    virtual StatusCode convert(const ROBFragmentList& vecRobs);
    virtual StatusCode convert(const ROBFragmentList& vecRobs,
                               const std::vector<IdentifierHash>& coll);    
    /** the new ones */
    virtual StatusCode convert(); //!< for the entire event 
    virtual StatusCode convert(const std::vector<IdentifierHash>&); //!< for a selection of rdo collections
    virtual StatusCode convert(const std::vector<uint32_t>&);

private:


    //const RpcPadIdHash*                 m_hashfunc;

    //    ServiceHandle<StoreGateSvc>         m_eventStore;
    ToolHandle<IRpcROD_Decoder>         m_decoder;
//    std::string                         m_rdoContainerKey;
    SG::WriteHandleKey<RpcPadContainer>            m_containerKey {
	this, "RdoLocation", "RPCPAD", "Name of the RPCPAD produced by RawDataProvider"};
    SG::WriteHandleKey<RpcSectorLogicContainer>    m_sec{
	this, "RPCSec", "RPC_SECTORLOGIC", "Name of the RPC_SECTORLOGIC produced by RawDataProvider"};
    bool m_AllowCreation;

    std::vector<IdentifierHash> to_be_converted(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag,
                            const std::vector<IdentifierHash>& coll) const;
    ActiveStoreSvc*                     m_activeStore;

    /// RPC cabling Svc
    const IRPCcablingSvc *m_rpcCabling;
    // Rob Data Provider handle 
    ServiceHandle<IROBDataProviderSvc>          m_robDataProvider;

    /// RPC container cache key
    SG::UpdateHandleKey<RpcPad_Cache> m_rdoContainerCacheKey ;
    /// Turn on/off RpcSectorConfig writing
    bool m_WriteOutRpcSectorLogic;

    
};

} // end of namespace

#endif
