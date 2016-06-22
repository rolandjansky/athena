/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STOREGATE_VARHANDLEKEYARRAYPROPERTY
#define STOREGATE_VARHANDLEKEYARRAYPROPERTY 1

/**
 * @file StoreGate/VarHandleKeyArrayProperty.h
 * @author C. Leggett
 * @date Updated: Jun 17, 2016
 * @brief class to handle Properties for  VarHandleKeyArray
 */

#include "StoreGate/VarHandleKeyArray.h"
#include "StoreGate/ReadHandleKeyArray.h"
#include "StoreGate/WriteHandleKeyArray.h"
#include "GaudiKernel/Property.h"
#include <iostream>

namespace Gaudi { 
  namespace Parsers {
    GAUDI_API
    StatusCode parse(SG::VarHandleKeyArray& v, const std::string& s);    
  }

  namespace Utils {
    GAUDI_API
    std::ostream& toStream(const SG::VarHandleKeyArray& v, std::ostream& o);    
  }
}

namespace SG {

  class GAUDI_API VarHandleKeyArrayProperty
    : public ::Property 
  {
  public:
 
    VarHandleKeyArrayProperty( const std::string& name, 
                               SG::VarHandleKeyArray& ref );
 
    VarHandleKeyArrayProperty& operator=( const SG::VarHandleKeyArray& value );
 
    virtual VarHandleKeyArrayProperty* clone() const override;
 
    virtual bool load( Property& destination ) const override;
 
    virtual bool assign( const Property& source ) override;
 
    virtual std::string toString() const override;
 
    virtual void toStream(std::ostream& out) const override;
 
    virtual StatusCode fromString(const std::string& s) override;
 
    const SG::VarHandleKeyArray& value() const;
 
    bool setValue( const SG::VarHandleKeyArray& value );
 
 
  private:
    /** Pointer to the real property. Reference would be better, 
     *  but Reflex does not support references yet
     */
    SG::VarHandleKeyArray* m_pValue;
  };


} // namespace SG

template<>
class SimplePropertyRef< SG::VarHandleKeyArray > :
  public SG::VarHandleKeyArrayProperty
{
public:
  SimplePropertyRef(const std::string& name, SG::VarHandleKeyArray& value) :
    SG::VarHandleKeyArrayProperty(name, value) {}
};


template<typename T>
class SimplePropertyRef< SG::ReadHandleKeyArray<T> > :
  public SG::VarHandleKeyArrayProperty
{
public:
  SimplePropertyRef(const std::string& name, SG::ReadHandleKeyArray<T>& value) :
    SG::VarHandleKeyArrayProperty(name, value) {}
};

template<typename T>
class SimplePropertyRef< SG::WriteHandleKeyArray<T> > :
  public SG::VarHandleKeyArrayProperty
{
public:
  SimplePropertyRef( const std::string& name, 
                     SG::WriteHandleKeyArray<T>& value ) :
    SG::VarHandleKeyArrayProperty(name, value) {}
};




#endif
