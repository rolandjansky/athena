/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPerfPlot_fakes.cxx
 * @author shaun roe
 **/

#include "InDetPerfPlot_fakes.h"
#include <cmath> // for std::sqrt
#include "InDetPhysValMonitoringUtilities.h"
using namespace IDPVM;

InDetPerfPlot_fakes::InDetPerfPlot_fakes(InDetPlotBase* pParent, const std::string& sDir) :
  InDetPlotBase(pParent, sDir),
  m_fakepT{},
  m_fakePtLow{},
  m_fakephi{},
  m_fakeeta{},
  m_faked0{},
  m_fakez0{},
  m_track_fakerate_vs_pt{},
  m_track_fakerate_vs_eta{},
  m_track_fakerate_vs_phi{},
  m_track_fakerate_vs_d0{},
  m_track_fakerate_vs_z0{},
  m_fakeEtaTotal{},
  m_fakePtPrimary{},
  m_fakeetaPrimary{},
  m_fakePhiPrimary{},
  m_faked0Primary{},
  m_fakez0Primary{},
  m_fakePtSecondary{},
  m_fakeetaSecondary{},
  m_fakePhiSecondary{},
  m_faked0Secondary{},
  m_fakez0Secondary{},
  m_fakePtUnlinkedFrac{},
  m_fakeetaUnlinkedFrac{},
  m_fakePhiUnlinkedFrac{},
  m_faked0UnlinkedFrac{},
  m_fakez0UnlinkedFrac{},
  m_incTrkRatePtcutVals{1.,2.,5.},
  m_incTrkRatePtcutSuff{"1","2","5"},
  m_nRecTrkVsEtaPtcut{nullptr},
  m_nTruthTrkVsEtaPtcut{nullptr},
  m_incTrkRateVsEtaPtcut{nullptr},
  m_incTrkRateEtacutVals{2.7,3.5},
  m_incTrkRateEtacutSuff{"lt2p7", "gt2p7_lt3p5", "gt3p5", "Total"},
  m_nRecTrkVsMuEtacut{nullptr},
  m_nTruthTrkVsMuEtacut{nullptr},
  m_incTrkRateVsMuEtacut{nullptr},
  m_mu{} {
  // nop
}



void
InDetPerfPlot_fakes::initializePlots() {
 
  book(m_fakepT, "fakepT");
  book(m_fakePtLow, "fakepTlow");
  book(m_fakephi, "fakephi");
  book(m_fakeeta, "fakeeta");
  book(m_faked0, "faked0");
  book(m_fakez0, "fakez0");
  //
  book(m_track_fakerate_vs_eta, "track_fakerate_vs_eta");
  book(m_track_fakerate_vs_pt, "track_fakerate_vs_pt");
  book(m_track_fakerate_vs_phi, "track_fakerate_vs_phi");
  book(m_track_fakerate_vs_d0, "track_fakerate_vs_d0");
  book(m_track_fakerate_vs_z0, "track_fakerate_vs_z0");

  book(m_fakeEtaTotal, "fakeEtaTotal");
  book(m_fakePtTotal , "fakePtTotal");
  
  book(m_fakePtPrimary, "fakePtPrimary");
  book(m_fakeetaPrimary, "fakeetaPrimary");
  book(m_fakePhiPrimary, "fakePhiPrimary");
  book(m_faked0Primary, "faked0Primary");
  book(m_fakez0Primary, "fakez0Primary");

  book(m_fakePtSecondary, "fakePtSecondary");
  book(m_fakeetaSecondary, "fakeetaSecondary");
  book(m_fakePhiSecondary, "fakePhiSecondary");
  book(m_faked0Secondary, "faked0Secondary");
  book(m_fakez0Secondary, "fakez0Secondary");

  book(m_fakePtUnlinkedFrac, "fakePtUnlinkedFrac");
  book(m_fakeetaUnlinkedFrac, "fakeetaUnlinkedFrac");
  book(m_fakePhiUnlinkedFrac, "fakePhiUnlinkedFrac");
  book(m_faked0UnlinkedFrac, "faked0UnlinkedFrac");
  book(m_fakez0UnlinkedFrac, "fakez0UnlinkedFrac");

  book(m_mu,"mu");

  // inclusive track rates:
  std::string nTruth("nTruthTrk");
  std::string nRec("nRecTrk");
  std::string iRate("incTrkRate");
  std::string ptsuff="_vs_eta_pt";
  std::string musuff="_vs_mu_eta";
  // vs eta, for pt cut
  for (unsigned int i=0; i<m_nPtHists;++i) {
    std::string cutsuff=ptsuff+m_incTrkRatePtcutSuff[i];
    book(m_nRecTrkVsEtaPtcut[i],nRec+cutsuff);
    book(m_nTruthTrkVsEtaPtcut[i],nTruth+cutsuff);
    book(m_incTrkRateVsEtaPtcut[i],iRate+cutsuff);
  }
  // vs mu, for eta cut
  for (unsigned int i=0; i<m_nEtaHists;++i) {
    std::string cutsuff=musuff+m_incTrkRateEtacutSuff[i];
    book(m_nRecTrkVsMuEtacut[i],nRec+cutsuff);
    book(m_nTruthTrkVsMuEtacut[i],nTruth+cutsuff);
    book(m_incTrkRateVsMuEtacut[i],iRate+cutsuff);
  }
  
}

