#ifndef IPRESOLSTUDIES_C
#define IPRESOLSTUDIES_C
#include "IPResolStudies.h" 

///////////////////////////
IPResolStudies::IPResolStudies() : m_printLevel (0),
				   m_EtaBins (20),
				   m_EtaLower (-2.5),
				   m_EtaUpper (2.5),
				   m_d0Bins (100),
				   m_d0Lower (-0.1),
				   m_d0Upper (0.1),
                                   m_z0Bins (100),
				   m_z0Lower (-1.0),
				   m_z0Upper (1.0),
				   m_PtBins (17),//22
				   m_PtLower (15.),
                                   m_isMC_IPResol(false), //a hard-coded flag for the IPResolStudies, by default to false
				   m_GaussianFitRange (1.5)
{
  
  h_d0_vs_pt_eta = NULL;
  h_z0_vs_pt_eta = NULL;
  h_entries_vs_pt_eta = NULL;

  /*h_d0_vs_pt_eta_pos = NULL;
  h_z0_vs_pt_eta_neg = NULL;

  h_d0_vs_pt_eta_unfold = NULL;
  h_z0_vs_pt_eta_unfold = NULL;

  h_d0_vs_pt_eta_pos_unfold = NULL;
  h_z0_vs_pt_eta_neg_unfold = NULL;*/

  h_d0sigma = NULL;
  h_z0sigma = NULL;

  /*h_d0sigma_pos = NULL;
  h_z0sigma_neg = NULL;

  h_d0sigma_unfold = NULL;
  h_z0sigma_unfold = NULL;

  h_d0sigma_pos_unfold = NULL;
  h_z0sigma_neg_unfold = NULL;*/

  h_d0sigma_unfold_kpvup = NULL;
  h_d0sigma_unfold_kpvdown = NULL;


  m_PV_d0Sigma_pos = 0.;
  m_PV_z0Sigma_pos = 0.;
  m_PV_d0Sigma_neg = 0.;
  m_PV_z0Sigma_neg = 0.;

  m_RunNumber = 0.;
  
  return;
}

///////////////////////////
IPResolStudies::~IPResolStudies()
{
}

