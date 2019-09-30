/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 /////////////////////////////////////////////////////////////////// 	 
// BCM_RawDataProviderTool.cxx 	 
//   Implementation file for class BCM_RawDataProviderTool 	 
/////////////////////////////////////////////////////////////////// 	 
// (c) ATLAS BCM Detector software 	 
/////////////////////////////////////////////////////////////////// 	 
/////////////////////////////////////////////////////////////////// 	 
//  Version 00-00-01 12/05/2008 Daniel Dobos 	 
//  Version 00-00-02 19/05/2008 Daniel Dobos 	 
//  Version 00-00-11 05/02/2009 Daniel Dobos
///////////////////////////////////////////////////////////////////

#include "BCM_RawDataByteStreamCnv/BCM_RawDataProviderTool.h"

#include "InDetBCM_RawData/BCM_RDO_Container.h"
#include "BCM_RawDataByteStreamCnv/BCM_RodDecoder.h"

static const InterfaceID IID_IBCM_RawCollByteStreamTool("BCM_RawDataProviderTool", 1, 0);
const InterfaceID& BCM_RawDataProviderTool::interfaceID( )
{ return IID_IBCM_RawCollByteStreamTool; }

////////////////////////
// contructor
////////////////////////
BCM_RawDataProviderTool::BCM_RawDataProviderTool
( const std::string& type, const std::string& name,const IInterface* parent )
  :  AthAlgTool(type,name,parent),
     m_decoder("BCM_RodDecoder")
{
  declareProperty ("Decoder", m_decoder);
  declareInterface<BCM_RawDataProviderTool>(this);   
  m_lastLvl1ID = 0xffffffff;
  m_DecodeErrCount =0;
}

////////////////////////
// destructor 
////////////////////////
BCM_RawDataProviderTool::~BCM_RawDataProviderTool()
{}
 
////////////////////////
// initialize() -
////////////////////////
StatusCode BCM_RawDataProviderTool::initialize()
{
   StatusCode sc = AthAlgTool::initialize(); 
   if (sc.isFailure()) {
     if (msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "Failed to init baseclass" << endmsg;
     return StatusCode::FAILURE;
   }

   // Retrieve decoder
   if (m_decoder.retrieve().isFailure()) {
     if (msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "Failed to retrieve tool " << m_decoder << endmsg;
     return StatusCode::FAILURE;
   } else {
     if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "Retrieved tool " << m_decoder << endmsg;
   }

   return StatusCode::SUCCESS;
}

////////////////////////
// finalize() -
////////////////////////
StatusCode BCM_RawDataProviderTool::finalize()
{
   StatusCode sc = AthAlgTool::finalize();
   return sc;
}


StatusCode BCM_RawDataProviderTool::convert( std::vector<const ROBFragment*>& vecRobs, BCM_RDO_Container* rdoCont)
{
  if(vecRobs.size() == 0) return StatusCode::SUCCESS;

  std::vector<const ROBFragment*>::const_iterator rob_it = vecRobs.begin();

  // are we working on a new event ?
  if ( (*rob_it)->rod_lvl1_id() != m_lastLvl1ID) {
#ifdef BCM_DEBUG
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "New event, reset the collection set" << endmsg;
#endif
    // remember last Lvl1ID
    m_lastLvl1ID = (*rob_it)->rod_lvl1_id();
    // and clean up the container!
    rdoCont->clear();
  }

  // loop over the ROB fragments
  for(; rob_it!=vecRobs.end(); ++rob_it) {

    StatusCode sc = m_decoder->fillCollection(&**rob_it, rdoCont);
    if (sc != StatusCode::SUCCESS) {
       if (m_DecodeErrCount < 100) {
          if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "Problem with BCM ByteStream Decoding!" << endmsg;
       } else if (100 == m_DecodeErrCount) {
          if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "Too many Problems with BCM Decoding. Turning message off." << endmsg;
       }
        m_DecodeErrCount++;
      }
  }

  return StatusCode::SUCCESS;
}
