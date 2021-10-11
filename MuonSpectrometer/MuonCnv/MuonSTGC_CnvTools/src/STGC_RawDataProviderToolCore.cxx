/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <atomic>
#include "STGC_RawDataProviderToolCore.h"
using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;

//================ Constructor =================================================

Muon::STGC_RawDataProviderToolCore::STGC_RawDataProviderToolCore(const std::string& t, const std::string& n, const IInterface* p) 
: AthAlgTool(t, n, p)
, m_robDataProvider("ROBDataProviderSvc",n) 
{ }

//================ Initialisation ==============================================
StatusCode Muon::STGC_RawDataProviderToolCore::initialize()
{
  ATH_CHECK(m_idHelperSvc.retrieve());
  ATH_CHECK(m_decoder.retrieve());
  ATH_CHECK(m_robDataProvider.retrieve()); // ROBDataProviderSvc
  ATH_CHECK(m_rdoContainerKey.initialize());

  m_maxhashtoUse = m_idHelperSvc->stgcIdHelper().module_hash_max();  
  return StatusCode::SUCCESS;
}

//==============================================================================
StatusCode Muon::STGC_RawDataProviderToolCore::convertIntoContainer(const std::vector<const ROBFragment*>& vecRobs, const std::vector<IdentifierHash>& rdoIdhVect, STGC_RawDataContainer& stgcRdoContainer) const
{
  // Loop on the passed ROB fragments, and call the decoder for each one to fill the RDO container.

  static std::atomic_int DecodeErrCount{0};

  for (const ROBFragment* fragment : vecRobs) {
    if (m_decoder->fillCollection(*fragment, rdoIdhVect, stgcRdoContainer).isFailure()) {
      if (DecodeErrCount < 100) {
        ATH_MSG_INFO( "Problem with STGC ByteStream Decoding!" );
        ++DecodeErrCount;
      } else if (DecodeErrCount == 100) {
        ATH_MSG_INFO( "Too many Problems with STGC Bytestream Decoding messages. Turning messaging off." );
        ++DecodeErrCount;
      }
    }
  }

  ATH_MSG_DEBUG("Size of sTgcRdoContainer is " << stgcRdoContainer.size());
  return StatusCode::SUCCESS;
}

