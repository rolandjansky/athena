/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef __ASGELECTRONSELECTORTOOL__
#define __ASGELECTRONSELECTORTOOL__


// Atlas includes
#include "AsgTools/AsgTool.h"
#include "EgammaAnalysisInterfaces/IAsgElectronLikelihoodTool.h"
#include "xAODEgamma/ElectronFwd.h"
#include "PATCore/TAccept.h"            // for TAccept
#include "PATCore/TResult.h"            // for TResult

class ElectronDNNCalculator;

class AsgElectronSelectorTool final : public asg::AsgTool,
                                      virtual public IAsgElectronLikelihoodTool
{
  ASG_TOOL_CLASS2(AsgElectronSelectorTool, IAsgElectronLikelihoodTool, IAsgSelectionTool)

public:
  /** Standard constructor */
  AsgElectronSelectorTool( const std::string myname );


  /** Standard destructor */
  virtual ~AsgElectronSelectorTool();
public:
  /** Gaudi Service Interface method implementations */
  virtual StatusCode initialize() override;

  // Main methods for IAsgSelectorTool interface
  /** The main accept method: using the generic interface */
  const Root::TAccept& accept( const xAOD::IParticle* part ) const override;

  /** The main accept method: the actual cuts are applied here */
  const Root::TAccept& accept( const xAOD::Electron* eg ) const override{
    return accept (eg, -99); // mu = -99 as input will force accept to grab the pileup variable from the xAOD object
  }

  /** The main accept method: the actual cuts are applied here */
  const Root::TAccept& accept( const xAOD::Egamma* eg ) const override{
    return accept (eg, -99); // mu = -99 as input will force accept to grab the pileup variable from the xAOD object
  }

  /** The main accept method: in case mu not in EventInfo online */
  const Root::TAccept& accept( const xAOD::Electron* eg, double mu ) const override;

  /** The main accept method: in case mu not in EventInfo online */
  const Root::TAccept& accept( const xAOD::Egamma* eg, double mu ) const override;

  // Main methods for IAsgCalculatorTool interface
public:
  /** The main result method: the actual mva score is calculated here */
  const Root::TResult& calculate( const xAOD::IParticle* part ) const override;

  /** The main result method: the actual mva score is calculated here */
  const Root::TResult& calculate( const xAOD::Electron* eg ) const override{
    return calculate (eg, -99); // mu = -99 as input will force accept to grab the pileup variable from the xAOD object
  }

  /** The main result method: the actual mva score is calculated here */
  const Root::TResult& calculate( const xAOD::Egamma* eg ) const override{
    return calculate (eg, -99); // mu = -99 as input will force accept to grab the pileup variable from the xAOD object
  }

  /** The main result method: the actual mva score is calculated here */
  const Root::TResult& calculate( const xAOD::Electron* eg, double mu ) const override;

  /** The main result method: the actual mva score is calculated here */
  const Root::TResult& calculate( const xAOD::Egamma* eg, double mu ) const override;

  /** Method to get the plain TAccept */
  virtual const Root::TAccept& getTAccept() const override
  {
    return m_acceptMVA;
  }


  /** Method to get the plain TResult */
  virtual const Root::TResult& getTResult() const override
  {
    return m_resultMVA;
  }


  virtual std::string getOperatingPointName() const override;

  // Private methods
private:
  /// check for FwdElectron
  bool isForwardElectron( const xAOD::Egamma* eg, const float eta ) const;

  /** Applies a logit transformation to the score returned by the underlying MVA tool*/
  double transformMLOutput( double score ) const;

  /** Combines the six output nodes of a multiclass model into one discriminant. */
  double combineOutputs(const std::vector<double>& mvaScores, double eta) const;

  /** Gets the Discriminant Eta bin [0,s_fnDiscEtaBins-1] given the eta*/
  unsigned int getDiscEtaBin( double eta ) const;

  /** Gets the Descriminant Et bin the et (MeV) [0,s_fnDiscEtBins-1]*/
  unsigned int getDiscEtBin( double et ) const;

  // NOTE that this will only perform the cut interpolation up to ~45 GeV, so
  // no smoothing is done above this for the high ET MVA binning yet
  /** Interpolates cut values along pt*/
  double interpolateCuts( const std::vector<double>& cuts, double et, double eta ) const;

  /// Set all entries of the TResult to a default value of -9999
  void resetTResult() const;

  // Private member variables
private:
  /** TAccept object to return MVA result*/
  // Should not be swept/cherry-picked into master. Need to change to appropriate thread-safe idioms and need to drop mutable
  mutable Root::TAccept m_acceptMVA;
  /** TResult object to return MVA result*/
  // Should not be swept/cherry-picked into master. Need to change to appropriate thread-safe idioms and need to drop mutable
  mutable Root::TResult m_resultMVA;


  /// Working Point
  std::string m_workingPoint;

  /// The input config file.
  std::string m_configFile;

  /** Pointer to the class that calculates the MVA score. const for thread safety */
  std::unique_ptr<const ElectronDNNCalculator> m_mvaTool;

  /// The input file name that holds the model
  std::string m_modelFileName;

  /// The input file name that holds the QuantileTransformer
  std::string m_quantileFileName;

  /// Variables used in the MVA Tool
  std::vector<std::string> m_variables;


  /// Multiclass model or not
  bool m_multiClass;
  /// Use the CF output node in the numerator or the denominator
  bool m_cfSignal;
  /// Fractions to combine the output nodes of a multiclass model into one discriminant.
  std::vector<double> m_fractions;

  /// do cut on ambiguity bit
  std::vector<int> m_cutAmbiguity;
  /// cut min on b-layer hits
  std::vector<int> m_cutBL;
  /// cut min on pixel hits
  std::vector<int> m_cutPi;
  /// cut min on precision hits
  std::vector<int> m_cutSCT;
  /// do smooth interpolation between bins
  bool m_doSmoothBinInterpolation;
  /// cut on mva output
  std::vector<double> m_cutSelector;


  /// The position of the kinematic cut bit in the AcceptInfo return object
  int m_cutPosition_kinematic;
  /// The position of the NSilicon cut bit in the AcceptInfo return object
  int m_cutPosition_NSilicon;
  /// The position of the NPixel cut bit in the AcceptInfo return object
  int m_cutPosition_NPixel;
  /// The position of the NBlayer cut bit in the AcceptInfo return object
  int m_cutPosition_NBlayer;
  /// The position of the ambiguity cut bit in the AcceptInfo return object
  int m_cutPosition_ambiguity;
  /// The position of the MVA cut bit in the AcceptInfo return object
  int m_cutPosition_MVA;
  /// The position of the MVA value bit in the TResult return object
  int m_resultPosition_MVA;
  /// The position of the electron probability bit in the TResult return object
  int  m_resultPosition_EL;
  /// The position of the chargeFlip probability bit in the TResult return object
  int  m_resultPosition_CF;
  /// The position of the photonConversion probability bit in the TResult return object
  int  m_resultPosition_PC;
  /// The position of the heavyFlavor probability bit in the TResult return object
  int  m_resultPosition_HF;
  /// The position of the lightFlavorEgamma probability bit in the TResult return object
  int  m_resultPosition_LE;
  /// The position of the lightFlavorHadron probability bit in the TResult return object
  int  m_resultPosition_LH;

  /// number of discrimintants vs Et
  static const unsigned int s_fnDiscEtBins = 10;
  /// number of discriminants vs |eta|
  static const unsigned int s_fnDiscEtaBins = 10;


}; // End: class definition

#endif
