/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_COOLDB_H
#define ANP_COOLDB_H

/**********************************************************************************
 * @Package: TrigCostRate
 * @Class  : CoolDB
 * @Author : Rustem Ospanov
 *
 * @Brief  : Wrapper class for COOL database access
 * 
 **********************************************************************************/

// C/C++
#include <map>
#include <string>

// COOL
#include "CoolKernel/DatabaseId.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "CoolKernel/IDatabase.h"

namespace Anp
{

  class CoolDB {
  public:

    CoolDB();
    ~CoolDB();
    
    void SetConnectionCOOL(const std::string &id) { fCoolId = id; }
    
    cool::DatabaseId&   cool_id()  { return fCoolId; }
    cool::IDatabasePtr& cool_ptr() { return fCoolPtr; }
    
    bool IsOpen();
    void CloseDB();
    bool OpenDB();

  private:

    cool::IDatabaseSvc& databaseService();

  private:

    cool::DatabaseId            fCoolId;        // COOL connection id
    cool::IDatabasePtr          fCoolPtr;       // COOL database pointer    
  };
}

#endif
