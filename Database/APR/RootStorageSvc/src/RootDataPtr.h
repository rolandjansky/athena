/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//        Root Database implementation
//====================================================================
//
//        Package    : RootStorageSvc (The POOL project)
//
//        Author     : M.Frank
//====================================================================
#ifndef POOL_ROOTSTORAGESVC_ROOTDATAPTR_H
#define POOL_ROOTSTORAGESVC_ROOTDATAPTR_H 1

#include "PersistentDataModel/Token.h"
#include "StorageSvc/poolDb.h"
#include "StorageSvc/DbBlob.h"
#include "StorageSvc/DbArray.h"
#include <string>

class TObject;

/*
 * POOL namespace declaration
 */
namespace pool  { 
  // Forward declarations
  class Blob;
  class DbPointer;

  /** @union RootDataPtrRootDataPtr.h
    */
  union RootDataPtr {
    void*                    ptr;
    void**                   pptr;
    const void*              cptr;
    char*                    c_str;
    std::string*             str;
    TObject*                 tobj;
    DbArray<int>*            tlinks;
    DbArray<char>*           array;
    std::vector<Token::OID_t>* links;
    DbBlob*                  blob;
    DbPointer*               pointer;
    RootDataPtr(const void* p)  { cptr = p;                   }
    void* deref()               { return *pptr;               }
    const char* string()        { return str->c_str();        }
    int* ibuffer()              { return tlinks->m_buffer;    }
    int  isize()                { return tlinks->m_size;      }
    int  blobSize()             { return blob->buffPointer(); }
    char* blobData()            { return blob->data();        }
    int  linkSize()             { return int(links->size());  }
    long long* linkData()             { return links->size()>0 ? &(links->begin()->first) : 0; }
  };
}       // end namespace pool
#endif  /* POOL_ROOTSTORAGESVC_ROOTDB_PTR_H */
