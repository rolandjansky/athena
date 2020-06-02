/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DiMuMon_H
#define DiMuMon_H

#include <vector>
#include <map>

#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODMuon/MuonContainer.h"


class TH1F;
class TH2F;


class DiMuMon : public ManagedMonitorToolBase
{
 public:

  DiMuMon( const std::string & type, const std::string & name,
	   const IInterface* parent );

  virtual ~DiMuMon();

  virtual StatusCode initialize();
  virtual StatusCode bookHistograms();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();


  template <class T> void RegisterHisto(MonGroup& mon, T* histo);
  double getInvmass(const xAOD::TrackParticle* track1, const xAOD::TrackParticle* track2, double Mass) const;
  double getCrtDiff(const xAOD::TrackParticle* id1, const xAOD::TrackParticle* id2 ) const;
  double getEta(const xAOD::TrackParticle* id1, const xAOD::TrackParticle* id2 ) const;
  double getPhi(const xAOD::TrackParticle* id1, const xAOD::TrackParticle* id2 ) const;
  double getPt(const xAOD::TrackParticle* id1, const xAOD::TrackParticle* id2 ) const;
  void iterativeGausFit(TH2F* hin, std::vector<TH1F*> hout, int mode);
  bool trackQuality(const xAOD::TrackParticle *idTrk);


 protected:

  int m_lumiBlockNum{};

  int m_nMassBins{};
  int m_nVarBins{};
  int m_nPtBins{};
  int m_nEtaBins{};
  int m_nPhiBins{};

  double m_coneSize{};
  double m_isolationCut{};
  double m_minInvmass{};
  double m_maxInvmass{};
  double m_ptCut{};
  double m_etaCut{};

  std::string m_resonName{};
  std::string m_triggerChainName{};
  SG::ReadHandleKey<xAOD::MuonContainer> m_muonCollection{this, "muonCollection", "Muons"};

  bool m_doFits{};
  bool m_doSaveFits{};
  bool m_setDebug{};


  std::vector< std::string > m_regions;
  std::vector< std::string > m_varsVSmean;
  std::vector< std::string > m_varsVSwidth;
  std::vector< std::string > m_varsDistr;
  std::map< std::string, std::pair <double, double> > m_varRanges;
  std::map< std::string, double > m_varValues;
  std::map< std::string, std::string > m_varLabels;

  TH1F* m_chi2{};
  TH1F* m_stat{};
  std::map< std::string, TH1F* > m_invmass;
  std::map< std::string, std::map< std::string, TH2F*> > m_2DinvmassVSx;
  std::map< std::string, std::map< std::string, TH1F*> > m_invmassVSx;
  std::map< std::string, std::map< std::string, TH1F*> > m_widthVSx;
  std::map< std::string, std::map< std::string, TH1F*> > m_xDistr;

};

#endif
