/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STOREGATE_DECORHANDLEKEYARRAY_H
#define STOREGATE_DECORHANDLEKEYARRAY_H 1

#include "StoreGate/HandleKeyArray.h"

namespace SG {

  /**
   * @class SG::DecorHandleKeyArray<T>
   * @brief class to hold an array of DecorHandleKeys
   *
   * since it inherits from std::vector, all vector operations are
   * permitted.
   *
   * This class is based on HandleKeyArray but adds another Property Constructor, with
   *    a std::string that is the name of the base container. It is up to the user
   *    in the algorithm initialize to convert the HandleKeyArray to to have the
   *    needed handle keys.
   *
   * Note: makeHandles only works if the type is the same; otherwise have do do individually
   *
   */
  template <class T_Handle, class T_HandleKey, Gaudi::DataHandle::Mode MODE>
  class DecorHandleKeyArray : 
    public HandleKeyArray< T_Handle, T_HandleKey, MODE> {

  public:

    /**
     * @brief auto-declaring Property Constructor from a HandleKeyArray 
     * that takes an initializer list of std::strings, and associates the WHKA
     * with the specified Property name
     * @param name name of Property
     * @param std::strings that is the name of the base incput container
     * @param doc documentation string
     */
    template <class OWNER, 
              typename = typename std::enable_if<std::is_base_of<IProperty, OWNER>::value>::type>
    inline DecorHandleKeyArray( OWNER* owner,
                                std::string name,
                                std::string baseContName = "",
                                std::string doc="") :
      HandleKeyArray<T_Handle, T_HandleKey, MODE> {} {
      // For now just create one key with just the base name
      // This should be updated in the initialize phase
      if (baseContName != "") {
	this->push_back(T_HandleKey{baseContName});
      }
      auto p = owner->declareProperty(std::move(name), *this, std::move(doc));
      p->template setOwnerType<OWNER>();
    }
  };

} // namespace SG

#endif
