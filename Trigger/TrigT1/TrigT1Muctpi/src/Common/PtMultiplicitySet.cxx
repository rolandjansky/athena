/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PtMultiplicitySet.cxx 700318 2015-10-13 14:13:15Z wengler $

// STL include(s):
#include <iostream>
#include <sstream>
#include <cassert>

// Local include(s):
#include "PtMultiplicitySet.h"

//***********************************************************************
//
//       Version : $Revision: 700318 $
//
//   Description :
//
//        Author : $Author: krasznaa $
//          Date : $Date: 2015-10-13 16:13:15 +0200 (Tue, 13 Oct 2015) $
//
//
//
//***********************************************************************

namespace LVL1MUCTPI {

   PtMultiplicitySet::PtMultiplicitySet( unsigned int ptarr[ MAX_NUMBER_OF_THRESHOLDS ] ) {

      //
      // Initialise the internal array to zero:
      //
      for( unsigned int ix = 0; ix < MAX_NUMBER_OF_THRESHOLDS; ++ix ) {
         m_multiplicity_pt[ ix ] = 0;
      }

      //
      // If an array was given in the constructor, copy its contents:
      //
      if( ptarr ) {
         for( unsigned int i = 0; i < MAX_NUMBER_OF_THRESHOLDS; ++i ) {
            m_multiplicity_pt[ i ] = ptarr[ i ];
         }
      }

   }

   PtMultiplicitySet& PtMultiplicitySet::operator=( const PtMultiplicitySet& a ) {

      for( unsigned int i = 0; i < MAX_NUMBER_OF_THRESHOLDS; ++i ) {
         m_multiplicity_pt[ i ] = a.m_multiplicity_pt[ i ];
      }
      return *this;

   }

   PtMultiplicitySet& PtMultiplicitySet::operator+=( const PtMultiplicitySet& a) {

      // No check is performed for the "correctness" of the input PtMultiplicitySet,
      // since there should be no way in which it can be created in an "incorrect"
      // way...
      for( unsigned int i = 0; i < MAX_NUMBER_OF_THRESHOLDS; ++i ) {
         m_multiplicity_pt[ i ] += a.m_multiplicity_pt[ i ];
         // the multiplicities are clipped at MAX_MULTIPLICITY
         if( m_multiplicity_pt[ i ] > MAX_MULTIPLICITY ) {
            m_multiplicity_pt[ i ] = MAX_MULTIPLICITY;
         }
      }
      return *this;

   }

   // the definition of the + operator is based on that of the += operator
   // the multiplicities are clipped at MAX_MULTIPLICITY

   PtMultiplicitySet PtMultiplicitySet::operator+( const PtMultiplicitySet& a ) {

      PtMultiplicitySet result = *this;
      result += a;
      return result;

   }

   // Access to multiplicity of a particluar threshold
   int PtMultiplicitySet::getMultiplicity( const unsigned int threshold ) const {

      // check if threshold argument is valid
      if( threshold > MAX_NUMBER_OF_THRESHOLDS || threshold < 1 ) {
         std::cerr << "Invalid Threshold given" << std::endl;
         assert( 0 );
	 return( 0 );  // added to silence the coverty complaints
      }
      return m_multiplicity_pt[ threshold - 1 ];

   }

   void PtMultiplicitySet::clear() {

      for( unsigned int i = 0; i < MAX_NUMBER_OF_THRESHOLDS; ++i ) {
         m_multiplicity_pt[ i ] = 0;
      }

      return;
   }

   void PtMultiplicitySet::print( std::string& result ) const {

      std::ostringstream outStream;
      outStream << "\n Multiplicity Set : \n" << "--------------------\n\n";
      for( unsigned int ix = 0; ix < MAX_NUMBER_OF_THRESHOLDS; ++ix ) {
         outStream << "       " << ix + 1 << "   " << m_multiplicity_pt[ ix ] << "\n";
      }
      result = outStream.str();

      return;

   }

   void PtMultiplicitySet::addMuon( const unsigned int pt ) {

      // a muon is added if it is in the correct pt range and if the
      // maximal multiplicity for the correspnding pt thresholds has not been
      // reached yet.
      if( pt >= 1 && pt <= MAX_NUMBER_OF_THRESHOLDS ) {
         if( m_multiplicity_pt[ pt - 1 ] < MAX_MULTIPLICITY ) {
            m_multiplicity_pt[ pt - 1 ]++;
         }
      }
      return;

   }

} // namespace LVL1MUCTPI
