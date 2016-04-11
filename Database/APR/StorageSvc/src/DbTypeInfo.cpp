/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// $Id: DbTypeInfo.cpp 726071 2016-02-25 09:23:05Z krasznaa $
//====================================================================
//  DbTypeInfo implementation
//--------------------------------------------------------------------
//
//  Package    : StorageSvc (The POOL project)
//
//  Description: Type information for persistent objects
//
//  @author      M.Frank
//====================================================================

// Framework include files
#include "StorageSvc/DbToken.h"
#include "StorageSvc/DbArray.h"
#include "StorageSvc/DbColumn.h"
#include "StorageSvc/DbReflex.h"
#include "StorageSvc/DbTypeInfo.h"
#include "StorageSvc/DbTransform.h"
#include "StorageSvc/DbInstanceCount.h"
#include "StorageSvc/DbString.h"

// STL include files
#include <stdexcept>
#include <cstring>
#include <cstdio>
#include <vector>

#include "cxxabi.h"


static const std::string __typeinfoName( const std::type_info& tinfo) {
  const char* class_name = tinfo.name();
  std::string result;
  if ( ::strlen(class_name) == 1 ) {
    // See http://www.realitydiluted.com/mirrors/reality.sgi.com/dehnert_engr/cxx/abi.pdf
    // for details
    switch(class_name[0]) {
    case 'v':
      result = "void";
      break;
    case 'w':
      result = "wchar_t";
      break;
    case 'b':
      result = "bool";
      break;
    case 'c':
      result = "char";
      break;
    case 'h':
      result = "unsigned char";
      break;
    case 's':
      result = "short";
      break;
    case 't':
      result = "unsigned short";
      break;
    case 'i':
      result = "int";
      break;
    case 'j':
      result = "unsigned int";
      break;
    case 'l':
      result = "long";
      break;
    case 'm':
      result = "unsigned long";
      break;
    case 'x':
      result = "long long";
      break;
    case 'y':
      result = "unsigned long long";
      break;
    case 'n':
      result = "__int128";
      break;
    case 'o':
      result = "unsigned __int128";
      break;
    case 'f':
      result = "float";
      break;
    case 'd':
      result = "double";
      break;
    case 'e':
      result = "long double";
      break;
    case 'g':
      result = "__float128";
      break;
    case 'z':
      result = "ellipsis";
      break;
    }
  }
  else  {
    char buff[16*1024];
    size_t len = sizeof(buff);
    int    status = 0;
    result = __cxxabiv1::__cxa_demangle(class_name, buff, &len, &status);
  }
  return result;
}

namespace pool  {

   const std::string typeName(const std::type_info& typ) {
    return __typeinfoName(typ);
  }
   
  void clearColumns(DbTypeInfo::Columns& cols)  {
    if ( cols.empty() ) return;
    for(DbTypeInfo::Columns::iterator i=cols.begin(); i != cols.end(); ++i)  {
      if ( 0 != (*i) ) delete (*i);
    }
  }
   
}

using namespace pool;
using namespace std;

/// Standard Constructor
DbTypeInfo::DbTypeInfo(const Guid& guid, TypeH type, Columns& cols)
      : Shape(),
        m_refCount( 0 ),
        m_columns(),
        m_mult( 0 ),
        m_class( type )
{
   DbInstanceCount::increment(this);
   setShapeID(guid);
   if( cols.size() == 0 )   {
      std::string full = DbReflex::fullTypeName(type);
      std::string nam = (full.substr(0,2)=="::") ? full.substr(2) : full;
      int col_type = DbColumn::POINTER;
      if(clazz()) {
        if (clazz().TypeInfo() == typeid(unsigned char)) col_type = DbColumn::UCHAR;
        else if (clazz().TypeInfo() == typeid(unsigned short)) col_type = DbColumn::USHORT;
        else if (clazz().TypeInfo() == typeid(unsigned int)) col_type = DbColumn::UINT;
        else if (clazz().TypeInfo() == typeid(unsigned long)) col_type = DbColumn::ULONG;
        else if (clazz().TypeInfo() == typeid(char)) col_type = DbColumn::CHAR;
        else if (clazz().TypeInfo() == typeid(short)) col_type = DbColumn::SHORT;
        else if (clazz().TypeInfo() == typeid(int)) col_type = DbColumn::INT;
        else if (clazz().TypeInfo() == typeid(long)) col_type = DbColumn::LONG;
        else if (clazz().TypeInfo() == typeid(bool)) col_type = DbColumn::BOOL;
        else if (clazz().TypeInfo() == typeid(float)) col_type = DbColumn::FLOAT;
        else if (clazz().TypeInfo() == typeid(double)) col_type = DbColumn::DOUBLE;
        else if (clazz().TypeInfo() == typeid(char*)) col_type = DbColumn::NTCHAR;
        else if (clazz().TypeInfo() == typeid(unsigned long long)) col_type = DbColumn::ULONGLONG;
        else if (clazz().TypeInfo() == typeid(long long)) col_type = DbColumn::LONGLONG;
        else if (clazz().TypeInfo() == typeid(Token)) col_type = DbColumn::TOKEN;
      }
      m_columns.push_back(new DbColumn(type.Name(), nam, col_type, 0));
      // cout << " -- DbTypeInfo: created column: " << type.Name() << ",  " << nam << endl;
   }
   else  {
      m_columns = cols;
      cols.clear();
   }
}


