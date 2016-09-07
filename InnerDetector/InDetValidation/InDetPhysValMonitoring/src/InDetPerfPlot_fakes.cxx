/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPerfPlot_fakes.cxx
 * @author shaun roe
 **/

#include "InDetPerfPlot_fakes.h"
// #include <cmath> //for M_PI


InDetPerfPlot_fakes::InDetPerfPlot_fakes(InDetPlotBase *pParent, const std::string &sDir) :
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
  m_incFakevsTracks{},
  m_selectedTracks_vs_nTracks{},
  m_fakePtNumPrimary{},
  m_fakephiNumPrimary{},
  m_fakeetaNumPrimary{},
  m_faked0NumPrimary{},
  m_fakez0NumPrimary{},
  m_fakePtNumSecondary{},
  m_fakephiNumSecondary{},
  m_fakeetaNumSecondary{},
  m_faked0NumSecondary{},
  m_fakez0NumSecondary{},
  m_fakePtDenom{},
  m_fakephiDenom{},
  m_fakeetaDenom{},
  m_faked0Denom{},
  m_fakez0Denom{},
  m_fakePtUnlinked{},
  m_fakephiUnlinked{},
  m_fakeetaUnlinked{},
  m_faked0Unlinked{},
  m_fakez0Unlinked{},

  m_fakePtPrimary{},
  m_fakephiPrimary{},
  m_fakeetaPrimary{},
  m_faked0Primary{},
  m_fakez0Primary{},
  m_fakePtSecondary{},
  m_fakephiSecondary{},
  m_fakeetaSecondary{},
  m_faked0Secondary{},
  m_fakez0Secondary{},
  m_fakePtUnlinkedFrac{},
  m_fakephiUnlinkedFrac{},
  m_fakeetaUnlinkedFrac{},
  m_faked0UnlinkedFrac{},
  m_fakez0UnlinkedFrac{},

  m_incFakeDenomEta{},
  m_incFakeDenomPt{},
  m_incFakeEta{},
  m_incFakePt{} {
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

  book(m_incFakevsTracks, "incFakevsTracks");
  book(m_selectedTracks_vs_nTracks, "selectedTracks_vs_nTracks");

  m_fakePtUnlinked = Book1D("fakePtUnlinked", "fake Pt Unlinked", 400, 0, 200, prependDirectory);
  m_fakephiUnlinked = Book1D("fakephiUnlinked", "fake Phi Unlinked", 60, -3.1415926, 3.1415926, prependDirectory);
  m_fakeetaUnlinked = Book1D("fakeetaUnlinked", "fake Eta Unlinked", 80, -4, 4, prependDirectory);
  m_faked0Unlinked = Book1D("faked0Unlinked", "fake D0 Unlinked", 60, -1.5, 1.5, prependDirectory);
  m_fakez0Unlinked = Book1D("fakez0Unlinked", "fake z0 Unlinked", 50, -200, 200, prependDirectory);

  m_fakePtNumPrimary = Book1D("fakePtNumPrimary", "fake Pt Num Linked (Primary)", 400, 0, 200, prependDirectory);
  m_fakephiNumPrimary = Book1D("fakephiNumPrimary", "fake Phi Num Linked (Primary)", 60, -3.1415926, 3.1415926,
                               prependDirectory);
  m_fakeetaNumPrimary = Book1D("fakeetaNumPrimary", "fake Eta Num Linked (Primary)", 80, -4, 4, prependDirectory);
  m_faked0NumPrimary = Book1D("faked0NumPrimary", "fake D0 Num Linked (Primary)", 60, -1.5, 1.5, prependDirectory);
  m_fakez0NumPrimary = Book1D("fakez0NumPrimary", "fake z0 Num Linked (Primary)", 50, -200, 200, prependDirectory);

  m_fakePtNumSecondary = Book1D("fakePtNumSecondary", "fake Pt Num Linked (Secondary)", 400, 0, 200, prependDirectory);
  m_fakephiNumSecondary = Book1D("fakephiNumSecondary", "fake Phi Num Linked (Secondary)", 60, -3.1415926, 3.1415926,
                                 prependDirectory);
  m_fakeetaNumSecondary = Book1D("fakeetaNumSecondary", "fake Eta Num Linked (Secondary)", 80, -4, 4, prependDirectory);
  m_faked0NumSecondary =
    Book1D("faked0NumSecondary", "fake D0 Num Linked (Secondary)", 60, -1.5, 1.5, prependDirectory);
  m_fakez0NumSecondary =
    Book1D("fakez0NumSecondary", "fake z0 Num Linked (Secondary)", 50, -200, 200, prependDirectory);

  m_fakePtDenom = Book1D("fakePtDenom", "fake Pt Denom ", 400, 0, 200, prependDirectory);
  m_fakephiDenom = Book1D("fakephiDenom", "fake Phi Denom ", 60, -3.1415926, 3.1415926, prependDirectory);
  m_fakeetaDenom = Book1D("fakeetaDenom", "fake Eta Denom ", 80, -4, 4, prependDirectory);
  m_faked0Denom = Book1D("faked0Denom", "fake D0 Denom ", 60, -1.5, 1.5, prependDirectory);
  m_fakez0Denom = Book1D("fakez0Denom", "fake z0 Denom ", 50, -200, 200, prependDirectory);

  m_fakePtPrimary = Book1D("fakePtPrimary", "fake Pt  Linked (Primary)", 400, 0, 200, prependDirectory);
  m_fakephiPrimary = Book1D("fakephiPrimary", "fake Phi Linked (Primary)", 60, -3.1415926, 3.1415926, prependDirectory);
  m_fakeetaPrimary = Book1D("fakeetaPrimary", "fake Eta Linked (Primary)", 80, -4, 4, prependDirectory);
  m_faked0Primary = Book1D("faked0Primary", "fake D0 Linked (Primary)", 60, -1.5, 1.5, prependDirectory);
  m_fakez0Primary = Book1D("fakez0Primary", "fake z0  Linked (Primary)", 50, -200, 200, prependDirectory);

  m_fakePtSecondary = Book1D("fakePtSecondary", "fake Pt  Linked (Secondary)", 400, 0, 200, prependDirectory);
  m_fakephiSecondary = Book1D("fakephiSecondary", "fake Phi Linked (Secondary)", 60, -3.1415926, 3.1415926,
                              prependDirectory);
  m_fakeetaSecondary = Book1D("fakeetaSecondary", "fake Eta Linked (Secondary)", 80, -4, 4, prependDirectory);
  m_faked0Secondary = Book1D("faked0Secondary", "fake D0 Linked (Secondary)", 60, -1.5, 1.5, prependDirectory);
  m_fakez0Secondary = Book1D("fakez0Secondary", "fake z0  Linked (Secondary)", 50, -200, 200, prependDirectory);

  m_fakePtUnlinkedFrac = Book1D("fakePtUnlinkedFrac", "Pt Unlinked Fraction", 400, 0, 200, prependDirectory);
  m_fakephiUnlinkedFrac = Book1D("fakephiUnlinkedFrac", "Phi Unlinked Fraction", 60, -3.1415926, 3.1415926,
                                 prependDirectory);
  m_fakeetaUnlinkedFrac = Book1D("fakeetaUnlinkedFrac", "Eta Unlinked Fraction", 80, -4, 4, prependDirectory);
  m_faked0UnlinkedFrac = Book1D("faked0UnlinkedFrac", "D0 Unlinked Fraction", 60, -1.5, 1.5, prependDirectory);
  m_fakez0UnlinkedFrac = Book1D("fakez0UnlinkedFrac", "z0 Unlinked Fraction", 50, -200, 200, prependDirectory);

  m_incFakeDenomEta = Book1D("incFakeDenomEta", "Inclusive FakeRate Denominator (eta)", 80, -4, 4, prependDirectory);
  m_incFakeDenomPt = Book1D("incFakeDenomPt", "Inclusive FakeRate Denominator (pt)", 400, 0, 200, prependDirectory);
  m_incFakeEta = Book1D("incFakeEta", "Inclusive FakeRate Denominator (eta)", 80, -4, 4, prependDirectory);
  m_incFakePt = Book1D("incFakePt", "Inclusive FakeRate Denominator (pt)", 400, 0, 200, prependDirectory);
}

