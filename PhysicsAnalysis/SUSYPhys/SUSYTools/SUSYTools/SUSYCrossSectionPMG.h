/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __SUSYCROSSSECTIONPMG__
#define __SUSYCROSSSECTIONPMG__

// Function arguments
#include <string>

// For PMG centralized tool's wrapper tool handles
#include "AsgTools/AnaToolHandle.h"
// Lots of function inlining!
#include "PMGAnalysisInterfaces/IPMGCrossSectionTool.h"

namespace PMGTools {
  class IPMGCrossSectionTool;
}

namespace SUSY
{

class CrossSectionDBPMG
{
public:
  CrossSectionDBPMG(const std::string& txtfilenameOrDir = "SUSYTools/data/mc15_13TeV/");

  void loadFile(const std::string&);

  float xsectTimesEff(int id) const { return m_pmgxs->getSampleXsection(id); };
  float rawxsect(int id) const { return m_pmgxs->getAMIXsection(id); };
  float kfactor(int id) const { return m_pmgxs->getKfactor(id); };

  float efficiency(int id/*, int proc = 0*/) const { return m_pmgxs->getFilterEff(id); }
  float rel_uncertainty(int /*id*/, int /*proc = 0*/) const { return -1; };
  float sumweight(int /*id, int proc = 0*/) const { return -1; };

  // invalid ID return "" string
  std::string name(int id) const { return m_pmgxs->getSampleName(id); }

private:

  //PMG tool
  asg::AnaToolHandle<PMGTools::IPMGCrossSectionTool> m_pmgxs;

};

}

#endif
