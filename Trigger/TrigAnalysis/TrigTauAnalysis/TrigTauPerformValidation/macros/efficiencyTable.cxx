/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iomanip>

void efficiencyTable()
{
  //cout << "\n\n  +++++++++++++++ 13.0.30.3 ++++++++++++++++++++\n\n\n";
  //efficiencyTable( TrigTauSelectionManager::MENU13030pre, TrigTauSelectionManager::LOOSE);
  cout << "\n\n  +++++++++++++++ LOOSE ++++++++++++++++++++\n\n\n";
  efficiencyTable( TrigTauSelectionManager::MENU1400, TrigTauSelectionManager::LOOSE);
  cout << "\n\n  +++++++++++++++ MEDIUM ++++++++++++++++++++\n\n\n";
  efficiencyTable( TrigTauSelectionManager::MENU1400, TrigTauSelectionManager::MEDIUM);
  cout << "\n\n  +++++++++++++++ TIGHT ++++++++++++++++++++\n\n\n";
  efficiencyTable( TrigTauSelectionManager::MENU1400, TrigTauSelectionManager::TIGHT);
}



void efficiencyTable(TrigTauSelectionManager::MENUVERSION version ,
		     TrigTauSelectionManager::CUTLEVEL cutLevel)
{

  cout << "======================== J0 sample ====================\n";
  efficiencyTable(5009,0, version, cutLevel);
  TH1F * hJ0 = ((TH1F*)gROOT->Get("hEff"))->Clone("hJ0");
  cout << "======================== J1 sample ====================\n";
  efficiencyTable(5010,0, version, cutLevel);
  TH1F * hJ1 = ((TH1F*)gROOT->Get("hEff"))->Clone("hJ1");
  cout << "======================== J2 sample ====================\n";
  efficiencyTable(5011,0, version, cutLevel);
  TH1F * hJ2 = ((TH1F*)gROOT->Get("hEff"))->Clone("hJ2");
  cout << "======================== J3 sample ====================\n";
  efficiencyTable(5012,0, version, cutLevel);
  TH1F * hJ3 = ((TH1F*)gROOT->Get("hEff"))->Clone("hJ3");
  cout << "======================== W sample ====================\n";
  efficiencyTable(5107,1, version, cutLevel);
  TH1F * h5107 = ((TH1F*)gROOT->Get("hEff"))->Clone("h5107");
  cout << "======================== Z sample ====================\n";
//   efficiencyTable(5188,1, version, cutLevel);
//   cout << "======================== A(m=800) sample ====================\n";
//   efficiencyTable(5862,1, version, cutLevel);

  cout << "==============================================================\n";
  hJ0->Scale(hJ0->GetBinContent(1));
  hJ1->Scale(hJ1->GetBinContent(1));
  hJ2->Scale(hJ2->GetBinContent(1));
  hJ3->Scale(hJ3->GetBinContent(1));
  int nDec = hJ0->GetNbinsX();
  for(unsigned int itau = 1 ; itau < nDec+1 ; ++itau)
    {
      if( TString(hDec->GetXaxis()->GetBinLabel(itau)).Length() == 0 ) 
	{ cout << "\\hline \n"; continue;}
      float rate = oGetRate(hJ0->GetBinContent(itau),hJ1->GetBinContent(itau),
		      hJ2->GetBinContent(itau),hJ3->GetBinContent(itau), false);
      cout << "\\sig\{" <<  std::setw(15)<<hJ0->GetXaxis()->GetBinLabel(itau) << "} \& "
	   <<  std::setprecision(3)<< rate << endl;      
      //if( itau == 2 || itau == singleOffset+nTauSingle || 
      //  itau == tautauOffset+nTauTau || itau==taumetOffset+nTauMet || 
      //  itau== taueOffset+nTauE || itau==taumuOffset+nTauMu || itau==tauJetOffset+nTauJet 
      //  ) cout << " \\hline \n";
    }
  



}


