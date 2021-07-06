/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbOption.cpp 726071 2016-02-25 09:23:05Z krasznaa $
//====================================================================
//  Implementation file of a Database options
//--------------------------------------------------------------------
//
//  Package    : StorageSvc (The POOL project)
//
//  @author      M.Frank
//====================================================================

// Framework include files
#include "StorageSvc/pool.h"
#include "StorageSvc/DbOption.h"

// C++ include files
#include <iostream>

using namespace pool;

namespace {
  template <class T, class Q> struct Marshal {
    static DbStatus get(const void* from, void* to)   {
      *(Q*)to = *(T*)from;  
     return Success;
   }
  };
  template <>
    inline DbStatus Marshal<double, float>::get(const void* from, void* to)  {
    *(float*)to = float(*(double*)from);  
    return Success;
  }
}

/// Initializing constructor with type definition
DbOption::DbOption(const std::string& nam, const std::string& opt)
: m_type(DbColumn::UNKNOWN), m_name(nam), m_opt(opt)
{
}

/// Access to OS independent type name
std::string DbOption::typeName() const  {
  return DbColumn::typeName(m_type);
}

/// Set the option value
DbStatus DbOption::i_setValue(const std::type_info& typ, const void* value)  {
  if ( typ == typeid(bool) )
    return setValue(DbColumn::BOOL, value);
  else if ( typ == typeid(char) )
    return setValue(DbColumn::CHAR, value);
  else if ( typ == typeid(signed char) )
    return setValue(DbColumn::CHAR, value);
  else if ( typ == typeid(unsigned char) )
    return setValue(DbColumn::UCHAR, value);
  else if ( typ == typeid(short) )
    return setValue(DbColumn::SHORT, value);
  else if ( typ == typeid(signed short) )
    return setValue(DbColumn::SHORT, value);
  else if ( typ == typeid(unsigned short) )
    return setValue(DbColumn::USHORT, value);
  else if ( typ == typeid(int) )
    return setValue(DbColumn::INT, value);
  else if ( typ == typeid(signed int) )
    return setValue(DbColumn::INT, value);
  else if ( typ == typeid(unsigned int) )
    return setValue(DbColumn::UINT, value);
  else if ( typ == typeid(long) )
    return setValue(DbColumn::LONG, value);
  else if ( typ == typeid(signed long) )
    return setValue(DbColumn::LONG, value);
  else if ( typ == typeid(unsigned long) )
    return setValue(DbColumn::ULONG, value);
  else if ( typ == typeid(long long int) )
    return setValue(DbColumn::LONGLONG, value);
  else if ( typ == typeid(unsigned long long int) )
    return setValue(DbColumn::ULONGLONG, value);
  else if ( typ == typeid(double) )
    return setValue(DbColumn::DOUBLE, value);
  else if ( typ == typeid(float) )
    return setValue(DbColumn::FLOAT, value);
  else if ( typ == typeid(void*) )
    return setValue(DbColumn::ANY, value);
  else if ( typ == typeid(const void*) )
    return setValue(DbColumn::ANY, value);
  else if ( typ == typeid(char*) )
    return setValue(DbColumn::NTCHAR, value);
  else if ( typ == typeid(const char*) )
    return setValue(DbColumn::NTCHAR, value);
  else
    std::cout << "DbOption::getValue> unknown data type" << std::endl;
  return Error;
}

/// Set the option value
DbStatus DbOption::i_getValue(const std::type_info& typ, void* value) const {
  //const char* n = typ.name();
  if ( typ == typeid(bool) )
    return getValue(DbColumn::BOOL, value);
  else if ( typ == typeid(char) )
    return getValue(DbColumn::CHAR, value);
  else if ( typ == typeid(signed char) )
    return getValue(DbColumn::CHAR, value);
  else if ( typ == typeid(unsigned char) )
    return getValue(DbColumn::UCHAR, value);
  else if ( typ == typeid(short) )
    return getValue(DbColumn::SHORT, value);
  else if ( typ == typeid(signed short) )
    return getValue(DbColumn::SHORT, value);
  else if ( typ == typeid(unsigned short) )
    return getValue(DbColumn::USHORT, value);
  else if ( typ == typeid(int) )
    return getValue(DbColumn::INT, value);
  else if ( typ == typeid(signed int) )
    return getValue(DbColumn::INT, value);
  else if ( typ == typeid(unsigned int) )
    return getValue(DbColumn::UINT, value);
  else if ( typ == typeid(long) )
    return getValue(DbColumn::LONG, value);
  else if ( typ == typeid(signed long) )
    return getValue(DbColumn::LONG, value);
  else if ( typ == typeid(unsigned long) )
    return getValue(DbColumn::ULONG, value);
  else if ( typ == typeid(long long int) )
    return getValue(DbColumn::LONGLONG, value);
  else if ( typ == typeid(unsigned long long int) )
    return getValue(DbColumn::ULONGLONG, value);
  else if ( typ == typeid(double) )
    return getValue(DbColumn::DOUBLE, value);
  else if ( typ == typeid(float) )
    return getValue(DbColumn::FLOAT, value);
  else if ( typ == typeid(void*) )
    return getValue(DbColumn::ANY, value);
  else if ( typ == typeid(const void*) )
    return getValue(DbColumn::ANY, value);
  else if ( typ == typeid(char*) )
    return getValue(DbColumn::NTCHAR, value);
  else if ( typ == typeid(const char*) )
    return getValue(DbColumn::NTCHAR, value);
  else
    std::cout << "DbOption::getValue> unknown data type" << std::endl;
  return Error;
}

