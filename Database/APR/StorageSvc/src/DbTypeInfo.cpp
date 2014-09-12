/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// $Id: DbTypeInfo.cpp 601961 2014-06-16 14:49:09Z mnowak $
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

#include "DbString.h"

// STL include files
#include <stdexcept>
#include <cstring>
#include <cstdio>
#include <vector>

#if !defined(WIN32)
#include "cxxabi.h"
#endif

#if ROOT_VERSION_CODE < ROOT_VERSION(5,99,0)
#include "Reflex/Builder/ReflexBuilder.h" 

typedef const std::vector<void*>& _Args;

template <class T> static void ctor(void* ret, void* m, _Args, void*)
{  if(ret)*(void**)ret = ::new(m) T(); else ::new(m) T();     }
template <class T> static void dtor(void*, void* m, _Args, void*)  
{  ((T*)m)->~T();                                             }
#if 0
static void Token_db(void* ret, void* m, _Args, void*)
{  if(ret) *(new(ret) Guid()) = ((Token*)m)->dbID();          }
static void Token_cont(void* ret, void* m, _Args, void*)
{  if(ret) *(new(ret) std::string) = ((Token*)m)->contID();   }
static void Token_technology(void* ret, void* m, _Args, void*)
{  if(ret) *(new(ret) (int)) = ((Token*)m)->technology();     }
static void Token_classID(void* ret, void* m, _Args, void*)
{  if(ret) *(new(ret) Guid()) = ((Token*)m)->classID();       }
static void Token_setDb(void* m, void*, _Args a, void*)
{  ((Token*)m)->setDb(*(Guid*)a[0]);                          }
static void Token_setCont(void* m, void*, _Args a, void*)
{  ((Token*)m)->setCont(*(std::string*)a[0]);                 }
static void Token_setTechnology(void* m, void*, _Args a, void*)
{  ((Token*)m)->setTechnology(*(int*)a[0]);                   }
static void Token_setClassID(void* m, void*, _Args a, void*)
{  ((Token*)m)->setClassID(*(Guid*)a[0]);                     }
#endif

#if 0
template <class T, class Q> void __initdbarraydictionary(T*, Q*, const std::string& typ, const std::string& clid) {
  TypeH type_void = TypeBuilder("void");
  TypeH ptr_type  = TypeDistiller<T*>::Get();
  ClassBuilderT<Q > b;
  b.AddProperty("ClassID", clid);
  b.AddFunctionMember(FunctionTypeBuilder(type_void), typ.c_str(),  ctor<Q >, 0, 0, PUBLIC | CONSTRUCTOR );
  b.AddFunctionMember(FunctionTypeBuilder(type_void), std::string("~"+typ).c_str(), dtor<Q >, 0, 0, PUBLIC | DESTRUCTOR | VIRTUAL );
  b.AddDataMember(TypeDistiller<int>::Get(), "m_size", size_t(((char*)&((Q*)64)->m_size)-64), PUBLIC);
  b.AddDataMember(ptr_type, "m_buffer", size_t(((char*)&((Q*)64)->m_buffer)-64), PUBLIC);
  TypeH t = TypeH::ByTypeInfo(typeid(Q));
  t.DataMemberByName("m_size").Properties().AddProperty(  "comment", "Blob size specifier");
  t.DataMemberByName("m_buffer").Properties().AddProperty("comment", "[m_size]");
  //RootType rtype(typ); rtype.AddProperty("id", clid.c_str());  
}

#define __INITDBARRAYDICTIONARY(inside, wrapper, clid)   __initdbarraydictionary((inside*)0, (wrapper*)0, #wrapper, clid);
#endif

POOL_BEGIN_STATIC_BLOCK(DbTypeInfo)
  using namespace pool;
  // Dictionary declaration for Token base class
  Reflex::ClassBuilderT<Token >()
    .AddProperty("ClassID", "E013D4B1-CA9C-4E3C-9BE2-8691BBAFCB9A")
    .AddFunctionMember<void(void)>("Token",  ctor<Token>, 0, 0, Reflex::PUBLIC | Reflex::CONSTRUCTOR )
    .AddFunctionMember<void(void)>("~Token", dtor<Token>, 0, 0, Reflex::PUBLIC | Reflex::DESTRUCTOR | Reflex::VIRTUAL );
