// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUSELECTIONTOOL_H
#define TAUSELECTIONTOOL_H

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
#include "PATCore/IAsgSelectionTool.h"

// Local include(s):
#include "TauAnalysisTools/ITauSelectionTool.h"
#include "TauAnalysisTools/SelectionCuts.h"
#include "TauAnalysisTools/Enums.h"

// ROOT include(s):
#include "TH1F.h"

namespace TauAnalysisTools
{

/// forward declarations
class SelectionCut;
class SelectionCutPt;
class SelectionCutAbsEta;
class SelectionCutAbsCharge;
class SelectionCutNTracks;
class SelectionCutBDTJetScore;
class SelectionCutJetIDWP;
class SelectionCutBDTEleScore;
class SelectionCutEleBDTWP;

enum SelectionCuts
{
  NoCut           = 0,   	// 0000000000
  CutPt           = 1,   	// 0000000001
  CutAbsEta       = 1<<1,	// 0000000010
  CutPhi          = 1<<2,	// 0000000100
  CutNTrack       = 1<<3,	// 0000001000
  CutAbsCharge    = 1<<4,	// 0000010000
  CutJetBDTScore  = 1<<5,	// 0000100000
  CutJetIDWP      = 1<<6,	// 0001000000
  CutEleBDTScore  = 1<<7,	// 0010000000
  CutEleBDTWP     = 1<<8,	// 0100000000
  CutMuonVeto     = 1<<9	// 1000000000
};

class TauSelectionTool : public virtual IAsgSelectionTool,
  public virtual ITauSelectionTool,
  public asg::AsgTool
{
  /// need to define cut classes to be friends to access protected variables,
  /// needed for access of cut thresholds
  friend class SelectionCut;
  friend class SelectionCutPt;
  friend class SelectionCutAbsEta;
  friend class SelectionCutAbsCharge;
  friend class SelectionCutNTracks;
  friend class SelectionCutBDTJetScore;
  friend class SelectionCutJetIDWP;
  friend class SelectionCutBDTEleScore;
  friend class SelectionCutEleBDTWP;
  friend class SelectionCutMuonVeto;

  /// Create a proper constructor for Athena
  ASG_TOOL_CLASS2( TauSelectionTool,
                   IAsgSelectionTool,
                   TauAnalysisTools::ITauSelectionTool )

  // declaration of classes as friends to access private member variables
  friend class TauEfficiencyCorrectionsTool;

public:
  /// Constructor for standalone usage
  TauSelectionTool( const std::string& name );

  virtual ~TauSelectionTool();

  /// Function initialising the tool
  virtual StatusCode initialize();

  /// Get an object describing the "selection steps" of the tool
  virtual const Root::TAccept& getTAccept() const;

  /// Get the decision using a generic IParticle pointer
  virtual const Root::TAccept& accept( const xAOD::IParticle* p ) const;

  /// Get the decision for a specific TauJet object
  virtual const Root::TAccept& accept( const xAOD::TauJet& tau ) const;

  /// Set default recommended properties
  void setRecommendedProperties();

  /// Set output file for control histograms
  void setOutFile( TFile* fOutFile );

  /// Write control histograms to output file
  void writeControlHistograms();

private:
  template<typename T, typename U>
  void FillRegionVector(std::vector<T>* vRegion, U tMin, U tMax);
  template<typename T, typename U>
  void FillValueVector(std::vector<T>* vRegion, U tVal);
  template<typename T>
  void PrintConfigRegion(std::string sCutName, std::vector<T>* vRegion);
  template<typename T>
  void PrintConfigValue(std::string sCutName, std::vector<T>* vRegion);

  // bitmask of tau selection cuts
  int m_iSelectionCuts;
  // vector of transverse momentum cut regions
  std::vector<double> m_vPtRegion;
  // vector of absolute eta cut regions
  std::vector<double> m_vAbsEtaRegion;
  // vector of absolute charge requirements
  std::vector<int> m_vAbsCharges;
  // vector of number of track requirements
  std::vector<size_t> m_vNTracks;
  // vector of JetBDT cut regions
  std::vector<double> m_vJetBDTRegion;
  // JetID working point
  int m_iJetIDWP;
  // vector of EleBDT cut regions
  std::vector<double> m_vEleBDTRegion;
  // EleBDT working point
  int m_iEleBDTWP;

  double m_dPtMin;
  double m_dPtMax;
  double m_dAbsEtaMin;
  double m_dAbsEtaMax;
  double m_iAbsCharge;
  double m_iNTrack;
  double m_dJetBDTMin;
  double m_dJetBDTMax;
  double m_dEleBDTMin;
  double m_dEleBDTMax;

  TFile* m_fOutFile;
  TH1F* m_hCutFlow;

  std::map<SelectionCuts, TauAnalysisTools::SelectionCut*> m_cMap;

  void setupCutFlowHistogram();
  void declarePropertiesBaseline();
protected:
  bool m_bCreateControlPlots;

  /// Object used to store the last decision
  mutable Root::TAccept m_aAccept;



}; // class TauSelectionTool

} // namespace TauAnalysisTools

#endif // TAUSELECTIONTOOL_H