void
InDetPerfPlot_fakes::fill(const xAOD::TrackParticle& trkprt, const bool isFake, const Category /*f*/) {
  const unsigned int fakeNum = (unsigned int) isFake;
  // constexpr double degreesPerRadian(180./M_PI);
  double pt = trkprt.pt() * 1_GeV;
  double eta(trkprt.eta());
  double phi(trkprt.phi());
  // double phi_degrees(phi * degreesPerRadian);
  double d0(trkprt.d0());
  double z0(trkprt.z0());

  if (isFake) {
    fillHisto(m_fakepT, pt);
    fillHisto(m_fakePtLow, pt);
    fillHisto(m_fakeeta, eta);
    fillHisto(m_fakephi, phi);
    fillHisto(m_faked0, d0);
    fillHisto(m_fakez0, z0);
  }
  fillHisto(m_track_fakerate_vs_eta, eta, fakeNum);
  fillHisto(m_track_fakerate_vs_pt, pt, fakeNum);
  fillHisto(m_track_fakerate_vs_phi, phi, fakeNum);
  fillHisto(m_track_fakerate_vs_d0, d0, fakeNum);
  fillHisto(m_track_fakerate_vs_z0, z0, fakeNum);
}

void
InDetPerfPlot_fakes::fillIncTrkRateVsEtaPtcut(std::array<TH1*, m_nPtHists>& hists, double trkpt, double trketa) {
  if (hists.size()!=m_nPtCuts)
    return;
  for (unsigned int i=0; i<m_nPtCuts; ++i) {
    // pt cuts are in inclusive intervals
    if (trkpt>m_incTrkRatePtcutVals[i]) {
      fillHisto(hists[i], trketa);
    }   
  }
}

void
InDetPerfPlot_fakes::fillIncTrkRateVsMuEtacut(std::array<TH1*, m_nEtaHists>& hists, double trketa, unsigned int mu) {
  double trkabseta=std::abs(trketa);
  // eta cuts are in exclusive intervals
  if (trkabseta<m_incTrkRateEtacutVals[0])
    fillHisto(hists[0], mu);
  else if (trkabseta>m_incTrkRateEtacutVals[m_nEtaCuts-1])
    fillHisto(hists[m_nEtaHists-2], mu);
  else {
    for (unsigned int i=1; i<m_nEtaCuts; ++i) {
      if (trkabseta>m_incTrkRateEtacutVals[i-1] && trkabseta<m_incTrkRateEtacutVals[i]) {
	fillHisto(hists[i], mu);
      }
    }   
  }
  // last: total histogram
  fillHisto(hists[m_nEtaHists-1],mu);
}

