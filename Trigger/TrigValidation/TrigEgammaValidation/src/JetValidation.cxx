/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigEgammaValidation/JetValidation.h"

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

JetValidation::JetValidation(const std::string & name, ISvcLocator * pSvcLocator)
  : FEXValidationAlgo(name, pSvcLocator, "Validation")
{
}

JetValidation::~JetValidation()
{
}

StatusCode JetValidation::childInitialize()
{
  StatusCode sc;

  addDirectory(name()+"/Dump");
  if ( (sc = prepare()) != StatusCode::SUCCESS)
    return sc;

  addDirectory(name()+"/PreRates");
  addHistogram(new TH1F("L1Et", "Level 1 E_{T} for jets ; E_{T} [GeV] ; Frequency [Hz] ", 80, 0., 80.));
  addHistogram(new TH1F("L1Eta", "Level 1 \\eta for jets ; \\eta ; Frequency [Hz]", 128, -3.2, 3.2));
  addHistogram(new TH1F("L1Phi", "Level 1 \\phi for jets ; \\phi [rad] ; Frequency [Hz]", 128, -3.2, 3.2));
  
  addHistogram(new TH1F("L2Et", "Level 2 E_{T} for jets ; E_{T} [GeV] ; Frequency [Hz]", 80, 0., 80.));
  addHistogram(new TH1F("L2Eta", "Level 2 \\eta for jets ; \\eta ; Frequency [Hz]", 128, -3.2, 3.2));
  addHistogram(new TH1F("L2Phi", "Level 2 \\phi for jets ; \\phi [rad] ; Frequency [Hz]", 128, -3.2, 3.2));
  
  // For ID Efficiencies
  addHistogram(new TH1F("L2IDScanEt", "E_{T} after Level 2 ID (ID Scan Match) for jets ; E_{T} [GeV] ; Frequency [Hz]", 80, 0., 80.));
  addHistogram(new TH1F("L2IDScanPhi", "\\phi after Level 2 ID (ID Scan Match) for jets ; \\phi [rad] ; Frequency [Hz]", 128, -3.2, 3.2));
  addHistogram(new TH1F("L2IDScanEta", "\\eta after Level 2 ID (ID Scan Match) for jets ; \\eta ; Frequency [Hz]", 128, -3.2, 3.2));
  
  addHistogram(new TH1F("L2IDSiEt", "E_{T} after Level 2 ID (SiTrack Match) for jets ; E_{T} [GeV] ; Frequency [Hz]", 80, 0., 80.));
  addHistogram(new TH1F("L2IDSiPhi", "\\phi after Level 2 ID (SiTrack Match) for jets ; \\phi [rad] ; Frequency [Hz]", 128, -3.2, 3.2));
  addHistogram(new TH1F("L2IDSiEta", "\\eta after Level 2 ID (SiTrack Match) for jets ; \\eta ; Frequency [Hz]", 128, -3.2, 3.2));
  
  // EF Calo
  addHistogram(new TH1F("EFScanCaloEt", "E_{T} at EF Calo (after L2 ID Scan Match) for jets ; E_{T} [GeV] ; Frequency [Hz]", 80, 0, 80.));
  addHistogram(new TH1F("EFScanCaloEta", "\\eta at EF Calo (after L2 ID Scan Match) for jets ; \\eta ; Frequency [Hz]", 128, -3.2, 3.2));
  addHistogram(new TH1F("EFScanCaloPhi", "\\phi at EF Calo (after L2 ID Scan Match) for jets ; \\phi [rad] ; Frequency [Hz]", 128, -3.2, 3.2));

  addHistogram(new TH1F("EFSiCaloEt", "E_{T} at EF Calo (after L2 ID SiTrack Match) for jets ; E_{T} [GeV] ; Frequency [Hz]", 80, 0, 80.));
  addHistogram(new TH1F("EFSiCaloEta", "\\eta at EF Calo (after L2 ID SiTrack Match) for jets ; \\eta ; Frequency [Hz]", 128, -3.2, 3.2));
  addHistogram(new TH1F("EFSiCaloPhi", "\\phi at EF Calo (after L2 ID SiTrack Match) for jets ; \\phi [rad] ; Frequency [Hz]", 128, -3.2, 3.2));

  // EF ID
  addHistogram(new TH1F("EFScanTrackEt", "E_{T} after EF Track (and L2 ID Scan Match) for jets ; E_{T} [GeV] ; Frequency [Hz]", 80, 0, 80.));
  addHistogram(new TH1F("EFScanTrackPhi", "\\phi after EF Track (and L2 ID Scan Match) for jets ; \\phi [rad] ; Frequency [Hz]", 128, -3.2, 3.2));
  addHistogram(new TH1F("EFScanTrackEta", "\\eta after EF Track (and L2 ID Scan Match) for jets ; \\eta ; Frequency [Hz]", 128, -3.2, 3.2));

  addHistogram(new TH1F("EFSiTrackEt", "E_{T} after EF Track (and L2 ID SiTrack Match) for jets ; E_{T} [GeV] ; Frequency [Hz]", 80, 0, 80.));
  addHistogram(new TH1F("EFSiTrackPhi", "\\phi after EF Track (and L2 ID SiTrack Match) for jets ; \\phi [rad] ; Frequency [Hz]", 128, -3.2, 3.2));
  addHistogram(new TH1F("EFSiTrackEta", "\\eta after EF Track (and L2 ID SiTrack Match) for jets ; \\eta ; Frequency [Hz]", 128, -3.2, 3.2));

  return StatusCode::SUCCESS;
}

