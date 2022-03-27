/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRPCRAWDATAPROVIDERTOOLMT_H
#define MUONRPCRAWDATAPROVIDERTOOLMT_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"
#include "MuonRDO/RpcPad_Cache.h"

#include "RPC_RawDataProviderToolCore.h"


namespace Muon
{
    class IRpcROD_Decoder;

/**
 * Tool to do decoding of RPC RAW data. This tool should be safe to use with athenaMT.
 * It will not work with the legacy run-2 trigger code, since the tool creates a new
 * container each time.
 *
 * The main work of decoding is done in the RPC_RawDataProviderToolCore class.
 **/
class RPC_RawDataProviderToolMT
  : public extends<RPC_RawDataProviderToolCore, IMuonRawDataProviderTool>
{
    public:
    
    RPC_RawDataProviderToolMT(const std::string& t, 
                              const std::string& n, 
                              const IInterface* p);


    virtual ~RPC_RawDataProviderToolMT();
    
    virtual StatusCode initialize() override;

    /** Decoding method. - current methods: let's keep them! */
    virtual StatusCode convert(const ROBFragmentList& vecRobs) const override;
    virtual StatusCode convert(const ROBFragmentList& vecRobs, const std::vector<IdentifierHash>& coll) const override;
    /** the new ones */
    virtual StatusCode convert() const override; //!< for the entire event 
    virtual StatusCode convert(const std::vector<IdentifierHash>&) const override; //!< for a selection of rdo collections
    virtual StatusCode convert(const std::vector<uint32_t>&) const override;
    /** EventContext ones **/
    virtual StatusCode convert(const ROBFragmentList&, const EventContext&) const override;
    virtual StatusCode convert(const ROBFragmentList&, const std::vector<IdentifierHash>&, const EventContext&) const override;
    virtual StatusCode convert(const EventContext&) const override;
    virtual StatusCode convert(const std::vector<IdentifierHash>&, const EventContext&) const override;
    virtual StatusCode convert(const std::vector<uint32_t>&, const EventContext&) const override;
   

private:

    /// RPC container cache key
    SG::UpdateHandleKey<RpcPad_Cache> m_rdoContainerCacheKey ;
    /// Turn on/off RpcSectorConfig writing
    Gaudi::Property< bool > m_WriteOutRpcSectorLogic { this, "WriteOutRpcSectorLogic", true, "Turn on/off RpcSectorLogic writing" };
    
};

} // end of namespace

#endif
