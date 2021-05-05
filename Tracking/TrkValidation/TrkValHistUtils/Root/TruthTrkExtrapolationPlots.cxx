/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <utility>



#include "TrkValHistUtils/TruthTrkExtrapolationPlots.h"

namespace Trk {
  TruthTrkExtrapolationPlots::TruthTrkExtrapolationPlots(PlotBase *pParent, const std::string& sDir) : PlotBase(pParent, sDir),
    m_CaloEntry(this, "", "CaloEntry"),
    m_MuonEntry(this, "", "MuonEntry"),
    m_MuonExit(this, "", "MuonExit")
 {// ,
    // m_Calo(this,"Scattering/CaloOnly","CaloEntryLayer", "MuonEntryLayer"),
    // m_MS(this,"/Scattering/MSOnly","MuonEntryLayer", "MuonExitLayer"),
    // m_IP(this,"/Scattering/IPOnly","IP","CaloEntryLayer"),
    // m_IP_Calo(this, "/Scattering/IP_Calo","IP","MuonEntryLayer"),
    // m_Calo_MS(this, "/Scattering/Calo_MS","CaloEntryLayer","MuonExitLayer"),
    // m_IP_MS(this, "/Scattering/IP_Calo_Ms","IP","MuonExitLayer")
  }

  void
  TruthTrkExtrapolationPlots::fill(const xAOD::TruthParticle &truthprt, float weight) {
    m_CaloEntry.fill(truthprt, "CaloEntryLayer", weight);
    m_MuonEntry.fill(truthprt, "MuonEntryLayer", weight);
    m_MuonExit.fill(truthprt, "MuonExitLayer", weight);

    // m_Calo.fill(truthprt,"CaloEntryLayer_","MuonEntryLayer_");
    // m_MS.fill(truthprt, "MuonEntryLayer_","MuonExitLayer_");
    // m_IP.fill(truthprt, "","CaloEntryLayer_");
    // m_IP_Calo.fill(truthprt,"","MuonEntryLayer_");
    // m_Calo_MS.fill(truthprt,"CaloEntryLayer_","MuonExitLayer_");
    // m_IP_MS.fill(truthprt, "","MuonExitLayer_");
  }

  ExtrLayerPlots::ExtrLayerPlots(PlotBase *pParent, const std::string& sDir, std::string sLayerName) : PlotBase(pParent, sDir),
    m_sLayerName(std::move(sLayerName)),
    p{},px{},py{},pz{},p_extr{},px_extr{},py_extr{},pz_extr{},dp_truth{},dpx_truth{},dpy_truth{},dpz_truth{},
    pt{},eta{},phi{},theta{}{
    //nop
  }

