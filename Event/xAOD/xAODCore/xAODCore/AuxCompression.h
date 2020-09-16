/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODCORE_AUXCOMPRESSION_H
#define XAODCORE_AUXCOMPRESSION_H

// System include(s):
#include <map>
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

      /// Set which variables should be compressed per compression setting
      virtual void
      setCompressedAuxIDs( const std::map< unsigned int, std::set< std::string > >& attributes );

      /// Return those variables that are selected to be compressed per compression setting
      virtual std::map< unsigned int, SG::auxid_set_t >
      getCompressedAuxIDs( const SG::auxid_set_t& fullset ) const;

   protected:
      /// Properties following the variable selection convention
      std::map< unsigned int, std::set< std::string > > m_compression_map;

   }; // class AuxCompression

} // namespace xAOD

#endif // XAODCORE_AUXCOMPRESSION_H
