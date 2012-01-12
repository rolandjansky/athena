/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <iomanip>
using namespace std;
#include "TrigTauPerformValidation/TTPValDistribution.h"



TTPValDistribution::TTPValDistribution(TrigTauBugFix::TTPVERSION /*version*/, bool debug1) { 
  cout << "WARNING: Filename not specified!! " << endl; 
  initialize(debug1);
  fileDefined = false;
  return; 
}

TTPValDistribution::TTPValDistribution (const char * filename1, TrigTauBugFix::TTPVERSION version,  bool debug1) 
  : bugfix(version, TrigTauBugFix::RDO)  {
    
  if (debug1) cout << " Creating validation4 object..." << endl; 
  filename = filename1;  
  chain = new TChain("eventTree");
  chain->Add(filename.c_str());
  initialize(debug1);
  fileDefined = true;

}

void TTPValDistribution::initialize(bool debug1) {


  debug = debug1;
  signal = true;
  maxVisEta = 2.5;
  minLeadingPt = 6.0;
  selected = true;
  logScale = false;
  plotAllMatchedEvents = false;
  gStyle->SetPadGridX(true);
  gStyle->SetPadGridY(true);
  menu = TrigTauFlags::TAU20;
  cutLevel = TrigTauSelectionManager::LOOSE;
  offlineType =  TrigTauMCTauList::TAUCUTSAFEMEDIUM;
  menuVersion = TrigTauSelectionManager::MENU1560;
  chainType = TrigTauFlags::SITRK; 
 
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

  hnames.push_back("ROI_phi");

  nL1Cuts = 6;
  nL2CaloCuts = 8;
  nL2TrkCuts = 8;
  nEFCuts = 10;
  nHistos = hnames.size()*3;
  nNames = hnames.size();


  if ( nL1Cuts+nL2CaloCuts+nL2TrkCuts+nEFCuts != nNames || nHistos != nHistograms ) {
    cout << "ERROR: Number of Cuts for different levels incorrect!!!" << endl;
    return;
  }

  

}


void TTPValDistribution::setPlotAllEvents (bool newValue) {

  ignoreMatching = newValue;
  if (newValue == true) plotAllMatchedEvents = true;
  return;
}



void TTPValDistribution::setFile(const char * filename1) {
  filename = filename1;
  chain = new TChain("eventTree"); //potential memory leak
  chain->Add(filename.c_str());
  fileDefined = true;
  return;
}


void TTPValDistribution::analyze(int number) {
  
  if (!fileDefined) {
    cout << "File was not defined and variables not initialized!!" << endl;
    return;
  }

  if (debug) cout << "Creating Histograms..." << endl;
  createHistograms(histograms);
  if (debug) cout << "Histograms created!   Building selmanager..." << endl;
  selmanager.setMenuVersion(menuVersion);
  selmanager.setTauMenu(menu, cutLevel);
  if (debug) cout << "Validation object created!!" << endl;


  // MC truth
  for (unsigned int i = 0; i!=3; ++i) {

  analyzer[i].truthList.setMaxVisEta(maxVisEta);
  analyzer[i].truthList.setMinVisPt(menu); // in GeV
  analyzer[i].truthList.setMinLeadingPt(minLeadingPt); // pt of the leading pt of track is 
  if ( i == 1 ) analyzer[i].truthList.select1Prong();
  else if ( i == 2 ) analyzer[i].truthList.select3Prong();
  analyzer[i].truthList.setRecoMatchdR(0.2);
  analyzer[i].trigMan.setL1MET(0.);
  analyzer[i].trigMan.setMenuVersion(menuVersion );
  analyzer[i].trigMan.setTauMenu(menu, cutLevel);
  analyzer[i].chainList.setRoiWordMatching(false);
  analyzer[i].chainList.setHLTAuthor(chainType);
  }

  event  = new TrigTauEvent();
  chain->SetBranchAddress("EventBranch", &event);
  chain->SetBranchStatus("*", 1);

  // Also set an L1 MET cut if desired
  
  int nev = int(chain->GetEntries());
  if ( nev > number ) nev = number;
  for(int iev = 0; iev<nev; ++iev) {
    analyzeEvent( iev );
  }
}


