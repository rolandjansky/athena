/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* To run with something like
valgrind --tool=memcheck --leak-check=full --suppressions=$ROOTSYS/etc/valgrind-root.supp  --error-limit=no --track-origins=yes --smc-check=all --trace-children=yes  --track-fds=yes --num-callers=30  $ROOTCOREBIN/bin/x86_64-slc6-gcc49-opt/EgEfficiencyCorr_mem_check>valgrind.log 2>&1 & 

In order to identify memory leaks in out methods 
Look here:
http://valgrind.org/docs/manual/faq.html#faq.deflost
*/

#include <string>
#include <iostream>
#include <vector>
#include "ElectronEfficiencyCorrection/AsgElectronEfficiencyCorrectionTool.h"
#include "AsgTools/AsgMessaging.h"

int main( ) {

  std::vector<std::string> inputFiles ={
    "ElectronEfficiencyCorrection/2015_2016/rel20.7/ICHEP_June2016_v1/offline/efficiencySF.offline.LooseAndBLayerLLH_d0z0_v11.2015_2016.13TeV.rel20.7.25ns.v01.root"} ;

  AsgElectronEfficiencyCorrectionTool myEgCorrections ("myEgCorrections");
  myEgCorrections.msg().setLevel(MSG::INFO);
  if (myEgCorrections.setProperty("CorrelationModel", "FULL" ) &&
      myEgCorrections.setProperty("ForceDataType",1) &&
      myEgCorrections.setProperty("CorrectionFileNameList",inputFiles) &&
      myEgCorrections.initialize())
    {
      std::cout<<"ALL FINE" <<std::endl;
      
    }

  
  return 0;
}