void efficiencyTable(int id,  int signal  = 0,
		     TrigTauSelectionManager::MENUVERSION version,
		     TrigTauSelectionManager::CUTLEVEL cutLevel
		     )//, TTree * tree)
{
  
  bool doDouble = 1;
  bool doTauMet = 0;
  bool doTauE = 0;
  bool doTauMu = 0;
  bool doTauJet = 0;





  int nDec = 1; // 0 bin is all events

  //Tau chains
  nDec++;
  int singleOffset = nDec;
  const unsigned int nTauSingle = 9;
  TrigTauFlags::TRIGGERMENU bits[nTauSingle] = { TrigTauFlags::TAU12,  
						 TrigTauFlags::TAU16I,  TrigTauFlags::TAU16I,
						 TrigTauFlags::TAU29I,  TrigTauFlags::TAU38,
						 TrigTauFlags::TAU38I,  TrigTauFlags::TAU50,
						 TrigTauFlags::TAU84,  TrigTauFlags::TAU125};
  bool passedSingle[nTauSingle];
  
  nDec += nTauSingle;

  // 2taus
  nDec++;
  int tautauOffset = nDec;
  const unsigned int nTauTau = 5;
  TString tauTauNames[nTauTau] = { "2tau12",
				   "2tau20i",
				   "2tau29i",
				   "tau12\\_tau29i",
				   "tau12\\_tau38"};
  nDec +=nTauTau;
  

  // TAU+met
  nDec++;
  int taumetOffset = nDec;
  const int nTauMet = 14;
  TString tauMetNames[nTauMet] = { "tau12\\_xE20",
				   "tau16i\\_xE30",
				   "tau16i\\_EFxE30",
				   "tau16i\\_EFxE50",
				   "tau16i\\_xE70",
				   "tau16i\\_EFxE70",
				   "tau20i\\_xE30",
				   "tau29i\\_xE20",
				   "tau29i\\_xE30",
				   "tau29i\\_xE40",
				   "tau29i\\_EFxE40",
				   "tau38\\_xE20",
				   "tau38\\_xE40",
				   "tau38\\_EFxE40"
 };
  nDec+=nTauMet;

  // tau+e
  nDec++;
  int taueOffset = nDec;
  const int nTauE = 6;
  TString tauENames[nTauE] = { "tau12\\_e10",
			       "tau16i\\_e10",
			       "tau16i\\_e15i",
			       "tau20i\\_e10",
			       "tau20i\\_e15i",
			       "tau20i\\_e25i"};
  nDec += nTauE;

  // tau+mu
  nDec++;
  int taumuOffset = nDec;
  const int nTauMu = 5;
  TString tauMuNames[nTauMu] = {"tau16i\\_mu6",
			       "tau16i\\_mu10",
			       "tau20i\\_mu6",
			       "tau20i\\_mu10",
			       "tau20i\\_mu20"};
  nDec += nTauMu;

  // tau+jets
  int tauJetOffset = nDec;
  const int nTauJet = 6;
  TString tauJetNames[nTauJet] = {"tau16i\\_j70",
				 "tau16i\\_3j23",
				 "tau16i\\_4j23",
				  "tau20i\\_4j50",
				  "tau16i\\_xE30\\_j18",
				  "tau16i\\_xE30\\_j35"};
  nDec += nTauJet;
  
    
  TH1F * hDec = new TH1F("hDec","Passed trigger",nDec+1, -0.5, nDec+0.5);
  hDec->GetXaxis()->SetBinLabel(1,"offline tau");
  for(unsigned int itau = 0;  itau < nTauSingle; ++itau )
    {
      TString name =  TrigTauFlags::menuName(bits[itau]);
      hDec->GetXaxis()->SetBinLabel(singleOffset+itau+1,name);
    }
  for(unsigned int itau = 0;  itau < nTauTau; ++itau )
    hDec->GetXaxis()->SetBinLabel(tautauOffset+itau+1, tauTauNames[itau]);
  for(unsigned int itau = 0;  itau < nTauMet; ++itau )
    hDec->GetXaxis()->SetBinLabel(taumetOffset+itau+1, tauMetNames[itau]);
  for(unsigned int itau = 0;  itau < nTauE; ++itau )
    hDec->GetXaxis()->SetBinLabel(taueOffset+itau+1, tauENames[itau]);
  for(unsigned int itau = 0;  itau < nTauMu; ++itau )
    hDec->GetXaxis()->SetBinLabel(taumuOffset+itau+1, tauMuNames[itau]);
  for(unsigned int itau = 0;  itau < nTauJet; ++itau )
    hDec->GetXaxis()->SetBinLabel(tauJetOffset+itau+1, tauJetNames[itau]);
  hDec->LabelsOption("v");

  TH1F * hMC = hDec->Clone("hMC"); hMC->SetTitle(" passed MC acceptance");
  hMC->LabelsOption("v");




  //-------------------------------------------------------------------------
  //TrigTauSelectionManager::MENUVERSION version = TrigTauSelectionManager::MENU13030pre;


  TrigTauBugFix bugfix (TrigTauBugFix::TTP9, TrigTauBugFix::RDO);
  //bugfix.setTTPVersion(filename);

  //-------------------------------------------------------------------------
  // Now work with events

  int nprong        = 0;
  bool checkOffline = true;
  TrigTauMCTauList::OFFLINESELECTION offlineType =  TrigTauMCTauList::EITHERTAU;

  TrigTauAnalyzer analyzer;
  analyzer.trigMan.setMenuVersion(version);
  analyzer.trigMan.setTauMenu(TrigTauFlags::TAU16I, cutLevel);

  analyzer.truthList.setMaxVisEta(2.5);
  analyzer.truthList.setMinVisPt(10.);
  if( nprong == 1 )  analyzer.truthList.select1Prong();
  else  if( nprong == 3 )  analyzer.truthList.select3Prong();
  analyzer.truthList.setRecoMatchdR(0.2);


  TChain * tree = new TChain("eventTree");
  bool filter= true; 
  if( version == TrigTauSelectionManager::MENU13030pre )
    oSampleTTP06(id, tree);
  else {
    // oSampleTTP06_dR03(id, tree);
    oSampleTTP09(id, tree);
    filter = false;
  }

  if (tree == 0) {cerr << "Can't find eventTree in " << filename << endl;    return 0; }

  TrigTauEvent* event = new TrigTauEvent();

  tree->SetBranchAddress("EventBranch", &event);
  tree->SetBranchStatus("*", 1);

  //===================  loop over events =======================================
  int nEvent = 0;
  int nev = int(tree->GetEntries());
  //if( nev > 1000 ) nev = 1000;
  for(int iev = 0; iev<nev; ++iev) {
    event->reset();
    tree->GetEvent(iev);
    bugfix.fix(event);
    hMC->Fill(0);

    if( !signal && !event->decision.passedL1(TrigTauFlags::TAU12) )
      {
	//nEvent++;
	//hDec->Fill(0);
	continue;
      }

    analyzer.analyze(event, signal, offlineType);

    // ============ Select type of events (e.g. signal) ================
     if( signal &&  analyzer.truthList.nMCTauGood() ==0 ) continue;
    nEvent++;
    hDec->Fill(0);

    
    float leadingTauPt = 0;
    if( analyzer.truthList.leadingPtGoodTau() ) 
      leadingTauPt = analyzer.truthList.leadingPtGoodTau()->visible().Pt();
    float nextTauPt = 0;
    if(analyzer.truthList.nextPtGoodTau() ) 
      nextTauPt = analyzer.truthList.nextPtGoodTau()->visible().Pt();
    //cout << leadingTauPt << endl;
    
    // now look for leptons
    int nMC = event->nMC();
    float leadingEPt = 0;
    float leadingMuPt = 0;
    for(unsigned int iMC = 0; iMC < nMC; ++iMC)
      {
	TrigTauTrueParticle * part = &event->MCEntry(iMC);
	if( part == 0 ) continue;
	int lund = abs(part->pdgId());
	//cout << " lund " << lund << endl;
	if(lund != 11 && lund != 13 ) continue;

	// offline match
	if( lund == 11 ){
	  int nElectron = event->offlineEvent.nElectrons();
	  for(unsigned int iElectron = 0; iElectron < nElectron; ++iElectron)
	    {
	      TrigTauElectron * elec = &event->offlineEvent.electronEntry(iElectron);
	      if( part->DeltaR(*elec) > 0.2 ) continue;
	      // offline match is found!
	      float pt = elec->Pt();
	      if( pt > leadingEPt ) leadingEPt  =pt;
	    } 
	  
	}
	if( lund == 13 ){
	  int nMuon = event->offlineEvent.nMuons();
	  for(unsigned int iMuon = 0; iMuon < nMuon; ++iMuon)
	    {
	      TrigTauMuon * muon = &event->offlineEvent.muonEntry(iMuon);
	      if( part->DeltaR(*muon) > 0.2 ) continue;
	      // offline match is found!
	      float pt = muon->Pt();
	      if( pt > leadingMuPt ) leadingMuPt  =pt;
	    } 
	}
      }

    float jetPt[10];
    for(unsigned int i=0; i<10; ++i)
      jetPt[i] = 0;
    int nJet = event->offlineEvent.nJets();
    int jetPtfilled = 0;
    for(unsigned int iJet = 0; iJet < nJet; ++iJet)
      {
	TLorentzVector * jet = &event->offlineEvent.jetEntry(iJet);
	//remove overlap with leading tau
	if( analyzer.truthList.leadingPtGoodTau() &&
	    jet->DeltaR( analyzer.truthList.leadingPtGoodTau()->visible()) < 0.2 ) 
	  continue;

	//match to truth
	int ntrueJet = event->mcEvent.nTrueJets();
	bool matched = false;
	for(unsigned int itruejet = 0; !matched && itruejet < ntrueJet; ++itruejet)
	  {
	    if( jet->DeltaR( event->mcEvent.trueJetsEntry(itruejet)) < 0.2)
	      matched = true;
	  }
	if( !matched) continue;

	
	
	if( jetPtfilled < 10 )
	  {
	    jetPt[jetPtfilled] = jet->Pt();
	    jetPtfilled++;
	  }else{
	  float pt = jet->Pt();
	  mysort(jetPt);
	  if( jetPt[9] > pt ) jetPt[9] = pt;
	}
      } //end loop over jets

    mysort(jetPt);

    //==================================================================
    // single taus :
    for(unsigned int itau1 = 0; itau1 < nTauSingle; ++itau1)
      {
	passedSingle[itau1] = false; 
	if( signal && !passedTauPt( leadingTauPt, bits[itau1] ) ) continue;
	int ibin = singleOffset+itau1;
	hMC->Fill(float(ibin));

	TrigTauSelectionManager trigMan;
	trigMan.setMenuVersion(version);
	trigMan.setTauMenu(bits[itau1],cutLevel);
	
	bool noCandidate = true;
	
	unsigned int numCh = analyzer.chainList.nTrigChains();
	unsigned int nOther = event->nOtherTrigRoI();
	for (unsigned int iCh1=0; noCandidate && iCh1 < numCh; iCh1++) {
	  TrigTauTrigChain ch1 = analyzer.chainList.trigChainEntry(iCh1);
	  //cout << " tries \n";
	  if (trigMan.passedEF(ch1))noCandidate = false;
	  //if( !noCandidate ) cout << " found !!!!!! \n";
	}

	if( !noCandidate ){ hDec->Fill(float(ibin)); passedSingle[itau1] = true; }
      } // end single tau
    
    // 2tau                                                                 FIRST low PT, then High PT
    if( doDouble ) {
    checkTauTau(analyzer, event, leadingTauPt, nextTauPt,tautauOffset+0, hMC,hDec, TrigTauFlags::TAU12, TrigTauFlags::TAU12,version, cutLevel,signal, passedSingle[3], passedSingle[3] );
    checkTauTau(analyzer, event, leadingTauPt, nextTauPt,tautauOffset+1, hMC,hDec, TrigTauFlags::TAU20I, TrigTauFlags::TAU20I,version, cutLevel,signal, passedSingle[5], passedSingle[5]);
    checkTauTau(analyzer, event, leadingTauPt, nextTauPt,tautauOffset+2, hMC,hDec, TrigTauFlags::TAU29I, TrigTauFlags::TAU29I,version, cutLevel,signal, passedSingle[6], passedSingle[6]);
    checkTauTau(analyzer, event, leadingTauPt, nextTauPt,tautauOffset+3, hMC,hDec, TrigTauFlags::TAU10I, TrigTauFlags::TAU38, version, cutLevel,signal, passedSingle[1], passedSingle[7]);
    checkTauTau(analyzer, event, leadingTauPt, nextTauPt,tautauOffset+4, hMC,hDec, TrigTauFlags::TAU12, TrigTauFlags::TAU29I,version, cutLevel,signal, passedSingle[3], passedSingle[6]);
    checkTauTau(analyzer, event, leadingTauPt, nextTauPt,tautauOffset+5, hMC,hDec, TrigTauFlags::TAU12, TrigTauFlags::TAU38, version, cutLevel,signal, passedSingle[3], passedSingle[7]);
    }    
    // tau+MET
    if( doTauMet ) {
      int ibintaumet = taumetOffset;
    checkTauMET(analyzer, event, leadingTauPt, ibintaumet++, hMC, hDec, TrigTauFlags::TAU15,  TrigTauFlags::MET20, version, cutLevel,signal, passedSingle[3]);
    checkTauMET(analyzer, event, leadingTauPt, ibintaumet++, hMC, hDec, TrigTauFlags::TAU16I,  TrigTauFlags::MET30, version, cutLevel,signal, passedSingle[4]);
    checkTauEFMET(analyzer, event, leadingTauPt, ibintaumet++, hMC, hDec, TrigTauFlags::TAU16I,  TrigTauFlags::MET30, version, cutLevel,signal, passedSingle[4]);
    checkTauEFMET(analyzer, event, leadingTauPt, ibintaumet++, hMC, hDec, TrigTauFlags::TAU16I,  TrigTauFlags::MET50, version, cutLevel,signal, passedSingle[4]);
    checkTauMET(analyzer, event, leadingTauPt, ibintaumet++, hMC, hDec, TrigTauFlags::TAU16I,  TrigTauFlags::MET70, version, cutLevel,signal, passedSingle[4]);
    checkTauEFMET(analyzer, event, leadingTauPt, ibintaumet++, hMC, hDec, TrigTauFlags::TAU16I,  TrigTauFlags::MET70, version, cutLevel,signal, passedSingle[4]);

    checkTauMET(analyzer, event, leadingTauPt, ibintaumet++, hMC, hDec, TrigTauFlags::TAU20I,  TrigTauFlags::MET30, version, cutLevel,signal, passedSingle[5]);
    checkTauMET(analyzer, event, leadingTauPt, ibintaumet++, hMC, hDec, TrigTauFlags::TAU29I,  TrigTauFlags::MET20, version, cutLevel,signal, passedSingle[6]);
    checkTauMET(analyzer, event, leadingTauPt, ibintaumet++, hMC, hDec, TrigTauFlags::TAU29I,  TrigTauFlags::MET30, version, cutLevel,signal, passedSingle[6]);
    checkTauMET(analyzer, event, leadingTauPt, ibintaumet++, hMC, hDec, TrigTauFlags::TAU29I,  TrigTauFlags::MET40, version, cutLevel,signal, passedSingle[6]);
    checkTauEFMET(analyzer, event, leadingTauPt, ibintaumet++, hMC, hDec, TrigTauFlags::TAU29I,  TrigTauFlags::MET40, version, cutLevel,signal, passedSingle[6]);
    checkTauMET(analyzer, event, leadingTauPt, ibintaumet++, hMC, hDec, TrigTauFlags::TAU38,  TrigTauFlags::MET20, version, cutLevel,signal, passedSingle[8]);
    checkTauMET(analyzer, event, leadingTauPt, ibintaumet++, hMC, hDec, TrigTauFlags::TAU38,   TrigTauFlags::MET40, version, cutLevel,signal, passedSingle[7]);
    checkTauEFMET(analyzer, event, leadingTauPt, ibintaumet++, hMC, hDec, TrigTauFlags::TAU38,  TrigTauFlags::MET40, version, cutLevel,signal, passedSingle[7]);
    }

    // tau+lep
    if( doTauE ) {
    checkTauE(analyzer, event, leadingTauPt, leadingEPt,taueOffset+0, hMC, hDec, TrigTauFlags::TAU12,  TrigTauFlags::E10,version, cutLevel,signal, passedSingle[3]);
    checkTauE(analyzer, event, leadingTauPt, leadingEPt,taueOffset+1, hMC, hDec, TrigTauFlags::TAU16I,  TrigTauFlags::E10,version, cutLevel,signal, passedSingle[4]);
    checkTauE(analyzer, event, leadingTauPt, leadingEPt,taueOffset+2, hMC, hDec, TrigTauFlags::TAU16I,  TrigTauFlags::E15I,version, cutLevel,signal, passedSingle[4]);
    checkTauE(analyzer, event, leadingTauPt, leadingEPt,taueOffset+3, hMC, hDec, TrigTauFlags::TAU20I,  TrigTauFlags::E10,version, cutLevel,signal, passedSingle[5]);
    checkTauE(analyzer, event, leadingTauPt, leadingEPt,taueOffset+4, hMC, hDec, TrigTauFlags::TAU20I,  TrigTauFlags::E15I,version, cutLevel,signal, passedSingle[5]);
    checkTauE(analyzer, event, leadingTauPt, leadingEPt,taueOffset+5, hMC, hDec, TrigTauFlags::TAU20I,  TrigTauFlags::E25I,version, cutLevel,signal, passedSingle[5]);
    }
    if( doTauMu ) {
    checkTauMu(analyzer, event, leadingTauPt, leadingMuPt, taumuOffset+0, hMC, hDec, TrigTauFlags::TAU16I,  TrigTauFlags::MU6, version, cutLevel,signal, passedSingle[4]);
    checkTauMu(analyzer, event, leadingTauPt, leadingMuPt, taumuOffset+1, hMC, hDec, TrigTauFlags::TAU16I,  TrigTauFlags::MU10, version, cutLevel,signal, passedSingle[4]);
    checkTauMu(analyzer, event, leadingTauPt, leadingMuPt, taumuOffset+2, hMC, hDec, TrigTauFlags::TAU20I,  TrigTauFlags::MU6, version, cutLevel,signal, passedSingle[5]);
    checkTauMu(analyzer, event, leadingTauPt, leadingMuPt, taumuOffset+3, hMC, hDec, TrigTauFlags::TAU20I,  TrigTauFlags::MU10, version, cutLevel,signal, passedSingle[5]);
    checkTauMu(analyzer, event, leadingTauPt, leadingMuPt, taumuOffset+4, hMC, hDec, TrigTauFlags::TAU20I,  TrigTauFlags::MU20, version, cutLevel,signal, passedSingle[5]);
    }

//     // tau+jet
    if( doTauJet ) {
    checkTauJet(analyzer, event, leadingTauPt, jetPt, tauJetOffset+0, hMC, hDec,  TrigTauFlags::TAU16I,  TrigTauFlags::J70, version, cutLevel,signal, passedSingle[4]);
    checkTauJet(analyzer, event, leadingTauPt, jetPt, tauJetOffset+1, hMC, hDec,  TrigTauFlags::TAU16I,  TrigTauFlags::THREEJ18, version, cutLevel,signal, passedSingle[4]); // 23 in reality
    checkTauJet(analyzer, event, leadingTauPt, jetPt, tauJetOffset+2, hMC, hDec,  TrigTauFlags::TAU16I,  TrigTauFlags::FOURJ23, version, cutLevel,signal, passedSingle[4]);
    checkTauJet(analyzer, event, leadingTauPt, jetPt, tauJetOffset+3, hMC, hDec,  TrigTauFlags::TAU20I,  TrigTauFlags::FOURJ50, version, cutLevel,signal, passedSingle[5]);

    checkTauJetEFMET(analyzer, event, leadingTauPt, jetPt, tauJetOffset+4, hMC, hDec,  TrigTauFlags::TAU16I,  TrigTauFlags::J18, TrigTauFlags::MET30, version, cutLevel,signal, passedSingle[4]);
    checkTauJetEFMET(analyzer, event, leadingTauPt, jetPt, tauJetOffset+5, hMC, hDec,  TrigTauFlags::TAU16I,  TrigTauFlags::J35, TrigTauFlags::MET30, version, cutLevel,signal, passedSingle[4]);
    

    }
    
  }  // end loop over events

  delete event;
  gROOT->cd();  
  TH1F * hEff = (TH1F*)gROOT->Get("hEff");
  if( hEff) delete hEff;
  hEff = (TH1F*)hDec->Clone("hEff"); hEff->SetTitle(" Trigger efficiency for good MC events");
  for(unsigned int itau = 1 ; itau < nDec+1 ; ++itau)
    {
      float eff = 0; float errEff = 0;
      if( hMC->GetBinContent(itau) > 0 ) 
	{ 
	  eff  = float(hDec->GetBinContent(itau))/float( hMC->GetBinContent(itau));
	  errEff = sqrt(eff*(1.-eff)/float( hMC->GetBinContent(itau)));
	  //eff *= 100.;
	  //errEff *= 100.;
	  hEff->SetBinContent(itau, eff);
	  hEff->SetBinError(itau, errEff);
	}
    }
  
  // ------------------- print ----------------------------------------
  if(id == 5188 && signal == 1)
    cout << "$Z\\to\\tau\\tau$\n";
  else if(id == 5200 && signal == 1)
    cout << "$t\\bar{t}$ \n";
  else if(id == 5402 && signal == 1)
    cout << "SU2 \n";
  else if(id == 5403 && signal == 1)
    cout << "SU3 \n";
  else if(id == 5107 && signal == 1)
    cout << "$W\\to\\tau\\nu$ \n";

  cout << " Select " << nEvent << " interesting events out of " << nev << " total \n";
  
  cout << " Item \& nMC \& trigger efficiency \& Nevents/100 \\ipb \\\\ \n";
  cout << " \\hline \n";
  cout.setf(ios_base::fixed,ios_base::floatfield);
  for(unsigned int itau = 1 ; itau < nDec+1 ; ++itau)
    {
      if( TString(hDec->GetXaxis()->GetBinLabel(itau)).Length() == 0 ) 
	{ cout << "\\hline \n"; continue;}
      cout.setf(ios_base::left,ios_base::adjustfield );
      cout << "\\sig\{" <<  std::setw(15)<<hDec->GetXaxis()->GetBinLabel(itau) << "} \& " ;

      // print number of good MC events
      cout << std::setw(7) << int(hMC->GetBinContent(itau)) << " \& ";      
      
      // print trigger efficiency
      cout.setf(ios_base::internal,ios_base::adjustfield );
      //if( hEff->GetBinError(itau) > 0.5 )
      //cout << "$" << std::setw(5)<< std::setprecision(0) << hEff->GetBinContent(itau) << " \\pm "
      //     <<  std::setw(3)<< std::setprecision(0)<< hEff->GetBinError(itau) <<  "$" ;
      //else
      int precision = 8;
	cout << "$" << std::setw(5)<< std::setprecision(precision) << hEff->GetBinContent(itau) << " \\pm "
	     <<  std::setw(3)<< std::setprecision(precision)<< hEff->GetBinError(itau) <<  "$" ;

	//  print number of events in 100 pb^-1
	float n100pb = getRate( id, hEff->GetBinContent(itau)/100.* hMC->GetBinContent(itau)/nev, filter);

       if( n100pb > 100 )       cout << " \& " <<  std::setw(6) <<int(n100pb)  ;
       else                    cout << " \& " <<  std::setw(6) <<std::setprecision(2)<< n100pb  ;
      
       
      cout << " \\\\ \n";
      //if( itau == 2 || itau == singleOffset+nTauSingle || 
      //  itau == tautauOffset+nTauTau || itau==taumetOffset+nTauMet || 
      //  itau== taueOffset+nTauE || itau==taumuOffset+nTauMu || itau==tauJetOffset+nTauJet 
      //  ) cout << " \\hline \n";
    }

  // --------------------- plot ----------------------------
  gStyle->SetPadBottomMargin(0.4);

  hEff->LabelsOption("v");
  hEff->Draw();

  return ;

}

