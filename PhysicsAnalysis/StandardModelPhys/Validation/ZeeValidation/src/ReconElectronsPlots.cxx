/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ZeeValidation/ReconElectronsPlots.h"
#include "xAODEgamma/EgammaDefs.h"
	
using CLHEP::GeV;

namespace ZeeValidation{
  
  ReconElectronsPlots::ReconElectronsPlots(PlotBase* pParent, std::string sDir, std::string sParticleType):
    PlotBase(pParent, sDir),
    m_sParticleType(sParticleType),
    h_electron_n(NULL), h_photon_n(NULL), 
    h_electron_author(NULL), h_nvtx(NULL), 
    h_mu(NULL), h_bcid(NULL), 
    h_track_n(NULL), 
    h_pv_x(NULL), h_pv_y(NULL), h_pv_z(NULL), 
    h_f1(NULL), h_f3(NULL), h_f1core(NULL), h_f3core(NULL), 
    h_e233(NULL), h_e237(NULL), h_e277(NULL), h_reta(NULL), h_rphi(NULL),
    h_weta1(NULL), h_weta2(NULL), h_wtots1(NULL), 
    h_ethad(NULL), h_ethad1(NULL),
    h_fethad(NULL), h_fethad1(NULL), 
    h_e2tsts1(NULL), h_fracs1(NULL), 
    h_emins1(NULL), h_emaxs1(NULL), 
    h_demm1(NULL), h_iso(NULL), 
    h_eratio(NULL), 
    h_deta1(NULL), h_deta2(NULL), 
    h_dphi1(NULL), h_dphi2(NULL), 
    h_dphires2(NULL), 
    h_etcone20(NULL), h_etcone30(NULL), h_etcone40(NULL), 
    h_fetcone20(NULL), h_fetcone30(NULL), h_fetcone40(NULL), 
    h_ptcone20(NULL), h_ptcone30(NULL), h_ptcone40(NULL), 
    h_n_blayer_hits(NULL), h_n_pixel_hits(NULL),
    h_n_si_hits(NULL), h_n_trt_hits(NULL), 
    h_n_trt_hits_high(NULL), h_r_trt_hits(NULL),
    h_n_blayer_hits_outliers(NULL), h_n_pixel_hits_outliers(NULL), 
    h_n_si_hits_outliers(NULL), h_n_trt_hits_outliers(NULL), 
    h_n_trt_hits_high_outliers(NULL), h_r_trt_hits_outliers(NULL), 
    h_d0(NULL), h_sigmad0(NULL), h_eoverp(NULL), h_eoverpq(NULL)
  {
    for (int i = 0; i < nLevels; i++) {
      h_electron_pt[i] = NULL;
      h_electron_eta[i] = NULL;
      h_electron_phi[i] = NULL;
    }
    for (int i = 0; i < nLevels-1; i++) {
      h_electron_eff_pt[i] = NULL;
      h_electron_eff_eta[i] = NULL;
      h_electron_eff_phi[i] = NULL;
    }
  } 

