/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <iomanip>
using namespace std;
#include "TrigTauPerformValidation/TTPValBackgroundStudy.h"



TTPValBackgroundStudy::TTPValBackgroundStudy ( bool invertL21, TrigTauBugFix::POOLFILE poolfile, TrigTauBugFix::TTPVERSION version, bool debug1) 
  : bugfix(version, poolfile)  { 
  cout << "WARNING: Filename not specified!! " << endl; 
  initialize(debug1, invertL21);
  fileDefined = false;
  return; 
}

TTPValBackgroundStudy::TTPValBackgroundStudy (TString filename1, bool invertL21, TrigTauBugFix::POOLFILE poolfile,  TrigTauBugFix::TTPVERSION version,  bool debug1) 
  : bugfix(version, poolfile)  {
    
  if (debug1) cout << " Creating validation4 object..." << endl; 
  filename = filename1;  
  chain = new TChain("eventTree");
  chain->Add(filename);
  initialize(debug1, invertL21);
  fileDefined = true;
  return;
}

void TTPValBackgroundStudy::initialize(bool debug1, bool invertL21) {


  type = 1;
  debug = debug1;
  signal = false;
  invertL2 = invertL21;
  nprong = 0; 
  maxVisEta = 2.5;
  minLeadingPt = 6.0;
  totalPassed =0;
  gStyle->SetPadGridX(true);
  gStyle->SetPadGridY(true);
  menu = TrigTauFlags::TAU16;
  cutLevel = TrigTauSelectionManager::LOOSE;
  offlineType =  TrigTauMCTauList::TAUCUTSAFEMEDIUM;
  menuVersion = TrigTauSelectionManager::MENU1560;
  chainType = TrigTauFlags::SITRK;

  hTot[0] = new TH1F("TTPValBackgroundStudy_hptTot","",50,0,120000);
  hTot[1] = new TH1F("TTPValBackgroundStudy_hphiTot","",64,-3.2,3.2);
  hTot[2] = new TH1F("TTPValBackgroundStudy_hetaTot","",50,-2.5,2.5);

  testHisto[0] = new TH2F("TTPValBackgroundStudy_EF Calo EtHad", "",64, -3.2, 3.2, 100, 0, 1.5);
  testHisto[0]->GetXaxis()->SetTitle("eta");
  testHisto[0]->GetYaxis()->SetTitle("EtHadCalib: EF/Offline");

  testHisto[1] = new TH2F("TTPValBackgroundStudy_EF Calo EtEM", "",64, -3.2, 3.2, 100, 0, 1.5);
  testHisto[1]->GetXaxis()->SetTitle("eta");
  testHisto[1]->GetYaxis()->SetTitle("EtEMCalib: EF/Offline");


  //Level 1
  hnames.push_back("L1_TauCluster");
  hnames.push_back("L1_EMIsol");
  hnames.push_back("L1_HDIsol");
  hnames.push_back("L1_EMCore");
  hnames.push_back("L1_HDCore");
  hnames.push_back("L1_EMEnergy");
  
  if (invertL2) {
    //Level2 Track
    hnames.push_back("L2Trk_nTrk");;
    hnames.push_back("L2Trk_nSlow");
    hnames.push_back("L2Trk_nIso");
    hnames.push_back("L2Trk_nMatch");
    hnames.push_back("L2Trk_totCharge");
    hnames.push_back("L2Trk_maxPt");
    hnames.push_back("L2Trk_sumPtRatio");
    hnames.push_back("L2Trk_Et (Trk+EM)");
    //Level2 Calo
    hnames.push_back("L2Calo_Et");
    hnames.push_back("L2Calo_Et_Calib");
    hnames.push_back("L2Calo_nStrips");
    hnames.push_back("L2Calo_stripWidth");
    hnames.push_back("L2Calo_EMRadius");
    hnames.push_back("L2Calo_IsoFrac");
    hnames.push_back("L2Calo_EMWidth");
    hnames.push_back("L2Calo_Et_Nar");
  }
  else {
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
    hnames.push_back("L2Trk_nTrk");;
    hnames.push_back("L2Trk_nSlow");
    hnames.push_back("L2Trk_nIso");
    hnames.push_back("L2Trk_nMatch");
    hnames.push_back("L2Trk_totCharge");
    hnames.push_back("L2Trk_maxPt");
    hnames.push_back("L2Trk_sumPtRatio");
    hnames.push_back("L2Trk_Et (Trk+EM)");
  }
  //EF
  hnames.push_back("EF_min nTrk_EFID");
  hnames.push_back("EF_min nTrk");
  hnames.push_back("EF_max nTrk");
  hnames.push_back("EF_EMRadius");
  hnames.push_back("EF_IsoFrac");
  hnames.push_back("EF_EMFrac");
  hnames.push_back("EF_Pt_max");
  hnames.push_back("EF_Et");
  hnames.push_back("EF_EM_Et");
  hnames.push_back("EF_Mass");

  nL1Cuts = 6;
  nL2CaloCuts = 8;
  nL2TrkCuts = 8;
  nEFCuts = 10;
  nHistos = hnames.size()*3;
  nNames = hnames.size();

  for ( unsigned int j =0; j < nNames; ++j )
    passedCounter[j] =0;


  //static const unsigned int nHistosConst = nHistos;
  //TH1F * histograms[nHistosConst];

  if ( nL1Cuts+nL2CaloCuts+nL2TrkCuts+nEFCuts != nNames || nNames*3 != nHistograms ) {
    cout << "ERROR: Number of Cuts for different levels incorrect!!!" << endl;
    return;
  }

  if (debug) cout << "Creating Histograms..." << endl;
  createHistograms(histograms);
  if (debug) cout << "Histograms created!   Building selmanager..." << endl;

}


