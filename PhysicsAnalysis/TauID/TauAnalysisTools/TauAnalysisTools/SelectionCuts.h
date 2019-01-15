// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUANALYSISTOOLS_SELECTIONCUTS_H
#define TAUANALYSISTOOLS_SELECTIONCUTS_H

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
#include "xAODTau/TauJet.h"
#include "PATCore/TAccept.h"

// ROOT include(s):
#include "TH1F.h"

// EDM include(s)
#include "xAODMuon/MuonContainer.h"

namespace TauAnalysisTools
{

class TauSelectionTool;

class SelectionCut
{

public:
  SelectionCut(std::string sName, TauAnalysisTools::TauSelectionTool* tTST);
  virtual ~SelectionCut();

  void writeControlHistograms();
  void fillHistogramCutPre(const xAOD::TauJet& xTau);
  void fillHistogramCut(const xAOD::TauJet& xTau);
  virtual bool accept(const xAOD::TauJet& xTau) = 0;
  TH1F* CreateControlPlot(const char* sName, const char* sTitle, int iBins, double dXLow, double dXUp);

  std::string getName()
  {
    return m_sName;
  };

  void setProperty(const std::string& name, const std::string& value);

protected:
  std::string m_sName;

  TH1F* m_hHistCutPre;
  TH1F* m_hHistCut;

  TauSelectionTool* m_tTST;

  void declareProperty(const std::string& name, std::string& loc);
  std::map<std::string, std::string&> m_mProperties;
  std::string getProperty(const std::string& name);

private:
  virtual void fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist) = 0;
};

class SelectionCutPt
  : public SelectionCut
{
public:
  SelectionCutPt(TauSelectionTool* tTST);
  bool accept(const xAOD::TauJet& xTau);
private:
  void fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist);
};

class SelectionCutAbsEta
  : public SelectionCut
{
public:
  SelectionCutAbsEta(TauSelectionTool* tTST);
  bool accept(const xAOD::TauJet& xTau);
private:
  void fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist);
};

class SelectionCutAbsCharge
  : public SelectionCut
{
public:
  SelectionCutAbsCharge(TauSelectionTool* tTST);
  bool accept(const xAOD::TauJet& xTau);
private:
  void fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist);
};

class SelectionCutNTracks
  : public SelectionCut
{
public:
  SelectionCutNTracks(TauSelectionTool* tTST);
  bool accept(const xAOD::TauJet& xTau);
private:
  void fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist);
};

class SelectionCutBDTJetScore
  : public SelectionCut
{
public:
  SelectionCutBDTJetScore(TauSelectionTool* tTST);
  bool accept(const xAOD::TauJet& xTau);
private:
  void fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist);
};

class SelectionCutBDTJetScoreSigTrans
  : public SelectionCut
{
public:
  SelectionCutBDTJetScoreSigTrans(TauSelectionTool* tTST);
  bool accept(const xAOD::TauJet& xTau);
private:
  void fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist);
};

class SelectionCutRNNJetScoreSigTrans
  : public SelectionCut
{
public:
  SelectionCutRNNJetScoreSigTrans(TauSelectionTool* tTST);
  bool accept(const xAOD::TauJet& xTau);
private:
  void fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist);
};

class SelectionCutJetIDWP
  : public SelectionCut
{
public:
  SelectionCutJetIDWP(TauSelectionTool* tTST);
  bool accept(const xAOD::TauJet& xTau);
private:
  void fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist);
};

class SelectionCutBDTEleScore
  : public SelectionCut
{
public:
  SelectionCutBDTEleScore(TauSelectionTool* tTST);
  bool accept(const xAOD::TauJet& xTau);
private:
  std::string m_sEleBDTDecorationName;
  void fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist);
};

class SelectionCutEleBDTWP
  : public SelectionCut
{
public:
  SelectionCutEleBDTWP(TauSelectionTool* tTST);
  bool accept(const xAOD::TauJet& xTau);
private:
  std::string m_sEleBDTDecorationName;
  void fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist);
};

class SelectionCutEleOLR
  : public SelectionCut
{
public:
  SelectionCutEleOLR(TauSelectionTool* tTST);
  bool accept(const xAOD::TauJet& xTau);
  ~SelectionCutEleOLR();

  bool getEvetoPass(const xAOD::TauJet& xTau);
private:
#ifndef XAODTAU_VERSIONS_TAUJET_V3_H
  bool m_bCheckEleMatchPassAvailable;
  bool m_bEleMatchPassAvailable;
#endif // not XAODTAU_VERSIONS_TAUJET_V3_H

  std::string m_sEleOlrPassDecorationName;

  void fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist);
  std::string m_sEleOlrLhScoreDecorationName;
};

class SelectionCutMuonVeto
  : public SelectionCut
{
public:
  SelectionCutMuonVeto(TauSelectionTool* tTST);
  bool accept(const xAOD::TauJet& xTau);
private:
  void fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist);
};

//added by Li-Gang Xia < ligang.xia@cern.ch >
//to remove taus overlapping with mouns satisfying pt > 2 GeV and not calo-tagged
class SelectionCutMuonOLR
  : public SelectionCut
{
public:
  SelectionCutMuonOLR(TauSelectionTool* tTST);
  bool accept(const xAOD::TauJet& xTau);
private:
  bool m_bTauMuonOLR; //False: overlapped, the tau is not kept. True: not overlapped, the tau is kept.)
  const xAOD::MuonContainer* m_xMuonContainer;
  void fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist);
};


}

#endif // TAUANALYSISTOOLS_SELECTIONCUTS_H
