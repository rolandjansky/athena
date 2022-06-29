/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_RawDataByteStreamCnv/ALFA_RawDataProviderTool_charge.h"

using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;

////////////////////////
// contructor
////////////////////////

ALFA_RawDataProviderTool_charge::ALFA_RawDataProviderTool_charge
(const std::string& type, const std::string& name, const IInterface* parent)
:  AthAlgTool(type,name,parent)
{
}

////////////////////////
// destructor
////////////////////////
ALFA_RawDataProviderTool_charge::~ALFA_RawDataProviderTool_charge(){}

////////////////////////
// initialize() -
////////////////////////
StatusCode ALFA_RawDataProviderTool_charge::initialize() {

   // Retrieve decoder
   ATH_CHECK(m_decoder_charge.retrieve());

   return StatusCode::SUCCESS;
}



////////////////////////
// convert() -
////////////////////////

StatusCode ALFA_RawDataProviderTool_charge::convert_charge( std::vector<const ROBFragment*>& vecRobs, ALFA_RawDataContainer_charge* rdoCont)

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
      ATH_CHECK( m_decoder_charge->fillCollection(&**rob_it, rdoCont) );
      ATH_MSG_DEBUG(" ROB Fragment with ID " << std::hex<<robid<<std::dec << " fill Container");
    }
  }// loop over the ROB fragments

  return StatusCode::SUCCESS;
}