void TTPValDistribution::analyzeEvent(int eventNumber) {

  event->reset();
  chain->GetEvent(eventNumber);
  bugfix.fix(event);

  for (unsigned int j =0; j!=3; ++j) {

    analyzer[j].analyze(event, signal, offlineType);
 
    if (j==2 && debug) cout <<"Checking 3-prong decay..." << endl;

    // ============ Select type of events (e.g. signal) ================
    if( signal ) {
      // analyzer[j].truthList.fillPureList(&event->mcEvent);
//       analyzer[j].truthList.fillGoodList();
//       analyzer[j].truthList.requireRecoMatch(*event, offlineType);
      if ( debug && analyzer[j].truthList.nMCTauGood() ==0 ) cout << "Event contains no good taus! " << endl;
      if(  analyzer[j].truthList.nMCTauGood() ==0 ) return;
    }

    if (j==2 && debug) cout << "3-Prong decay found!" << endl;
  
    // loop over taus after fiducial cuts
    
    if ( debug ) cout << "Event contains " << analyzer[j].truthList.nMCTauGood() << " good tau!" << endl;
    for( unsigned int iTrue = 0; iTrue <  analyzer[j].truthList.nMCTauGood(); ++iTrue )  //loop over all true taus
      {
	const TrigTauMCTau * truth = & analyzer[j].truthList.mcTauGoodEntry(iTrue);    //select tau
			
        unsigned int nTrigChains = analyzer[j].chainList.nTrigChains() ;
	if (debug ) cout << "Value of nTrigChains: " << nTrigChains << endl;
	if ( nTrigChains == 0 ) continue;

        for( unsigned int ichain = 0; ichain != nTrigChains; ++ichain) {
           if ( debug ) cout << "Starting chain " << ichain << "!" << endl;
           const TrigTauTrigChain *ch = &analyzer[j].chainList.trigChainEntry(ichain);
	   const TrigTauL1Object* l1object = ch->L1;
           const TrigTauL2Object* l2object = ch->L2;
	   const TrigTauL2Object* l2calobject = ch->L2Cal;
           const TrigTauL2Object* l2trkobject = ch->L2Trk;
           const TrigTauRecEFObject* efobject = ch->EF;

	   //check Level 1
	   if (l1object == 0 && !plotAllMatchedEvents) {
	     if ( debug ) cout << "L1 Object is empty; go to next chain" << endl;
	     continue;
	   }
	   if (debug) cout << "Checking Level 1..." << endl;
	   if( !(l1object == 0 ) && (truth->DeltaR(*l1object)<0.2 || ignoreMatching) ) {
	     if (debug) cout << "Good L1 object found... running cutWord" << endl;
	     unsigned int currentWord = selmanager.l1Sel.cutWord(*l1object);
	     //loop over all possible variables to cut on -> fill histograms; Probably can get it from TrigTauL1Object
	     histograms[0*3+j]->Fill(l1object->m_TauEnergy);
	     histograms[1*3+j]->Fill(l1object->m_EmIsol);
	     histograms[2*3+j]->Fill(l1object->m_HdIsol);
	     histograms[3*3+j]->Fill(l1object->m_EmCore);
	     histograms[4*3+j]->Fill(l1object->m_HdCore);
	     histograms[5*3+j]->Fill(l1object->m_EmEnergy);
	     //histograms[0*3+j]->Fill(l1object->m_EmEnergy);
	     //histograms[0*3+j]->Fill(l1object->m_EmEnergy);
	     if (debug) cout << "Histograms filled..." << endl;
	     
	     if (!plotAllMatchedEvents && currentWord != 0 ) continue;
	     if (debug) cout << "L1 done... " << endl;
           }
	   else if (!plotAllMatchedEvents) continue;

           //check Level 2 (including Trk and Cal)
           if (l2object == 0 && !plotAllMatchedEvents) {
	     if ( debug ) cout << "L2 object is empty; go to next chain" << endl;
	     continue;
	   }
	   if ( debug && !l2object == 0 ) cout << "     L2 DeltaR is :" << ( truth->DeltaR(*l2object) ) << endl;
           if( !(l2object == 0) && (truth->DeltaR(*l2object)<0.2 || ignoreMatching) ) {
		    
	     unsigned int currentWord1 = selmanager.l2CalSel.cutWord(*l2calobject);
	     unsigned int currentWord2 = selmanager.l2TrkSel.cutWord(*l2trkobject);
             histograms[6*3+j]->Fill(l2calobject->etClus());
	     histograms[7*3+j]->Fill(l2calobject->etClusCalib());
	     histograms[8*3+j]->Fill(l2calobject->m_nStrips);
	     if (!selected || l2calobject->m_stripWidth2 > -90 ) histograms[9*3+j]->Fill(l2calobject->m_stripWidth2);
	     if (!selected || (l2calobject->m_EMRadius > -0.5 && l2calobject->m_EMRadius < 0.5) ) histograms[10*3+j]->Fill(l2calobject->m_EMRadius);
	     if (!selected || (l2calobject->m_isoFrac > -2 && l2calobject->m_isoFrac < 2) ) histograms[11*3+j]->Fill(l2calobject->m_isoFrac);
	     if (!selected || (l2calobject->m_EMEnergyWidth[2] < 100 && l2calobject->m_EMEnergyWidth[2] > -90) ) histograms[12*3+j]->Fill(l2calobject->m_EMEnergyWidth[2]);
	     histograms[13*3+j]->Fill(l2calobject->etNarrow());
	     //TrigTauL2TrkFEX l2FEX(l2object);
	     //l2FEX.setL2Object(l2object);
	     //l2FEX.calculate();
	     histograms[14*3+j]->Fill(l2object->m_nCoreTracks);
	     histograms[15*3+j]->Fill(l2object->m_nSlowTracks);
	     histograms[16*3+j]->Fill(l2object->m_nIsoTracks);
	     histograms[17*3+j]->Fill(l2object->m_nMatchedTracks);
	     histograms[18*3+j]->Fill(l2object->m_charge);
	     histograms[19*3+j]->Fill(l2object->m_leadingTrackPt);
	     histograms[20*3+j]->Fill(l2object->m_scalarPtSumIso/l2object->m_scalarPtSumCore);
	     histograms[21*3+j]->Fill(l2object->ptTot());
	          
	     if ( (currentWord1 !=0 || currentWord2 !=0) && !plotAllMatchedEvents ) continue;
           }
	   else if (!plotAllMatchedEvents) continue;


           //check EF
	   if (efobject == 0) {
             if ( debug ) cout << "EF object is empty; go to next chain" << endl;
             continue;
	   }
           if ( debug ) cout << "     EF DeltaR is :" << ( truth->DeltaR(*efobject) ) << endl;
           if ( truth->DeltaR(*efobject)<0.2 || ignoreMatching ) {
	     unsigned int currentWord = selmanager.efSel.cutWord(*efobject);
	     histograms[22*3+j]->Fill(efobject->nTracks());
	     histograms[23*3+j]->Fill(efobject->nTracks());
	     if (!selected || efobject ->m_EMRadius > -0.2 ) histograms[24*3+j]->Fill(efobject->m_EMRadius);
             if (debug) cout << "EMRadius histogram filled." << endl;
	     if (!selected || efobject->m_isoFrac > -0.2 ) histograms[25*3+j]->Fill(efobject->m_isoFrac);
	     float emFrac=0;
	     if (!selected || efobject->m_etEMCalib>0 ) emFrac = efobject->m_etEMCalib/(efobject->m_etEMCalib + efobject->m_etHadCalib);
	     histograms[26*3+j]->Fill(emFrac);
	     if (debug) cout << "EMFrac histogram filled. " << endl;
	     float ptMax = 999999999.;
	     if (efobject->nTracks() ) {
		if (debug) cout << "Non-zero number of tracks found." << endl;
		if (efobject->leadingTrack()) ptMax = efobject->leadingTrack()->Pt();
	     }
             if (debug) cout << "ptMax calculated." << endl;
	     if (!selected || ptMax < 100000 ) histograms[27*3+j]->Fill(ptMax);
	     histograms[28*3+j]->Fill(efobject->Et());
	     if (debug) cout << "Et histogram filled" << endl;
	     histograms[29*3+j]->Fill(efobject->m_energy/cosh(efobject->m_caloEta));
	     //histograms[30*3+j]->Fill(efobject->mass()); mass is more complicated; will be added later.	
	     histograms[30*3+j]->Fill(j);
             histograms[31*3+j]->Fill(efobject->Phi());
	     if (debug) cout << "All histograms filled..." << endl;
	     if ( currentWord == 0 && debug ) cout << "Tau passed trigger!!!" << endl;

           }
	   if (debug ) cout << "All levels for chain " << ichain  << " checked." << endl;
	} // end chain loop
      } // end loop over truth
  } //end loop over different prong decays
    return;
}