#if 0
    .AddFunctionMember<const std::string&(void)>("db",    Token_db, 0, 0, PUBLIC )
    .AddFunctionMember<void(const std::string&)>("setDb", Token_setDb, 0, 0, PUBLIC )
    .AddFunctionMember<const std::string&(void)>("contID",  Token_cont, 0, 0, PUBLIC )
    .AddFunctionMember<void(const std::string&)>("setCont", Token_setCont, 0, 0, PUBLIC )
    .AddFunctionMember<int(void)>("technology", Token_technology, 0, 0, PUBLIC )
    .AddFunctionMember<void(int)>("setTechnology", Token_setTechnology, 0, 0, PUBLIC )
    .AddFunctionMember<const Guid&(void)>("classID", Token_classID, 0, 0, PUBLIC )
    .AddFunctionMember<void(const Guid&)>("setClassID", Token_setClassID, 0, 0, PUBLIC );
#endif
     //RootType rtypeToken("Token"); rtypeToken.AddProperty("id", "E013D4B1-CA9C-4E3C-9BE2-8691BBAFCB9A");

#if 0
  // Dictionary declaration for DbToken class
  ClassBuilderT<pool::DbToken >()
    .AddProperty("ClassID", "B30D24AA-1F1D-4EE0-A9B6-C10405D65854")
    .AddBase<Token>()
    .AddFunctionMember<void(void)>("DbToken",  ctor<DbToken>, 0, 0, PUBLIC | CONSTRUCTOR )
    .AddFunctionMember<void(void)>("~DbToken", dtor<DbToken>, 0, 0, PUBLIC | DESTRUCTOR | VIRTUAL );
#endif
    //RootType rtypeDbToken("pool::DbToken"); rtypeDbToken.AddProperty("id", "B30D24AA-1F1D-4EE0-A9B6-C10405D65854");

#if 0
  // Dictionary declaration for external link class
  ClassBuilderT<Token::OID_t >()
    .AddProperty("ClassID", "9E944D67-6C88-47E2-9834-FCCDA9020F29")
    .AddFunctionMember<void(void)>("OID_t ",  ctor<Token::OID_t>, 0, 0, PUBLIC | CONSTRUCTOR )
    .AddFunctionMember<void(void)>("~OID_t ", dtor<Token::OID_t>, 0, 0, PUBLIC | DESTRUCTOR );
#endif
     //RootType rtypeOID("Token::OID_t"); rtypeOID.AddProperty("id", "9E944D67-6C88-47E2-9834-FCCDA9020F29");

  // Dictionary declaration for internal Database strings
  TypeH type_void = Reflex::FunctionDistiller<void(void)>::Get();
  TypeH rtypePoolString = TypeH::ByName("pool::DbString");
  rtypePoolString.Properties().AddProperty("id", "DA8F479C-09BC-49D4-94BC-99D025A23A3B");
  rtypePoolString.AddFunctionMember("DbString", type_void, ctor<DbString>, 0, 0, Reflex::PUBLIC | Reflex::CONSTRUCTOR );
  rtypePoolString.AddFunctionMember("~DbString", type_void, dtor<DbString>, 0, 0, Reflex::PUBLIC | Reflex::CONSTRUCTOR  | Reflex::VIRTUAL);
  Reflex::ClassBuilderT<pool::DbString >()
#if 0
    .AddProperty("ClassID", "DA8F479C-09BC-49D4-94BC-99D025A23A3B")
#endif
    .AddFunctionMember<void(void)>("DbString",  ctor<DbString>, 0, 0, Reflex::PUBLIC | Reflex::CONSTRUCTOR )
    .AddFunctionMember<void(void)>("~DbString", dtor<DbString>, 0, 0, Reflex::PUBLIC | Reflex::DESTRUCTOR | Reflex::VIRTUAL );

#if 0
  __INITDBARRAYDICTIONARY(char,          CharDbArray,  "42A3E929-5016-4D97-932A-15D21E2D17A9");
  __INITDBARRAYDICTIONARY(unsigned char, UCharDbArray, "B798D007-C1B2-41AD-8BEC-ABC6F0E342E8");
  __INITDBARRAYDICTIONARY(short,         ShortDbArray, "35A281C2-8A14-430A-BEDC-FC8CA5C88B4B");
  __INITDBARRAYDICTIONARY(unsigned short,UShortDbArray,"A26688E9-C6A2-4F31-B954-F7B992A3C638");
  __INITDBARRAYDICTIONARY(int,           IntDbArray,   "2C02D4FD-3AC8-4241-A6A2-63CD2C86478C");
  __INITDBARRAYDICTIONARY(unsigned int,  UIntDbArray,  "F297366F-A876-4FCE-9AC3-D4BEDAEAFAE8");
  __INITDBARRAYDICTIONARY(long,          LongDbArray,  "1601B72A-87FE-45CF-B128-FE577C0A438C");
  __INITDBARRAYDICTIONARY(unsigned long, ULongDbArray, "B228247A-AFEE-449E-BD6D-3A1C43555662");
  __INITDBARRAYDICTIONARY(float,         FloatDbArray, "EC636D76-EE64-431A-A046-BEE7A3B73456");
  __INITDBARRAYDICTIONARY(double,        DoubleDbArray,"DF9DDD4F-CBFC-4b75-8A46-FF0A64DD0B2F");
