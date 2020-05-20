/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */



#include "AthenaMonitoring/AthenaMonManager.h"
#include "HIMonitoring/HIMonitoringEventShapeTool.h"
#include "xAODTrigMinBias/TrigT2ZdcSignals.h"

#include "LWHists/TH1D_LW.h"
#include "LWHists/TH2D_LW.h"
#include "LWHists/TProfile_LW.h"

HIMonitoringEventShapeTool::
HIMonitoringEventShapeTool( const std::string & type, const std::string & name,
                            const IInterface* parent ): ManagedMonitorToolBase( type, name, parent )
{
	m_FCalEt=0;
	m_FCalEt_A=0;
	m_FCalEt_C=0;
	m_ZDC_HG=0;
	m_ZDC_LG=0;
	m_h_FCalEt=0;
	m_h_FCalEt_vs_eta=0;
	m_nbins_phi=64;
	m_nbins_eta=s_num_of_eta_bins;
	m_eta_range=5.0;
	m_Pi = 3.14159265359;
	declareProperty( "ZDCmon", m_ZDCmon=true);
	declareProperty( "ESmon", m_ESmon=true);
	declareProperty( "FCalEt_eta_hist_cut", m_FCalEt_eta_hist_cut=0.05); // in TeV
	declareProperty( "FCalEt_nbins", m_FCalEt_nbins=95);
	declareProperty( "lowFCalEt", m_low_FCalEt=-0.15);
	declareProperty( "highFCalEt", m_high_FCalEt=0.8);
}

HIMonitoringEventShapeTool::~HIMonitoringEventShapeTool() {
}

// Description: Used for rebooking unmanaged histograms
StatusCode HIMonitoringEventShapeTool::bookHistogramsRecurrent( ) {
  return StatusCode::SUCCESS;
}

// Description: Used for re-booking managed histograms
StatusCode HIMonitoringEventShapeTool::bookHistograms( ) {
  if (m_environment == AthenaMonManager::online) {
    // book histograms that are only made in the online environment...
  }

  if (m_dataType == AthenaMonManager::cosmics) {
    // book histograms that are only relevant for cosmics data...
  }

  if (m_ESmon) bookFCalEt_hist();
  if (m_ESmon) bookES_hist();
  if (m_ZDCmon) bookZDC_hist();

  return StatusCode::SUCCESS;
}

StatusCode HIMonitoringEventShapeTool::fillHistograms() {
  StatusCode sc;

  /// HI event shape
  if (m_ESmon) {
    const xAOD::HIEventShapeContainer* evtShape;
    sc = evtStore()->retrieve(evtShape, "HIEventShape");
    if (sc.isFailure()) {
      ATH_MSG_ERROR("Could not find HIEventShape");
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_INFO("HIEventShape retrieved from StoreGate");
    }

    getFCalEt(evtShape);
    getES(evtShape);

    fillFCalEt_hist();
    fillES_hist();
  }

  /// ZDC trigg container
  if (m_ZDCmon) {
    const xAOD::TrigT2ZdcSignalsContainer* TrigZdc_p;
    sc = evtStore()->retrieve(TrigZdc_p, "HLT_xAOD__TrigT2ZdcSignalsContainer_zdcsignals");
    if (sc.isFailure()) {
      ATH_MSG_ERROR("Could not find TrigT2ZdcSignalsContainer");
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_INFO("TrigT2ZdcSignalsContainer retrieved from StoreGate");
    }

    getZDC(TrigZdc_p);
    fillZDC_hist();
  }

  return StatusCode::SUCCESS;
}

StatusCode HIMonitoringEventShapeTool::procHistograms( ) {
  //if( endOfLowStatFlag() || endOfLumiBlockFlag() )  { }

  if (endOfRunFlag()) {
    if (m_h_FCalEt->GetEntries() > 0) m_h_FCalEt->Scale(1. / m_h_FCalEt->GetEntries());

    for (int i = 0; i < 2; i++) {
      if (m_h_FCalEt_sides[i]->GetEntries() > 0) m_h_FCalEt_sides[i]->Scale(1. / m_h_FCalEt_sides[i]->GetEntries());
    }
  }

  return StatusCode::SUCCESS;
}