void checkTauMET(TrigTauAnalyzer& analyzer, 
		 TrigTauEvent * event, float leadingTauPt,
		 int ibin, TH1F * hMC, TH1F * hDec,
		 TrigTauFlags::TRIGGERMENU tauItem,
		 TrigTauFlags::TRIGGERMENU metItem,
		 TrigTauSelectionManager::MENUVERSION version,
		 TrigTauSelectionManager::CUTLEVEL cutLevel , bool signal,
		 bool passedTau)
{  
  bool mcGood = false;
  bool EFGood = false;
  passedMET(event, metItem, mcGood, EFGood);

  
  mcGood &= passedTauPt(leadingTauPt, tauItem);

  if( signal && !mcGood ) return;

  hMC->Fill(ibin); 
  if(  EFGood   &&
       //event->decision.passedEF( metItem) &&  
       event->decision.passedL1( metItem)&& passedTau  )
    hDec->Fill(ibin);
}

void checkTauEFMET(TrigTauAnalyzer& analyzer, 
		   TrigTauEvent * event, float leadingTauPt,
		 int ibin, TH1F * hMC, TH1F * hDec,
		 TrigTauFlags::TRIGGERMENU tauItem,
		 TrigTauFlags::TRIGGERMENU metItem,
		 TrigTauSelectionManager::MENUVERSION version,
		   TrigTauSelectionManager::CUTLEVEL cutLevel , bool signal, bool passedTau )
{  
  bool mcGood = false;
  bool EFGood = false;
  passedMET(event, metItem, mcGood, EFGood);
  
  mcGood &= passedTauPt(leadingTauPt, tauItem);

  if( signal && !mcGood ) return;


  hMC->Fill(ibin); 
  if(   EFGood   && passedTau )
    hDec->Fill(ibin);

}


