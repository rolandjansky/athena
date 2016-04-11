/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Package pool/StorageSvc - interfaces
#include "StorageSvc/IStorageSvc.h"
#include "StorageSvc/IOODatabase.h"
#include "StorageSvc/IDbContainer.h"
#include "StorageSvc/IDbDatabase.h"
#include "StorageSvc/IDbDomain.h"
#include "StorageSvc/IDbSession.h"
#include "StorageSvc/FileDescriptor.h"

// Package pool/StorageSvc - implementation
#include "StorageSvc/DbArray.h"
#include "StorageSvc/DbBlob.h"
#include "StorageSvc/DbOption.h"
#include "StorageSvc/DbCallBack.h"
#include "StorageSvc/DbObjectCallBack.h"
#include "StorageSvc/DbObjectGuard.h"
#include "StorageSvc/DbConnection.h"

#include "StorageSvc/DbObject.h"
#include "StorageSvc/DbContainer.h"
#include "StorageSvc/DbDatabase.h"
#include "StorageSvc/DbDomain.h"
#include "StorageSvc/DbSession.h"

#include "StorageSvc/DbTransaction.h"
#include "StorageSvc/DbReflex.h"
#include "StorageSvc/DbSelect.h"
#include "StorageSvc/DbOption.h"
#include "StorageSvc/DbToken.h"
#include "StorageSvc/DbString.h"
#include "StorageSvc/DbColumn.h"
#include "StorageSvc/DbTypeInfo.h"

namespace pool {
  class StorageSvc {
    StorageSvc() {}
    virtual ~StorageSvc() {}
  };
}

namespace {
  struct GCCXML_DUMMY_INSTANTIATION_STORAGESVC {
    pool::DbArray<char>            m__c_array;
    pool::DbArray<unsigned char>   m_uc_array;
    pool::DbArray<short>           m__s_array;
    pool::DbArray<unsigned short>  m_us_array;
    pool::DbArray<int>             m__i_array;
    pool::DbArray<unsigned int>    m_ui_array;
    pool::DbArray<long>            m__l_array;
    pool::DbArray<unsigned long>   m_ul_array;
    pool::DbArray<float>           m__f_array;
    pool::DbArray<double>          m__d_array;
  };
}
