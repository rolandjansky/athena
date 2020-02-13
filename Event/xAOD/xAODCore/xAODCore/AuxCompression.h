/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODCORE_AUXCOMPRESSION_H
#define XAODCORE_AUXCOMPRESSION_H

// System include(s):
#include <set>
#include <string>

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
      setCompressedAuxIDs( const std::set< std::string >& attributes );

      /// Return those variables that are selected to be compressed
      virtual SG::auxid_set_t
      getCompressedAuxIDs( const SG::auxid_set_t& fullset ) const;

   protected:
      /// Properties following the variable selection convention
      std::set< std::string > m_names;

   }; // class AuxCompression

} // namespace xAOD

#endif // XAODCORE_AUXCOMPRESSION_H