void checkTauMu(TrigTauAnalyzer& analyzer, 
		TrigTauEvent * event, float leadingTauPt, float muonPt,
		 int ibin, TH1F * hMC, TH1F * hDec,
		 TrigTauFlags::TRIGGERMENU tauItem,
		 TrigTauFlags::TRIGGERMENU muItem,
		 TrigTauSelectionManager::MENUVERSION version,
		TrigTauSelectionManager::CUTLEVEL cutLevel, bool signal , bool passedTau)
{  
  bool mcGood = false;
  switch ( muItem )
    {
    case TrigTauFlags::MU4  :     mcGood = muonPt>4000 ; break;
    case TrigTauFlags::MU6  :     mcGood = muonPt>6000 ; break;
    case TrigTauFlags::MU6L :     mcGood = muonPt>6000 ; break;
    case TrigTauFlags::MU8  :     mcGood = muonPt>8000 ; break;
    case TrigTauFlags::MU10 :     mcGood = muonPt>10000 ; break;
    case TrigTauFlags::MU15 :     mcGood = muonPt>15000 ; break;
    case TrigTauFlags::MU20 :     mcGood = muonPt>20000 ; break;
    case TrigTauFlags::TWOMU6 :   mcGood = muonPt>6000 ; break;
    case TrigTauFlags::TWOMU20 :  mcGood = muonPt>20000 ; break;
    case TrigTauFlags::THREEMU6 : mcGood = muonPt>6000 ; break;
    default :
      cout << " ERROR checkTauMu : unknown menu " << TrigTauFlags::menuName(muItem) << endl;
    }
  

  mcGood &= passedTauPt(leadingTauPt, tauItem);

  if( signal && !mcGood ) return;


  hMC->Fill(ibin); 

  // look now if there is other RoI of proper type
  unsigned int nOther = event->nOtherTrigRoI();
  bool noCandidate = true;
  for( unsigned int iMuon = 0; noCandidate && iMuon < nOther; ++iMuon)
    {
      TrigTauRoI * muon = &event->otherTrigRoIEntry(iMuon);
      if( !muon->m_tags.passedEF(muItem) ) continue;
      noCandidate = false;
    }
  if( noCandidate ) return;
  // now taus
  if( !passedTau) return;
  
  hDec->Fill(ibin);
  
}




