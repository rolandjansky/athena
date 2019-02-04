/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CSC_RawDataProviderTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MUONCSC_CNVTOOLS_CSC_RAWDATAPROVIDERTOOL_H
#define MUONCSC_CNVTOOLS_CSC_RAWDATAPROVIDERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"
#include "MuonCSC_CnvTools/ICSC_ROD_Decoder.h"
#include "MuonRDO/CscRawDataContainer.h"
#include "MuonRDO/CscRawDataCollection_Cache.h"
#include "CSCcabling/CSCcablingSvc.h"
#include "CSC_Hid2RESrcID.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandleKey.h"

class IROBDataProviderSvc;

namespace MuonGM
{
    class MuonDetectorManager;
}

namespace Muon
{

class CSC_RawDataProviderTool : virtual public IMuonRawDataProviderTool, public AthAlgTool
{
public:
    CSC_RawDataProviderTool(const std::string& t, const std::string& n, const IInterface* p);

    /** default destructor */
    virtual ~CSC_RawDataProviderTool();

    /** standard Athena-Algorithm method */
    virtual StatusCode initialize() override;
    /** standard Athena-Algorithm method */
    virtual StatusCode finalize() override;

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

  /** member variables for algorithm properties: */
  ToolHandle<ICSC_ROD_Decoder>        m_decoder;

  const MuonGM::MuonDetectorManager*  m_muonMgr;

  SG::WriteHandleKey<CscRawDataContainer> m_containerKey{
     this, "RdoLocation", "CSCRDO", "Name of the CSCRDO produced by RawDataProvider"};
  SG::ReadHandleKey<xAOD::EventInfo>  m_eventInfoKey
  { this, "EventInfoKey", "EventInfo", "" };
  CSC_Hid2RESrcID                     m_hid2re;

  ServiceHandle<IROBDataProviderSvc>  m_robDataProvider;
  ServiceHandle<CSCcablingSvc>        m_cabling;

  bool				      m_createContainerEachEvent;

  /// CSC container cache key
  SG::UpdateHandleKey<CscRawDataCollection_Cache> m_rdoContainerCacheKey ;
};
} // end of namespace

#endif
