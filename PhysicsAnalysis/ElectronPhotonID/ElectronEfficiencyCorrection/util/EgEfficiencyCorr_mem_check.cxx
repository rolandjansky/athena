/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

/* 
   To run with something like
   valgrind --tool=memcheck --leak-check=full --suppressions=$ROOTSYS/etc/valgrind-root.supp  --error-limit=no \
   --track-origins=yes --smc-check=all --trace-children=yes  --track-fds=yes --num-callers=30  \
   $ROOTCOREBIN/bin/x86_64-slc6-gcc49-opt/EgEfficiencyCorr_mem_check>valgrind.log 2>&1 & 

   In order to identify memory leaks in out methods 
   Look here:
http://valgrind.org/docs/manual/faq.html#faq.deflost
*/

#include <string>
#include <vector>
#include "EgammaAnalysisInterfaces/IAsgElectronEfficiencyCorrectionTool.h"
#include "AsgMessaging/AsgMessaging.h"
#include "AsgTools/AnaToolHandle.h"
#ifdef XAOD_STANDALONE
// xAOD include(s):
#   include "xAODRootAccess/TEvent.h"
#endif // XAOD_STANDALONE
#include "CxxUtils/ubsan_suppress.h"
#include "TInterpreter.h"


int main( ) {

    // Suppress known ubsan warning we get from cling.
    CxxUtils::ubsan_suppress ([]() { TInterpreter::Instance(); });
    
#ifdef XAOD_STANDALONE
    xAOD::TEvent event;
#endif

    using namespace asg::msgUserCode;
    ANA_CHECK_SET_TYPE (int);

    asg::AnaToolHandle<IAsgElectronEfficiencyCorrectionTool>  tool ("AsgElectronEfficiencyCorrectionTool/ElectronEffCorrection");
    ANA_CHECK(tool.setProperty("CorrelationModel", "FULL" ) &&
            tool.setProperty("ForceDataType",1) &&
            tool.setProperty("IdKey", "Medium") &&
            tool.retrieve());

    asg::AnaToolHandle<IAsgElectronEfficiencyCorrectionTool> eccTool;
    eccTool.setTypeAndName("CP::ElectronChargeEfficiencyCorrectionTool/ElectronChargeCorrection");
    ANA_CHECK(eccTool.setProperty( "CorrectionFileName", 
                "ElectronEfficiencyCorrection/2015_2016/rel20.7/Moriond_February2017_v1/charge_misID/ChargeCorrectionSF.Medium_FixedCutTight.root" )&&
            eccTool.retrieve());

    return 0;
}