void
InDetPerfPlot_fakes::fill(const xAOD::TrackParticle &trkprt, const bool isFake, const Category /*f*/) {
  const unsigned int fakeNum = (unsigned int) isFake;
  // constexpr double degreesPerRadian(180./M_PI);
  double pt = trkprt.pt() * 0.001;
  double eta(trkprt.eta());
  double phi(trkprt.phi());
  // double phi_degrees(phi * degreesPerRadian);
  double d0(trkprt.d0());
  double z0(trkprt.z0());

  fillHisto(m_fakepT,pt);
  if (isFake) {
    fillHisto(m_fakePtLow,pt);
    fillHisto(m_fakeeta,eta);
    fillHisto(m_fakephi,phi);
    fillHisto(m_faked0,d0);
    fillHisto(m_fakez0,z0);
  }
  fillHisto(m_track_fakerate_vs_eta,eta, fakeNum);
  fillHisto(m_track_fakerate_vs_pt,pt, fakeNum);
  fillHisto(m_track_fakerate_vs_phi,phi, fakeNum);
  fillHisto(m_track_fakerate_vs_d0,d0, fakeNum);
  fillHisto(m_track_fakerate_vs_z0,z0, fakeNum);
}

void
InDetPerfPlot_fakes::fillLinked(const xAOD::TrackParticle &trkprt, const xAOD::TruthParticle &particle, double prob) {
  double pt = trkprt.pt() / 1000.;
  double eta(trkprt.eta());
  double phi(trkprt.phi());
  double d0(trkprt.d0());
  double z0(trkprt.z0());

  fillHisto(m_fakePtDenom,pt);
  fillHisto(m_fakeetaDenom,eta);
  fillHisto(m_fakephiDenom,phi);
  fillHisto(m_faked0Denom,d0);
  fillHisto(m_fakez0Denom,z0);

  if (prob < .5 && particle.barcode() < 200000) {
    fillHisto(m_fakePtNumPrimary,pt);
    fillHisto(m_fakeetaNumPrimary,eta);
    fillHisto(m_fakephiNumPrimary,phi);
    fillHisto(m_faked0NumPrimary,d0);
    fillHisto(m_fakez0NumPrimary,z0);
  }
  if (prob < .5 && particle.barcode() > 200000) {
    fillHisto(m_fakePtNumSecondary,pt);
    fillHisto(m_fakeetaNumSecondary,eta);
    fillHisto(m_fakephiNumSecondary,phi);
    fillHisto(m_faked0NumSecondary,d0);
    fillHisto(m_fakez0NumSecondary,z0);
  }
  
}

