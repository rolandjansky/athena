// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUANALYSISTOOLS_TAUEFFICIENCYELEIDTOOL_H
#define TAUANALYSISTOOLS_TAUEFFICIENCYELEIDTOOL_H

/*
  author: Dirk Duschinger
  mail: dirk.duschinger@cern.ch
  documentation in: ../README.rst
                    or
                    https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/tags/TauAnalysisTools-<tag>/README.rst
		    or
                    https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/trunk/README.rst
*/

// Local include(s):
#include "CommonEfficiencyTool.h"

namespace TauAnalysisTools
{

class TauEfficiencyEleIDTool : public CommonEfficiencyTool
{

public:

  TauEfficiencyEleIDTool(std::string sName);

  virtual ~TauEfficiencyEleIDTool();

  CP::CorrectionCode getEfficiencyScaleFactor(const xAOD::TauJet& xTau,
      double& dEfficiencyScaleFactor);

private:
  void setupWorkingPointSubstrings();
  std::string convertIDToString(int iLevel) const;
  std::string convertEVetoToString(int iLevel) const;
  std::string convertOLRToString(int iLevel) const;

  // cache configs;
  int m_iIDLevelCache;
  int m_iEVLevelCache;
  int m_iOLRLevelCache;

  // string configs
  std::string m_sIDLevel;
  std::string m_sEVLevel;
  std::string m_sOLRLevel;

  // string configs for 3p eveto
  std::string m_sIDLevel_eveto3p;
  std::string m_sEVLevel_eveto3p;
  std::string m_sOLRLevel_eveto3p;

  // declaration of the working point
  std::string m_sWorkingPoint_1p;
  std::string m_sWorkingPoint_3p;

  // up/down direction
  int m_iSysDirection;

};
} // namespace TauAnalysisTools

#endif // TAUANALYSISTOOLS_TAUEFFICIENCYELEIDTOOL_H
