// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "PATCore/AcceptData.h"
#include "PATCore/AcceptInfo.h"

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
  virtual void setAcceptInfo (asg::AcceptInfo& info) const = 0;
  virtual bool accept(const xAOD::TauJet& xTau,
                      asg::AcceptData& accept) = 0;
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
  virtual void setAcceptInfo (asg::AcceptInfo& info) const override;
  virtual bool accept(const xAOD::TauJet& xTau,
                      asg::AcceptData& accept) override;
private:
  virtual void fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist) override;
};

class SelectionCutAbsEta
  : public SelectionCut
{
public:
  SelectionCutAbsEta(TauSelectionTool* tTST);
  virtual void setAcceptInfo (asg::AcceptInfo& info) const override;
  virtual bool accept(const xAOD::TauJet& xTau,
                      asg::AcceptData& accept) override;
private:
  virtual void fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist) override;
};

class SelectionCutAbsCharge
  : public SelectionCut
{
public:
  SelectionCutAbsCharge(TauSelectionTool* tTST);
  virtual void setAcceptInfo (asg::AcceptInfo& info) const override;
  virtual bool accept(const xAOD::TauJet& xTau,
                      asg::AcceptData& accept) override;
private:
  virtual void fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist) override;
};

class SelectionCutNTracks
  : public SelectionCut
{
public:
  SelectionCutNTracks(TauSelectionTool* tTST);
  virtual void setAcceptInfo (asg::AcceptInfo& info) const override;
  virtual bool accept(const xAOD::TauJet& xTau,
                      asg::AcceptData& accept) override;
private:
  virtual void fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist) override;
};

class SelectionCutBDTJetScore
  : public SelectionCut
{
public:
  SelectionCutBDTJetScore(TauSelectionTool* tTST);
  virtual void setAcceptInfo (asg::AcceptInfo& info) const override;
  virtual bool accept(const xAOD::TauJet& xTau,
                      asg::AcceptData& accept) override;
private:
  virtual void fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist) override;
};

class SelectionCutJetIDWP
  : public SelectionCut
{
public:
  SelectionCutJetIDWP(TauSelectionTool* tTST);
  virtual void setAcceptInfo (asg::AcceptInfo& info) const override;
  virtual bool accept(const xAOD::TauJet& xTau,
                      asg::AcceptData& accept) override;
private:
  virtual void fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist) override;
};

class SelectionCutBDTEleScore
  : public SelectionCut
{
public:
  SelectionCutBDTEleScore(TauSelectionTool* tTST);
  virtual void setAcceptInfo (asg::AcceptInfo& info) const override;
  virtual bool accept(const xAOD::TauJet& xTau,
                      asg::AcceptData& accept) override;
private:
  std::string m_sEleBDTDecorationName;
  virtual void fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist) override;
};

class SelectionCutEleBDTWP
  : public SelectionCut
{
public:
  SelectionCutEleBDTWP(TauSelectionTool* tTST);
  virtual void setAcceptInfo (asg::AcceptInfo& info) const override;
  virtual bool accept(const xAOD::TauJet& xTau,
                      asg::AcceptData& accept) override;
private:
  std::string m_sEleBDTDecorationName;
  virtual void fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist) override;
};

class SelectionCutEleOLR
  : public SelectionCut
{
public:
  SelectionCutEleOLR(TauSelectionTool* tTST);
  virtual ~SelectionCutEleOLR();
  virtual void setAcceptInfo (asg::AcceptInfo& info) const override;
  virtual bool accept(const xAOD::TauJet& xTau,
                      asg::AcceptData& accept) override;

  bool getEvetoPass(const xAOD::TauJet& xTau);
private:
#ifndef XAODTAU_VERSIONS_TAUJET_V3_H
  bool m_bCheckEleMatchPassAvailable;
  bool m_bEleMatchPassAvailable;
#endif // not XAODTAU_VERSIONS_TAUJET_V3_H

  std::string m_sEleOlrPassDecorationName;

  virtual void fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist) override;
  std::string m_sEleOlrLhScoreDecorationName;
};

class SelectionCutMuonVeto
  : public SelectionCut
{
public:
  SelectionCutMuonVeto(TauSelectionTool* tTST);
  virtual void setAcceptInfo (asg::AcceptInfo& info) const override;
  virtual bool accept(const xAOD::TauJet& xTau,
                      asg::AcceptData& accept) override;
private:
  virtual void fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist) override;
};

//added by Li-Gang Xia < ligang.xia@cern.ch >
//to remove taus overlapping with mouns satisfying pt > 2 GeV and not calo-tagged
class SelectionCutMuonOLR
  : public SelectionCut
{
public:
  SelectionCutMuonOLR(TauSelectionTool* tTST);
  virtual void setAcceptInfo (asg::AcceptInfo& info) const override;
  virtual bool accept(const xAOD::TauJet& xTau,
                      asg::AcceptData& accept) override;
private:
  bool m_bTauMuonOLR; //False: overlapped, the tau is not kept. True: not overlapped, the tau is kept.)
  const xAOD::MuonContainer* m_xMuonContainer;
  virtual void fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist) override;
};


}

#endif // TAUANALYSISTOOLS_SELECTIONCUTS_H
