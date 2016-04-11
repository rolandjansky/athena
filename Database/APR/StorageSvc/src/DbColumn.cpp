/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbColumn.cpp 726071 2016-02-25 09:23:05Z krasznaa $
//====================================================================
//  Implementation file of a Database column
//--------------------------------------------------------------------
//
//  Package    : StorageSvc  (The POOL project)
//
//  @author      M.Frank
//====================================================================
// Framework header files
#include "StorageSvc/pool.h"
#include "StorageSvc/DbReflex.h"
#include "StorageSvc/DbColumn.h"
// C++ header files
#include <cstdio>
#include <cstring>

using namespace pool;
using namespace std;

/// Initializing constructor for composite columns
DbColumn::DbColumn( const string& nam,
                    const string& clazz,
                    int   object_type,
                    int   object_offset)
: m_typeName(clazz),
  m_colName(nam), 
  m_offset(object_offset),
  m_nElem(1),
  m_opts(0),
  m_size(0),
  m_type(object_type)
{
}

/// Initializing constructor for fixed size arrays
DbColumn::DbColumn( const string& nam, 
                    int object_type,
                    int object_offset,
                    int object_size,
                    int number_of_elements,
                    int column_opts)
: m_colName(nam), 
  m_offset(object_offset),
  m_nElem(number_of_elements),
  m_opts(column_opts),
  m_size(object_size),
  m_type(object_type)
{
  m_typeName = typeName(m_type);
}

/// Create persistable description
const string DbColumn::toString() const   {
  char txt[4096];
  ::sprintf(txt,"{NAME=%s}{CLASS=%s}{TYP=%d}{OPT=%d}{OFF=%d}{SIZ=%d}{CNT=%d}",
      m_colName.c_str(),
      m_typeName.c_str(),
      m_type,
      m_opts,
      m_offset,
      m_size,
      m_nElem);
  return txt;
}
static const char* itm[] = {
  "{NAME=", 
  "{CLASS=",
  "{TYP=",
  "{OPT=", 
  "{OFF=", 
  "{SIZ=",
  "{CNT="
};

DbStatus DbColumn::fromString( const string& string_rep)  {
  m_colName = typeName(UNKNOWN);
  string tmp = string_rep;
  char* p1 = (char*)tmp.c_str();
  int nread = 0;
  for(size_t i = 0; i < sizeof(itm)/sizeof(itm[0]); ++i)   {
    p1 = ::strstr(p1, itm[i]);
    char* pp1 = p1+strlen(itm[i]);
    if ( p1 )    {
      char* p2 = ::strstr(pp1, "}");
      if ( p2 )   {
        *p1 = 0;
        *p2 = 0;
        switch(i)   {
        case 0:
          m_colName = pp1;
          nread += 1;
          break;
        case 1:
          m_typeName = pp1;
          nread += 1;
          break;
        case 2:
          nread += ::sscanf(pp1, "%99d", &m_type);
          break;
        case 3:
          nread += ::sscanf(pp1, "%99d", &m_opts);
          break;
        case 4:
          nread += ::sscanf(pp1, "%99d", &m_offset);
          break;
        case 5:
          nread += ::sscanf(pp1, "%99d", &m_size);
          break;
        case 6:
          nread += ::sscanf(pp1, "%99d", &m_nElem);
          break;
        default:
          break;
        }
        *p1 = '{';
        *p2 = '}';
      }
    }
  }
  if ( nread == 6 )   { // Must match the number of elements in sscanf
    return Success;
  }
  return Error;
}

#define _NAME_(x) #x

// Access the type name by type_info
const string DbColumn::typeName(int typ)    {
  switch(typ)   {
  case UCHAR:
    return _NAME_(unsigned char);
  case USHORT:
    return _NAME_(unsigned short);
  case UINT:
    return _NAME_(unsigned int);
  case ULONG:
    return _NAME_(unsigned long);
  case CHAR:
    return _NAME_(char);
  case SHORT:
    return _NAME_(short);
  case INT:
    return _NAME_(int);
  case LONG:
    return _NAME_(long);
  case BOOL:
    return _NAME_(bool);
  case FLOAT:
    return _NAME_(float);
  case DOUBLE:
    return _NAME_(double);
  case LONGLONG:
    return _NAME_(long long);
  case ULONGLONG:
    return _NAME_(unsigned long long);
  case STRING:
  case LONG_STRING:
    return "string"; //_NAME_(string);
  case NTCHAR:
  case LONG_NTCHAR:
    return _NAME_(char*);
  case TOKEN:
    return "Token";
  case BLOB:
    return "Blob";
  case ANY:
    return "Any";
  case OBJECT_REF:
    return "OBJECT_REF";
  case CONTAINED_REF:
    return "CONTAINED_REF";
  case UNKNOWN:
  default:
    return "UNKNOWN";
  }
}