void checkTauE(TrigTauAnalyzer& analyzer, 
	       TrigTauEvent * event, float leadingTauPt, float elecPt,
	       int ibin, TH1F * hMC, TH1F * hDec,
	       TrigTauFlags::TRIGGERMENU tauItem,
	       TrigTauFlags::TRIGGERMENU eItem,
	       TrigTauSelectionManager::MENUVERSION version,
	       TrigTauSelectionManager::CUTLEVEL cutLevel, bool signal, bool passedTau )
{  
  bool mcGood = false;
  switch ( eItem )
    {
    case TrigTauFlags::E10   :   mcGood = elecPt>10000 ; break;
    case TrigTauFlags::E15   :   mcGood = elecPt>15000 ; break;
    case TrigTauFlags::E15I  :   mcGood = elecPt>15000 ; break;
    case TrigTauFlags::E20   :   mcGood = elecPt>20000 ; break;
    case TrigTauFlags::E20I  :   mcGood = elecPt>20000 ; break;
    case TrigTauFlags::E25   :   mcGood = elecPt>25000 ; break;
    case TrigTauFlags::E25I  :   mcGood = elecPt>25000 ; break;
    case TrigTauFlags::E60   :   mcGood = elecPt>60000 ; break;
    default :
      cout << " ERROR checkTauMu : unknown menu " << TrigTauFlags::menuName(muItem) << endl;
    }
  
  mcGood &= passedTauPt(leadingTauPt, tauItem);
  if(signal && !mcGood ) return;

  hMC->Fill(ibin); 

  if( !passedTau )// ||  !event->decision.passedEF( eItem) )
    return;

  // now check for overlap
  TrigTauSelectionManager trigMan;
  trigMan.setMenuVersion(version);
  trigMan.setTauMenu(tauItem,cutLevel);
  
  bool noCandidate = true;

  unsigned int numCh = analyzer.chainList.nTrigChains();
  unsigned int nOther = event->nOtherTrigRoI();
  for (unsigned int iCh1=0; noCandidate && iCh1 < numCh; iCh1++) {
    TrigTauTrigChain ch1 = analyzer.chainList.trigChainEntry(iCh1);

    if (!trigMan.passedEF(ch1)) continue;
    
    for( unsigned int iElec = 0; noCandidate && iElec < nOther; ++iElec)
      {
	TrigTauRoI * elec = &event->otherTrigRoIEntry(iElec);
	if( !elec->m_tags.passedEF(eItem) ) continue;

	if( elec->DeltaR(*ch1.EF) > 0.1 )
	  noCandidate = false; // FOUND!
      }

  }

  if( noCandidate == false ) hDec->Fill(ibin);

}


