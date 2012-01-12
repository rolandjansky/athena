/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void validation3(const char * filename, int type = 2, bool debug = false ) {
 
  TChain * chain = new TChain("eventTree");
   chain->Add(filename);
   //chain->Add("ntuples/14.2.0_May28/5107/tmp.*");
   //chain->Add("ntuples/14.2.0/5107/tmp.*");
   //chain->Add("ntuples/TTP09prelim/tmp.*");
   validation3(chain, type, debug);

}


void validation3(TTree * tree, int type = 2, bool debug = false)
{
  gStyle->SetPadGridX(true);
  gStyle->SetPadGridY(true);
  TrigTauFlags::TRIGGERMENU menu1 = TrigTauFlags::TAU16I;
  TrigTauSelectionManager::CUTLEVEL cutLevel = TrigTauSelectionManager::LOOSE;
  bool signal = true;

  int nprong        = 3;
  // TAU1P3P, TAUREC, EITHERTAU, BOTHTAU, NONE
  TrigTauMCTauList::OFFLINESELECTION offlineType =  TrigTauMCTauList::NONE;

  TrigTauBugFix bugfix(TrigTauBugFix::TTP7, TrigTauBugFix::RDO);

  //===================================================
  TrigTauAnalyzer analyzer;
  // MC truth
  analyzer.truthList.setMaxVisEta(2.5);
  analyzer.truthList.setMinVisPt(menu1); // in GeV
  analyzer.truthList.setMinLeadingPt(6.); // pt of the leading pt of track is 
  if( nprong == 1       )  analyzer.truthList.select1Prong();
  else  if( nprong == 3 )  analyzer.truthList.select3Prong();
  analyzer.truthList.setRecoMatchdR(0.2);


  TrigTauEvent* event = new TrigTauEvent();
  tree->SetBranchAddress("EventBranch", &event);
  tree->SetBranchStatus("*", 1);
  //tree->SetBranchStatus("mcEvent*", 0);

  // Also set an L1 MET cut if desired
  analyzer.trigMan.setL1MET(0.);
  analyzer.trigMan.setMenuVersion(TrigTauSelectionManager::MENU1400 );
  analyzer.trigMan.setTauMenu(menu1, cutLevel);
  analyzer.chainList.setRoiWordMatching(false);

  TH1F * hTot[3];
  hTot[0] = new TH1F("hptTot","",120,0,120000);
  hTot[1] = new TH1F("hphiTot","",64,-3.2,3.2);
  hTot[2] = new TH1F("hetaTot","",50,-2.5,2.5);

  //create list of histograms
  vector<TString> hnames;

  //Level 1
  hnames.push_back("L1_TauCluster");
  hnames.push_back("L1_EMIsol");
  hnames.push_back("L1_HDIsol");
  hnames.push_back("L1_EMCore");
  hnames.push_back("L1_HDCore");
  hnames.push_back("L1_EMEnergy");
  //Level2 Calo
  hnames.push_back("L2Calo_Et");
  hnames.push_back("L2Calo_Et_Calib");
  hnames.push_back("L2Calo_nStrips");
  hnames.push_back("L2Calo_stripWidth");
  hnames.push_back("L2Calo_EMRadius");
  hnames.push_back("L2Calo_IsoFrac");
  hnames.push_back("L2Calo_EMWidth");
  hnames.push_back("L2Calo_Et_Nar");
  //Level2 Track
  hnames.push_back("L2Trk_nMinTrk");;
  hnames.push_back("L2Trk_nSlow");
  hnames.push_back("L2Trk_nIso");
  hnames.push_back("L2Trk_nMatch");
  hnames.push_back("L2Trk_totCharge");
  hnames.push_back("L2Trk_maxPt");
  hnames.push_back("L2Trk_sumPtRatio");
  hnames.push_back("L2Trk_Et (Trk+EM)");
  //EF
  hnames.push_back("EF_min nTrk");
  hnames.push_back("EF_max nTrk");
  hnames.push_back("EF_EMRadius");
  hnames.push_back("EF_IsoFrac");
  hnames.push_back("EF_EMFrac");
  hnames.push_back("EF_Pt_max");
  hnames.push_back("EF_Et");
  hnames.push_back("EF_EM_Et");
  hnames.push_back("EF_Mass");

  const unsigned int nL1Cuts = 6;
  const int nL2CaloCuts = 8;
  const int nL2TrkCuts = 8;
  const int nEFCuts = 9;
  const unsigned int nHistos = hnames.size()*3;
  const unsigned int nNames = hnames.size();

  if ( nL1Cuts+nL2CaloCuts+nL2TrkCuts+nEFCuts != nNames ) {
    cout << "ERROR: Number of Cuts for different levels incorrect!!!" << endl;
    return;
  }
  
  TH1F * histograms[nHistos];
  createHistograms(hnames, nNames, histograms);
  
  TrigTauSelectionManager selmanager;
  selmanager.setMenuVersion(TrigTauSelectionManager::MENU1400);
  selmanager.setTauMenu(menu1, TrigTauSelectionManager::LOOSE);
	unsigned long l1Word[100];
        unsigned long l2CaloWord[100];
        unsigned long l2TrkWord[100];
        unsigned long l2Word[100];
        unsigned long efWord[100];

  
  // Loop over all events
  int nev = int(tree->GetEntries());
  //if ( nev > 100 ) nev = 100;
  for(int iev = 0; iev<nev; ++iev) {
    if ( debug ) cout << "============== Checking event " << iev << " ==================" <<  endl;
    event->reset();
    tree->GetEvent(iev);
    bugfix.fix(event);

    analyzer.analyze(event, signal, offlineType);

    // ============ Select type of events (e.g. signal) ================
    if( signal ) {
      analyzer.truthList.fillPureList(&event->mcEvent);
      analyzer.truthList.fillGoodList();
      analyzer.truthList.requireRecoMatch(*event, offlineType);
      if ( debug && analyzer.truthList.nMCTauGood() ==0 ) cout << "Event contains no good taus! " << endl;
      if(  analyzer.truthList.nMCTauGood() ==0 ) continue;
    }
  
    // loop over taus after fiducial cuts

    if ( debug ) cout << "Event contains " << analyzer.truthList.nMCTauGood() << " good tau!" << endl;
    for( unsigned int iTrue = 0; iTrue <  analyzer.truthList.nMCTauGood(); ++iTrue )  //loop over all true taus
      {
	const TrigTauMCTau * truth = & analyzer.truthList.mcTauGoodEntry(iTrue);    //select tau
	float pt = truth->Pt();
	float eta = truth->Eta();
	float phi = truth->Phi();
	hTot[0]->Fill(pt); hTot[1]->Fill(phi);  hTot[2]->Fill(eta);

        const unsigned int nTrigChains = analyzer.chainList.nTrigChains() ;
	if (debug ) cout << "Value of nTrigChains: " << nTrigChains << endl;
	if ( nTrigChains == 0 ) continue;

        unsigned int nl1Word = 0;
        //unsigned int nl2CaloWord = 0;
        //unsigned int nl2TrkWord = 0;
        unsigned int nl2Word = 0;
        unsigned int nefWord = 0;
        
	if ( debug ) cout << " There are " << nTrigChains << " chains to analyze." << endl;
        for( unsigned int ichain = 0; ichain < nTrigChains; ++ichain) {
           if ( debug ) cout << "Starting chain " << ichain << "!" << endl;
           TrigTauTrigChain *ch = &analyzer.chainList.trigChainEntry(ichain);
 	   const TrigTauL1Object* l1object = ch->L1;
           const TrigTauL2Object* l2object = ch->L2;
	   const TrigTauL2Object* l2calobject = ch->L2Cal;
           const TrigTauL2Object* l2trkobject = ch->L2Trk;
           const TrigTauRecEFObject* efobject = ch->EF;

	   //check Level 1
	   if (l1object == 0 ) {
	     if ( debug ) cout << "L1 Object is empty; go to next chain" << endl;
	     continue;
	   }
	   if( truth->DeltaR(*l1object)<0.2 ) {
	     if ( debug ) cout << "nL1Word is " << nl1Word << endl;
	     unsigned int currentWord = selmanager.l1Sel.cutWord(*l1object);
	     l1Word[nl1Word]  = currentWord;
             if ( debug ) cout << "Found L1 matched chain for number " << nl1Word << " with word :" << l1Word[nl1Word] << endl;
	     nl1Word++;
	     if (currentWord != 0 ) continue;
           }
	   else continue;

           //check Level 2 (including Trk and Cal)
           if (l2object == 0 ) {
	     if ( debug ) cout << "L2 object is empty; go to next chain" << endl;
	     continue;
	   }
	   if ( debug ) cout << "     L2 DeltaR is :" << ( truth->DeltaR(*l2object) ) << endl;
           if( truth->DeltaR(*l2object)<0.2 ) {
		    l2CaloWord[nl2Word] = selmanager.l2CalSel.cutWord(*l2calobject);
		    if ( l2CaloWord[nl2Word] !=0 ) {
		      l2TrkWord[nl2Word] = 255;
		      nl2Word++;
		      continue;
		    }
		    l2TrkWord[nl2Word] = selmanager.l2TrkSel.cutWord(*l2trkobject);
                    if ( debug ) cout << "Found L2 matched chain with words :" << l2CaloWord[nl2Word] << " and " << l2TrkWord[nl2Word] << endl;
		    nl2Word++;
		    if ( (l2CaloWord[nl2Word-1] !=0 ) || (l2TrkWord[nl2Word-1] !=0 )  ) continue;
           }
	   else continue;


           //check EF
	   if (efobject == 0 ) {
             if ( debug ) cout << "EF object is empty; go to next chain" << endl;
             continue;
	   }
           if ( debug ) cout << "     EF DeltaR is :" << (truth->DeltaR(*efobject) ) << endl;
           if( truth->DeltaR(*efobject)<0.2 ) {
		    efWord[nefWord] = selmanager.efSel.cutWord(*efobject);
                    if ( debug ) cout << "Found EF matched chain with word :" << efWord[nefWord] << endl;
		    nefWord++;
		    if ( efWord[nefWord-1] == 0 && debug ) cout << "Tau passed trigger!!!" << endl;

           }
	   if ( ichain ==0 && debug ) cout << "Word No " << nTrigChains-1 << " is " << l1Word[nTrigChains-1] << endl;
        } // end chain loop


	bool fillHisto[nNames];
	for (int i=0; i!=nNames; ++i)
	  fillHisto[i] = false;
	
	for ( int i =0; i!=nl1Word; ++i) {
	  if ( debug ) cout << "The L1Word for chain " << i << " is " << l1Word[i] << endl;
	  for (int j=0; j!=nL1Cuts; ++j) {
	    if ( ((l1Word[i] >> j) & 0x01) ==0 ) fillHisto[j] = true;
	    else break;
	  } 
	}

	
	for ( int i =0; i!=nl2Word; ++i) {
	  //if ( debug ) cout << "The L2Word for chain " << i << " is " << hex << l2Word[i] << endl;
	  for (int j=0; j!=nL2CaloCuts; ++j) {
	    if ( ((l2CaloWord[i] >> j) & 0x01) ==0 ) fillHisto[j+nL1Cuts] = true;
	    else break;
	  }
	  for (int j=0; j!=nL2TrkCuts; ++j) {
	    if ( ((l2TrkWord[i] >> j) & 0x01) ==0 ) fillHisto[j+nL1Cuts+nL2CaloCuts] = true;
	    else break;
	  }
	}

	for ( int i =0; i!=nefWord; ++i) {
	  if ( debug ) cout << "The EFWord for chain " << i << " is " << efWord[i] << endl;
	  for (int j=0; j!=nEFCuts; ++j) {
	    if ( ((efWord[i] >> j) & 0x01) ==0 ) fillHisto[j+nL1Cuts+nL2CaloCuts+nL2TrkCuts] = true;
	    else break;
	  } 
	}

	if ( debug ) {
	    cout << "TauCluster found - " << fillHisto[0] << endl;
	    cout << "L1EMIsol found - " << fillHisto[1] << endl;
	    cout << "L1HDIsol found - " << fillHisto[2] << endl;
	}

	if ( debug ) cout << "First Histogram found; check if it will be filled." << endl;
       	for ( int j =0; j < nNames; ++j ) {
	  if ( fillHisto[j] ) {
	    //if ( debug ) cout << "Filling histogram..." <<endl;
	    TH1F * h =(TH1F*)histograms[3*j];
	    h->Fill(pt);
	    h = histograms[3*j+1];
	    h->Fill(phi);
	    h = histograms[3*j+2];
	    h-> Fill(eta);
	  }
	}
      } // end loop over truth

      
  }  // end loop over events

  TH1F * EffHistograms[nHistos];
  gROOT->cd();
  char word;
  switch ( type ) {

  case 1:
    createHistograms(hnames, nNames, EffHistograms);
    if ( debug ) cout << "Array for Histograms created..." << endl;
    for (int j = 0; j != nHistos ; ++j ) {
      if ( debug ) cout << "Pointer to histograms created" << endl;
      if (!(j%3)) TH1F * effHist =(TH1F*)getEffHist(hTot[0],histograms[j],hnames[j/3]+"_absolut_eff", debug);
      else if (!((j-1)%3)) TH1F * effHist =(TH1F*)getEffHist(hTot[1],histograms[j],hnames[(j-1)/3]+"_absolut_eff", debug);
      else if (!((j-2)%3)) TH1F * effHist =(TH1F*)getEffHist(hTot[2],histograms[j],hnames[(j-2)/3]+"_absolut_eff", debug);      
      if ( debug ) cout <<j << " Efficicency Histograms created... " << endl;
      EffHistograms[j] = effHist;
    }
    if (debug ) cout << "Efficiency Histograms filled." << endl;

    break;

  case 2 :
      
    createHistograms(hnames, nNames, EffHistograms);
    if ( debug ) cout << "Array for Histograms created..." << endl;
    for (int j = 0; j != nHistos ; ++j ) {
      if ( debug ) cout << "Pointer to histograms created" << endl;
      if ( j==0 || j==1 || j==2 ) TH1F * effHist = getEffHist(hTot[j],histograms[j],hnames[0]+"_rel_eff", debug);
      else TH1F * effHist =(TH1F*)getEffHist(histograms[j-3],histograms[j],hnames[0]+"_rel_eff", debug);
      if ( debug ) cout <<j << " Efficicency Histograms created... " << endl;
      EffHistograms[j] = effHist;
    }
    if ( debug ) cout << "Efficiency Histograms filled." << endl;

    break;
      
  };
  
  TCanvas * c1 = (TCanvas *) gROOT->Get("c1");
  if( c1 ) delete c1;
  c1 = new TCanvas("c1","",10,10,800,500);

  if ( debug ) cout << "Draw Efficiency Histograms..." << endl;
  for (int j = 0; j < nHistos ; ++j ) {
    TH1F * histo = (TH1F*)EffHistograms[j];

    if ( debug ) cout << "Draw Histogram No. " << j << endl;
    if ( histo != 0 ) histo->Draw();
    if ( debug ) cout << "Printing Histogram No " << j << endl;
    
    if ( j==0 ) c1->Print("ntuple.eps(");
    else if ( j+1==nHistos ) c1->Print("ntuple.eps)");
    else c1.Print("ntuple.eps");
    }

  return;
}






