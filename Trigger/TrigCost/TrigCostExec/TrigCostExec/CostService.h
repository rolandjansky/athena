/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_COSTSERVICE_H
#define ANP_COSTSERVICE_H

//
// Singleton class to read cost ntuples with external event loop
//  - Read configuration
//  - Create and configure C++ code
//  - Read event data and present cost summary
//

// C/C++
#include <map>
#include <string>

// Local
#include "TrigCostExec/ExecData.h"

class TFile;
class TTree;

namespace Anp
{
  class Registry;
  class RunModule;
  
  class CostService {
  public:

    static CostService& Instance();

    void Config(const std::string &config_path);

    bool LoadNextEvent();
    
    //
    // Return current event summary
    //
    Exec::Event GetEvent(const std::string &alg_name) const;    
    
    //
    // Return list of algorithms executed on current event
    //
    std::vector<Ptr<Exec::AlgCall> > GetAlgCalls(const std::string &alg_name) const;

  private:
    
    CostService();
    ~CostService();
    
    CostService(const CostService &);
    CostService& operator=(const CostService &);

  private:

    RunModule    *fModule;        // RunModule for processing algorithms
    TFile        *fFile;          // Current ROOT file
    TTree        *fTree;          // Current event tree
    
    unsigned      fCurrFileIndex; // Current file index
    int           fCurrTreeIndex; // Current tree entry
  };
}

#endif