/// *** FCal sum Et *** ///
void HIMonitoringEventShapeTool::bookFCalEt_hist() {
  std::string path = "HeavyIon/FCal";

  m_h_FCalEt = new TH1D("h_FCalEt", "; FCal #Sigma E_{T} [TeV]; entries", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt);
  regHist(m_h_FCalEt, path, run).ignore();


  m_h_FCalEt_vs_eta = TProfile_LW::create("h_FCalEt_vs_eta", "; #eta; < FCal #Sigma E_{T} > [TeV]", m_nbins_eta,
                                          -m_eta_range, m_eta_range);
  regHist(m_h_FCalEt_vs_eta, path, run).ignore();

  m_h_FCalEt_A_vs_C = TH2D_LW::create("h_FCalEt_A_vs_C",
                                      "; FCal #Sigma E_{T} Sice A [TeV]; FCal #Sigma E_{T} Sice C [TeV]",
                                      m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_FCalEt_nbins, m_low_FCalEt,
                                      m_high_FCalEt);
  regHist(m_h_FCalEt_A_vs_C, path, run).ignore();

  for (int i = 0; i < 2; i++) {
    TString hist_name = "h_FCalEt_side" + m_side_id[i];
    TString hist_aixis = "; FCal Side " + m_side_id[i] + " #Sigma E_{T} [TeV]; entries";
    m_h_FCalEt_sides[i] = new TH1D(hist_name, hist_aixis, m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt);
    regHist(m_h_FCalEt_sides[i], path, run).ignore();
  }
}

void HIMonitoringEventShapeTool::getFCalEt(const xAOD::HIEventShapeContainer* evtShape) {
  for (int i = 0; i < s_num_of_eta_bins; i++) m_FCalEt_eta[i] = 0.0;

  int size = evtShape->size();
  for (int i = 0; i < size; i++) {
    const xAOD::HIEventShape* sh = evtShape->at(i);
    float slice_eta = (sh->etaMin() + sh->etaMax()) / 2.0;
    if (sh->layer() == 21 || sh->layer() == 22 || sh->layer() == 23) {
      m_FCalEt += sh->et();
      m_FCalEt_eta[ES_eta_layer2bin(slice_eta)] += sh->et();
      // m_h_FCalEt_vs_eta->Fill(slice_eta, sh->et());
      if (slice_eta > 0) m_FCalEt_A += sh->et();
      else m_FCalEt_C += sh->et();
    }
  }

  m_FCalEt = m_FCalEt / 1000000.;
  m_FCalEt_A = m_FCalEt_A / 1000000.;
  m_FCalEt_C = m_FCalEt_C / 1000000.;
  for (int i = 0; i < s_num_of_eta_bins; i++)
    if (m_FCalEt_eta[i] != 0) m_FCalEt_eta[i] = m_FCalEt_eta[i] / 1000000.0;
}

void HIMonitoringEventShapeTool::fillFCalEt_hist() {
  m_h_FCalEt->Fill(m_FCalEt);
  m_h_FCalEt_sides[0]->Fill(m_FCalEt_A);
  m_h_FCalEt_sides[1]->Fill(m_FCalEt_C);
  m_h_FCalEt_A_vs_C->Fill(m_FCalEt_A, m_FCalEt_C);
  for (int i = 0; i < s_num_of_eta_bins; i++) {
    m_h_FCalEt_vs_eta->Fill(ES_bin2eta_layer(i), m_FCalEt_eta[i]);
  }
}