void TTPValBackgroundStudy::setFile( TString filename1) {
  filename = filename1;
  chain = new TChain("eventTree"); //potential memory leak
  chain->Add(filename);
  fileDefined = true;
  return;
}


void TTPValBackgroundStudy::analyze() {
  
  if (!fileDefined) {
    cout << "File was not defined and variables not initialized!!" << endl;
    return;
  }

  selmanager.setMenuVersion(menuVersion);
  selmanager.setTauMenu(menu, cutLevel );

  // MC truth
  analyzer.truthList.setMaxVisEta(maxVisEta);
  analyzer.truthList.setMinVisPt(menu); // in GeV
  analyzer.truthList.setMinLeadingPt(minLeadingPt); // pt of the leading pt of track is 
  if( nprong == 1       )  analyzer.truthList.select1Prong();
  else  if( nprong == 3 )  analyzer.truthList.select3Prong();
  analyzer.truthList.setRecoMatchdR(0.2);

   event  = new TrigTauEvent();
  chain->SetBranchAddress("EventBranch", &event);
  chain->SetBranchStatus("*", 1);

  // Also set an L1 MET cut if desired
  analyzer.trigMan.setL1MET(0.);
  analyzer.trigMan.setMenuVersion(menuVersion);
  analyzer.trigMan.setTauMenu(menu, cutLevel);
  analyzer.chainList.setRoiWordMatching(false);
  analyzer.chainList.setHLTAuthor(chainType);

  testing = 0;

  nev = int(chain->GetEntries());
  //if ( nev > 100 ) nev = 100;
  for(int iev = 0; iev<nev; ++iev) {
    analyzeEvent( iev );
  }
  if (debug) cout << "L1 Had Energy accepted events: " << testing  <<endl;
  return;
}