void checkTauTau(TrigTauAnalyzer& analyzer, 
		 TrigTauEvent * event, float leadingTauPt, float nextTauPt,
		 int ibin, TH1F * hMC, TH1F * hDec,
		 TrigTauFlags::TRIGGERMENU tauLowPtItem,
		 TrigTauFlags::TRIGGERMENU tauHighPtItem,
		 TrigTauSelectionManager::MENUVERSION version,
		 TrigTauSelectionManager::CUTLEVEL cutLevel, bool signal, bool passedLow, bool passedHigh)
{  
  bool mcGood = passedTauPt(leadingTauPt, tauHighPtItem) &  passedTauPt(nextTauPt, tauLowPtItem);
  if( signal && !mcGood ) return;

  hMC->Fill(ibin); 


  if( !passedHigh ) // ||  !event->decision.passedEF( eItem) )
    return;

  // now check for overlap
  TrigTauSelectionManager trigManLow;
  trigManLow.setMenuVersion(version);
  trigManLow.setTauMenu(tauLowPtItem,cutLevel);

  TrigTauSelectionManager trigManHigh;
  trigManHigh.setMenuVersion(version);
  trigManHigh.setTauMenu(tauHighPtItem,cutLevel);
  
  bool noCandidate = true;

  unsigned int numCh = analyzer.chainList.nTrigChains();
  unsigned int nOther = event->nOtherTrigRoI();
  for (unsigned int iCh1=0; noCandidate && iCh1 < numCh; iCh1++) {
    TrigTauTrigChain ch1 = analyzer.chainList.trigChainEntry(iCh1);
    
    if (!trigManLow.passedEF(ch1)) continue;
    
    for (unsigned int iCh2=iCh1+1; noCandidate && iCh2 < numCh; iCh2++) {
      TrigTauTrigChain ch2 = analyzer.chainList.trigChainEntry(iCh2);
      if (!trigManHigh.passedEF(ch2)) continue;
      noCandidate = false;
    }
  }

  if( noCandidate == false ) hDec->Fill(ibin);

}