///////////////////////////
void IPResolStudies::BookHistograms()
{
  float vector_etavalues[m_EtaBins+1];
  this->FormVectorEtaValues(vector_etavalues);

  float vector_d0values[m_d0Bins+1];
  this->FormVectord0Values(vector_d0values);

  float vector_z0values[m_z0Bins+1];
  this->FormVectorz0Values(vector_z0values);

  float vector_ptvalues[m_PtBins+1];
  this->FormVectorPtValues(vector_ptvalues);

  if (m_printLevel >= 10) {
    std::cout << " ** IPResolStudies::BookHistograms ** " << std::endl;  
    for (int i=0; i <= m_EtaBins; i++) {
      std::cout << "    eta[" <<i << "] = " << vector_etavalues[i] << std::endl;
    }
    for (int i=0; i <= m_d0Bins; i++) {
      std::cout << "    d0[" <<i << "] = " << vector_d0values[i] << std::endl;
    }
    for (int i=0; i <= m_z0Bins; i++) {
      std::cout << "    z0[" <<i << "] = " << vector_z0values[i] << std::endl;
    }
    for (int i=0; i <= m_PtBins; i++) {
      std::cout << "    Pt[" <<i << "] = " << vector_ptvalues[i] << std::endl;
    }

  }

  h_entries_vs_pt_eta = new TH2D("hentries_pt_eta","Entries per #eta-p_{T}; p_{T} [GeV];#eta", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);

  //Reconstructed
  h_d0_vs_pt_eta = new TH3F ("h_d0_vs_pt_eta","d0 vs pt-eta; p_{T} [GeV]; #eta; d_{0} [mm]", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues, m_d0Bins, vector_d0values);
  h_d0_vs_pt_eta_pos = new TH3F ("h_d0_vs_pt_eta_pos","mu pos: d0 vs pt-eta; p_{T} [GeV]; #eta; d_{0} [mm]", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues, m_d0Bins, vector_d0values);
  h_d0_vs_pt_eta_neg = new TH3F ("h_d0_vs_pt_eta_neg","mu neg: d0 vs pt-eta; p_{T} [GeV]; #eta; d_{0} [mm]", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues, m_d0Bins, vector_d0values);

  h_z0_vs_pt_eta = new TH3F ("h_z0_vs_pt_eta","z0 vs pt-eta; p_{T} [GeV]; #eta; z_{0} [mm]", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues, m_z0Bins, vector_z0values);
  h_z0_vs_pt_eta_pos = new TH3F ("h_z0_vs_pt_eta_pos","mu pos: z0 vs pt-eta", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues, m_z0Bins, vector_z0values);
  h_z0_vs_pt_eta_neg = new TH3F ("h_z0_vs_pt_eta_neg","mu neg: z0 vs pt-eta", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues, m_z0Bins, vector_z0values);


  h_d0sigma = new TH2D("histd0_sigma_reco","#sigma_{d_{0}} measured map;p_{T} [GeV];#eta;[mm]", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_d0sigma_pos = new TH2D("histd0_sigma_reco_pos","#mu^{+}: #sigma_{d_{0}} measured map;p_{T} [GeV];#eta", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_d0sigma_neg = new TH2D("histd0_sigma_reco_neg","#mu^{-}: #sigma_{d_{0}} measured map;p_{T} [GeV];#eta", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_z0sigma = new TH2D("histz0_sigma_reco","#sigma_{z_{0}} measured map;p_{T} [GeV];#eta", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_z0sigma_pos = new TH2D("histz0_sigma_reco_pos","#mu^{+}: #sigma_{z_{0}} measured map;p_{T} [GeV];#eta", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_z0sigma_neg = new TH2D("histz0_sigma_reco_neg","#mu^{-}: #sigma_{z_{0}} measured map;p_{T} [GeV];#eta", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);

		       
  // unfolded
  h_d0_vs_pt_eta_unfold = new TH3F ("h_d0_vs_pt_eta_unfold","unfold d0 vs pt-eta; p_{T} [GeV]; #eta; d_{0} [mm]", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues, m_d0Bins, vector_d0values);
  h_d0_vs_pt_eta_pos_unfold = new TH3F ("h_d0_vs_pt_eta_pos_unfold","mu pos unfold: d0 vs pt-eta; p_{T} [GeV]; #eta; d_{0} [mm]", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues, m_d0Bins, vector_d0values);
  h_d0_vs_pt_eta_neg_unfold = new TH3F ("h_d0_vs_pt_eta_neg_unfold","mu neg unfold: d0 vs pt-eta; p_{T} [GeV]; #eta; d_{0} [mm]", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues, m_d0Bins, vector_d0values);

  h_z0_vs_pt_eta_unfold = new TH3F ("h_z0_vs_pt_eta_unfold","unfold z0 vs pt-eta; p_{T} [GeV]; #eta; z_{0} [mm]", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues, m_z0Bins, vector_z0values);
  h_z0_vs_pt_eta_pos_unfold = new TH3F ("h_z0_vs_pt_eta_pos_unfold","mu pos unfold: z0 vs pt-eta; p_{T} [GeV]; #eta; z_{0} [mm]", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues, m_z0Bins, vector_z0values);
  h_z0_vs_pt_eta_neg_unfold = new TH3F ("h_z0_vs_pt_eta_neg_unfold","mu neg unfold: z0 vs pt-eta; p_{T} [GeV]; #eta; z_{0} [mm]", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues, m_z0Bins, vector_z0values);

  h_d0sigma_unfold = new TH2D("histd0_sigma_unfold","Unfolded #sigma_{d_{0}} map;p_{T} [GeV];#eta; [mm]", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_d0sigma_pos_unfold = new TH2D("histd0_sigma_pos_unfold","#mu^{+}: unfolded #sigma_{d_{0}} map;p_{T} [GeV];#eta", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_d0sigma_neg_unfold = new TH2D("histd0_sigma_neg_unfold","#mu^{-}: unfolded #sigma_{d_{0}} map;p_{T} [GeV];#eta", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_z0sigma_unfold = new TH2D("histz0_sigma_unfold","Unfolded #sigma_{z_{0}} map;p_{T} [GeV];#eta", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_z0sigma_pos_unfold = new TH2D("histz0_sigma_pos_unfold","#mu^{+}: unfolded #sigma_{z_{0}} map;p_{T} [GeV];#eta", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_z0sigma_neg_unfold = new TH2D("histz0_sigma_neg_unfold","#mu^{-}: unfolded #sigma_{z_{0}} map;p_{T} [GeV];#eta", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);

  // unfolding using kpv uncertainties
  h_d0_vs_pt_eta_unfold_kpvup = new TH3F ("h_d0_vs_pt_eta_unfold_kpvup","unfold d0 vs pt-eta (#sigma(K_{PV}) up); p_{T} [GeV]; #eta; d_{0} [mm]", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues, m_d0Bins, vector_d0values);
  h_d0_vs_pt_eta_unfold_kpvdown = new TH3F ("h_d0_vs_pt_eta_unfold_kpvdown","unfold d0 vs pt-eta (#sigma(K_{PV}) down); p_{T} [GeV]; #eta; d_{0} [mm]", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues, m_d0Bins, vector_d0values);
  h_d0sigma_unfold_kpvup = new TH2D("histd0_sigma_unfold_kpvup","Unfolded #sigma_{d_{0}} map (#sigma(K_{PV}) up);p_{T} [GeV];#eta; [mm]", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_d0sigma_unfold_kpvdown = new TH2D("histd0_sigma_unfold_kpvdown","Unfolded #sigma_{d_{0}} map (#sigma(K_{PV}) down);p_{T} [GeV];#eta; [mm]", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);

  h_z0_vs_pt_eta_unfold_kpvup = new TH3F ("h_z0_vs_pt_eta_unfold_kpvup","unfold z0 vs pt-eta (#sigma(K_{PV}) up); p_{T} [GeV]; #eta; z_{0} [mm]", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues, m_z0Bins, vector_z0values);
  h_z0_vs_pt_eta_unfold_kpvdown = new TH3F ("h_z0_vs_pt_eta_unfold_kpvdown","unfold z0 vs pt-eta (#sigma(K_{PV}) down); p_{T} [GeV]; #eta; z_{0} [mm]", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues, m_z0Bins, vector_z0values);
  h_z0sigma_unfold_kpvup = new TH2D("histz0_sigma_unfold_kpvup","Unfolded #sigma_{z_{0}} map (#sigma(K_{PV}) up);p_{T} [GeV];#eta", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_z0sigma_unfold_kpvdown = new TH2D("histz0_sigma_unfold_kpvdown","Unfolded #sigma_{z_{0}} map (#sigma(K_{PV}) down);p_{T} [GeV];#eta", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);

  // unfolding using ktrk uncertainties
  h_d0_vs_pt_eta_unfold_ktrkup = new TH3F ("h_d0_vs_pt_eta_unfold_ktrkup","unfold d0 vs pt-eta (#sigma(K_{trk}) up); p_{T} [GeV]; #eta; d_{0} [mm]", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues, m_d0Bins, vector_d0values);
  h_d0_vs_pt_eta_unfold_ktrkdown = new TH3F ("h_d0_vs_pt_eta_unfold_ktrkdown","unfold d0 vs pt-eta (#sigma(K_{trk}) down); p_{T} [GeV]; #eta; d_{0} [mm]", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues, m_d0Bins, vector_d0values);
  h_d0sigma_unfold_ktrkup = new TH2D("histd0_sigma_unfold_ktrkup","Unfolded #sigma_{d_{0}} map (#sigma(K_{trk}) up);p_{T} [GeV];#eta; [mm]", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_d0sigma_unfold_ktrkdown = new TH2D("histd0_sigma_unfold_ktrkdown","Unfolded #sigma_{d_{0}} map (#sigma(K_{trk}) down);p_{T} [GeV];#eta; [mm]", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);

  h_z0_vs_pt_eta_unfold_ktrkup = new TH3F ("h_z0_vs_pt_eta_unfold_ktrkup","unfold z0 vs pt-eta (#sigma(K_{trk}) up); p_{T} [GeV]; #eta; z_{0} [mm]", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues, m_z0Bins, vector_z0values);
  h_z0_vs_pt_eta_unfold_ktrkdown = new TH3F ("h_z0_vs_pt_eta_unfold_ktrkdown","unfold z0 vs pt-eta (#sigma(K_{trk}) down); p_{T} [GeV]; #eta; z_{0} [mm]", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues, m_z0Bins, vector_z0values);
  h_z0sigma_unfold_ktrkup = new TH2D("histz0_sigma_unfold_ktrkup","Unfolded #sigma_{z_{0}} map (#sigma(K_{trk}) up);p_{T} [GeV];#eta", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_z0sigma_unfold_ktrkdown = new TH2D("histz0_sigma_unfold_ktrkdown","Unfolded #sigma_{z_{0}} map (#sigma(K_{trk}) down);p_{T} [GeV];#eta", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);

  // wide fit
  h_d0sigma_unfold_widerfit = new TH2D("histd0_sigma_unfold_widerfit","Unfolded #sigma_{d_{0}} map (widerfit);p_{T} [GeV];#eta", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_z0sigma_unfold_widerfit = new TH2D("histz0_sigma_unfold_widerfit","Unfolded #sigma_{z_{0}} map (widerfit);p_{T} [GeV];#eta", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);



  // K-trk histograms
  h_Ktrk_d0_pt_eta = new TH2D("histktrk_d0_pt_eta","d0 K_{trk} map;p_{T} [GeV];#eta", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_Ktrk_z0_pt_eta = new TH2D("histktrk_z0_pt_eta","z0 K_{trk} map;p_{T} [GeV];#eta", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
 
  // mean histograms
  h_d0mean = new TH2D("histd0_mean","Reco mean d_{0} map;p_{T} [GeV];#eta; [mm]", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_z0mean = new TH2D("histz0_mean","Reco mean z_{0} map;p_{T} [GeV];#eta; [mm]", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_d0mean_unfold = new TH2D("histd0_mean_unfold","Unfold mean d_{0} map;p_{T} [GeV];#eta; [mm]", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_z0mean_unfold = new TH2D("histz0_mean_unfold","Unfold mean z_{0} map;p_{T} [GeV];#eta; [mm]", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_d0mean_neg = new TH2D("histd0_mean_neg","#mu^{-}: Reco mean d_{0} map;p_{T} [GeV];#eta; [mm]", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_z0mean_neg = new TH2D("histz0_mean_neg","#mu^{-}: Reco mean z_{0} map;p_{T} [GeV];#eta; [mm]", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_d0mean_neg_unfold = new TH2D("histd0_mean_neg_unfold","#mu^{-}: Unfold mean d_{0} map;p_{T} [GeV];#eta; [mm]", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_z0mean_neg_unfold = new TH2D("histz0_mean_neg_unfold","#mu^{-}: Unfold mean z_{0} map;p_{T} [GeV];#eta; [mm]", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_d0mean_pos = new TH2D("histd0_mean_pos","#mu^{+}: Reco mean d_{0} map;p_{T} [GeV];#eta; [mm]", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_z0mean_pos = new TH2D("histz0_mean_pos","#mu^{+}: Reco mean z_{0} map;p_{T} [GeV];#eta; [mm]", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_d0mean_pos_unfold = new TH2D("histd0_mean_pos_unfold","#mu^{+}: Unfold mean d_{0} map;p_{T} [GeV];#eta; [mm]", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_z0mean_pos_unfold = new TH2D("histz0_mean_pos_unfold","#mu^{+}: Unfold mean z_{0} map;p_{T} [GeV];#eta; [mm]", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_d0mean_unfold_kpvup = new TH2D("histd0_mean_unfold_kpvup","#mu^{+}: Unfold mean d_{0} map (K_{PV} up);p_{T} [GeV];#eta; [mm]", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_d0mean_unfold_kpvdown = new TH2D("histd0_mean_unfold_kpvdown","#mu^{+}: Unfold mean d_{0} map (K_{PV} down);p_{T} [GeV];#eta; [mm]", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_z0mean_unfold_kpvup = new TH2D("histz0_mean_unfold_kpvup","Unfold mean z_{0} map (#sigma(K_{PV}) up);p_{T} [GeV];#eta; [mm]", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_z0mean_unfold_kpvdown = new TH2D("histz0_mean_unfold_kpvdown","Unfold mean z_{0} map (#sigma(K_{PV}) down);p_{T} [GeV];#eta; [mm]", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_d0mean_unfold_ktrkup = new TH2D("histd0_mean_unfold_ktrkup","#mu^{+}: Unfold mean d_{0} map (K_{trk} up);p_{T} [GeV];#eta; [mm]", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_d0mean_unfold_ktrkdown = new TH2D("histd0_mean_unfold_ktrkdown","#mu^{+}: Unfold mean d_{0} map (K_{trk} down);p_{T} [GeV];#eta; [mm]", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_z0mean_unfold_ktrkup = new TH2D("histz0_mean_unfold_ktrkup","Unfold mean z_{0} map (#sigma(K_{trk}) up);p_{T} [GeV];#eta; [mm]", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_z0mean_unfold_ktrkdown = new TH2D("histz0_mean_unfold_ktrkdown","Unfold mean z_{0} map (#sigma(K_{trk}) down);p_{T} [GeV];#eta; [mm]", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);

  h_d0mean_unfold_widerfit = new TH2D("histd0_mean_unfold_widerfit","Unfold mean d_{0} map (widerfit);p_{T} [GeV];#eta; [mm]", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_z0mean_unfold_widerfit = new TH2D("histz0_mean_unfold_widerfit","Unfold mean z_{0} map (widerfit);p_{T} [GeV];#eta; [mm]", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);

  // chi2 histograms
  h_d0chi2 = new TH2D("histd0_chi2","#chi^{2}/ndf d_{0} map;p_{T} [GeV];#eta;", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_z0chi2 = new TH2D("histz0_chi2","#chi^{2}/ndf z_{0} map;p_{T} [GeV];#eta;", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_d0chi2_unfold = new TH2D("histd0_chi2_unfold","#chi^{2}/ndf Unfold  d_{0} map;p_{T} [GeV];#eta;", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_z0chi2_unfold = new TH2D("histz0_chi2_unfold","#chi^{2}/ndf Unfold mean z_{0} map;p_{T} [GeV];#eta;", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_d0chi2_neg = new TH2D("histd0_chi2_neg","#chi^{2}/ndf #mu^{-}: Reco mean d_{0} map;p_{T} [GeV];#eta;", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_z0chi2_neg = new TH2D("histz0_chi2_neg","#chi^{2}/ndf #mu^{-}: Reco mean z_{0} map;p_{T} [GeV];#eta;", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_d0chi2_neg_unfold = new TH2D("histd0_chi2_neg_unfold","#chi^{2}/ndf #mu^{-}: Unfold mean d_{0} map;p_{T} [GeV];#eta;", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_z0chi2_neg_unfold = new TH2D("histz0_chi2_neg_unfold","#chi^{2}/ndf #mu^{-}: Unfold mean z_{0} map;p_{T} [GeV];#eta;", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_d0chi2_pos = new TH2D("histd0_chi2_pos","#chi^{2}/ndf #mu^{+}: Reco mean d_{0} map;p_{T} [GeV];#eta;", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_z0chi2_pos = new TH2D("histz0_chi2_pos","#chi^{2}/ndf #mu^{+}: Reco mean z_{0} map;p_{T} [GeV];#eta;", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_d0chi2_pos_unfold = new TH2D("histd0_chi2_pos_unfold","#chi^{2}/ndf #mu^{+}: Unfold mean d_{0} map;p_{T} [GeV];#eta;", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_z0chi2_pos_unfold = new TH2D("histz0_chi2_pos_unfold","#chi^{2}/ndf #mu^{+}: Unfold mean z_{0} map;p_{T} [GeV];#eta;", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_d0chi2_unfold_kpvup = new TH2D("histd0_chi2_unfold_kpvup","#chi^{2}/ndf Unfold  d_{0} map (K_{PV} up);p_{T} [GeV];#eta;", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_d0chi2_unfold_kpvdown = new TH2D("histd0_chi2_unfold_kpvdown","#chi^{2}/ndf Unfold  d_{0} map (K_{PV} down);p_{T} [GeV];#eta;", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_z0chi2_unfold_kpvup = new TH2D("histz0_chi2_unfold_kpvup","#chi^{2}/ndf Unfold mean z_{0} map (#sigma(K_{PV}) up);p_{T} [GeV];#eta;", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_z0chi2_unfold_kpvdown = new TH2D("histz0_chi2_unfold_kpvdown","#chi^{2}/ndf Unfold mean z_{0} map (#sigma(K_{PV}) down);p_{T} [GeV];#eta;", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_d0chi2_unfold_ktrkup = new TH2D("histd0_chi2_unfold_ktrkup","#chi^{2}/ndf Unfold mean d_{0} map (#sigma(K_{trk}) up);p_{T} [GeV];#eta;", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_d0chi2_unfold_ktrkdown = new TH2D("histd0_chi2_unfold_ktrkdown","#chi^{2}/ndf Unfold  d_{0} map (K_{trk} down);p_{T} [GeV];#eta;", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_z0chi2_unfold_ktrkup = new TH2D("histz0_chi2_unfold_ktrkup","#chi^{2}/ndf Unfold mean z_{0} map (#sigma(K_{trk}) up);p_{T} [GeV];#eta;", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_z0chi2_unfold_ktrkdown = new TH2D("histz0_chi2_unfold_ktrkdown","#chi^{2}/ndf Unfold mean z_{0} map (#sigma(K_{trk}) down);p_{T} [GeV];#eta;", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);

  h_d0chi2_unfold_widerfit = new TH2D("histd0_chi2_unfold_widerfit","#chi^{2}/ndf Unfold mean d_{0} map (widerfit);p_{T} [GeV];#eta;", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);
  h_z0chi2_unfold_widerfit = new TH2D("histz0_chi2_unfold_widerfit","#chi^{2}/ndf Unfold mean z_{0} map (widerfit);p_{T} [GeV];#eta;", m_PtBins, vector_ptvalues, m_EtaBins, vector_etavalues);

  return;
}

///////////////////////////
void IPResolStudies::FillHistograms()
{
  if (m_printLevel >= 10) std::cout << " ** IPResolStudies::FillHistograms ** mu+ pt " <<  m_mupos_4mom.Pt() 
				    << "  eta: " <<  m_mupos_4mom.Eta() 
				    << "  d0: " << m_mupos_d0 
				    << "  z0: " << m_mupos_z0 << std::endl;
  h_d0_vs_pt_eta->Fill( m_mupos_4mom.Pt(),  m_mupos_4mom.Eta(), m_mupos_d0);
  h_d0_vs_pt_eta->Fill( m_muneg_4mom.Pt(),  m_muneg_4mom.Eta(), m_muneg_d0);

  h_d0_vs_pt_eta_pos->Fill( m_mupos_4mom.Pt(),  m_mupos_4mom.Eta(), m_mupos_d0);
  h_d0_vs_pt_eta_neg->Fill( m_muneg_4mom.Pt(),  m_muneg_4mom.Eta(), m_muneg_d0);

  h_z0_vs_pt_eta->Fill( m_mupos_4mom.Pt(),  m_mupos_4mom.Eta(), m_mupos_z0);
  h_z0_vs_pt_eta->Fill( m_muneg_4mom.Pt(),  m_muneg_4mom.Eta(), m_muneg_z0);

  h_z0_vs_pt_eta_pos->Fill( m_mupos_4mom.Pt(),  m_mupos_4mom.Eta(), m_mupos_z0);
  h_z0_vs_pt_eta_neg->Fill( m_muneg_4mom.Pt(),  m_muneg_4mom.Eta(), m_muneg_z0);

  h_entries_vs_pt_eta->Fill (m_mupos_4mom.Pt(),  m_mupos_4mom.Eta());
  h_entries_vs_pt_eta->Fill (m_muneg_4mom.Pt(),  m_muneg_4mom.Eta());

  // unfolded
  float unfoldFactor = 1;
  float K_PV_d0 = 1;
  float K_PV_z0 = 1;
  float K_PV_d0_err = 0.;
  float K_PV_z0_err = 0.;

  K_PV_d0 = this->GetKPVFactor(m_RunNumber, IPResolStudies::D0, IPResolStudies::CENTRAL_VALUE);
  K_PV_z0 = this->GetKPVFactor(m_RunNumber, IPResolStudies::Z0, IPResolStudies::CENTRAL_VALUE);

  // Ktrack factors
  float K_trk_d0_pos, K_trk_z0_pos = 1., K_trk_d0_neg, K_trk_z0_neg = 1.;
  
  K_trk_d0_pos = this->GetKtrkFactor(m_mupos_4mom.Pt(), m_mupos_4mom.Eta(), IPResolStudies::D0, IPResolStudies::CENTRAL_VALUE);
  K_trk_d0_neg = this->GetKtrkFactor(m_muneg_4mom.Pt(), m_muneg_4mom.Eta(), IPResolStudies::D0, IPResolStudies::CENTRAL_VALUE);
  K_trk_z0_pos = this->GetKtrkFactor(m_mupos_4mom.Pt(), m_mupos_4mom.Eta(), IPResolStudies::Z0, IPResolStudies::CENTRAL_VALUE);
  K_trk_z0_neg = this->GetKtrkFactor(m_muneg_4mom.Pt(), m_muneg_4mom.Eta(), IPResolStudies::Z0, IPResolStudies::CENTRAL_VALUE);
  

  unfoldFactor = this->ComputeUnfoldFactor(K_PV_d0, K_trk_d0_pos, m_PV_d0Sigma_pos, m_mupos_d0Err);
  h_d0_vs_pt_eta_unfold->Fill( m_mupos_4mom.Pt(),  m_mupos_4mom.Eta(), m_mupos_d0 * unfoldFactor);
  h_d0_vs_pt_eta_pos_unfold->Fill( m_mupos_4mom.Pt(), m_mupos_4mom.Eta(), m_mupos_d0 * unfoldFactor);

  unfoldFactor = this->ComputeUnfoldFactor(K_PV_d0, K_trk_d0_neg, m_PV_d0Sigma_neg, m_muneg_d0Err);
  h_d0_vs_pt_eta_unfold->Fill( m_muneg_4mom.Pt(),  m_muneg_4mom.Eta(), m_muneg_d0 * unfoldFactor);
  h_d0_vs_pt_eta_neg_unfold->Fill( m_muneg_4mom.Pt(), m_muneg_4mom.Eta(), m_muneg_d0 * unfoldFactor);

  unfoldFactor = this->ComputeUnfoldFactor(K_PV_z0, K_trk_z0_pos, m_PV_z0Sigma_pos, m_mupos_z0Err);
  h_z0_vs_pt_eta_unfold->Fill( m_mupos_4mom.Pt(),  m_mupos_4mom.Eta(), m_mupos_z0 * unfoldFactor);
  h_z0_vs_pt_eta_pos_unfold->Fill( m_mupos_4mom.Pt(), m_mupos_4mom.Eta(), m_mupos_z0 * unfoldFactor);

  unfoldFactor = this->ComputeUnfoldFactor(K_PV_z0, K_trk_z0_neg, m_PV_z0Sigma_neg, m_muneg_z0Err);
  h_z0_vs_pt_eta_unfold->Fill( m_muneg_4mom.Pt(),  m_muneg_4mom.Eta(), m_muneg_z0 * unfoldFactor);
  h_z0_vs_pt_eta_neg_unfold->Fill( m_muneg_4mom.Pt(), m_muneg_4mom.Eta(), m_muneg_z0 * unfoldFactor);

  
  // using Kpv uncertainties up 
  K_PV_d0 = this->GetKPVFactor(m_RunNumber, IPResolStudies::D0, IPResolStudies::UNCERT_UP);
  K_PV_z0 = this->GetKPVFactor(m_RunNumber, IPResolStudies::Z0, IPResolStudies::UNCERT_UP);

  unfoldFactor = this->ComputeUnfoldFactor(K_PV_d0, K_trk_d0_pos, m_PV_d0Sigma_pos, m_mupos_d0Err);
  h_d0_vs_pt_eta_unfold_kpvup->Fill( m_mupos_4mom.Pt(),  m_mupos_4mom.Eta(), m_mupos_d0 * unfoldFactor);
  unfoldFactor = this->ComputeUnfoldFactor(K_PV_d0, K_trk_d0_neg, m_PV_d0Sigma_neg, m_muneg_d0Err);
  h_d0_vs_pt_eta_unfold_kpvup->Fill( m_muneg_4mom.Pt(),  m_muneg_4mom.Eta(), m_muneg_d0 * unfoldFactor);

  unfoldFactor = this->ComputeUnfoldFactor(K_PV_z0, K_trk_z0_pos, m_PV_z0Sigma_pos, m_mupos_z0Err);
  h_z0_vs_pt_eta_unfold_kpvup->Fill( m_mupos_4mom.Pt(),  m_mupos_4mom.Eta(), m_mupos_z0 * unfoldFactor);
  unfoldFactor = this->ComputeUnfoldFactor(K_PV_z0, K_trk_z0_neg, m_PV_z0Sigma_neg, m_muneg_z0Err);
  h_z0_vs_pt_eta_unfold_kpvup->Fill( m_muneg_4mom.Pt(),  m_muneg_4mom.Eta(), m_muneg_z0 * unfoldFactor);

  // using Kpv uncertainties down 
  K_PV_d0 = this->GetKPVFactor(m_RunNumber, IPResolStudies::D0, IPResolStudies::UNCERT_DOWN);
  K_PV_z0 = this->GetKPVFactor(m_RunNumber, IPResolStudies::Z0, IPResolStudies::UNCERT_DOWN);

  unfoldFactor = this->ComputeUnfoldFactor(K_PV_d0, K_trk_d0_pos, m_PV_d0Sigma_pos, m_mupos_d0Err);
  h_d0_vs_pt_eta_unfold_kpvdown->Fill( m_mupos_4mom.Pt(),  m_mupos_4mom.Eta(), m_mupos_d0 * unfoldFactor);
  unfoldFactor = this->ComputeUnfoldFactor(K_PV_d0, K_trk_d0_neg, m_PV_d0Sigma_neg, m_muneg_d0Err);
  h_d0_vs_pt_eta_unfold_kpvdown->Fill( m_muneg_4mom.Pt(),  m_muneg_4mom.Eta(), m_muneg_d0 * unfoldFactor);

  unfoldFactor = this->ComputeUnfoldFactor(K_PV_z0, K_trk_z0_pos, m_PV_z0Sigma_pos, m_mupos_z0Err);
  h_z0_vs_pt_eta_unfold_kpvdown->Fill( m_mupos_4mom.Pt(),  m_mupos_4mom.Eta(), m_mupos_z0 * unfoldFactor);
  unfoldFactor = this->ComputeUnfoldFactor(K_PV_z0, K_trk_z0_neg, m_PV_z0Sigma_neg, m_muneg_z0Err);
  h_z0_vs_pt_eta_unfold_kpvdown->Fill( m_muneg_4mom.Pt(),  m_muneg_4mom.Eta(), m_muneg_z0 * unfoldFactor);


  // before moving K track factors, reset K_PV factors
  K_PV_d0 = this->GetKPVFactor(m_RunNumber, IPResolStudies::D0, IPResolStudies::CENTRAL_VALUE);
  K_PV_z0 = this->GetKPVFactor(m_RunNumber, IPResolStudies::Z0, IPResolStudies::CENTRAL_VALUE);

  // using Ktrack uncertainties up
  K_trk_d0_pos = this->GetKtrkFactor(m_mupos_4mom.Pt(), m_mupos_4mom.Eta(), IPResolStudies::D0, IPResolStudies::UNCERT_UP);
  K_trk_d0_neg = this->GetKtrkFactor(m_muneg_4mom.Pt(), m_muneg_4mom.Eta(), IPResolStudies::D0, IPResolStudies::UNCERT_UP);
  K_trk_z0_pos = this->GetKtrkFactor(m_mupos_4mom.Pt(), m_mupos_4mom.Eta(), IPResolStudies::Z0, IPResolStudies::UNCERT_UP);
  K_trk_z0_neg = this->GetKtrkFactor(m_muneg_4mom.Pt(), m_muneg_4mom.Eta(), IPResolStudies::Z0, IPResolStudies::UNCERT_UP);

  unfoldFactor = this->ComputeUnfoldFactor(K_PV_d0, K_trk_d0_pos, m_PV_d0Sigma_pos, m_mupos_d0Err);
  h_d0_vs_pt_eta_unfold_ktrkup->Fill( m_mupos_4mom.Pt(),  m_mupos_4mom.Eta(), m_mupos_d0 * unfoldFactor);
  unfoldFactor = this->ComputeUnfoldFactor(K_PV_d0, K_trk_d0_neg, m_PV_d0Sigma_neg, m_muneg_d0Err);
  h_d0_vs_pt_eta_unfold_ktrkup->Fill( m_muneg_4mom.Pt(),  m_muneg_4mom.Eta(), m_muneg_d0 * unfoldFactor);
  unfoldFactor = this->ComputeUnfoldFactor(K_PV_z0, K_trk_z0_pos, m_PV_z0Sigma_pos, m_mupos_z0Err);
  h_z0_vs_pt_eta_unfold_ktrkup->Fill( m_mupos_4mom.Pt(),  m_mupos_4mom.Eta(), m_mupos_z0 * unfoldFactor);
  unfoldFactor = this->ComputeUnfoldFactor(K_PV_z0, K_trk_z0_neg, m_PV_z0Sigma_neg, m_muneg_z0Err);
  h_z0_vs_pt_eta_unfold_ktrkup->Fill( m_muneg_4mom.Pt(),  m_muneg_4mom.Eta(), m_muneg_z0 * unfoldFactor);


  // using Ktrack uncertainties down
  K_trk_d0_pos = this->GetKtrkFactor(m_mupos_4mom.Pt(), m_mupos_4mom.Eta(), IPResolStudies::D0, IPResolStudies::UNCERT_DOWN);
  K_trk_d0_neg = this->GetKtrkFactor(m_muneg_4mom.Pt(), m_muneg_4mom.Eta(), IPResolStudies::D0, IPResolStudies::UNCERT_DOWN);
  K_trk_z0_pos = this->GetKtrkFactor(m_mupos_4mom.Pt(), m_mupos_4mom.Eta(), IPResolStudies::Z0, IPResolStudies::UNCERT_DOWN);
  K_trk_z0_neg = this->GetKtrkFactor(m_muneg_4mom.Pt(), m_muneg_4mom.Eta(), IPResolStudies::Z0, IPResolStudies::UNCERT_DOWN);

  unfoldFactor = this->ComputeUnfoldFactor(K_PV_d0, K_trk_d0_pos, m_PV_d0Sigma_pos, m_mupos_d0Err);
  h_d0_vs_pt_eta_unfold_ktrkdown->Fill( m_mupos_4mom.Pt(),  m_mupos_4mom.Eta(), m_mupos_d0 * unfoldFactor);
  unfoldFactor = this->ComputeUnfoldFactor(K_PV_d0, K_trk_d0_neg, m_PV_d0Sigma_neg, m_muneg_d0Err);
  h_d0_vs_pt_eta_unfold_ktrkdown->Fill( m_muneg_4mom.Pt(),  m_muneg_4mom.Eta(), m_muneg_d0 * unfoldFactor);
  unfoldFactor = this->ComputeUnfoldFactor(K_PV_z0, K_trk_z0_pos, m_PV_z0Sigma_pos, m_mupos_z0Err);
  h_z0_vs_pt_eta_unfold_ktrkdown->Fill( m_mupos_4mom.Pt(),  m_mupos_4mom.Eta(), m_mupos_z0 * unfoldFactor);
  unfoldFactor = this->ComputeUnfoldFactor(K_PV_z0, K_trk_z0_neg, m_PV_z0Sigma_neg, m_muneg_z0Err);
  h_z0_vs_pt_eta_unfold_ktrkdown->Fill( m_muneg_4mom.Pt(),  m_muneg_4mom.Eta(), m_muneg_z0 * unfoldFactor);



  return;
		       
}

///////////////////////////
void IPResolStudies::SetMuPosTrackparams (TLorentzVector * newfourmom, double newd0, double newz0, double newd0Err, double newz0Err)
{
  double MeVtoGeV = 0.001;
  m_mupos_4mom.SetXYZM(newfourmom->Px()*MeVtoGeV, newfourmom->Py()*MeVtoGeV, newfourmom->Pz()*MeVtoGeV, newfourmom->M()*MeVtoGeV);
  m_mupos_d0 = newd0;
  m_mupos_z0 = newz0;
  m_mupos_d0Err = newd0Err;
  m_mupos_z0Err = newz0Err;

  if (m_printLevel >= 10) {
    std::cout << " ** IPResolStudies::SetMuPosTrackparams ** px: " << m_mupos_4mom.Px() 
	      << "  py: " <<  m_mupos_4mom.Py()
	      << "  pz: " <<  m_mupos_4mom.Pz()
	      << "  m: " <<  m_mupos_4mom.M()
	      << "  d0: " << m_mupos_d0
	      << "  z0: " << m_mupos_z0
	      << "  d0Err: " << m_mupos_d0Err
	      << "  z0Err: " << m_mupos_z0Err
	      << std::endl;
  }
  return;
}

///////////////////////////
void IPResolStudies::SetMuNegTrackparams (TLorentzVector *  newfourmom, double newd0, double newz0, double newd0Err, double newz0Err)
{
  double MeVtoGeV = 0.001;
  m_muneg_4mom.SetXYZM(newfourmom->Px()*MeVtoGeV, newfourmom->Py()*MeVtoGeV, newfourmom->Pz()*MeVtoGeV, newfourmom->M()*MeVtoGeV);
  m_muneg_d0 = newd0;
  m_muneg_z0 = newz0;
  m_muneg_d0Err = newd0Err;
  m_muneg_z0Err = newz0Err;

  return;
}

///////////////////////////
void IPResolStudies::SetPVParams (double possigmad0, double possigmaz0, double negsigmad0, double negsigmaz0)
{
  m_PV_d0Sigma_pos = possigmad0;
  m_PV_z0Sigma_pos = possigmaz0;
  m_PV_d0Sigma_neg = negsigmad0;
  m_PV_z0Sigma_neg = negsigmaz0;

  if (m_printLevel >= 10) std::cout << " ** IPResolStudies::SetPVParams ** d0SigmaPos = " << m_PV_d0Sigma_pos 
				    << "  d0SigmaNeg= " << m_PV_d0Sigma_neg
				    << "  z0SigmaPos= " << m_PV_z0Sigma_pos 
				    << "  z0SigmaNeg= " << m_PV_z0Sigma_neg 
				    << std::endl;
  return;
}

///////////////////////////
void IPResolStudies::SetRunNumber (double runnumber)
{
  m_RunNumber = runnumber;
  
  if (m_printLevel >= 10) std::cout << " ** IPResolStudies::SetRunNumber ** RunNumber: " << m_RunNumber 
                                   << std::endl;
  return;
}

///////////////////////////
void IPResolStudies::SetInputIteration (int iteration)
{
  m_Iteration = iteration;
  
  if (m_printLevel >= 10) std::cout << " ** IPResolStudies::SetInputIteration ** Iteration: " << m_Iteration 
                                   << std::endl;
  return;
}

///////////////////////////
void IPResolStudies::FormVectorEtaValues (float * theVector)
{
  float theGap = (m_EtaUpper - m_EtaLower)/m_EtaBins;

  for (int i=0; i <= m_EtaBins; i++) {
    theVector[i] = m_EtaLower + i * theGap;
  }

  if (m_printLevel >= 10) {
    std::cout << " ** IPResolStudies::FormVectorEtaValues ** " << std::endl;  
    for (int i=0; i <= m_EtaBins; i++) {
      std::cout << "    eta[" <<i << "] = " << theVector[i] << std::endl;
    }
  }
  return;
}

///////////////////////////
void IPResolStudies::FormVectord0Values (float * theVector)
{
  float theGap = (m_d0Upper - m_d0Lower)/m_d0Bins;

  for (int i=0; i <= m_d0Bins; i++) {
    theVector[i] = m_d0Lower + i * theGap;
  }

  if (m_printLevel >= 10) {
    std::cout << " ** IPResolStudies::FormVectod0Values ** " << std::endl;  
    for (int i=0; i <= m_EtaBins; i++) {
      std::cout << "    d0[" <<i << "] = " << theVector[i] << std::endl;
    }
  }
  return;
}

///////////////////////////
void IPResolStudies::FormVectorz0Values (float * theVector)
{
  float theGap = (m_z0Upper - m_z0Lower)/m_z0Bins;

  for (int i=0; i <= m_z0Bins; i++) {
    theVector[i] = m_z0Lower + i * theGap;
  }

  if (m_printLevel >= 10) {
    std::cout << " ** IPResolStudies::FormVectorz0Values ** " << std::endl;  
    for (int i=0; i <= m_EtaBins; i++) {
      std::cout << "    z0[" <<i << "] = " << theVector[i] << std::endl;
    }
  }
  return;
}

///////////////////////////
void IPResolStudies::FormVectorPtValues (float * theVector)
{
  // fill with default values:

  for (int i=0; i <= m_PtBins; i++) {
    theVector[i] = 0.;
  }

  // give values
  
  float theGap = 5;
  float thePt = m_PtLower;
  for (int i=0; i <= m_PtBins; i++) {
    if (i == 0) { thePt = m_PtLower-theGap;}
    if (thePt < 50) theGap = 5;
    //if ( 50 <= thePt && thePt < 150) theGap = 10;
    if ( 50 <= thePt && thePt < 100) theGap = 10;
    if ( 100 <= thePt && thePt < 200) theGap = 25;
    if ( thePt >= 200) theGap = 50; 
    //if (150 <= thePt) theGap = 40;//20
    thePt = thePt + theGap;
    theVector[i] = thePt; 
  }

  if (m_printLevel >= 10) {
    std::cout << " ** IPResolStudies::FormVectod0Values ** " << std::endl;  
    for (int i=0; i <= m_PtBins; i++) {
      std::cout << "    Pt[" <<i << "] = " << theVector[i] << std::endl;
    }
  }
  return;
}

///////////////////////////
bool IPResolStudies::IterativeGaussianFit (TH1D * theHist)
{
  bool goodFit = true;
  float nIters = 2;
  float theRMS = theHist->GetRMS();
  float theMean = 0.;
  
  if (theHist->GetEntries() < 100) goodFit = false;
  TF1 *f = new TF1("fgaus", "gaus", theMean -m_GaussianFitRange * theRMS, theMean + m_GaussianFitRange * theRMS);
  f->SetLineWidth(2);
  theHist->SetMarkerStyle(kFullCircle);
  theHist->SetMarkerSize(1.0);
  theHist->SetLineColor(theHist->GetMarkerColor());
  if (goodFit == true) {
    for (int iter=1; iter <= nIters; iter++) {
      theHist->Fit(f, "Q","", theMean - m_GaussianFitRange*theRMS, theMean + m_GaussianFitRange*theRMS);
      gStyle->SetOptFit(1);
      //cout << "CHI2/NDF: " << f->GetChisquare()/f->GetNDF() << endl;
      theRMS = theHist->GetFunction("fgaus")->GetParameter(2);
      theMean = theHist->GetFunction("fgaus")->GetParameter(1);
      //f->SetRange(-m_GaussianFitRange * theSigma, m_GaussianFitRange * theSigma);
      //if (f->GetChisquare()/f->GetNDF() > 10) goodFit = false;
    }
  theHist->Draw("pe");
  }
  if (goodFit) {
    //Create a histogram to hold the confidence intervals
    TString hname(theHist->GetName());
    hname.Append("_band");
    TH1D *hint = new TH1D(hname.Data(), "Fitted gaussian with .95 conf.band",  theHist->GetNbinsX(), theHist->GetXaxis()->GetXmin(), theHist->GetXaxis()->GetXmax());
    (TVirtualFitter::GetFitter())->GetConfidenceIntervals(hint);
    //Now the "hint" histogram has the fitted function values as the
    //bin contents and the confidence intervals as bin errors
    hint->SetStats(kFALSE);
    hint->SetFillColorAlpha(f->GetLineColor(), 0.3);
    hint->Draw("pe3 same"); 
  }
  return goodFit;
}


///////////////////////////
float IPResolStudies::ComputeUnfoldFactor(float K_PV, float K_trk, float sigma_PV, float sigma_trk)
{
  if (m_printLevel >= 10) std::cout <<  " **IPResolStudies::ComputeUnfoldFactor() ** START = " 
			      << "  K_trk:" << K_trk 
			      << "  sigma_trk: " << sigma_trk
			      << "  K_PV: " << K_PV
			      << "  sigma_PV: " << sigma_PV
			      << std::endl;

  float theFactor = TMath::Sqrt( TMath::Power(K_trk*sigma_trk, 2.0) / ( TMath::Power(K_trk*sigma_trk, 2.0)+TMath::Power(K_PV*sigma_PV, 2.0) ) );
  if( theFactor>1.) std::cout <<  " **IPResolStudies::ComputeUnfoldFactor() ** WARNINING unfoldFactor = " <<  theFactor
			      << "  K_trk:" << K_trk 
			      << "  sigma_trk: " << sigma_trk
			      << "  K_PV: " << K_PV
			      << "  sigma_PV: " << sigma_PV
			      << std::endl;
  if (m_printLevel >= 10) std::cout <<  " **IPResolStudies::ComputeUnfoldFactor() ** Completed ** unfold factor " <<  theFactor
			      << std::endl;
  return theFactor;
}

///////////////////////////                                                                                                                    
float IPResolStudies::GetKPVFactor(int RunNumber, int trackpar, int selectCase)
{
  float theK_PV = 1;
  float errK_PV = 0;

  if (m_isMC_IPResol) {
    // MC: KPV values from Sarah Boutle, 18/05/15
    float K_PV_d0 = 0.95, K_PV_d0_err = 0.03;
    float K_PV_z0 = 0.98, K_PV_z0_err = 0.02;      
    if(trackpar == IPResolStudies::D0) {
      theK_PV = K_PV_d0;
      errK_PV = K_PV_d0_err;
    }
    if(trackpar == IPResolStudies::Z0) {
      theK_PV = K_PV_z0;
      errK_PV = K_PV_z0_err;
    } 
  }
  else {
    // Real data  Values from Federico Sforza, 21/12/16  
    if (RunNumber >= 296939 && RunNumber <= 300287) {theK_PV = 1.192; errK_PV = 0.036;}
    else if (RunNumber >= 300345 && RunNumber <= 300908) {theK_PV = 1.21; errK_PV = 0.034;}
    else if (RunNumber >= 301912 && RunNumber <= 302393) {theK_PV = 1.235; errK_PV = 0.049;}
    else if (RunNumber >= 302737 && RunNumber <= 303560) {theK_PV = 1.265; errK_PV = 0.034;}
    else if (RunNumber >= 303638 && RunNumber <= 303892) {theK_PV = 1.27; errK_PV = 0.033;}
    else if (RunNumber >= 303943 && RunNumber <= 304494) {theK_PV = 1.261; errK_PV = 0.034;}
    else if (RunNumber >= 305291 && RunNumber <= 306714) {theK_PV = 1.244; errK_PV = 0.032;}
    else if (RunNumber >= 305359 && RunNumber <= 310216) {theK_PV = 1.221; errK_PV = 0.083;}
    else if (RunNumber >= 308979 && RunNumber <= 309166) {theK_PV = 1.193; errK_PV = 0.11;}
    else if (RunNumber >= 307124 && RunNumber <= 308084) {theK_PV = 1.26; errK_PV = 0.030;}
    else if (RunNumber >= 309311 && RunNumber <= 309759) {theK_PV = 1.259; errK_PV = 0.033;}
    else if (RunNumber >= 310015 && RunNumber <= 311287) {theK_PV = 1.256; errK_PV = 0.034;}
  }

  float finalK_PV = theK_PV;
  if (selectCase == IPResolStudies::CENTRAL_VALUE) finalK_PV = theK_PV;
  if (selectCase == IPResolStudies::UNCERT_UP) finalK_PV = theK_PV + errK_PV;
  if (selectCase == IPResolStudies::UNCERT_DOWN) finalK_PV = theK_PV - errK_PV;

  if (m_printLevel >= 10) {
    std::cout << " ** IPResolStudies::GetKPVFactor( " << RunNumber 
	      << ", " << trackpar 
	      << ", " << selectCase  
	      << ") = " << theK_PV;
    if (selectCase == IPResolStudies::UNCERT_UP) std::cout << " + " << errK_PV;
    if (selectCase == IPResolStudies::UNCERT_DOWN) std::cout << " - " << errK_PV;
    std::cout << " --> " << finalK_PV 
	      << std::endl;
  }
  return finalK_PV;
}

///////////////////////////                                                                                                                    
float IPResolStudies::GetKtrkFactor(float pt, float eta, int trackpar, int selectCase)
{
    float theKfactor = 1;
    float finalKfactor = 1;
    float errKfactor = 0.;

  if (m_Iteration == 1) {
    return 1; // no k-track factors yet
  }
  else {
    TString hTempName;
    TH2D * h_temp = NULL;
    if(trackpar == IPResolStudies::D0) {
      hTempName.Clear(); hTempName.Form("/Iteration%d/histktrk_d0_pt_eta",m_Iteration-1);
    }
    if(trackpar == IPResolStudies::Z0) {
      hTempName.Clear(); hTempName.Form("/Iteration%d/histktrk_z0_pt_eta",m_Iteration-1);
    }
    h_temp = (TH2D*)m_tfile->Get(hTempName.Data());
    if (h_temp == NULL) {
      std::cout << " ** IPResolStudies::GetKtrkFactor ** ERROR retrieving " << hTempName.Data() << std::endl;
    }
   
    theKfactor = h_temp->GetBinContent(h_Ktrk_d0_pt_eta->GetXaxis()->FindBin(pt), h_Ktrk_d0_pt_eta->GetYaxis()->FindBin(eta));
    errKfactor = h_temp->GetBinError(h_Ktrk_d0_pt_eta->GetXaxis()->FindBin(pt), h_Ktrk_d0_pt_eta->GetYaxis()->FindBin(eta));
    
    if (selectCase == IPResolStudies::CENTRAL_VALUE) finalKfactor = theKfactor;
    if (selectCase == IPResolStudies::UNCERT_UP) finalKfactor = theKfactor + errKfactor;
    if (selectCase == IPResolStudies::UNCERT_DOWN) finalKfactor = theKfactor - errKfactor;

    if (m_printLevel >= 10) {
    std::cout << " ** IPResolStudies::GetKtrkFactor( " << h_temp->GetName() 
	      << ", " << pt 
 	      << ", " << eta 
	      << ", " << trackpar 
	      << ", " << selectCase  
	      << ") = " << theKfactor;
    if (selectCase == IPResolStudies::UNCERT_UP) std::cout << " + " << errKfactor;
    if (selectCase == IPResolStudies::UNCERT_DOWN) std::cout << " - " << errKfactor;
    std::cout << " --> " << finalKfactor 
	      << std::endl;
  }

 }

  return finalKfactor;
}

///////////////////////////
void IPResolStudies::DrawHistos()
{
  TCanvas *canvas = new TCanvas("canvas","canvas",800,600);
  gPad->SetRightMargin(0.15);
  h_d0sigma->SetStats(kFALSE);
  const Int_t NCont = 50;
  const Int_t NRGBs = 5;
  Double_t stops[NRGBs] = {0.25, 0.50, 0.75, 0.90, 1.00 };
  Double_t red[NRGBs] =   {0.10, 0.99, 0.99, 0.75, 0.50 };
  Double_t green[NRGBs] = {0.99, 0.90, 0.10, 0.10, 0.10 };
  Double_t blue[NRGBs] =  {0.10, 0.25, 0.10, 0.50, 0.90 };
  TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont); 
  gStyle->SetNumberContours(99);
  h_d0sigma->SetMaximum(0.030);
  h_d0sigma->Draw("colz");
  canvas->Print("maptestd0.png");
  
  TCanvas *canvas2 = new TCanvas("canvas2","canvas",800,600);
  gPad->SetRightMargin(0.15);
  h_z0sigma->SetStats(kFALSE);
  //h_z0sigma->SetMaximum(0.050);
  h_z0sigma->Draw("colz");
  canvas2->Print("maptestz0.png"); 
  
  
  //TCanvas *canvas2 = new TCanvas("canvas2","canvas2",800,600);
  //histentries->SetStats(kFALSE);
  //histentries->Draw("colz");
  //canvas2->Print("entriesmaptest.png");  

  return;
}

///////////////////////////
void IPResolStudies::Execute()
{
  if (m_printLevel >= 0) {std::cout << " ** IPResolStudies ** Execute() ** start ** " << std::endl;}
  gErrorIgnoreLevel = kWarning; // this removes the message 

  bool outputFileOpen = false;
  TCanvas *can_projectionfit = new TCanvas("canprojectionfit","distribution fits",1000, 850);

  cout << "Eta bins: " << h_d0sigma->GetYaxis()->GetNbins() << "   Pt bins: " << h_z0sigma->GetXaxis()->GetNbins() << endl;                                                       
  this->SetGaussianFitRange(1.5);
  int numberOfCases = 22;
  for (int theCase=1; theCase <= numberOfCases; theCase++) {
    outputFileOpen = false;
    TH3 * hInput;
    TH2 * hTarget;
    TH2 * hTarget_mean;
    TH2 * hTarget_chi2;
    TString theSubindex;
    switch (theCase) {
    case 1: // d0
      hInput = h_d0_vs_pt_eta;
      hTarget = h_d0sigma;
      hTarget_mean = h_d0mean;
      hTarget_chi2 = h_d0chi2;
      theSubindex.Clear(); theSubindex.Append("_d0");
      break;
    case 2:
      hInput = h_z0_vs_pt_eta;
      hTarget = h_z0sigma;
      hTarget_mean = h_z0mean;
      hTarget_chi2 = h_z0chi2;
      theSubindex.Clear(); theSubindex.Append("_z0");
      break;
    case 3:
      hInput = h_d0_vs_pt_eta_unfold;
      hTarget = h_d0sigma_unfold;
      hTarget_mean = h_d0mean_unfold;
      hTarget_chi2 = h_d0chi2_unfold;
      theSubindex.Clear(); theSubindex.Append("_d0_unfold");
      break;
    case 4:
      hInput = h_z0_vs_pt_eta_unfold;
      hTarget = h_z0sigma_unfold;
      hTarget_mean = h_z0mean_unfold;
      hTarget_chi2 = h_z0chi2_unfold;
      theSubindex.Clear(); theSubindex.Append("_z0_unfold");
      break;
    case 5:
      hInput = h_d0_vs_pt_eta_pos;
      hTarget = h_d0sigma_pos;
      hTarget_mean = h_d0mean_pos;
      hTarget_chi2 = h_d0chi2_pos;
      theSubindex.Clear(); theSubindex.Append("_d0_pos");
      break;
    case 6:
      hInput = h_d0_vs_pt_eta_neg;
      hTarget = h_d0sigma_neg;
      hTarget_mean = h_d0mean_neg;
      hTarget_chi2 = h_d0chi2_neg;
      theSubindex.Clear(); theSubindex.Append("_d0_neg");
      break;
    case 7:
      hInput = h_d0_vs_pt_eta_pos_unfold;
      hTarget = h_d0sigma_pos_unfold;
      hTarget_mean = h_d0mean_pos_unfold;
      hTarget_chi2 = h_d0chi2_pos_unfold;
      theSubindex.Clear(); theSubindex.Append("_d0_pos_unfold");
      break;
    case 8:
      hInput = h_d0_vs_pt_eta_neg_unfold;
      hTarget = h_d0sigma_neg_unfold;
      hTarget_mean = h_d0mean_neg_unfold;
      hTarget_chi2 = h_d0chi2_neg_unfold;
      theSubindex.Clear(); theSubindex.Append("_d0_neg_unfold");
      break;
    case 9:
      hInput = h_z0_vs_pt_eta_pos;
      hTarget = h_z0sigma_pos;
      hTarget_mean = h_z0mean_pos;
      hTarget_chi2 = h_z0chi2_pos;
      theSubindex.Clear(); theSubindex.Append("_z0_pos");
      break;
    case 10:
      hInput = h_z0_vs_pt_eta_neg;
      hTarget = h_z0sigma_neg;
      hTarget_mean = h_z0mean_neg;
      hTarget_chi2 = h_z0chi2_neg;
      theSubindex.Clear(); theSubindex.Append("_z0_neg");
      break;
    case 11:
      hInput = h_z0_vs_pt_eta_pos_unfold;
      hTarget = h_z0sigma_pos_unfold;
      hTarget_mean = h_z0mean_pos_unfold;
      hTarget_chi2 = h_z0chi2_pos_unfold;
      theSubindex.Clear(); theSubindex.Append("_z0_pos_unfold");
      break;
    case 12:
      hInput = h_z0_vs_pt_eta_neg_unfold;
      hTarget = h_z0sigma_neg_unfold;
      hTarget_mean = h_z0mean_neg_unfold;
      hTarget_chi2 = h_z0chi2_neg_unfold;
      theSubindex.Clear(); theSubindex.Append("_z0_neg_unfold");
      break;
    case 13:
      hInput = h_d0_vs_pt_eta_unfold_kpvup;
      hTarget = h_d0sigma_unfold_kpvup;
      hTarget_mean = h_d0mean_unfold_kpvup;
      hTarget_chi2 = h_d0chi2_unfold_kpvup;
      theSubindex.Clear(); theSubindex.Append("_d0_unfold_kpvup");
      break;
    case 14:
      hInput = h_d0_vs_pt_eta_unfold_kpvdown;
      hTarget = h_d0sigma_unfold_kpvdown;
      hTarget_mean = h_d0mean_unfold_kpvdown;
      hTarget_chi2 = h_d0chi2_unfold_kpvdown;
      theSubindex.Clear(); theSubindex.Append("_d0_unfold_kpvdown");
      break;
    case 15:
      hInput = h_z0_vs_pt_eta_unfold_kpvup;
      hTarget = h_z0sigma_unfold_kpvup;
      hTarget_mean = h_z0mean_unfold_kpvup;
      hTarget_chi2 = h_z0chi2_unfold_kpvup;
      theSubindex.Clear(); theSubindex.Append("_z0_unfold_kpvup");
      break;
    case 16:
      hInput = h_z0_vs_pt_eta_unfold_kpvdown;
      hTarget = h_z0sigma_unfold_kpvdown;
      hTarget_mean = h_z0mean_unfold_kpvdown;
      hTarget_chi2 = h_z0chi2_unfold_kpvdown;
      theSubindex.Clear(); theSubindex.Append("_z0_unfold_kpvdown");
      break;
    case 17:
      hInput = h_d0_vs_pt_eta_unfold_ktrkup;
      hTarget = h_d0sigma_unfold_ktrkup;
      hTarget_mean = h_d0mean_unfold_ktrkup;
      hTarget_chi2 = h_d0chi2_unfold_ktrkup;
      theSubindex.Clear(); theSubindex.Append("_d0_unfold_ktrkup");
      break;
    case 18:
      hInput = h_d0_vs_pt_eta_unfold_ktrkdown;
      hTarget = h_d0sigma_unfold_ktrkdown;
      hTarget_mean = h_d0mean_unfold_ktrkdown;
      hTarget_chi2 = h_d0chi2_unfold_ktrkdown;
      theSubindex.Clear(); theSubindex.Append("_d0_unfold_ktrkdown");
      break;
    case 19:
      hInput = h_z0_vs_pt_eta_unfold_ktrkup;
      hTarget = h_z0sigma_unfold_ktrkup;
      hTarget_mean = h_z0mean_unfold_ktrkup;
      hTarget_chi2 = h_z0chi2_unfold_ktrkup;
      theSubindex.Clear(); theSubindex.Append("_z0_unfold_ktrkup");
      break;
    case 20:
      hInput = h_z0_vs_pt_eta_unfold_ktrkdown;
      hTarget = h_z0sigma_unfold_ktrkdown;
      hTarget_mean = h_z0mean_unfold_ktrkdown;
      hTarget_chi2 = h_z0chi2_unfold_ktrkdown;
      theSubindex.Clear(); theSubindex.Append("_z0_unfold_ktrkdown");
      break;
    case 21:
      this->SetGaussianFitRange(2.0);
      hInput = h_d0_vs_pt_eta_unfold;
      hTarget = h_d0sigma_unfold_widerfit;
      hTarget_mean = h_d0mean_unfold_widerfit;
      hTarget_chi2 = h_d0chi2_unfold_widerfit;
      theSubindex.Clear(); theSubindex.Append("_d0_unfold_widerfit");
      break;
    case 22:
      this->SetGaussianFitRange(2.0);
      hInput = h_z0_vs_pt_eta_unfold;
      hTarget = h_z0sigma_unfold_widerfit;
      hTarget_mean = h_z0mean_unfold_widerfit;
      hTarget_chi2 = h_z0chi2_unfold_widerfit;
      theSubindex.Clear(); theSubindex.Append("_z0_unfold_widerfit");
      break;
    default:
      hInput = h_d0_vs_pt_eta;
      hTarget = h_d0sigma;
      hTarget_mean = h_d0mean;
      hTarget_chi2 = h_d0chi2;
      theSubindex.Clear(); theSubindex.Append("_d0");
      break;
    }
    if (m_printLevel>= 0) std::cout << " ** IPResolStudies::Execute ** case: " << theCase << " / " << numberOfCases << " --> " << theSubindex.Data() << std::endl;
    // name of the file storing the fits of the projections in pt-eta bins
    TString outputFileName("projectionsfit");
    outputFileName.Append(theSubindex.Data());
    outputFileName.Append(".pdf");
    
    if (hInput != NULL) std::cout << " hinput ok" << std::endl;
    if (hTarget != NULL) std::cout << " htarget ok" << std::endl;
    if (hTarget_mean != NULL) std::cout << " htarget_mean ok" << std::endl;
    if (hTarget_chi2 != NULL) std::cout << " htarget_chi2 ok" << std::endl;

    for ( int etabin = 1; etabin <= h_d0sigma->GetYaxis()->GetNbins(); etabin++) {
      for ( int ptbin = 1; ptbin <= h_d0sigma->GetXaxis()->GetNbins(); ptbin++) {
        if (m_printLevel >= 10) std::cout << "IN THE LOOP:   " << "etabin: " << etabin << "  ptbin: " << ptbin << std::endl;
        TString hname("h"); hname.Append(theSubindex.Data()); 
        hname.Append("_ptbin_"); hname += ptbin;
	hname.Append("_etabin_"); hname += etabin;
        TH1D * h_thisProjection = hInput->ProjectionZ(hname.Data(), ptbin, ptbin, etabin, etabin,"e");
        if(this->IterativeGaussianFit(h_thisProjection) == true){
	  hTarget->SetBinContent(ptbin,etabin, h_thisProjection->GetFunction("fgaus")->GetParameter(2));
	  hTarget->SetBinError(ptbin,etabin, h_thisProjection->GetFunction("fgaus")->GetParError(2));
          hTarget_mean->SetBinContent(ptbin,etabin,h_thisProjection->GetFunction("fgaus")->GetParameter(1));
          hTarget_mean->SetBinError(ptbin,etabin,h_thisProjection->GetFunction("fgaus")->GetParError(1));
          hTarget_chi2->SetBinContent(ptbin,etabin,h_thisProjection->GetFunction("fgaus")->GetChisquare()/h_thisProjection->GetFunction("fgaus")->GetNDF());          
        }            
	else {
	  h_thisProjection->Draw();
	  hTarget->SetBinContent(ptbin,etabin,0.);
          hTarget_mean->SetBinContent(ptbin,etabin,0.);
          hTarget_chi2->SetBinContent(ptbin,etabin,0.);
	}
	
	can_projectionfit->cd();
	
	if (etabin== h_d0sigma->GetYaxis()->GetNbins() && ptbin== h_d0sigma->GetXaxis()->GetNbins()){
	  // this is the last --> close the file 
	  TString theOuputFileWithCloseClause(outputFileName.Data());  theOuputFileWithCloseClause.Append(")");
	  can_projectionfit->Print( theOuputFileWithCloseClause.Data(), "pdf");
	}
	else {
	  if (outputFileOpen) can_projectionfit->Print(outputFileName.Data(),"pdf");	    
	}
	if (!outputFileOpen) {
	  // output file is not yet open -> open the file
	  TString theOuputFileWithOpenClause(outputFileName.Data());
	  theOuputFileWithOpenClause.Append("(");
	  can_projectionfit->Print(theOuputFileWithOpenClause,"pdf");
	  outputFileOpen = true;   
	} 	 	
      }    
    }
    //if(theCase == 14) TH2F* d0unf_Pt_Eta_kPVErDw_Tight = (TH2F*)h_d0sigma_unfold_kpvdown->Clone();
  }
  
  //Calculate Ktrk Factors
  float K_trk_d0 = 1., K_trk_d0_err = 0.;
  float K_trk_z0 = 1., K_trk_z0_err = 0.;
 
  for (int ptbin = 1; ptbin <= m_PtBins; ptbin++) {
    for (int etabin = 1; etabin <= m_EtaBins; etabin++){
      K_trk_d0 = 1.;
      K_trk_z0 = 1.;
      K_trk_d0_err = 0.;
      K_trk_z0_err = 0.;

      //cout << "(Ptbin, etabin) : " << "(" << ptbin << "," <<  etabin << ")" << endl; 

      if ( h_d0sigma->GetBinContent(ptbin, etabin) > 0) {
	K_trk_d0 = h_d0sigma_unfold->GetBinContent(ptbin, etabin) / h_d0sigma->GetBinContent(ptbin,etabin);  
      //cout << "H_D0_UNFOLD: " << h_d0sigma_unfold->GetBinContent(ptbin, etabin) << "   H_D0_RECO: " <<  h_d0sigma->GetBinContent(ptbin,etabin) << endl;
	K_trk_d0_err = K_trk_d0 * sqrt(pow(h_d0sigma_unfold->GetBinError(ptbin, etabin)/h_d0sigma_unfold->GetBinContent(ptbin, etabin),2) + 
				       pow(h_d0sigma->GetBinError(ptbin, etabin)/h_d0sigma->GetBinContent(ptbin, etabin),2));
      }
      if ( h_z0sigma->GetBinContent(ptbin, etabin) > 0) {
	K_trk_z0 = h_z0sigma_unfold->GetBinContent(ptbin, etabin) / h_z0sigma->GetBinContent(ptbin,etabin);   
	K_trk_z0_err = K_trk_z0 * sqrt(pow(h_z0sigma_unfold->GetBinError(ptbin, etabin)/h_z0sigma_unfold->GetBinContent(ptbin, etabin),2) + 
				       pow(h_z0sigma->GetBinError(ptbin, etabin)/h_z0sigma->GetBinContent(ptbin, etabin),2));
      //cout << "H_Z0_UNFOLD: " << h_z0sigma_unfold->GetBinContent(ptbin, etabin) << "   H_Z0_RECO: " <<  h_z0sigma->GetBinContent(ptbin,etabin) << endl; 
      }
      h_Ktrk_d0_pt_eta->SetBinContent( ptbin, etabin, K_trk_d0);  
      h_Ktrk_d0_pt_eta->SetBinError( ptbin, etabin, K_trk_d0_err);  
      h_Ktrk_z0_pt_eta->SetBinContent( ptbin, etabin, K_trk_z0);  
      h_Ktrk_z0_pt_eta->SetBinError( ptbin, etabin, K_trk_z0_err);  
   }
  }
 
  this->DrawHistos();
  
  if (m_printLevel >= 0) {std::cout << " ** IPResolStudies ** Execute() ** completed ** " << std::endl;}  
  return;
}
#endif 
