/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_RawDataByteStreamCnv/ALFA_RawDataProviderTool.h"

using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;

////////////////////////
// contructor
////////////////////////

ALFA_RawDataProviderTool::ALFA_RawDataProviderTool
(const std::string& type, const std::string& name, const IInterface* parent)
:  AthAlgTool(type,name,parent)
{
}

////////////////////////
// destructor
////////////////////////
ALFA_RawDataProviderTool::~ALFA_RawDataProviderTool(){}

////////////////////////
// initialize() -
////////////////////////
StatusCode ALFA_RawDataProviderTool::initialize() {

   // Retrieve decoder
   ATH_CHECK( m_decoder.retrieve() );

   return StatusCode::SUCCESS;
}



////////////////////////
// convert() -
////////////////////////

StatusCode ALFA_RawDataProviderTool::convert( std::vector<const ROBFragment*>& vecRobs, ALFA_RawDataContainer* rdoCont)

{
  std::set<uint32_t> robIdSet;

  ATH_MSG_DEBUG("Number of ROB fragments is " << vecRobs.size());

  if(vecRobs.size() == 0) return StatusCode::SUCCESS;

  std::vector<const ROBFragment*>::const_iterator rob_it = vecRobs.begin();

  // loop over the ROB fragments
  for(; rob_it!=vecRobs.end(); ++rob_it) {

    uint32_t robid = (*rob_it)->rod_source_id();
    
    
    // check if this ROBFragment was already decoded
    if (!robIdSet.insert(robid).second) {
      ATH_MSG_DEBUG(" ROB Fragment with ID " << std::hex<<robid<<std::dec << " already decoded, skip");
    } else {
      StatusCode sc = m_decoder->fillCollection(&**rob_it, rdoCont);
      if (sc.isFailure()) return sc;
      ATH_MSG_DEBUG(" ROB Fragment with ID " << std::hex<<robid<<std::dec << " fill Container");
    }
  }// loop over the ROB fragments

  return StatusCode::SUCCESS;

}