/// *** event shape veraiables *** ///
void HIMonitoringEventShapeTool::bookES_hist() {
  std::string path = "HeavyIon/EventShape";
  TString hist_name = "";
  TString hist_axis = "";

  for (int i = 0; i < s_num_of_harm; i++) {
    std::string fullpath = path + "/q" + m_sqn_num[i];

    hist_name = "h_q" + m_sqn_num[i] + "x_vs_FCalEt";
    hist_axis = "; FCal #Sigma E_{T} [TeV]; q_{" + m_sqn_num[i] + ",x}";
    m_h_qnx_vs_FCalEt[i] = TProfile_LW::create(hist_name, hist_axis, m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt);
    regHist(m_h_qnx_vs_FCalEt[i], fullpath, run).ignore();

    hist_name = "h_q" + m_sqn_num[i] + "y_vs_FCalEt";
    hist_axis = "; FCal #Sigma E_{T} [TeV]; q_{" + m_sqn_num[i] + ",y}";
    m_h_qny_vs_FCalEt[i] = TProfile_LW::create(hist_name, hist_axis, m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt);
    regHist(m_h_qny_vs_FCalEt[i], fullpath, run).ignore();

    hist_name = "h_q" + m_sqn_num[i] + "x_A_vs_FCalEt";
    hist_axis = "; FCal #Sigma E_{T} [TeV]; q_{" + m_sqn_num[i] + ",x} Side A";
    m_h_qnx_A_vs_FCalEt[i] = TProfile_LW::create(hist_name, hist_axis, m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt);
    regHist(m_h_qnx_A_vs_FCalEt[i], fullpath, run).ignore();

    hist_name = "h_q" + m_sqn_num[i] + "y_A_vs_FCalEt";
    hist_axis = "; FCal #Sigma E_{T} [TeV]; q_{" + m_sqn_num[i] + ",y} Side A";
    m_h_qny_A_vs_FCalEt[i] = TProfile_LW::create(hist_name, hist_axis, m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt);
    regHist(m_h_qny_A_vs_FCalEt[i], fullpath, run).ignore();

    hist_name = "h_q" + m_sqn_num[i] + "x_C_vs_FCalEt";
    hist_axis = "; FCal #Sigma E_{T} [TeV]; q_{" + m_sqn_num[i] + ",x} Side C";
    m_h_qnx_C_vs_FCalEt[i] = TProfile_LW::create(hist_name, hist_axis, m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt);
    regHist(m_h_qnx_C_vs_FCalEt[i], fullpath, run).ignore();

    hist_name = "h_q" + m_sqn_num[i] + "y_C_vs_FCalEt";
    hist_axis = "; FCal #Sigma E_{T} [TeV]; q_{" + m_sqn_num[i] + ",y} Side C";
    m_h_qny_C_vs_FCalEt[i] = TProfile_LW::create(hist_name, hist_axis, m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt);
    regHist(m_h_qny_C_vs_FCalEt[i], fullpath, run).ignore();

    hist_name = "h_q" + m_sqn_num[i] + "x_vs_eta";
    hist_axis = "; #eta; q_{" + m_sqn_num[i] + ",x}";
    m_h_qnx_vs_eta[i] = TProfile_LW::create(hist_name, hist_axis, m_nbins_eta, -m_eta_range, m_eta_range);
    regHist(m_h_qnx_vs_eta[i], fullpath, run).ignore();

    hist_name = "h_q" + m_sqn_num[i] + "y_vs_eta";
    hist_axis = "; #eta; q_{" + m_sqn_num[i] + ",y}";
    m_h_qny_vs_eta[i] = TProfile_LW::create(hist_name, hist_axis, m_nbins_eta, -m_eta_range, m_eta_range);
    regHist(m_h_qny_vs_eta[i], fullpath, run).ignore();

    hist_name = "h_q" + m_sqn_num[i] + "_vs_FCalEt";
    hist_axis = "; FCal #Sigma E_{T} [TeV]; q_{" + m_sqn_num[i] + "}";
    m_h_qn_vs_FCalEt[i] = TH2D_LW::create(hist_name, hist_axis, m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, 300, 0.0,
                                          1.0);
    regHist(m_h_qn_vs_FCalEt[i], fullpath, run).ignore();

    hist_name = "h_psi" + m_sqn_num[i] + "_A_vs_FCalEt";
    hist_axis = "; FCal #Sigma E_{T} [TeV]; #psi_{" + m_sqn_num[i] + "} Side A";
    m_h_psin_A_vs_FCalEt[i] = TH2D_LW::create(hist_name, hist_axis, m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt,
                                              m_nbins_phi, -m_Pi / (i + 1), m_Pi / (i + 1));
    regHist(m_h_psin_A_vs_FCalEt[i], fullpath, run).ignore();

    hist_name = "h_psi" + m_sqn_num[i] + "_C_vs_FCalEt";
    hist_axis = "; FCal #Sigma E_{T} [TeV]; #psi_{" + m_sqn_num[i] + "} Side C";
    m_h_psin_C_vs_FCalEt[i] = TH2D_LW::create(hist_name, hist_axis, m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt,
                                              m_nbins_phi, -m_Pi / (i + 1), m_Pi / (i + 1));
    regHist(m_h_psin_C_vs_FCalEt[i], fullpath, run).ignore();

    hist_name = "h_psi" + m_sqn_num[i] + "_ACdiff_vs_FCalEt";
    hist_axis = "; FCal #Sigma E_{T} [TeV]; #psi_{" + m_sqn_num[i] + "} Side A - #psi_{" + m_sqn_num[i] + "} Side C";
    m_h_psin_ACdiff_vs_FCalEt[i] = TH2D_LW::create(hist_name, hist_axis, m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt,
                                                   m_nbins_phi, -m_Pi / (i + 1), m_Pi / (i + 1));
    regHist(m_h_psin_ACdiff_vs_FCalEt[i], fullpath, run).ignore();

    hist_name = "h_psi" + m_sqn_num[i] + "_R_vs_FCalEt";
    hist_axis = "; FCal #Sigma E_{T} [TeV]; < cos " + m_sqn_num[i] + "(#psi_{" + m_sqn_num[i] + "}^{A} - #psi_{" +
                m_sqn_num[i] + "}^{C}) >";
    m_h_psin_R_vs_FCalEt[i] = TProfile_LW::create(hist_name, hist_axis, m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt);
    regHist(m_h_psin_R_vs_FCalEt[i], fullpath, run).ignore();
  }
}

