/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLUTILITIES_ATHENAATTRIBUTESPECIFICATION_H
#define ATHENAPOOLUTILITIES_ATHENAATTRIBUTESPECIFICATION_H

#include <string>
#include <iostream>
#include <sstream>
#include <typeinfo>

#include "AthenaPoolUtilities/AthenaAttributeType.h"

/////////////////////////////////////////////////
//
// Class which acts like coral::AttributeSpecification 
//   BUT allows annotation like pool::CollectionDescription
//   BUT which is encased in name,value pair interface
//   which is not accessible through the constructor
//
////////////////////////////////////////////////

class AthenaAttributeSpecification
{
public:
  // 
  AthenaAttributeSpecification(const std::string& n, 
                               const std::string& t,
                               const std::string& g = "UNKNOWN",
                               const std::string& u = "UNKNOWN") :
    m_name(n),m_type(t, u, g)
    {}
  AthenaAttributeSpecification(const std::string& n, 
                               const AthenaAttributeType& t) :
    m_name(n),m_type(t)
    {}
  
  std::string name() const {return m_name;}
  std::string typeName() const {return m_type.typeName();}
  std::string unit() const {return m_type.unit();}
  std::string group() const {return m_type.group();}
  AthenaAttributeType attributeType() const {return m_type;}
  
  /// Equal operator
  bool operator==( const AthenaAttributeSpecification& rhs ) const;

  /// Comparison operator
  bool operator!=( const AthenaAttributeSpecification& rhs ) const;

private:
  std::string m_name;
  AthenaAttributeType m_type;
};


inline bool
AthenaAttributeSpecification::operator==( const AthenaAttributeSpecification& rhs ) const
{
  return ( this->m_name == rhs.m_name &&
           this->m_type == rhs.m_type );
}
  
inline bool
AthenaAttributeSpecification::operator!=( const AthenaAttributeSpecification& rhs ) const
{
  return ( this->m_name != rhs.m_name ||
           this->m_type != rhs.m_type );
}

#endif
