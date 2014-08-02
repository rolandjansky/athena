/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigEgammaValidation/ZeeOfflineValidation.h"

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

#include "egammaEvent/egammaContainer.h"
#include "egammaEvent/egamma.h"
#include "egammaEvent/EMTrackMatch.h"
#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"

#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCluster.h"

#include "FourMom/P4EEtaPhiM.h"

#include "TH1F.h"
#include "TMath.h"

ZeeOfflineValidation::ZeeOfflineValidation(const std::string & name, ISvcLocator * pSvcLocator)
  : FEXValidationAlgo(name, pSvcLocator, "Validation")
{
}

ZeeOfflineValidation::~ZeeOfflineValidation()
{
}

StatusCode ZeeOfflineValidation::childInitialize() {
  StatusCode sc;
  
  addDirectory(name()+"/Dump");
  if ( (sc = prepare()) != StatusCode::SUCCESS)
    return sc;

  addDirectory(name()+"/PreOfflineEfficiencies");
  addHistogram(new TH1F("OffEtHist", "Offline E_{T} for e^{-} ; E_{T} [GeV] ; Count", 80, 0., 80.));
  addHistogram(new TH1F("OffEtaHist", "Offline \\eta for e^{-} ; \\eta ; Count", 128, -3.2, 3.2));
  addHistogram(new TH1F("OffPhiHist", "Offline \\phi for e^{-} ; \\phi [rad] ; Count", 128, -3.2, 3.2));
  
  addHistogram(new TH1F("OffL1Et", "Level 1 Offline E_{T} for e^{-} ; E_{T} [GeV] ; Count", 80, 0., 80.));
  addHistogram(new TH1F("OffL1Eta", "Level 1 Offline \\eta for e^{-} ; \\eta ; Count", 128, -3.2, 3.2));
  addHistogram(new TH1F("OffL1Phi", "Level 1 Offline \\phi for e^{-} ; \\phi [rad] ; Count", 128, -3.2, 3.2));
  
  addHistogram(new TH1F("OffL2Et", "Level 2 Offline E_{T} for e^{-} ; E_{T} [GeV] ; Count", 80, 0., 80.));
  addHistogram(new TH1F("OffL2Eta", "Level 2 Offline \\eta for e^{-} ; \\eta ; Count", 128, -3.2, 3.2));
  addHistogram(new TH1F("OffL2Phi", "Level 2 Offline \\phi for e^{-} ; \\phi [rad] ; Count", 128, -3.2, 3.2));
  
  // For ID Efficiencies
  addHistogram(new TH1F("OffL2IDScanEt", "p_{T} after Level 2 ID (ID Scan Match) for e^{-} ; E_{T} [GeV] ; Count", 80, 0., 80.));
  addHistogram(new TH1F("OffL2IDScanPhi", "\\phi after Level 2 ID (ID Scan Match) for e^{-} ; \\phi [rad] ; Count", 128, -3.2, 3.2));
  addHistogram(new TH1F("OffL2IDScanEta", "\\eta after Level 2 ID (ID Scan Match) for e^{-} ; \\eta ; Count", 128, -3.2, 3.2));
  
  addHistogram(new TH1F("OffL2IDSiEt", "p_{T} after Level 2 ID (SiTrack Match) for e^{-} ; E_{T} [GeV] ; Count", 80, 0., 80.));
  addHistogram(new TH1F("OffL2IDSiPhi", "\\phi after Level 2 ID (SiTrack Match) for e^{-} ; \\phi [rad] ; Count", 128, -3.2, 3.2));
  addHistogram(new TH1F("OffL2IDSiEta", "\\eta after Level 2 ID (SiTrack Match) for e^{-} ; \\eta ; Count", 128, -3.2, 3.2));
 
  // EF Calo
  addHistogram(new TH1F("OffEFScanCaloEt", "Offline E_{T} after EF Calo (and L2 ID Scan Match) for e^{-} ; E_{T} [GeV] ; Count", 80, 0, 80.));
  addHistogram(new TH1F("OffEFScanCaloPhi", "Offline \\phi after EF Calo (and L2 ID Scan Match) for e^{-} ; \\phi [rad] ; Count", 128, -3.2, 3.2));
  addHistogram(new TH1F("OffEFScanCaloEta", "Offline \\eta after EF Calo (and L2 ID Scan Match) for e^{-} ; \\eta ; Count", 128, -3.2, 3.2));

  addHistogram(new TH1F("OffEFSiCaloEt", "Offline E_{T} after EF Calo (and L2 ID SiTrack Match) for e^{-} ; E_{T} [GeV] ; Count", 80, 0, 80.));
  addHistogram(new TH1F("OffEFSiCaloPhi", "Offline \\phi after EF Calo (and L2 ID SiTrack Match) for e^{-} ; \\phi [rad] ; Count", 128, -3.2, 3.2));
  addHistogram(new TH1F("OffEFSiCaloEta", "Offline \\eta after EF Calo (and L2 ID SiTrack Match) for e^{-} ; \\eta ; Count", 128, -3.2, 3.2));

  // EF ID
  addHistogram(new TH1F("OffEFScanTrackEt", "Offline E_{T} after EF Track (and L2 ID Scan Match) for e^{-} ; E_{T} [GeV] ; Count", 80, 0, 80.));
  addHistogram(new TH1F("OffEFScanTrackPhi", "Offline \\phi after EF Track (and L2 ID Scan Match) for e^{-} ; \\phi [rad] ; Count", 128, -3.2, 3.2));
  addHistogram(new TH1F("OffEFScanTrackEta", "Offline \\eta after EF Track (and L2 ID Scan Match) for e^{-} ; \\eta ; Count", 128, -3.2, 3.2));

  addHistogram(new TH1F("OffEFSiTrackEt", "Offline E_{T} after EF Track (and L2 ID SiTrack Match) for e^{-} ; E_{T} [GeV] ; Count", 80, 0, 80.));
  addHistogram(new TH1F("OffEFSiTrackPhi", "Offline \\phi after EF Track (and L2 ID SiTrack Match) for e^{-} ; \\phi [rad] ; Count", 128, -3.2, 3.2));
  addHistogram(new TH1F("OffEFSiTrackEta", "Offline \\eta after EF Track (and L2 ID SiTrack Match) for e^{-} ; \\eta ; Count", 128, -3.2, 3.2));

  addDirectory(name()+"/Resolution");
  addHistogram(new TH1F("ResEtL1", "Level 1 E_{T} resolution w.r.t. Offline ; E_{T} resolution (%) ; Count", 200, -100., 100.));
  addHistogram(new TH1F("ResEtaL1", "Level 1 \\eta resolution w.r.t. Offline ; \\eta resolution (%) ; Count", 200, -100., 100.));
  addHistogram(new TH1F("ResPhiL1", "Level 1 \\phi resolution w.r.t. Offline ; \\phi resolution (%) ; Count", 200, -100., 100.));
  addHistogram(new TH2F("ResEtL1OfflineEt", "Level 1 resolution E_{T} versus E_{T} for e^{-} ; E_{T} [GeV] ; E_{T} Resolution (%)", 80, 0., 80., 200, -100., 100.));
  addHistogram(new TH2F("ResEtL1OfflineEta", "Level 1 resolution E_{T} versus \\eta for e^{-} ; \\eta ; E_{T} Resolution (%)", 128, -3.2, 3.2, 200, -100., 100.));
  addHistogram(new TH2F("ResEtL1OfflinePhi", "Level 1 resolution E_{T} versus \\phi for e^{-} ; \\phi ; E_{T} Resolution (%)", 128, -3.2, 3.2, 200, -100., 100.));
  
  addHistogram(new TH1F("ResEtL2", "Level 2 E_{T} resolution w.r.t. Offline ; E_{T} resolution (%) ; Count", 200, -100., 100.));
  addHistogram(new TH1F("ResEtaL2", "Level 2 \\eta resolution w.r.t. Offline ; \\eta resolution (%) ; Count", 200, -100., 100.));
  addHistogram(new TH1F("ResPhiL2", "Level 2 \\phi resolution w.r.t. Offline ; \\phi resolution (%) ; Count", 200, -100., 100.));
  addHistogram(new TH2F("ResEtL2OfflineEt", "Level 2 resolution E_{T} versus E_{T} for e^{-} ; E_{T} [GeV] ; E_{T} Resolution (%)", 80, 0., 80., 200, -100., 100.));
  addHistogram(new TH2F("ResEtL2OfflineEta", "Level 2 resolution E_{T} versus \\eta for e^{-} ; \\eta ; E_{T} Resolution (%)", 128, -3.2, 3.2, 200, -100., 100.));
  addHistogram(new TH2F("ResEtL2OfflinePhi", "Level 2 resolution E_{T} versus \\phi for e^{-} ; \\phi ; E_{T} Resolution (%)", 128, -3.2, 3.2, 200, -100., 100.));
  
  addHistogram(new TH1F("ResEtEFScan", "Event Filter [L2 Scan] E_{T} resolution w.r.t. Offline ; E_{T} resolution (%) ; Count", 200, -100., 100.));
  addHistogram(new TH1F("ResEtaEFScan", "Event Filter [L2 Scan] \\eta resolution w.r.t. Offline ; \\eta resolution (%) ; Count", 200, -100., 100.));
  addHistogram(new TH1F("ResPhiEFScan", "Event Filter [L2 Scan] \\phi resolution w.r.t. Offline ; \\phi resolution (%) ; Count", 200, -100., 100.));
  addHistogram(new TH2F("ResEtEFScanOfflineEt", "Event Filter [L2 Scan] resolution E_{T} versus E_{T} for e^{-} ; E_{T} [GeV] ; E_{T} Resolution (%)", 80, 0., 80., 200, -100., 100.));
  addHistogram(new TH2F("ResEtEFScanOfflineEta", "Event Filter [L2 Scan] resolution E_{T} versus \\eta for e^{-} ; \\eta ; E_{T} Resolution (%)", 128, -3.2, 3.2, 200, -100., 100.));
  addHistogram(new TH2F("ResEtEFScanOfflinePhi", "Event Filter [L2 Scan] resolution E_{T} versus \\phi for e^{-} ; \\phi ; E_{T} Resolution (%)", 128, -3.2, 3.2, 200, -100., 100.));
  addHistogram(new TH1F("ResEtEFSi", "Event Filter [L2 SiTrack] E_{T} resolution w.r.t. Offline ; E_{T} resolution (%) ; Count", 200, -100., 100.));
  addHistogram(new TH1F("ResEtaEFSi", "Event Filter [L2 SiTrack] \\eta resolution w.r.t. Offline ; \\eta resolution (%) ; Count", 200, -100., 100.));
  addHistogram(new TH1F("ResPhiEFSi", "Event Filter [L2 SiTrack] \\phi resolution w.r.t. Offline ; \\phi resolution (%) ; Count", 200, -100., 100.));
  addHistogram(new TH2F("ResEtEFSiOfflineEt", "Event Filter [L2 SiTrack] resolution E_{T} versus E_{T} for e^{-} ; E_{T} [GeV] ; E_{T} Resolution (%)", 80, 0., 80., 200, -100., 100.));
  addHistogram(new TH2F("ResEtEFSiOfflineEta", "Event Filter [L2 SiTrack] resolution E_{T} versus \\eta for e^{-} ; \\eta ; E_{T} Resolution (%)", 128, -3.2, 3.2, 200, -100., 100.));
  addHistogram(new TH2F("ResEtEFSiOfflinePhi", "Event Filter [L2 SiTrack] resolution E_{T} versus \\phi for e^{-} ; \\phi ; E_{T} Resolution (%)", 128, -3.2, 3.2, 200, -100., 100.));

  addDirectory(name()+"/ZReconstruction");
  addHistogram(new TH1F("OfflineElectronsFound", "Number of Offline Electrons found per event ; Number of Electrons; Count", 10, 0, 10));
  addHistogram(new TH1F("OfflineZMass", "Reconstructed invariant Z Mass from Offline Analysis ; m_{Z} ; Count", 100, 40, 140));
  addHistogram(new TH1F("L1ElectronsFound", "Number of L1 Electrons found per event ; Number of Electrons; Count", 10, 0, 10));
  addHistogram(new TH1F("L1CaloZMass", "Reconstructed invariant Z Mass from L1 Calo Analysis ; m_{Z} ; Count", 100, 40, 140));
  addHistogram(new TH1F("L2CaloElectronsFound", "Number of L2 Calo Electrons found per event ; Number of Electrons; Count", 10, 0, 10));
  addHistogram(new TH1F("L2CaloZMass", "Reconstructed invariant Z Mass from L2 Calo Analysis ; m_{Z} ; Count", 100, 40, 140));
  addHistogram(new TH1F("EFCaloScanElectronsFound", "Number of EF Calo [L2 Scan] Electrons found per event ; Number of Electrons; Count", 10, 0, 10));
  addHistogram(new TH1F("EFCaloScanZMass", "Reconstructed invariant Z Mass from EF Calo [L2 Scan] Analysis ; m_{Z} ; Count", 100, 40, 140));
  addHistogram(new TH1F("EFCaloSiElectronsFound", "Number of EF Calo [L2 SiTrack] Electrons found per event ; Number of Electrons; Count", 10, 0, 10));
  addHistogram(new TH1F("EFCaloSiZMass", "Reconstructed invariant Z Mass from EF Calo [L2 SiTrack] Analysis ; m_{Z} ; Count", 100, 40, 140));

  return StatusCode::SUCCESS;
}

