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
#include "ElectronEfficiencyCorrection/IAsgElectronEfficiencyCorrectionTool.h"
#include "AsgAnalysisInterfaces/IEfficiencyScaleFactorTool.h"
#include "AsgTools/AsgMessaging.h"
#include "AsgTools/AnaToolHandle.h"

int main( ) {



  std::vector<std::string> inputFiles ={
    "ElectronEfficiencyCorrection/2015_2016/rel20.7/ICHEP_June2016_v1/offline/efficiencySF.offline.LooseAndBLayerLLH_d0z0_v11.2015_2016.13TeV.rel20.7.25ns.v01.root"} ;

  asg::AnaToolHandle<IAsgElectronEfficiencyCorrectionTool>  tool ("AsgElectronEfficiencyCorrectionTool/myEgCorrections");
  if (tool.setProperty("CorrelationModel", "FULL" ) &&
      tool.setProperty("ForceDataType",1) &&
      tool.setProperty("CorrectionFileNameList",inputFiles) &&
      tool.retrieve())
    {
      std::cout<<"ALL FINE" <<std::endl;
      
    }

  asg::AnaToolHandle<CP::IEfficiencyScaleFactorTool> eccTool;
  eccTool.setTypeAndName("CP::ElectronChargeEfficiencyCorrectionTool/myTool");
  if(
     eccTool.setProperty( "CorrectionFileName", "ElectronEfficiencyCorrection/2015_2016/rel20.7/Moriond_February2017_v1/charge_misID/ChargeCorrectionSF.Medium_FixedCutTight.root" )&&
     eccTool.retrieve()){
    std::cout<<"ALL FINE" <<std::endl;
  }
  
  return 0;
}
