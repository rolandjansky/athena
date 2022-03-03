/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUANALYSISTOOLS_TAUSELECTIONTOOL_H
#define TAUANALYSISTOOLS_TAUSELECTIONTOOL_H

/*
  author: Dirk Duschinger
  mail: dirk.duschinger@cern.ch
  documentation in: https://gitlab.cern.ch/atlas/athena/-/blob/master/PhysicsAnalysis/TauID/TauAnalysisTools/doc/README-TauSelectionTool.rst
*/

// Framework include(s):
#include "AsgTools/AsgMetadataTool.h"
#include "AsgTools/AnaToolHandle.h"
#include "PATCore/IAsgSelectionTool.h"
#include "AsgDataHandles/ReadHandleKey.h"

// Local include(s):
#include "TauAnalysisTools/ITauSelectionTool.h"
#include "TauAnalysisTools/Enums.h"
#include "TauAnalysisTools/HelperFunctions.h"

// EDM include(s):
#include "xAODMuon/MuonContainer.h"

// ROOT include(s):
#include "TH1F.h"
#include "TFile.h"

namespace TauAnalysisTools
{

/// forward declarations
class SelectionCut;
class SelectionCutPt;
class SelectionCutAbsEta;
class SelectionCutAbsCharge;
class SelectionCutNTracks;
class SelectionCutJetIDWP;
class SelectionCutRNNJetScoreSigTrans;
class SelectionCutRNNEleScore;
class SelectionCutEleIDWP;
class SelectionCutMuonOLR;

enum SelectionCuts
{
  // FIXME: harmonise names for RNN score cuts: "CutEleRNNScore" vs "CutJetRNNScoreSigTrans"
  NoCut           = 0,   	 // 000000000000
  CutPt           = 1,   	 // 000000000001
  CutAbsEta       = 1<<1,	 // 000000000010
  CutPhi          = 1<<2,	 // 000000000100
  CutNTrack       = 1<<3,	 // 000000001000
  CutAbsCharge    = 1<<4,	 // 000000010000
  CutJetIDWP      = 1<<5,	 // 000000100000
  CutEleRNNScore  = 1<<6,	 // 000001000000
  CutEleIDWP      = 1<<7,	 // 000010000000
  CutMuonOLR      = 1<<8,        // 000100000000
  CutJetRNNScoreSigTrans = 1<<9  // 001000000000
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
  friend class SelectionCutJetIDWP;
  friend class SelectionCutRNNJetScoreSigTrans;
  friend class SelectionCutRNNEleScore;
  friend class SelectionCutEleIDWP;
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

  // Execute at each event
  virtual StatusCode beginEvent() override;

  template<typename T, typename U>
  void FillRegionVector(std::vector<T>& vRegion, U tMin, U tMax) const;
  template<typename T, typename U>
  void FillValueVector(std::vector<T>& vRegion, U tVal) const;
  template<typename T>
  void PrintConfigRegion(const std::string& sCutName, std::vector<T>& vRegion) const;
  template<typename T>
  void PrintConfigValue(const std::string& sCutName, std::vector<T>& vRegion) const;
  template<typename T>
  void PrintConfigValue(const std::string& sCutName, T& sVal) const;

  // bitmask of tau selection cuts
  int m_iSelectionCuts;
  // vector of transverse momentum cut regions
  std::vector<float> m_vPtRegion;
  // vector of absolute eta cut regions
  std::vector<float> m_vAbsEtaRegion;
  // vector of absolute charge requirements
  std::vector<int> m_vAbsCharges;
  // vector of number of track requirements
  std::vector<unsigned> m_vNTracks;
  // vector of JetRNNSigTrans cut regions
  std::vector<float> m_vJetRNNSigTransRegion;
  // JetID working point
  std::string m_sJetIDWP;
  int m_iJetIDWP;
  // vector of EleRNN cut regions
  std::vector<float> m_vEleRNNRegion;
  // EleID working point
  std::string m_sEleIDWP;
  int m_iEleIDWP;
  // do muon OLR
  bool m_bMuonOLR;

  float m_dPtMin;
  float m_dPtMax;
  float m_dAbsEtaMin;
  float m_dAbsEtaMax;
  float m_iAbsCharge;
  float m_iNTrack;
  float m_dJetRNNSigTransMin;
  float m_dJetRNNSigTransMax;
  float m_dEleRNNMin;
  float m_dEleRNNMax;

protected:
  TFile* m_fOutFile;//!
  std::shared_ptr<TH1F> m_hCutFlow;//!

private:
  std::string m_sConfigPath;
  SG::ReadHandleKey<xAOD::MuonContainer> m_muonContainerKey {this, "MuonContainerName", "Muons", "Muon container read handle key"};

  std::map<SelectionCuts, std::unique_ptr<TauAnalysisTools::SelectionCut>> m_cMap;

  void setupCutFlowHistogram();
  int convertStrToJetIDWP(const std::string& sJetIDWP) const;
  int convertStrToEleIDWP(const std::string& sEleIDWP) const;
  std::string convertJetIDWPToStr(int iJetIDWP) const;
  std::string convertEleIDWPToStr(int iEleIDWP) const;

protected:
  bool m_bCreateControlPlots;

  /// Object used to store selection information.
  asg::AcceptInfo m_aAccept;


}; // class TauSelectionTool

} // namespace TauAnalysisTools

#endif // TAUANALYSISTOOLS_TAUSELECTIONTOOL_H
