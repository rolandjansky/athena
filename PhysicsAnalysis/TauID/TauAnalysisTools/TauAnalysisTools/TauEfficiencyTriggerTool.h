/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

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

// BOOST include(s):
#include <boost/unordered_map.hpp>

namespace TauAnalysisTools
{

// forward declaration
class TauEfficiencyCorrectionsTool;

class TauEfficiencyTriggerTool
  : public CommonEfficiencyTool
{

public:

  TauEfficiencyTriggerTool(std::string sName);

  ~TauEfficiencyTriggerTool();

  virtual StatusCode initialize();

  // TauEfficiencyTriggerTool pure virtual public functionality
  //__________________________________________________________________________

  virtual CP::CorrectionCode getEfficiencyScaleFactor(const xAOD::TauJet& tau, double& dEfficiencyScaleFactor);
  virtual CP::CorrectionCode applyEfficiencyScaleFactor(const xAOD::TauJet& xTau);

  virtual void setParent(TauEfficiencyCorrectionsTool* tTECT);

  /// returns: whether this tool is affected by the given systematis
  virtual bool isAffectedBySystematic( const CP::SystematicVariation& systematic ) const;

  /// returns: the list of all systematics this tool can be affected by
  virtual CP::SystematicSet affectingSystematics() const;

  /// returns: the list of all systematics this tool recommends to use
  virtual CP::SystematicSet recommendedSystematics() const;

  /// configure this tool for the given list of systematic variations.  any
  /// requested systematics that are not affecting this tool will be silently
  /// ignored (unless they
  virtual CP::SystematicCode applySystematicVariation ( const CP::SystematicSet& sSystematicSet);

  virtual StatusCode setRunNumber(int iRunNumber);

protected:

  std::string ConvertProngToString(const int& iProngness);
  std::string convertPeriodToStr();

  typedef std::tuple<TObject*,
          CP::CorrectionCode (*)(const TObject* oObject,
                                 double& dEfficiencyScaleFactor,
                                 double dPt,
                                 double dEta) > tTupleObjectFunc;
  typedef std::map<std::string, tTupleObjectFunc > tSFMAP;

  void ReadInputs(TFile* fFile);
  virtual CP::CorrectionCode getValue(const std::string& sHistName,
                                      const xAOD::TauJet& xTau,
                                      double& dEfficiencyScaleFactor) const;

  static CP::CorrectionCode getValueTH2F(const TObject* oObject,
                                         double& dEfficiencyScaleFactor,
                                         double dPt,
                                         double dEta
                                        );
  static CP::CorrectionCode getValueTH2D(const TObject* oObject,
                                         double& dEfficiencyScaleFactor,
                                         double dPt,
                                         double dEta
                                        );
  static CP::CorrectionCode getValueTF1(const TObject* oObject,
                                        double& dEfficiencyScaleFactor,
                                        double dPt,
                                        double dEta
                                       );

  e_TruthMatchedParticleType checkTruthMatch(const xAOD::TauJet& xTau) const;
  void generateSystematicSets();

  e_DataPeriod m_ePeriod;
  int m_ePeriodBinning;

};
} // namespace TauAnalysisTools

#endif // TAUANALYSISTOOLS_TAUEFFICIENCYTRIGGERTOOL_H
