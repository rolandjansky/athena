/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STOREGATE_VARHANDLEKEYARRAY_H
#define STOREGATE_VARHANDLEKEYARRAY_H 1

/**
 * @file StoreGate/VarHandleKeyArray.h
 * @author C. Leggett
 * @date Updated: Jun 17, 2016
 * @brief Base class for VarHandleKeyArray for reading from StoreGate.
 */

#include "GaudiKernel/DataHandle.h"
#include "GaudiKernel/IDataHandleHolder.h"
#include "StoreGate/VarHandleKey.h"

#include <vector>
#include <string>

namespace SG {

  /**
   * @class SG::VarHandleKeyArray<T>
   * @brief untemplated base class for VarHandleKeyArrays
   */
  class VarHandleKeyArray {
  public:
    VarHandleKeyArray(){};
    virtual ~VarHandleKeyArray() = default;
    virtual StatusCode assign(const std::vector<std::string>& vs)=0;
    virtual std::string toString() const = 0;
    virtual Gaudi::DataHandle::Mode mode() const = 0;

    virtual std::vector<SG::VarHandleKey*> keys() const = 0;

    virtual void renounce() = 0;
    virtual bool renounced() const = 0;
    virtual void declare(IDataHandleHolder*)  = 0;

    virtual void setOwner( IDataHandleHolder* o ) = 0;
    virtual const IDataHandleHolder* owner() const = 0;
    virtual IDataHandleHolder* owner() = 0;

  };

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// mixin class for common functionality
//

  /**
   * @class SG::VarHandleKeyArrayCommon<T>
   * @brief mixin base class for VarHandleKeyArrays, inheriting from
   * std::vector as well as VarHandleKeyArray to provide vector-like
   * access
   *
   */

  template <class Base>
  class VarHandleKeyArrayCommon : public VarHandleKeyArray, 
                                  public std::vector<Base> {
  public:
    /**
     * @brief default base Constructor of mixin
     *
     */
    VarHandleKeyArrayCommon() : std::vector<Base>() {};
    
    /**
     * @brief base Constructor from a VarHandleKeyArray that takes a vector
     * @param v vector of Read/Write/UpdateHandleKey
     */
    VarHandleKeyArrayCommon( const std::vector<Base>& v ):
      std::vector<Base>(v) {};
    
    /**
     * @brief base Constructor from a VarHandleKeyArray that takes an 
     * initializer list of VarHandleKeys
     * @param l initializer list of Read/Write/UpdateHandleKey
     */
    VarHandleKeyArrayCommon( std::initializer_list<Base> l ):
      std::vector<Base>{l} {};
    
    /**
     * @brief base Constructor from a VarHandleKeyArray that takes an 
     * initializer list of std::strings.
     * @param l initializer list of std::strings used to create the
     *          VarHandleKeys
     */
    VarHandleKeyArrayCommon( std::initializer_list<std::string> l ) {
      for (auto &e : l) {
        this->push_back( Base{e} );
      }
    }    
    
    /**
     * @brief forward the initialization to the member VarHandleKeys
     * @param used If false, then this handle is not to be used.
     *             Instead of normal initialization, the key will be cleared.
     */
    StatusCode initialize (bool used = true);

    /**
     * @brief Set the contents of the VarHandleKeyArray from a 
     * vector of std::strings
     * @param vs vector of initializer strings
     */
    virtual StatusCode assign(const std::vector<std::string>& vs) override;

    /**
     * @brief string representation of the VarHandleKeyArray
     */
    virtual std::string toString() const override;

    /**
     * @brief create array of all base VarHandleKeys in the Array
     */
    virtual std::vector<SG::VarHandleKey*> keys() const override;
    

    /**
     * @brief if called, handles will not be declared in the algorithm I/O
     */
    virtual void renounce() override { m_isRenounced = true; }
    
    /**
     * @brief query renounced state
     **/ 
    virtual bool renounced() const override { return m_isRenounced; }

    virtual void declare( IDataHandleHolder* ) override;

    virtual void setOwner( IDataHandleHolder* o ) override { m_owner = o; }
    virtual const IDataHandleHolder* owner() const override { return m_owner; }
    virtual IDataHandleHolder* owner() override { return m_owner; }

  private:
    
    bool m_isRenounced{ false };
    IDataHandleHolder* m_owner{ nullptr };

  };
  
} // namespace SG

namespace std {
  ostream& operator<<(ostream& s, const SG::VarHandleKeyArray& m);
}


#include "StoreGate/VarHandleKeyArray.icc"

#endif
