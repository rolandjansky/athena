/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef __ASGELECTRONSELECTORTOOL__
#define __ASGELECTRONSELECTORTOOL__


// Atlas includes
#include "AsgTools/AsgTool.h"
#include "EgammaAnalysisInterfaces/IAsgElectronLikelihoodTool.h"
#include "xAODEgamma/ElectronFwd.h"

class EventContext;

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
  /** Method to get the plain AcceptInfo.
      This is needed so that one can already get the AcceptInfo
      and query what cuts are defined before the first object
      is passed to the tool. */
  virtual const asg::AcceptInfo& getAcceptInfo() const override;

  /** The main accept method: using the generic interface */
  asg::AcceptData accept( const xAOD::IParticle* part ) const override;
  asg::AcceptData accept( const EventContext& ctx, const xAOD::IParticle* part ) const override;

  /** The main accept method: the actual cuts are applied here */
  asg::AcceptData accept( const EventContext& ctx, const xAOD::Electron* eg ) const override {
    return accept (ctx, eg, -99); // mu = -99 as input will force accept to grab the pileup variable from the xAOD object
  }

  /** The main accept method: the actual cuts are applied here */
  asg::AcceptData accept(const EventContext& ctx, const xAOD::Egamma* eg ) const override {
    return accept (ctx, eg, -99); // mu = -99 as input will force accept to grab the pileup variable from the xAOD object
  }

  /** The main accept method: in case mu not in EventInfo online */
  asg::AcceptData accept( const EventContext& ctx, const xAOD::Electron* eg, double mu ) const override;

  /** The main accept method: in case mu not in EventInfo online */
  asg::AcceptData accept( const EventContext& ctx, const xAOD::Egamma* eg, double mu ) const override;

  // Main methods for IAsgCalculatorTool interface
public:
  /** The main result method: the actual mva score is calculated here */
  double calculate( const xAOD::IParticle* part ) const;
  double calculate( const EventContext &ctx, const xAOD::IParticle* part ) const override;

  /** The main result method: the actual mva score is calculated here */
  double calculate( const EventContext &ctx, const xAOD::Electron* eg ) const override {
    return calculate (ctx, eg, -99); // mu = -99 as input will force accept to grab the pileup variable from the xAOD object
  }

  /** The main result method: the actual mva score is calculated here */
  double calculate( const EventContext &ctx, const xAOD::Egamma* eg ) const override {
    return calculate (ctx, eg, -99); // mu = -99 as input will force accept to grab the pileup variable from the xAOD object
  }

  /** The main result method: the actual mva score is calculated here */
  double calculate( const EventContext &ctx, const xAOD::Electron* eg, double mu ) const override;

  /** The main result method: the actual mva score is calculated here */
  double calculate( const EventContext &ctx, const xAOD::Egamma* eg, double mu ) const override;

  virtual std::string getOperatingPointName() const override;

  // Private methods
private:
  /// Accept info
  asg::AcceptInfo m_acceptMVA;

  /// check for FwdElectron
  bool isForwardElectron( const xAOD::Egamma* eg, const float eta ) const;

  /** Applies a logit transformation to the score returned by the underlying MVA tool*/
  double transformMLOutput( double score ) const;

  /** Gets the Discriminant Eta bin [0,s_fnDiscEtaBins-1] given the eta*/
  unsigned int getDiscEtaBin( double eta ) const;

  /** Gets the Descriminant Et bin the et (MeV) [0,s_fnDiscEtBinsOneExtra-1] or [0,s_fnDiscEtBins-1]*/
  unsigned int getDiscEtBin( double et ) const;

  // NOTE that this will only perform the cut interpolation up to ~45 GeV, so
  // no smoothing is done above this for the high ET MVA binning yet
  /** Interpolates cut values along pt*/
  double interpolateCuts( const std::vector<double>& cuts, double et, double eta ) const;



  // Private member variables
private:

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


  /// number of discrimintants vs Et
  static const unsigned int s_fnDiscEtBins = 10;
  /// number of discriminants vs |eta|
  static const unsigned int s_fnDiscEtaBins = 10;


}; // End: class definition

#endif