void TTPValBackgroundStudy::analyzeEvent(int eventNumber) {

  event->reset();
  chain->GetEvent(eventNumber);
  bugfix.fix(event);

  analyzer.analyze(event, signal, offlineType);
 
    // ============ Select type of events (e.g. signal) ================
    if( signal ) {
     //  analyzer.truthList.fillPureList(&event->mcEvent);
//       analyzer.truthList.fillGoodList();
//       analyzer.truthList.requireRecoMatch(*event, offlineType);
       if ( debug && analyzer.truthList.nMCTauGood() ==0 ) cout << "Event contains no good taus! " << endl;
      if(  analyzer.truthList.nMCTauGood() ==0 ) return;
    }
  
    // loop over taus after fiducial cuts

    if ( debug ) cout << "Event contains " << analyzer.truthList.nMCTauGood() << " good tau!" << endl;
    
    unsigned int nTrigChains = analyzer.chainList.nTrigChains() ;
    if ( nTrigChains == 0 ) return;
    
    unsigned int nl1Word = 0;
    //unsigned int nl2CaloWord = 0;
    //unsigned int nl2TrkWord = 0;
    unsigned int nl2Word = 0;
    unsigned int nefWord = 0;
 
    if ( debug ) cout << " There are " << nTrigChains << " chains to analyze." << endl;
    for( unsigned int ichain = 0; ichain < nTrigChains; ++ichain) {
      if ( debug ) cout << "Starting chain " << ichain << "!" << endl;
      const TrigTauTrigChain *ch = &analyzer.chainList.trigChainEntry(ichain);
      const TrigTauL1Object* l1object = ch->L1;
      const TrigTauL2Object* l2object = ch->L2;
      const TrigTauL2Object* l2calobject = ch->L2Cal;
      const TrigTauL2Object* l2trkobject = ch->L2Trk;
      const TrigTauRecEFObject* efobject = ch->EF;
      const TrigTauRecOfflineObject* offlineobject = ch->tauRec;

      float pt = 0;
      float eta = 0;
      float phi = 0;

      if (offlineobject != 0) {
        pt = offlineobject->Pt();
        eta = offlineobject->Eta();
        phi = offlineobject->Phi(); 
      }

      bool stop = false; 
      
      //check Level 1
      if (l1object == 0 ) stop = true;
      if (!stop) {
        unsigned int currentWord = selmanager.l1Sel.cutWord(*l1object);
        l1Word[nl1Word]  = currentWord;
        nl1Word++;
        if (currentWord != 0 ) stop = true;
      }
        
      //check Level 2 (including Trk and Cal)
      if (invertL2) {
        if (l2object == 0 ) stop = true;
        if (!stop) {
          l2TrkWord[nl2Word] = selmanager.l2TrkSel.cutWord(*l2trkobject);
          if ( l2TrkWord[nl2Word] !=0 ) {
            l2CaloWord[nl2Word] = 255;
            stop = true;
          }

          if (!stop) l2CaloWord[nl2Word] = selmanager.l2CalSel.cutWord(*l2calobject);
          nl2Word++;
          if ( l2CaloWord[nl2Word-1] !=0 ) stop = true;
        }
      } 
      else {
        if (l2object == 0 ) stop = true;
        if (!stop) {
          l2CaloWord[nl2Word] = selmanager.l2CalSel.cutWord(*l2calobject);
          if ( l2CaloWord[nl2Word] !=0 ) {
	    l2TrkWord[nl2Word] = 255;
	    stop = true;
          }
      
          if (!stop) l2TrkWord[nl2Word] = selmanager.l2TrkSel.cutWord(*l2trkobject);
          nl2Word++;
          if ( l2TrkWord[nl2Word-1] !=0 ) stop = true;
        }      
      }
      
      //check EF
      if (efobject == 0 ) stop = true;
      if (!stop) {
        efWord[nefWord] = selmanager.efSel.cutWord(*efobject);
        nefWord++;
        if (offlineobject != 0 && offlineobject->m_etHadCalib != 0 ) testHisto[0]->Fill(efobject->Eta(),efobject->m_etHadCalib/offlineobject->m_etHadCalib);
        if (offlineobject != 0 && offlineobject->m_etEMCalib != 0 ) testHisto[1]->Fill(efobject->Eta(),efobject->m_etEMCalib/offlineobject->m_etEMCalib);
      }
     } 
      for (unsigned int i=0; i!=nNames; ++i)
	fillHisto[i] = false;
      
      for ( unsigned int i =0; i!=nl1Word; ++i) {
	for ( unsigned int j=0; j!=nL1Cuts; ++j) {
	  if ( ((l1Word[i] >> j) & 0x01) ==0 ) fillHisto[j] = true;
	  else break;
	} 
      }
      
      if (invertL2) { 
        for (unsigned int i =0; i!=nl2Word; ++i) {
          for (unsigned int j=0; j!=nL2TrkCuts; ++j) {
            if ( ((l2TrkWord[i] >> j) & 0x01) ==0 ) fillHisto[j+nL1Cuts+nL2CaloCuts] = true;
            else break;
          }
          for (unsigned int j=0; j!=nL2CaloCuts; ++j) {
            if ( ((l2CaloWord[i] >> j) & 0x01) ==0 ) fillHisto[j+nL1Cuts] = true;
            else break;
          }
        }
      }
      else {
        for (unsigned int i =0; i!=nl2Word; ++i) {
	  for (unsigned int j=0; j!=nL2CaloCuts; ++j) {
	    if ( ((l2CaloWord[i] >> j) & 0x01) ==0 ) fillHisto[j+nL1Cuts] = true;
	    else break;
	  }
	  for (unsigned int j=0; j!=nL2TrkCuts; ++j) {
	    if ( ((l2TrkWord[i] >> j) & 0x01) ==0 ) fillHisto[j+nL1Cuts+nL2CaloCuts] = true;
	    else break;
	  }
        }
      }      

      for ( unsigned int i =0; i!=nefWord; ++i) {
	for ( unsigned int j=0; j!=nEFCuts; ++j) {
	  if ( ((efWord[i] >> j) & 0x01) ==0 ) fillHisto[j+nL1Cuts+nL2CaloCuts+nL2TrkCuts] = true;
	  else break;
	} 
      }
      
      if ( debug ) {
	cout << "TauCluster found - " << fillHisto[0] << endl;
	cout << "L1EMIsol found - " << fillHisto[1] << endl;
	cout << "L1HDIsol found - " << fillHisto[2] << endl;
      }
      testing = testing+fillHisto[1]; 
      if ( debug ) cout << "First Histogram found; check if it will be filled." << endl;
      for ( unsigned int j =0; j < nNames; ++j ) {
	if ( fillHisto[j] ) {
	  //if ( debug ) cout << "Filling histogram..." <<endl;
	  TH1F * h =(TH1F*)histograms[3*j];
	  h->Fill(10);
	  h = histograms[3*j+1];
	  h->Fill(0);
	  h = histograms[3*j+2];
	  h-> Fill(0);
	}
    }
    for ( unsigned int j =0; j < nNames; ++j ) 
      if ( fillHisto[j] ) passedCounter[j]++;
    return;
}

