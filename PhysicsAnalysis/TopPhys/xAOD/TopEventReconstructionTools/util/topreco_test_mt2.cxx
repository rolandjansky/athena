/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>

#include "RootCore/Packages.h"

#ifdef ROOTCORE_PACKAGE_Oxbridgekinetics
#include "Mt2/Basic_Mt2_332_Calculator.h"
#include "Mt2/Mt2TwoVector.h"
#include "Mt2/Mt2LorentzVector.h"
#include "Mt2/Mt2LorentzTransverseVector.h"
#endif

int main() {

#ifndef ROOTCORE_PACKAGE_Oxbridgekinetics
    std::cout << "Oxbridgekinetics library not found when compiling" << std::endl;
#else
    std::cout << "Starting top mt2 test\n";
    Mt2::LorentzTransverseVector a;
    Mt2::LorentzTransverseVector b;
    Mt2::TwoVector c;
    double d = 0.;

    Mt2::Basic_Mt2_332_Calculator mt2Calculator;
    const double mt2  = mt2Calculator.mt2_332( a, b, c, d);

    std::cout << "mt2 " << mt2 << std::endl;
    std::cout << "Ending\n";
#endif

    return 0;
}