bool passedMET(TrigTauEvent * event, TrigTauFlags::TRIGGERMENU metItem, bool & mcGood, bool & EFGood )
{  
   mcGood = false;
  float trueMET = event->mcEvent.etMissTrue.m_EtMiss;
  float offlineMET = event->offlineEvent.etMissRecFinal.m_EtMiss;
  float trigMET = event->etMissEF.m_EtMiss;

   EFGood = false;
  switch ( metItem )
    {
    case TrigTauFlags::MET15 :   mcGood = (trueMET > 15000 && offlineMET > 15000 ); 
      EFGood = trigMET>15000;  break;
    case TrigTauFlags::MET20 :   mcGood = (trueMET > 20000 && offlineMET > 20000 );  
      EFGood = trigMET>20000;  break;
    case TrigTauFlags::MET25 :   mcGood = (trueMET > 25000 && offlineMET > 25000 ); 
      EFGood = trigMET>25000;   break;
    case TrigTauFlags::MET30 :   mcGood = (trueMET > 30000 && offlineMET > 30000 );  
      EFGood = trigMET>30000;  break;
    case TrigTauFlags::MET40 :   mcGood = (trueMET > 40000 && offlineMET > 40000 ); 
      EFGood = trigMET>40000;   break;
    case TrigTauFlags::MET50 :   mcGood = (trueMET > 50000 && offlineMET > 50000 ); 
      EFGood = trigMET>50000;   break;
    case TrigTauFlags::MET70 :   mcGood = (trueMET > 70000 && offlineMET > 70000 ); 
      EFGood = trigMET>70000;   break;
    case TrigTauFlags::MET80 :   mcGood = (trueMET > 80000 && offlineMET > 80000 ); 
      EFGood = trigMET>80000;   break;
    default :
      cout << " ERROR checkTauMET : unknown menu " << TrigTauFlags::menuName(metItem) << endl;
    }
  return EFGood;
}


bool passedTauPt(float leadingTauPt, TrigTauFlags::TRIGGERMENU tauItem)
{
  leadingTauPt= TrigTauMCTauList::nominalMinVisPt(tauItem)*1000.;
  return true;

}

float getRate(OTRIGTAU::OSampleTypes type, float eff, bool filter)
{

  float scaleFactor = 1; 
  if (filter)
    if( type ==  OTRIGTAU::J0 ) scaleFactor=0.08781; // for TTP06 only!
    else if( type ==  OTRIGTAU::J1 ) scaleFactor=0.4443; // for TTP06 only!
    else if( type ==  5001         ) scaleFactor=0.034947;

  switch (type)
    {
    case 5001         :   return eff* 670000*scaleFactor; // 0.67e+6 // filtere
    case OTRIGTAU::J0 :   return eff* 174000*scaleFactor;
    case OTRIGTAU::J1 :   return eff* 13800*scaleFactor;
    case OTRIGTAU::J2 :   return eff* 933;
    case OTRIGTAU::J3 :   return eff* 58.8;
    case 5013         :   return eff* 3.08;
    case 5014         :   return eff* 0.125;
    case 5015         :   return eff* 0.00360;

    case 5107         : return eff * 17.3*0.32*100000.; // 17.3 nb  for 100 pb^-1
    case 5188         : return eff * 1.64*0.15*100000.;  //  nb  for 100 pb^-1
    case 5189         : return eff * 1.58*0.29*100000.;  //  nb  for 100 pb^-1
    case 5116         : return eff * 1.58*0.29*100000.;  //  nb  for 100 pb^-1
    case 5200         : return eff *  854*0.54*100    ;  //  pb  for 100 pb^-1

    case 5401         : return eff *7.43 *100    ;  //  pb  for 100 pb^-1
    case 5402         : return eff *4.86 *100    ;  //  pb  for 100 pb^-1
    case 5403         : return eff *18.59*100    ;  //  pb  for 100 pb^-1
    case 6400         : return eff *262  *100    ;  //  pb  for 100 pb^-1
    case 5404         : return eff *4.48 *100    ;  //  pb  for 100 pb^-1
    case 5862         : return eff *63.3 *0.1    ;    //  fb  for 100 pb^-1  Wolfgang asnwers via tauWG HN
    case 5334         : return eff *0.145 *100    ;    //  pb  for 100 pb^-1 https://twiki.cern.ch/twiki/bin/view/Atlas/VBFHTauTau
    case 5338         : return eff *0.073 *100    ;    //  pb  for 100 pb^-1 https://twiki.cern.ch/twiki/bin/view/Atlas/VBFHTauTau

      //  case 5338         : return eff *  212*0.237*0.1   ;  //  fb  for 100 pb^-1


    default :
      std::cout << "Do not know how to estimate rate for process " << type << std::endl;
      return 1;
    }
  return 0;
}

float getRate(float effJ0, float effJ1, float effJ2, float effJ3, bool filter)
{
  float rate = getRate(OTRIGTAU::J0 , effJ0,filter) +  getRate(OTRIGTAU::J1 , effJ1,filter)
    +          getRate(OTRIGTAU::J2 , effJ2,filter) +  getRate(OTRIGTAU::J3 , effJ3,filter);
  return rate;
}