  void
  ExtrLayerPlots::initializePlots() {
    p = Book1D(m_sLayerName + "_p", m_sLayerName + " truth_p;Momentum [GeV];Entries", 100, 0., 400);
    p_extr = Book1D(m_sLayerName + "_pExtr", m_sLayerName + " truth_p_extr;Momentum [GeV];Entries", 100, 0., 400);
    dp_truth = Book1D(m_sLayerName + "_dp", m_sLayerName + " truth_dp;Momentum difference p-p_{extr} [GeV];Entries",
                      100, -10, 10);

    // px  = Book1D(m_sLayerName+"_truth_px", m_sLayerName+ " truth_px;Momentum [GeV];Entries / 2.5 GeV",200,-250.,250);
    // py  = Book1D(m_sLayerName+"_truth_py", m_sLayerName+ " truth_py;Momentum [GeV];Entries / 2.5 GeV",200,-250.,250);
    // pz  = Book1D(m_sLayerName+"_truth_pz", m_sLayerName+ " truth_pz;Momentum [GeV];Entries / 2.5 GeV",200,-250.,250);

    // px_extr  = Book1D(m_sLayerName+"_truth_px_extr", m_sLayerName+ " truth_px_extr;Momentum [GeV];Entries / 2.5
    // GeV",200,-250.,250);
    // py_extr  = Book1D(m_sLayerName+"_truth_py_extr", m_sLayerName+ " truth_py_extr;Momentum [GeV];Entries / 2.5
    // GeV",200,-250.,250);
    // pz_extr  = Book1D(m_sLayerName+"_truth_pz_extr", m_sLayerName+ " truth_pz_extr;Momentum [GeV];Entries / 2.5
    // GeV",200,-250.,250);

    // dpx_truth  = Book1D(m_sLayerName+"_truth_dpx", m_sLayerName+ " truth_dpx;Momentum difference
    // px-px_extr[GeV];Entries / 2.5 GeV",250,-25,25);
    // dpy_truth  = Book1D(m_sLayerName+"_truth_dpy", m_sLayerName+ " truth_dpy;Momentum difference
    // py-py_extr[GeV];Entries / 2.5 GeV",250,-25,25);
    // dpz_truth  = Book1D(m_sLayerName+"_truth_dpz", m_sLayerName+ " truth_dpz;Momentum difference
    // pz-pz_extr[GeV];Entries / 2.5 GeV",250,-25,25);
    // pt = Book1D(m_sLayerName+"_truth_pt", m_sLayerName+ " truth_pt;Transverse Momentum [GeV];Entries / 2.5
    // GeV",100,0.,250);
    // eta = Book1D(m_sLayerName+"_truth_eta",m_sLayerName+ " truth_eta;Pseudo-Rapidity;Entries / 0.1",64,-3.2,3.2);
    // phi = Book1D(m_sLayerName+"_truth_phi",m_sLayerName+ " truth_phi;Azimuthal Angle;Entries / 0.1",64,-3.2,3.2);
    // theta = Book1D(m_sLayerName+"_truth_theta",m_sLayerName+ " truth_theta;polar Angle;Entries / 0.1",32,0,3.2);
  }

  void
  ExtrLayerPlots::fill(const xAOD::TruthParticle &truthprt, const std::string& sNom, float weight) {
    if (!truthprt.isAvailable<float>(sNom + "_px") ||
        !truthprt.isAvailable<float>(sNom + "_py") ||
        !truthprt.isAvailable<float>(sNom + "_pz")) {
      return;
    }

    TVector3 vec(truthprt.auxdata<float>(sNom + "_px"),
                 truthprt.auxdata<float>(sNom + "_py"),
                 truthprt.auxdata<float>(sNom + "_pz"));

    TVector3 vec_extr;
    if (!truthprt.isAvailable<float>(sNom + "_px_extr") ||
        !truthprt.isAvailable<float>(sNom + "_py_extr") ||
        !truthprt.isAvailable<float>(sNom + "_pz_extr")) {
      return;
    }

    vec_extr.SetXYZ(truthprt.auxdata<float>(sNom + "_px_extr"),
                    truthprt.auxdata<float>(sNom + "_py_extr"),
                    truthprt.auxdata<float>(sNom + "_pz_extr"));

    p->Fill(vec.Mag() * 0.001, weight);
    // px->Fill(vec.Px()*0.001);
    // py->Fill(vec.Py()*0.001);
    // pz->Fill(vec.Pz()*0.001);
    p_extr->Fill(vec_extr.Mag() * 0.001, weight);
    // px_extr->Fill(vec_extr.Px()*0.001);
    // py_extr->Fill(vec_extr.Py()*0.001);
    // pz_extr->Fill(vec_extr.Pz()*0.001);
    dp_truth->Fill((vec.Mag() - vec_extr.Mag()) * 0.001, weight);
    // dpx_truth->Fill((vec.Px()-vec_extr.Px())*0.001);
    // dpy_truth->Fill((vec.Py()-vec_extr.Py())*0.001);
    // dpz_truth->Fill((vec.Pz()-vec_extr.Pz())*0.001);
    // pt->Fill(vec.Perp()*0.001);
    // eta->Fill(vec.PseudoRapidity());
    // phi->Fill(vec.Phi());
    // theta->Fill(vec.Theta());
  }

