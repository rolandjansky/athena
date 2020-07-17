/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CSC_RawDataProviderTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MUONCSC_CNVTOOLS_CSC_RAWDATAPROVIDERTOOL_H
#define MUONCSC_CNVTOOLS_CSC_RAWDATAPROVIDERTOOL_H
    
#include "CxxUtils/checker_macros.h"

#include "CSC_RawDataProviderToolCore.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

namespace Muon
{

/// This class is only used in a single-thread mode
class ATLAS_NOT_THREAD_SAFE CSC_RawDataProviderTool : virtual public IMuonRawDataProviderTool, public CSC_RawDataProviderToolCore
{
public:
    CSC_RawDataProviderTool(const std::string& t, const std::string& n, const IInterface* p);

    /** default destructor */
    virtual ~CSC_RawDataProviderTool();

    /** standard Athena-Algorithm method */
    virtual StatusCode initialize() override;

    virtual StatusCode convert(const ROBFragmentList& vecRobs,
                               const std::vector<IdentifierHash>& /*collections*/) override;
      
    virtual StatusCode convert(const ROBFragmentList& vecRobs) override
    {
      const CSC_RawDataProviderTool* cthis = this;
      return cthis->convert (vecRobs, Gaudi::Hive::currentContext());
    }

    virtual StatusCode convert(const std::vector<IdentifierHash>& collections) override;
    virtual StatusCode convert() override
    {
      const CSC_RawDataProviderTool* cthis = this;
      return cthis->convert (Gaudi::Hive::currentContext());
    }
    virtual StatusCode convert(const std::vector<uint32_t>&) override {return StatusCode::FAILURE;}

    StatusCode convert(const ROBFragmentList& vecRobs,
                       const EventContext& ctx) const;
    StatusCode convert(const EventContext& ctx) const;

private:
    
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

};
} // end of namespace

#endif
