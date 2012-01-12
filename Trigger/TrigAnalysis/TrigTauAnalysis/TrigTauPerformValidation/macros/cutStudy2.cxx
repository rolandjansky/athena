/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iomanip>

void cutStudy2(int select = 2)
{
  gSystem->Load("libHist.so");
  if( select == 1 ){
    //cutStudy("ntuples/sampleA/14.1.0/trigtau.5200.root");
    //cutStudy("ntuples/TTP06_dR03/5107/trig1_misal1_csc11.005107.pythia_Wtauhad.digit.RDO.v12003103.TTP06_dR03.merged.0.root");
    cutStudy("ntuples/TTP07/5107/trig1_misal1_csc11.005107.pythia_Wtauhad.RDO.v12003103.TTP07.merged.0.root");
    //copyHistos("align/Phi_efficiencies_TTP06_dR03_NONE.root");
    //copyHistos("align/Phi_efficiencies_TTP07_NONE.root");
    copyHistos("efficiencies.eta.root");
    }
  
  else if( select == 2 ){
    cutStudy("~/temp/val/trigtau.5200.root");
    copyHistos("align/Eta_efficiencies_RTT_dev_June01_NONE.root");
    }
  
  else{
    cutStudy("ntuples/misal1day-5107/trigtau.misal*");
    copyHistos("misal/efficiencies.eta.root");
    }
}




void cutStudy(const char * ntuppath)
{
   TrigTauFlags::TRIGGERMENU menu = TrigTauFlags::TAU16I;
   cout << ntuppath << endl;
   TChain * chain = new TChain("eventTree");
   chain->Add(ntuppath);
   //chain->Add("ntuples/14.2.0_May28/5107/tmp.*");
   //chain->Add("ntuples/14.2.0_May28/5862/tmp.*");
   //chain->Add("ntuples/14.2.0/5107/tmp.*");

     doSet(chain, menu, 0);
//   doSet(chain, menu, 1);
//   doSet(chain, menu, 3);

//   TChain * chain1 = new TChain("eventTree");
//   oSampleTTP04(5009,chain1);
//   oSampleTTP04(5010,chain1);
//   oSampleTTP04(5011,chain1);
//   oSampleTTP04(5012,chain1);
  
  int nEvent, nL1Sel, nL2Calo, nL2Sel, nEFSel;
  //cutStudy(chain1, menu, nEvent, nL1Sel, nL2Calo, nL2Sel, nEFSel, false);
  //plotAllEfficiencies(-1,"dijet");

}



void doSet(TTree * chain,  TrigTauFlags::TRIGGERMENU menu, int nprong)
{
  int nEvent, nL1Sel, nL2Calo, nL2Sel, nEFSel;
  TString nprStr = "";
  if( nprong == 1 ) nprStr = ".1pr";
  else if ( nprong == 3 ) nprStr = ".3pr";

  TString baseName = TString("signal_truth")+nprStr;
  cout << baseName<<endl;
  cutStudy(chain, menu, nEvent, nL1Sel, nL2Calo, nL2Sel, nEFSel, true, nprong, TrigTauMCTauList::NONE);
  //plotAllEfficiencies(2,baseName);

//   baseName = TString("signal_offline")+nprStr;
//   cout << baseName<<endl;
//   cutStudy(chain, menu, nEvent, nL1Sel, nL2Calo, nL2Sel, nEFSel, true, nprong, TrigTauMCTauList::EITHER);
//   plotAllEfficiencies(-1,baseName);

//   baseName = TString("signal_tauRec")+nprStr;
//   cout << baseName<<endl;
//   cutStudy(chain, menu, nEvent, nL1Sel, nL2Calo, nL2Sel, nEFSel, true, nprong, TrigTauMCTauList::TAUREC);
//   plotAllEfficiencies(-1,baseName);

//   baseName = TString("signal_tau1p3p")+nprStr;
//   cout << baseName<<endl;
//   cutStudy(chain, menu, nEvent, nL1Sel, nL2Calo, nL2Sel, nEFSel, true, nprong, TrigTauMCTauList::TAU1P3P);
//   plotAllEfficiencies(-1,baseName );

//   baseName = TString("signal_bothoffline")+nprStr;
//   cout << baseName<<endl;
//   cutStudy(chain, menu, nEvent, nL1Sel, nL2Calo, nL2Sel, nEFSel, true, nprong, TrigTauMCTauList::BOTHTAU);
//   plotAllEfficiencies(-1,baseName);
}