  const std::string ReconElectronsPlots::cLevelLabel[nLevels] = {
    "Reco",
    "OQ", 
    "Loose",
    "Medium",
    "Tight",
    "LHLoose",
    "LHMedium",
    "LHTight"
  };    
  //-------------------------------------------------
  void ReconElectronsPlots::initializePlots(){
    TH1::SetDefaultSumw2(kTRUE);    

    h_electron_n      = Book1D("NElectrons", "Number of Electrons; n ;Events", 20, 0., 20);
    h_photon_n        = Book1D("NPhotons", "Number of Photons; n ;Events", 20, 0., 20);
    h_electron_author = Book1D("Author", "author of " + m_sParticleType +";author;Events ", 10 , 0.0, 10.0);
    h_nvtx            = Book1D("Nvtx", "Nvtx distribution;n_{vtx};Events ", 30, 0, 30);
    h_mu              = Book1D("Mu", "<#mu> distribution;<mu>;Events", 40, 0, 40);
    h_bcid            = Book1D("BCID", "BCID distribution;BCID;Events", 3500, 0, 3500);
    h_track_n         = Book1D("NTracks", "#Tracks on primary vtx;N_{tracks};Events", 80, 0, 200);
    h_pv_x            = Book1D("Pv_x", "Primary Vertex X Position;x_{vtx};Events", 100, -.5, .5);
    h_pv_y            = Book1D("Pv_y", "Primary Vertex Y Position;y_{vtx};Events", 100, -1.5, 1.5);
    h_pv_z            = Book1D("Pv_z", "Primary Vertex Z Position;z_{vtx};Events", 100, -250, 250);

    for (int i = 0; i < nLevels; i++) {
      h_electron_pt[i]  = Book1D("Pt" + cLevelLabel[i], "P_{T} of "+ m_sParticleType + "; E_{T} (GeV);Events", 60, 20., 140.);
      h_electron_eta[i] = Book1D("Eta" + cLevelLabel[i], "#eta of " + m_sParticleType + ";#eta;Events", 50, -2.5, 2.5);
      h_electron_phi[i] = Book1D("Phi" + cLevelLabel[i], "#varphi of " + m_sParticleType + ";#varphi;Events", 60, -TMath::Pi(), TMath::Pi());
    }

   for (int i = 0; i < nLevels-1; i++) {
      h_electron_eff_pt[i]  = BookTProfile("PtEff" + cLevelLabel[i+1] + cLevelLabel[0], cLevelLabel[i+1] + " Efficiency vs P_{T} of "+ m_sParticleType + "; E_{T} (GeV);" + cLevelLabel[i+1] + "/" + cLevelLabel[0], 60, 20., 140.);
      h_electron_eff_eta[i] = BookTProfile("EtaEff" + cLevelLabel[i+1] + cLevelLabel[0], cLevelLabel[i+1] + " Efficiency vs #eta of " + m_sParticleType + ";#eta;"  + cLevelLabel[i+1] + "/" + cLevelLabel[0], 50, -2.5, 2.5);
      h_electron_eff_phi[i] = BookTProfile("PhiEff" + cLevelLabel[i+1] + cLevelLabel[0], cLevelLabel[i+1] + " Efficiency vs #varphi of " + m_sParticleType + ";#varphi;"  + cLevelLabel[i+1] + "/" + cLevelLabel[0], 60, -TMath::Pi(), TMath::Pi());
    }

   //showerShapeValue
   h_f1     = Book1D("F1", "E_{1}/E (f1); E_{1}/E (f1); Events", 50, 0., 1.); //+
   h_f3     = Book1D("F3", "E_{3}/E (f3); E_{3}/E (f3); Events", 50, 0., .1); //+
   h_f1core = Book1D("F1Core", "E_{1}(3x1)/E (f1core); E_{1}(3x1)/E (f1core); Events", 50, 0., 1.); //+
   h_f3core = Book1D("F3Core", "E_{3}(3x3)/E (f3core); E_{3}(3x3)/E (f3core); Events", 50, 0., .1); //+

   h_e233 = Book1D("E233", "E_{2}(3x3)/GeV; E_{2}(3x3)/GeV; Events", 100, 0., 500.);
   h_e237 = Book1D("E237", "E_{2}(3x7)/GeV; E_{2}(3x7)/GeV; Events", 100, 0., 500.);
   h_e277 = Book1D("E277", "E_{2}(7x7)/GeV; E_{2}(7x7)/GeV; Events", 100, 0., 500.);
   h_reta = Book1D("REta", "R_{#eta} (E_{2}(3x7)/E_{2}(7x7));R_{#eta}; Events", 50, 0.5, 1.);
   h_rphi = Book1D("RPhi", "R_{#phi} (E_{2}(3x3)/E_{2}(3x7));R_{#phi}; Events", 50, 0.5, 1.);

   h_weta1   = Book1D("WEta1", "w_{#eta 1} (weta1, shower width); w_{#eta 1}; Events", 70, 0.2, .9);
   h_weta2   = Book1D("WEta2", "w_{#eta 2} (weta2, lateral width); w_{#eta 2}; Events", 100, 0.005, .02);
   h_wtots1  = Book1D("WTots1", "w_{tot1} (wtots1, shower width); w_{tot1}; Events", 100, 0., 6.);

   h_ethad   = Book1D("EtHad", "E^{had}_{T}/GeV;E^{had}_{T}/GeV; Events", 100, -1., 9.);
   h_ethad1  = Book1D("EtHad1", "E^{had1}_{T}/GeV;E^{had1}_{T}/GeV; Events", 100, -1., 9.);
   h_fethad  = Book1D("FracEtHad", "E^{had}_{T}/E_{T};E^{had}_{T}/E_{T}; Events", 100, -0.05, .45);
   h_fethad1 = Book1D("FracEtHad1", "E^{had1}_{T}/E_{T};E^{had1}_{T}/E_{T}; Events", 100, -0.05, .45);

   h_e2tsts1 = Book1D("E2tsts1", "E_{1}^{max2}/GeV (E of 2^{nd} maximum); E_{1}^{max2}/GeV; Events", 100, 0., 3.);
   h_fracs1  = Book1D("Fracs1", "fracs1 (shower shape core); fracs1; Events", 50, 0., 1.);
   h_emins1  = Book1D("Emins1", "E_{1}^{min}/GeV (E^{min} betw. maximum); E_{1}^{min}/GeV; Events", 100, 0., 3.);
   h_emaxs1  = Book1D("Emaxs1", "E_{1}^{max}/GeV; E_{1}^{max}/GeV; Events", 100, 0., 3.);
   h_demm1   = Book1D("dEmm1", "#Delta E^{sep}_{1}/GeV (2^{nd} max. separation); #Delta E^{sep}_{1}/GeV; Events", 50, 0., 1.);
   h_iso     = Book1D("Iso", "1 - E(3x3)/E(3x7)(iso); 1 - E(3x3)/E(3x7); Events", 100, 0., 1.);
   h_eratio  = Book1D("Eratio", "(E_{1}^{max}/GeV - E_{1}^{max2}/GeV )/(E_{1}^{max}/GeV + E_{1}^{max2}/GeV ); E_{ratio}; Events", 50, 0., 1.);

   //TrackCaloMatchType
   h_deta1 = Book1D("dEta1", "#Delta #eta_{1} Cluster-Track; #Delta #eta_{1}; Events", 100, -.05, .05);
   h_deta2 = Book1D("dEta2", "#Delta #eta_{2} Cluster-Track; #Delta #eta_{2}; Events", 100, -.05, .05);
   h_dphi1 = Book1D("dPhi1", "#Delta #phi_{1} Cluster-Track; #Delta #phi_{1}; Events", 100, -.05, .05);
   h_dphi2 = Book1D("dPhi2", "#Delta #phi_{2} Cluster-Track; #Delta #phi_{2}; Events", 100, -.05, .05);
   h_dphires2 = Book1D("dPhiRes2", "#Delta #phi_{2} Cluster-Track (Rescaled); #Delta #phi_{2} (rescaled); Events", 100, -.05, .05);
                
   //isolationValue
   h_etcone20  = Book1D("EtCone20", "E_{T}^{cone20}/GeV; E_{T}^{cone20}/GeV; Events", 120, -4., 20.);
   h_etcone30  = Book1D("EtCone30", "E_{T}^{cone30}/GeV; E_{T}^{cone30}/GeV; Events", 120, -4., 20.);
   h_etcone40  = Book1D("EtCone40", "E_{T}^{cone40}/GeV; E_{T}^{cone40}/GeV; Events", 120, -4., 20.);
   h_fetcone20 = Book1D("FracEtCone20", "E_{T}^{cone20}/E_{T}; E_{T}^{cone20}/E_{T}; Events", 120, -.1, .5);
   h_fetcone30 = Book1D("FracEtCone30", "E_{T}^{cone30}/E_{T}; E_{T}^{cone30}/E_{T}; Events", 120, -.1, .5);
   h_fetcone40 = Book1D("FracEtCone40", "E_{T}^{cone40}/E_{T}; E_{T}^{cone40}/E_{T}; Events", 120, -.1, .5);

   h_ptcone20 = Book1D("Ptcone20", "p_{T}^{cone20}/GeV; p_{T}^{cone20}/GeV; Events", 80, 0., 20.);
   h_ptcone30 = Book1D("Ptcone30", "p_{T}^{cone30}/GeV; p_{T}^{cone30}/GeV; Events", 80, 0., 20.);
   h_ptcone40 = Book1D("Ptcone40", "p_{T}^{cone40}/GeV; p_{T}^{cone40}/GeV; Events", 80, 0., 20.);

   //RetrieveHitInfo
   h_n_blayer_hits   = Book1D("NBlayerHits", "N^{hits}_{BLayer}; N^{hits}; Events", 10, 0., 10.);
   h_n_pixel_hits    = Book1D("NPixelHits", "N^{hits}_{Pixels}; N^{hits}; Events", 10, 0., 10.);
   h_n_si_hits       = Book1D("NSiHits", "N^{hits}_{Si}; N^{hits}; Events", 30, 0., 30.);
   h_n_trt_hits      = Book1D("NTRTHits", "N^{hits}_{TRT}; N^{hits}; Events", 50, 0., 50.);
   h_n_trt_hits_high = Book1D("NTRTHitsHighThreshold", "N^{hits}_{TRT} High Threshold; N^{hits}; Events", 50, 0., 50.);
   h_r_trt_hits      = Book1D("RTRTHits", "N^{hits}_{TRT,HT} / N^{hits}_{TRT}; N^{hits}; Events", 50, 0., 1.);

   h_n_blayer_hits_outliers   = Book1D("NBlayerHitsOutliers", "N^{hits}_{BLayer} incl. Outliers; N^{hits}; Events", 10, 0., 10.);
   h_n_pixel_hits_outliers    = Book1D("NPixelHitsOutliers", "N^{hits}_{Pixels} incl. Outliers; N^{hits}; Events", 10, 0., 10.);
   h_n_si_hits_outliers       = Book1D("NSiHitsOutliers", "N^{hits}_{Si} incl. Outliers; N^{hits}; Events", 30, 0., 30.);
   h_n_trt_hits_outliers      = Book1D("NTRTHitsOutliers", "N^{hits}_{TRT} incl. Outliers; N^{hits}; Events", 50, 0., 50.);
   h_n_trt_hits_high_outliers = Book1D("NTRTHitsHighThresholdOutliers", "N^{hits}_{TRT} High Threshold incl. Outliers; N^{hits}; Events", 50, 0., 50.);
   h_r_trt_hits_outliers      = Book1D("RTRTHitsOutliers", "N^{hits}_{TRT,HT} / N^{hits}_{TRT} incl. Outliers; N^{hits}; Events", 50, 0., 1.);
 
   h_d0 = Book1D("TrackD0", "Track d0; d0; Events", 20, -1., 1.);
   h_sigmad0 = Book1D("TrackSigmaD0", "Track #sigma_{d0}; #sigma_{d0}; Events", 20, 0., 0.1);
   h_eoverp = Book1D("EoverP", "E/p; E/p; Events", 50, 0., 10.);
   h_eoverpq = Book1D("EoverPQ", "E/p * q; E/p * q; Events", 50, -5., 5.);

  }
  //------------------------------------------------- 
  void ReconElectronsPlots::fill(const xAOD::EventInfo* eventInfo, const xAOD::VertexContainer* vertices){
    
    int nGoodVertices = 0;
    for (const auto* vtx : vertices -> stdcont()){
      if (vtx -> vertexType() == xAOD::VxType::PriVtx){
	h_pv_x -> Fill(vtx -> x());
	h_pv_y -> Fill(vtx -> y());
	h_pv_z -> Fill(vtx -> z());
	h_track_n ->Fill(vtx -> nTrackParticles());
      }
      if (vtx -> vertexType() == xAOD::VxType::NoVtx) continue;
      nGoodVertices++;
    }
    h_nvtx -> Fill(nGoodVertices);
    h_mu -> Fill(eventInfo -> averageInteractionsPerCrossing());
    h_bcid -> Fill(eventInfo -> bcid()); 
  }
 //-------------------------------------------------
  void ReconElectronsPlots::fill(const xAOD::Electron* electron, int level){
    h_electron_eta[level] -> Fill(electron -> eta());
  }
 //-------------------------------------------------
  void ReconElectronsPlots::fillinAcc(const xAOD::Electron* electron, int level){
    h_electron_pt[level] -> Fill(electron -> pt()*(1./GeV));
    h_electron_phi[level] -> Fill(electron -> phi());
  }
 //-------------------------------------------------
  void ReconElectronsPlots::fillShowerShape(const xAOD::Electron* electron){

    float f1(0), f3(0), f1core(0), f3core(0), fracs1(0), emaxs1(0),  weta1(0), weta2(0), wtots1(0), r33over37allcalo(0), eratio(0);

    if (electron -> showerShapeValue(f1,     xAOD::EgammaParameters::f1))          h_f1 -> Fill(f1);
    if (electron -> showerShapeValue(f1core, xAOD::EgammaParameters::f1core))      h_f1core -> Fill(f1core);   
    if (electron -> showerShapeValue(f3,     xAOD::EgammaParameters::f3))          h_f3 -> Fill(f3);   
    if (electron -> showerShapeValue(f3core, xAOD::EgammaParameters::f3core))      h_f3core -> Fill(f3core);   
    if (electron -> showerShapeValue(emaxs1, xAOD::EgammaParameters::emaxs1))      h_emaxs1 -> Fill(emaxs1*(1./GeV));
    if (electron -> showerShapeValue(fracs1, xAOD::EgammaParameters::fracs1))      h_fracs1 -> Fill(fracs1);    
    if (electron -> showerShapeValue(weta1,  xAOD::EgammaParameters::weta1))       h_weta1 -> Fill(weta1);   
    if (electron -> showerShapeValue(weta2,  xAOD::EgammaParameters::weta2))       h_weta2 -> Fill(weta2);    
    if (electron -> showerShapeValue(wtots1, xAOD::EgammaParameters::wtots1))      h_wtots1 -> Fill(wtots1); 
    if (electron -> showerShapeValue(r33over37allcalo, xAOD::EgammaParameters::r33over37allcalo))     h_iso -> Fill(r33over37allcalo); 
    
    if (electron -> showerShapeValue(eratio,    xAOD::EgammaParameters::Eratio))   h_eratio -> Fill(eratio);
    float  e2tsts1(0), emins1(0);
    if (electron -> showerShapeValue(e2tsts1, xAOD::EgammaParameters::e2tsts1) &&
	electron->showerShapeValue(emins1, xAOD::EgammaParameters::emins1)){
      h_e2tsts1 -> Fill(e2tsts1*(1./GeV));
      h_emins1 -> Fill(emins1*(1./GeV));
      h_demm1 -> Fill((e2tsts1 - emins1)*(1./GeV));
    }   
    float e233(0), e237(0), e277(0), reta(0), rphi(0);
    if ( electron -> showerShapeValue(e233, xAOD::EgammaParameters::e233) && 
	 electron -> showerShapeValue(e237, xAOD::EgammaParameters::e237) && 
	 electron -> showerShapeValue(e277, xAOD::EgammaParameters::e277) &&
	 electron -> showerShapeValue(reta, xAOD::EgammaParameters::Reta) &&
	 electron -> showerShapeValue(rphi, xAOD::EgammaParameters::Rphi)){
      h_e233 -> Fill(e233*(1./GeV));
      h_e237 -> Fill(e237*(1./GeV));
      h_e277 -> Fill(e277*(1./GeV));
      h_reta -> Fill(reta);
      h_rphi -> Fill(rphi);
    }
    float  ethad(0), ethad1(0);
    if ( electron -> showerShapeValue(ethad, xAOD::EgammaParameters::ethad) &&
	 electron -> showerShapeValue(ethad1, xAOD::EgammaParameters::ethad1) ){
      h_ethad -> Fill(ethad*(1./GeV));
      h_ethad1 -> Fill(ethad1*(1./GeV));      
      h_fethad -> Fill(ethad/electron -> pt());
      h_fethad1 -> Fill(ethad1/electron -> pt());
    }
 
  }
 //-------------------------------------------------
  void ReconElectronsPlots::fillTrackCaloMatch(const xAOD::Electron* electron, const xAOD::TrackParticle* track){

    float deta1(0), deta2(0), dphi1(0), dphi2(0), dphires2(0);
    if ( electron -> trackCaloMatchValue(deta1, xAOD::EgammaParameters::deltaEta1 ))      h_deta1 -> Fill(deta1);
    if ( electron -> trackCaloMatchValue(deta2, xAOD::EgammaParameters::deltaEta2 ))      h_deta2 -> Fill(deta2);
    if ( electron -> trackCaloMatchValue(dphi1, xAOD::EgammaParameters::deltaPhi1 ))      h_dphi1 -> Fill(dphi1);
    if ( electron -> trackCaloMatchValue(dphi2, xAOD::EgammaParameters::deltaPhi2 ))      h_dphi2 -> Fill(dphi2);
    if ( electron -> trackCaloMatchValue(dphires2, xAOD::EgammaParameters::deltaPhiRescaled2 ))      h_dphires2 -> Fill(dphires2);

    h_d0 -> Fill(track -> d0());
    float sigmad0 = -99.;
    float vard0 = track -> definingParametersCovMatrix()(0, 0);
    if (vard0 > 0) sigmad0 = sqrtf(vard0);
    h_sigmad0 -> Fill(sigmad0);
   
    h_eoverp -> Fill(electron -> e() * TMath::Abs(track -> qOverP()));
    h_eoverpq -> Fill(electron -> e() * track -> qOverP());

  }
  //-------------------------------------------------
  void ReconElectronsPlots::fillIsolation(const xAOD::Electron* electron){

    float etcone20(0), etcone30(0), etcone40(0), ptcone20(0), ptcone30(0), ptcone40(0);
 
    if ( electron->isolationValue(etcone20, xAOD::Iso::etcone20) ){
      h_etcone20 -> Fill(etcone20*(1./GeV));
      h_fetcone20 -> Fill(etcone20/electron -> pt());
    }
    if ( electron->isolationValue(etcone30, xAOD::Iso::etcone30) ){
      h_etcone30 -> Fill(etcone30*(1./GeV));
      h_fetcone30 -> Fill(etcone30/electron -> pt());
    }
    if ( electron->isolationValue(etcone40, xAOD::Iso::etcone40) ){
      h_etcone40 -> Fill(etcone40*(1./GeV));
      h_fetcone40 -> Fill(etcone40/electron -> pt());
  }
  if ( electron->isolationValue(ptcone20, xAOD::Iso::ptcone20) )	 h_ptcone20 -> Fill(ptcone20*(1./GeV));
  if ( electron->isolationValue(ptcone30, xAOD::Iso::ptcone30) )	 h_ptcone30 -> Fill(ptcone30*(1./GeV));
  if ( electron->isolationValue(ptcone40, xAOD::Iso::ptcone40) )	 h_ptcone40 -> Fill(ptcone40*(1./GeV));

}
 //-------------------------------------------------
  void ReconElectronsPlots::fillHitInfo(const xAOD::Electron* electron){

    uint8_t blayerh(0), pixelh(0), scth(0), trth(0), trthighh(0), blayero(0), pixelo(0), scto(0), trto(0), trthigho(0);
    if( electron -> trackParticleSummaryValue(blayerh, xAOD::numberOfInnermostPixelLayerHits ) &&
	electron -> trackParticleSummaryValue(blayero, xAOD::numberOfInnermostPixelLayerOutliers) ){
      h_n_blayer_hits -> Fill(blayerh);
      h_n_blayer_hits_outliers -> Fill(blayerh + blayero);
    }
    if( electron -> trackParticleSummaryValue(pixelh, xAOD::numberOfPixelHits) &&  
	electron -> trackParticleSummaryValue(scth, xAOD::numberOfSCTHits ) &&
	electron -> trackParticleSummaryValue(pixelo, xAOD::numberOfPixelOutliers) &&  
	electron -> trackParticleSummaryValue(scto, xAOD::numberOfSCTOutliers)	){
      h_n_pixel_hits -> Fill(pixelh);
      h_n_si_hits -> Fill(pixelh + scth);
      h_n_pixel_hits_outliers -> Fill(pixelh + pixelo);
      h_n_si_hits_outliers -> Fill(pixelh + scth + pixelo + scto);

    }
    if( electron -> trackParticleSummaryValue(trth, xAOD::numberOfTRTHits ) && 
	electron -> trackParticleSummaryValue(trthighh, xAOD::numberOfTRTHighThresholdHits ) &&
	electron -> trackParticleSummaryValue(trto, xAOD::numberOfTRTOutliers) && 
	electron -> trackParticleSummaryValue(trthigho, xAOD::numberOfTRTHighThresholdOutliers)){
      h_n_trt_hits -> Fill(trth);
      h_n_trt_hits_high -> Fill(trthighh);
      double rtrt = (trth) > 0 ? ((double) (trthighh)/(trth) ) : 0.;
      h_r_trt_hits -> Fill(rtrt);

      h_n_trt_hits_outliers -> Fill(trth + trto);
      h_n_trt_hits_high_outliers -> Fill(trthighh + trthigho);
      double rtrto = (trth + trto) > 0 ? ((double) (trthigho + trthighh)/(trth + trto) ) : 0.;
      h_r_trt_hits_outliers -> Fill(rtrto);
    }
 
  }
 //-------------------------------------------------
  void ReconElectronsPlots::finalizePlots() {

    for (int i = 0; i < nLevels-1; i++) {
      makeEfficiencyPlot( h_electron_pt[0], h_electron_pt[i+1], h_electron_eff_pt[i]);
      makeEfficiencyPlot( h_electron_phi[0], h_electron_phi[i+1], h_electron_eff_phi[i]);
      makeEfficiencyPlot( h_electron_eta[0], h_electron_eta[i+1], h_electron_eff_eta[i]);
    }
  }
 //-------------------------------------------------
  void ReconElectronsPlots::makeEfficiencyPlot(TH1* hDenom, TH1* hNom, TProfile* hEff){

    if (hDenom->GetNbinsX() != hNom->GetNbinsX()) return;
    else{
      for (int bin_i = 1; bin_i <= hDenom -> GetNbinsX(); ++bin_i){
	if(hDenom -> GetBinContent(bin_i) == 0) continue;
	double binContent = hNom -> GetBinContent(bin_i) / hDenom -> GetBinContent(bin_i);
	double binCenter = hNom -> GetBinCenter(bin_i);

	hEff -> Fill(binCenter, binContent);
      }
    }

  }
 //-------------------------------------------------
}
