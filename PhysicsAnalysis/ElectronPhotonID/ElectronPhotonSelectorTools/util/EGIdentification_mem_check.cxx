/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* To run with something like
valgrind --tool=memcheck --leak-check=full --suppressions=$ROOTSYS/etc/valgrind-root.supp  --error-limit=no --track-origins=yes --smc-check=all --trace-children=yes  --track-fds=yes --num-callers=30  $ROOTCOREBIN/bin/x86_64-slc6-gcc49-opt/EGIdentification_mem_check>valgrind.log 2>&1 & 

In order to identify memory leaks in out methods 
Look here:
http://valgrind.org/docs/manual/faq.html#faq.deflost

*/

#include "ElectronPhotonSelectorTools/IAsgPhotonIsEMSelector.h"
#include "ElectronPhotonSelectorTools/IAsgForwardElectronIsEMSelector.h"
#include "ElectronPhotonSelectorTools/IAsgElectronIsEMSelector.h"
#include "ElectronPhotonSelectorTools/IAsgElectronLikelihoodTool.h"

#include "ElectronPhotonSelectorTools/AsgPhotonIsEMSelector.h"
#include "ElectronPhotonSelectorTools/AsgForwardElectronIsEMSelector.h"
#include "ElectronPhotonSelectorTools/AsgElectronIsEMSelector.h"
#include "ElectronPhotonSelectorTools/AsgElectronLikelihoodTool.h"
#include "CxxUtils/make_unique.h"


int main(){

  /*std::unique_ptr<AsgElectronIsEMSelector> electronMediumIsEMSelector = CxxUtils::make_unique<AsgElectronIsEMSelector> ("electronMediumIsEMSelector");
    electronMediumIsEMSelector->setProperty("WorkingPoint", "MediumElectron");
    electronMediumIsEMSelector->initialize();
  */

  std::unique_ptr<AsgElectronLikelihoodTool> MediumLH = CxxUtils::make_unique<AsgElectronLikelihoodTool> ("MediumLH");
  MediumLH->setProperty("WorkingPoint", "MediumLHElectron");
  MediumLH->initialize();

  return 0;
}
