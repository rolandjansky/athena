// Property.cxx

#include "AsgTools/Property.h"
#include <iostream>

using std::string;
using std::cout;
using std::endl;
typedef Property::Type Type;
using asg::ToolHandle;
using asg::ToolHandleArray;

//**********************************************************************

string Property::name(Type a_type) {
  switch(a_type) {
  case UNKNOWNTYPE:     return "unknown";
  case BOOL:            return "bool";
  case INT:             return "int";
  case FLOAT:           return "float";
  case DOUBLE:          return "double";
  case STRING:          return "string";
  case INTVECTOR:       return "vector<int>";
  case FLOATVECTOR:     return "vector<float>";
  case STRINGVECTOR:    return "vector<string>";
  case TOOLHANDLE:      return "ToolHandle";
  case TOOLHANDLEARRAY: return "ToolHandleArray";
  }
  return "error";
}

//**********************************************************************

Property::Property() : m_type(UNKNOWNTYPE) { }

//**********************************************************************

Property::Property(Type a_type) : m_type(a_type) { }

//**********************************************************************

Property::~Property() { }

//**********************************************************************

Type Property::type() const { return m_type; }

//**********************************************************************

bool Property::isValid() const { return m_type!=UNKNOWNTYPE; }

//**********************************************************************

string Property::typeName() const { return name(m_type); }

//**********************************************************************

int Property::setFrom(const Property&) {
  cout << "Property::setFrom: Property not set!" << endl;
  return -1;
}

//**********************************************************************
