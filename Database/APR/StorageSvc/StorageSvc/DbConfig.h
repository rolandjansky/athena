/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbConfig.h 458019 2011-09-09 10:11:57Z mnowak $
//====================================================================
//  Storage Service configuration file
//--------------------------------------------------------------------
//
//  - If you don't use the SealKernel Package, an isolation from it
//    is possible for the StorageSvc and the RootStorageSvc package
//    by uncommenting the flag 
//    #define NO_SEALKERNEL
//
//  @author      M.Frank
//
//====================================================================
#ifndef POOL_STORAGESVC_CONFIG_H
#define POOL_STORAGESVC_CONFIG_H

#ifndef NO_SEALKERNEL

// Uncomment if no SealKernel package is present
#define NO_SEALKERNEL

#endif

#endif    // POOL_STORAGESVC_CONFIG_H