  ExtrRegionPlots::ExtrRegionPlots(PlotBase *pParent, const std::string& sDir, std::string sRegionBegin,
                                   std::string sRegionEnd) : PlotBase(pParent, sDir),
    m_sRegionBegin(std::move(sRegionBegin)),
    m_sRegionEnd(std::move(sRegionEnd)),
    dp{},dpt{},dpx{},dpy{},dpz{},dp_extr{},dpx_extr{},dpy_extr{},dpz_extr{},
    dp_vs_p{},dp_vs_phi{},dp_vs_eta{},dp_vs_eta_phi{},p_vs_p{},p_extr_vs_p_extr{},dp_extr_vs_eta{},
    dR{},dR_vs_p{},dR_vs_dp{},dR_vs_eta{},dR_vs_phi{},dR_vs_eta_phi{},dphi{},dtheta{},dAngle{},
    dp_CaloEntry_IP_dp_CaloEntry_etxr{} {
    //nop
  }

  void
  ExtrRegionPlots::initializePlots() {
    // histogram definitions for scattering plots
    // variables within subdetector(s)
    // energy loss related quantities
    dp = Book1D(m_sRegionBegin + "_vs_" + m_sRegionEnd + "_dp",
                m_sRegionBegin + " vs " + m_sRegionEnd + " difference in truth p: p_{begin} vs p_{end}; #Delta p=p_{begin}-p_{end} [GeV];Entries / 0.5 GeV", 80, -20.,
                20.);
    dpt = Book1D(m_sRegionBegin + "_vs_" + m_sRegionEnd + "_dpt",
                 m_sRegionBegin + " vs " + m_sRegionEnd + " difference in truth pt;#Delta pt=pt_{begin}-pt_{end} [GeV];Entries / 0.5 GeV", 80, -20.,
                 20.);
    dpx = Book1D(m_sRegionBegin + "_vs_" + m_sRegionEnd + "_dpx",
                 m_sRegionBegin + " vs " + m_sRegionEnd + " difference in truth px;#Delta px=px_{begin}-px_{end} [GeV];Entries / 0.5 GeV", 80, -20.,
                 20.);
    dpy = Book1D(m_sRegionBegin + "_vs_" + m_sRegionEnd + "_dpy",
                 m_sRegionBegin + " vs " + m_sRegionEnd + " difference in truth py;#Delta py=py_{begin}-py_{end} [GeV];Entries / 0.5 GeV", 80, -20.,
                 20.);
    dpz = Book1D(m_sRegionBegin + "_vs_" + m_sRegionEnd + "_dpz",
                 m_sRegionBegin + " vs " + m_sRegionEnd + " difference in truth pz;#Delta pz=pz_{begin}-pz_{end} [GeV];Entries / 0.5 GeV", 80, -20.,
                 20.);

    // extrapolated quantities: CaloEntryLayer_p_extr=extrapolated value from IP to CaloEntryLayer using e-loss
    // parametrization
    // if comparison to IP, truth_p_extr=truth_p
    dp_extr = Book1D(m_sRegionBegin + "_vs_" + m_sRegionEnd + "_dp_extr",
                     m_sRegionBegin + " vs " + m_sRegionEnd + " difference in truth p, extrapolated from IP to current layer; #Delta p=p_extr_{begin}-p_extr_{end} [GeV];Entries / 0.5 GeV", 80, -20.,
                     20.);
    dpx_extr = Book1D(m_sRegionBegin + "_vs_" + m_sRegionEnd + "_dpx_extr",
                      m_sRegionBegin + " vs " + m_sRegionEnd + " difference in truth px, extrapolated from IP to current layer; #Delta px=px_extr_{begin}-px_extr_{end} [GeV];Entries / 0.5 GeV", 80, -20.,
                      20.);
    dpy_extr = Book1D(m_sRegionBegin + "_vs_" + m_sRegionEnd + "_dpy_extr",
                      m_sRegionBegin + " vs " + m_sRegionEnd + " difference in truth py, extrapolated from IP to current layer; #Delta py=py_extr_{begin}-py_extr_{end} [GeV];Entries / 0.5 GeV", 80, -20.,
                      20.);
    dpz_extr = Book1D(m_sRegionBegin + "_vs_" + m_sRegionEnd + "_dpz_extr",
                      m_sRegionBegin + " vs " + m_sRegionEnd + " difference in truth pz, extrapolated from IP to current layer; #Delta pz=pz_extr_{begin}-pz_extr_{end} [GeV];Entries / 0.5 GeV", 80, -20.,
                      20.);

    // 2D plots
    dp_vs_p = Book2D(m_sRegionBegin + "_vs_" + m_sRegionEnd + "_dp_vs_p",
                     m_sRegionBegin + " vs " + m_sRegionEnd + " difference in p (#Delta p=p_{begin}-p_{end}) vs p_{begin};p_{begin} [GeV];#Delta p=p_{begin}-p_{end} [GeV]", 250, 0., 250., 80, 0.,
                     20);
    dp_vs_phi = Book2D(m_sRegionBegin + "_vs_" + m_sRegionEnd + "_dp_vs_phi",
                       m_sRegionBegin + " vs " + m_sRegionEnd + " difference in p(#Delta p=p_{begin}-p_{end}) vs phi_{begin};phi_{begin};#Delta p=p_{begin}-p_{end} [GeV]", 128, -3.2, 3.2, 80, 0.,
                       20);
    dp_vs_eta = Book2D(m_sRegionBegin + "_vs_" + m_sRegionEnd + "_dp_vs_eta",
                       m_sRegionBegin + " vs " + m_sRegionEnd + " difference in p(#Delta p=p_{begin}-p_{end}) vs eta_{begin};#eta_{begin};#Delta p=p_{begin}-p_{end} [GeV]", 128, -3.2, 3.2, 80, 0.,
                       20);
    p_vs_p = Book2D(m_sRegionBegin + "_vs_" + m_sRegionEnd + "_p_vs_p",
                    m_sRegionBegin + " vs " + m_sRegionEnd + " p_{begin} vs p_{end};p_{begin} [GeV]; p_{end} [GeV]",
                    250, 0., 250, 250, 0., 250);
    p_extr_vs_p_extr = Book2D(m_sRegionBegin + "_vs_" + m_sRegionEnd + "_p_extr_vs_p_extr",
                              m_sRegionBegin + " vs " + m_sRegionEnd + " p_{begin extr} vs p_{end extr};p_{begin extr} [GeV]; p_{end extr} [GeV]", 250, 0., 250, 250, 0.,
                              250);
    dp_extr_vs_eta = Book2D(m_sRegionBegin + "_vs_" + m_sRegionEnd + "_dp_extr_vs_eta",
                            m_sRegionBegin + " vs " + m_sRegionEnd + " difference in p_{extr}(#Delta p_{extr}=p_{begin extr}-p_{end extr}) vs eta_{begin};#eta__{begin};#Delta p_{extr}=p_{begin extr}-p_{end extr} [GeV]", 128, -3.2, 3.2, 80, 0.,
                            20);

    // 3D
    dp_vs_eta_phi = Book2D(m_sRegionBegin + "_vs_" + m_sRegionEnd + "_dp_vs_eta_phi",
                           m_sRegionBegin + " vs " + m_sRegionEnd + " sum of bin entries: difference in p(#Delta p=p_{begin}-p_{end}) vs eta_{begin} and phi_{begin};#eta__{begin};#phi__{begin};#Delta p=p_{begin}-p_{end} [GeV]", 128, -3.2, 3.2, 128, -3.2,
                           -3.2);

    // measure for multiple scattering
    dR = Book1D(m_sRegionBegin + "_vs_" + m_sRegionEnd + "_dR",
                m_sRegionBegin + " vs " + m_sRegionEnd + " dR between the two positions in the detector;pt_{begin}->dR(pt_{end});Entries / 0.05", 100, 0.,
                0.5);
    // 2D
    dR_vs_p = Book2D(m_sRegionBegin + "_vs_" + m_sRegionEnd + "_dR_vs_p",
                     m_sRegionBegin + " vs " + m_sRegionEnd + " dR vs p_{begin};p_{begin} [GeV];_{begin}->dR(p_{end})",
                     250, 0., 250, 100, 0., 0.5);
    dR_vs_dp = Book2D(m_sRegionBegin + "_vs_" + m_sRegionEnd + "_dR_vs_dp",
                      m_sRegionBegin + " vs " + m_sRegionEnd + " dR vs difference in p;#Delta p=p_{begin}-p_{end} [GeV];p_{begin}->dR(p_{end})", 80, 0., 20, 100, 0.,
                      0.5);
    dR_vs_eta = Book2D(m_sRegionBegin + "_vs_" + m_sRegionEnd + "_dR_vs_eta",
                       m_sRegionBegin + " vs " + m_sRegionEnd + " dR vs eta;#eta_{begin};p_{begin}->dR(p_{end})", 128,
                       -3.2, 3.2, 100, 0., 0.5);
    dR_vs_phi = Book2D(m_sRegionBegin + "_vs_" + m_sRegionEnd + "_dR_vs_phi",
                       m_sRegionBegin + " vs " + m_sRegionEnd + " dR vs phi;#phi_{begin};p_{begin}->dR(p_{end})", 128,
                       -3.2, 3.2, 100, 0., 0.5);
    dR_vs_eta_phi = Book2D(m_sRegionBegin + "_vs_" + m_sRegionEnd + "_dR_vs_eta_phi",
                           m_sRegionBegin + " vs " + m_sRegionEnd + " sum of bin entries: dR vs eta and phi;#eta_{begin};#phi_{begin};#DeltaR", 128, -3.2, 3.2, 128, -3.2,
                           3.2);

    dphi = Book1D(m_sRegionBegin + "_vs_" + m_sRegionEnd + "_dphi",
                  m_sRegionBegin + " vs " + m_sRegionEnd + " difference in phi;#Delta phi;Entries / 0.05", 100, -0.25,
                  0.25);
    dtheta = Book1D(m_sRegionBegin + "_vs_" + m_sRegionEnd + "_dtheta",
                    m_sRegionBegin + " vs " + m_sRegionEnd + " difference in theta;#Delta theta;Entries / 0.002", 100,
                    -0.25, 0.25);
    dAngle = Book1D(m_sRegionBegin + "_vs_" + m_sRegionEnd + "_dAngle",
                    m_sRegionBegin + " vs " + m_sRegionEnd + " total angle difference;#Delta angle;Entries / 0.002",
                    100, -0.25, 0.25);
  }

