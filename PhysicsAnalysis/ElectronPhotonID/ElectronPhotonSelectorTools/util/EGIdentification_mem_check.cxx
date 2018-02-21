/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* To run with something like
valgrind --tool=memcheck --leak-check=full --suppressions=$ROOTSYS/etc/valgrind-root.supp  --error-limit=no --track-origins=yes --smc-check=all --trace-children=yes  --track-fds=yes --num-callers=30  $ROOTCOREBIN/bin/x86_64-slc6-gcc49-opt/EGIdentification_mem_check>valgrind.log 2>&1 & 

In order to identify memory leaks in out methods 
Look here:
http://valgrind.org/docs/manual/faq.html#faq.deflost

*/

#include "EgammaAnalysisInterfaces/IAsgPhotonIsEMSelector.h"
#include "EgammaAnalysisInterfaces/IAsgForwardElectronIsEMSelector.h"
#include "EgammaAnalysisInterfaces/IAsgElectronIsEMSelector.h"
#include "EgammaAnalysisInterfaces/IAsgElectronLikelihoodTool.h"
#include "AsgTools/AnaToolHandle.h"
#include "AsgTools/MessageCheck.h"

int main(){
  using namespace asg::msgUserCode;
  ANA_CHECK_SET_TYPE (int);

  asg::AnaToolHandle<IAsgElectronLikelihoodTool> MediumLH("AsgElectronLikelihoodTool/MediumLH");
  ANA_CHECK(MediumLH.setProperty("WorkingPoint", "MediumLHElectron"));
  ANA_CHECK(MediumLH.initialize());

  return 0;
}
