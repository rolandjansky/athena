/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void cutStudy()
{
   TrigTauFlags::TRIGGERMENU menu = TrigTauFlags::TAU25I;

  TChain * chain = new TChain("eventTree");
  oSampleTTP04(5107,chain);
  oSampleTTP04(5862,chain);
  oSampleTTP04(5189,chain);

  doSet(chain, menu, 0);
  doSet(chain, menu, 1);
  doSet(chain, menu, 3);

  TChain * chain1 = new TChain("eventTree");
  oSampleTTP04(5009,chain1);
  oSampleTTP04(5010,chain1);
  oSampleTTP04(5011,chain1);
  oSampleTTP04(5012,chain1);
  
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
  cutStudy(chain, menu, nEvent, nL1Sel, nL2Calo, nL2Sel, nEFSel, true, nprong, false, false, false);
  plotAllEfficiencies(-1,baseName);

  baseName = TString("signal_offline")+nprStr;
  cout << baseName<<endl;
  cutStudy(chain, menu, nEvent, nL1Sel, nL2Calo, nL2Sel, nEFSel, true, nprong, true, true, false);
  plotAllEfficiencies(-1,baseName);

  baseName = TString("signal_tauRec")+nprStr;
  cout << baseName<<endl;
  cutStudy(chain, menu, nEvent, nL1Sel, nL2Calo, nL2Sel, nEFSel, true, nprong, true, false, false);
  plotAllEfficiencies(-1,baseName);

  baseName = TString("signal_tau1p3p")+nprStr;
  cout << baseName<<endl;
  cutStudy(chain, menu, nEvent, nL1Sel, nL2Calo, nL2Sel, nEFSel, true, nprong, false, true, false);
  plotAllEfficiencies(-1,baseName );

  baseName = TString("signal_bothoffline")+nprStr;
  cout << baseName<<endl;
  cutStudy(chain, menu, nEvent, nL1Sel, nL2Calo, nL2Sel, nEFSel, true, nprong, true, true, true);
  plotAllEfficiencies(-1,baseName);
}


void cutStudy(TTree * tree, 
	      TrigTauFlags::TRIGGERMENU menu1,
	      int & nEvent , int & nL1Sel, 
	      int & nL2Calo, int &  nL2Sel, int &  nEFSel,
	      bool signal=false, bool nprong = 0, 
	      bool dotauRec=false, bool dotau1p3p=false, bool bothoffline=false )
{
  
  TrigTauHistogram::HISTTYPE histType=TrigTauHistogram::PTHIST;

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
  truthList.setMaxVisEta(2.5);
  truthList.setMinVisPt(menu1);

  if( nprong == 1 )  truthList.select1Prong();
  else  if( nprong == 3 )  truthList.select3Prong();

  truthList.setRecoMatchdR(0.2);
  bool checkOffline = true;
  // TAU1P3P, TAUREC, EITHERTAU, BOTHTAU
  TrigTauMCTauList::OFFLINESELECTION offlineType;
  if( dotauRec && dotau1p3p ) {
    if( bothoffline ) offlineType= TrigTauMCTauList::BOTHTAU;
    else              offlineType= TrigTauMCTauList::EITHERTAU; 
  }else if (dotauRec       ) offlineType= TrigTauMCTauList::TAUREC; 
  else  if (dotau1p3p      ) offlineType= TrigTauMCTauList::TAU1P3P;
  else                    checkOffline = false;
  
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
  trigMan1.setMenuVersion(TrigTauSelectionManager::MENU13030pre);
  trigMan1.setTauMenu(menu1);

  trigMan1.l1Sel.setHistDebug(histType);
  trigMan1.l2CalSel.setHistDebug(histType);
  trigMan1.l2TrkSel.setHistDebug(histType);
  trigMan1.efSel.setHistDebug(histType);


  //trigMan1.setL2CalOnly(false);
  bool checkL2Cal = true;


  // Loop over all events
  int nev = int(tree->GetEntries());
  for(int iev = 0; iev<nev; ++iev) {
    event->reset();
    tree->GetEvent(iev);

    // ============ Select type of events (e.g. signal) ================
    if( signal ) {
      truthList.fillPureList(&event->mcEvent);
      truthList.fillGoodList();
      if( checkOffline ) truthList.requireRecoMatch(*event, offlineType);
      if(  truthList.nMCTauGood() ==0 ) continue;

    //==================================================================
    }

    nEvent++;
       

    // Analyze this event
    analyzer.chainList.fillChainList(event);

    bool pass = false;


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
	      if( truth->DeltaR(*l1)<0.2 ) matchTruth=true;
	    }
	  if( !matchTruth ) continue;
	}

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
	      if( truth->DeltaR(*l2)<0.2 ) matchTruth=true;
	    }
	  if( !matchTruth ) continue;
	}

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
	      if( truth->DeltaR(*l2)<0.2 ) matchTruth=true;
	    }
	  if( !matchTruth ) continue;
	}

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
	      if( truth->DeltaR(*ef)<0.2 ) matchTruth=true;
	    }
	  if( !matchTruth ) continue;
	}

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
  trigMan1.l1Sel.histManager()->plotAllEfficiencies();
  trigMan1.l2CalSel.histManager()->plotAllEfficiencies();
  trigMan1.l2TrkSel.histManager()->plotAllEfficiencies();
  trigMan1.efSel.histManager()->plotAllEfficiencies();

  
  cout << "N event " << nEvent << endl
       << " L1    " << nL1Sel    << " "  << float(nL1Sel)/float(nEvent)  << " "  << float(nL1Sel)/float(nEvent)<<   endl
       << " L2Cal " << nL2Calo   << " "  << float(nL2Calo)/float(nEvent) << " "  << float(nL2Calo)/float(nL1Sel)<< endl
       << " L2Trk " << nL2Sel    << " "  << float(nL2Sel)/float(nEvent)  << " "  << float(nL2Sel)/float(nL2Calo)<< endl
       << " EF    " << nEFSel    << " "  << float(nEFSel)/float(nEvent)  << " "  << float(nEFSel)/float(nL2Sel)<< endl;

  cout << " \& "<< nEvent << " & " << setprecision(3)
       <<  float(nL1Sel)/float(nEvent)<< " & " << setprecision(3)
       <<  float(nL2Calo)/float(nL1Sel)<< " & "<< setprecision(3)
       <<  float(nL2Sel)/float(nL2Calo) << " & "<< setprecision(3)
       <<  float(nEFSel)/float(nL2Sel)<< " \\ \n";


  delete event;
  //delete tree;
  // delete file;

  cout << " Use menu " << menu1.menuName(menu1) << endl;


  return;
}


