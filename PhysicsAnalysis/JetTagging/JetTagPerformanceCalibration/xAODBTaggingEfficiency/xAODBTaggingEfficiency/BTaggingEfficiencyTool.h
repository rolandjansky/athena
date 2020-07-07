// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BTaggingEfficiencyTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef CPBTAGGINGEFFICIENCYTOOL_H
#define CPBTAGGINGEFFICIENCYTOOL_H

#include "FTagAnalysisInterfaces/IBTaggingEfficiencyTool.h"
#include "FTagAnalysisInterfaces/IBTaggingSelectionTool.h"
#include "PATInterfaces/ISystematicsTool.h"

#include "xAODBTagging/BTagging.h"

#include <fstream>
#include <string>
#include <set>
#include <vector>
#include <map>
//#include <memory>

#include "AsgTools/AsgTool.h"
#include "AsgTools/AnaToolHandle.h"

#include "CalibrationDataInterface/CalibrationDataVariables.h"
#include "CalibrationDataInterface/CalibrationDataInterfaceROOT.h"

class BTaggingEfficiencyTool: public asg::AsgTool,
            public virtual IBTaggingEfficiencyTool
{
  //  typedef double (xAOD::BTagging::* tagWeight_member_t)() const;

  /// Create a proper constructor for Athena
  ASG_TOOL_CLASS2( BTaggingEfficiencyTool , IBTaggingEfficiencyTool, ISystematicsTool )

  public:


  /// Create a constructor for standalone usage
  BTaggingEfficiencyTool( const std::string& name );

  /// Create a constructor for standalone usage
  virtual ~BTaggingEfficiencyTool();

  // For now, disable the generation of a default copy constructor (since it would not be constructed correctly)
  // BTaggingEfficiencyTool(const BTaggingEfficiencyTool& other) = delete;

  // /// Silly copy constructor for the benefit of dictionary generation
  // BTaggingEfficiencyTool(const BTaggingEfficiencyTool& other);

  /// @name Methods implementing the main jet-by-jet access in the xAOD context
  /// @{

  /** Computes the data/MC efficiency scale factor for the given jet.
      The tagger and operating point under consideration are part of the configuration and hence aren't function arguments.
   */
  CP::CorrectionCode getScaleFactor( const xAOD::Jet & jet,
             float & sf) const;

  /** Computes the data efficiency for the given jet.
      The tagger and operating point under consideration are part of the configuration and hence aren't function arguments.
   */
  CP::CorrectionCode getEfficiency( const xAOD::Jet & jet,
				    float & eff) const;

  /** Computes the data inefficiency for the given jet.
      The tagger and operating point under consideration are part of the configuration and hence aren't function arguments.
   */
  CP::CorrectionCode getInefficiency( const xAOD::Jet & jet,
              float & eff) const;

  /** Computes the data/MC inefficiency scale factor for the given jet.
      The tagger and operating point under consideration are part of the configuration and hence aren't function arguments.
   */
  CP::CorrectionCode getInefficiencyScaleFactor( const xAOD::Jet & jet,
             float & sf) const;

  /** Computes the MC efficiency for the given jet.
      The tagger and operating point under consideration are part of the configuration and hence aren't function arguments.
   */
  CP::CorrectionCode getMCEfficiency( const xAOD::Jet & jet,
              float & eff) const;

  /// @name Methods equivalent to those above but not relying on the xAOD format
  /// @{

  /** Computes the data/MC efficiency scale factor for the jet, given its kinematics, (possibly) tagger weight and truth flavour.
      The tagger and operating point under consideration are part of the configuration and hence aren't function arguments.
  */
  CP::CorrectionCode getScaleFactor( int flavour, const Analysis::CalibrationDataVariables& v,
             float & sf) const;

  /** Computes the data efficiency for the jet, given its kinematics, (possibly) tagger weight and truth flavour.
      The tagger and operating point under consideration are part of the configuration and hence aren't function arguments.
   */
  CP::CorrectionCode getEfficiency( int flavour, const Analysis::CalibrationDataVariables& v,
            float & eff) const;

  /** Computes the data inefficiency for the jet, given its kinematics, (possibly) tagger weight and truth flavour.
      The tagger and operating point under consideration are part of the configuration and hence aren't function arguments.
   */
  CP::CorrectionCode getInefficiency( int flavour, const Analysis::CalibrationDataVariables& v,
              float & eff) const;

  /** Computes the data/MC inefficiency scale factor for the jet, given its kinematics, (possibly) tagger weight and truth flavour.
      The tagger and operating point under consideration are part of the configuration and hence aren't function arguments.
   */
  CP::CorrectionCode getInefficiencyScaleFactor( int flavour, const Analysis::CalibrationDataVariables& v,
						 float & sf) const;

  /** Computes the MC efficiency for the jet, given its kinematics, (possibly) tagger weight and truth flavour.
      The tagger and operating point under consideration are part of the configuration and hence aren't function arguments.
   */
  CP::CorrectionCode getMCEfficiency( int flavour, const Analysis::CalibrationDataVariables& v,
              float & eff) const;

  /// @}

  /// @name Other methods implementing the IBTagEfficiencyTool interface
  /// @{

  /** Initialise the tool.
   *
   *  This is the stage at which all of the configuration is done and the underlying @c CalibrationDataInferfaceROOT object is instantiated.
   *  The properties that can be set are documented in the
   *  <a href="https://twiki.cern.ch/twiki/bin/view/AtlasProtected/BTaggingCalibrationDataInterface#xAOD_interface">xAOD interface</a> section
   *  of the CalibrationDataInterface Twiki page.
   */
  StatusCode initialize();

  /** Set the tool to return "shifted" values.
      Note that only single-parameter variations or empty sets (the latter are to revert to nominal results)
      are supported at present; @c SystematicCode::Unsupported will be returned for variations of multiple parameters
      or variations that aren't recognised.
   */
  CP::SystematicCode applySystematicVariation(const CP::SystematicSet & set);

  /** Return a list of all systematic variations supported by this tool.
      Note that this list depends on the uncertainty model used, and on the (dynamic)
      configuration of the eigenvector variations (if this uncertainty model is used).
   */
  CP::SystematicSet affectingSystematics() const;

  /** Return a list of "recommended" systematic variations supported by this tool.
      At present, multiple views of the same uncertainties (beyond the uncertainty model etc., see above)
      are not implemented, so this method simply calls @c affectingSystematics() .
   */
  CP::SystematicSet recommendedSystematics() const;

  /// Returns whether or not the given systematic variation is supported by this tool
  bool isAffectedBySystematic(const CP::SystematicVariation & systematic ) const;

  /**
   * @brief Specify the "map index" to be used for the given jet flavour (at initialisation time it will be set to 0).
   *
   * @return false if the requested index is invalid (in which case no setting will be changed)
   *
   * See the <a href="https://twiki.cern.ch/twiki/bin/view/AtlasProtected/BTaggingCalibrationDataInterface#MultipleMC">CalibrationDataInterface</a>
   * documentation for more detail on the meaning of the map index
   */
  bool setMapIndex(const std::string& flavour, unsigned int index);
  bool setMapIndex(unsigned int dsid);
  // bool setMapIndex(const std::string& flavour, const std::string & type);
  /// @}

  /// @name query methods
  /// @{
  /// Utility method returning a detailed list of systematics (including the flavours to which they apply)
  const std::map<CP::SystematicVariation, std::vector<std::string> > listSystematics() const;

  /// Retrieve the name of the tagger (as specified in the calibration file)
  std::string getTaggerName() const { return m_taggerName;}

  /// Retrieve the operating point (as specified in the calibration file)
  std::string getOperatingPoint() const { return m_OP;}

  /// Retrieve the jet collection name (as specified in the calibration file) for which this tool was setup
  std::string getJetAuthor() const { return m_jetAuthor;}

  // /// Returns false if the tool isn't initialised yet (it has to be initialised before processing jets)
  // bool isInitialized() const { return m_initialised;}

  /// Specify whether any systematic variation is being used at present
  bool applySystematics() const { return m_applySyst;}

  /**
   * This merely passes on the request to the underlying CDI object (listSystematics() cannot be used here, as corresponding CP::SystematicVariation objects may not exist).
   * Note that the uncertainty naming does not follow the rewriting conventions leading to the names one will see as CP::SystematicVariations, but rather follows the "raw"
   * names used on input (which are appropriate e.g. when excluding uncertainties from the eigenvalue decomposition).
   */
  std::map<std::string, std::vector<std::string> > listScaleFactorSystematics(bool named = false) const;
  /// @}

  /**
   * Run EigenvectorRecomposition method and get the coefficient map.
   * Calling EigenVectorRecomposition method in CDI and retrieve recomposition map.
   * If success, coefficientMap would be filled and return ok.
   * If failed, return error.
   * label  :  flavour label
   * coefficientMap: store returned coefficient map. This map could help expressing eigenvector NPs by linear
   * combination of original uncertainty NPs in workspace level of physics analysis. The coefficient value
   * is stored in the map in the format of:
   * map<"Eigen_B_0", map<"[original uncertainty name]", [corresponding coefficient value]>> 
   */
  CP::CorrectionCode getEigenRecompositionCoefficientMap(const std::string &label, std::map<std::string, std::map<std::string, float>> & coefficientMap);
  /// @}

private:

  struct SystInfo {
    SystInfo() : uncType(Analysis::SFEigen), isUp(true) {;}
    std::map<unsigned int, unsigned int> indexMap;
    Analysis::Uncertainty uncType;
    // bool isNamed;
    bool isUp;
    bool getIndex( unsigned int flavourID, unsigned int & index) const;
  };

  /// add entries to the systematics registry
  bool addSystematics(const std::vector<std::string> & systematicNames,unsigned int flavourID, Analysis::Uncertainty uncType);

  /// generate names for the eigenvector variations for the given jet flavour
  std::vector<std::string> makeEigenSyst(const std::string & flav, int number, const std::string& suffix);

  /// helper function for retrieving object indices
  bool getIndices(unsigned int flavour, unsigned int & sf, unsigned int & ef) const;

  /// convert integer flavour index to its string equivalent
  std::string getLabel(int flavourID) const {
    switch(flavourID) {
    case 5:
      return "B";
      break;
    case 4:
      return "C";
      break;
    case 15:
      return "T";
      break;
    case 0:
      return "Light";
      break;
    default:
      return "Light";
    }
  }

  /// convert string flavour to its integer index equivalent
  unsigned int getFlavourID(const std::string& label) const {
    // always default to "light" = 0
    if( label.size() <1)
      return 0;

    switch (label[0]) {
    case 'B':
      return 5; break;
    case 'C':
      return 4; break;
    case 'T':
      return 15; break;
    default:
      return 0;
    }
  }

  /** Fill the @c Analysis::CalibrationDataVariables struct with relevant information pertaining to the jet considered

      @return false if the requested information cannot be retrieved (this should never happen
              except if "continuous tagging" is used and the no b-tagging was applied to the jet)
   */
  bool fillVariables(const xAOD::Jet& jet, Analysis::CalibrationDataVariables& x) const;

  /** Fill the @c Analysis::CalibrationDataVariables struct with relevant information pertaining to the jet considered
   */
  bool fillVariables(const double jetPt, const double jetEta, const double jetTagWeight, Analysis::CalibrationDataVariables& x) const;

  /// pointer to the object doing the actual work
  Analysis::CalibrationDataInterfaceROOT*  m_CDI = nullptr;

  /// @name core configuration properties (set at initalization time and not modified afterwards)
  /// @{

  /// we need access to a BTaggingSelectionTool, at least for DL1 weight computation
  asg::AnaToolHandle<IBTaggingSelectionTool> m_selectionTool;

  /// name of the data/MC efficiency scale factor calibration file (may be changed by the @c PathResolver)
  std::string m_SFFile;
  /// name of the optional MC efficiency file (may be changed by the @c PathResolver)
  std::string m_EffFile;
  std::string m_EffConfigFile;
  /// names of the data/MC scale factor calibrations
  std::map<std::string, std::string> m_SFNames;
  /// specification of the eigenvector reduction strategy (if eigenvectors are used)
  std::map<std::string, std::string> m_EVReduction;
  /// semicolon-separated lists of MC efficiency parametrisation names
  std::map<std::string, std::string> m_EffNames;
  /// semicolon-separated list of uncertainties to be excluded from the eigenvector variation procedure for all flavours
  std::string m_excludeFromEV;
  /// semicolon-separated list of uncertainties to be excluded from the eigenvector variation procedure for b, c, and light-flavour jets
  std::map<std::string, std::string> m_excludeFlvFromEV;
  /// optional (per-flavour) suffix that can be used to decorrelate uncertainties (between flavours, or -in case of a result from different runs- between periods)
  std::map<std::string, std::string> m_uncertaintySuffixes;
  /// tagger name
  std::string m_taggerName;
  /// operating point
  std::string m_OP;
  ///  jet collection name
  std::string m_jetAuthor;
  ///  minimum jet pT
  float m_minPt;
  /// systematics model to be used (current choices are "SFEigen", "SFEigenRefined", and "Envelope")
  std::string m_systStrategy;
  /// if true, attempt to retrieve the data/MC efficiency scale factor calibration files from the @PathResolver development area
  bool m_useDevFile;
  /// if true, use cone-based labelling (as opposed to ghost association)
  bool m_coneFlavourLabel;
  /// if true, use an 'extended' labelling (allowing for multiple HF hadrons -or perhaps partons- in the jet)
  bool m_extFlavourLabel;
  /// when using cone-based labelling (see above), if true, use the "traditional" (parton-based) labelling instead of the current default (hadron-based, exclusive)
  bool m_oldConeFlavourLabel;
  // bool m_excludeJESFromEV;
  /// if true, extract pre-set lists of uncertainties to be recommended from the EV decomposition (in addition to user specified ones)
  bool m_useRecommendedEVExclusions;
  /// if true, ignore out-of-extrapolation range errors (i.e., return CorrectionCode::Ok if these are encountered)
  bool m_ignoreOutOfValidityRange;
  /// if false, suppress any non-error/warning printout from the underlying tool
  bool m_verboseCDITool;
  /// @}

  /// @name Cached variables
  /// @{

  /// flag to indicate tool is initialized correctly when set
  bool m_initialised;


  bool m_applySyst;
  SystInfo m_applyThisSyst;

  std::map<CP::SystematicVariation,SystInfo> m_systematicsInfo;
  // cached for affectedBySystematics
  CP::SystematicSet m_systematics;
  // specifically for continuous tagging
  bool m_isContinuous;
  // pointer to a member function of a b-tagger
  // tagWeight_member_t m_getTagWeight;

  // cache the mapIndex variables (one for each flavour)
  std::map<std::string, unsigned int> m_mapIndices;

  /// actual information identifying scale factor calibration objects
  std::map<unsigned int, unsigned int> m_SFIndices;
  /// actual information identifying efficiency calibration objects
  std::map<unsigned int, unsigned int> m_EffIndices;

  //cache for efficiency map config file that maps from a sample DSID to the correct efficiency map
  std::map<unsigned int, unsigned int> m_DSID_to_MapIndex;
  /// @}

};

#endif // CPBTAGGINGEFFICIENCYTOOL_H

