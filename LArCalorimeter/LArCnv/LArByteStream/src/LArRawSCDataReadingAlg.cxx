/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawSCDataReadingAlg.h"
#include "LArIdentifier/LArOnlineID.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h" 
#include "LArRawEvent/LArDigitContainer.h"
#include "LArRawEvent/LArRawSCContainer.h"
#include "LArRawEvent/LArLATOMEHeaderContainer.h"
#include "eformat/Version.h"
#include "eformat/index.h"


LArRawSCDataReadingAlg::LArRawSCDataReadingAlg(const std::string& name, ISvcLocator* pSvcLocator) :  
  AthReentrantAlgorithm(name, pSvcLocator),
  m_latomeDecoder ("LArByteStream/LATOMEDecoder", this) {

	declareProperty("LATOMEDecoder", m_latomeDecoder);
}


StatusCode LArRawSCDataReadingAlg::initialize() {

  m_doAdc = !m_adcCollKey.empty();
  ATH_CHECK(m_adcCollKey.initialize(m_doAdc));

  m_doAdcBas = !m_adcBasCollKey.empty();
  ATH_CHECK(m_adcBasCollKey.initialize(m_doAdcBas));

  m_doEt = !m_etCollKey.empty();
  ATH_CHECK(m_etCollKey.initialize(m_doEt));

  m_doEtId = !m_etIdCollKey.empty();
  ATH_CHECK(m_etIdCollKey.initialize(m_doEtId));

  m_doLATOMEHeader = !m_latomeHeaderCollKey.empty();
  ATH_CHECK(m_latomeHeaderCollKey.initialize(m_doLATOMEHeader));  

  ATH_CHECK(m_mapKey.initialize() );

  ATH_CHECK(m_robDataProviderSvc.retrieve());
  ATH_CHECK(detStore()->retrieve(m_onlineId,"LArOnlineID"));  

  ATH_CHECK(m_latomeDecoder.retrieve());

  return StatusCode::SUCCESS;
}     

  
StatusCode LArRawSCDataReadingAlg::execute(const EventContext& ctx) const {

  LArDigitContainer* adc_coll = nullptr;
  LArDigitContainer* adc_bas_coll = nullptr;
  LArRawSCContainer* et_coll = nullptr;
  LArRawSCContainer* et_id_coll = nullptr;
  LArLATOMEHeaderContainer* latome_header_coll = nullptr;

  if (m_doAdc) {
	SG::WriteHandle<LArDigitContainer> adcHdl(m_adcCollKey, ctx);
	ATH_CHECK(adcHdl.record(std::make_unique<LArDigitContainer>()));
	adc_coll = adcHdl.ptr();
	adc_coll->reserve(1000);
  }

  if (m_doAdcBas) {
	SG::WriteHandle<LArDigitContainer> adcBasHdl(m_adcBasCollKey, ctx);
	ATH_CHECK(adcBasHdl.record(std::make_unique<LArDigitContainer>()));
	adc_bas_coll = adcBasHdl.ptr();
	adc_bas_coll->reserve(1000);
  }

  if (m_doEt) {
	SG::WriteHandle<LArRawSCContainer> etHdl(m_etCollKey, ctx);
	ATH_CHECK(etHdl.record(std::make_unique<LArRawSCContainer>()));
	et_coll = etHdl.ptr();
	et_coll->reserve(1000); 
  }

  if (m_doEtId) {
	SG::WriteHandle<LArRawSCContainer> etIdHdl(m_etIdCollKey, ctx);
	ATH_CHECK(etIdHdl.record(std::make_unique<LArRawSCContainer>()));
	et_id_coll = etIdHdl.ptr();
	et_id_coll->reserve(1000); 
  }

  if (m_doLATOMEHeader) {
	SG::WriteHandle<LArLATOMEHeaderContainer> latomeHeaderHdl(m_latomeHeaderCollKey, ctx);
	ATH_CHECK(latomeHeaderHdl.record(std::make_unique<LArLATOMEHeaderContainer>()));
	latome_header_coll = latomeHeaderHdl.ptr();
	latome_header_coll->reserve(1000);
  }

  // Get the mapping
  SG::ReadCondHandle<LArLATOMEMapping> mapHdl(m_mapKey, ctx);
  const LArLATOMEMapping *map=*mapHdl;
  if(!map) {
     ATH_MSG_ERROR("Do not have LATOME mapping with the key " << m_mapKey.key());
     return StatusCode::FAILURE;
  }

  //Get the raw event
  const RawEvent* rawEvent = m_robDataProviderSvc->getEvent(ctx);

  // Call the converter
  StatusCode sc = m_latomeDecoder->convert(rawEvent, map,
                                           adc_coll, adc_bas_coll, et_coll, et_id_coll, 
                                           latome_header_coll);
  if (sc != StatusCode::SUCCESS) 
     ATH_MSG_WARNING("ERROR LATOMEDecoder tool returned an error. LAr SC containers might be garbage");

  return StatusCode::SUCCESS;
}
