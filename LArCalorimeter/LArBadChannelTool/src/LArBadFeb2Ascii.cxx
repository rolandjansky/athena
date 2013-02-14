/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArBadChannelTool/LArBadFeb2Ascii.h"

#include "StoreGate/StoreGate.h"

//#include "LArRecConditions/ILArBadChanTool.h"
#include "LArBadChannelTool/LArBadChanTool.h"
#include "LArBadChannelTool/LArBadChannelDBTools.h"
#include "LArIdentifier/LArOnlineID.h"
#include <fstream>

LArBadFeb2Ascii::LArBadFeb2Ascii(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm( name, pSvcLocator),
  m_BadChanTool("LArBadChanTool"),
  m_detStore(0)
{
  declareProperty("BadChannelTool", m_BadChanTool, "public, shared BadChannelTool");
  declareProperty("FileName",m_fileName="");
}

LArBadFeb2Ascii::~LArBadFeb2Ascii() {}


StatusCode LArBadFeb2Ascii::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "initialize()" << endreq;

  if ( m_BadChanTool.retrieve().isFailure() ) {
    log << MSG::FATAL  << "Could not retrieve bad channel tool" << endreq;
    return StatusCode::FAILURE;
  }

  // locate the conditions store ptr to it.
  if (service("DetectorStore", m_detStore).isFailure()) {
    log <<MSG::ERROR <<"Could not find DetStore" <<endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode LArBadFeb2Ascii::execute() 
{return StatusCode::SUCCESS;}

StatusCode LArBadFeb2Ascii::finalize() {
  MsgStream log(msgSvc(), name());

  const LArOnlineID* larOnlineID;
  StatusCode sc = m_detStore->retrieve(larOnlineID,"LArOnlineID");
  if (sc.isFailure()) {
    log  << MSG::ERROR << "Unable to retrieve  LArOnlineID from DetectorStore" 
	 << endreq;
    return StatusCode::FAILURE;
  }

  
  std::ostream *out = &(std::cout); 
  std::ofstream outfile;
  if (m_fileName.size()) {
    outfile.open(m_fileName.c_str(),std::ios::out);
    if (outfile.is_open()) {
      log << MSG::INFO << "Writing to file " << m_fileName << endreq;
      out = &outfile;
    }
    else
      log << MSG::ERROR << "Failed to open file " << m_fileName << endreq;
  }

  const LArBadFebBitPacking packing;


  std::vector<HWIdentifier>::const_iterator it  = larOnlineID->feb_begin();
  std::vector<HWIdentifier>::const_iterator it_e= larOnlineID->feb_end();
  unsigned count=0;
  for(;it!=it_e;++it) {  
    const HWIdentifier fid=*it;
    LArBadFeb bf = m_BadChanTool->febStatus(fid);
    if (bf.packedData()) {
      ++count;
      (*out) << larOnlineID->barrel_ec(fid) << " " 
	     << larOnlineID->pos_neg(fid) << " "
	     << larOnlineID->feedthrough(fid) << " "
	     << larOnlineID->slot(fid) << " ";

      (*out) << packing.stringStatus(bf);
      (*out) << "  # 0x" << std::hex << fid.get_identifier32().get_compact() << std::dec << std::endl;
    }
  }
  log << MSG::INFO << "Found " << count << " entries in the bad-FEB database." << endreq;
  if (outfile.is_open())
    outfile.close();
  return StatusCode::SUCCESS;
}


