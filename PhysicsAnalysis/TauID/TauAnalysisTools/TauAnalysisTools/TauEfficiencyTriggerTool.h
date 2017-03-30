#ifndef TAUANALYSISTOOLS_TAUEFFICIENCYTRIGGERTOOL_H
#define TAUANALYSISTOOLS_TAUEFFICIENCYTRIGGERTOOL_H

/*
  author: Dirk Duschinger
  mail: dirk.duschinger@cern.ch
  documentation in: ../README.rst
                    or
                    https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/tags/TauAnalysisTools-<tag>/README.rst
		    or
                    https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/trunk/README.rst
*/

// Framework include(s):
#include "AsgTools/AsgTool.h"

// EDM include(s):
#include "xAODTau/TauJet.h"
#include "xAODTruth/TruthParticle.h"
#include "PATInterfaces/CorrectionCode.h"

// Local include(s):
#include "TauAnalysisTools/Enums.h"
#include "TauAnalysisTools/ITauEfficiencyCorrectionsTool.h"
#include "TauAnalysisTools/HelperFunctions.h"
#include "TauAnalysisTools/CommonEfficiencyTool.h"

// ROOT include(s):
#include "TROOT.h"
#include "TClass.h"
#include "TFile.h"
#include "TH1F.h"
#include "TF1.h"
#include "TKey.h"

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

  TauEfficiencyTriggerTool(std::string sName);

  StatusCode initialize();

  virtual CP::CorrectionCode getEfficiencyScaleFactor(const xAOD::TauJet& tau, double& dEfficiencyScaleFactor);

  virtual CP::SystematicCode applySystematicVariation ( const CP::SystematicSet& sSystematicSet);

  virtual StatusCode setRunNumber(int iRunNumber);

  bool isSupportedRunNumber(int iRunNumber);

protected:

  std::string convertPeriodToStr();

  e_DataPeriod m_ePeriod;
  int m_ePeriodBinning;

  int m_iMinRunNumber;
  int m_iMaxRunNumber;
};
} // namespace TauAnalysisTools

#endif // TAUANALYSISTOOLS_TAUEFFICIENCYTRIGGERTOOL_H