void TTPValDistribution::print() {
 
  
  TCanvas * c1 = (TCanvas *)gROOT->Get("c1");
  if( c1 ) delete c1;
  c1 = new TCanvas("c1","",10,10,800,500);
  if (logScale) gPad->SetLogy(1);
  /*
  //c1->Divide(3,2);
  TPad *p1 = new TPad("p1","", 10, 10, 800, 500);
  c1->cd();
  p1->Draw();
  p1->cd();
  p1->Divide(3,2);
  //c1->Print("ntuple.eps[");

  for (unsigned int j = 0; j<nHistos; j=j+6) {
    for (unsigned int k = 0; k!=6; ++k) {
      if (j+k < nHistos) {
	c1->cd();
	if (debug) cout << "Drawing histo no. " << j+k << endl;
	c1->cd(k);
	if (debug) cout << "Creating pointer..." << endl;
	TH1F * histo = (TH1F*)histograms[j+k];
	if (histo != 0) histo->Draw();
	if (debug) cout << "Histogram created..." << endl;
      }
    }
    c1->Update();
    //c1->Print("ntuple.eps");
  }

  //c1->Print("ntuple.eps]");
  */
  if (debug) cout << "Number of total histograms: " << nHistos << endl;
  for (unsigned int j = 0; j < (nHistos) ; j+=3 ) {
    TH1F * histo1 = (TH1F*)histograms[j];
    TH1F * histo2 = (TH1F*)histograms[j+1];
    TH1F * histo3 = (TH1F*)histograms[j+2];
    
    if ( debug ) cout << "Draw Histogram No. " << j << endl;
    if ( histo1 != 0 ) histo1->Draw();
    histo2->SetLineColor(kRed);
    if ( histo2 != 0) histo2->Draw("same");
    histo3->SetLineColor(kBlue);
    if ( histo3 != 0 )histo3->Draw("same");
    else cout << "Missing 3Pr histogram" << endl;
    if ( debug ) cout << "Printing Histogram No " << j << endl;
    
    if ( j==0 ) c1->Print("trigtau.ntuple.eps(");
    else if ( j> nHistos-3 ) c1->Print("trigtau.ntuple.eps)");
    else c1->Print("trigtau.ntuple.eps");
    }
  
  return;
}

