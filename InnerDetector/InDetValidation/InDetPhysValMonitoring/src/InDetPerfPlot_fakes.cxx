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
  m_incFakeNum_pt1{},
  m_incFakeNum_pt2{},
  m_incFakeNum_pt5{},
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
  m_incFakeDenomEta_pt1{},
  m_incFakeDenomEta_pt2{},
  m_incFakeDenomEta_pt5{},
  m_fakePtUnlinkedFrac{},
  m_fakeetaUnlinkedFrac{},
  m_fakePhiUnlinkedFrac{},
  m_faked0UnlinkedFrac{},
  m_fakez0UnlinkedFrac{},
  m_incFakeEta_pt1{},
  m_incFakeEta_pt2{},
  m_incFakeEta_pt5{},
  m_nTracks_vs_mu{},
  m_nTruth_vs_mu{},
  m_incTrkRate_vs_mu{},
  m_nTracks_vs_mu2{},
  m_nTruth_vs_mu2{},
  m_incTrkRate_vs_mu2{},
  m_nTracks_vs_mu3{},
  m_nTruth_vs_mu3{},
  m_incTrkRate_vs_mu3{},
  m_mu{} {
  // nop
}

void
InDetPerfPlot_fakes::initializePlots() {
  const bool prependDirectory(false);

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

  m_incFakeNum_pt1 = Book1D("incFakeNum_pt1", "inclusive fake numerator p_{T} > 1 GeV ", 80, -4, 4, prependDirectory);
  m_incFakeNum_pt2 = Book1D("incFakeNum_pt2", "inclusive fake numerator p_{T} > 2 GeV ", 80, -4, 4, prependDirectory);
  m_incFakeNum_pt5 = Book1D("incFakeNum_pt5", "inclusive fake numerator p_{T} > 5 GeV ", 80, -4, 4, prependDirectory);

  book(m_fakePtUnlinkedFrac, "fakePtUnlinkedFrac");
  book(m_fakeetaUnlinkedFrac, "fakeetaUnlinkedFrac");
  book(m_fakePhiUnlinkedFrac, "fakePhiUnlinkedFrac");
  book(m_faked0UnlinkedFrac, "faked0UnlinkedFrac");
  book(m_fakez0UnlinkedFrac, "fakez0UnlinkedFrac");

  m_incFakeDenomEta_pt1 = Book1D("incFakeDenomEta_pt1", "Inclusive FakeRate Denominator vs #eta (p_{T} > 1 GeV)", 80,
                                 -4, 4, prependDirectory);
  m_incFakeDenomEta_pt2 = Book1D("incFakeDenomEta_pt2", "Inclusive FakeRate Denominator vs #eta (p_{T} > 2 GeV)", 80,
                                 -4, 4, prependDirectory);
  m_incFakeDenomEta_pt5 = Book1D("incFakeDenomEta_pt5", "Inclusive FakeRate Denominator vs #eta (p_{T} > 5 GeV)", 80,
                                 -4, 4, prependDirectory);
  m_incFakeEta_pt1 =
    Book1D("incFakeEta_pt1", "Inclusive FakeRate vs #eta (p_{T} > 1 GeV)", 80, -4, 4, prependDirectory);
  m_incFakeEta_pt2 =
    Book1D("incFakeEta_pt2", "Inclusive FakeRate vs #eta (p_{T} > 2 GeV)", 80, -4, 4, prependDirectory);
  m_incFakeEta_pt5 =
    Book1D("incFakeEta_pt5", "Inclusive FakeRate vs #eta (p_{T} > 5 GeV)", 80, -4, 4, prependDirectory);

  m_nTracks_vs_mu = Book1D("nTracks_vs_mu", " # tracks vs pileupEvents (#eta < 2.7)", 20, 150, 250, prependDirectory);
  m_nTruth_vs_mu = Book1D("nTruth_vs_mu", " # truth vs pileupEvents (#eta < 2.7)", 20, 150, 250, prependDirectory);
  m_incTrkRate_vs_mu =
    Book1D("incTrk_vs_mu", " inclusive track rate vs mu (#eta < 2.7)", 20, 150, 250, prependDirectory);
  m_nTracks_vs_mu2 = Book1D("nTracks_vs_mu2", " # tracks vs pileupEvents (#eta > 2.8, #eta < 3.5)", 20, 150, 250,
                            prependDirectory);
  m_nTruth_vs_mu2 = Book1D("nTruth_vs_mu2", " # truth vs pileupEvents (#eta > 2.8, #eta < 3.5)", 20, 150, 250,
                           prependDirectory);
  m_incTrkRate_vs_mu2 = Book1D("incTrk_vs_mu2", " inclusive track rate vs mu (#eta > 2.8, #eta < 3.5)", 20, 150, 250,
                               prependDirectory);
  m_nTracks_vs_mu3 = Book1D("nTracks_vs_mu3", " # tracks vs pileupEvents (#eta > 3.5)", 20, 150, 250, prependDirectory);
  m_nTruth_vs_mu3 = Book1D("nTruth_vs_mu3", " # truth vs pileupEvents (#eta > 3.5)", 20, 150, 250, prependDirectory);
  m_incTrkRate_vs_mu3 = Book1D("incTrk_vs_mu3", " inclusive track rate vs mu (#eta > 3.5)", 20, 150, 250,
                               prependDirectory);
  m_mu = Book1D("mu", " <mu> ", 10, 150, 250, prependDirectory);
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

  fillHisto(m_fakepT, pt);
  if (isFake) {
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
InDetPerfPlot_fakes::fillLinkedandUnlinked(const xAOD::TrackParticle& trkprt, float Prim_w, float Sec_w,
                                           float Unlinked_w) {
  double pt = trkprt.pt() * 1_GeV;
  double eta(trkprt.eta());
  double phi(trkprt.phi());
  double d0(trkprt.d0());
  double z0(trkprt.z0());

  if (pt > 1) {
    fillHisto(m_incFakeNum_pt1, eta);
  }
  if (pt > 2) {
    fillHisto(m_incFakeNum_pt2, eta);
  }
  if (pt > 5) {
    fillHisto(m_incFakeNum_pt5, eta);
  }

  if (Unlinked_w == 0) {
    fillHisto(m_fakeEtaTotal, eta, Prim_w + Sec_w);
  }

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

  fillHisto(m_fakePtUnlinkedFrac, pt, Unlinked_w);
  fillHisto(m_fakeetaUnlinkedFrac, eta, Unlinked_w);
  fillHisto(m_fakePhiUnlinkedFrac, phi, Unlinked_w);
  fillHisto(m_faked0UnlinkedFrac, d0, Unlinked_w);
  fillHisto(m_fakez0UnlinkedFrac, z0, Unlinked_w);
}

void
InDetPerfPlot_fakes::fillIncFakeDenom(const xAOD::TruthParticle& particle) {
  double eta = particle.eta();
  double pt = particle.pt() * 1_GeV;

  if (pt > 1) {
    fillHisto(m_incFakeDenomEta_pt1, eta);
  }
  if (pt > 2) {
    fillHisto(m_incFakeDenomEta_pt2, eta);
  }
  if (pt > 5) {
    fillHisto(m_incFakeDenomEta_pt5, eta);
  }
}

void
InDetPerfPlot_fakes::fillIncTrkRate(const unsigned int nMuEvents, std::vector<int> incTrkNum,
                                    std::vector<int> incTrkDenom) {
  fillHisto(m_nTracks_vs_mu, nMuEvents, incTrkNum[0]);
  fillHisto(m_nTruth_vs_mu, nMuEvents, incTrkDenom[0]);
  fillHisto(m_nTracks_vs_mu2, nMuEvents, incTrkNum[1]);
  fillHisto(m_nTruth_vs_mu2, nMuEvents, incTrkDenom[1]);
  fillHisto(m_nTracks_vs_mu3, nMuEvents, incTrkNum[2]);
  fillHisto(m_nTruth_vs_mu3, nMuEvents, incTrkDenom[2]);
  fillHisto(m_mu, nMuEvents);
}

void
InDetPerfPlot_fakes::finalizePlots() {
  if (m_incFakeEta_pt1 && m_incFakeEta_pt2 && m_incFakeEta_pt5 && m_incTrkRate_vs_mu && m_incTrkRate_vs_mu2 &&
      m_incTrkRate_vs_mu3) {
    m_incFakeEta_pt1->Sumw2();
    m_incFakeEta_pt1->Divide(m_incFakeNum_pt1, m_incFakeDenomEta_pt1, 1, 1, "B");
    m_incFakeEta_pt2->Sumw2();
    m_incFakeEta_pt2->Divide(m_incFakeNum_pt2, m_incFakeDenomEta_pt2, 1, 1, "B");
    m_incFakeEta_pt5->Sumw2();
    m_incFakeEta_pt5->Divide(m_incFakeNum_pt5, m_incFakeDenomEta_pt5, 1, 1, "B");

    m_incTrkRate_vs_mu->Divide(m_nTracks_vs_mu, m_nTruth_vs_mu, 1, 1, "B");
    m_incTrkRate_vs_mu2->Divide(m_nTracks_vs_mu2, m_nTruth_vs_mu2, 1, 1, "B");
    m_incTrkRate_vs_mu3->Divide(m_nTracks_vs_mu3, m_nTruth_vs_mu3, 1, 1, "B");
  } else {
    ATH_MSG_INFO(
      "InDetPerfPlot_fakes: some plots have null pointer, probably were not fully specified in the histogram definition xml file");
  }

  static const int nPlot = 6;
  TH1* incTrkNum[nPlot] = {
    m_nTracks_vs_mu, m_nTracks_vs_mu2, m_nTracks_vs_mu3, m_incFakeNum_pt1, m_incFakeNum_pt2, m_incFakeNum_pt5
  };
  TH1* incTrkDenom[nPlot] = {
    m_nTruth_vs_mu, m_nTruth_vs_mu2, m_nTruth_vs_mu3, m_incFakeDenomEta_pt1, m_incFakeDenomEta_pt2,
    m_incFakeDenomEta_pt5
  };
  TH1* incTrk[nPlot] = {
    m_incTrkRate_vs_mu, m_incTrkRate_vs_mu2, m_incTrkRate_vs_mu3, m_incFakeEta_pt1, m_incFakeEta_pt2, m_incFakeEta_pt5
  };

  for (int j = 0; j < nPlot; j++) {
    int nBins = incTrkDenom[j]->GetNbinsX();
    for (int i = 0; i < nBins; i++) {
      double binError = 0;
      double yBC = incTrkDenom[j]->GetBinContent(i);
      double xBC = incTrkNum[j]->GetBinContent(i);
      incTrkDenom[j]->SetBinError(i, std::sqrt(yBC));
      incTrkNum[j]->SetBinError(i, std::sqrt(xBC));
      binError = std::sqrt((xBC * xBC / (yBC * yBC * yBC)) + (xBC) / (yBC * yBC));
      if (binError > 0) {
        incTrk[j]->SetBinError(i, binError);
      }
    }
  }
}
