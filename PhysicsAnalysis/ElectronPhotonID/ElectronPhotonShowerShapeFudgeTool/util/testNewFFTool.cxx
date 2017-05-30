/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// to make this work:
// uncomment everything here
// then in TPhotonMCShifterTool.h comment the namespace IDVAR part and add #include "ElectronPhotonShowerShapeFudgeTool/FudgeMCTool.h"

/*
#include "ElectronPhotonShowerShapeFudgeTool/FudgeMCTool.h"
#include "ElectronPhotonShowerShapeFudgeTool/TPhotonMCShifterTool.h"
#include "TRandom3.h"
#include <iostream>

bool verbose = false;

int main() {

  FudgeMCTool oldfftool;
  TPhotonMCShifterTool newfftool;
  
  int nchecks = 10000;
  for (int i=0; i<nchecks; i++) {
    if (i%(nchecks/100)==0) std::cout << (i*100/nchecks) << "%" << std::endl;
    int preselection = gRandom->Integer(18);
    if (preselection==9) continue;
    newfftool.LoadFFs(preselection);
    oldfftool.LoadFFs(preselection);
    double pt = gRandom->Uniform(200000.);
    double eta2 = gRandom->Uniform(2.37);
    for (int ivar=0; ivar<12; ivar++)
      for (int conv=0; conv<2; conv++) {
	double newff = newfftool.GetFF(ivar, pt, eta2, conv);
	double oldff = oldfftool.GetFF(ivar, pt, eta2, conv, preselection);
	if (verbose)
	  std::cout << Form("ivar = %d, pt = %lf, eta2 = %lf, conv = %d: old = %lf, new = %lf", ivar, pt, eta2, conv, oldff, newff) << std::endl;
	if (newff!=oldff) {
	  std::cout << Form("Different results for ivar = %d, pt = %lf, eta2 = %lf, conv = %d: old = %lf, new = %lf", ivar, pt, eta2, conv, oldff, newff) << std::endl;
	  exit(-1);
	}
      }
  }
  return 0;
}

*/
