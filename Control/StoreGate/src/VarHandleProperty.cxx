///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// VarHandleProperty.cxx 
// Implementation file for class VarHandleProperty
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// stl includes
#include <sstream>
#include <map>
#include <boost/tokenizer.hpp>

// StoreGate includes
#include "StoreGate/VarHandleProperty.h"

namespace Gaudi { 
namespace Parsers {

StatusCode 
parse(SG::VarHandleBase& v, const std::string& s)
{
  // default values
  StatusCode sc(StatusCode::FAILURE);
  std::string prop;
  sc = Gaudi::Parsers::parse(prop, s);
  if (sc.isSuccess()) {
    //split the /-separated string in 1 or 2 words: 
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> sep("/");
    tokenizer tokens(prop, sep);
    int nToks(distance(tokens.begin(), tokens.end()));    
    if (nToks == 1) {
      v.m_store = "StoreGateSvc";
      v.m_sgkey = *(tokens.begin());
    } else if (nToks == 2) {
      auto tok = tokens.begin();
      v.m_store = *tok;
      v.m_sgkey = *(++tok);
    } else {
      sc = StatusCode::FAILURE;
    }
  }
  return sc;
}

} //> ns Parsers

namespace Utils {

std::ostream& 
toStream(const SG::VarHandleBase& v, std::ostream& o)
{
  o << "'" << v.name() << "'";
  return o;
}
    
} //> ns Utils

} //> ns Gaudi


VarHandleProperty::VarHandleProperty( const std::string& name, 
                                      SG::VarHandleBase& ref )
  : Property( name, typeid( SG::VarHandleBase ) ), 
    m_pValue( &ref ) 
{}

VarHandleProperty::~VarHandleProperty()
{}

StatusCode 
VarHandleProperty::fromString(const std::string& s)
{
  if (!Gaudi::Parsers::parse(*m_pValue, s).isSuccess()) {
    return StatusCode::FAILURE;
  }
  return useUpdateHandler()
    ? StatusCode::SUCCESS
    : StatusCode::FAILURE;
}

bool
VarHandleProperty::setValue( const SG::VarHandleBase& value ) 
{
  m_pValue->operator=(value);
  return useUpdateHandler();
}

std::string
VarHandleProperty::toString( ) const 
{
  useReadHandler();
  std::ostringstream o;
  Gaudi::Utils::toStream(*m_pValue, o);
  return o.str();
}

void
VarHandleProperty::toStream(std::ostream& out) const
{
  useReadHandler();
  out << this->toString();
}

