/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "LUCID_RawDataByteStreamCnv/LUCID_RodDecoder.h"

LUCID_RodDecoder::LUCID_RodDecoder() {}
LUCID_RodDecoder::~LUCID_RodDecoder() {}

StatusCode LUCID_RodDecoder::decode(const ROBFragment* robFragment, std::vector<uint32_t>& data_block) {
  
  if (msgLevel(MSG::DEBUG)) msg(MSG::DEBUG) << " LUCID_RodDecoder::decode" << endmsg;
  
  uint32_t ROD_source_ID = robFragment->rod_source_id();
  uint32_t ROD_ndata     = robFragment->rod_ndata();
  uint32_t ROD_nstatus   = robFragment->rod_nstatus();
 
  if (msgLevel(MSG::DEBUG)) msg(MSG::DEBUG) 
    << std::hex
    << " ROD_source_ID: " << ROD_source_ID << endmsg
    << " getSourceID(): " << getSourceID() << endmsg
    << " ROD_ndata:     " << ROD_ndata     << endmsg
    << " ROD_nstatus:   " << ROD_nstatus   << endmsg
    << std::dec;
  
  if (ROD_source_ID != getSourceID()       ) return StatusCode::FAILURE;
  if (ROD_ndata     != 4 && ROD_ndata != 12) return StatusCode::FAILURE;
  if (ROD_nstatus   != 1                   ) return StatusCode::FAILURE;

  if (msgLevel(MSG::DEBUG)) msg(MSG::DEBUG) << " Filling the data_block " << endmsg; 
  
  OFFLINE_FRAGMENTS_NAMESPACE::PointerType vintData;
  OFFLINE_FRAGMENTS_NAMESPACE::PointerType vintStat;
  
  robFragment->rod_data  (vintData);
  robFragment->rod_status(vintStat);
  
  for (uint32_t dataword_it = 0; dataword_it < ROD_ndata; ++dataword_it)
    data_block.push_back(vintData[dataword_it]);
  
  for (uint32_t dataword_it = 0; dataword_it < ROD_nstatus; ++dataword_it)
    data_block.push_back(vintStat[dataword_it]);
  
  return StatusCode::SUCCESS;
}
