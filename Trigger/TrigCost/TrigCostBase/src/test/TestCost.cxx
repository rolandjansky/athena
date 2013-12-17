/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <iostream>
#include <TSystem.h>

#include "TrigCostBase/RunModule.h"

class CostModule {
 public:
  
  CostModule() { std::cout << "CostModule ctor" << std::endl; }
  ~CostModule() { std::cout << "CostModule dtor" << std::endl; }

  void Run() 
  { 
    std::cout << "CostModule - run" << std::endl; 
    
    gSystem->Load("libTrigMonitoringEvent");
    gSystem->Load("libTrigMonitoringEventDict");
    gSystem->Load("libTrigCostBase");
    gSystem->Load("libTrigCostExec");
    gSystem->Load("libTrigCostRate");
    
    Anp::RunModule *run = new Anp::RunModule();
    run->Execute("RunModule.xml");
    delete run;    
  }
};

int main( int /*argc*/, char* /*argv*/[] ) 
{
  CostModule test;
  test.Run();
}
