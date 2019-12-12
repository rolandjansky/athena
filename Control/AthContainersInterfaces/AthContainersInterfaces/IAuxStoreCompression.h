/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef ATHCONTAINERSINTERFACES_IAUXSTORECOMPRESSION_H
#define ATHCONTAINERSINTERFACES_IAUXSTORECOMPRESSION_H


#include "AthContainersInterfaces/AuxTypes.h"
#include <typeinfo>
#include <set>


namespace SG {


class IAuxStoreCompression
{
public:
  /// Destructor
  virtual ~IAuxStoreCompression() {}

  /// Set the list of variables to be compressed
  virtual void
  setCompressedAuxIDs ( const std::set<std::string>& /*attr*/ ) = 0;

  /// Get the list of variables to be compressed
  virtual SG::auxid_set_t
  getCompressedAuxIDs() const = 0;

  /// Get compressed value given a float
  virtual float
  getCompressedValue( float /*val*/ ) const = 0;

}; // class IAuxStoreCompression


} // namespace SG


#ifndef XAOD_STANDALONE
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( SG::IAuxStoreCompression, 55263982, 1 )
#endif


#endif // not ATHCONTAINERSINTERFACES_IAUXSTOREIO_H
