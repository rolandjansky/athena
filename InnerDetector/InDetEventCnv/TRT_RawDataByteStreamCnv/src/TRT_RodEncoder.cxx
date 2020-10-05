/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Implementation of TRT_RodEncoder class 

#include <deque>

#include "TRT_RodEncoder.h" 

#include "ByteStreamData/RawEvent.h"
#include "InDetIdentifier/TRT_ID.h"
#include "InDetRawData/TRT_RDORawData.h"
#include "TRT_Cabling/ITRT_CablingSvc.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

/** constructor 
*/ 
TRT_RodEncoder::TRT_RodEncoder()
   :m_trt_id          ( nullptr ),
    m_CablingSvc      ( "TRT_CablingSvc", "TRT_RodEncoder" ),
    m_RodBlockVersion ( 0 )
{}

/** destructor 
*/ 
TRT_RodEncoder::~TRT_RodEncoder() {}

/** for a given ROD fragment, convert TRT RDOs to a vector of 32bit words
 */
StatusCode
TRT_RodEncoder::fillROD( std::vector<uint32_t>&  v32rod )
{
   if ( ! m_CablingSvc )
      return StatusCode::FAILURE;

   int TRT_MaxBufferSize = 120*16;
   // Make sure vector is big enough, fill it with 0
   v32rod.resize(TRT_MaxBufferSize,0 );

   VRDO::iterator       rdo_it     = m_RDOs.begin();
   const VRDO::iterator rdo_it_end = m_RDOs.end();

   const RDO* rawdata;
   uint32_t   StrawWord;
   Identifier StrawID;
   uint32_t   BufferOffset;

   while ( rdo_it != rdo_it_end )
   {
      rawdata = *rdo_it;
      
      StrawWord = rawdata->getWord();
      StrawID   = rawdata->identify();

      BufferOffset = m_CablingSvc->getBufferOffset( StrawID );

      //      if ( 0 == BufferOffset )
      //	cout << m_trt_id->print_to_string( StrawID ) << " ->" 
      //	     << BufferOffset << "<- " << endl;

      v32rod[BufferOffset] = StrawWord;

      ++rdo_it;
   }

   return StatusCode::SUCCESS;  
}       // end of fillROD(...)

/** for a given ROD fragment, compression versions convert TRT RDOs to a vector of 32bit words
 */
StatusCode
TRT_RodEncoder::fillROD3( std::vector<uint32_t>&  v32rod )
{
   if ( ! m_CablingSvc )
      return StatusCode::FAILURE;

   const int TRT_MaxBufferSize     = 120*16;
   int tmp_data[TRT_MaxBufferSize] = {0}; // does init to 0 for all !

   std::deque<bool> bitstowrite;

   VRDO::iterator       rdo_it     = m_RDOs.begin();
   const VRDO::iterator rdo_it_end = m_RDOs.end();

   const RDO* rawdata;
   uint32_t   StrawWord;
   Identifier StrawID;
   uint32_t   BufferOffset;

   while ( rdo_it != rdo_it_end )
   {
      rawdata = *rdo_it;
      
      StrawWord = rawdata->getWord();
      StrawID   = rawdata->identify();

      BufferOffset = m_CablingSvc->getBufferOffset( StrawID );

      //      if ( 0 == BufferOffset )
      //	cout << m_trt_id->print_to_string( StrawID ) << " ->" 
      //	     << BufferOffset << "<- " << endl;

      tmp_data[BufferOffset] = StrawWord;

      ++rdo_it;
   }

   for ( int i=0; i<TRT_MaxBufferSize; i++ )
   {
     if ( tmp_data[i] )
     {
       /*
	* Push escape code
	*/
       bitstowrite.push_back( 0 );
       bitstowrite.push_back( 0 );
       bitstowrite.push_back( 0 );
       bitstowrite.push_back( 0 );
       bitstowrite.push_back( 0 );

       /*
	* Push Straw Word
	*/

       //for ( int j=27-1; j>=0; j-- )
       for ( int j=0; j<27; j++ )
	 bitstowrite.push_back( (tmp_data[i] >> j) & 1 );
     }
     else
       bitstowrite.push_back( 1 );
   }

   int BufferLength;
			 
   BufferLength = bitstowrite.size() / 32;
   if ( bitstowrite.size() % 32 )
     BufferLength++;

   //   v32rod.resize( BufferLength );             // Make sure vector is big enough

   int bit_in_word = 0;
   int out_word = 0;
   while ( ! bitstowrite.empty() )
   {
     out_word = out_word | ( (bitstowrite.front()?1:0) << bit_in_word );
     bitstowrite.pop_front();
     bit_in_word++;

     if ( bit_in_word > 31 )
     {
       v32rod.push_back( out_word );
       out_word = 0;
       bit_in_word = 0;
     }
   }

   if ( 0 != bit_in_word )
     v32rod.push_back( out_word );

   return StatusCode::SUCCESS;  
}       // end of fillROD(...)