#endif
POOL_END_STATIC_BLOCK(DbTypeInfo)

#endif

static const std::string __typeinfoName( const std::type_info& tinfo) {
  const char* class_name = tinfo.name();
  std::string result;
#ifdef WIN32
  size_t off = 0;
  if ( ::strncmp(class_name, "class ", 6) == 0 )   {
    // The returned name is prefixed with "class "
    off = 6;
  }
  if ( ::strncmp(class_name, "struct ", 7) == 0 )   {
    // The returned name is prefixed with "struct "
    off = 7;
  }
  if ( off != std::string::npos )    {
    std::string tmp = class_name + off;
    size_t loc = 0;
    while( (loc = tmp.find("class ")) != std::string::npos )  {
      tmp.erase(loc, 6);
    }
    loc = 0;
    while( (loc = tmp.find("struct ")) != std::string::npos )  {
      tmp.erase(loc, 7);
    }
    result = tmp;
  }
  else  {
    result = class_name;
  }
  // Change any " *" to "*"
  while ( (off=result.find(" *")) != std::string::npos )    {
    result.replace(off, 2, "*");
  }
  // Change any " &" to "&"
  while ( (off=result.find(" &")) != std::string::npos )    {
    result.replace(off, 2, "&");
  }
#elif defined(sun)
  result = class_name;
#else
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
#endif
  return result;
}

namespace pool  {

   const std::string typeName(const std::type_info& typ) {
    return __typeinfoName(typ);
  }
//  static void delete_DbTypeInfo(void* p) {
//    if ( p ) ((pool::DbTypeInfo*)p)->destroy();
//  }
   
  void clearColumns(DbTypeInfo::Columns& cols)  {
    if ( cols.empty() ) return;
    for(DbTypeInfo::Columns::iterator i=cols.begin(); i != cols.end(); ++i)  {
      if ( 0 != (*i) ) delete (*i);
    }
  }
   
}

using namespace pool;

