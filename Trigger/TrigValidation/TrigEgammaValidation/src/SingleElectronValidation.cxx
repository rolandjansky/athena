/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigEgammaValidation/SingleElectronValidation.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/Algorithm.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrigCaloEvent/TrigEMCluster.h"
#include "GeneratorObjects/McEventCollection.h"

#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigInDetTrackHelper.h"

#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "AnalysisTriggerEvent/EmTau_ROI.h"

#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCluster.h"

#include "egammaEvent/egammaContainer.h"
#include "egammaEvent/egamma.h"
#include "egammaEvent/EMTrackMatch.h"
#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"

#include "TH1F.h"
#include "TMath.h"

SingleElectronValidation::SingleElectronValidation(const std::string & name, ISvcLocator * pSvcLocator)
  : FEXValidationAlgo(name, pSvcLocator, "Validation")
{
}

SingleElectronValidation::~SingleElectronValidation()
{
}

StatusCode SingleElectronValidation::childInitialize()
{
  StatusCode sc;

  addDirectory(name()+"/Dump");
  if ( (sc = prepare()) != StatusCode::SUCCESS)
    return sc;
  
  addDirectory(name()+"/PreEfficiencies");
  addHistogram(new TH1F("TruthEtHist", "Truth E_{T} for single e^{-} ; E_{T} [GeV] ; Count", 80, 0., 80.));
  addHistogram(new TH1F("TruthEtaHist", "Truth \\eta for single e^{-} ; \\eta ; Count", 128, -3.2, 3.2));
  addHistogram(new TH1F("TruthPhiHist", "Truth \\phi for single e^{-} ; \\phi [rad] ; Count", 128, -3.2, 3.2));
  
  addHistogram(new TH1F("TruthL1Et", "Level 1 Truth E_{T} for single e^{-} ; E_{T} [GeV] ; Count", 80, 0., 80.));
  addHistogram(new TH1F("TruthL1Eta", "Level 1 Truth \\eta for single e^{-} ; \\eta ; Count", 128, -3.2, 3.2));
  addHistogram(new TH1F("TruthL1Phi", "Level 1 Truth \\phi for single e^{-} ; \\phi [rad] ; Count", 128, -3.2, 3.2));
  
  addHistogram(new TH1F("TruthL2Et", "Level 2 Truth E_{T} for single e^{-} ; E_{T} [GeV] ; Count", 80, 0., 80.));
  addHistogram(new TH1F("TruthL2Eta", "Level 2 Truth \\eta for single e^{-} ; \\eta ; Count", 128, -3.2, 3.2));
  addHistogram(new TH1F("TruthL2Phi", "Level 2 Truth \\phi for single e^{-} ; \\phi ; Count", 128, -3.2, 3.2));
  
  // For ID Efficiencies
  addHistogram(new TH1F("TruthL2IDScanEt", "E_{T} after Level 2 ID (ID Scan Match) for single e^{-} ; E_{T} ; Count", 80, 0., 80.));
  addHistogram(new TH1F("TruthL2IDScanPhi", "\\phi after Level 2 ID (ID Scan Match) for single e^{-} ; \\phi [rad] ; Count", 128, -3.2, 3.2));
  addHistogram(new TH1F("TruthL2IDScanEta", "\\eta after Level 2 ID (ID Scan Match) for single e^{-} ; \\eta ; Count", 128, -3.2, 3.2));
  
  addHistogram(new TH1F("TruthL2IDSiEt", "E_{T} after Level 2 ID (SiTrack Match) for single e^{-} ; E_{T} ; Count", 80, 0., 80.));
  addHistogram(new TH1F("TruthL2IDSiPhi", "\\phi after Level 2 ID (SiTrack Match) for single e^{-} ; \\phi [rad] ; Count", 128, -3.2, 3.2));
  addHistogram(new TH1F("TruthL2IDSiEta", "\\eta after Level 2 ID (SiTrack Match) for single e^{-} ; \\eta ; Count", 128, -3.2, 3.2));
  
  // Resolution Studies
  addDirectory(name()+"/Resolution");
  addHistogram(new TH1F("ResEtL1", "Level 1 E_{T} resolution w.r.t. Truth ; E_{T} resolution (%) ; Count", 200, -100., 100.));
  addHistogram(new TH1F("ResEtaL1", "Level 1 \\eta resolution w.r.t. Truth ; \\eta resolution (%) ; Count", 200, -100., 100.));
  addHistogram(new TH1F("ResPhiL1", "Level 1 \\phi resolution w.r.t. Truth ; \\phi resolution (%) ; Count", 200, -100., 100.));
  addHistogram(new TH2F("ResEtL1TruthEt", "Level 1 resolution E_{T} versus E_{T} for e^{-} ; E_{T} [GeV] ; E_{T} Resolution (%)", 80, 0., 80., 200, -100., 100.));
  addHistogram(new TH2F("ResEtL1TruthEta", "Level 1 resolution E_{T} versus \\eta for e^{-} ; \\eta ; E_{T} Resolution (%)", 128, -3.2, 3.2, 200, -100., 100.));
  addHistogram(new TH2F("ResEtL1TruthPhi", "Level 1 resolution E_{T} versus \\phi for e^{-} ; \\phi ; E_{T} Resolution (%)", 128, -3.2, 3.2, 200, -100., 100.));
  
  addHistogram(new TH1F("ResEtL2", "Level 2 E_{T} resolution w.r.t. Truth ; E_{T} resolution (%) ; Count", 200, -100., 100.));
  addHistogram(new TH1F("ResEtaL2", "Level 2 \\eta resolution w.r.t. Truth ; \\eta resolution (%) ; Count", 200, -100., 100.));
  addHistogram(new TH1F("ResPhiL2", "Level 2 \\phi resolution w.r.t. Truth ; \\phi resolution (%) ; Count", 200, -100., 100.));
  addHistogram(new TH2F("ResEtL2TruthEt", "Level 2 resolution E_{T} versus E_{T} for e^{-} ; E_{T} [GeV] ; E_{T} Resolution (%)", 80, 0., 80., 200, -100., 100.));
  addHistogram(new TH2F("ResEtL2TruthEta", "Level 2 resolution E_{T} versus \\eta for e^{-} ; \\eta ; E_{T} Resolution (%)", 128, -3.2, 3.2, 200, -100., 100.));
  addHistogram(new TH2F("ResEtL2TruthPhi", "Level 2 resolution E_{T} versus \\phi for e^{-} ; \\phi ; E_{T} Resolution (%)", 128, -3.2, 3.2, 200, -100., 100.));
  
  addHistogram(new TH1F("ResEtEFScan", "Event Filter [L2 Scan] E_{T} resolution w.r.t. Truth ; E_{T} resolution (%) ; Count", 200, -100., 100.));
  addHistogram(new TH1F("ResEtaEFScan", "Event Filter [L2 Scan] \\eta resolution w.r.t. Truth ; \\eta resolution (%) ; Count", 200, -100., 100.));
  addHistogram(new TH1F("ResPhiEFScan", "Event Filter [L2 Scan] \\phi resolution w.r.t. Truth ; \\phi resolution (%) ; Count", 200, -100., 100.));
  addHistogram(new TH2F("ResEtEFScanTruthEt", "Event Filter [L2 Scan] resolution E_{T} versus E_{T} for e^{-} ; E_{T} [GeV] ; E_{T} Resolution (%)", 80, 0., 80., 200, -100., 100.));
  addHistogram(new TH2F("ResEtEFScanTruthEta", "Event Filter [L2 Scan] resolution E_{T} versus \\eta for e^{-} ; \\eta ; E_{T} Resolution (%)", 128, -3.2, 3.2, 200, -100., 100.));
  addHistogram(new TH2F("ResEtEFScanTruthPhi", "Event Filter [L2 Scan] resolution E_{T} versus \\phi for e^{-} ; \\phi ; E_{T} Resolution (%)", 128, -3.2, 3.2, 200, -100., 100.));
  addHistogram(new TH1F("ResEtEFSi", "Event Filter [L2 SiTrack] E_{T} resolution w.r.t. Truth ; E_{T} resolution (%) ; Count", 200, -100., 100.));
  addHistogram(new TH1F("ResEtaEFSi", "Event Filter [L2 SiTrack] \\eta resolution w.r.t. Truth ; \\eta resolution (%) ; Count", 200, -100., 100.));
  addHistogram(new TH1F("ResPhiEFSi", "Event Filter [L2 SiTrack] \\phi resolution w.r.t. Truth ; \\phi resolution (%) ; Count", 200, -100., 100.));
  addHistogram(new TH2F("ResEtEFSiTruthEt", "Event Filter [L2 SiTrack] resolution E_{T} versus E_{T} for e^{-} ; E_{T} [GeV] ; E_{T} Resolution (%)", 80, 0., 80., 200, -100., 100.));
  addHistogram(new TH2F("ResEtEFSiTruthEta", "Event Filter [L2 SiTrack] resolution E_{T} versus \\eta for e^{-} ; \\eta ; E_{T} Resolution (%)", 128, -3.2, 3.2, 200, -100., 100.));
  addHistogram(new TH2F("ResEtEFSiTruthPhi", "Event Filter [L2 SiTrack] resolution E_{T} versus \\phi for e^{-} ; \\phi ; E_{T} Resolution (%)", 128, -3.2, 3.2, 200, -100., 100.));

  cd(name()+"/PreEfficiencies");
  // EF Calo
  addHistogram(new TH1F("TruthEFScanCaloEt", "Truth E_{T} after EF Calo (and L2 ID Scan Match) for single e^{-} ; E_{T} [GeV] ; Count", 80, 0, 80.));
  addHistogram(new TH1F("TruthEFScanCaloPhi", "Truth \\phi after EF Calo (and L2 ID Scan Match) for single e^{-} ; \\phi [rad] ; Count", 128, -3.2, 3.2));
  addHistogram(new TH1F("TruthEFScanCaloEta", "Truth \\eta after EF Calo (and L2 ID Scan Match) for single e^{-} ; \\eta ; Count", 128, -3.2, 3.2));

  addHistogram(new TH1F("TruthEFSiCaloEt", "Truth E_{T} after EF Calo (and L2 ID SiTrack Match) for single e^{-} ; E_{T} [GeV] ; Count", 80, 0, 80.));
  addHistogram(new TH1F("TruthEFSiCaloPhi", "Truth \\phi after EF Calo (and L2 ID SiTrack Match) for single e^{-} ; \\phi [rad] ; Count", 128, -3.2, 3.2));
  addHistogram(new TH1F("TruthEFSiCaloEta", "Truth \\eta after EF Calo (and L2 ID SiTrack Match) for single e^{-} ; \\eta ; Count", 128, -3.2, 3.2));

  // EF ID
  addHistogram(new TH1F("TruthEFScanTrackEt", "Truth E_{T} after EF Track (and L2 ID Scan Match) for single e^{-} ; E_{T} ; Count", 80, 0, 80.));
  addHistogram(new TH1F("TruthEFScanTrackPhi", "Truth \\phi after EF Track (and L2 ID Scan Match) for single e^{-} ; \\phi [rad] ; Count", 128, -3.2, 3.2));
  addHistogram(new TH1F("TruthEFScanTrackEta", "Truth \\eta after EF Track (and L2 ID Scan Match) for single e^{-} ; \\eta ; Count", 128, -3.2, 3.2));

  addHistogram(new TH1F("TruthEFSiTrackEt", "Truth E_{T} after EF Track (and L2 ID SiTrack Match) for single e^{-} ; E_{T} [GeV] ; Count", 80, 0, 80.));
  addHistogram(new TH1F("TruthEFSiTrackPhi", "Truth \\phi after EF Track (and L2 ID SiTrack Match) for single e^{-} ; \\phi [rad] ; Count", 128, -3.2, 3.2));
  addHistogram(new TH1F("TruthEFSiTrackEta", "Truth \\eta after EF Track (and L2 ID SiTrack Match) for single e^{-} ; \\eta ; Count", 128, -3.2, 3.2));

  return StatusCode::SUCCESS;
}