void TTPValBackgroundStudy::print(float lum, float xsection) {
  
  print("ntuple.background.eps", lum, xsection);

}


void TTPValBackgroundStudy::print(TString name, float lum, float xsection) {

  createEffHistograms();

  TCanvas * c1 = (TCanvas *)gROOT->Get("c1");
  if( c1 ) delete c1;
  c1 = new TCanvas("c1","",10,10,800,500);

  if ( debug ) cout << "Draw Efficiency Histograms..." << endl;
  for (unsigned int j = 0; j < nHistos ; ++j ) {
    TH1F * histo = (TH1F*)effHistograms[j];

    if ( debug ) cout << "Draw Histogram No. " << j << endl;
    if ( histo != 0 ) histo->Draw();
    if ( debug ) cout << "Printing Histogram No " << j << endl;

    if ( j==0 ) c1->Print(TString(name)+"(");
    //else if ( j+1==nHistos ) c1->Print(TString(name)+")");
    else c1->Print(name);
    }

  TH2F * histo2 = (TH2F*)testHisto[0];
  if (histo2 !=0 ) histo2->Draw("BOX");
  c1->Print(TString(name));

  histo2 = 0;

  histo2 = (TH2F*)testHisto[1];
  if (histo2 !=0 ) histo2->Draw("BOX");

  c1->Print(TString(name)+")");

  cout << "Rates after various cuts: " << endl;

  for (unsigned int i = 0; i<hnames.size(); ++i) {
    cout << setiosflags(ios::left) << setw(20) << hnames[i] << ": " << lum*xsection*passedCounter[i]/nev << " Hz" << endl;
    //cout << "     PassedCounter of " << i << " is "<< passedCounter[i] << endl;
  }
  //cout << "Total Number of Events: " << nev << endl; 
  return;
}

