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
#include "StorageSvc/DataCallBack.h"
#include "RootDataPtr.h"
#include "RootCallEnv.h"
#include "TBuffer.h"
#include "TClass.h"

using namespace pool;

RootCallEnv::RootCallEnv(DataCallBack* cb,RootDataPtr& ctxt)
      : call(cb), context(ctxt)
{}


void RootCallEnv::read(TBuffer& R__b)   {
  int icol;
  char  text[1024];
  const DbTypeInfo* typ = (const DbTypeInfo*)call->shape();
  const DbTypeInfo::Columns& cols = typ->columns();
  DbTypeInfo::Columns::const_iterator k;
  RootDataPtr user(0);
  DbStatus status = call->start(DataCallBack::GET, context.ptr, &user.ptr);
  if ( status.isSuccess() )  {
    /// Loop over columns...
    for(icol=0, k=cols.begin(); k != cols.end(); ++k, ++icol)  {
      RootDataPtr p(0), q(0);
      DbStatus sc = call->bind(DataCallBack::GET, *k, icol, user.ptr, &p.ptr);
      if ( sc.isSuccess() )   {
        const std::string& typ = (*k)->typeName();
        unsigned char bool_buf;
        int size = 0;
        TClass* cl = 0;
        switch ( (*k)->typeID() ) {
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
    call->end(DataCallBack::GET, context.ptr);
  }
}

void RootCallEnv::write(TBuffer& R__b)    {
  int icol = 0;
  RootDataPtr p(0), user(0);
  DbTypeInfo::Columns::const_iterator k;
  const DbTypeInfo* typ = (const DbTypeInfo*)call->shape();
  const DbTypeInfo::Columns& cols = typ->columns();

  DbStatus status = call->start(DataCallBack::PUT, context.ptr, &user.ptr);
  if ( status.isSuccess() ) {
    for(k = cols.begin(); k != cols.end(); ++k, ++icol) {
      DbStatus sc = call->bind(DataCallBack::PUT, *k, icol, user.ptr, &p.ptr);
      const std::string& typ = (*k)->typeName();
      if ( sc.isSuccess() )   {
        TClass* cl = 0;
        switch ( (*k)->typeID() ) {
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
    call->end(DataCallBack::PUT, context.ptr);
  }
}
