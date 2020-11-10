/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArBadChannelTool/LArBadFeb2Ascii.h"

#include "LArBadChannelTool/LArBadChannelDBTools.h"
#include "LArIdentifier/LArOnlineID.h"
#include <fstream>

LArBadFeb2Ascii::LArBadFeb2Ascii(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm( name, pSvcLocator),
  m_BFKey("LArBadFeb")
{
  declareProperty("BFKey",m_BFKey);
  declareProperty("FileName",m_fileName="");
}

LArBadFeb2Ascii::~LArBadFeb2Ascii() {}


StatusCode LArBadFeb2Ascii::initialize() {

  ATH_MSG_INFO ( "initialize()" );
  ATH_CHECK(m_BFKey.initialize());
  return StatusCode::SUCCESS;
}

StatusCode LArBadFeb2Ascii::execute() 
{return StatusCode::SUCCESS;}

StatusCode LArBadFeb2Ascii::finalize() {
  const LArOnlineID* larOnlineID = nullptr;
  ATH_CHECK( detStore()->retrieve(larOnlineID,"LArOnlineID") );
  
  std::ostream *out = &(std::cout); 
  std::ofstream outfile;
  if (m_fileName.size()) {
    outfile.open(m_fileName.c_str(),std::ios::out);
    if (outfile.is_open()) {
      ATH_MSG_INFO ( "Writing to file " << m_fileName );
      out = &outfile;
    }
    else
      ATH_MSG_ERROR ( "Failed to open file " << m_fileName );
  }


  SG::ReadCondHandle<LArBadFebCont> bfc{m_BFKey};
  const LArBadFebCont* badfebCont{*bfc};


  const LArBadFebBitPacking packing;


  std::vector<HWIdentifier>::const_iterator it  = larOnlineID->feb_begin();
  std::vector<HWIdentifier>::const_iterator it_e= larOnlineID->feb_end();
  unsigned count=0;
  for(;it!=it_e;++it) {  
    const HWIdentifier fid=*it;
    LArBadFeb bf = badfebCont->status(fid);
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
  ATH_MSG_INFO ( "Found " << count << " entries in the bad-FEB database." );
  if (outfile.is_open())
    outfile.close();
  return StatusCode::SUCCESS;
}


