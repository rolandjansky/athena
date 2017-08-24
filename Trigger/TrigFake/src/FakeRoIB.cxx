/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <fstream>
#include "TrigT1Result/RoIBResult.h"
#include "eformat/SourceIdentifier.h"
#include "FakeRoIB.h"
using namespace ROIB;

FakeRoIB::FakeRoIB( const std::string& name, ISvcLocator* pSvcLocator ):                                
  AthAlgorithm( name, pSvcLocator ), 
  m_eventCounter( 0 ) {
  declareProperty( "InputFile", m_inputFile, "File wiht fake input" );
}

StatusCode FakeRoIB::initialize() {
  std::ifstream inputFile( m_inputFile );
  if ( not inputFile.good() ) {
    ATH_MSG_DEBUG( "Input file is wrong: " << m_inputFile );
    return StatusCode::FAILURE;
  }
  auto checkBitChar = [&]( char c ){ return ( ( c == '1' or c == '0' ) ?  StatusCode::SUCCESS : StatusCode::FAILURE );  };
  auto isChar   = [&]( char c, const char req ) { return ( ( c == req ) ?  StatusCode::SUCCESS : StatusCode::FAILURE ); };

  auto setBit   = [&]( std::vector<uint32_t>& v, size_t position ) {
    size_t word = position / 32;
    uint32_t bitMask  = 1<< ( position % 32 );
    v[word] = v[word] | bitMask;
  };

  std::string line;
  while ( std::getline( inputFile, line ) ) {
    std::istringstream items( line );
    std::vector<uint32_t>  TBP( 16 ); // 16  x 32 =  512 bits, number of CTP item bits
    std::vector<uint32_t>  TAV( 16 );

    while ( items )  {
      int itemID( 0 );
      items >> itemID;
      char colon;
      items >> colon;
      CHECK( isChar( colon, ':' ) );
      // now we will read pttern like 1,0 which TBP and TAV
      char TBPBit, TAVBit, coma;

      items >> TBPBit >> coma >> TAVBit;
      CHECK( checkBitChar( TBPBit ) );
      CHECK( checkBitChar( TAVBit ) );

      CHECK( isChar( coma, ',' ) );
      
      if ( TBPBit == '1' ) setBit( TBP, itemID );
      if ( TAVBit == '1' ) setBit( TAV, itemID );
    }
    m_data.push_back( CTPRecord( TBP, TAV ) );
  }

  return StatusCode::SUCCESS;
}

StatusCode FakeRoIB::execute() {
  const size_t index = m_eventCounter % m_data.size(); 
  ATH_MSG_DEBUG( "Inserting event: " << index );
  
  CTPResult ctp = FakeRoIBHelper::buildCTPFragment( m_data[index].TBP, m_data[index].TAV, index );
  ATH_MSG_DEBUG( "CTP words: " << ctp.print() );
  if ( not ctp.isComplete() )  {
    ATH_MSG_ERROR( "Cant't construct compte CTP fragment" );
    return StatusCode::FAILURE;
  }
    
  RoIBResult *result = new RoIBResult ( ctp );

  CHECK( evtStore()->record( result, "RoIBResult" ) );

  m_eventCounter ++;
  return StatusCode::SUCCESS;
}




namespace FakeRoIBHelper {
  CTPResult buildCTPFragment( const std::vector<uint32_t>& tbp, const std::vector<uint32_t>& tav,  uint32_t index ) {
    const uint32_t eformatVersion = 5;
    const uint32_t subdetID = 0x770001;
    Header h( subdetID, index, eformatVersion );

    std::vector<uint32_t> in( 2 ); // unused timing words
    in[0] = 0x00;
    in[1] = 0x00;
    in.insert( in.end(), tbp.begin(), tbp.end() );
    in.insert( in.end(), tav.begin(), tav.end() );
    Trailer t( in.size() ); 
    CTPResult ctp( eformatVersion, h, t, in );
    return ctp;
  }

}