StatusCode ZeeOfflineValidation::childExecute()
{
  StatusCode sc;

  (*m_log) << MSG::DEBUG << name()+"::execute()" << endreq;

  const double ElectronMass = 0.510998910;
  
  std::vector<P4EEtaPhiM> pe_off;
  std::vector<P4EEtaPhiM> pe_l1;
  std::vector<P4EEtaPhiM> pe_l2calo;
  std::vector<P4EEtaPhiM> pe_efcaloscan;
  std::vector<P4EEtaPhiM> pe_efcalosi;
  CLHEP::HepLorentzVector pz_off, pz_l1, pz_l2calo, pz_efcaloscan, pz_efcalosi;
  
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
  
  if (sc.isFailure() || (lvl2t_first == lvl2t_last)) {
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
  std::vector<float> EFScanOffCaloEt;
  std::vector<float> EFScanOffCaloEta;
  std::vector<float> EFScanOffCaloPhi;

  bool EFSiCalo_pass = false;
  std::vector<float> EFSiOffCaloEt;
  std::vector<float> EFSiOffCaloEta;
  std::vector<float> EFSiOffCaloPhi;

  bool doEFTrack = true;
  const DataHandle<egammaContainer> EFTrk_i;
  const DataHandle<egammaContainer> EFTrk_l;
  sc = m_storeGate->retrieve(EFTrk_i, EFTrk_l);
  if (sc.isFailure()) {
    (*m_log) << MSG::WARNING << "Could not retrieve egammaContainer" << endreq;
    doEFTrack = false;
  }

  bool EFScanTrk_pass = false;
  std::vector<float> EFScanOffTrackEt;
  std::vector<float> EFScanOffTrackEta;
  std::vector<float> EFScanOffTrackPhi;

  bool EFSiTrk_pass = false;
  std::vector<float> EFSiOffTrackEt;
  std::vector<float> EFSiOffTrackEta;
  std::vector<float> EFSiOffTrackPhi;

  double caloeta, calophi;

  const DataHandle<egammaContainer> egCbegin,egCend;
  sc = m_storeGate->retrieve(egCbegin,egCend);
  if (sc.isFailure() || (egCbegin == egCend)) {
    (*m_log) << MSG::WARNING << "No egammaContainer found in TDS. Aborting!!!" << endreq;
    return StatusCode::SUCCESS; // Don't stop proccessing events
  }

  egammaContainer::const_iterator Itr, ItrE;
  for ( ; egCbegin != egCend; ++egCbegin) { // For each event collection
    (*m_log) << MSG::DEBUG << "Found egammaContainer named " << egCbegin.key() << endreq;
    if ( egCbegin.key() == "ElectronCollection" ) { 
      Itr = egCbegin->begin();
      ItrE = egCbegin->end();
      for (; Itr != ItrE; ++Itr) { // For each egamma
	// *Itr is an egamma
	if ( ((*Itr)->isem()) == 0 ) { // If it is an electron/positron
	  
	  if ( ((*Itr)->et() > m_lowEtCut) && ((*Itr)->eta() <= m_maxEtaCut) ) {
            cd(name()+"/PreOfflineEfficiencies");
            hist1("OffEtHist")->Fill((*Itr)->et()/1.e3);
            hist1("OffEtaHist")->Fill((*Itr)->eta());
            hist1("OffPhiHist")->Fill((*Itr)->phi());
	    m_nTotal++;
	    
	    pe_off.push_back(P4EEtaPhiM((*Itr)->et()*cosh((*Itr)->eta()), (*Itr)->eta(), (*Itr)->phi(), ElectronMass));

	    if (doL1) {
	      // Did it trigger LVL1?
              LVL1_ROI::emtaus_type::const_iterator itEMTau   = lvl1ROI->getEmTauROIs().begin();
              LVL1_ROI::emtaus_type::const_iterator itEMTau_e = lvl1ROI->getEmTauROIs().end();

              double DetaL1 = 0.2, DphiL1 = 0.2;
              LVL1_ROI::emtaus_type::const_iterator matchedL1;
              bool match_found=false;
              for( ; itEMTau != itEMTau_e; ++itEMTau) {
		if ((fabsf((*Itr)->eta() - (itEMTau)->getEta()) < DetaL1) &&
		    (delta((*Itr)->phi() - (itEMTau)->getPhi()) < DphiL1)) {
		  DetaL1 = fabsf((*Itr)->eta() - (itEMTau)->getEta());
		  DphiL1 = delta((*Itr)->phi() - (itEMTau)->getPhi());
		  matchedL1 = itEMTau;
		  match_found = true;
		}
	      }
	      if (match_found) { // This one activated level 1!!!
	        pe_off.push_back(P4EEtaPhiM(matchedL1->getEMClus(), matchedL1->getEta(), matchedL1->getPhi(), ElectronMass));
                cd(name()+"/Resolution");
                hist2("ResEtL1OfflineEt")->Fill((*Itr)->et()/1.e3, 100*((matchedL1->getEMClus() - (*Itr)->et())/((*Itr)->et())));
                hist2("ResEtL1OfflineEta")->Fill((*Itr)->eta(), 100*((matchedL1->getEMClus() - (*Itr)->et())/((*Itr)->et())));
                hist2("ResEtL1OfflinePhi")->Fill((*Itr)->phi(), 100*((matchedL1->getEMClus() - (*Itr)->et())/((*Itr)->et())));
                hist1("ResEtL1")->Fill(100*((matchedL1->getEMClus() - (*Itr)->et())/((*Itr)->et())));
                hist1("ResPhiL1")->Fill(100*((matchedL1->getPhi() - (*Itr)->phi())/((*Itr)->phi())));
                hist1("ResEtaL1")->Fill(100*((matchedL1->getEta() - (*Itr)->eta())/((*Itr)->eta())));

		// Fill Histograms for L1 info
                cd(name()+"/Dump");
		FillLVL1(matchedL1);

		if ( (matchedL1->getEMClus() > m_L1EmClus) &&
		     (matchedL1->getEMIsol() <= m_L1EmIsol) &&
		     (matchedL1->getHadCore() <= m_L1HadCore) &&
		     (matchedL1->getHadIsol() <= m_L1HadIsol) ) { // Triggered LVL 1!

                  cd(name()+"/PreOfflineEfficiencies");
                  hist1("OffL1Et")->Fill((*Itr)->et()/1.e3);
                  hist1("OffL1Eta")->Fill((*Itr)->eta());
                  hist1("OffL1Phi")->Fill((*Itr)->phi());
		  
		  m_nL1++;
		  
		  if (doL2) {
		    
		    // Finds closest (eta, phi)
		    TrigEMClusterContainer::const_iterator theEMCluster = EMClusterBegin;
		    TrigEMCluster *matchedL2 = 0;
		    double DetaL2 = 0.2;
		    double DphiL2 = 0.2;
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

	              pe_l2calo.push_back(P4EEtaPhiM(matchedL2->energy(), matchedL2->eta(), matchedL2->phi(), ElectronMass));
                      cd(name()+"/Resolution");
                      hist2("ResEtL2OfflineEt")->Fill((*Itr)->et()/1.e3, 100*((matchedL2->energy()/cosh(matchedL2->eta()) - (*Itr)->et())/((*Itr)->et())));
                      hist2("ResEtL2OfflineEta")->Fill((*Itr)->eta(), 100*((matchedL2->energy()/cosh(matchedL2->eta()) - (*Itr)->et())/((*Itr)->et())));
                      hist2("ResEtL2OfflinePhi")->Fill((*Itr)->phi(), 100*((matchedL2->energy()/cosh(matchedL2->eta()) - (*Itr)->et())/((*Itr)->et())));
                      hist1("ResEtL2")->Fill(100*((matchedL2->energy()/cosh(matchedL2->eta()) - (*Itr)->et())/((*Itr)->et())));
                      hist1("ResPhiL2")->Fill(100*((matchedL2->phi() - (*Itr)->phi())/((*Itr)->phi())));
                      hist1("ResEtaL2")->Fill(100*((matchedL2->eta() - (*Itr)->eta())/((*Itr)->eta())));
		      
		      // Fill FEX histograms
                      cd(name()+"/Dump");
		      FillLVL2Calo(matchedL2);

		      // Level 2 cut
		      if ( ((matchedL2->e237()/matchedL2->e277()) > m_L2Rcore_cut) &&
			   (((matchedL2->emaxs1()-matchedL2->e2tsts1())/(matchedL2->emaxs1()+matchedL2->e2tsts1())) > m_L2Rstrip_cut) &&
			   ((matchedL2->energy()/cosh(matchedL2->eta())) > m_L2Et_cut) &&
			   ((matchedL2->ehad1()/cosh(matchedL2->eta())) < m_L2Ehad_cut) ) {
			
			// Got thru level 2!
                        cd(name()+"/PreOfflineEfficiencies");
                        hist1("OffL2Et")->Fill((*Itr)->et()/1.e3);
                        hist1("OffL2Eta")->Fill((*Itr)->eta());
                        hist1("OffL2Phi")->Fill((*Itr)->phi());
			m_nL2++;

			// Did it trigger the Level 2 Inner Detector?
			
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
                              cd(name()+"/PreOfflineEfficiencies");
                              hist1("OffL2IDScanEt")->Fill((*Itr)->et()/1.e3);
                              hist1("OffL2IDScanEta")->Fill((*Itr)->eta());
                              hist1("OffL2IDScanPhi")->Fill((*Itr)->phi());
			      m_nL2IDScan++;

			      // EF Calo
                              if (doEFCalo) {
                                CaloClusterContainer::const_iterator CCItr;
                                const CaloCluster *matchedEFCalo = 0;
                                float DetaEFCalo = 0.2;
                                float DphiEFCalo = 0.2;
                                const DataHandle<CaloClusterContainer> EFCalo_f = EFCalo_i;
                                for (; EFCalo_f != EFCalo_l; EFCalo_f++) {
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
	                          pe_efcaloscan.push_back(P4EEtaPhiM(matchedEFCalo->et()*cosh(matchedEFCalo->eta()), matchedEFCalo->eta(), matchedEFCalo->phi(), ElectronMass));
                                  cd(name()+"/Resolution");
                                  hist2("ResEtEFScanOfflineEt")->Fill((*Itr)->et()/1.e3, 100*(matchedEFCalo->et() - (*Itr)->et())/((*Itr)->et()));
                                  hist2("ResEtEFScanOfflineEta")->Fill((*Itr)->eta(), 100*(matchedEFCalo->et() - (*Itr)->et())/((*Itr)->et()));
                                  hist2("ResEtEFScanOfflinePhi")->Fill((*Itr)->phi(), 100*(matchedEFCalo->et() - (*Itr)->et())/((*Itr)->et()));
                                  hist1("ResEtEFScan")->Fill(100*(matchedEFCalo->et() - (*Itr)->et())/((*Itr)->et()));
                                  hist1("ResPhiEFScan")->Fill(100*(matchedEFCalo->phi() - (*Itr)->phi())/((*Itr)->phi()));
                                  hist1("ResEtaEFScan")->Fill(100*(matchedEFCalo->eta() - (*Itr)->eta())/((*Itr)->eta()));

                                  cd(name()+"/Dump");
				  FillEFCalo(matchedEFCalo, DetaEFCalo, DphiEFCalo, "Scan");

				  if ( (DetaEFCalo < m_EFCaloDeta) &&
				       (DphiEFCalo < m_EFCaloDphi) &&
				       (matchedEFCalo->et() > m_EFCaloEt) ) {
				    // WHOLE Event thru EF Calo
				    EFScanCalo_pass = true;
				  }
				  EFScanOffCaloEt.push_back((*Itr)->et()/1.e3);
				  EFScanOffCaloEta.push_back((*Itr)->eta());
				  EFScanOffCaloPhi.push_back((*Itr)->phi());

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

				      EFScanOffTrackEt.push_back((*Itr)->et()/1.e3);
				      EFScanOffTrackEta.push_back((*Itr)->eta());
				      EFScanOffTrackPhi.push_back((*Itr)->phi());
				    }
				  }
				  
				}
			      }
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
                              cd(name()+"/PreOfflineEfficiencies");
                              hist1("OffL2IDSiEt")->Fill((*Itr)->et()/1.e3);
                              hist1("OffL2IDSiEta")->Fill((*Itr)->eta());
                              hist1("OffL2IDSiPhi")->Fill((*Itr)->phi());
			      m_nL2IDSi++;

			      // EF Calo
			      if (doEFCalo) {
				CaloClusterContainer::const_iterator CCItr;
				const CaloCluster *matchedEFCalo = 0;
				float DetaEFCalo = 0.2;
				float DphiEFCalo = 0.2;
				const DataHandle<CaloClusterContainer> EFCalo_f = EFCalo_i;
				for (; EFCalo_f != EFCalo_l; EFCalo_f++) {
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
	                          pe_efcalosi.push_back(P4EEtaPhiM(matchedEFCalo->et()*cosh(matchedEFCalo->eta()), matchedEFCalo->eta(), matchedEFCalo->phi(), ElectronMass));
                                  cd(name()+"/Resolution");
                                  hist2("ResEtEFSiOfflineEt")->Fill((*Itr)->et()/1.e3, 100*(matchedEFCalo->et() - (*Itr)->et())/((*Itr)->et()));
                                  hist2("ResEtEFSiOfflineEta")->Fill((*Itr)->eta(), 100*(matchedEFCalo->et() - (*Itr)->et())/((*Itr)->et()));
                                  hist2("ResEtEFSiOfflinePhi")->Fill((*Itr)->phi(), 100*(matchedEFCalo->et() - (*Itr)->et())/((*Itr)->et()));
                                  hist1("ResEtEFSi")->Fill(100*(matchedEFCalo->et() - (*Itr)->et())/((*Itr)->et()));
                                  hist1("ResPhiEFSi")->Fill(100*(matchedEFCalo->phi() - (*Itr)->phi())/((*Itr)->phi()));
                                  hist1("ResEtaEFSi")->Fill(100*(matchedEFCalo->eta() - (*Itr)->eta())/((*Itr)->eta()));

                                  cd(name()+"/Dump");
				  FillEFCalo(matchedEFCalo, DetaEFCalo, DphiEFCalo, "Si");
                                  
				  if ( (DetaEFCalo < m_EFCaloDeta) &&
				       (DphiEFCalo < m_EFCaloDphi) &&
				       (matchedEFCalo->et() > m_EFCaloEt) ) {
				    // WHOLE Event thru EF Calo
				    EFSiCalo_pass = true;
				  }
				  EFSiOffCaloEt.push_back((*Itr)->et()/1.e3);
				  EFSiOffCaloEta.push_back((*Itr)->eta());
				  EFSiOffCaloPhi.push_back((*Itr)->phi());

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

				      EFSiOffTrackEt.push_back((*Itr)->et()/1.e3);
				      EFSiOffTrackEta.push_back((*Itr)->eta());
				      EFSiOffTrackPhi.push_back((*Itr)->phi());
				    }
				  }
				}
			      }
			    }
			  }
			} // do L2 ID
		      } // level 2 cut
		    } // activated level 2
		  } // do L2
		} // level 1 cuts
	      } // activated level 1
	    } // do L1
	  } // kinematic cut
	} // isem()
      } // for every one
    } // if it is in egammaCollection
  } // for every collection

  // Add EF stuff to histos
  if (doEFCalo) {
    if (EFScanCalo_pass) {
      cd(name()+"/PreOfflineEfficiencies");
      for (unsigned int counter = 0; counter < EFScanOffCaloEt.size(); counter++) {
        hist1("OffEFScanCaloEt")->Fill(EFScanOffCaloEt.at(counter));
        hist1("OffEFScanCaloEta")->Fill(EFScanOffCaloEta.at(counter));
        hist1("OffEFScanCaloPhi")->Fill(EFScanOffCaloPhi.at(counter));
      }
      m_nEFScanCalo += EFScanOffCaloEt.size();

      if (doEFTrack) {
        cd(name()+"/PreOfflineEfficiencies");
	for (unsigned int counter = 0; counter < EFScanOffTrackEt.size(); counter++) {
	  if (EFScanTrk_pass) {
            hist1("OffEFScanTrackEt")->Fill(EFScanOffTrackEt.at(counter));
            hist1("OffEFScanTrackEta")->Fill(EFScanOffTrackEta.at(counter));
            hist1("OffEFScanTrackPhi")->Fill(EFScanOffTrackPhi.at(counter));
	  }
	}
        cd(name()+"/Dump");
	FillEFScanTrackHistos();
	if (EFScanTrk_pass)
	  m_nEFScanTrack += EFScanOffTrackEt.size();
      }
    } 
    
    if (EFSiCalo_pass) {
      cd(name()+"/PreOfflineEfficiencies");
      for (unsigned int counter = 0; counter < EFSiOffCaloEt.size(); counter++) {
        hist1("OffEFSiCaloEt")->Fill(EFSiOffCaloEt.at(counter));
        hist1("OffEFSiCaloEta")->Fill(EFSiOffCaloEta.at(counter));
        hist1("OffEFSiCaloPhi")->Fill(EFSiOffCaloPhi.at(counter));
      }
      m_nEFSiCalo += EFSiOffCaloEt.size();

      if (doEFTrack) {
        cd(name()+"/PreOfflineEfficiencies");
	for (unsigned int counter = 0; counter < EFSiOffTrackEt.size(); counter++) {
	  if (EFSiTrk_pass) {
            hist1("OffEFSiTrackEt")->Fill(EFSiOffTrackEt.at(counter));
            hist1("OffEFSiTrackEta")->Fill(EFSiOffTrackEta.at(counter));
            hist1("OffEFSiTrackPhi")->Fill(EFSiOffTrackPhi.at(counter)); 
          }
	}
        cd(name()+"/Dump");
	FillEFSiTrackHistos();
	if (EFSiTrk_pass)
          m_nEFSiTrack += EFSiOffTrackEt.size();
      }
    }
  }

  cd(name()+"/ZReconstruction");
  if (pe_off.size() == 2) {
    pz_off = pe_off[0].hlv() + pe_off[1].hlv();
    hist1("OfflineZMass")->Fill(pz_off.m()/1e3);
  }
  hist1("OfflineElectronsFound")->Fill(pe_off.size());
  
  if (pe_l1.size() == 2) {
    pz_l1 = pe_l1[0].hlv() + pe_l1[1].hlv();
    hist1("L1CaloZMass")->Fill(pz_l1.m()/1e3);
  }
  hist1("L1ElectronsFound")->Fill(pe_l1.size());
  
  if (pe_l2calo.size() == 2) {
    pz_l2calo = pe_l2calo[0].hlv() + pe_l2calo[1].hlv();
    hist1("L2CaloZMass")->Fill(pz_l2calo.m()/1e3);
  }
  hist1("L2CaloElectronsFound")->Fill(pe_l2calo.size());
  
  if (pe_efcaloscan.size() == 2) {
    pz_efcaloscan = pe_efcaloscan[0].hlv() + pe_efcaloscan[1].hlv();
    hist1("EFCaloScanZMass")->Fill(pz_efcaloscan.m()/1e3);
  }
  hist1("EFCaloScanElectronsFound")->Fill(pe_efcaloscan.size());
  
  if (pe_efcalosi.size() == 2) {
    pz_efcalosi = pe_efcalosi[0].hlv() + pe_efcalosi[1].hlv();
    hist1("EFCaloSiZMass")->Fill(pz_efcalosi.m()/1e3);
  }
  hist1("EFCaloSiElectronsFound")->Fill(pe_efcalosi.size());

  return StatusCode::SUCCESS;

}