TH1F * getEffHist(TH1F * hTot, TH1F * hCut, const char * name, bool debug = false)
{
  if( hTot == 0 || hCut == 0 ) cout << "ERROR: Input to getEffHist is bad!!! " << endl;

  TH1F * hEff = (TH1F*)hTot->Clone(name);
  if ( debug ) cout << "Dividing now...." << endl;
  if (debug ) cout << "Dividing....done!" << endl;
  hEff->GetYaxis()->SetTitle("Efficiency");
  hEff->SetMaximum(1.1);
  hEff->SetMinimum(0);
  if ( debug ) cout << "Configuration of histogram....done" << endl;
  hEff->SetTitle(hCut->GetTitle());
  if ( debug ) cout << "Title set to " << hCut->GetTitle() << endl;

  int nbin = hCut->GetNbinsX();
  if ( debug ) cout << hCut->GetNbinsX() << " bins found..." << endl;
  for(unsigned int itau = 1 ; itau < nbin ; ++itau)
    {
      float eff = 0; float errEff = 0;
      if( hTot->GetBinContent(itau) > 0 )
        {
	  if (debug ) cout << "Bin content of hCut is " << hCut->GetBinContent(itau) << endl;
	  if ( debug ) cout << "Bin content of hTol is " << hTot->GetBinContent(itau) << endl;
          float eff  = float(hCut->GetBinContent(itau))/float( hTot->GetBinContent(itau));
          float errEff = sqrt(eff*(1.-eff)/float( hTot->GetBinContent(itau)));
	  if ( debug ) cout << "Reuting efficiency is " << eff << endl;
          //eff *= 100.;
          //errEff *= 100.;
          hEff->SetBinContent(itau, eff);
          hEff->SetBinError(itau, errEff);
        }
    }
  return hEff;
}

void createHistograms( vector<TString> names, unsigned int nNames, TH1F ** histoList) {
  
  for (int i = 0; i!=nNames; ++i ) {
    TString name(names[i]+"_pt");
    //cout << " b\n";
    histoList[3*i] = new TH1F(name, name, 120, 0, 120000);
    //cout << " c\n";
    TH1F * h1 = (TH1F*)histoList[3*i];
    //cout << " d\n";
    h1->GetXaxis()->SetTitle("pt");
    //cout << " e\n";
    
    TString name(names[i]+"_phi");
    //cout << " f "<< 3*i+1 << " " << nNames<<"\n";
    histoList[3*i+1] = new TH1F(name, name, 64, -3.2, 3.2);
    //cout << " g\n";
    TH1 * h2 = (TH1*)histoList[3*i+1];
    //cout << " h\n";
    h2->GetXaxis()->SetTitle("phi");
    //cout << " i\n";
    
    TString name(names[i]+"_eta");
    histoList[3*i+2] = new TH1F(name, name, 50, -2.5, 2.5);
    TH1 * h3 = (TH1*)histoList[3*i+2];
    h3->GetXaxis()->SetTitle("eta");
    //cout << " j\n";
    }
    
  return;
  
}