void cutStudy(TTree * tree, 
	      TrigTauFlags::TRIGGERMENU menu1,
	      int & nEvent , int & nL1Sel, 
	      int & nL2Calo, int &  nL2Sel, int &  nEFSel,
	      bool signal=false, bool nprong = 0, 
	      TrigTauMCTauList::OFFLINESELECTION offlineType = TrigTauMCTauList::NONE)
{
  
  TrigTauHistogram::HISTTYPE histType=TrigTauHistogram::EXTERNAL; //PTHIST;

  //====================================================

  //gROOT->cd();
  // Open file
//   TFile* file = new TFile(filename);
//   if (!file->IsOpen()) {
//     cerr << "Can't open file " << filename << endl;
//     return;
//   }

  // Initialize tree
  //  TTree* tree = (TTree*)file->Get("eventTree");
  if (tree == 0) {
    cout << " Usage .x cutStudy(tree, menu)\n";
    return;
  }

  //===================================================
  // MC truth
  
  TrigTauMCTauList truthList;
  //truthList.setMaxVisEta(2.5);
  truthList.setMinVisPt(menu1);

  if( nprong == 1 )  truthList.select1Prong();
  else  if( nprong == 3 )  truthList.select3Prong();

  truthList.setRecoMatchdR(0.2);
  
  //===================================================

  nEvent = 0;
  nL1Sel = 0;
  nL2Calo = 0;
  nL2Sel = 0;
  nEFSel = 0;



  TrigTauEvent* event = new TrigTauEvent();
  tree->SetBranchAddress("EventBranch", &event);
  tree->SetBranchStatus("*", 1);
  tree->SetBranchStatus("mcEvent*", 1);

  // Create our analyzer class
  TrigTauAnalyzer analyzer;

  // Also set an L1 MET cut if desired
  analyzer.trigMan.setL1MET(0.);
  TrigTauSelectionManager trigMan1;
  trigMan1.setMenuVersion(TrigTauSelectionManager::MENU1400);
  trigMan1.setTauMenu(menu1, TrigTauSelectionManager::LOOSE);

  if( histType == TrigTauHistogram::EXTERNAL )
    {
      //TrigTauHistogram::setHistLimits( TrigTauHistogram::EXTERNAL, "external", "true phi",67, -3.35, 3.35);
      TrigTauHistogram::setHistLimits( TrigTauHistogram::EXTERNAL, "external", "true eta",60, -3, 3);
      //TrigTauHistogram::setHistLimits( TrigTauHistogram::EXTERNAL, "external", "visible true pt, MeV", 60, 0, 120000);
    }
  trigMan1.l1Sel.setHistDebug(histType);
  trigMan1.l2CalSel.setHistDebug(histType);
  trigMan1.l2TrkSel.setHistDebug(histType);
  trigMan1.efSel.setHistDebug(histType);  
 
  analyzer.chainList.setRoiWordMatching(false); 

  TrigTauBugFix bugfix (TrigTauBugFix::TTP7, TrigTauBugFix::RDO);

  //trigMan1.setL2CalOnly(false);
  bool checkL2Cal = true;

  // Loop over all events
  int nev = int(tree->GetEntries());
  //if (nev > 100) nev=100;
  for(int iev = 0; iev<nev; ++iev) {
    event->reset();
    tree->GetEvent(iev);

    // ============ Select type of events (e.g. signal) ================
    if( signal ) {
      truthList.fillPureList(&event->mcEvent);
      truthList.fillGoodList();
      truthList.requireRecoMatch(*event, offlineType);
      if(  truthList.nMCTauGood() ==0 ) continue;

    //==================================================================
    }
    
    nEvent++;
       

    // Analyze this event
    analyzer.chainList.fillChainList(event);

    bool pass = false;

    float externalVar = 0;
 
    bugfix.fix(event);

    // Look for L1 pass
    unsigned int numCh = analyzer.chainList.nTrigChains();
    for (unsigned int iCh1=0; iCh1 < numCh; ++iCh1) {
      TrigTauTrigChain ch1 = analyzer.chainList.trigChainEntry(iCh1);
      if( !ch1.L1 ) continue;

      if( signal) // make sure that this is correct RoI
	{
	  bool matchTruth = false;
	  const TrigTauL1Object* l1 = ch1.L1;
	  for(unsigned int iTrue = 0; iTrue <  truthList.nMCTauGood(); ++iTrue)
	    {
	      const TrigTauMCTau * truth = &truthList.mcTauGoodEntry(iTrue);
	      if( truth->DeltaR(*l1)<0.2 ) { 
		matchTruth=true; 
		//externalVar = truth->Pt(); 
		//externalVar = truth->Phi(); 
		externalVar = truth->Eta(); 
	      }
	    }
	  if( !matchTruth ) continue;
	}

      trigMan1.l1Sel.histManager()->setExternal(externalVar);

       if (!trigMan1.passedL1(ch1)){
          continue;
      }
      pass = true;
      if (pass) break;
    }


    if (!pass) continue;
    nL1Sel++;

    // Look for L2Calo pass
    if( checkL2Cal ){
    pass = false;
    for (unsigned int iCh1=0; iCh1 < numCh; iCh1++) {
      TrigTauTrigChain ch1 = analyzer.chainList.trigChainEntry(iCh1);
      if( !ch1.L2 ) continue;

      if( signal) // make sure that this is correct RoI
	{
	  bool matchTruth = false;
	  const TrigTauL2Object* l2 = ch1.L2;
	  for(unsigned int iTrue = 0; iTrue <  truthList.nMCTauGood(); ++iTrue)
	    {
	      const TrigTauMCTau * truth = &truthList.mcTauGoodEntry(iTrue);
	      if( truth->DeltaR(*l2)<0.2 ) { 
		matchTruth=true; 
		//externalVar = truth->Pt(); 
		externalVar = truth->Eta(); 
                //externalVar = truth->Phi();
	      }
	    }
	  if( !matchTruth ) continue;
	}

      trigMan1.l2CalSel.histManager()->setExternal(externalVar);
      if (!trigMan1.passedL2Cal(ch1)) continue;

      pass = true;
      if (pass) break;
    }

    if (!pass) continue;
    nL2Calo++;
    }

    // continue;

    // Look for L2 pass
    pass = false;
    for (unsigned int iCh1=0; iCh1 < numCh; iCh1++) {
      TrigTauTrigChain ch1 = analyzer.chainList.trigChainEntry(iCh1);
      if( !ch1.L2 ) continue;

      if( signal) // make sure that this is correct RoI
	{
	  bool matchTruth = false;
	  const TrigTauL2Object* l2 = ch1.L2;
	  for(unsigned int iTrue = 0; iTrue <  truthList.nMCTauGood(); ++iTrue)
	    {
	      const TrigTauMCTau * truth = &truthList.mcTauGoodEntry(iTrue);
	      if( truth->DeltaR(*l2)<0.2 ) { 
		matchTruth=true; 
		//externalVar = truth->Pt(); 
		externalVar = truth->Eta();
                //externalVar = truth->Phi(); 
	      }
	    }
	  if( !matchTruth ) continue;
	}

      trigMan1.l2TrkSel.histManager()->setExternal(externalVar);
      if (!trigMan1.passedL2(ch1)) continue;

      pass = true;
      if (pass) break;
    }

    if (!pass) continue;
    nL2Sel++;



    // Look for EF pass
    pass = false;
    for (unsigned int iCh1=0; iCh1 < numCh; iCh1++) {
      TrigTauTrigChain ch1 = analyzer.chainList.trigChainEntry(iCh1);
      if( !ch1.EF ) continue;

      if( signal) // make sure that this is correct RoI
	{
	  bool matchTruth = false;
	  const TrigTauRecEFObject* ef = ch1.EF;
	  for(unsigned int iTrue = 0; iTrue <  truthList.nMCTauGood(); ++iTrue)
	    {
	      const TrigTauMCTau * truth = &truthList.mcTauGoodEntry(iTrue);
	      if( truth->DeltaR(*ef)<0.2 ) { 
		matchTruth=true; 
		//externalVar = truth->Pt(); 
		externalVar = truth->Eta(); 
                //externalVar = truth->Phi();
	      }
	    }
	  if( !matchTruth ) continue;
	}

      trigMan1.efSel.histManager()->setExternal(externalVar);
      if (!trigMan1.passedEF(ch1)) continue;
      pass = true;
      if (pass) break;
    }

    if (!pass) continue;
    nEFSel++;


  }  // end loop over events

  //  trigMan1.print();
  //  trigMan2.print();
  //trigMan1.l2CalSel.histManager()->plotAll();
  trigMan1.l1Sel.histManager()->plotAllEfficiencies(-1);
  trigMan1.l2CalSel.histManager()->plotAllEfficiencies(-1);
  trigMan1.l2TrkSel.histManager()->plotAllEfficiencies(-1);
  trigMan1.efSel.histManager()->plotAllEfficiencies(-1);

  
  cout << "N event " << nEvent << endl
       << " L1    " << nL1Sel    << " "  << float(nL1Sel)/float(nEvent)  << " "  << float(nL1Sel)/float(nEvent)<<   endl
       << " L2Cal " << nL2Calo   << " "  << float(nL2Calo)/float(nEvent) << " "  << float(nL2Calo)/float(nL1Sel)<< endl
       << " L2Trk " << nL2Sel    << " "  << float(nL2Sel)/float(nEvent)  << " "  << float(nL2Sel)/float(nL2Calo)<< endl
       << " EF    " << nEFSel    << " "  << float(nEFSel)/float(nEvent)  << " "  << float(nEFSel)/float(nL2Sel)<< endl;

//  cout << menu1.menuName(menu1) << " \& "<< nEvent << " & " << setprecision(3)
//       <<  float(nL1Sel)/float(nEvent)<< " & " << setprecision(3)
//       <<  float(nL2Calo)/float(nL1Sel)<< " & "<< setprecision(3)
//       <<  float(nL2Sel)/float(nL2Calo) << " & "<< setprecision(3)
//       <<  float(nEFSel)/float(nL2Sel)<< " \\ \n";


  delete event;
  //delete tree;
  // delete file;

  cout << " Use menu " << menu1.menuName(menu1) << endl;


  return;
}


