/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * JPsiBasicTPEfficiencyPlots.cxx
 *
 *  Created on: Dec 22, 2014
 *      Author: Maximiliano Sioli
 */

#include "xAODTracking/TrackParticle.h"
#include "xAODMuon/Muon.h"
#include "xAODTracking/VertexContainer.h"
#include "MuonPerformanceHistUtils/JPsiTagProbeDileptonPlots.h"

JPsiTagProbeDileptonPlots::JPsiTagProbeDileptonPlots(PlotBase* pParent, std::string sDir, bool isMatched, bool ApplySF)
: MuonTPEfficiencyPlotBase (pParent, sDir, isMatched, ApplySF),
  m_isMatched(isMatched) {}

void JPsiTagProbeDileptonPlots::initializePlots()
{
  m_ll = Book1D("m_ll","mll; m_{ll} [GeV]; Entries",250,2000.,4500.);
  m_opening_angle = Book1D("opening_angle","opening_angle; Dilepton opening angle; Entries",40,0.0,TMath::Pi());
  m_jpsi_pt  = Book1D("jpsi_pt","jpsi_pt; JPsi p_{T} [GeV] ; Entries",100,0.0,50000.);

  //  const double pi = 3.141592653589;

  // 23 variable-size bins in pt  
  double bins_p[] = {0., 0.5, 1., 1.5, 2., 2.5, 3., 3.5, 4., 4.5, 5., 5.5, 6., 6.5, 7., 7.5, 8., 9., 10., 12., 14., 16., 18., 20.};

  // 25 fixed-size bins in eta
  double min_e = -2.5;
  double max_e = 2.5;
  int nBins_e = 25;
  double step_e = (max_e-min_e)/nBins_e; // 0.2 eta units
  double bins_e[26];

  double tmp = min_e;
  int i = 0;
  while (tmp < max_e) {
    bins_e[i] = tmp;
    tmp += step_e;
    i++;
  }
  bins_e[i] = max_e;

  // 25 fixed-size bins in phi

  double min_f = -m_pi;
  double max_f =  m_pi;
  int nBins_f = 25;
  double step_f = (max_f-min_f)/nBins_f; // ~0.25 rad
  double bins_f[26];
  
  tmp = min_f;
  i = 0;
  while (tmp < max_f) {
    bins_f[i] = tmp;
    tmp += step_f;
    i++;
  }
  bins_f[i] = max_f;

  // 200 fixed-size bins in mll

  double min_m = 2.0;
  double max_m = 4.5;
  int nBins_m = 200;
  double step_m = (max_m-min_m)/nBins_m; // 12.5 MeV
  double bins_m[201];

  tmp = min_m;
  i = 0;
  while (tmp < max_m) {
    bins_m[i] = tmp;
    tmp += step_m;
    i++;
  }
  bins_m[i] = max_m;

  int n_bins_p = sizeof (bins_p) / sizeof (double) - 1;
  int n_bins_e = sizeof (bins_e) / sizeof (double) - 1;
  int n_bins_f = sizeof (bins_f) / sizeof (double) - 1;
  int n_bins_m = sizeof (bins_m) / sizeof (double) - 1;

  //  std::cout << "eccolo: " << n_bins_f << " " << bins_f[0] << " " << bins_f[n_bins_f+1] << std::endl;

  TH3D pem_dummy ("pem_dummy","pemdummy", n_bins_p, bins_p, n_bins_e, bins_e, n_bins_m, bins_m);
  TH3D pem_dummy_etaq ("pem_dummy_etaq","pemdummy_etaq", n_bins_p, bins_p, n_bins_e, bins_e, n_bins_m, bins_m);

  TH3D fem_dummy ("fem_dummy","femdummy", n_bins_f, bins_f, n_bins_e, bins_e, n_bins_m, bins_m);
  TH3D fem_dummy_etaq ("fem_dummy_etaq","femdummy_etaq", n_bins_f, bins_f, n_bins_e, bins_e, n_bins_m, bins_m);

  TH3D pfm_dummy ("pfm_dummy","pfmdummy", n_bins_p, bins_p, n_bins_f, bins_f, n_bins_m, bins_m);

  m_pem_num = PlotBase::Book3D("_pem_num", &pem_dummy, "pem_num; p_{T} [GeV]; eta; m_{ll} [GeV]; Entries");
  m_pem_num_etaq = PlotBase::Book3D("_pem_num_etaq", &pem_dummy_etaq, "pem_num_etaq; p_{T} [GeV]; eta*Q; m_{ll} [GeV]; Entries");

  m_fem_num = PlotBase::Book3D("_fem_num", &fem_dummy, "fem_num; phi; eta; m_{ll} [GeV]; Entries");
  m_fem_num_etaq = PlotBase::Book3D("_fem_num_etaq", &fem_dummy_etaq, "fem_num_etaq; phi; eta*Q; m_{ll} [GeV]; Entries");

  m_pfm_num = PlotBase::Book3D("_pfm_num", &pfm_dummy, "pfm_num; p_{T} [GeV]; phi; m_{ll} [GeV]; Entries");

}


void JPsiTagProbeDileptonPlots::fill(Probe& probe)
{

    if(m_isMatched && !probe.isMatched()) return;

    float sfweight = (m_isMatched && m_apply_SF ? probe.sfweight() : 1.);
    TLorentzVector z = probe.probeTrack().p4() + probe.tagTrack().p4();

    m_ll->Fill(z.M(),sfweight );
    m_jpsi_pt->Fill(z.Pt(),sfweight);
    m_opening_angle->Fill(fabs(probe.probeTrack().p4().Angle(probe.tagTrack().p4().Vect())),sfweight);

    float vtx_mass = z.M();
    float probePt = probe.pt();
    float probeEta = probe.eta();
    float probePhi = probe.phi();

    int probeQ = 1000;

    // case of a muon probe
    const xAOD::Muon* muprobe = dynamic_cast<const xAOD::Muon*>(&probe.probeTrack());
    if (muprobe) probeQ = muprobe->trackParticle(xAOD::Muon::Primary)->charge();

    // case of an ID probe
    const xAOD::TrackParticle* trkprobe = dynamic_cast<const xAOD::TrackParticle*>(&probe.probeTrack());
    if (trkprobe) probeQ = trkprobe->charge();

    if ( probeQ < 1000 ) {
      m_pem_num->Fill(probePt/1000., probeEta, vtx_mass/1000., sfweight);
      m_pem_num_etaq->Fill(probePt/1000., probeEta*probeQ, vtx_mass/1000., sfweight);

      m_fem_num->Fill(probePhi, probeEta, vtx_mass/1000., sfweight);
      m_fem_num_etaq->Fill(probePhi, probeEta*probeQ, vtx_mass/1000., sfweight);

      m_pfm_num->Fill(probePt/1000., probePhi, vtx_mass/1000., sfweight);
    }

}
