/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//                                                     
//NAME:     PrescaledClockLoader.h
//PACKAGE:  TrigConfStorage      
//
//AUTHOR:   J.Haller (CERN)	Johannes.Haller@cern.ch 
//CREATED:  31. Oct. 2005                                                     
//                                                     
//PURPOSE:
//
//
//////////////////////////////////////////////////////////////////////

#ifndef TrigConf_PrescaledClockLoader
#define TrigConf_PrescaledClockLoader 

#include "TrigConfStorage/IPrescaledClockLoader.h"
#include "TrigConfStorage/DBLoader.h"


namespace TrigConf {
  
  class PrescaledClockLoader : virtual public IPrescaledClockLoader, public DBLoader {
  public:

    PrescaledClockLoader( StorageMgr& sm,  coral::ISessionProxy& session) : DBLoader(sm, session) {}
    virtual ~PrescaledClockLoader(){};

    virtual bool load( PrescaledClock& data);
  };
}

#endif
