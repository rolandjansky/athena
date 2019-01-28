/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STOREGATE_HANDLEKEYARRAY_H
#define STOREGATE_HANDLEKEYARRAY_H 1

#include "StoreGate/VarHandleKeyArray.h"
#include "GaudiKernel/EventContext.h"

#include <vector>
#include <string>

namespace SG {

  /**
   * @class SG::HandleKeyArray<T>
   * @brief class to hold an array of HandleKeys
   *
   * since it inherits from std::vector, all vector operations are 
   * permitted.
   *
   * initialization can be done in three ways.
   * 1: with an std::vector<HandleKey> as a parameter
   *    SG::Cond<foo>::ReadHandleKeyArray  m_foo ( std::vector<ReadCondHandleKey> );
   *    SG::Cond<foo>::WriteHandleKeyArray m_foo ( std::vector<WriteCondHandleKey> );
   * 2: with an initializer list of HandleKeys
   *    SG::Cond<foo>::ReadHandleKeyArray  m_foo { ReadCondHandleKey<foo> k1, ReadCondHandleKey<foo> k2 };
   *    SG::Cond<foo>::WriteHandleKeyArray m_foo { WriteCondHandleKey<foo> k1, WriteCondHandleKey<foo> k2 };
   * 3: with an initializer list of std::strings, that will be used to
   *    internally create HandleKeys with those initializers
   *    SG::Cond<foo>::ReadHandleKeyArray  m_foo { "key1", "key2", "key3" };
   *    SG::Cond<foo>::WriteHandleKeyArray m_foo { "key1", "key2", "key3" };
   */

  template <class T_Handle, class T_HandleKey, Gaudi::DataHandle::Mode MODE>
  class HandleKeyArray : public VarHandleKeyArrayCommon< T_HandleKey > {
  public:
    /**
     * @brief default Constructor from a HandleKeyArray
     */
    HandleKeyArray(){}

    /**
     * @brief Constructor from a HandleKeyArray that takes a vector
     * of ReadHandleKeys
     * @param v vector of HandleKey
     */
    HandleKeyArray( const std::vector<T_HandleKey>& v ) :
      VarHandleKeyArrayCommon<T_HandleKey> ( v ) {}

    /**
     * @brief Constructor from a HandleKeyArray that takes an 
     * initializer list of HandleKeys
     * @param l initializer list of HandleKey
     */
    HandleKeyArray( std::initializer_list<T_HandleKey> l ):
      VarHandleKeyArrayCommon<T_HandleKey> {l} {}

    /**
     * @brief Constructor from a HandleKeyArray that takes an 
     * initializer list of std::strings.
     * @param l initializer list of std::strings used to create the
     *          HandleKeys
     */
    HandleKeyArray( std::initializer_list<std::string> key_names):
      VarHandleKeyArrayCommon<T_HandleKey> {key_names} {}

    /**
     * @brief auto-declaring Property Constructor from a HandleKeyArray 
     * that takes an initializer list of std::strings, and associates the WHKA
     * with the specified Property name
     * @param name name of Property
     * @param l initializer list of std::strings used to create the
     *          HandleKeys
     * @param doc documentation string
     */
    template <class OWNER, 
              typename = typename std::enable_if<std::is_base_of<IProperty, OWNER>::value>::type>
    inline HandleKeyArray( OWNER* owner,
                                std::string name,
                                std::initializer_list<std::string> l,
                                std::string doc="") :
      VarHandleKeyArrayCommon<T_HandleKey> {l} {
      auto p = owner->declareProperty(std::move(name), *this, std::move(doc));
      p->template setOwnerType<OWNER>();
    }


    /**
     * @brief return the type (Read/Write/Update) of handle
     */
    Gaudi::DataHandle::Mode mode() const { return MODE; }

    /**
     * @brief create a vector of Handles from the HandleKeys
     * in the array
     */
    std::vector< T_Handle > makeHandles() const {
      std::vector< T_Handle > hndl;
      typename std::vector<T_HandleKey>::const_iterator itr;
      for (itr = this->begin(); itr != this->end(); ++itr) {
        hndl.push_back ( T_Handle( *itr) );
      }
      return hndl;
    }

    /**
     * @brief create a vector of Handles from the HandleKeys
     * in the array, with explicit EventContext.
     */
    std::vector< T_Handle > makeHandles (const EventContext& ctx) const
    {
      std::vector< T_Handle > hndl;
      typename std::vector<T_HandleKey>::const_iterator itr;
      for (itr = this->begin(); itr != this->end(); ++itr) {
        hndl.push_back ( T_Handle( *itr, ctx) );
      }
      return hndl;
    }

  };

} // namespace SG

#endif