StatusCode SingleElectronValidation::childExecute()
{
  StatusCode sc;

  bool doL1 = true;
  const LVL1_ROI * lvl1ROI;
  sc = m_storeGate->retrieve(lvl1ROI);
  if (sc.isFailure()) {
    (*m_log) << MSG::WARNING << "Could not find LVL1_ROI in StoreGate." << endreq;
    doL1 = false;
  }

  bool doL2 = true;
  TrigEMClusterContainer::const_iterator EMClusterBegin, EMClusterEnd;
  const DataHandle<TrigEMClusterContainer> EMClusterContainer;
  sc = m_storeGate->retrieve(EMClusterContainer, "HLT_TrigT2CaloEgamma");
  if (sc.isFailure()) {
    (*m_log) << MSG::WARNING
	    << "It wasn't possible to retrieve the TrigEMClusterContainer."
	    << endreq;
    doL2 = false; // Don't stop proccessing events
  } else {
    EMClusterBegin = EMClusterContainer->begin();
    EMClusterEnd = EMClusterContainer->end();
  }

  bool doL2ID = true;
  const DataHandle<TrigInDetTrackCollection> lvl2t_first;
  const DataHandle<TrigInDetTrackCollection> lvl2t_last;
  
  sc = m_storeGate->retrieve(lvl2t_first,lvl2t_last);
  
  if (sc.isFailure()) {
    (*m_log) << MSG::WARNING << "Could not find TrigInDetTrack" << endreq;
    doL2ID = false;
  }

  bool doEFCalo = true;
  const DataHandle<CaloClusterContainer> EFCalo_i;
  const DataHandle<CaloClusterContainer> EFCalo_l;
  sc = m_storeGate->retrieve(EFCalo_i, EFCalo_l);
  if (sc.isFailure()) {
    (*m_log) << MSG::WARNING << "Could not find CaloClusterContainer" << endreq;
    doEFCalo = false;
  }

  bool EFScanCalo_pass = false;
  std::vector<float> EFScanTruthCaloEt;
  std::vector<float> EFScanTruthCaloEta;
  std::vector<float> EFScanTruthCaloPhi;

  bool EFSiCalo_pass = false;
  std::vector<float> EFSiTruthCaloEt;
  std::vector<float> EFSiTruthCaloEta;
  std::vector<float> EFSiTruthCaloPhi;

  bool doEFTrack = true;
  const DataHandle<egammaContainer> EFTrk_i;
  const DataHandle<egammaContainer> EFTrk_l;
  sc = m_storeGate->retrieve(EFTrk_i, EFTrk_l);
  if (sc.isFailure()) {
    (*m_log) << MSG::WARNING << "Could not retrieve egammaContainer" << endreq;
    doEFTrack = false;
  }

  bool EFScanTrk_pass = false;
  std::vector<float> EFScanTruthTrackEt;
  std::vector<float> EFScanTruthTrackEta;
  std::vector<float> EFScanTruthTrackPhi;

  bool EFSiTrk_pass = false;
  std::vector<float> EFSiTruthTrackEt;
  std::vector<float> EFSiTruthTrackEta;
  std::vector<float> EFSiTruthTrackPhi;

  double caloeta, calophi;

  const DataHandle<McEventCollection> themcpartTES,mcpartTESend;
  sc = m_storeGate->retrieve( themcpartTES,mcpartTESend);
  if (sc.isFailure()) {
    (*m_log) << MSG::WARNING << "No AOD MC truth particle container found in StoreGate. Aborting!!!" << endreq;
    return StatusCode::SUCCESS; // Don't stop proccessing events
  }

  // iterators over the container
  McEventCollection::const_iterator Itr;
  McEventCollection::const_iterator ItrE;  
  for ( ; themcpartTES != mcpartTESend; ++themcpartTES) { // For each event collection
    if ( themcpartTES.key() == "TruthEvent" ) { // As long as it is in "TruthEvent"
      Itr  = themcpartTES->begin();
      ItrE = themcpartTES->end();
      for (; Itr != ItrE; ++Itr) { // For each event
        if (!(*Itr)) {
	  continue;
	}
	HepMC::GenEvent::particle_const_iterator itrParticle  = (*Itr)->particles_begin();
	HepMC::GenEvent::particle_const_iterator itrParticleE = (*Itr)->particles_end();
	for ( ; itrParticle != itrParticleE; ++itrParticle) { // For each particle in the event
	  // *Itr is a GenEvent
	  // *itrParticle is a GenParticle
	  if (!(*itrParticle)) {
	    continue;
	  }
	  
	  // Finds an e-
	  if ( fabsf((*itrParticle)->pdg_id()) == 11) {
	    break;
	  }
	}

	if (itrParticle == itrParticleE) {
	  continue;
	}
	// Fill the Truth Histograms for these particles
        float truthEt = (*itrParticle)->momentum().e()/cosh((*itrParticle)->momentum().eta());
	if ( (truthEt > m_lowEtCut) && (fabsf((*itrParticle)->momentum().eta()) <= m_maxEtaCut) ) {

          cd(name()+"/PreEfficiencies");
	  hist1("TruthEtHist")->Fill(truthEt/1.e3);
	  hist1("TruthEtaHist")->Fill((*itrParticle)->momentum().eta());
	  hist1("TruthPhiHist")->Fill((*itrParticle)->momentum().phi());
	  
	  m_nTotal++;
	  
	  if (doL1) {

	    // Did it trigger LVL1?
            LVL1_ROI::emtaus_type::const_iterator itEMTau   = lvl1ROI->getEmTauROIs().begin();
            LVL1_ROI::emtaus_type::const_iterator itEMTau_e = lvl1ROI->getEmTauROIs().end();
            double DetaL1 = 0.1, DphiL1 = 0.1;
            LVL1_ROI::emtaus_type::const_iterator matchedL1;
            bool match_found=false;
            for( ; itEMTau != itEMTau_e; ++itEMTau) {
	      
	      if ((fabsf((*itrParticle)->momentum().eta()-(itEMTau)->getEta()) < DetaL1) &&
		  (delta((*itrParticle)->momentum().phi() - (itEMTau)->getPhi()) < DphiL1)) {
		DetaL1 = fabsf((*itrParticle)->momentum().eta()-(itEMTau)->getEta());
		DphiL1 = delta((*itrParticle)->momentum().phi() - (itEMTau)->getPhi());
		matchedL1 = itEMTau;
		match_found = true;
	      }
	      // May be there is someone at the same spot that might trigger LVL 1 on a closest shot ...
	    }
	    
	    if (match_found) { // This one got thru level 1!!!

              cd(name()+"/Resolution");
              hist2("ResEtL1TruthEt")->Fill(truthEt/1.e3, 100*((matchedL1->getEMClus() - truthEt)/(truthEt)));
              hist2("ResEtL1TruthEta")->Fill((*itrParticle)->momentum().eta(), 100*((matchedL1->getEMClus() - truthEt)/(truthEt)));
              hist2("ResEtL1TruthPhi")->Fill((*itrParticle)->momentum().phi(), 100*((matchedL1->getEMClus() - truthEt)/(truthEt)));
              hist1("ResEtL1")->Fill(100*((matchedL1->getEMClus() - truthEt)/(truthEt)));
              hist1("ResPhiL1")->Fill(100*((matchedL1->getPhi() - (*itrParticle)->momentum().phi())/((*itrParticle)->momentum().phi())));
              hist1("ResEtaL1")->Fill(100*((matchedL1->getEta() - (*itrParticle)->momentum().eta())/((*itrParticle)->momentum().eta())));

	      // Fill Histograms for L1 info
              cd(name()+"/Dump");
	      FillLVL1(matchedL1);

	      if ( (matchedL1->getEMClus() > m_L1EmClus) &&
		   (matchedL1->getEMIsol() <= m_L1EmIsol) &&
		   (matchedL1->getHadCore() <= m_L1HadCore) &&
		   (matchedL1->getHadIsol() <= m_L1HadIsol) ) { // Triggered LVL 1!
	      
                cd(name()+"/PreEfficiencies");
		hist1("TruthL1Et")->Fill(truthEt/1.e3);
		hist1("TruthL1Eta")->Fill((*itrParticle)->momentum().eta());
		hist1("TruthL1Phi")->Fill((*itrParticle)->momentum().phi());
		
		m_nL1++;
		
		if (doL2) {
		  
		  // Finds closest (eta, phi)
		  TrigEMClusterContainer::const_iterator theEMCluster = EMClusterBegin;
		  TrigEMCluster *matchedL2 = 0;
		  double DetaL2 = 0.1;
		  double DphiL2 = 0.1;
		  for (; theEMCluster != EMClusterEnd; theEMCluster++) {
		    if ( (fabsf((*theEMCluster)->eta() - matchedL1->getEta()) < DetaL2) &&
			 (delta((*theEMCluster)->phi() - matchedL1->getPhi()) < DphiL2) ) {
		      DetaL2 = fabsf((*theEMCluster)->eta() - matchedL1->getEta());
		      DphiL2 = delta((*theEMCluster)->phi() - matchedL1->getPhi());
		      matchedL2 = const_cast<TrigEMCluster *>(*theEMCluster);
		    }
		  }
		  if (matchedL2) {
		    
		    // This is the right electron!
		    // This one activated level 2!!!
                    cd(name()+"/Resolution");
                    hist2("ResEtL2TruthEt")->Fill( truthEt/1.e3, 100*(((matchedL2->energy()/cosh(matchedL2->eta())) - truthEt)/(truthEt)));
                    hist2("ResEtL2TruthEta")->Fill( (*itrParticle)->momentum().eta(), 100*(((matchedL2->energy()/cosh(matchedL2->eta())) - truthEt)/(truthEt)));
                    hist2("ResEtL2TruthPhi")->Fill( (*itrParticle)->momentum().phi(), 100*(((matchedL2->energy()/cosh(matchedL2->eta())) - truthEt)/(truthEt)));
                    hist1("ResEtL2")->Fill(100*(((matchedL2->energy()/cosh(matchedL2->eta())) - truthEt)/(truthEt)));
                    hist1("ResEtaL2")->Fill(100*((matchedL2->eta() - (*itrParticle)->momentum().eta())/((*itrParticle)->momentum().eta())));
                    hist1("ResPhiL2")->Fill(100*((matchedL2->phi() - (*itrParticle)->momentum().phi())/((*itrParticle)->momentum().phi())));
		    
		    // Fill FEX hist1grams for electrons
                    cd(name()+"/Dump");
		    FillLVL2Calo(matchedL2);

		    // Level 2 cut
		    if ( ((matchedL2->e237()/matchedL2->e277()) > m_L2Rcore_cut) &&
			 (((matchedL2->emaxs1()-matchedL2->e2tsts1())/(matchedL2->emaxs1()+matchedL2->e2tsts1())) > m_L2Rstrip_cut) &&
			 ((matchedL2->energy()/cosh(matchedL2->eta())) > m_L2Et_cut) &&
			 ((matchedL2->ehad1()/cosh(matchedL2->eta())) < m_L2Ehad_cut) ) {
		      
		      // Got thru level 2!
		      m_nL2++;
		      
                      cd(name()+"/PreEfficiencies");
		      hist1("TruthL2Et")->Fill(truthEt/1.e3);
		      hist1("TruthL2Eta")->Fill((*itrParticle)->momentum().eta());
		      hist1("TruthL2Phi")->Fill((*itrParticle)->momentum().phi());
		      
		      if (doL2ID) {
			
			float Pt, Eta, Phi, A0, Z0;
			if (runTrack(lvl2t_first, lvl2t_last, matchedL2->eta(), matchedL2->phi(), Pt, Eta, Phi, A0, Z0, TrigInDetTrack::IDSCANID)) {
                          cd(name()+"/Dump");
			  FillLVL2ID(Pt, Eta, Phi, A0, Z0, matchedL2, "Scan");

			  if ( (fabsf(Pt) > m_L2IDPt) &&
			       (fabsf(Eta - matchedL2->eta()) < m_L2IDDeta) &&
			       (delta(Phi - matchedL2->phi()) < m_L2IDDphi) &&
			       (fabsf(matchedL2->energy()/cosh(matchedL2->eta())/Pt) < m_L2IDEP) ) {
			    // Got thru L2ID cut!
			    m_nL2IDScan++;
			    
                            cd(name()+"/PreEfficiencies");
			    hist1("TruthL2IDScanEt")->Fill(truthEt/1.e3);
			    hist1("TruthL2IDScanEta")->Fill((*itrParticle)->momentum().eta());
			    hist1("TruthL2IDScanPhi")->Fill((*itrParticle)->momentum().phi());
 
			    // EF Calo
			    if (doEFCalo) {
			      CaloClusterContainer::const_iterator CCItr;
			      const CaloCluster *matchedEFCalo = 0;
			      float DetaEFCalo = 0.2;
			      float DphiEFCalo = 0.2;
			      const DataHandle<CaloClusterContainer> EFCalo_f = EFCalo_i;
			      for (;EFCalo_f != EFCalo_l;
				   EFCalo_f++) {
				if (EFCalo_f.key().find("HLTAutoKey_TrigCaloClusterMaker") != std::string::npos) {
				  CCItr = EFCalo_f->begin();
				  for (; CCItr != EFCalo_f->end(); CCItr++) {
				    if ( (fabsf((*CCItr)->eta() - matchedL2->eta()) < DetaEFCalo) &&
				         (delta((*CCItr)->phi() - matchedL2->phi()) < DphiEFCalo) ) {
				      DetaEFCalo = fabsf((*CCItr)->eta() - matchedL2->eta());
				      DphiEFCalo = delta((*CCItr)->phi() - matchedL2->phi());
				      matchedEFCalo = (*CCItr);
				    }
			  	  }
				}
			      }
			      if (matchedEFCalo) {
				
                                cd(name()+"/Resolution");
                                hist2("ResEtEFScanTruthEt")->Fill( truthEt/1.e3, 100*((matchedEFCalo->et() - truthEt)/truthEt));
                                hist2("ResEtEFScanTruthEta")->Fill( (*itrParticle)->momentum().eta(), 100*((matchedEFCalo->et() - truthEt)/truthEt));
                                hist2("ResEtEFScanTruthPhi")->Fill( (*itrParticle)->momentum().phi(), 100*((matchedEFCalo->et() - truthEt)/truthEt));
                                hist1("ResEtEFScan")->Fill(100*((matchedEFCalo->et() - truthEt)/truthEt));
                                hist1("ResEtaEFScan")->Fill(100*((matchedEFCalo->eta() - (*itrParticle)->momentum().eta())/((*itrParticle)->momentum().eta())));
                                hist1("ResPhiEFScan")->Fill(100*((matchedEFCalo->phi() - (*itrParticle)->momentum().phi())/((*itrParticle)->momentum().phi())));

                                cd(name()+"/Dump");
				FillEFCalo(matchedEFCalo, DetaEFCalo, DphiEFCalo, "Scan");

				if ( (DetaEFCalo < m_EFCaloDeta) &&
				     (DphiEFCalo < m_EFCaloDphi) &&
				     (matchedEFCalo->et() > m_EFCaloEt) ) {
				  // WHOLE Event thru EF Calo
				  EFScanCalo_pass = true;
				}
				EFScanTruthCaloEt.push_back(truthEt/1.e3);
				EFScanTruthCaloEta.push_back((*itrParticle)->momentum().eta());
				EFScanTruthCaloPhi.push_back((*itrParticle)->momentum().phi());
				
				// Do EF Track
				// Must do it even if EFScanCalo doesn't pass in this iteration,
				// because the hole event might pass on another one
				if (doEFTrack) {
				  egammaContainer::const_iterator eCItr;
				  const DataHandle<egammaContainer> EFTrk_f = EFTrk_i;
				  const egamma *matchedEFTrk = 0;
				  float DetaEFTrk = 0.2;
				  float DphiEFTrk = 0.2;
				  for (; EFTrk_f != EFTrk_l; EFTrk_f++) {
				    if (EFTrk_f.key().find("HLT_egamma") != std::string::npos) {
				      eCItr = EFTrk_f->begin();
				      for (; eCItr != EFTrk_f->end(); eCItr++) {
					if ((*eCItr)->trackParticle()) {
					  extrapolateToFirstSurface((*eCItr)->trackParticle(), caloeta, calophi);
					  if ( (fabsf(caloeta - matchedL2->eta()) < DetaEFTrk) &&
					       (delta(calophi - matchedL2->phi()) < DphiEFTrk) ) {
					    DetaEFTrk = fabsf(caloeta - matchedL2->eta());
					    DphiEFTrk = delta(calophi - matchedL2->phi());
					    matchedEFTrk = (*eCItr);
					  }
					}
				      }
				    }
				  }
				  if (matchedEFTrk) {
				    extrapolateToFirstSurface(matchedEFTrk->trackParticle(), caloeta, calophi);
				    const EMTrackMatch *emTrackMatch = matchedEFTrk->detail<EMTrackMatch>();
				    const Trk::Perigee *per = dynamic_cast<const Trk::Perigee *>(&(matchedEFTrk->trackParticle()->definingParameters()));
                                    const CaloCluster *EFTrackCluster = matchedEFTrk->cluster();
                                    float EoverP = EFTrackCluster->energy()/per->momentum().mag();
				    if (emTrackMatch && EFTrackCluster) {
				      if ( (matchedEFTrk->trackParticle()->trackSummary()) &&
					   (fabsf(emTrackMatch->deltaEta(2)) < m_EFTrackDeta) &&
					   (delta(emTrackMatch->deltaPhi(1)) < m_EFTrackDphi) ) {
					if (fabsf(EFTrackCluster->eta()) > m_EFTrackEtaCut ) {
					  // high eta
					  if ( (EoverP > m_EFTrackLowEoPHighEta) && (EoverP < m_EFTrackHighEoPHighEta) ) {
					    EFScanTrk_pass = true;
					  }
					} else {
					  // low eta
					  if ( (EoverP > m_EFTrackLowEoPLowEta) && (EoverP < m_EFTrackHighEoPLowEta) ) {
					    EFScanTrk_pass = true;
					  }
					}
				      }
				    }
                                    cd(name()+"/Dump");
				    FillEFTrack(matchedEFTrk, "Scan");

				    EFScanTruthTrackEt.push_back(truthEt/1.e3);
				    EFScanTruthTrackEta.push_back((*itrParticle)->momentum().eta());
				    EFScanTruthTrackPhi.push_back((*itrParticle)->momentum().phi());
				  }
				}
				
			      }
			    } // if (doEFCalo)

			  }
			}
			if (runTrack(lvl2t_first, lvl2t_last, matchedL2->eta(), matchedL2->phi(), Pt, Eta, Phi, A0, Z0, TrigInDetTrack::SITRACKID)) {
                          cd(name()+"/Dump");
			  FillLVL2ID(Pt, Eta, Phi, A0, Z0, matchedL2, "Si");

			  if ( (fabsf(Pt) > m_L2IDPt) &&
			       (fabsf(Eta - matchedL2->eta()) < m_L2IDDeta) &&
			       (delta(Phi - matchedL2->phi()) < m_L2IDDphi) &&
			       (fabsf(matchedL2->energy()/cosh(matchedL2->eta())/Pt) < m_L2IDEP) ) {
			    // Got thru L2ID cut!
			    m_nL2IDSi++;
			    
                            cd(name()+"/PreEfficiencies");
			    hist1("TruthL2IDSiEt")->Fill(truthEt/1.e3);
			    hist1("TruthL2IDSiEta")->Fill((*itrParticle)->momentum().eta());
			    hist1("TruthL2IDSiPhi")->Fill((*itrParticle)->momentum().phi());

			    // EF Calo
			    if (doEFCalo) {
			      CaloClusterContainer::const_iterator CCItr;
			      const CaloCluster *matchedEFCalo = 0;
			      float DetaEFCalo = 0.2;
			      float DphiEFCalo = 0.2;
			      const DataHandle<CaloClusterContainer> EFCalo_f = EFCalo_i;
			      for (;EFCalo_f != EFCalo_l;
				   EFCalo_f++) {
				if (EFCalo_f.key().find("HLTAutoKey_TrigCaloClusterMaker") != std::string::npos) {
				  CCItr = EFCalo_f->begin();
				  for (; CCItr != EFCalo_f->end(); CCItr++) {
				    if ( (fabsf((*CCItr)->eta() - matchedL2->eta()) < DetaEFCalo) &&
				         (delta((*CCItr)->phi() - matchedL2->phi()) < DphiEFCalo) ) {
				      DetaEFCalo = fabsf((*CCItr)->eta() - matchedL2->eta());
				      DphiEFCalo = delta((*CCItr)->phi() - matchedL2->phi());
				      matchedEFCalo = (*CCItr);
				    }
				  }
			        }
		              }
			      if (matchedEFCalo) {
                                cd(name()+"/Resolution");
                                hist2("ResEtEFSiTruthEt")->Fill( truthEt/1.e3, 100*((matchedEFCalo->et() - truthEt)/truthEt));
                                hist2("ResEtEFSiTruthEta")->Fill( (*itrParticle)->momentum().eta(), 100*((matchedEFCalo->et() - truthEt)/truthEt));
                                hist2("ResEtEFSiTruthPhi")->Fill( (*itrParticle)->momentum().phi(), 100*((matchedEFCalo->et() - truthEt)/truthEt));
                                hist1("ResEtEFSi")->Fill(100*((matchedEFCalo->et() - truthEt)/truthEt));
                                hist1("ResEtaEFSi")->Fill(100*((matchedEFCalo->eta() - (*itrParticle)->momentum().eta())/((*itrParticle)->momentum().eta())));
                                hist1("ResPhiEFSi")->Fill(100*((matchedEFCalo->phi() - (*itrParticle)->momentum().phi())/((*itrParticle)->momentum().phi())));
				
                                cd(name()+"/Dump");
	 		        FillEFCalo(matchedEFCalo, DetaEFCalo, DphiEFCalo, "Si");

				if ( (DetaEFCalo < m_EFCaloDeta) &&
				     (DphiEFCalo < m_EFCaloDphi) &&
				     (matchedEFCalo->et() > m_EFCaloEt) ) {
				  // HOLE Event thru EF Calo
				  EFSiCalo_pass = true;
				}
				EFSiTruthCaloEt.push_back(truthEt/1.e3);
				EFSiTruthCaloEta.push_back((*itrParticle)->momentum().eta());
				EFSiTruthCaloPhi.push_back((*itrParticle)->momentum().phi());

				// Do EF Track
				// Must do it even if EFSiCalo doesn't pass in this iteration,
				// because the hole event might pass on another one
				if (doEFTrack) {
				  egammaContainer::const_iterator eCItr;
				  const DataHandle<egammaContainer> EFTrk_f = EFTrk_i;
				  const egamma *matchedEFTrk = 0;
				  float DetaEFTrk = 0.2;
				  float DphiEFTrk = 0.2;
				  for (; EFTrk_f != EFTrk_l; EFTrk_f++) {
				    if (EFTrk_f.key().find("HLT_egamma") != std::string::npos) {
				      eCItr = EFTrk_f->begin();
				      for (; eCItr != EFTrk_f->end(); eCItr++) {
					if ((*eCItr)->trackParticle()) {
					  extrapolateToFirstSurface((*eCItr)->trackParticle(), caloeta, calophi);
					  if ( (fabsf(caloeta - matchedL2->eta()) < DetaEFTrk) &&
					       (delta(calophi - matchedL2->phi()) < DphiEFTrk) ) {
					    DetaEFTrk = fabsf(caloeta - matchedL2->eta());
					    DphiEFTrk = delta(calophi - matchedL2->phi());
					    matchedEFTrk = (*eCItr);
					  }
					}
				      }
				    }
				  }
				  if (matchedEFTrk) {
				    extrapolateToFirstSurface(matchedEFTrk->trackParticle(), caloeta, calophi);
				    const EMTrackMatch *emTrackMatch = matchedEFTrk->detail<EMTrackMatch>();
				    const Trk::Perigee *per = dynamic_cast<const Trk::Perigee *>(&(matchedEFTrk->trackParticle()->definingParameters()));
                                    const CaloCluster *EFTrackCluster = matchedEFTrk->cluster();
                                    float EoverP = EFTrackCluster->energy()/per->momentum().mag();
				    if (emTrackMatch && EFTrackCluster) {
				      if ( (matchedEFTrk->trackParticle()->trackSummary()) &&
					   (fabsf(emTrackMatch->deltaEta(2)) < m_EFTrackDeta) &&
					   (delta(emTrackMatch->deltaPhi(1)) < m_EFTrackDphi) ) {
					if (fabsf(EFTrackCluster->eta()) > m_EFTrackEtaCut ) {
					  // high eta
					  if ( (EoverP > m_EFTrackLowEoPHighEta) && (EoverP < m_EFTrackHighEoPHighEta) ) {
					    EFSiTrk_pass = true;
					  }
					} else {
					  // low eta
					  if ( (EoverP > m_EFTrackLowEoPLowEta) && (EoverP < m_EFTrackHighEoPLowEta) ) {
					    EFSiTrk_pass = true;
					  }
					}
				      }
				    }
                                    cd(name()+"/Dump");
				    FillEFTrack(matchedEFTrk, "Si");

				    EFSiTruthTrackEt.push_back(truthEt/1.e3);
				    EFSiTruthTrackEta.push_back((*itrParticle)->momentum().eta());
				    EFSiTruthTrackPhi.push_back((*itrParticle)->momentum().phi());
				  }
				}
				
			      }
			    } // if (doEFCalo)
			  }
			}
		      } // do L2 ID
		    } // level 2 cut
		  } // activated Level 2
		} // do L2
	      } // L1 cut
	    } // matched L1
	  } // do L1
	} // if it got thru the low Et cut
      } // for all events
    } // if event collections in TruthEvent key
  } // for all event collections

  // Add EF stuff to hist1s
  if (doEFCalo) {
    if (EFScanCalo_pass) {
      for (unsigned int counter = 0; counter < EFScanTruthCaloEt.size(); counter++) {
        cd(name()+"/PreEfficiencies");
	hist1("TruthEFScanCaloEt")->Fill(EFScanTruthCaloEt.at(counter));
	hist1("TruthEFScanCaloEta")->Fill(EFScanTruthCaloEta.at(counter));
	hist1("TruthEFScanCaloPhi")->Fill(EFScanTruthCaloPhi.at(counter));
      }
      m_nEFScanCalo += EFScanTruthCaloEt.size();

      if (doEFTrack) {
	for (unsigned int counter = 0; counter < EFScanTruthTrackEt.size(); counter++) {
	  if (EFScanTrk_pass) {
            cd(name()+"/PreEfficiencies");
            hist1("TruthEFScanTrackEt")->Fill(EFScanTruthTrackEt.at(counter));
            hist1("TruthEFScanTrackEta")->Fill(EFScanTruthTrackEta.at(counter));
            hist1("TruthEFScanTrackPhi")->Fill(EFScanTruthTrackPhi.at(counter));
	  }
	}
        cd(name()+"/Dump");
	FillEFScanTrackHistos();
	if (EFScanTrk_pass)
	  m_nEFScanTrack += EFScanTruthTrackEt.size();
      }

    } 
    
    if (EFSiCalo_pass) {
      for (unsigned int counter = 0; counter < EFSiTruthCaloEt.size(); counter++) {
        cd(name()+"/PreEfficiencies");
	hist1("TruthEFSiCaloEt")->Fill(EFSiTruthCaloEt.at(counter));
	hist1("TruthEFSiCaloEta")->Fill(EFSiTruthCaloEta.at(counter));
	hist1("TruthEFSiCaloPhi")->Fill(EFSiTruthCaloPhi.at(counter));
      }
      m_nEFSiCalo += EFSiTruthCaloEt.size();

      if (doEFTrack) {
	for (unsigned int counter = 0; counter < EFSiTruthTrackEt.size(); counter++) {
	  if (EFSiTrk_pass) {
            cd(name()+"/PreEfficiencies");
            hist1("TruthEFSiTrackEt")->Fill(EFSiTruthTrackEt.at(counter));
            hist1("TruthEFSiTrackEta")->Fill(EFSiTruthTrackEta.at(counter));
            hist1("TruthEFSiTrackPhi")->Fill(EFSiTruthTrackPhi.at(counter)); 
          }
	}
        cd(name()+"/Dump");
	FillEFSiTrackHistos();
	if (EFSiTrk_pass)
          m_nEFSiTrack += EFSiTruthTrackEt.size();
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode SingleElectronValidation::childFinalize()
{
  cd(name()+"/Dump");
  FillTable();

  addDirectory(name()+"/Efficiencies");
  std::vector<std::string> PreEffPattern;
  std::vector<std::string> PreEffNotPattern;
  PreEffPattern.push_back(name()+"/PreEfficiencies");
  PreEffNotPattern.push_back("Hist");

  hist1("TruthEtHist", name()+"/PreEfficiencies")->Sumw2();
  hist1("TruthEtaHist", name()+"/PreEfficiencies")->Sumw2();
  hist1("TruthPhiHist", name()+"/PreEfficiencies")->Sumw2();

  std::map<std::string, TH1 *> PreHists;
  getHistsFromPath(PreEffPattern, PreEffNotPattern, PreHists);
  for (std::map<std::string, TH1 *>::iterator i = PreHists.begin(); i != PreHists.end(); i++) {
    TH1F *newhist = new TH1F(*( (TH1F *) i->second));
    addHistogram(newhist);
    newhist->GetYaxis()->SetTitle("Efficiency");
    newhist->Sumw2();
    if (i->first.find_last_of("Et") == (i->first.size() - 1)) {
      newhist->Divide(hist1("TruthEtHist", name()+"/PreEfficiencies"));
    } else if (i->first.find_last_of("Eta") == (i->first.size() - 1)) {
      newhist->Divide(hist1("TruthEtaHist", name()+"/PreEfficiencies"));
    } else if (i->first.find_last_of("Phi") == (i->first.size() - 1)) {
      newhist->Divide(hist1("TruthPhiHist", name()+"/PreEfficiencies"));
    }
  }

  return StatusCode::SUCCESS;
}

