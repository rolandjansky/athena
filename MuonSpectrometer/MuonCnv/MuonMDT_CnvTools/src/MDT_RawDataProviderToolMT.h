/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONMDTCNVTOOLS_MUONMDTRAWDATAPROVIDERTOOLMT_H
#define MUONMDTCNVTOOLS_MUONMDTRAWDATAPROVIDERTOOLMT_H

#include <string>
#include <vector>

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MDT_RawDataProviderToolCore.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"
#include "MuonRDO/MdtCsm_Cache.h"

namespace Muon {

    /** @class MDT_RawDataProviderToolMT
        @author  Mark Owen <markowen@cern.ch>
    */

    class MDT_RawDataProviderToolMT : public extends<MDT_RawDataProviderToolCore, IMuonRawDataProviderTool> {
    public:
        MDT_RawDataProviderToolMT(const std::string&, const std::string&, const IInterface*);

        /** default destructor */
        virtual ~MDT_RawDataProviderToolMT() {}

        /** standard Athena-Algorithm method */
        virtual StatusCode initialize() override;

        /** standard Athena-Algorithm method */
        virtual StatusCode finalize() override;

        /** Convert method - declared in Muon::IMuonRdoToPrepDataTool*/
        virtual StatusCode convert(const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vecRobs) const override;
        virtual StatusCode convert(const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vecRobs,
                                   const std::vector<IdentifierHash>&) const override;
        /** the new ones */
        virtual StatusCode convert() const override;  //!< for the entire event
        virtual StatusCode convert(const std::vector<IdentifierHash>& HashVec) const override;
        virtual StatusCode convert(const std::vector<uint32_t>& robIds) const override;  //!< for a particular vector of ROBId's
        /** EventContext **/
        virtual StatusCode convert(const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vecRobs,
                                   const EventContext& ctx) const override;
        virtual StatusCode convert(const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vecRobs,
                                   const std::vector<IdentifierHash>&, const EventContext& ctx) const override;
        virtual StatusCode convert(const EventContext& ctx) const override;  //!< for the entire event
        virtual StatusCode convert(const std::vector<IdentifierHash>& HashVec, const EventContext& ctx) const override;
        virtual StatusCode convert(const std::vector<uint32_t>& robIds,
                                   const EventContext& ctx) const override;  //!< for a particular vector of ROBId's

    private:
        /// This is the key for the cache for the CSM containers, can be empty
        SG::UpdateHandleKey<MdtCsm_Cache> m_rdoContainerCacheKey;
    };
}  // namespace Muon

#endif
