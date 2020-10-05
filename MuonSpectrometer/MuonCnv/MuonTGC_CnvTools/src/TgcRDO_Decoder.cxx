/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TgcRDO_Decoder.h"

#include "MuonDigitContainer/TgcDigit.h"
#include "MuonRDO/TgcRawData.h"
#include "Identifier/Identifier.h"
#include "TGCcablingInterface/ITGCcablingSvc.h"
#include "TGCcablingInterface/ITGCcablingServerSvc.h"

// constructor 
Muon::TgcRDO_Decoder::TgcRDO_Decoder
(const std::string& type, const std::string& name, const IInterface* parent)
  : AthAlgTool(type, name, parent), 
    m_cabling(0), 
    m_applyPatch(false) 
{
  declareInterface<ITGC_RDO_Decoder>(this);
}

StatusCode Muon::TgcRDO_Decoder::initialize() 
{
  ATH_MSG_DEBUG("TgcRDO_Decoder::initialize"); 

  //try to configure the cabling service
  StatusCode sc = getCabling();
  if(sc.isFailure()) {
      ATH_MSG_INFO( "TGCcablingServer not yet configured; postone TGCcabling initialization at first event. " );
  }

  return StatusCode::SUCCESS;
}

StatusCode Muon::TgcRDO_Decoder::finalize() 
{ 
  return StatusCode::SUCCESS; 
}

void Muon::TgcRDO_Decoder::applyPatch(bool patch) 
{ 
  m_applyPatch = patch; 
}

TgcDigit* Muon::TgcRDO_Decoder::getDigit(const TgcRawData * rawData, bool orFlag) const 
{
  // ITGCcablingSvc should be configured at initialise
  if(!m_cabling){
    ATH_MSG_ERROR("ITGCcablingSvc is not available in TgcRDO_Decoder::getDigit()");
    return 0;
  }

  int offset=0, offsetORed=0;

  const uint16_t sswId = rawData->sswId();
  const uint16_t slbId = rawData->slbId();
  const uint16_t bitpos = rawData->bitpos();
  
  if(m_applyPatch) {
    if(sswId==9) {
      if((slbId%4==3 && bitpos>=77 && bitpos<=84) ||
	 (slbId%4!=3 && bitpos>=77 && bitpos<=111)) {
	offset=1;
      }
    } else if(sswId>=3 && sswId<=8) {
      if((slbId==1 && bitpos>=66 && bitpos<=73) ||
	 (slbId==2 && bitpos>=42 && bitpos<=63)) {
	offset=36;
      }
      if((slbId==1 && bitpos>=102 && bitpos<=109) ||
	 (slbId==2 && bitpos>= 78 && bitpos<= 99)) {
	offset=-36;
      }
      if((slbId==1 && bitpos>=74 && bitpos<=75) ||
	 (slbId==2 && bitpos>=40 && bitpos<=41)) {
	offsetORed=36;
      }
      if((slbId==1 && bitpos>=110 && bitpos<=111) ||
	 (slbId==2 && bitpos>= 76 && bitpos<= 77)) {
	offsetORed=-36;
      }
    }
  }
  int corr = orFlag ? offsetORed : offset;

  // get official channel ID
  Identifier chanId;
  bool c_found = m_cabling->getOfflineIDfromReadoutID
    (chanId, rawData->subDetectorId(), rawData->rodId(),
     sswId, slbId, bitpos+corr, orFlag);
  
  if(!c_found) return 0;
  
  return new TgcDigit(chanId,rawData->bcTag());
}

Identifier Muon::TgcRDO_Decoder::getOfflineData(const TgcRawData * rawData, bool orFlag, uint16_t& bctag) const
{
  Identifier chanId(0);

  // ITGCcablingSvc should be configured at initialise                              
  if(!m_cabling){
    ATH_MSG_ERROR("ITGCcablingSvc is not available in TgcRDO_Decoder::getOfflineData()");
    return chanId;
  }


  bctag=TgcDigit::BC_UNDEFINED;

  const uint16_t sswId = rawData->sswId();
  const uint16_t slbId = rawData->slbId();
  const uint16_t bitpos = rawData->bitpos();
  
  int offset=0, offsetORed=0;
  if(m_applyPatch) {
    if(sswId==9) {
      if((slbId%4==3 && bitpos>=77 && bitpos<= 84) ||
	 (slbId%4!=3 && bitpos>=77 && bitpos<=111)) {
	offset=1;
      }
    } else if(sswId>=3 && sswId<=8) {
      if((slbId==1 && bitpos>=66 && bitpos<=73) ||
	 (slbId==2 && bitpos>=42 && bitpos<=63)) {
	offset=36;
      }
      if((slbId==1 && bitpos>=102 && bitpos<=109) ||
	 (slbId==2 && bitpos>= 78 && bitpos<= 99)) {
	offset=-36;
      }
      if((slbId==1 && bitpos>=74 && bitpos<=75) ||
	 (slbId==2 && bitpos>=40 && bitpos<=41)) {
	offsetORed=36;
      }
      if((slbId==1 && bitpos>=110 && bitpos<=111) ||
	 (slbId==2 && bitpos>= 76 && bitpos<= 77)) {
	offsetORed=-36;
      }
    }
  }
  int corr = orFlag ? offsetORed : offset;

  // get official channel ID
  bool c_found = m_cabling->getOfflineIDfromReadoutID
    (chanId, rawData->subDetectorId(), rawData->rodId(),
     sswId, slbId, bitpos+corr, orFlag);

  if(!c_found) return chanId;

  bctag = rawData->bcTag();
  return chanId;
}

StatusCode Muon::TgcRDO_Decoder::getCabling() {
  // get TGC cablingSvc

  const ITGCcablingServerSvc* TgcCabGet = 0;
  StatusCode sc = service("TGCcablingServerSvc", TgcCabGet);
  if(!sc.isSuccess()) {
    msg(sc.isFailure() ? MSG::FATAL : MSG::ERROR) << "Could not get TGCcablingServerSvc !" << endmsg;
    return sc;
  }
  
  sc = TgcCabGet->giveCabling(m_cabling);
  if(!sc.isSuccess()) {
    msg(sc.isFailure() ? MSG::FATAL : MSG::ERROR) << "Could not get ITGCcablingSvc from Server!" << endmsg;
    return sc;
  } else {
    ATH_MSG_DEBUG("Found the ITGCcablingSvc.");
  }
 
  return sc;
} 

