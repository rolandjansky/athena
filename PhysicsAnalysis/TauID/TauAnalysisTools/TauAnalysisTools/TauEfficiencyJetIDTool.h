// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUANALYSISTOOLS_TAUEFFICIENCYJETIDTOOL_H
#define TAUANALYSISTOOLS_TAUEFFICIENCYJETIDTOOL_H

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

class TauEfficiencyJetIDTool : public CommonEfficiencyTool
{
  ASG_TOOL_CLASS( TauEfficiencyJetIDTool,
                  TauAnalysisTools::ITauEfficiencyCorrectionsTool )

public:

  TauEfficiencyJetIDTool(std::string sName);

  virtual ~TauEfficiencyJetIDTool();

  CP::CorrectionCode getEfficiencyScaleFactor(const xAOD::TauJet& xTau,
      double& efficiencyScaleFactor);
};
} // namespace TauAnalysisTools

#endif // TAUANALYSISTOOLS_TAUEFFICIENCYJETIDTOOL_H
