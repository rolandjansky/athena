/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
// constructor
////////////////////////
BCM_RawDataProviderTool::BCM_RawDataProviderTool
( const std::string& type, const std::string& name,const IInterface* parent )
  :  AthAlgTool(type,name,parent),
     m_decoder("BCM_RodDecoder")
{
  declareProperty ("Decoder", m_decoder);
  declareInterface<BCM_RawDataProviderTool>(this);   
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
   ATH_CHECK( AthAlgTool::initialize() );

   // Retrieve decoder
   ATH_CHECK( m_decoder.retrieve() );
   ATH_MSG_INFO( "Retrieved tool " << m_decoder );

   return StatusCode::SUCCESS;
}

////////////////////////
// finalize() -
////////////////////////
StatusCode BCM_RawDataProviderTool::finalize()
{
   ATH_CHECK( AthAlgTool::finalize() );
   return StatusCode::SUCCESS;
}


StatusCode BCM_RawDataProviderTool::convert( std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vecRobs, BCM_RDO_Container* rdoCont) const
{
  if(vecRobs.size() == 0) return StatusCode::SUCCESS;

  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>::const_iterator rob_it = vecRobs.begin();

  // loop over the ROB fragments
  for(; rob_it!=vecRobs.end(); ++rob_it) {

    StatusCode sc = m_decoder->fillCollection(&**rob_it, rdoCont);
    if (sc != StatusCode::SUCCESS) {
       if (m_DecodeErrCount < 100) {
          ATH_MSG_INFO( "Problem with BCM ByteStream Decoding!" );
       } else if (100 == m_DecodeErrCount) {
          ATH_MSG_INFO( "Too many Problems with BCM Decoding. Turning message off." );
       }
        m_DecodeErrCount++;
      }
  }

  return StatusCode::SUCCESS;
}
