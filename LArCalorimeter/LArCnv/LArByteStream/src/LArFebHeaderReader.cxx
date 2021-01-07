/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawEvent/LArFebHeader.h"
#include "LArByteStream/LArRodBlockStructure.h"
#include "LArFebHeaderReader.h"


void LArFebHeaderReader::fillFebHeader(LArFebHeader* larFebHeader,           //output
				       const LArRodBlockStructure* rodBlock, //input 
				       const ROBFragment& rob) {             //input   
  larFebHeader->SetFormatVersion(rob.rod_version());
  larFebHeader->SetSourceId(rob.rod_source_id());
  larFebHeader->SetRunNumber(rob.rod_run_no());
  larFebHeader->SetELVL1Id(rob.rod_lvl1_id());
  larFebHeader->SetBCId(rob.rod_bc_id());
  larFebHeader->SetLVL1TigType(rob.rod_lvl1_trigger_type());
  larFebHeader->SetDetEventType(rob.rod_detev_type());
  
  //set DSP data
  const unsigned nsample=rodBlock->getNumberOfSamples();
  larFebHeader->SetRodStatus(rodBlock->getStatus());
  larFebHeader->SetDspCodeVersion(rodBlock->getDspCodeVersion()); 
  larFebHeader->SetDspEventCounter(rodBlock->getDspEventCounter()); 
  larFebHeader->SetRodResults1Size(rodBlock->getResults1Size()); 
  larFebHeader->SetRodResults2Size(rodBlock->getResults2Size()); 
  larFebHeader->SetRodRawDataSize(rodBlock->getRawDataSize()); 
  larFebHeader->SetNbSweetCells1(rodBlock->getNbSweetCells1()); 
  larFebHeader->SetNbSweetCells2(rodBlock->getNbSweetCells2()); 
  larFebHeader->SetNbSamples(nsample); 
  larFebHeader->SetOnlineChecksum(rodBlock->onlineCheckSum());
  larFebHeader->SetOfflineChecksum(rodBlock->offlineCheckSum());

  if(!rodBlock->hasControlWords()) {
    larFebHeader->SetFebELVL1Id(rob.rod_lvl1_id());
    larFebHeader->SetFebBCId(rob.rod_bc_id());
  } else {
    const uint16_t evtid = rodBlock->getCtrl1(0) & 0x1f;
    const uint16_t bcid  = rodBlock->getCtrl2(0) & 0x1fff;
    larFebHeader->SetFebELVL1Id(evtid);
    larFebHeader->SetFebBCId(bcid);
    for(int iadc=0;iadc<16;iadc++) {
      larFebHeader->SetFebCtrl1(rodBlock->getCtrl1(iadc));
      larFebHeader->SetFebCtrl2(rodBlock->getCtrl2(iadc));
      larFebHeader->SetFebCtrl3(rodBlock->getCtrl3(iadc));
    }
    for(unsigned int i = 0; i<nsample; i++ ) {
      larFebHeader->SetFebSCA(rodBlock->getRadd(0,i) & 0xff);
    }
  }//end else no control words

  return;
}