void TTPValBackgroundStudy::save() {

  save("histo.background.root");

}


void TTPValBackgroundStudy::save(TString name) {

  createEffHistograms();

  TH1F * histo;
  TH2F * histo2;

  if (debug) cout << "Starting Save..." << endl;
  TFile f(name, "recreate");
  if (debug) cout << "File created..." << endl;
  for (unsigned int j = 0; j < nHistos ; ++j ) {
    histo = (TH1F*)effHistograms[j];
    if (debug) cout << "Writing histogram " << j << "...." << endl;
    histo->Write();
    histo->Draw();
  }
  //f->Write();

  histo2 = (TH2F*)testHisto[0];
  histo2->Write();
  histo2->Draw("BOX");

  histo2 = (TH2F*)testHisto[1];
  histo2->Write();
  histo2->Draw("BOX");

  if (debug) cout << "Histograms written..." << endl;
  f.Close();
  return;
  
}

void TTPValBackgroundStudy::compare(TString reference) {

  compare(reference, "", "ntuple.eps");

}

void TTPValBackgroundStudy::compare(TString reference, TString saveFile) {

  compare(reference, "", saveFile);

}


void TTPValBackgroundStudy::compare(TString reference1, TString reference2, TString saveFile) {

  createEffHistograms();

  TCanvas * c2 = (TCanvas *)gROOT->Get("c2");
  if( c2 ) delete c2;
  c2 = new TCanvas("c1","",10,10,800,500);

  gStyle->SetPadGridX(true);
  gStyle->SetPadGridY(true);

  if (debug) cout << "Opening file..." << endl;

  TFile * file1 = new TFile(reference1);
  TFile * file2 = 0;
  if (reference2[0] != '\0') file2 = new TFile(reference2);

  for (unsigned int j=0; j!= nHistos; ++j) {

    if (debug) cout << "Processing Histogram " << j << endl;

    TH1* h1 = (TH1*)gROOT->Get("h1");
    if( h1 ) delete h1; 
    if (debug) cout << "     Cloning h1...." << endl;
    h1 = (TH1*)effHistograms[j]->Clone("h1");

    if (debug) cout << "     h1 found...." << endl;

    if (debug) cout << "Creating TString..." << endl;
    TString histoName("TTPValBackgroundStudy_");
    if (debug) cout << "Initial TString created..." << endl;
    histoName+=hnames[j/3];
    if (debug) cout <<"TString 1nd part added..." << endl;
    if ( !(j%3) ) histoName+="_pt";
    else if (!((j-1)%3)) histoName+="_eta";
    else if (!((j-2)%3)) histoName+="_phi";

    TH1* h2 = 0;
    //h2 = (TH1*)gROOT->Get("h2");
    //if( h2 ) delete h2;
    if (debug) cout << "Accessing histogram: " << histoName << endl;
    h2 = (TH1*)file1->Get(histoName)->Clone("h2");
    //if ( !(j%3) ) h2 = (TH1*)file1->Get("TTPValBackgroundStudy_"+hnames[j/3]+"_pt")->Clone("h2");
    //else if (!((j-1)%3))  h2 = (TH1*)file1->Get("TTPValBackgroundStudy_"+hnames[(j-1)/3]+"_eta")->Clone("h2");
    //else if (!((j-2)%3))  h2 = (TH1*)file1->Get("TTPValBackgroundStudy_"+hnames[(j-2)/3]+"_phi")->Clone("h2");

    if (debug) cout << "     h2 found..." << endl;


    //TH1* h2 = (TH1*)gROOT->Get("h2");
    //if( h2 ) delete h2;
    //if ( !(j%3) ) h2 = (TH1*)file1->Get(hnames[j/3]+"_pt")->Clone("h2");
    //else if (!((j-1)%3))  h2 = (TH1*)file1->Get(hnames[(j-1)/3]+"_eta")->Clone("h2");
    //else if (!((j-2)%3))  h2 = (TH1*)file1->Get(hnames[(j-2)/3]+"_phi")->Clone("h2");

    //if (debug) cout << "     h2 found..." << endl;

    TH1* h3 = 0;
    if (file2) {

    h3 = (TH1*)gROOT->Get("h3");
    if( h3 ) delete h3;
    if ( !(j%3) ) h3 = (TH1*)file2->Get(hnames[j/3]+"_pt")->Clone("h3");
    else if (!((j-1)%3))  h3 = (TH1*)file2->Get(hnames[(j-1)/3]+"_eta")->Clone("h3");
    else if (!((j-2)%3))  h3 = (TH1*)file2->Get(hnames[(j-2)/3]+"_phi")->Clone("h3");

    }

    if( h1 ){
      h1->SetLineColor(1);
      h1->Draw();
    }
    else cout << "No h1 found \n";
    
    if( h2 ){
      h2->SetLineColor(2);
      h2->Draw("lsame");
    }
    else cout << "No h2 found \n";
    
    if ( h3 ) {
      h3->SetLineColor(3);
      h3->Draw("lsame");
    }
    else cout << "No h3 found \n";
    
    if (debug) cout << "     Both h1 and h2 not empty..." << endl;

    if ( j==0 ) c2->Print(TString(saveFile)+"(");
    else if ( j+1==nHistos ) c2->Print(TString(saveFile)+")");
    else c2->Print(saveFile);
    
  }
}


