/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void saveAll( const char * filename = "ntuple/TTP12.1422252/5188/tmp.*", const char * outputFile = "validationAnalysis.root" ) {

  TFile * file = new TFile(outputFile, "recreate");
  file->Close();

  int type = 1; //1 - relative efficiencies for cutStudy and effTableMaker; 2 - absolute
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
  double crossSec = 0;
  double luminosity = 1E31;


  //const char * filename = "ntuple/TTP12.1422252/5188/tmp.*";

  cout << "Starting TTPValCutStudy. " << endl;
  TTPValCutStudy study(filename);
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
  study.update(outputFile);
  study.effSequencePlotsUpdate(outputFile);

  //effSequence plots for NONE offline matching
  TTPValCutStudy study2(filename);
  study2.setDebug(debug);
  study2.setNprong(nprong);
  study2.setMenu(menu);
  study2.setSignal(signal);
  study2.setType(type);
  study2.setOfflineType(TrigTauMCTauList::NONE);
  study2.setCutLevel(cutLevel);
  study2.setMenuVersion(menuVersion);
  study2.setChainType(chainType);
  study2.analyze();
  study2.effSequencePlotsUpdate(outputFile);

  TTPValCutStudy study3(filename);
  study3.setDebug(debug);
  study3.setNprong(nprong);
  study3.setMenu(menu2);
  study3.setSignal(signal);
  study3.setType(type);
  study3.setOfflineType(offlineType);
  study3.setCutLevel(cutLevel);
  study3.setMenuVersion(menuVersion);
  study3.setChainType(chainType);
  study3.analyze();
  study3.update(outputFile);
  study3.effSequencePlotsUpdate(outputFile);

  //effSequence plots for NONE offline matching
  TTPValCutStudy study4(filename);
  study4.setDebug(debug);
  study4.setNprong(nprong);
  study4.setMenu(menu2);
  study4.setSignal(signal);
  study4.setType(type);
  study4.setOfflineType(TrigTauMCTauList::NONE);
  study4.setCutLevel(cutLevel);
  study4.setMenuVersion(menuVersion);
  study4.setChainType(chainType);
  study4.analyze();
  study4.effSequencePlotsUpdate(outputFile);

  cout << "Starting TTPValMultiPlots. " << endl;
  TTPValMultiPlots plots(filename);
  plots.setDebug(debug);
  plots.setNprong(nprong);
  plots.setMenu(menu);
  plots.setSignal(signal);
  plots.setOfflineType(offlineType);
  plots.setCutLevel(cutLevel);
  plots.setMenuVersion(menuVersion);
  plots.setChainType(chainType);
  plots.analyze();
  plots.update(outputFile); 

  cout << "Starting TTPValDistribution." << endl;
  TTPValDistribution dist(filename);
  dist.setDebug(debug);
  dist.setMenu(menu);
  //dist.setSignal(signal);
  dist.setOfflineType(offlineType);
  dist.setCutLevel(cutLevel);
  dist.setMenuVersion(menuVersion);
  dist.setChainType(chainType);
  dist.analyze();
  dist.update(outputFile);

  cout << "Starting TTPValEffTableMaker." << endl;
  TTPValEffTableMaker maker(filename);
  maker.setDebug(debug);
  maker.setMenu(menu);
  maker.setSignal(signal);
  //maker.setType(type);
  maker.setCutLevel(cutLevel);
  maker.setMenuVersion(menuVersion);
  maker.setChainType(chainType);
  maker.analyze();
  maker.update(outputFile, type);

  cout << "Starting TTPValRateTable." << endl;
  TTPValRateTable table(filename);
  table.setDebug(debug);
  table.setNprong(nprong);
  table.setMenu(menu);
  table.setSignal(signal);
  table.setOfflineType(offlineType);
  table.setCutLevel(cutLevel);
  table.setMenuVersion(menuVersion);
  table.setChainType(chainType);
  table.setDetailedPrinting(detailedPrinting);
  table.analyze();
  table.update(outputFile, luminosity, crossSec);
 
  return;

}
