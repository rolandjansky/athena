/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __SUSYCROSSSECTIONPMG__
#define __SUSYCROSSSECTIONPMG__

// Function arguments
#include <string>

//For PMG centralized tool's wrapper
#include "PMGTools/PMGCrossSectionTool.h"

namespace SUSY
{

class CrossSectionDBPMG
{
public:
  CrossSectionDBPMG(const std::string& txtfilenameOrDir = "SUSYTools/data/mc15_13TeV/");

  void loadFile(const std::string&);

  //PMG tool
  PMGTools::PMGCrossSectionTool pmgxs;

  float xsectTimesEff(int id) const { return pmgxs.getSampleXsection(id); };
  float rawxsect(int id) const { return pmgxs.getAMIXsection(id); };
  float kfactor(int id) const { return pmgxs.getKfactor(id); };

  float efficiency(int id/*, int proc = 0*/) const { return pmgxs.getFilterEff(id); }
  float rel_uncertainty(int /*id*/, int /*proc = 0*/) const { return -1; };
  float sumweight(int /*id, int proc = 0*/) const { return -1; };

  // invalid ID return "" string
  std::string name(int id) const { return pmgxs.getSampleName(id); }

};

}

#endif
