/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//#include "Mogens/tauRoI.h";

bool signal = true;

void signal_write( int nevMax=1000000)
{
  tauRoI tauRoI;

  gROOT->cd();

  TrigTauMCTauList::OFFLINESELECTION offlineType = TrigTauMCTauList::NONE;

  // Set an L1 MET cut if desired
  /*
  TrigTauSelectionManager trigMan1;
  trigMan1.setMenuVersion(TrigTauSelectionManager::MENU13030pre);
  trigMan1.setTauMenu(TrigTauFlags::TAU16I);
  trigMan1.l1Sel.print();
  */

  //const char* filename = "ntuples/TTP09/TTP09_prelim/tmp*root";
  //std::string input = "5107.TTP09_prelim";
  //const char* filename = "ntuples/TTP09/holesAmbSolv/tmp*root";
  //std::string input = "5107.holesAmbSolv";
  // const char* filename = "ntuples/TTP09/largeRoIholesAmbNoBackTrk/5107/tmp*root";
  //std::string input = "5107.largeRoIholesAmbNoBackTrk";
  //const char* filename = "ntuples/TTP09/bigRoI/5107/tmp*root";
  //std::string input = "5107.largeRoIholesAmb";
  //const char* filename = "ntuples/TTP09/oldEFID/5107/tmp*root";
  //std::string input = "5107.KalmanFitter";
  //const char* filename = "ntuples/TTP09/July22/5107/tmp*root";
  //std::string input = "5107.July22";
  //const char* filename = "ntuples/TTP09/July22_tauRecCalo/5107/tmp*root";
  //std::string input = "5107.July22_tauRecCalo";
  const char* filename = "ntuples/TTP06_dR03/5107/user.Eli*root";
  std::string input = "5107.TTP06_dR03";



//   switch ( input ) {
//   case 5107:   // W->tau
//     const char* filename = "/Users/dam/Data/TTP09/5107/*";
//     break;
//   case 5188:   // Z -> tautau
//     const char* filename = "/Users/dam/Data/TTP09/5188/*";
//     break;
//   case 5862:   // Z -> tautau
//     const char* filename = "/Users/dam/Data/TTP09/5862/*";
//     break;
//   default:
//     cout << "Incorrect choice of input ntuples" << endl;
//     return;
//   }

  cout << "Opening file(s): " << filename << endl;

  TChain chain("eventTree");
  chain.Reset();
  chain.Add(filename);

  TTree* tree = &chain;

  TrigTauBugFix bugfix(TrigTauBugFix::TTP7, TrigTauBugFix::RDO);

  TrigTauEvent* event = new TrigTauEvent();
  tree->SetBranchAddress("EventBranch", &event);
  tree->SetBranchStatus("*", 1);

  gROOT->cd();

  std::ostringstream os;
  os << "RoI_" << input << ".root";
  TString fileName(os.str());
  TFile *tmvafile = new TFile(fileName,"RECREATE");

  TTree *tmvatree = new TTree("tauRoI","tauRoI variables");
  tmvatree->Branch("tmva", &tauRoI,
		   "run/I:event/I:trigChain/I:mc_Pt/F:mc_decayType/I:seedType/I:nTrk/I:nTrkLoose/I:nTrkEFID/I:nTrkMy/I:chrgTot/I:ptTot/F:ptLead/F:Et/F:EtEMCalib/F:EtHadCalib/F:eta/F:phi/F:nStrips/I:EMRadius/F:stripWidth2/F:isoFrac/F:centFrac/F:sumEmCellEt/F:hadRadius/F:sumHadCellEt/F:calRadius/F:trkMass/F:trkMass3/F:trkMass0/F:tr_Et/F:tr_eta/F:tr_phi/F:tr_EtEMCalib/F:tr_EtHadCalib/F:tr_nStrips/I:tr_EMRadius/F:tr_stripWidth2/F:tr_isoFrac/F:tr_Likelihood/F:tr_nTrk/I:tr_charge/I:tr_llh/F:tr_discriCut/F:L2_eta/F:L2_phi/F:L2_nTrk/I:L1_Et/F:L1_EmIsol/F");

  // Create analyzer class
  TrigTauAnalyzer analyzer;
  /*
  analyzer.truthList.setMaxVisEta(2.5);
  analyzer.truthList.setMinVisPt(0.); // in GeV
  analyzer.truthList.setMinLeadingPt(0.); // pt of the leading pt of track is
  analyzer.truthList.setRecoMatchdR(0.2);
  */
  int nEvent;

  // Loop over events

  //Yes, I know. Problem with Navigation. You need to set up :
  analyzer.chainList.setRoiWordMatching(false);
  //before event loop, then you will not get the problem below.
  //Thanks, Olya

  int nnn[10]={0};

  int nev = int(tree->GetEntries());
  cout << "Number of events in chain: " << nev << endl;
  int nLoop = nev;
  if ( nevMax<nev) nLoop = nevMax;
  for ( int iev = 0; iev<nLoop; ++iev) {
    nEvent++;
    cout << iev << endl;
    if( iev == 466 || iev == 471 || iev == 720 || iev == 1076  ) continue;
    if ( nEvent%1000 == 0 ) cout << "Working ... " << nEvent << endl;

    // Get this event
    event->reset();
    tree->GetEvent(iev);
    bugfix.fix(event);

    // Analyze this event
    analyzer.analyze(event, signal, offlineType);

    unsigned int nTau     = analyzer.truthList.nMCTauGood();
    unsigned int numCh    = analyzer.chainList.nTrigChains();
    unsigned int nTauRec  = event->nTauRec();

    //    cout << "nTau= " << nTau << ", numCh= " << numCh
    //	 << ", nTauRec= " << nTauRec << endl;

    for ( unsigned int iTau = 0; iTau < nTau; ++iTau ) {
      TrigTauMCTau* tau = &analyzer.truthList.mcTauGoodEntry(iTau);
      nnn[0]++;
      if ( tau == 0 )                                     continue;
      nnn[1]++;
      if ( !tau->hadronicDecay() )                        continue;
      nnn[2]++;
      if ( fabs(tau->visible().Eta()) > 2.5 )             continue;
      nnn[3]++;

      if ( tau->visible().Pt() < 15000. )                 continue;
      nnn[4]++;

      bool match=false;
      for (unsigned int iCh=0; iCh < numCh; ++iCh) {
	TrigTauTrigChain ch = analyzer.chainList.trigChainEntry(iCh);
	TrigTauL1Object* tauL1 = ch.L1;
	if ( tauL1 == 0 )                                 continue;
	if ( analyzer.chainList.getL1Tau(*tau) == tauL1 ) {
	  match = true;
	  break;
	}
      }
      if ( !match )                                       continue;
      nnn[5]++;

      if ( tauL1->m_TauEnergy < 6001. )                   continue;
      nnn[6]++;

      const TrigTauRecEFObject* tauEF = ch.EF;
      if ( tauEF == 0 )                                   continue;
      nnn[7]++;

      float mass, mass3, mass0;
      mass = 0.;
      mass0 = 0.;
      if ( tauEF->nLooseTracks()==1 ) {
	mass = tauEF->looseTrack(0).M();
      } else if ( tauEF->nLooseTracks()>1 ) {
	float Px, Py, Pz, E, E0;
	Px = Py = Pz = E = E0 = mass3 = 0.;
	for ( unsigned int iTrk=0; iTrk<tauEF->nLooseTracks(); ++iTrk ) {
	  Px += tauEF->looseTrack(iTrk).Px();
	  Py += tauEF->looseTrack(iTrk).Py();
	  Pz += tauEF->looseTrack(iTrk).Pz();
	  E  += tauEF->looseTrack(iTrk).E();
	  E0 += tauEF->looseTrack(iTrk).P();
	  if ( iTrk==2 ) mass3 = sqrt(E*E-Px*Px-Py*Py-Pz*Pz);
	}
	mass = sqrt(E*E-Px*Px-Py*Py-Pz*Pz);
	mass0 = sqrt(E0*E0-Px*Px-Py*Py-Pz*Pz);
      }
      if ( tauEF->nLooseTracks()<3 ) mass3 = mass;

      float sumCellEt = tauEF->m_sumEmCellEt + tauEF->m_sumHadCellEt;
      float calRadius = -999.;
      if ( sumCellEt>0. )
	calRadius = (tauEF->m_EMRadius * tauEF->m_sumEmCellEt +
		     tauEF->m_hadRadius * tauEF->m_sumHadCellEt)/sumCellEt;


      const TrigTauTrackCollection * trkEFID =
	(const TrigTauTrackCollection*) tauEF->r_EFIDtrackCollection.GetObject();
      int nTrkMy = 0;
      for ( int jTrk=0; trkEFID && jTrk<trkEFID->nTracks(); ++jTrk ) {
	const TrigTauTrack *pTrk = &(trkEFID->track(jTrk));
	double dEta = tauEF->Eta() - pTrk->Eta();
	double dPhi = tauEF->Phi() - pTrk->Phi();
	if (dPhi < -TMath::Pi()) dPhi += TMath::TwoPi();
	if (dPhi > +TMath::Pi()) dPhi -= TMath::TwoPi();
	double dR = sqrt(dEta*dEta+dPhi*dPhi);
	if (    dR               < 0.3
	     && pTrk->Pt()       > 1000.
	     && pTrk->nSiHits()  > 6
	     && pTrk->nPixBlay() > 0
	     && fabs(pTrk->m_a0) < 0.5
 	                                 ) nTrkMy++;
      }

      // Output variables...
      tauRoI.run          = event->runNumber;
      tauRoI.event        = event->eventNumber;
      tauRoI.trigChain    = iCh;
      tauRoI.mc_Pt        = tau->visible().Pt();
      tauRoI.mc_decayType = tau->decayType();
      tauRoI.seedType     = tauEF->m_seedType;
      tauRoI.nTrk         = tauEF->nTracks();
      tauRoI.nTrkLoose    = tauEF->nLooseTracks();
      if( trkEFID )       tauRoI.nTrkEFID     = trkEFID->nTracks();
      else                tauRoI.nTrkEFID     = 0;
      tauRoI.nTrkMy       = nTrkMy;
      tauRoI.chrgTot      = tauEF->chargeTotTracks();
      tauRoI.ptTot        = tauEF->ptTotTracks();
      tauRoI.ptLead       = ( tauEF->nLooseTracks()>0 ) ? tauEF->looseTrack(0).Pt() : 0.;
      tauRoI.Et           = tauEF->Et();
      tauRoI.eta          = tauEF->Eta();
      tauRoI.phi          = tauEF->Phi();
      tauRoI.EtEMCalib    = tauEF->m_etEMCalib;
      tauRoI.EtHadCalib   = tauEF->m_etHadCalib;
      tauRoI.nStrips      = tauEF->m_nStrips;
      tauRoI.EMRadius     = tauEF->m_EMRadius;
      tauRoI.stripWidth2  = tauEF->m_stripWidth2;
      tauRoI.isoFrac      = tauEF->m_isoFrac;
      tauRoI.centFrac     = tauEF->m_centralityFraction;
      tauRoI.sumEmCellEt  = tauEF->m_sumEmCellEt;
      tauRoI.hadRadius    = tauEF->m_hadRadius;
      tauRoI.sumHadCellEt = tauEF->m_sumHadCellEt;
      tauRoI.calRadius    = calRadius;
      tauRoI.trkMass      = mass;
      tauRoI.trkMass3     = mass3;
      tauRoI.trkMass0     = mass0;

      tauRoI.L1_Et        = tauL1->m_TauEnergy;
      tauRoI.L1_EmIsol    = tauL1->m_EmIsol;

      // tauRec
      bool Match = false;
      for ( int iTauR=0; iTauR<nTauRec; ++iTauR ) {
	const TrigTauRecOfflineObject *tauTR = &(event->tauRecEntry(iTauR));
	double dEta = tauEF->Eta() - tauTR->Eta();
	double dPhi = tauEF->Phi() - tauTR->Phi();
	if (dPhi < -TMath::Pi()) dPhi += TMath::TwoPi();
	if (dPhi > +TMath::Pi()) dPhi -= TMath::TwoPi();
	double dR = sqrt(dEta*dEta+dPhi*dPhi);
	if ( dR<0.20 ) {
	  Match = true;
	  break;
	}
      }

      if ( !Match ) {
	tauRoI.tr_Et          = -999.;
	tauToI.tr_eta         = -999.;
	tauToI.tr_phi         = -999.;
	tauRoI.tr_EtEMCalib   = -999.;
	tauRoI.tr_EtHadCalib  = -999.;
	tauRoI.tr_nStrips     = -999;
	tauRoI.tr_EMRadius    = -999.;
	tauRoI.tr_stripWidth2 = -999.;
	tauRoI.tr_isoFrac     = -999.;
	tauRoI.tr_Likelihood  = -999.;
	tauRoI.tr_nTrk        = -1;
	tauRoI.tr_charge      = -1;
      } else {
	tauRoI.tr_Et          = tauTR->Et();
	tauRoI.tr_eta         = tauTR->Eta();
	tauRoI.tr_phi         = tauTR->Phi();
	tauRoI.tr_EtEMCalib   = tauTR->m_etEMCalib;
	tauRoI.tr_EtHadCalib  = tauTR->m_etHadCalib;
	tauRoI.tr_nStrips     = tauTR->m_nStrips;
	tauRoI.tr_EMRadius    = tauTR->m_EMRadius;
	tauRoI.tr_stripWidth2 = tauTR->m_stripWidth2;
	tauRoI.tr_isoFrac     = tauTR->m_isoFrac;
	tauRoI.tr_Likelihood  = tauTR->m_Likelihood;
	tauRoI.tr_nTrk        = tauTR->nTracks();
	tauRoI.tr_charge      = tauTR->m_charge;
      }

      // L2
      TrigTauL2Object* L2Tau = ch.L2;
      if (L2Tau == 0) {
	tauRoI.L2_eta  = -999.;
	tauRoI.L2_phi  = -999.;
	tauRoI.L2_nTrk = -999;
      } else {
	tauRoI.L2_eta  = L2Tau->Eta();
	tauRoI.L2_phi  = L2Tau->Phi();
	tauRoI.L2_nTrk = L2Tau->nTracks();
      }

      tmvatree->Fill();

    }
  }

  cout << "Analysed file(s): " << filename << endl;
  cout << "Total of " << nEvent << " events" << endl;

  delete event;

  for ( int i=0; i<10; ++i ) {
    printf(" %2d, %6d\n", i, nnn[i] );
  }

  tmvatree->Print();
  tmvatree->Write();

  delete tmvatree;
  delete tmvafile;
  return;
}
