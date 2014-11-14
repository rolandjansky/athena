/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COMMONEFFICIENCYTOOL_H
#define COMMONEFFICIENCYTOOL_H

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
#include "PathResolver/PathResolver.h"

// EDM include(s):
#include "xAODTau/TauJet.h"
#include "PATInterfaces/CorrectionCode.h"
#include "PATInterfaces/ISystematicsTool.h"

// Local include(s):
#include "TauAnalysisTools/Enums.h"
#include "TauAnalysisTools/ITauEfficiencyCorrectionsTool.h"

#include "TROOT.h"
#include "TClass.h"
#include "TFile.h"
#include "TH1F.h"
#include "TKey.h"

namespace TauAnalysisTools
{

// forward declaration
class TauEfficiencyCorrectionsTool;

class CommonEfficiencyTool
  : public virtual ITauEfficiencyCorrectionsTool
  , public TObject
  , public virtual CP::ISystematicsTool
  , public asg::AsgTool
{

public:

  CommonEfficiencyTool(std::string sName);

  ~CommonEfficiencyTool();

  virtual StatusCode initialize();

  // CommonEfficiencyTool pure virtual public functionality
  //__________________________________________________________________________

  virtual CP::CorrectionCode getEfficiencyScaleFactor(const xAOD::TauJet& tau, double& dEfficiencyScaleFactor) = 0;
  virtual CP::CorrectionCode applyEfficiencyScaleFactor(const xAOD::TauJet& xTau) = 0;
  virtual CP::CorrectionCode getEfficiencyScaleFactorStatUnc(const xAOD::TauJet& tau, double& efficiencyScaleFactorStatUnc) = 0;
  virtual CP::CorrectionCode applyEfficiencyScaleFactorStatUnc(const xAOD::TauJet& tau) = 0;
  virtual CP::CorrectionCode getEfficiencyScaleFactorSysUnc(const xAOD::TauJet& tau, double& efficiencyScaleFactorSysUnc) = 0;
  virtual CP::CorrectionCode applyEfficiencyScaleFactorSysUnc(const xAOD::TauJet& tau) = 0;

  virtual void setParent(TauEfficiencyCorrectionsTool* tTECT);

  /// returns: whether this tool is affected by the given systematis
  virtual bool isAffectedBySystematic( const CP::SystematicVariation& systematic ) const = 0;

  /// returns: the list of all systematics this tool can be affected by
  virtual CP::SystematicSet affectingSystematics() const = 0;

  /// returns: the list of all systematics this tool recommends to use
  virtual CP::SystematicSet recommendedSystematics() const = 0;

  /// configure this tool for the given list of systematic variations.  any
  /// requested systematics that are not affecting this tool will be silently
  /// ignored (unless they
  virtual CP::SystematicCode applySystematicVariation ( const CP::SystematicSet& sSystematicSet) = 0;


protected:

  std::string ConvertProngToString(const int& iProngness);

  typedef std::map<std::string, TH1F*> SFMAP;
  SFMAP* m_mSF;
  TauEfficiencyCorrectionsTool* m_tTECT;
  const CP::SystematicSet* m_sSystematicSet;
  std::map<std::string, int> m_mSystematics;

  template<class T>
  void ReadInputs(TFile* fFile, std::map<std::string, T>* mMap);
};
} // namespace TauAnalysisTools

#endif // COMMONEFFICIENCYTOOL_H