void TTPValDistribution::save(const char * name) {

  TH1F * histo;

  if (debug) cout << "Starting Save..." << endl;
  TFile f(name, "recreate");
  if (debug) cout << "File created..." << endl;
  for (unsigned int j = 0; j < nHistos ; ++j ) {
    histo = (TH1F*)histograms[j];
    if (debug) cout << "Writing histogram " << j << "...." << endl;
    histo->Scale(1/histo->Integral());
    histo->Write();
    histo->Draw();
  }
  //f->Write();
  if (debug) cout << "Histograms written..." << endl;
  f.Close();
  return;
}

void TTPValDistribution::compare(const char * reference, const char * output) {

  compare(reference, "", output);

}


void TTPValDistribution::compare(const char * reference1,
                                 const char * reference2, 
                                 const char * output) {

  TCanvas * c2 = (TCanvas *)gROOT->Get("c2");
  if( c2 ) delete c2;
  c2 = new TCanvas("c1","",10,10,800,500);
  if (logScale) gPad->SetLogy(1);

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
    h1 = (TH1*)histograms[j]->Clone("h1");

    if (debug) cout << "     h1 found...." << endl;

    TH1* h2 = (TH1*)gROOT->Get("h2");
    if( h2 ) delete h2;
    if ( !(j%3) ) h2 = (TH1*)file1->Get(hnames[j/3]+"_ALL")->Clone("h2");
    else if (!((j-1)%3))  h2 = (TH1*)file1->Get(hnames[(j-1)/3]+"_1Prong")->Clone("h2");
    else if (!((j-2)%3))  h2 = (TH1*)file1->Get(hnames[(j-2)/3]+"_3Prong")->Clone("h2");

    if (debug) cout << "     h2 found..." << endl;
    
    TH1 * h3 = 0;
    if (file2) {
      
    h3 = (TH1*)gROOT->Get("h3");
    if( h3 ) delete h3;
    if ( !(j%3) ) h3 = (TH1*)file2->Get(hnames[j/3]+"_ALL")->Clone("h3");
    else if (!((j-1)%3))  h3 = (TH1*)file2->Get(hnames[(j-1)/3]+"_1Prong")->Clone("h3");
    else if (!((j-2)%3))  h3 = (TH1*)file2->Get(hnames[(j-2)/3]+"_3Prong")->Clone("h3");

    if (debug) cout << "     h3 found..." << endl;

    }


    if( h1 ){
      h1->SetLineColor(1);
      h1->DrawNormalized("",1);
    }
    else cout << "No h1 found \n";
    
    if( h2 ){
      h2->SetLineColor(2);
      h2->DrawNormalized("same", 1);
    }
    else cout << "No h2 found \n";
    
    if (h3) {
      h3->SetLineColor(3);
      h3->DrawNormalized("same", 1);
    }
    else cout << "No h3 found \n";


    if ( j==0 ) c2->Print(TString(output)+"(");
    else if ( j+1==nHistos ) c2->Print(TString(output)+")");
    else c2->Print(output);
    
  }

}