DbTypeInfo::DbTypeInfo(const Guid& guid)
: Shape(),
  m_refCount( 0 ),
  m_columns(),
  m_mult( 0 ),
  m_class( )
{
  DbInstanceCount::increment(this);
  setShapeID(guid);
}

// Destructor
DbTypeInfo::~DbTypeInfo()    {
  DbInstanceCount::decrement(this);
  for(size_t i = 0; i < m_columns.size(); ++i )   {
    delete m_columns[i];
  }
  m_columns.clear();
}

/// Access to reflection class (if availible)
TypeH DbTypeInfo::clazz( bool noIdScan )  const  {
  if ( m_class || noIdScan )  {
    return m_class;
  }
  else if ( !m_class && m_columns.size() > 0 )  {
    static bool convert = false;
    if ( !convert )  {
      try   {
        DbTypeInfo* thisPtr = (DbTypeInfo*)this;
        convert = true;
        thisPtr->m_class = DbReflex::forGuid(m_id);
      }
      catch(...)  {
      }
    }
    return m_class;
  }
  return m_class;
}

/// Allow usage of base classes
DbStatus DbTypeInfo::declareBase(const DbTypeInfo* pInfo) {
  if ( pInfo )    {
    for(size_t i = 0; i < pInfo->m_columns.size(); ++i )   {
      m_columns.push_back(new DbColumn(*(pInfo->m_columns[i])));
    }
    return Success;
  }
  return Error;
}

/// Create persistable description
const std::string DbTypeInfo::toString() const   {
  std::string rep;
  char txt[256];
  rep = "{ID=";
  rep += shapeID().toString();
  rep += "}";
  if ( !m_class )
    rep += "{CL=<no_class>}";
  else
    rep += "{CL=" + DbReflex::fullTypeName(m_class) + "}";
  sprintf(txt,"{NCOL=%ld}{CNT=%d}", long(m_columns.size()), m_mult);
  rep += txt;
  for(Columns::const_iterator i=m_columns.begin(); i<m_columns.end();++i) {
    rep += "{COL={";
    rep += (*i)->toString();
    rep += "}}";
  }
  return rep;
}

static const char* itm[][2] = { 
  {"{ID=", "}"}, 
  {"{CL=", "}"}, 
  {"{NCOL=", "}"},
  {"{COL={", "}}"} 
};

DbStatus DbTypeInfo::i_fromString( const std::string& string_rep)  {
  size_t i;
  int ncol=-1;
  std::string tmp = string_rep;
  std::string cl_name;
  m_mult = 0;
  for(i = 0; i < m_columns.size(); ++i)
    delete m_columns[i];
  m_columns.clear();
  char* p1 = (char*)tmp.c_str();
  DbColumn* col = 0;
  setShapeID(Guid::null());
  for(i = 0; i < sizeof(itm)/sizeof(itm[0]); ++i)   {
Again:
    p1 = ::strstr(p1, itm[i][0]);
    if ( p1 )    {
      char* pp1 = p1+strlen(itm[i][0]);
      char* p2 = ::strstr(pp1, itm[i][1]);
      if ( p2 )   {
        p2 += ::strlen(itm[i][1])-1;
        *p1 = 0;
        *p2 = 0;
        switch(i) {
        case 0:
          setShapeID(std::string(pp1));
          // this->clazz();
          break;
        case 1:
          m_class = DbReflex::forTypeName(pp1);
          if ( m_class && shapeID() == Guid::null() )  {
            setShapeID(DbReflex::guid(m_class));
          }
          break;
        case 2:
          ::sscanf(pp1, "%99d", &ncol);
          break;
        case 3:
          m_columns.push_back(col=new DbColumn());
          col->fromString(pp1);
          break;
        default:
          break;
        }
        *p1 = '{';
        *p2 = '}';
        if ( i > 2 && int(m_columns.size()) < ncol )  {
          p1++;
          goto Again;
        }
      }
    }
  }  
  return int(m_columns.size()) == ncol ? Success : Error;
}

/// Access type name by type identifier from RTTI
const std::string DbTypeInfo::typeName(const std::type_info& typ) {
  return pool::typeName(typ);
}


/* Create the type information object from a string representation
   Store it in a global "shape" list
   This method may possibly register duplicates if executed concurrently,
   but that does not affect functionality
   */