void
InDetPerfPlot_fakes::fillUnlinked(const xAOD::TrackParticle &trkprt, double /* unused prob*/) {
  double pt = trkprt.pt() / 1000.;
  double eta(trkprt.eta());
  double phi(trkprt.phi());
  double d0(trkprt.d0());
  double z0(trkprt.z0());

  fillHisto(m_fakePtDenom,pt);
  fillHisto(m_fakeetaDenom,eta);
  fillHisto(m_fakephiDenom,phi);
  fillHisto(m_faked0Denom,d0);
  fillHisto(m_fakez0Denom,z0);


  fillHisto(m_fakePtUnlinked,pt);
  fillHisto(m_fakeetaUnlinked,eta);
  fillHisto(m_fakephiUnlinked,phi);
  fillHisto(m_faked0Unlinked,d0);
  fillHisto(m_fakez0Unlinked,z0);
}

void
InDetPerfPlot_fakes::fillIncFakeDenom(const xAOD::TruthParticle &particle) {
  double eta = particle.eta();
  double pt = particle.pt() / 1000;

  fillHisto(m_incFakeDenomEta,eta);
  fillHisto(m_incFakeDenomPt,pt);
}

void
InDetPerfPlot_fakes::fillIncFake(int nTracks, double ifr, int nSelected) {
  fillHisto(m_incFakevsTracks,nTracks, ifr);
  fillHisto(m_selectedTracks_vs_nTracks,nTracks, nSelected);
}