void TTPValDistribution::update(const char * name) {

  TH1F * histo;

  if (debug) cout << "Starting Save..." << endl;
  TFile f(name, "update");
  if (debug) cout << "File created..." << endl;

  f.cd();
  f.mkdir("Distribution");
  f.cd("Distribution");

  for (unsigned int j = 0; j < nHistos ; ++j ) {
    histo = (TH1F*)histograms[j];
    if (debug) cout << "Writing histogram " << j << "...." << endl;
    histo->Write();
    histo->Draw();
  }
  //f->Write();
  if (debug) cout << "Histograms written..." << endl;
  f.Close();
  return;
}


void TTPValDistribution::createHistograms(TH1F ** histos) {

  TString name;
  for (unsigned int count = 0; count!=nNames; count++ ) {
    if (debug) cout << " Will create " << nNames << " histograms!" << endl;
    if (debug) cout << "Creating Histogram " << count << endl;
    name="TTPValDistribution_" + hnames[count]+"_ALL";
    //if (debug ) cout << " b\n";
    histos[3*count]= new TH1F(name, name, 100, -0.1, 0.1);
    histos[3*count]->SetBit(TH1::kCanRebin);
       
    name = "TTPValDistribution_" + hnames[count]+"_1Prong";
    //cout << " f "<< 3*i+1 << " " << nNames<<"\n";
    histos[3*count+1] = new TH1F(name, name, 100, -0.1, 0.1);
    histos[3*count+1]->SetBit(TH1::kCanRebin);
    if (debug) cout << " After creating histogram " << 3*count+1 << ", variable nNames: " << nNames << endl;
        
    name = "TTPValDistribution_" + hnames[count]+"_3Prong";
    histos[3*count+2] = new TH1F(name, name, 100, -0.1, 0.1);
    histos[3*count+2]->SetBit(TH1::kCanRebin);
    //cout << " j\n";
    if (debug) cout << " After creating histogram " << 3*count+2 << ", variable nNames: " << nNames << endl;
    }
    
  return;

}


