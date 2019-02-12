// $Id: ToolHandle.cxx 804869 2017-05-15 20:14:34Z krumnack $

// Local include(s):
#include "AsgTools/ToolHandle.h"
#include "AsgTools/IAsgTool.h"

ToolHandleBase::ToolHandleBase( const std::string& typeAndName, INamedInterface* parent )
: m_typeAndName( typeAndName ), m_type(), m_name(), m_parent(parent) {

   setTypeAndName(typeAndName);
}

void ToolHandleBase::setTypeAndName(const std::string& typeAndName) {
   m_typeAndName = typeAndName;
   // Decode the received string:
   size_t pos = 0;
   if( ( pos = m_typeAndName.find( '/' ) ) == std::string::npos ) {
      // The user specified a single string, which is used as both the type
      // and instance name.
      m_type = m_typeAndName;
      m_name = m_typeAndName;
   } else {
      // The user specified both a type and an instance name.
      m_type = m_typeAndName.substr( 0, pos );
      m_name = m_typeAndName.substr( pos + 1 );
   }
}

void ToolHandleBase::setName(const std::string& name) {
   m_name = name;
}

const std::string& ToolHandleBase::typeAndName() const {

   return m_typeAndName;
}

const std::string& ToolHandleBase::type() const {

   return m_type;
}

const std::string& ToolHandleBase::name() const {

   return m_name;
}

const std::string& ToolHandleBase::parentName() const {

  if (m_parent)
    return m_parent->name();
  static const std::string toolSvcName = "ToolSvc";
  return toolSvcName;
}