void
InDetPerfPlot_fakes::fillLinkedandUnlinked(const xAOD::TrackParticle& trkprt, float Prim_w, float Sec_w,
                                           float Unlinked_w,unsigned int mu) {
  double pt = trkprt.pt() * 1_GeV;
  double eta(trkprt.eta());
  double phi(trkprt.phi());
  double d0(trkprt.d0());
  double z0(trkprt.z0());

  fillIncTrkRateVsEtaPtcut(m_nRecTrkVsEtaPtcut,pt,eta);
  fillIncTrkRateVsMuEtacut(m_nRecTrkVsMuEtacut,eta,mu);

  if (Unlinked_w == 0) {
    fillHisto(m_fakeEtaTotal, eta, Prim_w + Sec_w);
    fillHisto(m_fakePtTotal, pt, Prim_w + Sec_w);
    
    fillHisto(m_fakePtPrimary, pt, Prim_w);
    fillHisto(m_fakeetaPrimary, eta, Prim_w);
    fillHisto(m_fakePhiPrimary, phi, Prim_w);
    fillHisto(m_faked0Primary, d0, Prim_w);
    fillHisto(m_fakez0Primary, z0, Prim_w);
    
    fillHisto(m_fakePtSecondary, pt, Sec_w);
    fillHisto(m_fakeetaSecondary, eta, Sec_w);
    fillHisto(m_fakePhiSecondary, phi, Sec_w);
    fillHisto(m_faked0Secondary, d0, Sec_w);
    fillHisto(m_fakez0Secondary, z0, Sec_w);
  }

  fillHisto(m_fakePtUnlinkedFrac, pt, Unlinked_w);
  fillHisto(m_fakeetaUnlinkedFrac, eta, Unlinked_w);
  fillHisto(m_fakePhiUnlinkedFrac, phi, Unlinked_w);
  fillHisto(m_faked0UnlinkedFrac, d0, Unlinked_w);
  fillHisto(m_fakez0UnlinkedFrac, z0, Unlinked_w);
}

void
InDetPerfPlot_fakes::fillIncFakeDenom(const xAOD::TruthParticle& particle, unsigned int mu) {
  double eta = particle.eta();
  double pt = particle.pt() * 1_GeV;
  fillIncTrkRateVsEtaPtcut(m_nTruthTrkVsEtaPtcut,pt,eta);
  fillIncTrkRateVsMuEtacut(m_nTruthTrkVsMuEtacut,eta,mu);
}

void InDetPerfPlot_fakes::setPlotErrors(TH1* hrat, TH1* hnum, TH1* hdenom) {

  if (!(hrat && hnum && hdenom))
    return;

  unsigned int nbins=hrat->GetNbinsX();
  for (unsigned int i = 0; i <nbins+1; i++) {
    double binError = 0;
    double yBC = hdenom->GetBinContent(i);
    double xBC = hnum->GetBinContent(i);
    hdenom->SetBinError(i, std::sqrt(yBC));
    hnum->SetBinError(i, std::sqrt(xBC));
    binError = std::sqrt((xBC * xBC / (yBC * yBC * yBC)) + (xBC) / (yBC * yBC));
    if (binError > 0) {
      hrat->SetBinError(i, binError);
    } 
  }
  
}

void
InDetPerfPlot_fakes::finalizePlots() {

  for (unsigned int i=0; i<m_incTrkRateVsEtaPtcut.size(); ++i) {
    if (m_incTrkRateVsEtaPtcut[i]&&m_nRecTrkVsEtaPtcut[i]&&m_nTruthTrkVsEtaPtcut[i]) {
      if (0==m_incTrkRateVsEtaPtcut[i]->GetSumw2N())
	m_incTrkRateVsEtaPtcut[i]->Sumw2();
      m_incTrkRateVsEtaPtcut[i]->Divide(m_nRecTrkVsEtaPtcut[i], m_nTruthTrkVsEtaPtcut[i], 1, 1, "B");
      setPlotErrors(m_incTrkRateVsEtaPtcut[i],m_nRecTrkVsEtaPtcut[i],m_nTruthTrkVsEtaPtcut[i]);
    }
  }

  for (unsigned int i=0; i<m_incTrkRateVsMuEtacut.size(); ++i) {
    if (m_incTrkRateVsMuEtacut[i]&&m_nRecTrkVsMuEtacut[i]&&m_nTruthTrkVsMuEtacut[i]) {
      if (0==m_incTrkRateVsMuEtacut[i]->GetSumw2N())
	m_incTrkRateVsMuEtacut[i]->Sumw2();
      m_incTrkRateVsMuEtacut[i]->Divide(m_nRecTrkVsMuEtacut[i], m_nTruthTrkVsMuEtacut[i], 1, 1, "B");
      setPlotErrors(m_incTrkRateVsMuEtacut[i],m_nRecTrkVsMuEtacut[i],m_nTruthTrkVsMuEtacut[i]);
    }
  }  
}