void HIMonitoringEventShapeTool::getES(const xAOD::HIEventShapeContainer* evtShape) {
  for (int i = 0; i < s_num_of_harm; i++) {
    m_qnx[i] = 0;
    m_qny[i] = 0;
    m_qn[i] = 0;
    m_qnx_A[i] = 0;
    m_qny_A[i] = 0;
    m_qnx_C[i] = 0;
    m_qny_C[i] = 0;
    for (int j = 0; j < s_num_of_eta_bins; j++) {
      m_qnx_eta[i][j] = 0;
      m_qny_eta[i][j] = 0;
    }
  }

  int size = evtShape->size();
  for (int i = 0; i < size; i++) {
    const xAOD::HIEventShape* sh = evtShape->at(i);
    float Et = sh->et();
    if (Et == 0) continue;

    float eta = (sh->etaMin() + sh->etaMax()) / 2.0;
    // std::cout << "eta values " << eta << " -> " <<  ES_eta_layer2bin(eta) <<  " -> " <<
    // ES_bin2eta_layer(ES_eta_layer2bin(eta))  << std::endl;

    if (sh->layer() == 21 ||
        sh->layer() == 22 ||
        sh->layer() == 23) { //FCal Only
      for (int i = 0; i < s_num_of_harm; i++) {
        m_qnx[i] += sh->etCos().at(i);
        m_qny[i] += sh->etSin().at(i);
        if (eta > 0) {
          m_qnx_A[i] += sh->etCos().at(i);
          m_qny_A[i] += sh->etSin().at(i);
        } else {
          m_qnx_C[i] += sh->etCos().at(i);
          m_qny_C[i] += sh->etSin().at(i);
        }

        m_qnx_eta[i][ES_eta_layer2bin(eta)] += sh->etCos().at(i);
        m_qny_eta[i][ES_eta_layer2bin(eta)] += sh->etSin().at(i);
      }
    }
  }

  for (int i = 0; i < s_num_of_harm; i++) {
    m_qnx[i] = m_qnx[i] / (m_FCalEt * 1000000.0);
    m_qny[i] = m_qny[i] / (m_FCalEt * 1000000.0);
    m_qn[i] = calc_qn(m_qnx[i], m_qny[i]);

    m_qnx_A[i] = m_qnx_A[i] / (m_FCalEt_A * 1000000.0);
    m_qny_A[i] = m_qny_A[i] / (m_FCalEt_A * 1000000.0);

    m_qnx_C[i] = m_qnx_C[i] / (m_FCalEt_A * 1000000.0);
    m_qny_C[i] = m_qny_C[i] / (m_FCalEt_A * 1000000.0);

    m_psin_A[i] = calc_psin(i, m_qnx_A[i], m_qny_A[i]);
    m_psin_C[i] = calc_psin(i, m_qnx_C[i], m_qny_C[i]);

    for (int j = 0; j < s_num_of_eta_bins; j++) {
      if (m_FCalEt_eta[j] != 0) {
        m_qnx_eta[i][j] = m_qnx_eta[i][j] / (m_FCalEt_eta[j] * 1000000.0);
        m_qny_eta[i][j] = m_qny_eta[i][j] / (m_FCalEt_eta[j] * 1000000.0);
      }
    }
  }
}