void TTPValBackgroundStudy::update(TString name) {

  createEffHistograms();

  TH1F * histo;
  TH2F * histo2;

  TString dirName("cutStudy");

  if (debug) cout << "Starting Save..." << endl;
  TFile f(name, "update");
  if (debug) cout << "File created..." << endl;
  f.mkdir(dirName);
  f.cd(dirName);
  for (unsigned int j = 0; j < nHistos ; ++j ) {
    histo = (TH1F*)effHistograms[j];
    if (debug) cout << "Writing histogram " << j << "...." << endl;
    bool empty = true;
    for (int k = 0; k < histo->GetNbinsX(); ++k) {
      if (!( 1 - histo->GetBinContent(k) < 1E-5) && histo->GetBinContent(k) > 1E-5) {
	empty = false;
	break; //end for loop
      }
    }
    if (!empty) histo->Write();
    if (!empty) histo->Draw();
  }
  //f->Write();

  gStyle->SetHistLineColor(1);
  gStyle->SetMarkerStyle(20);
  gStyle->SetMarkerColor(1);

  histo2 = (TH2F*)testHisto[0];
  histo2->Write();
  histo2->Draw("BOX");
  histo2->UseCurrentStyle();

  histo2 = (TH2F*)testHisto[1];
  histo2->Write();
  histo2->Draw("BOX");
  histo2->UseCurrentStyle();


  if (debug) cout << "Histograms written..." << endl;
  f.Close();
  return;
  
}




void TTPValBackgroundStudy::reset() {

  for ( unsigned int i=0; i<nHistograms; ++i) {
    histograms[i]=0;
    effHistograms[i]=0;
  }
  hTot[0] = new TH1F("hptTot","",50,0,120000);
  hTot[1] = new TH1F("hphiTot","",64,-3.2,3.2);
  hTot[2] = new TH1F("hetaTot","",50,-2.5,2.5); //memory leak

  createHistograms(histograms);

}

