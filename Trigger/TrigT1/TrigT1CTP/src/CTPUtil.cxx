/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


// this class heasder file
#include "./CTPUtil.h"

// standard include(s):
#include <cmath>
#include <iostream>

// STL include(s):
#include <sstream>
#include <iomanip>

// LVL1 includes
#include "TrigT1Result/CTPRoI.h"

namespace LVL1CTP {

   int CTPUtil::getMuonMult( unsigned int word, int threshold ) {

      return static_cast<int>( ( word >> ( 3 * threshold ) ) & 0x7 );

   }

   int CTPUtil::getEMTMult( unsigned int word, int threshold ) {

      return static_cast<int>( ( word >> ( 3 * threshold + 1 ) ) & 0x7 );

   }

   int CTPUtil::getJetMult( unsigned int word, int threshold ) {

      return CTPUtil::getEMTMult( word, threshold );

   }

   int CTPUtil::getFJetMult( unsigned int word, int threshold ) {

      return static_cast<int>( ( word >> ( 2 * threshold + 1 ) ) & 0x3 );

   }

   int CTPUtil::getJetEMult( unsigned int word, int threshold ) {

      return static_cast<int>( ( word >> ( 25 + threshold ) ) & 0x1 );

   }

   int CTPUtil::getETMult( unsigned int word, int threshold ) {

      return static_cast<int>( ( word >> ( 9 + threshold ) ) & 0x1 );

   }

   int CTPUtil::getTMMult( unsigned int word, int threshold ) {

      return static_cast<int>( ( word >> ( 1 + threshold ) ) & 0x1 );

   }

   int CTPUtil::getMult( uint64_t word, unsigned int startbit, unsigned int endbit ) {

      return static_cast<int>( ( word >> startbit ) & static_cast<unsigned int>( pow( 2, endbit - startbit + 1 ) - 1 ) );

   }

   int CTPUtil::getMult( const std::vector<unsigned int>& words, unsigned int startbit, unsigned int endbit ) {
      std::bitset<256> bits = convertToBitset(words);
      std::bitset<256> mask = pow( 2, endbit - startbit + 1 ) - 1;
      bits >>= startbit;
      return static_cast<int>((bits&mask).to_ulong());
   }

   unsigned int CTPUtil::getMultTopo( uint64_t word, unsigned int cableStart, unsigned int cableEnd, unsigned int clock ) {

      unsigned int mult = 0;

      for(int cb = (int)cableEnd; cb >= (int)cableStart; --cb) { // must be signed int
         unsigned int b = 2 * cb + clock;
         mult <<= 1;
         mult += (word & ( 1UL << b )) ? 1 : 0 ;
      }

      return mult;
   }


   std::bitset<256> CTPUtil::convertToBitset(const std::vector<uint32_t>& words) {
    
      std::bitset<256> bitset;

      for (size_t i(0); i < words.size(); ++i) {
         std::bitset<256> bs = words[i];
         bs <<= (i * 32);
         bitset |= bs;
      }

      return bitset;
   }
    
    
   std::bitset<512> CTPUtil::convertToLargeBitset(const std::vector<ROIB::CTPRoI>& words) {
        
      std::bitset<512> bitset;
        
      for (size_t i(0); i < words.size(); ++i) {
         std::bitset<512> bs = words[i].roIWord();
         bs <<= (i * 32);
         bitset |= bs;
      }
        
      return bitset;
   }
    
   std::bitset<512> CTPUtil::convertToLargeBitset(const std::vector<uint32_t>& words) {
        
      std::bitset<512> bitset;
        
      for (size_t i(0); i < words.size(); ++i) {
         std::bitset<512> bs = words[i];
         bs <<= (i * 32);
         bitset |= bs;
      }
        
      return bitset;
   }

   std::bitset<256> CTPUtil::convertToBitset(const std::vector<ROIB::CTPRoI>& words) {
    
      std::bitset<256> bitset;

      for (size_t i(0); i < words.size(); ++i) {
         std::bitset<256> bs = words[i].roIWord();
         bs <<= (i * 32);
         bitset |= bs;
      }

      return bitset;
   }

   unsigned int CTPUtil::alignBits( int value, int startbit, int endbit ) {

      int ostart = std::min( startbit, endbit );
      int oend   = std::max( startbit, endbit );
      int start = std::max( ostart, 0 );
      int end   = std::min( oend, 32 );

      unsigned int result = value;
      if( ostart < 0 ) result >>= static_cast< int >( std::abs( static_cast< double >( ostart ) ) );
      else result <<= ostart;
      result &= createMask( start, end );

      return result;

   }

   unsigned int CTPUtil::createMask( int startbit, int endbit ) {

      int start = std::min( std::max( startbit, 0 ), std::max( endbit, 0 ) );
      int end   = std::max( std::min( startbit, 32 ), std::min( endbit, 32 ) );

      unsigned int result = 0;
      for( int i = 0; i <= ( end - start ); ++i ) {
         result <<= 1;
         result |= 1;
      }
      result <<= start;

      return result;

   }

   std::string CTPUtil::printHex(const std::vector<uint32_t>& rhs, const std::string& delimiter)
   {
      std::ostringstream s;

      // switch to hex format
      s << std::hex <<  std::setfill( '0' );
    
      if (rhs.size() > 0) 
         s << " 0x" << std::setw(8) << rhs[0];
      for (size_t i(1); i < rhs.size(); ++i) {
         s << delimiter << " 0x" << std::setw(8) << rhs[i];
      }
    
      // switch back to normal format
      s  << std::dec << std::setfill(' ');
    
      return s.str();
   }
  

} // namespace LVL1CTP