void HIMonitoringEventShapeTool::fillES_hist() {
  for (int i = 0; i < s_num_of_harm; i++) {
    m_h_qnx_vs_FCalEt[i]->Fill(m_FCalEt, m_qnx[i]);
    m_h_qny_vs_FCalEt[i]->Fill(m_FCalEt, m_qny[i]);
    m_h_qn_vs_FCalEt[i]->Fill(m_FCalEt, m_qn[i]);

    m_h_qnx_A_vs_FCalEt[i]->Fill(m_FCalEt, m_qnx_A[i]);
    m_h_qny_A_vs_FCalEt[i]->Fill(m_FCalEt, m_qny_A[i]);

    m_h_qnx_C_vs_FCalEt[i]->Fill(m_FCalEt, m_qnx_C[i]);
    m_h_qny_C_vs_FCalEt[i]->Fill(m_FCalEt, m_qny_C[i]);

    m_h_psin_A_vs_FCalEt[i]->Fill(m_FCalEt, m_psin_A[i]);
    m_h_psin_C_vs_FCalEt[i]->Fill(m_FCalEt, m_psin_C[i]);
    m_h_psin_ACdiff_vs_FCalEt[i]->Fill(m_FCalEt, calc_psin_diff(i, m_psin_A[i], m_psin_C[i]));
    m_h_psin_R_vs_FCalEt[i]->Fill(m_FCalEt, cos((i + 1) * calc_psin_diff(i, m_psin_A[i], m_psin_C[i])));

    if (m_FCalEt > m_FCalEt_eta_hist_cut) {
      for (int j = 0; j < s_num_of_eta_bins; j++) {
        m_h_qnx_vs_eta[i]->Fill(ES_bin2eta_layer(j), m_qnx_eta[i][j]);
        m_h_qny_vs_eta[i]->Fill(ES_bin2eta_layer(j), m_qny_eta[i][j]);
      }
    }
  }
}

/// *** ZDC part  *** ///
void HIMonitoringEventShapeTool::bookZDC_hist() {
  std::string path = "HeavyIon/FCal";
  TString hist_name = "";

  m_h_FCalEt_vs_ZDC_HG = TH2D_LW::create("h_FCalEt_vs_ZDC_HG", "; FCal #Sigma E_{T} [TeV]; ZDC high gain ",
                                         m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, 100, 0, 10);
  regHist(m_h_FCalEt_vs_ZDC_HG, path, run).ignore();
  m_h_FCalEt_vs_ZDC_LG = TH2D_LW::create("h_FCalEt_vs_ZDC_LG", "; FCal #Sigma E_{T} [TeV]; ZDC low gain ",
                                         m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, 100, 0, 10);
  regHist(m_h_FCalEt_vs_ZDC_LG, path, run).ignore();
}

void HIMonitoringEventShapeTool::getZDC(const xAOD::TrigT2ZdcSignalsContainer* TrigZdc_p) {

  double zdc_energies[2] = { 0.0, 0.0 }; // {High gain, Low gain}
  int size = TrigZdc_p->size();

  for (int i = 0; i < size; i++) {
    const xAOD::TrigT2ZdcSignals* zdc = TrigZdc_p->at(i);
    std::vector<float> triggerEnergies = zdc->triggerEnergies();
    for (auto & zdc_towers  : triggerEnergies)
			zdc_energies[i]+= zdc_towers;
  }

  m_ZDC_HG = zdc_energies[0] * 1e-3;
  m_ZDC_LG = zdc_energies[1] * 1e-3;
}

void HIMonitoringEventShapeTool::fillZDC_hist() {
  m_h_FCalEt_vs_ZDC_HG->Fill(m_FCalEt, m_ZDC_HG);
  m_h_FCalEt_vs_ZDC_LG->Fill(m_FCalEt, m_ZDC_LG);
}

/// ***    *** ///
double HIMonitoringEventShapeTool::calc_qn(double qnx, double qny) {
  double qn = qnx * qnx + qny * qny;

  if (qn > 0) qn = sqrt(qn);
  else qn = 0;

  return qn;
}

double HIMonitoringEventShapeTool::calc_psin(int n, double qnx, double qny) {
  n = n + 1;
  double psin = (1. / n) * atan2(qny, qnx);
  return psin;
}

double HIMonitoringEventShapeTool::calc_psin_diff(int n, double psi1, double psi2) {
  n = n + 1;
  double diff = psi1 - psi2;
  if (diff > (m_Pi / n)) {
    return(diff - (2 * m_Pi / n));
  }
  if (diff < (-m_Pi / n)) {
    return(diff + (2 * m_Pi / n));
  }
  if ((diff >= (-m_Pi / n)) &&
      (diff <= (m_Pi / n))) {
    return diff;
  }

  return 999.999;
}

int HIMonitoringEventShapeTool::ES_eta_layer2bin(double eta) {
  int bin = 0;

  bin = (int) (((eta - 0.05) * 10.0) + 50.1);
  return bin;
}

double HIMonitoringEventShapeTool::ES_bin2eta_layer(int bin) {
  double eta = 0.0;

  eta = (double) (((bin - 50.0) / 10.0) + 0.05);
  return eta;
}
