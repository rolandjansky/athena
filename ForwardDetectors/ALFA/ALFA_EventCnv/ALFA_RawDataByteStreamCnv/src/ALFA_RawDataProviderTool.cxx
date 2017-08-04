/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_RawDataByteStreamCnv/ALFA_RawDataProviderTool.h"

using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;

static const InterfaceID IID_IALFA_RawCollByteStreamTool("ALFA_RawDataProviderTool", 1, 0);
const InterfaceID& ALFA_RawDataProviderTool::interfaceID( )
{ return IID_IALFA_RawCollByteStreamTool; }

////////////////////////
// contructor
////////////////////////

ALFA_RawDataProviderTool::ALFA_RawDataProviderTool
(const std::string& type, const std::string& name, const IInterface* parent)
:  AthAlgTool(type,name,parent),
   m_decoder("ALFA_Decoder"),
   m_robIdSet()
      {
   
  declareProperty ("Decoder", m_decoder);
  declareInterface<ALFA_RawDataProviderTool>(this);  
  m_lastLvl1ID = 0;
}

////////////////////////
// destructor
////////////////////////
ALFA_RawDataProviderTool::~ALFA_RawDataProviderTool(){}

////////////////////////
// initialize() -
////////////////////////
StatusCode ALFA_RawDataProviderTool::initialize() {

  StatusCode sc = AthAlgTool::initialize(); 
  
  msg(MSG::DEBUG) << "INITIALIZE ALFA_RawDataProviderTool" << endmsg;
  
  if (sc.isFailure()) {
     msg(MSG::FATAL) << " Failed to init baseclass" << endmsg;
     return StatusCode::FAILURE;
   }

   // Retrieve decoder
   if (m_decoder.retrieve().isFailure()) {
     msg(MSG::FATAL) << " Failed to retrieve tool " << m_decoder << endmsg;
     return StatusCode::FAILURE;
   } else {
   msg(MSG::DEBUG) << "Retrieved tool m_decoder " << m_decoder << endmsg;
   }

   return StatusCode::SUCCESS;
  
  
}



////////////////////////
// convert() -
////////////////////////

StatusCode ALFA_RawDataProviderTool::convert( std::vector<const ROBFragment*>& vecRobs, ALFA_RawDataContainer* rdoCont)

{
  msg(MSG::DEBUG) << "CONVERT ALFA_RawDataProviderTool" << endmsg;
  
   static uint32_t LastLvl1ID = 0xffffffff;
   
  msg(MSG::DEBUG) << "Number of ROB fragments is " << vecRobs.size() << endmsg;

  if(vecRobs.size() == 0) return StatusCode::SUCCESS;

  std::vector<const ROBFragment*>::const_iterator rob_it = vecRobs.begin();

  msg(MSG::DEBUG) << "(*rob_it)->rod_lvl1_id() " << (*rob_it)->rod_lvl1_id() << " LastLvl1ID " << LastLvl1ID << endmsg;
 
  // are we working on a new event ?
  if ( ((*rob_it)->rod_lvl1_id())!= LastLvl1ID ) {

msg(MSG::DEBUG) << "**********************" <<  endmsg;
    msg(MSG::DEBUG) << "New event, reset the collection set" << endmsg;
  msg(MSG::DEBUG) << "**********************" <<  endmsg;
  
    
    // remember last Lvl1ID
    LastLvl1ID =  ((*rob_it)->rod_lvl1_id());
   
    // reset list of known robIds
    m_robIdSet.clear();
    // and clean up the container!
    rdoCont->clear();
  }
  else{
  msg(MSG::DEBUG) << "Same event!!!!!!!!!!!!!!" << endmsg;
  }


  // loop over the ROB fragments
  for(; rob_it!=vecRobs.end(); ++rob_it) {

    uint32_t robid = (*rob_it)->rod_source_id();
    
    
    // check if this ROBFragment was already decoded
    if (!m_robIdSet.insert(robid).second) {
     msg(MSG::DEBUG) << " ROB Fragment with ID " << std::hex<<robid<<std::dec << " already decoded, skip" << endmsg;
   } else {
      StatusCode sc = m_decoder->fillCollection(&**rob_it, rdoCont);
      if (sc.isFailure()) return sc;
      msg(MSG::DEBUG) << " ROB Fragment with ID " << std::hex<<robid<<std::dec << " fill Container" << endmsg;
    }
  }// loop over the ROB fragments

  return StatusCode::SUCCESS;

}


////////////////////////
// finalize() -
////////////////////////
StatusCode ALFA_RawDataProviderTool::finalize() { 

 StatusCode sc = AthAlgTool::finalize();

msg(MSG::DEBUG) << "FINALIZE ALFA_RawDataProviderTool" << endmsg;

   return sc;
}
