/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUANALYSISTOOLS_TAUEFFICIENCYTRIGGERTOOL_H
#define TAUANALYSISTOOLS_TAUEFFICIENCYTRIGGERTOOL_H

/*
  author: Dirk Duschinger
  mail: dirk.duschinger@cern.ch
  documentation in: ../README.rst
		    or
                    https://gitlab.cern.ch/atlas/athena/-/blob/master/PhysicsAnalysis/TauID/TauAnalysisTools/README.rst
*/

// Framework include(s):
#include "AsgTools/AsgTool.h"

// EDM include(s):
#include "xAODTau/TauJet.h"
#include "PATInterfaces/CorrectionCode.h"

// Local include(s):
#include "TauAnalysisTools/Enums.h"
#include "TauAnalysisTools/ITauEfficiencyCorrectionsTool.h"
#include "TauAnalysisTools/HelperFunctions.h"
#include "TauAnalysisTools/CommonEfficiencyTool.h"

namespace TauAnalysisTools
{

// forward declaration
class TauEfficiencyCorrectionsTool;

class TauEfficiencyTriggerTool
  : public CommonEfficiencyTool
{
  ASG_TOOL_CLASS( TauEfficiencyTriggerTool,
                  TauAnalysisTools::ITauEfficiencyCorrectionsTool )

public:

  TauEfficiencyTriggerTool(const std::string& sName);

  StatusCode initialize();

  virtual CP::CorrectionCode getEfficiencyScaleFactor(const xAOD::TauJet& tau, double& dEfficiencyScaleFactor, unsigned int iRunNumber = 0, unsigned int iMu = 0);

  virtual StatusCode applySystematicVariation ( const CP::SystematicSet& sSystematicSet);

  virtual StatusCode setRunNumber(int iRunNumber);

  bool isSupportedRunNumber(int iRunNumber) const;

protected:
  std::string convertPeriodToStr() const;

  e_DataPeriod m_ePeriod;
  int m_ePeriodBinning;

  int m_iMinRunNumber;
  int m_iMaxRunNumber;
};
} // namespace TauAnalysisTools

#endif // TAUANALYSISTOOLS_TAUEFFICIENCYTRIGGERTOOL_H
