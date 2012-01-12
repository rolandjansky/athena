/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void printCSC( TString outputFile = "csc_plots.eps" ) {

  const int Nbackground = 4;

  //define signal and background samples:
  TString signalSample[2];
  TString background[Nbackground];
 
//   signalSample[0] = "../data/TTP12.1500/5188/tmp.yjiXu19920.misal1_mc12.005188.A3_Ztautau_filter.digit.RDO.v12000605RDO.013377._00479.pool.root.1.TTP12.1500.root" ;
//   signalSample[1] = "../data/TTP12.1500/5107/tmp.ZEjNlX6375.misal1_csc11.005107.pythia_Wtauhad.digit.RDO.v12003103_tid004537._06851.pool.root.1.TTP12.1500.root" ;
  
//   background[0] = "../data/TTP12.1500/5009/tmp.ZDWMUG1550.misal1_csc11.005009.J0_pythia_jetjet.digit.RDO.v12003103RDO.016351._24844.pool.root.1.TTP12.1500.root" ;
//   background[1] = "../data/TTP12.1500/5010/tmp.Yrfzq28506.misal1_csc11.005010.J1_pythia_jetjet.digit.RDO.v12003103_tid003851._01529.pool.root.TTP12.1500.root" ;
//   background[2] = "../data/TTP12.1500/5011/tmp.ZCbBCB3815.misal1_csc11.005011.J2_pythia_jetjet.digit.RDO.v12003103RDO.016353._08634.pool.root.1.TTP12.1500.root" ;
//   background[3] = "../data/TTP12.1500/5012/tmp.yghkI19797.misal1_csc11.005012.J3_pythia_jetjet.digit.RDO.v12003103RDO.016364._08195.pool.root.1.TTP12.1500.root" ;
//   background[4] = "../data/TTP12.1500/5013/tmp.Ycnkq30838.misal1_csc11.005013.J4_pythia_jetjet.digit.RDO.v12003103_tid004016._00061.pool.root.1.TTP12.1500.root" ;
//   background[5] = "../data/TTP12.1500/5014/tmp.zGIrnA9890.misal1_csc11.005014.J5_pythia_jetjet.digit.RDO.v12003103RDO.018677._09157.pool.root.1.TTP12.1500.root" ;
 
  signalSample[0] = "../data/TTP13/5188/tmp.*" ;
  signalSample[1] = "../data/TTP12.1500/5107/tmp.*" ;
  
  background[0] = "../data/TTP13/5009/tmp.*" ;
  background[1] = "../data/TTP13/5010/tmp.*" ;
  background[2] = "../data/TTP13/5011/tmp.*" ;
  background[3] = "../data/TTP13/5012/tmp.*" ;
  //background[4] = "../data/TTP12.1500/5013/tmp.*" ; 
  //background[5] = "../data/TTP12.1500/5014/tmp.*" ; 

  double background_xsection[Nbackground];
  background_xsection[0] = 17.6E-27 ;
  background_xsection[1] = 1.38E-27 ;
  background_xsection[2] = 93.3E-30 ;
  background_xsection[3] = 5.88E-30 ;
  //background_xsection[4] = 308E-33  ;
  //background_xsection[5] = 12.5E-33 ;

  double luminosity = 1E31;

  int type = 1; //1 - relative efficiencies for cutStudy ; 2 - absolute
  bool maker_type = 0;
  bool debug = false;
  bool signal = true;
  int nprong = 0; 
  double maxVisEta = 2.5;
  double minLeadingPt = 6.0;
  TrigTauFlags::TRIGGERMENU menu = TrigTauFlags::TAU16I;
  TrigTauFlags::TRIGGERMENU menu2 = TrigTauFlags::TAU50;
  TrigTauSelectionManager::CUTLEVEL cutLevel = TrigTauSelectionManager::LOOSE;
  TrigTauMCTauList::OFFLINESELECTION offlineType =  TrigTauMCTauList::EITHERTAU;
  TrigTauSelectionManager::MENUVERSION menuVersion = TrigTauSelectionManager::MENU1400;
  TrigTauFlags::HLTAUTHOR chainType = TrigTauFlags::IDSCAN;
  bool detailedPrinting = false;

  TPostScript ps(outputFile);

  TTPValCutStudy study(signalSample[0]);
  study.setDebug(debug);
  study.setNprong(nprong);
  study.setMenu(menu);
  study.setSignal(signal);
  study.setType(type);
  study.setOfflineType(offlineType);
  study.setCutLevel(cutLevel);
  study.setMenuVersion(menuVersion);
  study.setChainType(chainType);
  study.analyze();
  study.effSequencePlotsPrintCSC(ps);
 
  //effSequence plots for menu2
  TTPValCutStudy study2(signalSample[0]);
  study2.setDebug(debug);
  study2.setNprong(nprong);
  study2.setMenu(menu2);
  study2.setSignal(signal);
  study2.setType(type);
  study2.setOfflineType(offlineType);
  study2.setCutLevel(cutLevel);
  study2.setMenuVersion(menuVersion);
  study2.setChainType(chainType);
  study2.analyze();
  study2.effSequencePlotsPrintCSC(ps);

  cout << "Starting TTPValMultiPlots for " << signalSample[0]  << endl;
  TTPValMultiPlots plots(signalSample[0]);
  plots.setDebug(debug);
  plots.setNprong(nprong);
  plots.setMenu(menu);
  plots.setSignal(signal);
  plots.setOfflineType(offlineType);
  plots.setCutLevel(cutLevel);
  plots.setMenuVersion(menuVersion);
  plots.setChainType(chainType);
  plots.analyze();
  plots.printCSC(ps);  
 
  cout << "Starting TTPValLevelCut for " << signalSample[0]  << endl;
  TTPValLevelCut cuts(signalSample[0]);
  cuts.setDebug(debug);
  cuts.setNprong(nprong);
  cuts.setMenu(menu);
  cuts.setSignal(signal);
  cuts.setOfflineType(offlineType);
  cuts.setMenuVersion(menuVersion);
  cuts.setChainType(chainType);
  cuts.analyze();
  cuts.printCSC(ps);

  cout << "Starting TTPValLevelCut for " << signalSample[0]  << endl;
  TTPValLevelCut cuts(signalSample[0]);
  cuts.setDebug(debug);
  cuts.setNprong(nprong);
  cuts.setMenu(menu2);
  cuts.setSignal(signal);
  cuts.setOfflineType(offlineType);
  cuts.setMenuVersion(menuVersion);
  cuts.setChainType(chainType);
  cuts.analyze();
  cuts.printCSC(ps);
 
  cout << "Starting TTPValEffTableMaker for file " << signalSample[0] << endl;
  TTPValEffTableMaker maker(signalSample[0]);
  maker.setDebug(debug);
  maker.setMenu(menu);
  maker.setSignal(signal);
  maker.setCutLevel(cutLevel);
  maker.setMenuVersion(menuVersion);
  maker.setChainType(chainType);
  maker.analyze();
  maker.printCSC(ps, maker_type);

  vector<double> counter;
  counter.push_back(0);
  counter.push_back(0);
  counter.push_back(0);
  counter.push_back(0);
 
  for (int i =0; i<Nbackground; i++) {

   cout << "Starting TTPValRateTable for file " << background[i] << endl;
    TTPValRateTable table((char)background[i]);
    table.reset();
    table.setDebug(debug);
    table.setNprong(nprong);
    table.setMenu(menu);
    table.setSignal(false);
    table.setOfflineType(offlineType);
    table.setCutLevel(cutLevel);
    table.setMenuVersion(menuVersion);
    table.setChainType(chainType);
    table.setDetailedPrinting(detailedPrinting);
    table.analyze();
    table.printCSC(ps, luminosity, background_xsection[i], counter);
  }

  //print counter;
  TH1F * Rates = new TH1F("Total Background Rate", "Total Background Rates", 10, 0, 10);

  Rates->SetBinContent(1, counter[0]);
  Rates->GetXaxis()->SetBinLabel(1, "L1 Rate");

  Rates->SetBinContent(2, counter[1]);
  Rates->GetXaxis()->SetBinLabel(2, "L2Cal Rate");

  Rates->SetBinContent(3, counter[2]);
  Rates->GetXaxis()->SetBinLabel(2, "L2Trk Rate");

  Rates->SetBinContent(4, counter[3]);
  Rates->GetXaxis()->SetBinLabel(3, "EF Rate");

  Rates->LabelsOption("u","X");

  TCanvas * c1 = (TCanvas *)gROOT->Get("c1");
  if( c1 ) delete c1;
  c1 = new TCanvas("c1","",10,10,800,500);

  ps.On();

  Rates->Draw();
  c1->Update();
  ps.Close();

  cout << "Total Rates for L1, L2Calo, L2Trk and EF are " << counter[0] << ", " << counter[1] << ", " << counter[2] << ", " << counter[3] << endl;     
   
  return;

}
