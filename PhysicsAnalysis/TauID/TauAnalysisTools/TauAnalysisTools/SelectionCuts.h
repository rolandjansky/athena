// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SELECTIONCUTS_H
#define SELECTIONCUTS_H

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
#ifndef __MAKECINT__
#include "xAODTau/TauJet.h"
#endif // __MAKECINT__
#include "PATCore/TAccept.h"

// Local include(s):
#include "TauAnalysisTools/TauSelectionTool.h"

// ROOT include(s):
#include "TH1F.h"

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

protected:
  std::string m_sName;

  TH1F* m_hHistCutPre;
  TH1F* m_hHistCut;

  TauSelectionTool* m_tTST;

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
  void fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist);
};

class SelectionCutEleBDTWP
  : public SelectionCut
{
public:
  SelectionCutEleBDTWP(TauSelectionTool* tTST);
  bool accept(const xAOD::TauJet& xTau);
private:
  void fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist);
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

}

#endif // SELECTIONCUTS_H