  void
  ExtrRegionPlots::fill(const xAOD::TruthParticle &truthprt, const std::string& sDetBegin, const std::string& sDetEnd, float weight) {
    if (!truthprt.isAvailable<float>(sDetBegin + "px") ||
        !truthprt.isAvailable<float>(sDetBegin + "py") ||
        !truthprt.isAvailable<float>(sDetBegin + "pz") ||
        !truthprt.isAvailable<float>(sDetEnd + "px") ||
        !truthprt.isAvailable<float>(sDetEnd + "py") ||
        !truthprt.isAvailable<float>(sDetEnd + "pz")) {
      return;
    }

    TVector3 vecDetBegin(truthprt.auxdata<float>(sDetBegin + "px"),
                         truthprt.auxdata<float>(sDetBegin + "py"),
                         truthprt.auxdata<float>(sDetBegin + "pz"));

    TVector3 vecDetEnd(truthprt.auxdata<float>(sDetEnd + "px"),
                       truthprt.auxdata<float>(sDetEnd + "py"),
                       truthprt.auxdata<float>(sDetEnd + "pz"));

    // check if IP, since extr is not available there (sDetBegin="")
    TVector3 vecDetBegin_extr;
    if (sDetBegin != "") {
      if (!truthprt.isAvailable<float>(sDetBegin + "px_extr") ||
          !truthprt.isAvailable<float>(sDetBegin + "py_extr") ||
          !truthprt.isAvailable<float>(sDetBegin + "pz_extr")) {
        return;
      }

      vecDetBegin_extr.SetXYZ(truthprt.auxdata<float>(sDetBegin + "px_extr"),
                              truthprt.auxdata<float>(sDetBegin + "py_extr"),
                              truthprt.auxdata<float>(sDetBegin + "pz_extr"));
    }else {
      vecDetBegin_extr = vecDetBegin;
    }

    TVector3 vecDetEnd_extr;
    if (sDetEnd != "") {
      if (!truthprt.isAvailable<float>(sDetEnd + "px_extr") ||
          !truthprt.isAvailable<float>(sDetEnd + "py_extr") ||
          !truthprt.isAvailable<float>(sDetEnd + "pz_extr")) {
        return;
      }

      vecDetEnd_extr.SetXYZ(truthprt.auxdata<float>(sDetEnd + "px_extr"),
                            truthprt.auxdata<float>(sDetEnd + "py_extr"),
                            truthprt.auxdata<float>(sDetEnd + "pz_extr"));
    }else {
      vecDetEnd_extr = vecDetEnd;
    }

    dp->Fill((vecDetBegin.Mag() - vecDetEnd.Mag()) * 0.001, weight);
    dpt->Fill((vecDetBegin.Perp() - vecDetEnd.Perp()) * 0.001, weight);
    dpx->Fill((vecDetBegin.Px() - vecDetEnd.Px()) * 0.001, weight);
    dpy->Fill((vecDetBegin.Py() - vecDetEnd.Py()) * 0.001, weight);
    dpz->Fill((vecDetBegin.Pz() - vecDetEnd.Pz()) * 0.001, weight);
    dp_extr->Fill((vecDetBegin_extr.Mag() - vecDetEnd_extr.Mag()) * 0.001, weight);
    dpx_extr->Fill((vecDetBegin_extr.Px() - vecDetEnd_extr.Px()) * 0.001, weight);
    dpy_extr->Fill((vecDetBegin_extr.Py() - vecDetEnd_extr.Py()) * 0.001, weight);
    dpz_extr->Fill((vecDetBegin_extr.Pz() - vecDetEnd_extr.Pz()) * 0.001, weight);
    // 2D
    dp_vs_p->Fill(vecDetBegin.Mag() * 0.001, (vecDetBegin.Mag() - vecDetEnd.Mag()) * 0.001, weight);
    dp_vs_phi->Fill(vecDetBegin.Phi(), (vecDetBegin.Mag() - vecDetEnd.Mag()) * 0.001, weight);
    dp_vs_eta->Fill(vecDetBegin.Eta(), (vecDetBegin.Mag() - vecDetEnd.Mag()) * 0.001, weight);
    // not sure how to weight this
    dp_vs_eta_phi->Fill(vecDetBegin.Eta(), vecDetBegin.Phi(), (vecDetBegin.Mag() - vecDetEnd.Mag()) * 0.001);
    p_vs_p->Fill(vecDetBegin.Mag() * 0.001, vecDetEnd.Mag() * 0.001, weight);
    p_extr_vs_p_extr->Fill(vecDetBegin_extr.Mag() * 0.001, vecDetEnd_extr.Mag() * 0.001, weight);
    dp_extr_vs_eta->Fill(vecDetBegin_extr.Eta(), (vecDetBegin_extr.Mag() - vecDetEnd_extr.Mag()) * 0.001, weight);

    // multiple scattering
    dR->Fill(vecDetBegin.DeltaR(vecDetEnd),weight);
    // 2D
    dR_vs_p->Fill(vecDetBegin.Mag() * 0.001, vecDetBegin.DeltaR(vecDetEnd),weight);
    dR_vs_dp->Fill((vecDetBegin.Mag() - vecDetEnd.Mag()) * 0.001, vecDetBegin.DeltaR(vecDetEnd), weight);
    dR_vs_eta->Fill(vecDetBegin.Eta(), vecDetBegin.DeltaR(vecDetEnd), weight);
    dR_vs_phi->Fill(vecDetBegin.Phi(), vecDetBegin.DeltaR(vecDetEnd), weight);
    // not sure how to weight this
    dR_vs_eta_phi->Fill(vecDetBegin.Eta(), vecDetBegin.Phi(), vecDetBegin.DeltaR(vecDetEnd) );

    dphi->Fill(vecDetBegin.DeltaPhi(vecDetEnd), weight);
    dtheta->Fill(vecDetBegin.Theta() - vecDetEnd.Theta(), weight);

    dAngle->Fill(vecDetBegin.Angle(vecDetEnd), weight);
  }
}// namespace trk
