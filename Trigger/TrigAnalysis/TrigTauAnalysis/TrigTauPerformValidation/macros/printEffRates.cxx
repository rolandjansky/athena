/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void printEffRates( TString outputFile = "RatesEfficiencies.txt" ) {

  const int Nbackground = 7;
  const int nMenus = 7;

  //define signal and background samples:
  TString signalSample[1];
  TString background[Nbackground];
  TrigTauFlags::TRIGGERMENU theMenu[nMenus];  
 
 //  signalSample[0] = "../data/TTP12.1500/5188/tmp.yjiXu19920.misal1_mc12.005188.A3_Ztautau_filter.digit.RDO.v12000605RDO.013377._00479.pool.root.1.TTP12.1500.root" ;
//   signalSample[1] = "../data/TTP12.1500/5107/tmp.ZEjNlX6375.misal1_csc11.005107.pythia_Wtauhad.digit.RDO.v12003103_tid004537._06851.pool.root.1.TTP12.1500.root" ;
  
//   background[0] = "../data/TTP12.1500/5009/tmp.ZDWMUG1550.misal1_csc11.005009.J0_pythia_jetjet.digit.RDO.v12003103RDO.016351._24844.pool.root.1.TTP12.1500.root" ;
//   background[1] = "../data/TTP12.1500/5010/tmp.Yrfzq28506.misal1_csc11.005010.J1_pythia_jetjet.digit.RDO.v12003103_tid003851._01529.pool.root.TTP12.1500.root" ;
//   background[2] = "../data/TTP12.1500/5011/tmp.ZCbBCB3815.misal1_csc11.005011.J2_pythia_jetjet.digit.RDO.v12003103RDO.016353._08634.pool.root.1.TTP12.1500.root" ;
//   background[3] = "../data/TTP12.1500/5012/tmp.yghkI19797.misal1_csc11.005012.J3_pythia_jetjet.digit.RDO.v12003103RDO.016364._08195.pool.root.1.TTP12.1500.root" ;
//   background[4] = "../data/TTP12.1500/5013/tmp.Ycnkq30838.misal1_csc11.005013.J4_pythia_jetjet.digit.RDO.v12003103_tid004016._00061.pool.root.1.TTP12.1500.root" ;
  // background[5] = "../data/TTP12.1500/5014/tmp.zGIrnA9890.misal1_csc11.005014.J5_pythia_jetjet.digit.RDO.v12003103RDO.018677._09157.pool.root.1.TTP12.1500.root" ;
 
  signalSample[0] = "../data/TTP12b.142255/106052/mc*" ;
  //signalSample[1] = "../data/TTP12b.142255/5107/tmp.*" ;
  
  background[0] = "../data/TTP12b.142255/105009/mc*" ;
  background[1] = "../data/TTP12b.142255/105010/mc*" ;
  background[2] = "../data/TTP12b.142255/105011/mc*" ;
  background[3] = "../data/TTP12b.142255/105012/mc*" ;
  background[4] = "../data/TTP12b.142255/105013/mc*" ; 
  background[5] = "../data/TTP12b.142255/105014/mc*" ; 
  background[6] = "../data/TTP12b.142255/105015/mc*" ;

  double background_xsection[Nbackground];
  background_xsection[0] = 11.7E-27 ;
  background_xsection[1] = 0.8668E-27 ;
  background_xsection[2] = 56.01E-30 ;
  background_xsection[3] = 3.28E-30 ;
  background_xsection[4] = 151.6E-33  ;
  background_xsection[5] = 5.112E-33 ;
  background_xsection[5] = 1.119E-34 ;

  theMenu[0] = TrigTauFlags::TAU12;
  theMenu[1] = TrigTauFlags::TAU16I;
  theMenu[2] = TrigTauFlags::TAU20I;
  theMenu[3] = TrigTauFlags::TAU29I;
  theMenu[4] = TrigTauFlags::TAU38I;
  theMenu[5] = TrigTauFlags::TAU50;
  theMenu[6] = TrigTauFlags::TAU84;

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

  for (unsigned int j = 0; j<nMenus; ++j) {

    cout << "Starting TTPValRateTable for file " << signalSample[0] << endl;
    TTPValRateTable table(signalSample[0]);
    table.reset();
    table.setDebug(debug);
    table.setNprong(nprong);
    table.setMenu(theMenu[j]);
    table.setSignal(true);
    table.setOfflineType(offlineType);
    table.setCutLevel(cutLevel);
    table.setMenuVersion(menuVersion);
    table.setChainType(chainType);
    table.setDetailedPrinting(detailedPrinting);
    table.analyze();
    table.print(outputFile, true);

    for (int i =0; i<Nbackground; i++) {

      cout << "Starting TTPValRateTable for file " << background[i] << endl;
      TTPValRateTable table((char)background[i]);
      table.reset();
      table.setDebug(debug);
      table.setNprong(nprong);
      table.setMenu(theMenu[j]);
      table.setSignal(false);
      table.setOfflineType(offlineType);
      table.setCutLevel(cutLevel);
      table.setMenuVersion(menuVersion);
      table.setChainType(chainType);
      table.setDetailedPrinting(detailedPrinting);
      table.analyze();
      table.print(luminosity, background_xsection[i], outputFile, true);
    }
  }
}