int plotAllEfficiencies(int select == 3, TString baseFileName = "")
{

  TString name = "_pt_eff";

  TString histList[10];
  TString labels[10];
  int index = 0;

  gStyle->SetPadGridY(true);
  gStyle->SetPadGridX(true);


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

  }else if ( select == -1 ){
    if( baseFileName == "" )
      {
	cout << " Please use .x plotAllEfficiencies.cxx(-1, baseFileName)\n";
	return 1;
      }
    TCanvas * c1 = (TCanvas * )gROOT->Get("c1");
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

  // set title
  TH1 * h1 = (TH1*)gROOT->Get(histList[0]);
  if( h1 == 0 ) {cout << " histogram " << histList[0] << " not found \n"; return 1;}
  if(      select == 0 ) h1->SetTitle(baseFileName+ " L1 selection");
  else if( select == 1 ) h1->SetTitle(baseFileName+ " L2 Calo selection");
  else if( select == 2 ) h1->SetTitle(baseFileName+ " L2 Trk selection");
  else if( select == 3 ) h1->SetTitle(baseFileName+ " EF selection");
  
  

  // draw
  
  TH1F * hist = (TH1F*)gROOT->Get(histList[0]);
  //float xmin = hist->GetXaxis()->GetXmin();
  TLegend *legend = new TLegend(0.5,0.2,0.7,0.5);
  
  hist->Draw(); legend->AddEntry(hist, labels[0], "l");
  for(unsigned int ihist = 1; ihist < index; ++ihist)
    {
      hist = (TH1F*)gROOT->Get(histList[ihist]);
      hist->Draw("same"); 
      legend->AddEntry(hist, labels[ihist], "l");
    }
  legend->Draw();
  return 0;
}




/*
TChain * chain = new TChain("eventTree")
oSampleTTP04(5107,chain)
oSampleTTP04(5862,chain)
oSampleTTP04(5189,chain)
.x cutStudy.cxx(chain, true)
.x plotAllEfficiencies.cxx(-1)

TChain * chain = new TChain("eventTree")
oSampleTTP04(5009,chain)
oSampleTTP04(5010,chain)
oSampleTTP04(5011,chain)
oSampleTTP04(5012,chain)
.x cutStudy.cxx(chain)

*/