void
InDetPerfPlot_fakes::finalizePlots() {
  m_fakePtPrimary->Sumw2();
  m_fakePtPrimary->Divide(m_fakePtNumPrimary, m_fakePtDenom, 1, 1, "B");
  m_fakePtPrimary->SetXTitle(" Pt (2 GeV bins)");
  m_fakeetaPrimary->Sumw2();
  m_fakeetaPrimary->Divide(m_fakeetaNumPrimary, m_fakeetaDenom, 1, 1, "B");
  m_fakephiPrimary->Sumw2();
  m_fakephiPrimary->Divide(m_fakephiNumPrimary, m_fakephiDenom, 1, 1, "B");
  m_faked0Primary->Sumw2();
  m_faked0Primary->Divide(m_faked0NumPrimary, m_faked0Denom, 1, 1, "B");
  m_fakez0Primary->Sumw2();
  m_fakez0Primary->Divide(m_fakez0NumPrimary, m_fakez0Denom, 1, 1, "B");

  m_fakePtSecondary->Sumw2();
  m_fakePtSecondary->Divide(m_fakePtNumSecondary, m_fakePtDenom, 1, 1, "B");
  m_fakeetaSecondary->Sumw2();
  m_fakeetaSecondary->Divide(m_fakeetaNumSecondary, m_fakeetaDenom, 1, 1, "B");
  m_fakephiSecondary->Sumw2();
  m_fakephiSecondary->Divide(m_fakephiNumSecondary, m_fakephiDenom, 1, 1, "B");
  m_faked0Secondary->Sumw2();
  m_faked0Secondary->Divide(m_faked0NumSecondary, m_faked0Denom, 1, 1, "B");
  m_fakez0Secondary->Sumw2();
  m_fakez0Secondary->Divide(m_fakez0NumSecondary, m_fakez0Denom, 1, 1, "B");

  m_fakePtUnlinkedFrac->Sumw2();
  m_fakePtUnlinkedFrac->Divide(m_fakePtUnlinked, m_fakePtDenom, 1, 1, "B");
  m_fakeetaUnlinkedFrac->Sumw2();
  m_fakeetaUnlinkedFrac->Divide(m_fakeetaUnlinked, m_fakeetaDenom, 1, 1, "B");
  m_fakephiUnlinkedFrac->Sumw2();
  m_fakephiUnlinkedFrac->Divide(m_fakephiUnlinked, m_fakephiDenom, 1, 1, "B");
  m_faked0UnlinkedFrac->Sumw2();
  m_faked0UnlinkedFrac->Divide(m_faked0Unlinked, m_faked0Denom, 1, 1, "B");
  m_fakez0UnlinkedFrac->Sumw2();
  m_fakez0UnlinkedFrac->Divide(m_fakez0Unlinked, m_fakez0Denom, 1, 1, "B");

  m_incFakeEta->Sumw2();
  m_incFakeEta->Divide(m_fakeetaDenom, m_incFakeDenomEta);
  m_incFakePt->Sumw2();
  m_incFakePt->Divide(m_fakePtDenom, m_incFakeDenomPt);
}
