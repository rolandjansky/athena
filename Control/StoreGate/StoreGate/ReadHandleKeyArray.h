/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// WARNING! WARNING! WARNING!
//
// This version of the file has been modified for use in 21.2 and
// should not be swept into master.


#ifndef STOREGATE_READHANDLEKEYARRAY_H
#define STOREGATE_READHANDLEKEYARRAY_H 1

#include "StoreGate/VarHandleKeyArray.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadHandle.h"

#include <vector>
#include <string>

namespace SG {

  /**
   * @class SG::ReadHandleKeyArray<T>
   * @brief class to hold an array of ReadHandleKeys
   *
   * since it inherits from std::vector, all vector operations are 
   * permitted.
   *
   * initialization can be done in three ways.
   * 1: with an std::vector<ReadHandleKey> as a parameter
   *    SG::ReadHandleKeyArray<foo> m_foo ( std::vector<ReadHandleKey> );
   * 2: with an initializer list of ReadHandleKeys
   *    SG::ReadHandleKeyArray<foo> m_foo { ReadHandleKey<foo> k1, ReadHandleKey<foo> k2 };
   * 3: with an initializer list of std::strings, that will be used to
   *    internally create ReadHandleKeys with those initializers
   *    SG::ReadHandleKeyArray<foo> m_foo { "key1", "key2", "key3" };
   */

  template <class T>
  class ReadHandleKeyArray : public VarHandleKeyArrayCommon< ReadHandleKey<T> > {
  public:
    /**
     * @brief default Constructor from a ReadHandleKeyArray
     */
    ReadHandleKeyArray(){};

    /**
     * @brief Constructor from a ReadHandleKeyArray that takes a vector
     * of ReaDHandleKeys
     * @param v vector of ReadHandleKey
     */
    ReadHandleKeyArray( const std::vector<ReadHandleKey<T>>& v ):
      VarHandleKeyArrayCommon<ReadHandleKey<T>> ( v ) {};

    /**
     * @brief Constructor from a ReadHandleKeyArray that takes an 
     * initializer list of ReadHandleKeys
     * @param l initializer list of ReadHandleKey
     */
    ReadHandleKeyArray( std::initializer_list<ReadHandleKey<T>> l ):
      VarHandleKeyArrayCommon<ReadHandleKey<T>> {l} {};

    /**
     * @brief Constructor from a ReadHandleKeyArray that takes an 
     * initializer list of std::strings.
     * @param l initializer list of std::strings used to create the
     *          ReadHandleKeys
     */
    ReadHandleKeyArray( std::initializer_list<std::string> l ):
      VarHandleKeyArrayCommon<ReadHandleKey<T>> {l} {};

    /**
     * @brief auto-declaring Property Constructor from a HandleKeyArray 
     * that takes an initializer list of std::strings, and associates the WHKA
     * with the specified Property name
     * @param name name of Property
     * @param l initializer list of std::strings used to create the
     *          HandleKeys
     * @param doc documentation string
     */
    template <class OWNER>
    inline ReadHandleKeyArray( OWNER* owner,
                                std::string name,
                                std::initializer_list<std::string> l,
                                std::string doc="") :
      VarHandleKeyArrayCommon<ReadHandleKey<T>> {l} {
      owner->declareProperty(std::move(name), *this, std::move(doc));
    }

    /**
     * @brief return the type (Read/Write/Update) of handle
     */
    Gaudi::DataHandle::Mode mode() const { return Gaudi::DataHandle::Reader; }

    /**
     * @brief create a vector of ReadHandles from the ReadHandleKeys
     * in the array
     */
    std::vector< ReadHandle<T> > makeHandles() const {
      std::vector< ReadHandle<T> > hndl;
      typename std::vector<ReadHandleKey<T>>::const_iterator itr;
      for (itr = this->begin(); itr != this->end(); ++itr) {
        hndl.push_back ( ReadHandle<T>( *itr) );
      }
      return ( std::move( hndl ) );
    }

  };


} // namespace SG

#endif