int plotAllEfficiencies(int select = 3, TString baseFileName = "")
{
  //cout << "Plotting efficiencies..." << endl;
  TString name = "_undefined_eff";

  TString histList[10];
  TString labels[10];
  int index = 0;

  gStyle->SetPadGridY(true);
  gStyle->SetPadGridX(true);

  cout << "Selecting Level for plot..." << endl;

  if( select == 0 ) 
    {      
      //histList[index] ="EMEnergy_1";     labels[index] = "EMEnergy Cut"; index++;
      histList[index] ="TauEnergy_2";     labels[index] = "TauEnergy Cut"; index++;
      histList[index] ="EmIsol_3";     labels[index] = "EmIsol Cut"; index++;
      //histList[index] ="HadIsol_4";     labels[index] = "HadIsol Cut"; index++;
      //histList[index] ="EmCore_5";     labels[index] = "EmCore Cut"; index++;
      //histList[index] ="HadCore_6";     labels[index] = "HadCore Cut"; index++;

    }      else   if( select == 1 )    {      
      //histList[index] ="et_1";         labels[index] = "Et Cut"; index++;
      histList[index] ="etCalib_2";     labels[index] = "EtCalib Cut"; index++;
      //histList[index] ="nStrips_3";    labels[index] = "nStrips Cut"; index++;
      histList[index] ="stripWidth_4"; labels[index] = "stripWidth Cut"; index++;
      histList[index] ="emrad_5";      labels[index] = "EMRad Cut"; index++;
      histList[index] ="isoFrac_6";    labels[index] = "IsoFrac Cut"; index++;
      //histList[index] ="EmEnWidth_7";  labels[index] = "EmEnWidth Cut"; index++;

    }else if ( select == 2 ) {

      histList[index] ="ntrkExist_1"; labels[index] = "Tracks found"; index++;
      histList[index] ="ntrk_2";      labels[index] = "Ntrk Cut"; index++;
      histList[index] ="leadPt_3";    labels[index] = "Leading Pt Cut"; index++;
      histList[index] ="nSlow_4";     labels[index] = "nSlow Cut"; index++;
      histList[index] ="nIso_5";      labels[index] = "nIso Cut"; index++;
      histList[index] ="nMatched_6";  labels[index] = "nMatched Cut"; index++;
      histList[index] ="Q_7";         labels[index] = "Charge Cut"; index++;
      histList[index] ="sumPtCore_8"; labels[index] = "sumPtRatio Cut"; index++;
      histList[index] ="Et_trkEM_9";  labels[index] = "Et(trk+EM) Cut"; index++;

  }else if(select == 3 ) {
      histList[index] ="etCalibEF_1";  labels[index] = "EtCalib Cut"; index++;
      histList[index] ="ntrkMinEF_2";  labels[index] = "ntrkMin Cut"; index++;
      histList[index] ="ntrkMaxEF_3";  labels[index] = "ntrkMax Cut"; index++;
      histList[index] ="emradEF_4";    labels[index] = "EMRadius Cut"; index++;
      histList[index] ="isoFracEF_5";  labels[index] = "isoFraction Cut"; index++;
      histList[index] ="emFracEF_6";   labels[index] = "EMFraction Cut"; index++;
      histList[index] ="leadPtEF_7";   labels[index] = "Leading pT Cut"; index++;
      //histList[index] ="emEtEF_8";     labels[index] = "EM Et Cut"; index++;

  }else if(select == 4 ) {
      name = "_pt";
      histList[index] ="baseL1_0";     labels[index] = "After Soft L1"; index++;
      histList[index] ="baseL2Cal_0";  labels[index] = "L2Calo input"; index++;
      histList[index] ="baseL2Trk_0";  labels[index] = "L2Trk input"; index++;
      histList[index] ="baseEF_0";     labels[index] = "EF input"; index++;
      histList[index] ="emEtEF_8";     labels[index] = "EF output"; index++;

  }

  else if ( select == -1 ){
    if( baseFileName == "" )
      {
	cout << " Please use .x plotAllEfficiencies.cxx(-1, baseFileName)\n";
	return 1;
      }
    TCanvas * c1 = (TCanvas * )gDirectory->Get("c1");
    if( c1 ) delete c1;
    c1 = new TCanvas("c1","Selected",10,10,800,620);
    plotAllEfficiencies(0, baseFileName);
    c1->Print(baseFileName+name+ ".eps(");
    //c1->Print(baseFileName+".l1"+name+".eps");
    plotAllEfficiencies(1, baseFileName);
    c1->Print(baseFileName+name+ ".eps");
    //c1->Print(baseFileName+".l2cal"+name+".eps");
    plotAllEfficiencies(2, baseFileName);
    c1->Print(baseFileName+name+ ".eps");
    //c1->Print(baseFileName+".l2trk"+name+".eps");
    plotAllEfficiencies(3, baseFileName);
    c1->Print(baseFileName+name+ ".eps");
    //c1->Print(baseFileName+".ef"+name+".eps");
    plotAllEfficiencies(4, baseFileName);
    c1->Print(baseFileName+name+ ".eps)");
    //c1->Print(baseFileName+".level"+name+".eps");
    return 0;
  }
  
  for(unsigned int i = 0; i < index ; ++i)
    {
      histList[i].Append(name);
    }

  cout << "Setting title..." << endl;

  // set title
  TH1 * h1 = (TH1*)gDirectory->Get(histList[0]);
  if( h1 == 0 ) {cout << " histogram " << histList[0] << " not found \n"; return 1;}
  if(      select == 0 ) h1->SetTitle(baseFileName+ " L1 selection");
  else if( select == 1 ) h1->SetTitle(baseFileName+ " L2 Calo selection");
  else if( select == 2 ) h1->SetTitle(baseFileName+ " L2 Trk selection");
  else if( select == 3 ) h1->SetTitle(baseFileName+ " EF selection");
  
  

  // draw
  cout << "Drawing histograms...." << endl;  
  TH1F * hist = (TH1F*)gDirectory->Get(histList[0]);
  //float xmin = hist->GetXaxis()->GetXmin();
  TLegend *legend = new TLegend(0.5,0.2,0.7,0.5);
  
  hist->Draw(); legend->AddEntry(hist, labels[0], "l");
  for(unsigned int ihist = 1; ihist < index; ++ihist)
    {
      hist = (TH1F*)gROOT->Get(histList[ihist]);
      hist->Draw("same"); 
      cout << "Add histogram " << labels[ihist] << endl;
      legend->AddEntry(hist, labels[ihist], "l");
    }
  legend->Draw();
  return 0;
}





