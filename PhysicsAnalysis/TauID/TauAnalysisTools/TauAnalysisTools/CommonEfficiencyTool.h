/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUANALYSISTOOLS_COMMONEFFICIENCYTOOL_H
#define TAUANALYSISTOOLS_COMMONEFFICIENCYTOOL_H

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
#include "xAODTruth/TruthParticle.h"
#include "PATInterfaces/CorrectionCode.h"

// Local include(s):
#include "TauAnalysisTools/Enums.h"
#include "TauAnalysisTools/ITauEfficiencyCorrectionsTool.h"
#include "TauAnalysisTools/HelperFunctions.h"

// ROOT include(s):
#include "TFile.h"
#include "TKey.h"

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

  CommonEfficiencyTool(const std::string& sName);

  ~CommonEfficiencyTool();

  virtual StatusCode initialize();

  // CommonEfficiencyTool pure virtual public functionality
  //__________________________________________________________________________

  virtual CP::CorrectionCode getEfficiencyScaleFactor(const xAOD::TauJet& tau, double& dEfficiencyScaleFactor, 
    unsigned int iRunNumber = 0, unsigned int iMu = 0 );

  virtual CP::CorrectionCode applyEfficiencyScaleFactor(const xAOD::TauJet& xTau, 
    unsigned int iRunNumber = 0, unsigned int iMu = 0);

  /// returns: whether this tool is affected by the given systematics
  virtual bool isAffectedBySystematic( const CP::SystematicVariation& systematic ) const;

  /// returns: the list of all systematics this tool can be affected by
  virtual CP::SystematicSet affectingSystematics() const;

  /// returns: the list of all systematics this tool recommends to use
  virtual CP::SystematicSet recommendedSystematics() const;

  /// configure this tool for the given list of systematic variations.  any
  /// requested systematics that are not affecting this tool will be silently ignored
  virtual StatusCode applySystematicVariation ( const CP::SystematicSet& sSystematicSet);

  virtual bool isSupportedRunNumber( int /*iRunNumber*/ ) const
  {
    return true;
  };

protected:

  std::string ConvertProngToString(const int iProngness) const;
  std::string ConvertMuToString(const int iMu) const;
  std::string GetMcCampaignString(const int iMu) const;
  std::string ConvertDecayModeToString(const int iDecayMode) const;

  typedef std::tuple<TObject*,
          CP::CorrectionCode (*)(const TObject* oObject,
                                 double& dEfficiencyScaleFactor,
                                 double dVars[] ) > tTupleObjectFunc;
  typedef std::map<std::string, tTupleObjectFunc > tSFMAP;
  // In gcc10 builds, cling gets confused by the type of m_mSF and produces
  // an ugly warning message.  Hide this from cling to suppress that
  // (substitute another unique_ptr so that the class layout remains the same).
#ifdef __CLING__
  std::unique_ptr<int> m_dummy;
#else
  std::unique_ptr< tSFMAP > m_mSF;
#endif

  std::unordered_map < CP::SystematicSet, std::string > m_mSystematicSets;
  const CP::SystematicSet* m_sSystematicSet;
  std::map<std::string, int> m_mSystematics;
  std::map<std::string, std::string> m_mSystematicsHistNames;

  // std::function<double> m_fX;
  double (*m_fX)(const xAOD::TauJet& xTau);
  double (*m_fY)(const xAOD::TauJet& xTau);

  void ReadInputs(const TFile& fFile);
  void addHistogramToSFMap(TKey* kKey, const std::string& sKeyName);

  virtual CP::CorrectionCode getValue(const std::string& sHistName,
                                      const xAOD::TauJet& xTau,
                                      double& dEfficiencyScaleFactor) const;

  static CP::CorrectionCode getValueTH2(const TObject* oObject,
                                        double& dEfficiencyScaleFactor,
                                        double dVars[]
                                        );
  static CP::CorrectionCode getValueTH3(const TObject* oObject,
                                        double& dEfficiencyScaleFactor,
                                        double dVars[]
                                        );
  static CP::CorrectionCode getValueTF1(const TObject* oObject,
                                        double& dEfficiencyScaleFactor,
                                        double dVars[]
                                       );

  void generateSystematicSets();

protected:

  CP::SystematicSet m_sAffectingSystematics;
  CP::SystematicSet m_sRecommendedSystematics;

  std::string m_sInputFilePath;
  std::string m_sInputFileName;
  std::string m_sWP;
  std::string m_sVarName;
  std::string m_sSFHistName;
  bool m_bSkipTruthMatchCheck;
  bool m_bUseHighPtUncert;
  bool m_bNoMultiprong;
  bool m_bUseTauSubstructure;
  int m_iJetIDLevel;
  int m_iEleIDLevel;

  e_TruthMatchedParticleType m_eCheckTruth;

  bool m_bSFIsAvailable;
  bool m_bSFIsAvailableChecked;
  bool m_bPtTauEtaCalibIsAvailable;
  bool m_bPtTauEtaCalibIsAvailableIsChecked;
  bool m_bSplitMu;
  bool m_bSplitMCCampaign;
  std::string m_sMCCampaign;
};
} // namespace TauAnalysisTools

#endif // TAUANALYSISTOOLS_COMMONEFFICIENCYTOOL_H
