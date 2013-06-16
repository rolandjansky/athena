/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PtMultiplicitySet.cxx 364083 2011-05-06 09:09:55Z krasznaa $

// STL include(s):
#include <iostream>
#include <sstream>
#include <cassert>

// Local include(s):
#include "PtMultiplicitySet.h"

//***********************************************************************
//
//       Version : $Revision: 364083 $
//
//   Description :
//
//        Author : $Author: krasznaa $
//          Date : $Date: 2011-05-06 11:09:55 +0200 (Fri, 06 May 2011) $
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