/// Set the option value
DbStatus DbOption::setValue(DbColumn::Type typ, const void* value) {
  m_type = typ;
  switch(type())   {
  case DbColumn::UCHAR:
    return Marshal<unsigned char,int>::get(value,&m_value.val_int);
  case DbColumn::USHORT:
    return Marshal<unsigned short,int>::get(value,&m_value.val_int);
  case DbColumn::UINT:
    return Marshal<unsigned int,int>::get(value,&m_value.val_int);
  case DbColumn::ULONG:
    return Marshal<unsigned long,int>::get(value,&m_value.val_int);
  case DbColumn::ULONGLONG:
    return Marshal<long long int,unsigned long long int>::get(value,&m_value.val_long);
  case DbColumn::CHAR:
    return Marshal<char,int>::get(value,&m_value.val_int);
  case DbColumn::SHORT:
    return Marshal<short,int>::get(value,&m_value.val_int);
  case DbColumn::INT:
    return Marshal<int,int>::get(value,&m_value.val_int);
  case DbColumn::LONG:
    return Marshal<long,int>::get(value,&m_value.val_int);
  case DbColumn::LONGLONG:
    return Marshal<long long int,long long int>::get(value,&m_value.val_long);
  case DbColumn::BOOL:
    m_value.val_int = *(bool*)value ? 1 : 0;
    return Success;
  case DbColumn::FLOAT:
    return Marshal<float,double>::get(value,&m_value.val_double);
  case DbColumn::DOUBLE:
    return Marshal<double,double>::get(value,&m_value.val_double);
  case DbColumn::NTCHAR:
    m_value.val_pchar = *(char**)value;
    return Success;
  case DbColumn::POINTER:
  case DbColumn::ANY:
    m_value.val_pvoid = *(void**)value;
    return Success;
  case DbColumn::UNKNOWN:
  default:
    std::cout << "DbOption::setValue> unknown data type" << std::endl;
  }
  return Error;
}

/// Set the option value
DbStatus DbOption::getValue(DbColumn::Type /*typ*/, void* value) const  {
  switch(type())   {
  case DbColumn::UCHAR:
    return Marshal<int, unsigned char>::get(&m_value.val_int, value);
  case DbColumn::USHORT:
    return Marshal<int, unsigned short>::get(&m_value.val_int, value);
  case DbColumn::UINT:
    return Marshal<int, unsigned int>::get(&m_value.val_int, value);
  case DbColumn::ULONG:
    return Marshal<int, unsigned long>::get(&m_value.val_int, value);
  case DbColumn::ULONGLONG:
    return Marshal<long long int, unsigned long long int>::get(&m_value.val_int, value);
  case DbColumn::CHAR:
    return Marshal<int, char>::get(&m_value.val_int, value);
  case DbColumn::SHORT:
    return Marshal<int, short>::get(&m_value.val_int, value);
  case DbColumn::INT:
    return Marshal<int, int>::get(&m_value.val_int, value);
  case DbColumn::LONG:
    return Marshal<int, long>::get(&m_value.val_int, value);
  case DbColumn::LONGLONG:
    return Marshal<long long int, long long int>::get(&m_value.val_int, value);
  case DbColumn::BOOL:
    *(bool*)value = m_value.val_int == 1;
    return Success;
  case DbColumn::FLOAT:
    return Marshal<double, float>::get(&m_value.val_int, value);
  case DbColumn::DOUBLE:
    return Marshal<double, double>::get(&m_value.val_int, value);
  case DbColumn::NTCHAR:
  case DbColumn::POINTER:
  case DbColumn::ANY:
    *(void**)value = m_value.val_pvoid;
    return Success;
  case DbColumn::UNKNOWN:
  default:
    std::cout << "DbOption::getValue> unknown data type" << std::endl;
  }
  return Error;
}
