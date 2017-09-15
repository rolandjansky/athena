/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STOREGATE_WRITEHANDLEKEYARRAY_H
#define STOREGATE_WRITEHANDLEKEYARRAY_H 1

#include "StoreGate/VarHandleKeyArray.h"

#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/WriteHandle.h"

#include <vector>
#include <string>

namespace SG {

  /**
   * @class SG::WriteHandleKeyArray<T>
   * @brief class to hold an array of WriteHandleKeys
   *
   * since it inherits from std::vector, all vector operations are 
   * permitted.
   *
   * initialization can be done in three ways.
   * 1: with an std::vector<WriteHandleKey> as a parameter
   *    SG::WriteHandleKeyArray<foo> m_foo ( std::vector<WriteHandleKey> );
   * 2: with an initializer list of WriteHandleKeys
   *    SG::WriteHandleKeyArray<foo> m_foo { WriteHandleKey<foo> k1, WriteHandleKey<foo> k2 };
   * 3: with an initializer list of std::strings, that will be used to
   *    internally create WriteHandleKeys with those initializers
   *    SG::WriteHandleKeyArray<foo> m_foo { "key1", "key2", "key3" };
   */

  template <class T>
  class WriteHandleKeyArray : public VarHandleKeyArrayCommon< WriteHandleKey<T> > {
  public:
    /**
     * @brief default Constructor from a WriteHandleKeyArray
     */
    WriteHandleKeyArray(){};

    /**
     * @brief Constructor from a WriteHandleKeyArray that takes a vector
     * of ReadHandleKeys
     * @param v vector of WriteHandleKey
     */
    WriteHandleKeyArray( const std::vector<WriteHandleKey<T>>& v ) :
      VarHandleKeyArrayCommon<WriteHandleKey<T>> ( v ) {};

    /**
     * @brief Constructor from a WriteHandleKeyArray that takes an 
     * initializer list of WriteHandleKeys
     * @param l initializer list of WriteHandleKey
     */
    WriteHandleKeyArray( std::initializer_list<WriteHandleKey<T>> l ):
      VarHandleKeyArrayCommon<WriteHandleKey<T>> {l} {};

    /**
     * @brief Constructor from a WriteHandleKeyArray that takes an 
     * initializer list of std::strings.
     * @param l initializer list of std::strings used to create the
     *          WriteHandleKeys
     */
    WriteHandleKeyArray( std::initializer_list<std::string> l ):
      VarHandleKeyArrayCommon<WriteHandleKey<T>> {l} {};

    /**
     * @brief auto-declaring Property Constructor from a WriteHandleKeyArray 
     * that takes an initializer list of std::strings, and associates the WHKA
     * with the specified Property name
     * @param name name of Property
     * @param l initializer list of std::strings used to create the
     *          WriteHandleKeys
     * @param doc documentation string
     */
    template <class OWNER, 
              typename = typename std::enable_if<std::is_base_of<IProperty, OWNER>::value>::type>
    inline WriteHandleKeyArray( OWNER* owner,
                                std::string name,
                                std::initializer_list<std::string> l,
                                std::string doc="") :
      VarHandleKeyArrayCommon<WriteHandleKey<T>> {l} {
      auto p = owner->declareProperty(std::move(name), *this, std::move(doc));
      p->template setOwnerType<OWNER>();
    }


    /**
     * @brief return the type (Read/Write/Update) of handle
     */
    Gaudi::DataHandle::Mode mode() const { return Gaudi::DataHandle::Writer; }

    /**
     * @brief create a vector of WriteHandles from the WriteHandleKeys
     * in the array
     */
    std::vector< WriteHandle<T> > makeHandles() const {
      std::vector< WriteHandle<T> > hndl;
      typename std::vector<WriteHandleKey<T>>::const_iterator itr;
      for (itr = this->begin(); itr != this->end(); ++itr) {
        hndl.push_back ( WriteHandle<T>( *itr) );
      }
      return ( std::move( hndl ) );
    }

    /**
     * @brief create a vector of WriteHandles from the WriteHandleKeys
     * in the array, with explicit EventContext.
     */
    std::vector< WriteHandle<T> > makeHandles (const EventContext& ctx) const
    {
      std::vector< WriteHandle<T> > hndl;
      typename std::vector<WriteHandleKey<T>>::const_iterator itr;
      for (itr = this->begin(); itr != this->end(); ++itr) {
        hndl.push_back ( WriteHandle<T>( *itr, ctx) );
      }
      return ( std::move( hndl ) );
    }

  };


} // namespace SG

#endif