void checkTauJet(TrigTauAnalyzer& analyzer, 
		 TrigTauEvent * event, float leadingTauPt, float jetPt[10],
		 int ibin, TH1F * hMC, TH1F * hDec,
		 TrigTauFlags::TRIGGERMENU tauItem,
		 TrigTauFlags::TRIGGERMENU jetItem,
		 TrigTauSelectionManager::MENUVERSION version,
		 TrigTauSelectionManager::CUTLEVEL cutLevel, bool signal, bool passedTau)
{  
  bool mcGood = false;
  int nJet = 0;
  float minJet = 0;
  switch ( jetItem )
    {
    case TrigTauFlags::J35       :   mcGood = jetPt[0]>35000 ; 
      nJet = 1; minJet=35000; break;
    case TrigTauFlags::J70       :   mcGood = jetPt[0]>70000 ; 
      nJet = 1; minJet=70000; break;
    case TrigTauFlags::THREEJ18  :   mcGood = jetPt[2]>23000 ; 
      nJet = 3; minJet=23000; break;
    case TrigTauFlags::FOURJ23   :   mcGood = jetPt[3]>23000 ; 
      nJet = 4; minJet=23000; break;
    case TrigTauFlags::FOURJ50   :   mcGood = jetPt[3]>50000 ;  
      nJet = 4; minJet=50000;break;
    default :
      cout << " ERROR checkTauMu : unknown menu " << TrigTauFlags::menuName(muItem) << endl;
    }
   
  if( signal && !mcGood ) return;
  mcGood = passedTauPt(leadingTauPt, tauItem);
  if( signal && !mcGood ) return;

  hMC->Fill(ibin); 

  if( !passedTau )// ||  !event->decision.passedEF( eItem) )
    return;

  // now check for overlap
  TrigTauSelectionManager trigMan;
  trigMan.setMenuVersion(version);
  trigMan.setTauMenu(tauItem,cutLevel);
  
  int nCandidate = 0;

  bool found = 0;

  unsigned int numCh = analyzer.chainList.nTrigChains();
  unsigned int nOther = event->nOtherTrigRoI();
  for (unsigned int iCh1=0; !found&& iCh1 < numCh; iCh1++) {
    TrigTauTrigChain ch1 = analyzer.chainList.trigChainEntry(iCh1);

    if (!trigMan.passedEF(ch1)) continue;
    
    for( unsigned int iJet = 0; iJet < nOther; ++iJet)
      {
	TrigTauRoI * jet = &event->otherTrigRoIEntry(iJet);
	if( !jet->isJet() ) continue;
	if( jet->Pt() < minJet ) continue;
	if( jet->DeltaR(*ch1.EF) > 0.2 )
	  nCandidate++;
      }

    if( nCandidate >= nJet ) found = true;
  }

  if( found ) hDec->Fill(ibin);

}

void checkTauJetEFMET(TrigTauAnalyzer& analyzer, 
		   TrigTauEvent * event, float leadingTauPt, float jetPt[10],
		      int ibin, TH1F * hMC, TH1F * hDec,
		      TrigTauFlags::TRIGGERMENU tauItem,
		      TrigTauFlags::TRIGGERMENU jetItem,
		      TrigTauFlags::TRIGGERMENU metItem,
		      TrigTauSelectionManager::MENUVERSION version,
		      TrigTauSelectionManager::CUTLEVEL cutLevel , bool signal, bool passedTau )
{  
  bool mcGood = false;
  bool EFGood = false;
  passedMET(event, metItem, mcGood, EFGood);
  
  mcGood &= passedTauPt(leadingTauPt, tauItem);
  if( signal & !mcGood) return ;
  int nJet = 0;
  float minJet = 0;
  switch ( jetItem )
    {
    case TrigTauFlags::J18       :   mcGood = jetPt[0]>18000 ; 
      nJet = 1; minJet=18000; break;
    case TrigTauFlags::J23       :   mcGood = jetPt[0]>23000 ; 
      nJet = 1; minJet=23000; break;
    case TrigTauFlags::J35       :   mcGood = jetPt[0]>35000 ; 
      nJet = 1; minJet=35000; break;
    case TrigTauFlags::J70       :   mcGood = jetPt[0]>70000 ; 
      nJet = 1; minJet=70000; break;
    case TrigTauFlags::J100       :   mcGood = jetPt[0]>100000 ; 
      nJet = 1; minJet=100000; break;
    default :
      cout << " ERROR checkTauMetJet : unknown menu " << TrigTauFlags::menuName(jetItem) << endl;
    }
  


  if( signal && !mcGood ) return;


  hMC->Fill(ibin); 
  if(   !EFGood   || ! passedTau ) return;
  
  TrigTauSelectionManager trigMan;
  trigMan.setMenuVersion(version);
  trigMan.setTauMenu(tauItem,cutLevel);
  
  int nCandidate = 0;

  bool found = 0;

  unsigned int numCh = analyzer.chainList.nTrigChains();
  unsigned int nOther = event->nOtherTrigRoI();
  for (unsigned int iCh1=0; !found&& iCh1 < numCh; iCh1++) {
    TrigTauTrigChain ch1 = analyzer.chainList.trigChainEntry(iCh1);

    if (!trigMan.passedEF(ch1)) continue;
    
    for( unsigned int iJet = 0; iJet < nOther; ++iJet)
      {
	TrigTauRoI * jet = &event->otherTrigRoIEntry(iJet);
	if( !jet->isJet() ) continue;
	if( jet->Pt() < minJet ) continue;
	if( jet->DeltaR(*ch1.EF) > 0.2 )
	  nCandidate++;
      }

    if( nCandidate >= nJet ) found = true;
  }

  if( found )    hDec->Fill(ibin);

}




void mysort(float jetPt[10]){
    // sort
    for (unsigned int x=0; x < 9; x++)
      for (unsigned int y=0; y < 9-x; y++)
        if (jetPt[y] < jetPt[y+1])
	  {
            float t=jetPt[y];
            jetPt[y]=jetPt[y+1];
            jetPt[y+1]=t;
	  }
}