/// Standard Constructor
DbTypeInfo::DbTypeInfo(const Guid& guid, TypeH cl)
: Shape(),
  m_refCount( 0 ),
  m_columns(),
  m_mult( 0 ),
  m_class( cl )
{
  DbInstanceCount::increment(this);
  setShapeID(guid);
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
TypeH DbTypeInfo::clazz()  const  {
  if ( m_class )  {
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
      convert = false;
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
  {"{ID=","}"}, 
  {"{CL=","}"}, 
  {"{NCOL=","}"},
  {"{COL={","}}"} 
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
          ::sscanf(pp1, "%d", &ncol);
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

/// Load type information object from string representation
const DbTypeInfo* DbTypeInfo::fromString(const std::string& string_rep) {
  Guid clsid(Guid::null());
  DbTypeInfo* pType = new DbTypeInfo(clsid);
  DbStatus sc = pType->i_fromString(string_rep);
  if ( sc.isSuccess() )   {
    DbTypeInfo *typ_info = 0;
    if (DbTransform::getShape(pType->shapeID(), (const DbTypeInfo*&)typ_info) == DbStatus::Success) {
      delete pType;
      return typ_info;
    }
    DbTransform::regShape(pType);
    return pType;
  }
  delete pType;
  return 0;
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
    typ_info = new DbTypeInfo(guid, type);
    if ( cols.size() == 0 )   {
      std::string full = DbReflex::fullTypeName(type);
      std::string nam = (full.substr(0,2)=="::") ? full.substr(2) : full;
      int col_type = DbColumn::POINTER;
      if (typ_info->clazz()) {
        if (typ_info->clazz().TypeInfo() == typeid(unsigned char)) col_type = DbColumn::UCHAR;
        else if (typ_info->clazz().TypeInfo() == typeid(unsigned short)) col_type = DbColumn::USHORT;
        else if (typ_info->clazz().TypeInfo() == typeid(unsigned int)) col_type = DbColumn::UINT;
        else if (typ_info->clazz().TypeInfo() == typeid(unsigned long)) col_type = DbColumn::ULONG;
        else if (typ_info->clazz().TypeInfo() == typeid(char)) col_type = DbColumn::CHAR;
        else if (typ_info->clazz().TypeInfo() == typeid(short)) col_type = DbColumn::SHORT;
        else if (typ_info->clazz().TypeInfo() == typeid(int)) col_type = DbColumn::INT;
        else if (typ_info->clazz().TypeInfo() == typeid(long)) col_type = DbColumn::LONG;
        else if (typ_info->clazz().TypeInfo() == typeid(bool)) col_type = DbColumn::BOOL;
        else if (typ_info->clazz().TypeInfo() == typeid(float)) col_type = DbColumn::FLOAT;
        else if (typ_info->clazz().TypeInfo() == typeid(double)) col_type = DbColumn::DOUBLE;
        else if (typ_info->clazz().TypeInfo() == typeid(char*)) col_type = DbColumn::NTCHAR;
        else if (typ_info->clazz().TypeInfo() == typeid(unsigned long long)) col_type = DbColumn::ULONGLONG;
        else if (typ_info->clazz().TypeInfo() == typeid(long long)) col_type = DbColumn::LONGLONG;
        else if (typ_info->clazz().TypeInfo() == typeid(Token)) col_type = DbColumn::TOKEN;
      }

      typ_info->m_columns.clear();
      typ_info->m_columns.push_back(new DbColumn(type.Name(),
                                    nam,
                                    col_type,
                                    0));
    }
    else  {
      typ_info->m_columns = cols;
      cols.clear();
    }
    DbTransform::regShape(typ_info);
    return typ_info;
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
  TypeH cl = DbReflex::forGuid(guid);
  if ( cl )    {
    typ_info = new DbTypeInfo(guid, cl);
    if ( cols.size() == 0 )   {
      std::string full = DbReflex::fullTypeName(cl);
      std::string nam = (full.substr(0,2)=="::") ? full.substr(2) : full;
      int col_type = DbColumn::POINTER;
      if (typ_info->clazz()) {
        if (typ_info->clazz().TypeInfo() == typeid(unsigned char)) col_type = DbColumn::UCHAR;
        else if (typ_info->clazz().TypeInfo() == typeid(unsigned short)) col_type = DbColumn::USHORT;
        else if (typ_info->clazz().TypeInfo() == typeid(unsigned int)) col_type = DbColumn::UINT;
        else if (typ_info->clazz().TypeInfo() == typeid(unsigned long)) col_type = DbColumn::ULONG;
        else if (typ_info->clazz().TypeInfo() == typeid(char)) col_type = DbColumn::CHAR;
        else if (typ_info->clazz().TypeInfo() == typeid(short)) col_type = DbColumn::SHORT;
        else if (typ_info->clazz().TypeInfo() == typeid(int)) col_type = DbColumn::INT;
        else if (typ_info->clazz().TypeInfo() == typeid(long)) col_type = DbColumn::LONG;
        else if (typ_info->clazz().TypeInfo() == typeid(bool)) col_type = DbColumn::BOOL;
        else if (typ_info->clazz().TypeInfo() == typeid(float)) col_type = DbColumn::FLOAT;
        else if (typ_info->clazz().TypeInfo() == typeid(double)) col_type = DbColumn::DOUBLE;
        else if (typ_info->clazz().TypeInfo() == typeid(char*)) col_type = DbColumn::NTCHAR;
        else if (typ_info->clazz().TypeInfo() == typeid(unsigned long long)) col_type = DbColumn::ULONGLONG;
        else if (typ_info->clazz().TypeInfo() == typeid(long long)) col_type = DbColumn::LONGLONG;
        else if (typ_info->clazz().TypeInfo() == typeid(Token)) col_type = DbColumn::TOKEN;
      }
      std::vector<const DbColumn*> c;
      c.push_back(new DbColumn(cl.Name(),
                               nam,
                               col_type,
                               0));
      typ_info->m_columns = c;
      c.clear();
    }
    else  {
      typ_info->m_columns = cols;
      cols.clear();
    }
    DbTransform::regShape(typ_info);
    return typ_info;
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
  TypeH cl = DbReflex::forGuid(guid);
  if ( !(cols.size() == 0 && !cl) ) {
    typ_info = new DbTypeInfo(guid, cl);
    if ( cols.size() == 0 ) {
      std::string full = DbReflex::fullTypeName(cl);
      std::string nam = (full.substr(0,2)=="::") ? full.substr(2) : full;
      std::vector<const DbColumn*> c;
      c.push_back(new DbColumn(cl.Name(),
                                nam,
                                DbColumn::POINTER,
                                0));
      typ_info->m_columns = c;
      c.clear();
    }
    else {
      typ_info->m_columns = cols;
      cols.clear();
    }
    DbTransform::regShape(typ_info);
    return typ_info;
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