void copyHistos(const char * name )
{
  TFile * fil = new TFile(name,"recreate");

TH1 * h1 = (TH1*)gROOT->Get("baseL1_0_external")->Clone("baseL1_0_external_a");
h1 = (TH1*)gROOT->Get("EmEnergy_1_external")->Clone("EmEnergy_1_external_a");
h1=(TH1*)gROOT->Get("TauEnergy_2_external")->Clone("TauEnergy_2_external_a");
h1=(TH1*)gROOT->Get("EmIsol_3_external")->Clone("EmIsol_3_external_a");
h1=(TH1*)gROOT->Get("HadIsol_4_external")->Clone("HadIsol_4_external_a");
h1=(TH1*)gROOT->Get("EmCore_5_external")->Clone("EmCore_5_external_a");
h1=(TH1*)gROOT->Get("HadCore_6_external")->Clone("HadCore_6_external_a");
h1=(TH1*)gROOT->Get("baseL2Cal_0_external")->Clone("baseL2Cal_0_external_a");
h1=(TH1*)gROOT->Get("et_1_external")->Clone("et_1_external_a");
h1=(TH1*)gROOT->Get("etCalib_2_external")->Clone("etCalib_2_external_a");
h1=(TH1*)gROOT->Get("nStrips_3_external")->Clone("nStrips_3_external_a");
h1=(TH1*)gROOT->Get("stripWidth_4_external")->Clone("stripWidth_4_external_a");
h1=(TH1*)gROOT->Get("emrad_5_external")->Clone("emrad_5_external_a");
h1=(TH1*)gROOT->Get("isoFrac_6_external")->Clone("isoFrac_6_external_a");
h1=(TH1*)gROOT->Get("EmEnWidth_7_external")->Clone("EmEnWidth_7_external_a");
h1=(TH1*)gROOT->Get("baseL2Trk_0_external")->Clone("baseL2Trk_0_external_a");
h1=(TH1*)gROOT->Get("ntrkExist_1_external")->Clone("ntrkExist_1_external_a");
h1=(TH1*)gROOT->Get("ntrk_2_external")->Clone("ntrk_2_external_a");
h1=(TH1*)gROOT->Get("leadPt_3_external")->Clone("leadPt_3_external_a");
h1=(TH1*)gROOT->Get("nSlow_4_external")->Clone("nSlow_4_external_a");
h1=(TH1*)gROOT->Get("nIso_5_external")->Clone("nIso_5_external_a");
h1=(TH1*)gROOT->Get("nMatched_6_external")->Clone("nMatched_6_external_a");
h1=(TH1*)gROOT->Get("Q_7_external")->Clone("Q_7_external_a");
h1=(TH1*)gROOT->Get("sumPtCore_8_external")->Clone("sumPtCore_8_external_a");
h1=(TH1*)gROOT->Get("Et_trkEM_9_external")->Clone("Et_trkEM_9_external_a");
h1=(TH1*)gROOT->Get("baseEF_0_external")->Clone("baseEF_0_external_a");
h1=(TH1*)gROOT->Get("etCalibEF_1_external")->Clone("etCalibEF_1_external_a");
h1=(TH1*)gROOT->Get("ntrkMinEF_2_external")->Clone("ntrkMinEF_2_external_a");
h1=(TH1*)gROOT->Get("ntrkMaxEF_3_external")->Clone("ntrkMaxEF_3_external_a");
h1=(TH1*)gROOT->Get("emradEF_4_external")->Clone("emradEF_4_external_a");
h1=(TH1*)gROOT->Get("isoFracEF_5_external")->Clone("isoFracEF_5_external_a");
h1=(TH1*)gROOT->Get("emFracEF_6_external")->Clone("emFracEF_6_external_a");
h1=(TH1*)gROOT->Get("leadPtEF_7_external")->Clone("leadPtEF_7_external_a");
h1=(TH1*)gROOT->Get("emEtEF_8_external")->Clone("emEtEF_8_external_a");
h1=(TH1*)gROOT->Get("massEF_9_external")->Clone("massEF_9_external_a");
h1=(TH1*)gROOT->Get("EmEnergy_1_external_eff")->Clone("EmEnergy_1_external_eff_a");
h1=(TH1*)gROOT->Get("TauEnergy_2_external_eff")->Clone("TauEnergy_2_external_eff_a");
h1=(TH1*)gROOT->Get("EmIsol_3_external_eff")->Clone("EmIsol_3_external_eff_a");
h1=(TH1*)gROOT->Get("HadIsol_4_external_eff")->Clone("HadIsol_4_external_eff_a");
h1=(TH1*)gROOT->Get("EmCore_5_external_eff")->Clone("EmCore_5_external_eff_a");
h1=(TH1*)gROOT->Get("HadCore_6_external_eff")->Clone("HadCore_6_external_eff_a");

h1=(TH1*)gROOT->Get("et_1_external_eff")->Clone("et_1_external_eff_a");
h1=(TH1*)gROOT->Get("etCalib_2_external_eff")->Clone("etCalib_2_external_eff_a");
h1=(TH1*)gROOT->Get("nStrips_3_external_eff")->Clone("nStrips_3_external_eff_a");
h1=(TH1*)gROOT->Get("stripWidth_4_external_eff")->Clone("stripWidth_4_external_eff_a");
h1=(TH1*)gROOT->Get("emrad_5_external_eff")->Clone("emrad_5_external_eff_a");
h1=(TH1*)gROOT->Get("isoFrac_6_external_eff")->Clone("isoFrac_6_external_eff_a");
h1=(TH1*)gROOT->Get("EmEnWidth_7_external_eff")->Clone("EmEnWidth_7_external_eff_a");

h1=(TH1*)gROOT->Get("ntrkExist_1_external_eff")->Clone("ntrkExist_1_external_eff_a");
h1=(TH1*)gROOT->Get("ntrk_2_external_eff")->Clone("ntrk_2_external_eff_a");
h1=(TH1*)gROOT->Get("leadPt_3_external_eff")->Clone("leadPt_3_external_eff_a");
h1=(TH1*)gROOT->Get("nSlow_4_external_eff")->Clone("nSlow_4_external_eff_a");
h1=(TH1*)gROOT->Get("nIso_5_external_eff")->Clone("nIso_5_external_eff_a");
h1=(TH1*)gROOT->Get("nMatched_6_external_eff")->Clone("nMatched_6_external_eff_a");
h1=(TH1*)gROOT->Get("Q_7_external_eff")->Clone("Q_7_external_eff_a");
h1=(TH1*)gROOT->Get("sumPtCore_8_external_eff")->Clone("sumPtCore_8_external_eff_a");
h1=(TH1*)gROOT->Get("Et_trkEM_9_external_eff")->Clone("Et_trkEM_9_external_eff_a");

h1=(TH1*)gROOT->Get("etCalibEF_1_external_eff")->Clone("etCalibEF_1_external_eff_a");
h1=(TH1*)gROOT->Get("ntrkMinEF_2_external_eff")->Clone("ntrkMinEF_2_external_eff_a");
h1=(TH1*)gROOT->Get("ntrkMaxEF_3_external_eff")->Clone("ntrkMaxEF_3_external_eff_a");
h1=(TH1*)gROOT->Get("emradEF_4_external_eff")->Clone("emradEF_4_external_eff_a");
h1=(TH1*)gROOT->Get("isoFracEF_5_external_eff")->Clone("isoFracEF_5_external_eff_a");
h1=(TH1*)gROOT->Get("emFracEF_6_external_eff")->Clone("emFracEF_6_external_eff_a");
h1=(TH1*)gROOT->Get("leadPtEF_7_external_eff")->Clone("leadPtEF_7_external_eff_a");
h1=(TH1*)gROOT->Get("emEtEF_8_external_eff")->Clone("emEtEF_8_external_eff_a");
h1=(TH1*)gROOT->Get("massEF_9_external_eff")->Clone("massEF_9_external_eff_a");

 fil->Write();
 fil->Close();
 delete fil;
}


/*







TChain*chain=newTChain("eventTree")
oSampleTTP04(5107,chain)
oSampleTTP04(5862,chain)
oSampleTTP04(5189,chain)
.xcutStudy.cxx(chain,true)
.xplotAllEfficiencies.cxx(-1)

TChain*chain=newTChain("eventTree")
oSampleTTP04(5009,chain)
oSampleTTP04(5010,chain)
oSampleTTP04(5011,chain)
oSampleTTP04(5012,chain)
.xcutStudy.cxx(chain)

*/