StatusCode JetValidation::childExecute()
{
  StatusCode sc;

  // All jet events
  m_nTotal++;
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
  std::vector<float> ProbEFScanCaloEt;
  std::vector<float> ProbEFScanCaloEta;
  std::vector<float> ProbEFScanCaloPhi;

  bool EFSiCalo_pass = false;
  std::vector<float> ProbEFSiCaloEt;
  std::vector<float> ProbEFSiCaloEta;
  std::vector<float> ProbEFSiCaloPhi;

  bool doEFTrack = true;
  const DataHandle<egammaContainer> EFTrk_i;
  const DataHandle<egammaContainer> EFTrk_l;
  sc = m_storeGate->retrieve(EFTrk_i, EFTrk_l);
  if (sc.isFailure()) {
    (*m_log) << MSG::WARNING << "Could not retrieve egammaContainer" << endreq;
    doEFTrack = false;
  }

  bool EFScanTrk_pass = false;
  std::vector<float> ProbEFScanTrackEt;
  std::vector<float> ProbEFScanTrackEta;
  std::vector<float> ProbEFScanTrackPhi;

  bool EFSiTrk_pass = false;
  std::vector<float> ProbEFSiTrackEt;
  std::vector<float> ProbEFSiTrackEta;
  std::vector<float> ProbEFSiTrackPhi;

  double caloeta, calophi;

  const LVL1_ROI * lvl1ROI;
  sc = m_storeGate->retrieve(lvl1ROI);
  if (sc.isFailure()) {
    (*m_log) << MSG::WARNING << "Could not find LVL1_ROI in StoreGate. Aborting!!!" << endreq;
    return StatusCode::SUCCESS; // Don't stop proccessing events
  }

  const Trk::Perigee* aMeasPer;
  float pt2;

  // Did it trigger LVL1?
  LVL1_ROI::emtaus_type::const_iterator itEMTau   = lvl1ROI->getEmTauROIs().begin();
  LVL1_ROI::emtaus_type::const_iterator itEMTau_e = lvl1ROI->getEmTauROIs().end();
  LVL1_ROI::emtaus_type::const_iterator matchedL1 = itEMTau;
  for( ; itEMTau != itEMTau_e; ++itEMTau) {
    matchedL1 = itEMTau;

    // Fill Histograms for L1 info
    cd(name()+"/Dump");
    FillLVL1(matchedL1);
    
    if ( (matchedL1->getEMClus() > m_L1EmClus) &&
	 (matchedL1->getEMIsol() <= m_L1EmIsol) &&
	 (matchedL1->getHadCore() <= m_L1HadCore) &&
	 (matchedL1->getHadIsol() <= m_L1HadIsol) ) {
      // Triggered LVL 1!
      m_nL1++;
      
      cd(name()+"/PreRates");
      hist1("L1Et")->Fill(matchedL1->et()/1.e3);
      hist1("L1Eta")->Fill(matchedL1->getEta());
      hist1("L1Phi")->Fill(matchedL1->getPhi());
      
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
	if (matchedL2){
	  // This is the right one!
	  // This one activated level 2!!!
	 
	  // Fill FEX histograms
          cd(name()+"/Dump");
	  FillLVL2Calo(matchedL2);
	  
	  // Level 2 cut
	  if ( ((matchedL2->e237()/matchedL2->e277()) > m_L2Rcore_cut) &&
	       (((matchedL2->emaxs1()-matchedL2->e2tsts1())/(matchedL2->emaxs1()+matchedL2->e2tsts1())) > m_L2Rstrip_cut) &&
	       ((matchedL2->energy()/cosh(matchedL2->eta())) > m_L2Et_cut) &&
	       ((matchedL2->ehad1()/cosh(matchedL2->eta())) < m_L2Ehad_cut) ) {
		 
	    // Got thru level 2!
	    m_nL2++;

	    cd(name()+"/PreRates");
            hist1("L2Et")->Fill((matchedL2->energy()/cosh(matchedL2->eta()))/1.e3);
            hist1("L2Eta")->Fill(matchedL2->eta());
            hist1("L2Phi")->Fill(matchedL2->phi());
	    
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
                  cd(name()+"/PreRates");
                  hist1("L2IDScanEt")->Fill(fabsf(Pt)/1.e3);
                  hist1("L2IDScanEta")->Fill(Eta);
                  hist1("L2IDScanPhi")->Fill(Phi);
		  
		  m_nL2IDScan++;

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
		      cd(name()+"/Dump");
		      FillEFCalo(matchedEFCalo, DetaEFCalo, DphiEFCalo, "Scan");

		      if ( (DetaEFCalo < m_EFCaloDeta) &&
			   (DphiEFCalo < m_EFCaloDphi) &&
			   (matchedEFCalo->et() > m_EFCaloEt) ) {
			// HOLE Event thru EF Calo
			EFScanCalo_pass = true;
		      }
		      ProbEFScanCaloEt.push_back(matchedEFCalo->et()/1.e3);
		      ProbEFScanCaloEta.push_back(matchedEFCalo->eta());
		      ProbEFScanCaloPhi.push_back(matchedEFCalo->phi());

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
			  
			  aMeasPer = matchedEFTrk->trackParticle()->perigee();
			  pt2 = pow(aMeasPer->parameters()[Trk::px], 2.) + pow(aMeasPer->parameters()[Trk::py], 2.);

			  ProbEFScanTrackEt.push_back(sqrt(pt2)/1.e3);
			  ProbEFScanTrackEta.push_back(caloeta);
			  ProbEFScanTrackPhi.push_back(calophi);
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
                  cd(name()+"/PreRates");
                  hist1("L2IDSiEt")->Fill(fabsf(Pt)/1.e3);
                  hist1("L2IDSiEta")->Fill(fabsf(Eta));
                  hist1("L2IDSiPhi")->Fill(fabsf(Phi));
		  
		  m_nL2IDSi++;
		  
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
                      cd(name()+"/Dump");
		      FillEFCalo(matchedEFCalo, DetaEFCalo, DphiEFCalo, "Si");

		      if ( (DetaEFCalo < m_EFCaloDeta) &&
			   (DphiEFCalo < m_EFCaloDphi) &&
			   (matchedEFCalo->et() > m_EFCaloEt) ) {
			// HOLE Event thru EF Calo
			EFSiCalo_pass = true;
		      }
		      ProbEFSiCaloEt.push_back(matchedEFCalo->et()/1.e3);
		      ProbEFSiCaloEta.push_back(matchedEFCalo->eta());
		      ProbEFSiCaloPhi.push_back(matchedEFCalo->phi());

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
			  
			  aMeasPer = matchedEFTrk->trackParticle()->perigee();
			  pt2 = pow(aMeasPer->parameters()[Trk::px], 2.) + pow(aMeasPer->parameters()[Trk::py], 2.);

			  ProbEFSiTrackEt.push_back(sqrt(pt2)/1.e3);
			  ProbEFSiTrackEta.push_back(caloeta);
			  ProbEFSiTrackPhi.push_back(calophi);
			}
		      }

		    }
		  }

		}
	      }
	    } // do L2 ID
	  } // level 2 cut
	} // activated Level 2
      } // do L2
    } // triggered Level 1
  } // activated Level 1

  // Add EF stuff to histos
  if (doEFCalo) {
    if (EFScanCalo_pass) {
      for (unsigned int counter = 0; counter < ProbEFScanCaloEt.size(); counter++) {
        cd(name()+"/PreRates");
        hist1("EFScanCaloEt")->Fill(ProbEFScanCaloEt.at(counter));
        hist1("EFScanCaloEta")->Fill(ProbEFScanCaloEta.at(counter));
        hist1("EFScanCaloPhi")->Fill(ProbEFScanCaloPhi.at(counter));
      }
      m_nEFScanCalo += ProbEFScanCaloEt.size();

      if (doEFTrack) {
	for (unsigned int counter = 0; counter < ProbEFScanTrackEt.size(); counter++) {
	  if (EFScanTrk_pass) {
            cd(name()+"/PreRates");
            hist1("EFScanTrackEt")->Fill(ProbEFScanTrackEt.at(counter));
            hist1("EFScanTrackEta")->Fill(ProbEFScanTrackEta.at(counter));
            hist1("EFScanTrackPhi")->Fill(ProbEFScanTrackPhi.at(counter));
	  }
	}
        cd(name()+"/Dump");
	FillEFScanTrackHistos();
	if (EFScanTrk_pass)
	  m_nEFScanTrack += ProbEFScanTrackEt.size();
      }
    } 
    
    if (EFSiCalo_pass) {
      for (unsigned int counter = 0; counter < ProbEFSiCaloEt.size(); counter++) {
        cd(name()+"/PreRates");
        hist1("EFSiCaloEt")->Fill(ProbEFSiCaloEt.at(counter));
        hist1("EFSiCaloEta")->Fill(ProbEFSiCaloEta.at(counter));
        hist1("EFSiCaloPhi")->Fill(ProbEFSiCaloPhi.at(counter));
      }
      m_nEFSiCalo += ProbEFSiCaloEt.size();

      if (doEFTrack) {
	for (unsigned int counter = 0; counter < ProbEFSiTrackEt.size(); counter++) {
	  if (EFScanTrk_pass) {
            cd(name()+"/PreRates");
            hist1("EFSiTrackEt")->Fill(ProbEFSiTrackEt.at(counter));
            hist1("EFSiTrackEta")->Fill(ProbEFSiTrackEta.at(counter));
            hist1("EFSiTrackPhi")->Fill(ProbEFSiTrackPhi.at(counter));
	  }
	}
        cd(name()+"/Dump");
	FillEFSiTrackHistos();
	if (EFSiTrk_pass)
	  m_nEFSiTrack += ProbEFSiTrackEt.size();
      }
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode JetValidation::childFinalize()
{
  // Re-scale Probability graphs for jets
  std::vector<std::string> PreEffPattern;
  std::vector<std::string> PreEffNotPattern;
  PreEffPattern.push_back(name()+"/PreRates");

  float factor = 0;
  if (m_nTotal) {
    factor = (m_sL)/m_nTotal; // in Hz
  }

  std::map<std::string, TH1 *> PreHists;
  getHistsFromPath(PreEffPattern, PreEffNotPattern, PreHists);

  addDirectory(name()+"/Rates");  
  for (std::map<std::string, TH1 *>::iterator i = PreHists.begin(); i != PreHists.end(); i++) {
    TH1F *newhist = new TH1F(*( (TH1F *) i->second));
    addHistogram(newhist);
    newhist->GetYaxis()->SetTitle("Frequency [Hz]");
    scaleError(factor, newhist);
  }
 
  cd(name()+"/Dump");
  FillTable();

  return StatusCode::SUCCESS;
}


