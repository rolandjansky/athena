/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODCORE_AUXCOMPRESSION_H
#define XAODCORE_AUXCOMPRESSION_H

// System include(s):
#include <set>
#include <string>
#include <vector>

// EDM include(s):
#include "AthContainersInterfaces/AuxTypes.h"

namespace xAOD {

   ///
   class AuxCompression {

   public:
      /// Default constructor
      AuxCompression();

      /// Set which variables should be compressed
      virtual void
      setCompressedAuxIDs( const std::vector< std::set< std::string > >& attributes );

      /// Return those variables that are selected to be compressed
      virtual SG::auxid_set_t
      getCompressedAuxIDs( const SG::auxid_set_t& fullset, const bool& highComp = true ) const;

      /// Set the number of bits that should be used in the compression
      virtual void
      setCompressionBits( const std::vector< unsigned int >& nbits );

      /// Return the number of bits that should be used in the compression
      virtual unsigned int
      getCompressionBits( const bool& highComp = true ) const;

      /// Enum for the indices used for the high and low compression setup
      enum FloatCompressionBits { High = 0, Low = 1, NTotal = 2 };

   protected:
      /// Properties following the variable selection convention
      std::vector< std::set< std::string > > m_names;

      /// Vector holding the number of mantissa bits for the compression
      std::vector< unsigned int > m_nbits;

   }; // class AuxCompression

} // namespace xAOD

#endif // XAODCORE_AUXCOMPRESSION_H