void TTPValBackgroundStudy::createEffHistograms() {


  TH1F * effHist=0;
  switch ( type ) {

  case 0:
    createHistograms(effHistograms);
    if ( debug ) cout << "Array for Histograms created..." << endl;
    for (unsigned int j = 0; j != nHistos ; ++j ) {
      if ( debug ) cout << "Pointer to histograms created" << endl;
      if ( debug ) cout << histograms[j]->GetNbinsX() << " bins in histograms found..." << endl;
      if (!(j%3)) {
	if (debug) cout << "pt-Histogram found... j=" << j << endl;
	effHist =getEffHist(hTot[0],histograms[j],"TTPValBackgroundStudy_" + hnames[j/3]+"_pt");
      }
      else if (!((j-1)%3)) effHist =getEffHist(hTot[1],histograms[j],"TTPValBackgroundStudy_" + hnames[(j-1)/3]+"_eta");
      else if (!((j-2)%3)) effHist =getEffHist(hTot[2],histograms[j],"TTPValBackgroundStudy_" + hnames[(j-2)/3]+"_phi");      
      if ( debug ) cout <<j << " Efficicency Histograms created... " << endl;
      effHistograms[j] = effHist;
      if ( debug ) cout << effHist->GetNbinsX() << " bins in effHist found..." << endl;
    }
    if (debug ) cout << "Efficiency Histograms filled." << endl;

    break;

  case 1 :
      
    createHistograms(effHistograms);
    if ( debug ) cout << "Array for Histograms created..." << endl;
    for (unsigned int j = 0; j != nHistos ; ++j ) {
      if ( debug ) cout << "Pointer to histograms created" << endl;
      if ( j==0 ) effHist = getEffHist(hTot[j],histograms[j],"TTPValBackgroundStudy_" + hnames[0]+"_pt");
      if ( j==1 ) effHist = getEffHist(hTot[j],histograms[j],"TTPValBackgroundStudy_" + hnames[0]+"_eta");
      if ( j==2 ) effHist = getEffHist(hTot[j],histograms[j],"TTPValBackgroundStudy_" + hnames[0]+"_phi");
      else {
	if (!(j%3)) effHist =getEffHist(histograms[j-3],histograms[j],"TTPValBackgroundStudy_" + hnames[j/3]+"_pt");
	else if (!((j-1)%3)) effHist =getEffHist(histograms[j-3],histograms[j],"TTPValBackgroundStudy_" + hnames[(j-1)/3]+"_eta");
	else if (!((j-2)%3)) effHist =getEffHist(histograms[j-3],histograms[j],"TTPValBackgroundStudy_" + hnames[(j-2)/3]+"_phi");
      }
      if ( debug ) cout <<j << " Efficicency Histograms created... " << endl;
      effHistograms[j] = effHist;
    }
    if ( debug ) cout << "Efficiency Histograms filled." << endl;

    break;
      
  };
}



