/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRPCRAWDATAPROVIDERTOOL_H
#define MUONRPCRAWDATAPROVIDERTOOL_H

#include "CxxUtils/checker_macros.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"
#include "MuonRDO/RpcPad_Cache.h"

#include "RPC_RawDataProviderToolCore.h"

class RpcPadIdHash;

namespace Muon
{
    class IRpcROD_Decoder;

/// This class is only used in a single-thread mode
class ATLAS_NOT_THREAD_SAFE RPC_RawDataProviderTool
  : public extends<RPC_RawDataProviderToolCore, IMuonRawDataProviderTool>
{
    public:
    
    RPC_RawDataProviderTool(const std::string& t, 
                            const std::string& n, 
                            const IInterface* p);


    virtual ~RPC_RawDataProviderTool();
    
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



    bool m_AllowCreation;    
};

} // end of namespace

#endif
