/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CSC_RawDataProviderToolMT.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MUONCSC_CNVTOOLS_CSC_RAWDATAPROVIDERTOOLMT_H
#define MUONCSC_CNVTOOLS_CSC_RAWDATAPROVIDERTOOLMT_H

#include "CSC_RawDataProviderToolCore.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonRDO/CscRawDataCollection_Cache.h"

namespace Muon {

    class CSC_RawDataProviderToolMT : public extends<CSC_RawDataProviderToolCore, IMuonRawDataProviderTool> {
    public:
        CSC_RawDataProviderToolMT(const std::string& t, const std::string& n, const IInterface* p);

        /** default destructor */
        virtual ~CSC_RawDataProviderToolMT();

        /** standard Athena-Algorithm method */
        virtual StatusCode initialize() override;

        virtual StatusCode convert(const ROBFragmentList& vecRobs, const std::vector<IdentifierHash>& /*collections*/) const override;

        virtual StatusCode convert(const ROBFragmentList& vecRobs) const override {
            return this->convert(vecRobs, Gaudi::Hive::currentContext());
        }

        virtual StatusCode convert(const std::vector<IdentifierHash>& collections) const override;
        virtual StatusCode convert() const override { return this->convert(Gaudi::Hive::currentContext()); }
        virtual StatusCode convert(const std::vector<uint32_t>&) const override { return StatusCode::FAILURE; }

        virtual StatusCode convert(const ROBFragmentList& vecRobs, const EventContext& ctx) const override;
        virtual StatusCode convert(const EventContext& ctx) const override;
        virtual StatusCode convert(const std::vector<IdentifierHash>&, const EventContext&) const override;

        // Not used
        virtual StatusCode convert(const std::vector<uint32_t>&, const EventContext&) const override { return StatusCode::FAILURE; }
        virtual StatusCode convert(const ROBFragmentList&, const std::vector<IdentifierHash>&, const EventContext&) const override {
            return StatusCode::FAILURE;
        }

    private:
        /// CSC container cache key
        SG::UpdateHandleKey<CscRawDataCollection_Cache> m_rdoContainerCacheKey;
    };
}  // namespace Muon

#endif