TH1F * TTPValBackgroundStudy::getEffHist(TH1F * hTot1, TH1F * hCut1, TString name)
{
  if( hTot1 == 0 || hCut1 == 0 ) cout << "ERROR: Input to getEffHist is bad!!! " << endl;
  
  TH1F * hEff = (TH1F*)hTot1->Clone(name);
  
  if ( debug ) cout << "Dividing now...." << endl;
  if ( debug ) cout << "Dividing....done!" << endl;
  hEff->GetYaxis()->SetTitle("Efficiency");
  hEff->SetMaximum(1.1);
  hEff->SetMinimum(0);
  if ( debug ) cout << "Configuration of histogram....done" << endl;
  hEff->SetTitle(hCut1->GetTitle());
  if ( debug ) cout << "Title set to " << hCut1->GetTitle() << endl;

  unsigned int nbin = hCut1->GetNbinsX();
  if ( debug ) cout << hCut1->GetNbinsX() << " bins found..." << endl;
  float eff; float errEff;
  for(unsigned int itau = 1 ; itau < nbin ; ++itau)
    {
      eff = 0; errEff = 0;
      if( hTot1->GetBinContent(itau) > 0 )
        {
	  if (debug ) cout << "Bin content of hCut is " << hCut1->GetBinContent(itau) << endl;
	  if ( debug ) cout << "Bin content of hTol is " << hTot1->GetBinContent(itau) << endl;
          eff  = float(hCut1->GetBinContent(itau))/float( hTot1->GetBinContent(itau));
          errEff = sqrt(eff*(1.-eff)/float( hTot1->GetBinContent(itau)));
	  if ( debug ) cout << "Reuting efficiency is " << eff << endl;
          //eff *= 100.;
          //errEff *= 100.;
          hEff->SetBinContent(itau, eff);
          hEff->SetBinError(itau, errEff);
        }
    }
  return hEff;
}

void TTPValBackgroundStudy::createHistograms(TH1F ** histos) {
  TString name;
  for (unsigned int count = 0; count!=nNames; count++ ) {
    if (debug) cout << " Will create " << nNames << " histograms!" << endl;

    if (debug) cout << "Creating Histogram " << count << endl;
    name="TTPValBackgroundStudy_" + hnames[count]+"_pt";
    //if (debug ) cout << " b\n";
    histos[3*count]= new TH1F("TTPValBackgroundStudy_" + name, name, 50, 0, 120000);
    if (debug) cout << " At step 2 of " << 3*count << ", variable nNames: " << nNames << endl;
    //if (debug) cout << " c\n";
    //if (debug) cout << " d\n";
    histos[3*count]->GetXaxis()->SetTitle("pt");
    //if (debug) cout << " e\n";
    if (debug) cout << " After creating histogram " << 3*count << ", variable nNames: " << nNames << endl;
    
    name = "TTPValBackgroundStudy_" + hnames[count]+"_phi";
    //cout << " f "<< 3*i+1 << " " << nNames<<"\n";
    histos[3*count+1] = new TH1F("TTPValBackgroundStudy_" + name, name, 64, -3.2, 3.2);
    //cout << " g\n";
    //cout << " h\n";
    histos[3*count+1]->GetXaxis()->SetTitle("phi");
    //cout << " i\n";
    if (debug) cout << " After creating histogram " << 3*count+1 << ", variable nNames: " << nNames << endl;
        
    name = "TTPValBackgroundStudy_" + hnames[count]+"_eta";
    histos[3*count+2] = new TH1F("TTPValBackgroundStudy_" + name, name, 50, -2.5, 2.5);
    histos[3*count+2]->GetXaxis()->SetTitle("eta");
    //cout << " j\n";
    if (debug) cout << " After creating histogram " << 3*count+2 << ", variable nNames: " << nNames << endl;
    }
    
   return;

}

void TTPValBackgroundStudy::ATLAS_LABEL(Double_t x,Double_t y,Color_t color) {

  TLatex l; //l.SetTextAlign(12); l.SetTextSize(tsize);
  l.SetNDC();
  l.SetTextFont(72);
  l.SetTextColor(color);
  l.DrawLatex(x,y,"ATLAS");
}

TString TTPValBackgroundStudy::getName(TrigTauMCTauList::OFFLINESELECTION offlineType1) {

  TString value;

  if (offlineType1 == TrigTauMCTauList::TAU1P3P) value = "TAU1P3P";
  if (offlineType1 == TrigTauMCTauList::TAUREC) value = "TAUREC";
  if (offlineType1 == TrigTauMCTauList::EITHERTAU) value = "EITERHTAU";
  if (offlineType1 == TrigTauMCTauList::BOTHTAU) value = "BOTHTAU";
  if (offlineType1 == TrigTauMCTauList::NONE) value = "NONE";

  return value;
}