const DbTypeInfo* DbTypeInfo::fromString(const std::string& string_rep)
/*
 Create extra shapes when GUID is the same but column names are different
 (column naming change happened by accident during Reflex migration
 - now they use fully qualified typenames)
*/
{
   const DbTypeInfo* main_type_info = 0;
   bool              created_main_ti = false;
   DbTypeInfo* new_type_info = new DbTypeInfo(Guid::null());
   //cout << " -- fromDbString DbTypeInfo: " << string_rep << endl;
   if( new_type_info->i_fromString(string_rep).isSuccess() )   {
      // find existing typeinfo or create a fresh one based on transient dictionary
      // do this first to ensure current type is first in the DbTransform list
      //cout << " -- fromDbString DbTypeInfo: " << string_rep << "  GUID=" << new_type_info->shapeID() <<  endl;
      if( DbTransform::getShape( new_type_info->shapeID() , main_type_info) != DbStatus::Success) {
         // new shape
         if( !new_type_info->m_class ) {
            // no transient type info for this type, use the string description as it is          
            DbTransform::regShape( new_type_info );
            //cout << "DbTypeInfo::fromString:  registered new  " << string_rep << endl;
            return new_type_info;
         }
         // create new shape from transient type
         Columns cols;
         main_type_info = new DbTypeInfo( new_type_info->shapeID(), new_type_info->m_class, cols );
         DbTransform::regShape(main_type_info);
         created_main_ti = true;
         // cout << "DbTypeInfo::fromString:  registered new " <<  main_type_info->toString() << endl;
      } 
      // current shape is now registered, check if the one from DB has the same column names
      if( new_type_info->m_class && main_type_info->toString() != string_rep ) {
         // difference. See if DB shape is already known
         if( created_main_ti ) {
            // store pointer for later deletion to avoid memory leak
            // the 'proper' shape will sit in the DbTransform list in case another database needs to use it
            DbTransform::ownShape( main_type_info );  
            // cout << "DbTypeInfo::fromString:  registered new MAIN " << main_type_info->toString() << endl;
         }
         // cout << " -- fromDbString DbTypeInfo:" <<"   searching for:" << string_rep << endl;
         const DbTypeInfo* extra_info = DbTransform::getShape( new_type_info->shapeID(), string_rep );
         if( extra_info ) {
            // this extra shape is already registered, use it
            main_type_info = extra_info;
            // cout << "DbTypeInfo::fromString:  found EXTRA cached " <<  main_type_info->toString() << endl; 
         } else {
            // register the new extra shape and use it in this database
            // cout << "DbTypeInfo::fromString:  registered new EXTRA " << string_rep << endl;
            DbTransform::regShape( new_type_info );
            return new_type_info;
         }
      } else {
         // cout << "DbTypeInfo::fromString:  found cached " << main_type_info->toString() << endl;
      }
   }
   delete new_type_info;
   return main_type_info;
}


/// Create type information using Guid only Class must already be registered.
DbTypeInfo* DbTypeInfo::create(const std::string& cl_name)  {
  Columns cols;
  return create(cl_name, cols);
}

/// Create type information using Guid only Class must already be registered.
DbTypeInfo* DbTypeInfo::create(const Guid& guid)  {
  Columns cols;
  return create(guid, cols);
}

/// Create type information using Guid only Class must already be registered.
DbTypeInfo* DbTypeInfo::createEx(const Guid& guid)  {
  Columns cols;
  return createEx(guid, cols);
}


// small helper method to add a new shape
DbTypeInfo* DbTypeInfo::regShape(const Guid& guid, const TypeH type, Columns& cols)
{
   DbTypeInfo* typ_info = new DbTypeInfo(guid, type, cols); 
   DbTransform::regShape(typ_info);
   return typ_info;
}


/// Create type information using name
DbTypeInfo* DbTypeInfo::create(const std::string& cl_name, Columns& cols)   {
  TypeH type = DbReflex::forTypeName(cl_name);
  if ( type )    {
    Guid guid(DbReflex::guid(type));
    DbTypeInfo* typ_info = 0;
    if (DbTransform::getShape(guid, (const DbTypeInfo*&)typ_info) == DbStatus::Success) {
       clearColumns(cols);
       return typ_info;
    }
    return regShape(guid, type, cols); 
  }
  clearColumns(cols);
  return 0;
}



/// Create type information using Guid only Class must already be registered.
DbTypeInfo* DbTypeInfo::create(const Guid& guid, Columns& cols)   {
  DbTypeInfo* typ_info = 0;
  if (DbTransform::getShape(guid, (const DbTypeInfo*&)typ_info) == DbStatus::Success) {
    clearColumns(cols);
    return typ_info;
  }
  TypeH type = DbReflex::forGuid(guid);
  if( type )    {
     return regShape(guid, type, cols); 
  }
  clearColumns(cols);
  return 0;
}

/// Create type information using Guid only Class must already be registered.
DbTypeInfo* DbTypeInfo::createEx(const Guid& guid, Columns& cols)   {
  DbTypeInfo* typ_info = 0;
  if (DbTransform::getShape(guid, (const DbTypeInfo*&)typ_info) == DbStatus::Success) {
    clearColumns(cols);
    return typ_info;
  }
  TypeH type = DbReflex::forGuid(guid);
  if( !(cols.size() == 0 && !type) ) {
     return regShape(guid, type, cols); 
  }
  clearColumns(cols);
  return 0;
}

/// Delete reference to object
void DbTypeInfo::deleteRef() const {
  if (--m_refCount <= 0) {
    DbTransform::removeShape(this);
    delete this;
  }
}