StatusCode ZeeOfflineValidation::childFinalize()
{
  cd(name()+"/Dump");
  FillTable();

  addDirectory(name()+"/OfflineEfficiencies");
  std::vector<std::string> PreEffPattern;
  std::vector<std::string> PreEffNotPattern;
  PreEffPattern.push_back(name()+"/PreOfflineEfficiencies");
  PreEffNotPattern.push_back("Hist");

  hist1("OffEtHist", name()+"/PreOfflineEfficiencies")->Sumw2();
  hist1("OffEtaHist", name()+"/PreOfflineEfficiencies")->Sumw2();
  hist1("OffPhiHist", name()+"/PreOfflineEfficiencies")->Sumw2();

  std::map<std::string, TH1 *> PreHists;
  getHistsFromPath(PreEffPattern, PreEffNotPattern, PreHists);
  for (std::map<std::string, TH1 *>::iterator i = PreHists.begin(); i != PreHists.end(); i++) {
    TH1F *newhist = new TH1F(*( (TH1F *) i->second));
    addHistogram(newhist);
    newhist->GetYaxis()->SetTitle("Efficiency");
    newhist->Sumw2();
    if (i->first.find_last_of("Et") == (i->first.size() - 1)) {
      newhist->Divide(hist1("OffEtHist", name()+"/PreOfflineEfficiencies"));
    } else if (i->first.find_last_of("Eta") == (i->first.size() - 1)) {
      newhist->Divide(hist1("OffEtaHist", name()+"/PreOfflineEfficiencies"));
    } else if (i->first.find_last_of("Phi") == (i->first.size() - 1)) {
      newhist->Divide(hist1("OffPhiHist", name()+"/PreOfflineEfficiencies"));
    }
  }

  return StatusCode::SUCCESS;
}

