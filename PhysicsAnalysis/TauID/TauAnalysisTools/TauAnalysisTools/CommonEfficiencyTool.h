/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUANALYSISTOOLS_COMMONEFFICIENCYTOOL_H
#define TAUANALYSISTOOLS_COMMONEFFICIENCYTOOL_H

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

class CommonEfficiencyTool
  : public virtual ITauEfficiencyCorrectionsTool
  , public asg::AsgTool
{
  /// Create a proper constructor for Athena
  ASG_TOOL_CLASS( CommonEfficiencyTool, TauAnalysisTools::ITauEfficiencyCorrectionsTool )

public:

  CommonEfficiencyTool(std::string sName);

  ~CommonEfficiencyTool();

  virtual StatusCode initialize();

  // CommonEfficiencyTool pure virtual public functionality
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

protected:

  std::string ConvertProngToString(const int& iProngness);

  typedef std::tuple<TObject*,
          CP::CorrectionCode (*)(const TObject* oObject,
                                 double& dEfficiencyScaleFactor,
                                 double dPt,
                                 double dEta) > tTupleObjectFunc;
  typedef std::map<std::string, tTupleObjectFunc > tSFMAP;
  tSFMAP* m_mSF;

  TauEfficiencyCorrectionsTool* m_tTECT;
  boost::unordered_map < CP::SystematicSet, std::string > m_mSystematicSets;
  const CP::SystematicSet* m_sSystematicSet;
  std::map<std::string, int> m_mSystematics;
  std::map<std::string, std::string> m_mSystematicsHistNames;

  // std::function<double> m_fX;
  double (*m_fX)(const xAOD::TauJet& xTau);
  double (*m_fY)(const xAOD::TauJet& xTau);

  void ReadInputs(TFile* fFile);
  void addHistogramToSFMap(TKey* kKey, const std::string& sKeyName);

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

  std::string m_sInputFilePath;
  std::string m_sInputFileName;
  std::string m_sWP;
  std::string m_sVarName;
  bool m_bSkipTruthMatchCheck;
  std::string m_sSFHistName;
  e_TruthMatchedParticleType m_eCheckTruth;
  bool m_bNoMultiprong;
  CP::SystematicSet m_sAffectingSystematics;
  CP::SystematicSet m_sRecommendedSystematics;

  bool m_bSFIsAvailable;
  bool m_bSFIsAvailableChecked;
};
} // namespace TauAnalysisTools

#endif // TAUANALYSISTOOLS_COMMONEFFICIENCYTOOL_H
