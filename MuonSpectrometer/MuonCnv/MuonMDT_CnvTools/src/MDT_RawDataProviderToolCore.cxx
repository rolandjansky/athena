/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MDT_RawDataProviderToolCore.h"
#include "MuonRDO/MdtCsmContainer.h"

Muon::MDT_RawDataProviderToolCore::MDT_RawDataProviderToolCore(const std::string& t, const std::string& n, const IInterface*  p ) :
  AthAlgTool(t,n,p){ 
}

StatusCode Muon::MDT_RawDataProviderToolCore::initialize()
{    
  ATH_MSG_VERBOSE("Starting init");

  ATH_MSG_VERBOSE("Getting m_robDataProvider");  
  
  // Get ROBDataProviderSvc
  if (m_robDataProvider.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve serive " << m_robDataProvider);
    return StatusCode::FAILURE;
  } else
    ATH_MSG_INFO("Retrieved service " << m_robDataProvider);
  
  ATH_CHECK(m_idHelperSvc.retrieve());
  
  ATH_MSG_VERBOSE("Getting m_decoder");  
  
  // Retrieve decoder
  if (m_decoder.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve tool " << m_decoder);
    return StatusCode::FAILURE;
  } else 
    ATH_MSG_INFO("Retrieved tool " << m_decoder);
  
  
  m_maxhashtoUse = m_idHelperSvc->mdtIdHelper().stationNameIndex("BME") != -1 ?
    m_idHelperSvc->mdtIdHelper().detectorElement_hash_max() : m_idHelperSvc->mdtIdHelper().module_hash_max();

  ATH_CHECK( m_rdoContainerKey.initialize() );
  ATH_CHECK( m_readKey.initialize() );  
  
  ATH_MSG_INFO("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

StatusCode Muon::MDT_RawDataProviderToolCore::convertIntoContainer( const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vecRobs, MdtCsmContainer& mdtContainer)
{
  ATH_MSG_VERBOSE("convert(): " << vecRobs.size()<<" ROBFragments.");    

  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>::const_iterator itFrag;
  
  for (itFrag = vecRobs.begin(); itFrag != vecRobs.end(); itFrag++)
    {
      //convert only if data payload is delivered
      if ( (**itFrag).rod_ndata()!=0 )
	{
	  //std::vector<IdentifierHash> coll =
	  //                          to_be_converted(**itFrag,collections);
	  
	  if (m_decoder->fillCollections(**itFrag, mdtContainer).isFailure())
            {
	      // store the error conditions into the StatusCode and continue
            }
	}
      else
	{
	  if(msgLvl(MSG::DEBUG))
	    {
	      uint32_t sourceId= (**itFrag).source_id();
	      msg(MSG::DEBUG) << " ROB " << MSG::hex << sourceId
			    << " is delivered with an empty payload" << MSG::dec;
	    }
	  // store the error condition into the StatusCode and continue
	}
    }
  //in presence of errors return FAILURE
  ATH_MSG_DEBUG("After processing numColls="<< mdtContainer.numberOfCollections());
  //ATH_CHECK( handle.record (std::move (csm)) );
  return StatusCode::SUCCESS;
}

