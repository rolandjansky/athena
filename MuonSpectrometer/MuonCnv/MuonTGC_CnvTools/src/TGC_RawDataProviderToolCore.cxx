/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TGC_RawDataProviderToolCore.h"

#include "MuonRDO/TgcRdoContainer.h"
#include "TGCcablingInterface/ITGCcablingServerSvc.h"

//================ Constructor =================================================

Muon::TGC_RawDataProviderToolCore::TGC_RawDataProviderToolCore(
						       const std::string& t,
						       const std::string& n,
						       const IInterface*  p) :
  AthAlgTool(t, n, p),
  m_cabling(nullptr),
  m_robDataProvider("ROBDataProviderSvc",n) 
{
}

//================ Initialisation =================================================

StatusCode Muon::TGC_RawDataProviderToolCore::initialize()
{
  ATH_CHECK(AthAlgTool::initialize());
  ATH_CHECK(m_idHelperSvc.retrieve());

  ATH_CHECK(m_decoder.retrieve());
  ATH_MSG_DEBUG( "Retrieved tool " << m_decoder );

  // Get ROBDataProviderSvc
  ATH_CHECK(m_robDataProvider.retrieve());
  ATH_MSG_DEBUG( "Retrieved service " << m_robDataProvider );

  m_maxhashtoUse = m_idHelperSvc->tgcIdHelper().module_hash_max();  

  ATH_CHECK(m_rdoContainerKey.initialize());

  //try to configure the cabling service
  StatusCode sc = getCabling();
  if(sc.isFailure()) {
      ATH_MSG_INFO( "TGCcablingServerSvc not yet configured; postone TGCcabling initialization at first event. " );
  }
  
  return StatusCode::SUCCESS;
}

//============================================================================================

StatusCode Muon::TGC_RawDataProviderToolCore::convertIntoContainer(const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vecRobs, TgcRdoContainer& tgcRdoContainer) 
{    

  /// Static variables are not thread safe
  static thread_local int DecodeErrCount = 0;

  // Update to range based loop
  for(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment* fragment : vecRobs){
    if(m_decoder->fillCollection(*fragment, tgcRdoContainer).isFailure()) {
      if(DecodeErrCount < 100) {
        ATH_MSG_INFO( "Problem with TGC ByteStream Decoding!" );
        DecodeErrCount++;
      } 
      else if(100 == DecodeErrCount) {
        ATH_MSG_INFO( "Too many Problems with TGC Decoding messages. Turning message off." );
        DecodeErrCount++;
      }
    }
  }
  ATH_MSG_DEBUG("Size of TgcRdoContainer is " << tgcRdoContainer.size());
  return StatusCode::SUCCESS;
}

StatusCode  Muon::TGC_RawDataProviderToolCore::getCabling() {
  const ITGCcablingServerSvc* TgcCabGet = 0;
  StatusCode sc = service("TGCcablingServerSvc", TgcCabGet, true);
  if(sc.isFailure()) {
    ATH_MSG_FATAL( "Could not get TGCcablingServerSvc !" );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_VERBOSE( " TGCcablingServerSvc retrieved" );
  } 

  sc = TgcCabGet->giveCabling(m_cabling);
  if(sc.isFailure()) {
    ATH_MSG_FATAL( "Could not get ITGCcablingSvc from the Server !" );
    m_cabling = 0;
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_VERBOSE( "ITGCcablingSvc obtained" );
  }    
  
  m_hid2re.set(m_cabling); 
  
  return StatusCode::SUCCESS;
}


std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> Muon::TGC_RawDataProviderToolCore::getROBData(const std::vector<IdentifierHash>& rdoIdhVect) {

  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> vecOfRobf;
  if(!m_cabling) {
    if( getCabling().isFailure() ) {
      ATH_MSG_ERROR("Could not get cabling, return empty vector of ROB fragments");
      return vecOfRobf;
    }
  }

  IdContext tgcContext = m_idHelperSvc->tgcIdHelper().module_context();
  
  std::vector<uint32_t> robIds;

  unsigned int size = rdoIdhVect.size();
  for(unsigned int i=0; i<size; ++i) {
    Identifier Id;
    if(m_idHelperSvc->tgcIdHelper().get_id(rdoIdhVect[i], Id, &tgcContext)) {
      ATH_MSG_WARNING( "Unable to get TGC Identifier from collection hash id " );
      continue;
    }
    const Identifier tgcId = Id;
    uint32_t rodId = m_hid2re.getRodID(tgcId);
    uint32_t robId = m_hid2re.getRobID(rodId);
    std::vector<uint32_t>::iterator it_robId = std::find(robIds.begin(), robIds.end(), robId); 
    if(it_robId==robIds.end()) {
      robIds.push_back(robId);
    }
  }
  m_robDataProvider->getROBData(robIds, vecOfRobf);
  ATH_MSG_VERBOSE( "Number of ROB fragments " << vecOfRobf.size() );
  return vecOfRobf;
}
