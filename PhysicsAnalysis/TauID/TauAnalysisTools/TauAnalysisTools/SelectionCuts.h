/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUANALYSISTOOLS_SELECTIONCUTS_H
#define TAUANALYSISTOOLS_SELECTIONCUTS_H

/*
  author: Dirk Duschinger
  mail: dirk.duschinger@cern.ch
  documentation in: https://gitlab.cern.ch/atlas/athena/-/blob/master/PhysicsAnalysis/TauID/TauAnalysisTools/doc/README-TauSelectionTool.rst
*/

// Framework include(s):
#include "xAODTau/TauJet.h"
#include "PATCore/AcceptData.h"
#include "PATCore/AcceptInfo.h"

// ROOT include(s):
#include "TH1F.h"

namespace TauAnalysisTools
{

class TauSelectionTool;

class SelectionCut
{

public:
  SelectionCut(const std::string& sName, TauAnalysisTools::TauSelectionTool* tTST);
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
  virtual void fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist) const = 0;
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
  virtual void fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist) const override;
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
  virtual void fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist) const override;
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
  virtual void fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist) const override;
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
  virtual void fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist) const override;
};

class SelectionCutRNNJetScoreSigTrans
  : public SelectionCut
{
public:
  SelectionCutRNNJetScoreSigTrans(TauSelectionTool* tTST);
  virtual void setAcceptInfo (asg::AcceptInfo& info) const override;
  virtual bool accept(const xAOD::TauJet& xTau,
                      asg::AcceptData& accept) override;
private:
  virtual void fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist) const override;
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
  virtual void fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist) const override;
};

class SelectionCutRNNEleScore
  : public SelectionCut
{
public:
  SelectionCutRNNEleScore(TauSelectionTool* tTST);
  virtual void setAcceptInfo (asg::AcceptInfo& info) const override;
  virtual bool accept(const xAOD::TauJet& xTau,
                      asg::AcceptData& accept) override;
private:
  virtual void fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist) const override;
};

class SelectionCutEleIDWP
  : public SelectionCut
{
public:
  SelectionCutEleIDWP(TauSelectionTool* tTST);
  virtual void setAcceptInfo (asg::AcceptInfo& info) const override;
  virtual bool accept(const xAOD::TauJet& xTau,
                      asg::AcceptData& accept) override;
private:
  virtual void fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist) const override;
};

//added by Li-Gang Xia < ligang.xia@cern.ch >
//to remove taus overlapping with muons satisfying pt > 2 GeV and not calo-tagged
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
  virtual void fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist) const override;
};


}

#endif // TAUANALYSISTOOLS_SELECTIONCUTS_H
