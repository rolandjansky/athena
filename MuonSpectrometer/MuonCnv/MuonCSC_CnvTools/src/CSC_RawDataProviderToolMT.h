/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CSC_RawDataProviderToolMT.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MUONCSC_CNVTOOLS_CSC_RAWDATAPROVIDERTOOLMT_H
#define MUONCSC_CNVTOOLS_CSC_RAWDATAPROVIDERTOOLMT_H

#include "CSC_RawDataProviderToolCore.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuonRDO/CscRawDataCollection_Cache.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

namespace Muon
{

class CSC_RawDataProviderToolMT : virtual public IMuonRawDataProviderTool, public CSC_RawDataProviderToolCore
{
public:
    CSC_RawDataProviderToolMT(const std::string& t, const std::string& n, const IInterface* p);

    /** default destructor */
    virtual ~CSC_RawDataProviderToolMT();

    /** standard Athena-Algorithm method */
    virtual StatusCode initialize() override;

    virtual StatusCode convert(const ROBFragmentList& vecRobs,
                               const std::vector<IdentifierHash>& /*collections*/) override;
      
    virtual StatusCode convert(const ROBFragmentList& vecRobs) override
    {
      const CSC_RawDataProviderToolMT* cthis = this;
      return cthis->convert (vecRobs, Gaudi::Hive::currentContext());
    }

    virtual StatusCode convert(const std::vector<IdentifierHash>& collections) override;
    virtual StatusCode convert() override
    {
      const CSC_RawDataProviderToolMT* cthis = this;
      return cthis->convert (Gaudi::Hive::currentContext());
    }
    virtual StatusCode convert(const std::vector<uint32_t>&) override {return StatusCode::FAILURE;}

    StatusCode convert(const ROBFragmentList& vecRobs,
                       const EventContext& ctx) const;
    StatusCode convert(const EventContext& ctx) const;

private:

  /// CSC container cache key
  SG::UpdateHandleKey<CscRawDataCollection_Cache> m_rdoContainerCacheKey ;
};
} // end of namespace

#endif
