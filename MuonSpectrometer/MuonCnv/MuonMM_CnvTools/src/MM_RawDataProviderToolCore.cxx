/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <atomic>
#include "MM_RawDataProviderToolCore.h"
using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;

//================ Constructor =================================================

Muon::MM_RawDataProviderToolCore::MM_RawDataProviderToolCore(const std::string& t, const std::string& n, const IInterface* p)
: AthAlgTool(t, n, p)
, m_robDataProvider("ROBDataProviderSvc",n) 
{ }

//================ Initialisation ==============================================
StatusCode Muon::MM_RawDataProviderToolCore::initialize()
{
  ATH_CHECK(AthAlgTool::initialize());
  ATH_CHECK(m_idHelperSvc.retrieve());
  ATH_CHECK(m_decoder.retrieve());
  ATH_CHECK(m_robDataProvider.retrieve());
  ATH_CHECK(m_rdoContainerKey.initialize());

  m_maxhashtoUse = m_idHelperSvc->mmIdHelper().module_hash_max();  

  return StatusCode::SUCCESS;
}

//==============================================================================
StatusCode Muon::MM_RawDataProviderToolCore::convertIntoContainer(const std::vector<const ROBFragment*>& vecRobs, const std::vector<IdentifierHash>& rdoIdhVect, MM_RawDataContainer& mmRdoContainer) const
{
  // Loop on the passed ROB fragments, and call the decoder for each one to fill the RDO container.

  static std::atomic_int DecodeErrCount{0};

  for (const ROBFragment* fragment : vecRobs) {
    if (m_decoder->fillCollection(*fragment, rdoIdhVect, mmRdoContainer).isFailure()) {
      if (DecodeErrCount < 100) {
        ATH_MSG_INFO( "Problem with MM ByteStream Decoding!" );
        ++DecodeErrCount;
      } else if (DecodeErrCount == 100) {
        ATH_MSG_INFO( "Too many Problems with MM Bytestream Decoding messages. Turning messaging off." );
        ++DecodeErrCount;
      }
    }
  }
	
  ATH_MSG_DEBUG("Size of mmRdoContainer is " << mmRdoContainer.size());
  return StatusCode::SUCCESS;
}

