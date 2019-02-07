// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUANALYSISTOOLS_TAUSELECTIONTOOL_H
#define TAUANALYSISTOOLS_TAUSELECTIONTOOL_H

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
#include "AsgTools/AsgMetadataTool.h"
#include "AsgTools/AnaToolHandle.h"
#include "PATCore/IAsgSelectionTool.h"

// Local include(s):
#include "TauAnalysisTools/ITauSelectionTool.h"
#include "TauAnalysisTools/Enums.h"
#include "TauAnalysisTools/HelperFunctions.h"
// for some reason it's not enough to include interface
#include "TauAnalysisTools/TauOverlappingElectronLLHDecorator.h"

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
class SelectionCutEleOLR;
class SelectionCutMuonVeto;
class SelectionCutMuonOLR;

enum SelectionCuts
{
  NoCut           = 0,   	// 000000000000
  CutPt           = 1,   	// 000000000001
  CutAbsEta       = 1<<1,	// 000000000010
  CutPhi          = 1<<2,	// 000000000100
  CutNTrack       = 1<<3,	// 000000001000
  CutAbsCharge    = 1<<4,	// 000000010000
  CutJetBDTScore  = 1<<5,	// 000000100000
  CutJetIDWP      = 1<<6,	// 000001000000
  CutEleBDTScore  = 1<<7,	// 000010000000
  CutEleBDTWP     = 1<<8,	// 000100000000
  CutMuonVeto     = 1<<9,	// 001000000000
  CutEleOLR       = 1<<10,	// 010000000000
  CutMuonOLR      = 1<<11	// 100000000000
};
  
class TauSelectionTool : public virtual IAsgSelectionTool,
  public virtual ITauSelectionTool,
  public asg::AsgMetadataTool
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
  friend class SelectionCutEleOLR;
  friend class SelectionCutMuonVeto;
  friend class SelectionCutMuonOLR;

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
  virtual StatusCode initialize() override;

  /// Function initialising the tool
  virtual StatusCode initializeEvent() override __attribute__ ((deprecated("This function is deprecated. Please remove it from your code.\nFor further information please refer to the README:\nhttps://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/trunk/doc/README-TauSelectionTool.rst")));

  /// Get an object describing the "selection steps" of the tool
  virtual const asg::AcceptInfo& getAcceptInfo() const override;

  /// Get the decision using a generic IParticle pointer
  virtual asg::AcceptData accept( const xAOD::IParticle* p ) const override;

  /// Get the decision for a specific TauJet object
  virtual asg::AcceptData accept( const xAOD::TauJet& tau ) const override;

  /// Set output file for control histograms
  virtual void setOutFile( TFile* fOutFile ) override;

  /// Write control histograms to output file
  virtual void writeControlHistograms() override;

private:

  // Execute at each new input file
  virtual StatusCode beginInputFile() override;
  // Execute at each event
  virtual StatusCode beginEvent() override;

  template<typename T, typename U>
  void FillRegionVector(std::vector<T>& vRegion, U tMin, U tMax);
  template<typename T, typename U>
  void FillValueVector(std::vector<T>& vRegion, U tVal);
  template<typename T>
  void PrintConfigRegion(std::string sCutName, std::vector<T>& vRegion);
  template<typename T>
  void PrintConfigValue(std::string sCutName, std::vector<T>& vRegion);
  template<typename T>
  void PrintConfigValue(std::string sCutName, T& sVal);

  // bitmask of tau selection cuts
  int m_iSelectionCuts;
  // vector of transverse momentum cut regions
  std::vector<float> m_vPtRegion;
  // vector of absolute eta cut regions
  std::vector<float> m_vAbsEtaRegion;
  // vector of absolute charge requirements
  std::vector<int> m_vAbsCharges;
  // vector of number of track requirements
  std::vector<size_t> m_vNTracks;
  // vector of JetBDT cut regions
  std::vector<float> m_vJetBDTRegion;
  // JetID working point
  std::string m_sJetIDWP;
  int m_iJetIDWP;
  // vector of EleBDT cut regions
  std::vector<float> m_vEleBDTRegion;
  // EleBDT working point
  std::string m_sEleBDTWP;
  int m_iEleBDTWP;
  // do electron overlap removal
  bool m_bEleOLR;
  // do muon veto
  bool m_bMuonVeto;
  // do muon OLR
  bool m_bMuonOLR;
  // ignore AODFix check and re-calculate electron OLR 
  bool m_bIgnoreAODFixCheck;
  bool m_bRecalcEleOLR;

  float m_dPtMin;
  float m_dPtMax;
  float m_dAbsEtaMin;
  float m_dAbsEtaMax;
  float m_iAbsCharge;
  float m_iNTrack;
  float m_dJetBDTMin;
  float m_dJetBDTMax;
  float m_dEleBDTMin;
  float m_dEleBDTMax;

protected:
  TFile* m_fOutFile;//!
  std::shared_ptr<TH1F> m_hCutFlow;//!

private:
  std::string m_sConfigPath;
  std::string m_sEleOLRFilePath;
  std::string m_sElectronContainerName;
  std::string m_sMuonContainerName;

  asg::AnaToolHandle<TauAnalysisTools::ITauOverlappingElectronLLHDecorator> m_tTOELLHDecorator;

  std::map<SelectionCuts, TauAnalysisTools::SelectionCut*> m_cMap;

  void setupCutFlowHistogram();
  int convertStrToJetIDWP(std::string sJetIDWP);
  int convertStrToEleBDTWP(std::string sEleBDTWP);
  std::string convertJetIDWPToStr(int iJetIDWP);
  std::string convertEleBDTWPToStr(int iEleBDTWP);

protected:
  bool m_bCreateControlPlots;

  /// Object used to store selection information.
  asg::AcceptInfo m_aAccept;



}; // class TauSelectionTool

} // namespace TauAnalysisTools

#endif // TAUANALYSISTOOLS_TAUSELECTIONTOOL_H
