/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "StorageSvc/DbObjectCallBack.h"
#include "StorageSvc/DbColumn.h"
using namespace pool;

DbStatus DbObjectCallBack::start(CallType /* action_type */, void* data, void** context)  {
  *context = data;
  return Success;
}


DbStatus DbObjectCallBack::bind( CallType action_type,
                                 const DbColumn* col_identifier,
                                 int /* col_number */,
                                 void* context,
                                 void** data )
{
  switch(action_type)  {
  case GET:
    // For reading we can only bind the starting address of the persistent data buffer.
    *data = &m_objectData;
    break;
  case PUT:
    /* This works for ROOT Trees but not for ROOT keys !!!
    if ( context != m_objectData ) {
      std::cerr << "Context different than persistent data" << std::endl;
      return Error;
    }
    */
    *data = static_cast< char* >( context ) + col_identifier->offset();
    break;
  default:
    return Error;
  };
  return Success;
}

DbStatus DbObjectCallBack::end( CallType /* action_type */, void* /* context */ ) {
  return Success;
}
