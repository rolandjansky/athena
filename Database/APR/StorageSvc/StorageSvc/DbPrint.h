/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbPrint.h 458019 2011-09-09 10:11:57Z mnowak $
//====================================================================
//  Storage Service Printer object
//--------------------------------------------------------------------
//
//  @author      M.Frank
//====================================================================
#ifndef POOL_STORAGESVC_DBPRINT_H
#define POOL_STORAGESVC_DBPRINT_H 1

// Framework customization file
#include "CoralBase/MessageStream.h"

/*
 *   POOL namespace declaration
 */
namespace pool {
  typedef coral::MessageStream DbPrint;
  namespace DbPrintLvl = coral;
}       // End namespace pool
#endif  // POOL_STORAGESVC_DBPRINT_H
