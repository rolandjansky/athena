/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//        Root Database container implementation
//--------------------------------------------------------------------
//
//        Package    : pool (The POOL project)
//
//        Author     : M.Frank
//====================================================================
// Framework include files
#include "StorageSvc/DbColumn.h"
#include "StorageSvc/DbTypeInfo.h"
#include "RootDataPtr.h"
#include "RootCallEnv.h"
#include "TBuffer.h"
#include "TClass.h"

using namespace pool;

RootCallEnv::RootCallEnv( RootDataPtr object, const DbTypeInfo *typ )
      : m_object( object ), m_objType( typ )
{}


void RootCallEnv::read(TBuffer& R__b)   {
  char  text[1024];
  /// Loop over columns...
  for( const auto col : m_objType->columns() ) {
     RootDataPtr p( m_object ), q(0);
     p.c_str += col->offset(); 
     unsigned char bool_buf;
     int size = 0;
     TClass* cl = 0;

     const std::string& typ = col->typeName();
     switch ( col->typeID() ) {
      case DbColumn::POINTER:
      case DbColumn::ANY:
         cl = TClass::GetClass(typ.c_str());
         if( cl )  {
            cl->ReadBuffer(R__b, q.ptr = cl->New());
         }
         *p.pptr = q.tobj;
         break;
      case DbColumn::BLOB:
         R__b >> size;
         //R__b >> size;
         p.blob->reserve(size);
         R__b.ReadFastArray(p.blob->data(), size);
         break;
      case DbColumn::TOKEN:
         R__b >> size;
         for ( ; size > 0; size-=2 ) {
            Token::OID_t link;
            R__b >> link.first >> link.second;
            p.links->push_back(link);
         }
         break;
      case DbColumn::STRING:
      case DbColumn::LONG_STRING:
         R__b.ReadString(text,sizeof(text));
         *p.str = text;
         break;
      case DbColumn::NTCHAR:
      case DbColumn::LONG_NTCHAR:
         R__b.ReadString(text,sizeof(text));
         ::strcpy(p.c_str, text);
         break;
      case DbColumn::UCHAR:
         R__b >> *(unsigned char*)p.ptr;
         break;
      case DbColumn::USHORT:
         R__b >> *(unsigned short*)p.ptr;
         break;
      case DbColumn::UINT:
         R__b >> *(unsigned int*)p.ptr;
         break;
      case DbColumn::ULONG:
         R__b >> *(unsigned long*)p.ptr;
         break;
      case DbColumn::CHAR:
         R__b >> *(char*)p.ptr;
         break;
      case DbColumn::SHORT:
         R__b >> *(short*)p.ptr;
         break;
      case DbColumn::INT:
         R__b >> *(int*)p.ptr;
         break;
      case DbColumn::LONG:
         R__b >> *(long*)p.ptr;
         break;
      case DbColumn::BOOL:
         R__b >> bool_buf;
         *(bool*)p.ptr = (bool_buf==1) ? true : false;
         break;
      case DbColumn::FLOAT:
         R__b >> *(float*)p.ptr;
         break;
      case DbColumn::DOUBLE:
         R__b >> *(double*)p.ptr;
         break;
      default:
         break;
     }
  }
}

void RootCallEnv::write(TBuffer& R__b)
{
   for( const auto col : m_objType->columns() ) {
      RootDataPtr p( m_object ), q(0);
      p.c_str += col->offset(); 
      TClass* cl = 0;

      const std::string& typ = col->typeName();
      switch ( col->typeID() ) {
       case DbColumn::POINTER:
       case DbColumn::ANY:
          cl = TClass::GetClass(typ.c_str());
          if( cl ) {
             cl->WriteBuffer(R__b, p.tobj);
          }
          break;
       case DbColumn::BLOB:
          R__b << p.blobSize();
          R__b.WriteFastArray(p.blobData(),p.blobSize());
          break;
       case DbColumn::TOKEN:
          R__b << 2*p.linkSize();
          R__b.WriteFastArray(p.linkData(),2*p.linkSize());
          break;
       case DbColumn::STRING:
       case DbColumn::LONG_STRING:
          R__b.WriteString(p.str->c_str());
          break;
       case DbColumn::NTCHAR:
       case DbColumn::LONG_NTCHAR:
          R__b.WriteString((char*)p.deref());
          break;
       case DbColumn::UCHAR:
          R__b << *(unsigned char*)p.ptr;
          break;
       case DbColumn::USHORT:
          R__b << *(unsigned short*)p.ptr;
          break;
       case DbColumn::UINT:
          R__b << *(unsigned int*)p.ptr;
          break;
       case DbColumn::ULONG:
          R__b << *(unsigned long*)p.ptr;
          break;
       case DbColumn::CHAR:
          R__b << *(char*)p.ptr;
          break;
       case DbColumn::SHORT:
          R__b << *(short*)p.ptr;
          break;
       case DbColumn::INT:
          R__b << *(int*)p.ptr;
          break;
       case DbColumn::LONG:
          R__b << *(long*)p.ptr;
          break;
       case DbColumn::BOOL:
          R__b << (unsigned char)((*(bool*)p.ptr) ? 1 : 0);
          break;
       case DbColumn::FLOAT:
          R__b << *(float*)p.ptr;
          break;
       case DbColumn::DOUBLE:
          R__b << *(double*)p.ptr;
          break;
       default:
          break;
      }
   }
}
