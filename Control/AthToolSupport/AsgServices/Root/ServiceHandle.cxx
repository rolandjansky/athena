/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack
/// @author David Adams <dladams@bnl.gov> (for original implementation for tools)
/// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch> (for original implementation for tools)



// Local include(s):
#include "AsgServices/ServiceHandle.h"
#include "AsgServices/IAsgService.h"

ServiceHandleBase ::
ServiceHandleBase( const std::string& typeAndName,
                   const std::string& parentName )
  : m_typeAndName( typeAndName ), m_type(), m_name(), m_parentName(parentName)
{
   setTypeAndName(typeAndName);
}



void ServiceHandleBase::setTypeAndName(const std::string& typeAndName)
{
  m_typeAndName = typeAndName;
  // Decode the received string:
  size_t pos = 0;
  if( ( pos = m_typeAndName.find( '/' ) ) == std::string::npos )
  {
    // The user specified a single string, which is used as both the type
    // and instance name.
    m_type = m_typeAndName;
    m_name = m_typeAndName;
  } else
  {
    // The user specified both a type and an instance name.
    m_type = m_typeAndName.substr( 0, pos );
    m_name = m_typeAndName.substr( pos + 1 );
  }
}



void ServiceHandleBase::setName(const std::string& name)
{
  m_name = name;
}



const std::string& ServiceHandleBase::typeAndName() const
{
  return m_typeAndName;
}



const std::string& ServiceHandleBase::type() const
{
  return m_type;
}



const std::string& ServiceHandleBase::name() const
{
  return m_name;
}



const std::string& ServiceHandleBase::parentName() const
{
  return m_parentName;
}
