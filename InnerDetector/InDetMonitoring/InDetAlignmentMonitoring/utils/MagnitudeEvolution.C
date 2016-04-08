/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////
// MagnitudeEvolution
//////////////////////////////////////////
//
#include "MagnitudeEvolution.h"


//////////////////////////////////////////
void MagnitudeEvolution()
{
  ME_init();
  ME_loop();
  ME_finalize();
  return;
}

//////////////////////////////////////////
void ME_init()
{
  ME_welcomeLogo();
  ME_clear();
  ME_getMonitoringFileList();
  ME_prepareEvolutionHistos();
  ME_prepareHistosPerStave();
  ME_prepareOutputTextFile();
  
  return;
}

//////////////////////////////////////////
void ME_loop()
{
  std::cout << " ** Going to loop on " << me_monitoringFileList.size() << " files " << std::endl;
  
  // loop on monitoring files
  for (int thisfile=0; thisfile < (int) me_monitoringFileList.size(); thisfile++) {
    ME_prepareLoop();
    std::cout << " ** processing file " << thisfile 
	      << " " << me_monitoringFileList.at(thisfile).Data() 
	      << "  #Run: " << me_runNumber.at(thisfile) << std::endl;
    ME_processFile(thisfile);
  }
  return;
}
//////////////////////////////////////////
void ME_finalize()
{
  ME_plotEvolutionHistos();
  ME_plotHistosPerStave();
  ME_writeOutputTextFile();
  ME_plotDeltaBx();
  ME_goodbye();
  return;
}

//////////////////////////////////////////
void ME_getMonitoringFileList()
{
  std::cout << " ** retrieving monitoring file list ... " << std::endl;

  // define input files. Lamentably this is hard coded
  // me_monitoringFileList.push_back("/afs/cern.ch/work/m/martis/public/athena/20.1.6.4/runTest/Iter0_278727/collisions/TotalMonitoring.root");
  
  /*
  me_monitoringFileList.push_back("/afs/cern.ch/work/m/martis/public/athena/20.1.6.4/runTest/Iter0_278748/collisions/TotalMonitoring.root");
  me_monitoringFileList.push_back("/afs/cern.ch/work/m/martis/public/athena/20.1.6.4/runTest/Iter0_278880/collisions/TotalMonitoring.root");
  me_monitoringFileList.push_back("/afs/cern.ch/work/m/martis/public/athena/20.1.6.4/runTest/Iter0_278912/collisions/TotalMonitoring.root");
  me_monitoringFileList.push_back("/afs/cern.ch/work/m/martis/public/athena/20.1.6.4/runTest/Iter0_278968/collisions/TotalMonitoring.root");
  me_monitoringFileList.push_back("/afs/cern.ch/work/m/martis/public/athena/20.1.6.4/runTest/Iter0_279169/collisions/TotalMonitoring.root");
  me_monitoringFileList.push_back("/afs/cern.ch/work/m/martis/public/athena/20.1.6.4/runTest/Iter0_279259/collisions/TotalMonitoring.root");
  me_monitoringFileList.push_back("/afs/cern.ch/work/m/martis/public/athena/20.1.6.4/runTest/Iter0_279279/collisions/TotalMonitoring.root");
  me_monitoringFileList.push_back("/afs/cern.ch/work/m/martis/public/athena/20.1.6.4/runTest/Iter0_279284/collisions/TotalMonitoring.root");
  me_monitoringFileList.push_back("/afs/cern.ch/work/m/martis/public/athena/20.1.6.4/runTest/Iter0_279345/collisions/TotalMonitoring.root");
  me_monitoringFileList.push_back("/afs/cern.ch/work/m/martis/public/athena/20.1.6.4/runTest/Iter0_279598/collisions/TotalMonitoring.root");
  me_monitoringFileList.push_back("/afs/cern.ch/work/m/martis/public/athena/20.1.6.4/runTest/Iter0_279685/collisions/TotalMonitoring.root");
  me_monitoringFileList.push_back("/afs/cern.ch/work/m/martis/public/athena/20.1.6.4/runTest/Iter0_279764/collisions/TotalMonitoring.root");
  me_monitoringFileList.push_back("/afs/cern.ch/work/m/martis/public/athena/20.1.6.4/runTest/Iter0_279813/collisions/TotalMonitoring.root");
  me_monitoringFileList.push_back("/afs/cern.ch/work/m/martis/public/athena/20.1.6.4/runTest/Iter0_279867/collisions/TotalMonitoring.root");
  me_monitoringFileList.push_back("/afs/cern.ch/work/m/martis/public/athena/20.1.6.4/runTest/Iter0_279928/collisions/TotalMonitoring.root");
  me_monitoringFileList.push_back("/afs/cern.ch/work/m/martis/public/athena/20.1.6.4/runTest/Iter0_279932/collisions/TotalMonitoring.root");
  me_monitoringFileList.push_back("/afs/cern.ch/work/m/martis/public/athena/20.1.6.4/runTest/Iter0_279984/collisions/TotalMonitoring.root");
  me_monitoringFileList.push_back("/afs/cern.ch/work/m/martis/public/athena/20.1.6.4/runTest/Iter0_280231/collisions/TotalMonitoring.root");
  me_monitoringFileList.push_back("/afs/cern.ch/work/m/martis/public/athena/20.1.6.4/runTest/Iter0_280273/collisions/TotalMonitoring.root");

  me_monitoringFileList.push_back("/afs/cern.ch/work/m/martis/public/athena/20.1.6.4/runTest/Iter0_280319/collisions/TotalMonitoring.root");
  me_monitoringFileList.push_back("/afs/cern.ch/work/m/martis/public/athena/20.1.6.4/runTest/Iter0_280368/collisions/TotalMonitoring.root");
  me_monitoringFileList.push_back("/afs/cern.ch/work/m/martis/public/athena/20.1.6.4/runTest/Iter0_280464/collisions/TotalMonitoring.root");
  me_monitoringFileList.push_back("/afs/cern.ch/work/m/martis/public/athena/20.1.6.4/runTest/Iter0_280500/collisions/TotalMonitoring.root");
  me_monitoringFileList.push_back("/afs/cern.ch/work/m/martis/public/athena/20.1.6.4/runTest/Iter0_280614/collisions/TotalMonitoring.root");
  
  //
  // -- from now on, monitoring files come with LB analysis histograms
  //
  me_monitoringFileList.push_back("/afs/cern.ch/work/m/martis/public/athena/20.1.6.4/runTest/Iter0_280673/collisions/TotalMonitoring.root");
  me_monitoringFileList.push_back("/afs/cern.ch/work/m/martis/public/athena/20.1.6.4/runTest/Iter0_280862/collisions/TotalMonitoring.root");
 
  me_monitoringFileList.push_back("/afs/cern.ch/work/m/martis/public/athena/20.1.6.4/runTest/Iter0_280950/collisions/TotalMonitoring.root");
  */
  //me_monitoringFileList.push_back("/afs/cern.ch/work/m/martis/public/athena/20.1.6.4/runTest/Iter0_280977/collisions/TotalMonitoring.root"); me_runNumber.push_back(280977);
  /*
    me_monitoringFileList.push_back("/afs/cern.ch/work/m/martis/public/athena/20.1.6.4/runTest/Iter0_281074/collisions/TotalMonitoring.root");
  */
  // me_monitoringFileList.push_back("/afs/cern.ch/work/m/martis/public/athena/20.1.6.4/runTest/Iter0_281411/collisions/TotalMonitoring.root"); me_runNumber.push_back(281411);
  //me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00281411/data15_13TeV.00281411.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00281411.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(281411);
  //me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00281411/data15_13TeV.00281411.calibration_IDTracks.idalignmerge.ROOT_MON.Iter1.c0/data15_13TeV.00281411.calibration_IDTracks.idalignmerge.ROOT_MON.Iter1.c0._0001.1"); me_runNumber.push_back(281411);
  // me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00281411/data15_13TeV.00281411.calibration_IDTracks.idalignmerge.ROOT_MON.Iter2.c0/data15_13TeV.00281411.calibration_IDTracks.idalignmerge.ROOT_MON.Iter2.c0._0001.1"); me_runNumber.push_back(281411);
  // me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00281411/data15_13TeV.00281411.calibration_IDTracks.idalignmerge.ROOT_MON.Iter3.c0/data15_13TeV.00281411.calibration_IDTracks.idalignmerge.ROOT_MON.Iter3.c0._0001.1"); me_runNumber.push_back(281411);
  //
  
  /*
  // Runs with no run number in the heading
  me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00279598/data15_13TeV.00279598.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00279598.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(279598);
  me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00279685/data15_13TeV.00279685.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00279685.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(279685);
 me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00279764/data15_13TeV.00279764.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00279764.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(279764);
  me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00279813/data15_13TeV.00279813.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00279813.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(279813);
  me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00279867/data15_13TeV.00279867.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00279867.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(279867);
  me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00279932/data15_13TeV.00279932.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00279932.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(279932);
  me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00279984/data15_13TeV.00279984.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00279984.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(279984);
  me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00280231/data15_13TeV.00280231.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00280231.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(280231);
  me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00280319/data15_13TeV.00280319.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00280319.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(280319);
  me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00280368/data15_13TeV.00280368.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00280368.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(280368);
  me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00280423/data15_13TeV.00280423.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00280423.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(280423);
  me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00280464/data15_13TeV.00280464.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00280464.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(280464);
  me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00280500/data15_13TeV.00280500.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00280500.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(280500);
  me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00280520/data15_13TeV.00280520.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00280520.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(280520);
  me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00280614/data15_13TeV.00280614.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00280614.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(280614);
  me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00280673/data15_13TeV.00280673.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00280673.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(280673);
  me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00280753/data15_13TeV.00280753.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00280753.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(280753);
  me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00280853/data15_13TeV.00280853.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00280853.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(280853);
  me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00280862/data15_13TeV.00280862.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00280862.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(280862);
  me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00280950/data15_13TeV.00280950.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00280950.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(280950);
  me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00280977/data15_13TeV.00280977.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00280977.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(280977);
  me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00281070/data15_13TeV.00281070.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00281070.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(281070);
  me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00281074/data15_13TeV.00281074.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00281074.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(281074);
  me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00281075/data15_13TeV.00281075.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00281075.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(281075);
  // end of list of runs without run number in the heading 

 me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00281130/data15_13TeV.00281130.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00281130.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(281130);
 me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00281143/data15_13TeV.00281143.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00281143.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(281143);
 me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00281317/data15_13TeV.00281317.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00281317.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(281317);
 me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00281381/data15_13TeV.00281381.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00281381.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(281381);
 me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00281385/data15_13TeV.00281385.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00281385.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(281385);
 me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00281411/data15_13TeV.00281411.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00281411.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(281411);
  
  me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00282625/data15_13TeV.00282625.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00282625.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(282625);
  me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00282631/data15_13TeV.00282631.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00282631.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(282631);
  me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00282712/data15_13TeV.00282712.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00282712.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(282712);
  me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00282784/data15_13TeV.00282784.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00282784.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(282784);
  me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00282992/data15_13TeV.00282992.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00282992.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(282992);
  me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00283074/data15_13TeV.00283074.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00283074.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(283074);
  me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00283155/data15_13TeV.00283155.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00283155.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(283155);
  me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00283270/data15_13TeV.00283270.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00283270.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(283270);
  me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00283429/data15_13TeV.00283429.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00283429.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(283429);
  me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00283608/data15_13TeV.00283608.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00283608.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(283608);
  me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00283780/data15_13TeV.00283780.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00283780.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(283780);
  me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00284006/data15_13TeV.00284006.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00284006.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(284006);
 
 me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00284154/data15_13TeV.00284154.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00284154.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(284154);
  me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00284213/data15_13TeV.00284213.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00284213.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(284213);
  me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00284285/data15_13TeV.00284285.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00284285.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(284285);
  me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00284420/data15_13TeV.00284420.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00284420.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(284420);
 me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00284427/data15_13TeV.00284427.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00284427.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(284427);
  me_monitoringFileList.push_back("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data15_13TeV/calibration_IDTracks/00284484/data15_13TeV.00284484.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0/data15_13TeV.00284484.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001.1"); me_runNumber.push_back(284484);
  */
  //
  //me_monitoringFileList.push_back("/afs/cern.ch/work/m/martis/public/athena/20.1.6.4/runTest/Iter0_280319_Part11/collisions/TotalMonitoring.root");
  //me_monitoringFileList.push_back("/afs/cern.ch/work/m/martis/public/athena/20.1.6.4/runTest/Iter0_280319_Part12/collisions/TotalMonitoring.root");
  //me_monitoringFileList.push_back("/afs/cern.ch/work/m/martis/public/athena/20.1.6.4/runTest/Iter0_280319_Part13/collisions/TotalMonitoring.root");
  //me_monitoringFileList.push_back("/afs/cern.ch/work/m/martis/public/athena/20.1.6.4/runTest/Iter0_280319_Part14/collisions/TotalMonitoring.root");
  //me_monitoringFileList.push_back("/afs/cern.ch/work/m/martis/public/athena/20.1.6.4/runTest/Iter0_280319_Part15/collisions/TotalMonitoring.root");
  //me_monitoringFileList.push_back("/afs/cern.ch/work/m/martis/public/athena/20.1.6.4/runTest/Iter0_280319_Part06/collisions/TotalMonitoring.root");

  //me_monitoringFileList.push_back("/afs/cern.ch/work/m/martis/public/athena/BowTest/runTest/Iter0_284213_BX_25NS/collisions/TotalMonitoring.root"); me_runNumber.push_back(284213);
  //me_monitoringFileList.push_back("/afs/cern.ch/work/m/martis/public/athena/BowTest/runTest/Iter2_284213_BX_25NS/collisions/TotalMonitoring.root"); me_runNumber.push_back(284213);
  //me_monitoringFileList.push_back("/afs/cern.ch/work/m/martis/public/athena/BowTest/runTest/Iter1_L16_284213_LB_301_400/collisions/TotalMonitoring.root"); me_runNumber.push_back(284213);  
  //me_monitoringFileList.push_back("/afs/cern.ch/work/m/martis/public/athena/BowTest/runTest/Iter1_L16_284213_LB_301_400/collisions/TotalMonitoring.root"); me_runNumber.push_back(284213); 
  //me_monitoringFileList.push_back("/afs/cern.ch/work/m/martis/public/athena/BowTest/runTest/Iter0_284213_GEO15/collisions/TotalMonitoring.root"); me_runNumber.push_back(284213);

  /* INPUT from L11 & L16 IDAlignment for 2015 data reprocessing */ 
  me_monitoringFileList.push_back("/afs/cern.ch/work/m/martis/public/athena/BowTest/runTest/Iter0_280423_L11/collisions/TotalMonitoring.root"); me_runNumber.push_back(280423);


  /*
  // now, for all files, extract run number... this method will depend on the way the file name is given  
  for (int thisfile=0; thisfile < (int) me_monitoringFileList.size(); thisfile++) {
    me_runNumber.push_back(ME_extractRunNumber(thisfile));
  }
  */

  std::cout << " ** monitoring file list contains " << me_monitoringFileList.size() << " files " << std::endl;
  return;
}

//////////////////////////////////////////
void ME_welcomeLogo()
{
  std::cout << std::endl;
  std::cout << "          +---------------------------------------+" << std::endl;
  std::cout << "          | IBL bowing magnitude evolution by run |" << std::endl;
  std::cout << "          +---------------------------------------+" << std::endl;
  std::cout << std::endl;
  
  return;
}

//////////////////////////////////////////
void ME_goodbye()
{
  std::cout << std::endl << std::endl;
  std::cout << "          +---------------------------------------+" << std::endl;
  std::cout << "          |       GAME OVER     Insert coin       |" << std::endl;
  std::cout << "          +---------------------------------------+" << std::endl;
  std::cout << std::endl;
  
  return;
}

//////////////////////////////////////////
void ME_clear()
{
  // clear the vectors data (just in case) 
  me_monitoringFileList.clear();
  me_runNumber.clear();

  return;
}

//////////////////////////////////////////
void ME_prepareOutputTextFile()
{
  if (WRITETEXTOUTPUT) {
    // open output text file (overwrite existing file)
    me_outputTextFile.open ("MagnitudeEvolutionByRun.txt");
  }
  return;
}

//////////////////////////////////////////
int ME_extractRunNumber(int fileid)
{
  int myrunnumber = 100000; // default initial value 

  bool runnumberok = false;
  bool debug = false;
  if (debug) std::cout << " ** extracting run number ... " << std::endl;

  int method = RUNNUMBEREXTRACTIONMODE; // this is hard coded. depending on the method the user must change the value to select the proper extraction method
  int startingpoint = 0;
  
  switch (method) {
  case 0:
    startingpoint = me_monitoringFileList.at(fileid).Index("Iter0_");
    myrunnumber = ((TString) me_monitoringFileList.at(fileid)(startingpoint+6,6)).Atoi();
    if (debug) std::cout << " ** starting point for run number: " << startingpoint << " --> " << ((TString) me_monitoringFileList.at(fileid)(startingpoint+6,6)) << std::endl;
    runnumberok = true;
    break;
  default:
    runnumberok = false;
    break;
  }
  
  // in case we don't know how to get the run number, return a default one
  if (!runnumberok) {
    myrunnumber += fileid;
  } 

  if (debug) std::cout << " ** run number = " << myrunnumber << std::endl;

  return myrunnumber;
}

//////////////////////////////////////////
void ME_processFile(int fileid)
{
  bool debug = false;
  // open the monitoring file
  me_currentMonitoringFile = TFile::Open(me_monitoringFileList.at(fileid).Data());

  // retrieve the IBL 3D residuals histo: residuals (z) vs module phi (y) and module eta (x)
  TString h_path = "run_"; 
  h_path += me_runNumber.at(fileid); // add run number

  h_path.Clear(); // --> for monitoring files without run number in the path 

  //h_path.Append("/IDAlignMon/InDetTrackParticles_AlignSel/Residuals/pix_b0_xresvsmodetaphi_3d");
  //h_path.Append("/IDAlignMon/AlignTracks_all/Residuals/pix_b0_xresvsmodetaphi_3d");
  h_path.Append("/IDAlignMon/");
  h_path += me_trackCollection;
  h_path.Append("/Residuals/pix_b0_xresvsmodetaphi_3d");
  if (debug) std::cout << " ** going to fetch TH3 histo: " << h_path.Data() << std::endl;
  //TH3F* h_xresiduals3d = (TH3F*) me_currentMonitoringFile->Get(h_path.Data());

  TH3F* h_xresiduals3d = ME_getIBLResiduals3DHisto (fileid);

  
  if (h_xresiduals3d == NULL) std::cout << " ** WARNING ** " << h_path.Data() << " not found !!!" << std::endl;   
  if (debug && h_xresiduals3d != NULL) std::cout << " ** 3D histogram with residuals captured: " << h_xresiduals3d->GetName() << std::endl; 

  if (FULLOUTPUT) {
    TString cname("IBLXres_Run_");
    cname += me_runNumber.at(fileid);
    TCanvas thisCanvas(cname.Data(), cname.Data());
    h_xresiduals3d->Draw("colz");
    thisCanvas.Print(cname+".png");
  }

  // now histogram has to be projected in order to get a residual average vs module eta
  //h_xresiduals3d->ProjectionZ("",0,-1,0,-1,"ed");
  // h_xresiduals3d->Project3DProfile("yx");

  for (int stave = 0; stave <= NIBLSTAVES; stave++) {
    me_h_IBLmeanResPerRing[stave] = ME_getIBLMeanResPerRing(h_xresiduals3d, stave, fileid); 
    ME_fitBowingMagnitude(me_h_IBLmeanResPerRing[stave], stave, fileid);
    if (FULLOUTPUT) {
      TString cname("IBLXmeanResVsRing_stave_");
      if (stave == 0) {
	cname.Append("all");
      }
      else {
	cname += stave;
      }
      cname.Append("_Run_");
      cname += me_runNumber.at(fileid);
      TCanvas thisCanvas(cname.Data(), cname.Data());
      me_h_IBLmeanResPerRing[stave]->Draw();
      thisCanvas.Print(cname+".png");
    }
  }

  // now loop on fit results and fill evolution histograms 
  for (int i=0; i < (int) me_bowingMagnitudeStave.size(); i++) {
    int stave = me_bowingMagnitudeStave.at(i); // this is because a stave may not be fitted due to stats or whatever
    me_h_bowingMagnitudeEvolution[stave]->SetBinContent(fileid+1, me_bowingMagnitudeValue.at(i));

    float total_uncertainty = sqrt(pow(me_bowingMagnitudeError.at(i),2) + pow(me_systUncertaintyBowing,2.));    
    me_h_bowingMagnitudeEvolution[stave]->SetBinError(fileid+1, total_uncertainty);

    // std::cout << " ** Filling evoution histograms: stave " << stave << "  entries = " << me_h_bowingMagnitudeEvolution[stave]->GetEntries() << std::endl; 
    me_h_bowingBaseLineEvolution[stave]->SetBinContent(fileid+1, me_bowingBaseLineValue.at(i));
    me_h_bowingBaseLineEvolution[stave]->SetBinError(fileid+1, me_bowingBaseLineError.at(i));
    
    // fill the bowing magnitude and base line histo per stave
    if (me_h_bowingMagnitudePerStave.size()>0) {
      std::cout << " me_h_bowingMagnitudePerStave.at(" << fileid << ")->SetBinContent(" << stave <<",  me_bowingMagnitudeValue.at(" << i << "));" << std::endl;
      me_h_bowingMagnitudePerStave.at(fileid)->SetBinContent(stave, me_bowingMagnitudeValue.at(i));
      me_h_bowingMagnitudePerStave.at(fileid)->SetBinError(stave, total_uncertainty);

      me_h_bowingBaseLinePerStave.at(fileid)->SetBinContent(stave,  me_bowingBaseLineValue.at(i));
      me_h_bowingBaseLinePerStave.at(fileid)->SetBinError(stave,  me_bowingBaseLineError.at(i));
    }
  }


  // in case lumiblock analysis is wanted
  if ( LUMIBLOCKANALYSIS ) {
    std::cout << "** LUMIBLOCK analysis ** starting ... " << std::endl;
    ME_performLumiBlockAnalysis(fileid);
  }



  return;
}

//////////////////////////////////////////
TH3F* ME_getIBLResiduals3DHisto (int fileid)
{
  bool debug = false;

  TH3F* h_xresiduals3d = NULL;

  TString h_path = "run_"; 
  h_path += me_runNumber.at(fileid); // add run number

  h_path.Clear(); // --> for monitoring files without run number in the path 
  //h_path.Append("/IDAlignMon/InDetTrackParticles_AlignSel/Residuals/pix_b0_xresvsmodetaphi_3d");
  //h_path.Append("/IDAlignMon/AlignTracks_all/Residuals/pix_b0_xresvsmodetaphi_3d");
  h_path.Append("/IDAlignMon/");
  h_path += me_trackCollection;
  h_path.Append("/Residuals/pix_b0_xresvsmodetaphi_3d");
  if (debug) std::cout << " ** going to fetch TH3 histo: " << h_path.Data() << std::endl;
  h_xresiduals3d = (TH3F*) me_currentMonitoringFile->Get(h_path.Data());
  
  if (debug && h_xresiduals3d == NULL) std::cout << " ** WARNING ** method 1 fails for " << h_path.Data() << " not found !!!" << std::endl;   
  if (debug && h_xresiduals3d != NULL) std::cout << " ** 3D histogram with residuals captured: " << h_xresiduals3d->GetName() << std::endl; 
  if (h_xresiduals3d != NULL) std::cout << " ** method 1 works ** 3D histogram with residuals captured: " << h_xresiduals3d->GetName() << std::endl; 

  if (h_xresiduals3d == NULL) { // try another method
    h_path.Clear();
    h_path.Append("run_");
    h_path += me_runNumber.at(fileid);
    h_path.Append("/IDAlignMon/");
    h_path.Append("InDetTrackParticles_AlignSel");
    h_path.Append("/Residuals/pix_b0_xresvsmodetaphi_3d");
    h_xresiduals3d = (TH3F*) me_currentMonitoringFile->Get(h_path.Data());  
    if (h_xresiduals3d != NULL) std::cout << " ** method 2 works ** 3D histogram with residuals captured: " << h_xresiduals3d->GetName() << std::endl; 
    if (debug && h_xresiduals3d == NULL) std::cout << " ** WARNING ** method 2 fails for " << h_path.Data() << " not found !!!" << std::endl;   
  }

  if (h_xresiduals3d == NULL) { // and yet another folder
    h_path.Clear();
    h_path.Append("run_");
    h_path += me_runNumber.at(fileid);
    h_path.Append("/IDAlignMon/");
    h_path.Append("AlignTracks_all");
    h_path.Append("/Residuals/pix_b0_xresvsmodetaphi_3d");
    h_xresiduals3d = (TH3F*) me_currentMonitoringFile->Get(h_path.Data());  
    if (h_xresiduals3d != NULL) std::cout << " ** method 3 works ** 3D histogram with residuals captured: " << h_xresiduals3d->GetName() << std::endl; 
    if (debug && h_xresiduals3d == NULL) std::cout << " ** WARNING ** method 3 fails for " << h_path.Data() << " not found !!!" << std::endl;   
  }

  if (h_xresiduals3d == NULL) { // and yet another folder
    h_path.Clear();
    h_path.Append("run_");
    h_path += me_runNumber.at(fileid);
    h_path.Append("/IDAlignMon/");
    h_path.Append("AlignTracks_all");
    h_path.Append("/Residuals/pix_b0_xresvsmodetaphi_3d");
    h_xresiduals3d = (TH3F*) me_currentMonitoringFile->Get(h_path.Data());  
    if (h_xresiduals3d != NULL) std::cout << " ** method 4 works ** 3D histogram with residuals captured: " << h_xresiduals3d->GetName() << std::endl; 
  }
  if (h_xresiduals3d == NULL) std::cout << " **  ME_getIBLResiduals3DHisto ** WARNING **  /Residuals/pix_b0_xresvsmodetaphi_3d was not found !!!" << std::endl;   

  return h_xresiduals3d;
}

//////////////////////////////////////////
TH1F* ME_getIBLMeanResPerRing(TH3F* h_residuals3d, int staveid, int fileid) 
{ 
 bool debug = false;
  // beware staveid = 0 --> whole IBL
  int lowerStave = 0;
  int upperStave = -1;
  // if a stave is given, then project only that stave
  if (staveid > 0) {
    lowerStave = staveid;
    upperStave = staveid;
  }
  
  // build the histogram to be returned
  // -set the name and title
  TString hname("h_IBL_ResVsEta_");
  TString htitle("IBL mean residual vs ring");
  if (staveid == 0) {
    hname.Append("All");
  }
  else {
    hname.Append("stave_"); 
    hname += staveid;
    htitle.Append(" ("); htitle += staveid; htitle.Append(")");
  }
  
  TH1F* h_meanResVsRing = new TH1F(hname.Data(), htitle.Data(), h_residuals3d->GetNbinsX(), h_residuals3d->GetXaxis()->GetXmin(), h_residuals3d->GetXaxis()->GetXmax()); 
  
  for (int ring=1; ring <= h_residuals3d->GetNbinsX(); ring++) { 
    // std::cout << " ring -> " << ring << std::endl;
    hname.Clear();
    hname.Append(h_residuals3d->GetName());
    hname.Append("_stave_"); hname += staveid;
    hname.Append("_ring_"); hname += ring;
    TH1F* h_residualsAtEta = (TH1F*) h_residuals3d->ProjectionZ(hname.Data(),ring,ring, lowerStave, upperStave,""); 
    h_residualsAtEta->SetXTitle("local x residuals [  #mu m]"); 
    bool distributionFitted = ME_fitResidualsDistribution(h_residualsAtEta, ring);
    float thisRingMeanValue, thisRingMeanError; 
    if (distributionFitted) {
      TF1 *fit = h_residualsAtEta->GetFunction("gaus");

      if (fit != NULL) {
	thisRingMeanValue = fit->GetParameter(1) * 1000; // --> microns
	thisRingMeanError = fit->GetParError(1) * 1000; // --> microns
	if (debug) std::cout << "  ** Residuals Gaussian fit result: mean= " << thisRingMeanValue << " +- " << thisRingMeanError << std::endl;
      }
      // store result
      h_meanResVsRing->SetBinContent(ring, thisRingMeanValue);
      h_meanResVsRing->SetBinError(ring, thisRingMeanError+0.1); // 0.1 a syst uncertainty of the Gaussian fit
    }
    if (FULLOUTPUT) {
      TString cname("IBLXRes_stave_");
      if (staveid == 0) {
	cname.Append("all");
      }
      else {
	cname += staveid;
      }
      cname.Append("_ring_");
      cname += ring;
      cname.Append("_Run_");
      cname += me_runNumber.at(fileid);
      TCanvas thisCanvas(cname.Data(), cname.Data());
      h_residualsAtEta->Draw();
      // text with result
      if (distributionFitted) {
	TString textWithResult("#mu = ");
	textWithResult += TString::Format("%4.3f",thisRingMeanValue);
	textWithResult.Append(" +- ");
	textWithResult += TString::Format("%4.3f",thisRingMeanError);
	TText aText;
	aText.SetTextSize(0.03);
	aText.SetTextColor(kGray+2);
	aText.DrawTextNDC(0.7,0.5,textWithResult.Data());
      }
      thisCanvas.Print(cname+".png");
    }
  }
  
  h_meanResVsRing->SetYTitle("mean residual [  #mum]"); 
  h_meanResVsRing->SetXTitle("#eta ring"); 
  return h_meanResVsRing;
}

//////////////////////////////////////////
bool ME_fitResidualsDistribution (TH1F* h_residuals, int ring)
{
  // fit the residual distribution to a Guassian
  // but first make some basic checks, about number of entries, entries in the most populated bin, rms
  
  bool distributionGoodForFit = true;
  bool debug = false;

  // test total number of entries
  if (h_residuals->GetEntries() < MINENTRIES) {
    distributionGoodForFit = false;
    if (debug) std::cout << " ** histogram: " << h_residuals->GetName() << " has too few entries ("
			 << h_residuals->GetEntries() << " < " << MINENTRIES << ")  --> NO FIT " << std::endl;
  }

  // test entries in most populated bin
  int mostPopulatedBin = h_residuals->GetMaximumBin();
  int entriesInMostPopulatedBin = h_residuals->GetBinContent(mostPopulatedBin);
  if (entriesInMostPopulatedBin < MINENTRIESINBIN) {
    distributionGoodForFit = false;
    if (debug) std::cout << " ** histogram: " << h_residuals->GetName() << " has too few entries in most populated bin "
			 << mostPopulatedBin << " (" << entriesInMostPopulatedBin << " < "<< MINENTRIESINBIN << ")  --> NO FIT " << std::endl;
  }

  // test rms
  float histoXRange = h_residuals->GetXaxis()->GetXmax() - h_residuals->GetXaxis()->GetXmin();
  if ( h_residuals->GetRMS() > 0.33 * histoXRange) {
    distributionGoodForFit = false;
    if (debug) std::cout << " ** histogram: " << h_residuals->GetName() << " has a too broad RMS ("
			 << h_residuals->GetRMS() << " > "<<  0.20 * histoXRange << ")  --> NO FIT " << std::endl;
  } 

  // planar only
  if (USEPLANARONLY) {
    if (ring == 1) distributionGoodForFit = false;
    if (ring == 2) distributionGoodForFit = false;
    if (ring == 3) distributionGoodForFit = false;
    if (ring == 4) distributionGoodForFit = false;
    if (ring ==17) distributionGoodForFit = false;
    if (ring ==18) distributionGoodForFit = false;
    if (ring ==19) distributionGoodForFit = false;
    if (ring ==20) distributionGoodForFit = false;
  } 

  if (distributionGoodForFit) {
    float upperRange = h_residuals->GetMean() +  RMSFACTOR * h_residuals->GetRMS();
    float lowerRange = h_residuals->GetMean() -  RMSFACTOR * h_residuals->GetRMS();
    // h_residuals->Fit("gaus","QR0", "", lowerRange, upperRange);
    h_residuals->Fit("gaus","QR", "", lowerRange, upperRange);
  }

  return distributionGoodForFit;
}

//////////////////////////////////////////
bool ME_fitBowingMagnitude (TH1F* h_input, int staveid, int fileid, int lumiblockid)
{
  // fit the IBL stave bowing magnitude
  // but first make some basic checks, about number of entries 
  
  bool distributionGoodForFit = true;
  bool debug = false;

  if (h_input->GetEntries() < MINGOODPOINTS) {
    distributionGoodForFit = false;
    if (debug and h_input->GetEntries()>0) std::cout << " ** histogram: " << h_input->GetName() << " has too few valid bins ("
						     << h_input->GetEntries() << " > "<<  MINGOODPOINTS << ")  --> NO FIT " << std::endl;
  }

  if (distributionGoodForFit) {
    // convert the points in eta-ring to the real z of the module
    TGraphErrors* thisGraph = ConvertHistoInGraph(h_input);

    // now fit the graph
    bool isFitGood =  MakeStaveFit (thisGraph, FITTYPE);
    double magValue, magError;
    double baseLineValue = 0, baseLineError = 0;
    TLine aLine; 
    aLine.SetLineColor(kGray+2);
    aLine.SetLineStyle(2);
    TArrow anArrow; 
    anArrow.SetLineColor(kGray+2);
    anArrow.SetLineStyle(2);
    anArrow.SetArrowSize(0.02);

    if (isFitGood) {
      TF1 *fitFunction = thisGraph->GetFunction("fitBowing");
      if (fitFunction != NULL) {
	if (FITTYPE == 2) {
	  magValue =  5.0 * fitFunction->GetParameter(1) * pow(z_fix,4);
	  magError =  5.0 * fitFunction->GetParError(1)  * pow(z_fix,4);
	}
	if (FITTYPE == 3) {
	  baseLineValue =  fitFunction->GetParameter(1);
	  baseLineError =  fitFunction->GetParError(1);
	  magValue =  fitFunction->GetParameter(2);
	  magError =  fitFunction->GetParError(2);
	}
	if (debug) std::cout << " ** Magnitude fit OK :) magValue: " << magValue << " +- " << magError << std::endl;
	me_bowingMagnitudeStave.push_back(staveid);
	me_bowingMagnitudeValue.push_back(magValue);
	me_bowingMagnitudeError.push_back(magError);
	me_bowingBaseLineValue.push_back(baseLineValue);
	me_bowingBaseLineError.push_back(baseLineError);
      }
      else {
	std::cout << " ** WARNING ** Magnitude fit went wrong :( " << std::endl;
      }
      if (FULLOUTPUT or true) {
	TString cname("IBLMagFit_stave_");
	if (staveid == 0) {
	  cname.Append("all");
	}
	else {
	  cname += staveid;
	}
	cname.Append("_Run_");
	cname += me_runNumber.at(fileid);
	if (lumiblockid >= 0) {
	  cname.Append("_LB_");
	  cname += lumiblockid;
	}
	TCanvas thisCanvas(cname.Data(), cname.Data());
	thisGraph->GetXaxis()->SetLimits(-365.,365.); // full stave size
	thisGraph->Draw();
	//
	// now draw the fitting function in the entire stave length
	fitFunction->SetRange(-365.,365.);
	fitFunction->Draw("same");
	//
	TString textWithResult("magnitude = ");
	textWithResult += TString::Format("%4.3f",magValue);
	textWithResult.Append(" +- ");
	textWithResult += TString::Format("%4.3f",magError);
	textWithResult.Append(" microns ");
	TText aText;
	aText.SetTextSize(0.04);
	aText.SetTextAlign(22);
	aText.SetTextColor(kGray+2);
	aText.DrawTextNDC(0.5,0.86,textWithResult.Data());

	if (baseLineValue != 0) {
	  textWithResult.Clear();
	  textWithResult.Append("base line = ");
	  textWithResult += TString::Format("%4.3f",baseLineValue);
	  textWithResult.Append(" +- ");
	  textWithResult += TString::Format("%4.3f",baseLineError);
	  textWithResult.Append(" microns ");
	aText.DrawTextNDC(0.5,0.80,textWithResult.Data());
	}
	aLine.DrawLine(-365.,baseLineValue, 365., baseLineValue);
	anArrow.DrawArrow(0., baseLineValue, 0., baseLineValue+magValue);

	thisCanvas.Print(cname+".png");
      }
    }
  }

  return distributionGoodForFit; 
}

//////////////////////////////////////////
// original function from PF Butti (extracted from BowingFit.cxx)
//TGraphErrors* ConvertHistoInGraph(const TH1D* histo, string title)
TGraphErrors* ConvertHistoInGraph(const TH1F* histo) 
{
  
  // --> SMG // ofstream txtfile;
  // --> SMG // string filename = title;
  // --> SMG // txtfile.open((filename+".txt").c_str());

  double z_axis[20] = {-322.8975, -301.7925, -280.6875,-259.5825,-228.2775,-186.7725,-145.2675,-103.7625,-62.2575,-20.7525,20.7525,62.2575,103.7625,145.2675,186.7725,228.2775,259.5825,280.6875,301.7925,322.8975};
  TGraphErrors* graph = new TGraphErrors();

  // find first which bins were filled (should have an error >0 )
  vector<int> filled_bins;
  for (int etabin=1; etabin <= histo->GetNbinsX(); etabin++) {
    if (histo->GetBinContent(etabin) != 0. && histo->GetBinError(etabin)>0) {
      filled_bins.push_back(etabin);
    }
  }

  /* --> SMG
  for (int etabin=1;etabin < 21;++etabin)
    {
      if (title.find("renormalised")!=string::npos && title.find("p15")!=string::npos)
	filled_bins.push_back(etabin);
      else
	if (histo->GetBinContent(etabin) != 0.)
	  {
	  }
      //else
      //cout<<"empty bin"<<etabin<<endl;
    }
  --> SMG */
 
  for (int ibin=0;ibin < (int) filled_bins.size();++ibin)
    {
      //cout<<"CHECK  "<<histo->GetName()<<endl;
      //cout<<filled_bins.at(ibin)<<endl;
      //cout<<"x  "<<z_axis[filled_bins.at(ibin)]<<endl;
      //cout<<"y  "<<histo->GetBinContent(filled_bins.at(ibin))<<endl;
      graph->Set(ibin+1); 
      graph->SetPoint(ibin,z_axis[filled_bins.at(ibin)-1],histo->GetBinContent(filled_bins.at(ibin)));
      graph->SetPointError(ibin,0,histo->GetBinError(filled_bins.at(ibin)));
      // --> SMG // txtfile<<z_axis[filled_bins.at(ibin)-1]<<"    "<< histo->GetBinContent(filled_bins.at(ibin))<<"    "<<histo->GetBinError(filled_bins.at(ibin))<<"\n";
    }
  // --> SMG // txtfile.close();
  graph->SetMarkerStyle(20);
  graph->SetMarkerSize(1);
  graph->GetXaxis()->SetTitle("Global Z Position [mm]");
  //cout<<"Setting range: "<<xmin<<"   "<<xmax<<endl; 
  // --> SMG xmin=-0.4;
  // --> SMG xmax=0.3;
  xmin = -me_bowingRangeInPlots;
  xmax =  me_bowingRangeInPlots/3;
  graph->GetYaxis()->SetRangeUser(xmin,xmax);
  graph->GetXaxis()->SetRangeUser(-z_fix,z_fix);
  graph->GetYaxis()->SetTitle("Local X Residual mean [   #mu m]");
  // --> SMG // if (getYresiduals) graph->GetYaxis()->SetTitle("Local Y Residual mean [mm]");
  /*
  graph->Draw();
  graph->Print();
  string input = "";
  cout << " ** RecoExample ** Please type RETURN to continue:\n>";
  getline(cin, input);
  */
  return graph;
}

//////////////////////////////////////////
// original function from PF Butti (extracted from BowingFit.cxx)
//TGraphErrors* MakeStaveFit(TFile* OutFile,TH1D* histo,string name,string title,string path,int type,bool renormalised)
bool MakeStaveFit (TGraphErrors* inputGraph, int type)

//type == 2   -> x^3 case. Two fixed ends.
//type == 4   -> x^4 case. P4 0 gradient in the ends.

{    
  bool graphGoodForFit = true;
  int fixbase=0;
  // --> SMG // OutFile->cd();
  // --> SMG // TGraphErrors* g = ConvertHistoInGraph(histo,title);
  // --> SMG // inputGraph->SetTitle(title.c_str());
  bool debug = false;

  TF1 *fit;
  if (type==2)
    {
      if (debug) std::cout<<"FIT HIDE FORMULA"<< std::endl;
      fit = new TF1("fitBowing", "[0]-[1]*(x+[2]) * (4.0*[2]*(x+[2])**2 - (x+[2])**3 - (2.0*[2])**3)", -z_fix, z_fix );
      fit->FixParameter(2, z_fix);
      fit->SetParameter(1, 5.e-12);
    }
  else if (type ==4)
    {
      fit = new TF1("fitBowing", "[0]+[1]*( (x+[2]/2.0)**2 ) * ( ([2]-(x+[2]/2.0))**2 )", -z_fix, z_fix );
      fit->FixParameter(2, z_fix*2);      
    }
  else if (type == 1)
    {
      fit = new TF1("fitBowing", "-[1]*[0]-[2]*[0]*[0] +[1]*x+[2]*x*x",-z_fix,z_fix); 
    }
  else if (type == 3)
    {
      if (debug) std::cout<<"Fitting SH formula"<<std::endl;
      //fit = new TF1("fitBowing", "[2]* ( 1 - (x-[1])*(x-[1])/([0]-[1])/([0]-[1]) )",-z_fix,z_fix);
      //fit->FixParameter(0,z_fix);
      
      fit = new TF1("fitBowing", "[1] - ([2]*(x*x-[0]*[0]))/([0]*[0])",-z_fix,z_fix);
      fit->FixParameter(0,z_fix);
    }

  else 
    {
        fit = new TF1("fitBowing","[0]+[1]+[2]",-z_fix,z_fix);
	std::cout<<"No fit will be made"<<std::endl;
        fixbase=1;
        fit->FixParameter(2,0);
    }
  
 
  if (fixbase==0)
    {
      if (type!=3)
	fit->SetParameter(0,0.1);
    }
  else 
    {
      if (debug) std::cout<<"Fixing baseline"<<std::endl;
      if (type == 1)
	fit->FixParameter(0,z_fix);
      else if (type==3)
	{
	  fit->FixParameter(1,0.);
	  // --> SMG // if (preM7) fit->FixParameter(1,-0.160);
	}
      else
	fit->FixParameter(0,0.);
      
    }
  

  
  TFitResultPtr r;
  /* --> SMG 
     if (!getYresiduals)
     {
     cout<<"FITTING "<<title<<endl;
     r = inputGraph->Fit(fit,"EX0S");
     }
     --> SMG */
  
  if (debug) std::cout<<" ** MakeStaveFit ** FITTING... " << std::endl;
  r = inputGraph->Fit(fit,"QEX0S");

  /* --> SMG 
  double mag       =  5.0 * fit->GetParameter(1) * (z_fix*z_fix*z_fix*z_fix);
  double mag_error =  5.0 * fit->GetParError(1)  * (z_fix*z_fix*z_fix*z_fix);
  --> SMG */
  double mag       =  5.0 * fit->GetParameter(1) * pow(z_fix,4);
  double mag_error =  5.0 * fit->GetParError(1)  * pow(z_fix,4);
  
  if (type==3 and false) {
    std::cout << " Giving Back fit parameters " << std::endl;
    mag       = fit->GetParameter(2);
    mag_error = fit->GetParError(2);
  }
 
  /* --> SMG
     string temp = path.erase(0,2);
     temp.erase(temp.find("/"),1);
     cout<<"Saving temperature "<<name+"_"+temp<<endl;
     string key = "";
     if (!renormalised)
     key = name+"_"+temp;
     else
     key = name;
     mags[key] = mag;
     mag_errors[key] = mag_error;
     
     for(map<string, float >::const_iterator it = mags.begin();
     it != mags.end(); ++it)
     {
     std::cout << it->first << " " << it->second<<endl;
     }
     
  
     cout<<"Adding baseline: "<<fit->GetParameter(0)<<endl;
     baselines[key]=fit->GetParameter(0);
     baseline_errors[key]=fit->GetParError(0);
     
     TCanvas c((name+"_"+temp).c_str(),title.c_str());
     inputGraph->Draw("AP");
     string plotTitle = "";
     string templabel = "";
     if (title.find("total")!=string::npos)
     {
     plotTitle = "IBL Stave Shape (averaged over all staves)";
     //if (!renormalised)
     bowingshapes["total"].push_back((TGraphErrors*)g->Clone());
     }
     
     else if (title.find("bottom")!=string::npos)
     {
     plotTitle = "IBL Stave Shape (averaged over 9-12 staves)";
     //if (!renormalised)
     bowingshapes["bottom"].push_back((TGraphErrors*)g->Clone());
     }
     else if (title.find("top")!=string::npos)
     {
     plotTitle = "IBL Stave Shape (averaged over 1-4 staves)";
     //if (!renormalised)
     bowingshapes["top"].push_back((TGraphErrors*)g->Clone());
     }
     else if (title.find("side")!=string::npos)
     {
     plotTitle = "IBL Stave Shape (averaged over 0,5-8,13 staves)";
     //if (!renormalised)
     bowingshapes["side"].push_back((TGraphErrors*)g->Clone());
     }
     else
     if (!renormalised)
     {
     plotTitle = title;
     std::cout<<"NAME FOR SHAPES: "<<name<<std::endl;
     bowingshapes[name].push_back((TGraphErrors*)g->Clone());
     }
     
     
     else
     plotTitle =title+"_renormalised";
     
     inputGraph->SetTitle(plotTitle.c_str());
     
     
     if (!getYresiduals)
     {
     DrawParameters(c,r->Chi2(),r->Ndf(),mag,mag_error,fit->Eval(z_fix));
     std::cout<<"Correlation between par 0 and par 1  "<< r->Correlation(0,1)<<std::endl;
     std::cout<<"Correlation between par 1 and par 2  "<< r->Correlation(1,2)<<std::endl;
     std::cout<<"Correlation between par 0 and par 2  "<< r->Correlation(0,2)<<std::endl;
     
     
     }
     
     
     TLine* stave;
     stave=MakeStaveLine(fit);
     stave->Draw();
     //stavelow.Draw();
     //stavemax.Draw();
     //c.SetGridy();
     //c.SetGridx();
     c.SaveAs((path+name+"_"+temp+oFext).c_str());
     delete fit;
     delete stave;
     //  return g;
     --> SMG */
     
  return graphGoodForFit;
}  

//////////////////////////////////////////
void ME_prepareEvolutionHistos()
{
  std::cout << " ** preparing evolution histograms for " << me_monitoringFileList.size() << " runs " << std::endl;
  TString hname, htitle, binLabel;
  

  for (int stave = 0; stave <= NIBLSTAVES; stave++) {
    hname.Clear();
    hname.Append("h_bowMagEvol");
    htitle.Clear();

    if (stave== 0) { // whole IBL
      hname.Append("IBL"); 
      htitle.Append("IBL all staves bowing magnitude evolution per run");
    }
    else {
      hname.Append("Stave");
      hname += stave;
      htitle.Append("IBL stave "); 
      htitle += stave;
      htitle.Append(" bowing magnitude evolution per run");
    }

    // stave bowing magnitude
    me_h_bowingMagnitudeEvolution[stave] = new TH1F(hname.Data(), htitle.Data(),
						    me_monitoringFileList.size(), -0.5, me_monitoringFileList.size()-0.5);

    // stave baseline
    hname.Clear();
    hname.Append("h_bowBaseLineEvol");
    htitle.Clear();

    if (stave== 0) { // whole IBL
      hname.Append("IBL"); 
      htitle.Append("IBL all staves bowing base line evolution per run");
    }
    else {
      hname.Append("Stave");
      hname += stave;
      htitle.Append("IBL stave "); 
      htitle += stave;
      htitle.Append(" bowing base line evolution per run");
    }
    
    me_h_bowingBaseLineEvolution[stave] = new TH1F (hname.Data(), htitle.Data(),
						    me_monitoringFileList.size(), -0.5, me_monitoringFileList.size()-0.5);


    // use run number as x-axis bin label
    for (int bin=1; bin <= (int) me_monitoringFileList.size(); bin++) {
      binLabel.Clear();
      binLabel += me_runNumber.at(bin-1);
      me_h_bowingMagnitudeEvolution[stave]->GetXaxis()->SetBinLabel(bin, binLabel.Data());
      me_h_bowingBaseLineEvolution[stave]->GetXaxis()->SetBinLabel(bin, binLabel.Data());
    }
    // style
    me_h_bowingMagnitudeEvolution[stave]->SetStats(kFALSE);
    me_h_bowingMagnitudeEvolution[stave]->SetYTitle("Bowing magnitude [  #mum]");
    me_h_bowingMagnitudeEvolution[stave]->SetXTitle("Run number");
    me_h_bowingMagnitudeEvolution[stave]->SetMarkerStyle(kFullCircle);
    me_h_bowingMagnitudeEvolution[stave]->SetLineWidth(2);

    me_h_bowingBaseLineEvolution[stave]->SetStats(kFALSE);
    me_h_bowingBaseLineEvolution[stave]->SetYTitle("Base line [  #mum]");
    me_h_bowingBaseLineEvolution[stave]->SetXTitle("Run number");
    me_h_bowingBaseLineEvolution[stave]->SetMarkerStyle(kFullCircle);
    me_h_bowingBaseLineEvolution[stave]->SetLineWidth(2);

    if (stave == 0) {
      me_h_bowingMagnitudeEvolution[stave]->SetMarkerColor(me_colorForIBL);
      me_h_bowingMagnitudeEvolution[stave]->SetLineColor(me_colorForIBL);
      me_h_bowingBaseLineEvolution[stave]->SetMarkerColor(me_colorForIBL);
      me_h_bowingBaseLineEvolution[stave]->SetLineColor(me_colorForIBL);
    }
    if (stave > 0) {
      me_h_bowingMagnitudeEvolution[stave]->SetMarkerColor(me_colorForStaves);
      me_h_bowingMagnitudeEvolution[stave]->SetLineColor(me_colorForStaves);
      me_h_bowingBaseLineEvolution[stave]->SetMarkerColor(me_colorForStaves);
      me_h_bowingBaseLineEvolution[stave]->SetLineColor(me_colorForStaves);
    }
  }

  return;
}

//////////////////////////////////////////
void ME_prepareHistosPerStave()
{
  bool debug = true;
  if (debug) std::cout << " ** preparing histograms per stave. There are in total " << me_monitoringFileList.size() << " runs " << std::endl;
  if (NIBLSTAVES > 0 ) {
    // this histograms are created if user wants to perform an stave by stave analysis
    TString hname, htitle, binLabel;
  
    
    for (int file = 0; file < (int)  me_monitoringFileList.size(); file++) {
      if (debug)  std::cout << " ** preparing histograms for file: " << file << " --> run: " << me_runNumber.at(file) << std::endl; 
      hname.Clear();
      hname.Append("h_bowMagPerStave_Run");
      hname += me_runNumber.at(file);
      htitle.Clear();
      htitle.Append("Bowing magnitude per stave for Run ");
      htitle += me_runNumber.at(file);

      // bowing magnitude per stave
      TH1F* h_bowingMagnitudePerStave = new TH1F(hname.Data(), htitle.Data(), NIBLSTAVES, 0.5, NIBLSTAVES+0.5);

      // stave baseline
      hname.Clear();
      hname.Append("h_baseLineEvolPerStave_Run");
      hname += me_runNumber.at(file);
      htitle.Clear();
      htitle.Append("Base line per stave for Run ");
      htitle += me_runNumber.at(file);

      TH1F* h_baseLinePerStave = new TH1F(hname.Data(), htitle.Data(), NIBLSTAVES, 0.5, NIBLSTAVES+0.5);
    
      // style
      h_bowingMagnitudePerStave->SetStats(kFALSE);
      h_bowingMagnitudePerStave->SetYTitle("Bowing magnitude [  #mum]");
      h_bowingMagnitudePerStave->SetXTitle("Stave number");
      h_bowingMagnitudePerStave->SetMarkerStyle(kFullCircle);
      h_bowingMagnitudePerStave->SetLineWidth(2);
      h_bowingMagnitudePerStave->SetMarkerColor(me_colorForStaves);
      h_bowingMagnitudePerStave->SetLineColor(me_colorForStaves);

      h_baseLinePerStave->SetStats(kFALSE);
      h_baseLinePerStave->SetYTitle("Base line [  #mum]");
      h_baseLinePerStave->SetXTitle("Stave number");
      h_baseLinePerStave->SetMarkerStyle(kFullCircle);
      h_baseLinePerStave->SetLineWidth(2);
      h_baseLinePerStave->SetMarkerColor(me_colorForStaves);
      h_baseLinePerStave->SetLineColor(me_colorForStaves);

      me_h_bowingMagnitudePerStave.push_back(h_bowingMagnitudePerStave);
      me_h_bowingBaseLinePerStave.push_back(h_baseLinePerStave);
    }
  }
  
std::cout << " ** Histograms per stave prepared for " << NIBLSTAVES << " runs " << std::endl;

  return;
}

//////////////////////////////////////////
void ME_prepareLoop()
{
  // clean vectors with fit results
  me_bowingMagnitudeStave.clear();
  me_bowingMagnitudeValue.clear();
  me_bowingMagnitudeError.clear();
  me_bowingBaseLineValue.clear();
  me_bowingBaseLineError.clear();
  return;
}

//////////////////////////////////////////
void ME_plotEvolutionHistos()
{
  TString cname, ctitle, fname;
  TText legend; 
  legend.SetTextColor(kGray+3);
  TText legend_2;
  legend_2.SetTextColor(kGray+2);
  legend_2.SetTextAlign(30);
  legend_2.SetTextAngle(90.);
  legend_2.SetTextSize(0.03);
  TLine aLine;
  aLine.SetLineColor(kGray+2);
  aLine.SetLineStyle(2);
  

  for (int stave=0; stave <= NIBLSTAVES; stave++) {
    cname.Clear();
    cname.Append("CanBowingMagEvolForStave");
    ctitle.Clear();
    ctitle.Append("IBL stave ");
    fname.Clear();
    fname.Append("BowingMagEvolForStave");
    if (stave == 0) {
      cname.Append("All");
      ctitle.Append("All ");
      fname.Append("All");
    }
    else {
      cname += stave;
      ctitle += stave;
      ctitle.Append("bowing magnitude evolution vs run");
      fname += stave;
    }
    fname.Append(".png");
    TCanvas* thisCanvas = new TCanvas (cname.Data(), ctitle.Data());
    thisCanvas->cd();
    gPad->SetTicks(1,1);
    me_h_bowingMagnitudeEvolution[stave]->SetMaximum( me_bowingRangeInPlots/3);
    me_h_bowingMagnitudeEvolution[stave]->SetMinimum(-me_bowingRangeInPlots);
    me_h_bowingMagnitudeEvolution[stave]->Draw("");
    aLine.DrawLine(me_h_bowingMagnitudeEvolution[stave]->GetXaxis()->GetXmin(),0.,
		   me_h_bowingMagnitudeEvolution[stave]->GetXaxis()->GetXmax(),0.);
    //    legend.DrawText(0.15*me_monitoringFileList.size(),17.,"Period E");
    //legend.DrawText(0.56*me_monitoringFileList.size(),17.,"Period F");
    //legend.DrawText(0.77*me_monitoringFileList.size(),17.,"Period G");
    //aLine.DrawLine(16.5, -me_bowingRangeInPlots, 16.5, 15.0);
    //aLine.DrawLine(20.5, -me_bowingRangeInPlots, 20.5, 15.0);
    
    // 278748
    // legend_2.DrawText( 0.4,-12., "7/Sep/2015");
    // 279984
    // legend_2.DrawText(16.0,-12., "21/Sep/2015");
    // 280319
    // legend_2.DrawText(20.,-12.,"26/Sep/2015");
    // 280319
    // legend_2.DrawText(23.,-12.,"29/Sep/2015");
   
    /*
    //legend_2.DrawText( 0.0,-5., "Run 280319");
    legend_2.DrawText( 0.0,-5., "Part 1");
    legend_2.DrawText( 1.0,-5., "Part 2");
    legend_2.DrawText( 2.0,-5., "Part 3");
    legend_2.DrawText( 3.0,-5., "Part 4");
    legend_2.DrawText( 4.0,-5., "Part 5");
    legend_2.DrawText( 5.0,-5., "Part 6");
    me_h_bowingMagnitudeEvolution[stave]->Fit("pol0");
    */
    thisCanvas->Print(fname.Data());

    ///////////////////    
    // drawing base line
    ///////////////////    
    cname.Clear();
    cname.Append("CanBowingBaseLineEvolForStave");
    ctitle.Clear();
    ctitle.Append("IBL stave ");
    fname.Clear();
    fname.Append("BowingBaseLineEvolForStave");
    if (stave == 0) {
      cname.Append("All");
      ctitle.Append("All ");
      fname.Append("All");
    }
    else {
      cname += stave;
      ctitle += stave;
      ctitle.Append("bowing baseline evolution vs run");
      fname += stave;
    }
    fname.Append(".png");
    TCanvas* thisCanvas2 = new TCanvas (cname.Data(), ctitle.Data());
    thisCanvas2->cd();
    gPad->SetTicks(1,1);

    me_h_bowingBaseLineEvolution[stave]->SetMaximum( me_bowingRangeInPlots/3);
    me_h_bowingBaseLineEvolution[stave]->SetMinimum(-me_bowingRangeInPlots);
    me_h_bowingBaseLineEvolution[stave]->Draw("");
    aLine.DrawLine(me_h_bowingBaseLineEvolution[stave]->GetXaxis()->GetXmin(),0.,
		   me_h_bowingBaseLineEvolution[stave]->GetXaxis()->GetXmax(),0.);
    aLine.DrawLine(16.5, -me_bowingRangeInPlots, 16.5, me_bowingRangeInPlots/3);
    aLine.DrawLine(20.5, -me_bowingRangeInPlots, 20.5, me_bowingRangeInPlots/3);
    thisCanvas2->Print(fname.Data());

  }
  return;
}
//////////////////////////////////////////
void ME_plotHistosPerStave()
{
  TString cname, ctitle, fname;
  TText legend; 
  legend.SetTextColor(kGray+3);
  TText legend_2;
  legend_2.SetTextColor(kGray+2);
  legend_2.SetTextAlign(30);
  legend_2.SetTextAngle(90.);
  legend_2.SetTextSize(0.03);
  TLine aLine;
  aLine.SetLineColor(kGray+2);
  aLine.SetLineStyle(2);
  
  if (me_h_bowingMagnitudePerStave.size() > 0 ) {
    for (int fileid=0; fileid < (int) me_runNumber.size(); fileid++) {
      cname.Clear();
      cname.Append("CanBowingMagPerStaveForRun");
      ctitle.Clear();
      ctitle.Append("Bowing magnitude per stave for run ");
      fname.Clear();
      fname.Append("BowingMagPerStave_Run");
      cname += me_runNumber.at(fileid);
      ctitle += me_runNumber.at(fileid);
      fname += me_runNumber.at(fileid);
      fname.Append(".png");
      TCanvas* canvasBM = new TCanvas (cname.Data(), ctitle.Data());
      canvasBM->cd();
      gPad->SetTicks(1,1);

      // check range
      if (me_h_bowingMagnitudePerStave.at(fileid)->GetMaximum() < 0) me_h_bowingMagnitudePerStave.at(fileid)->SetMaximum(1.); // 1 micron

      me_h_bowingMagnitudePerStave.at(fileid)->Draw();
      aLine.DrawLine(me_h_bowingMagnitudePerStave.at(fileid)->GetXaxis()->GetXmin(),0.,
		     me_h_bowingMagnitudePerStave.at(fileid)->GetXaxis()->GetXmax(),0.);

      canvasBM->Print(fname.Data());

      // now base line
      cname.Clear();
      cname.Append("CanBaseLinePerStaveForRun");
      ctitle.Clear();
      ctitle.Append("Base line per stave for run ");
      fname.Clear();
      fname.Append("BaseLinePerStave_Run");
      cname += me_runNumber.at(fileid);
      ctitle += me_runNumber.at(fileid);
      fname += me_runNumber.at(fileid);
      fname.Append(".png");
      TCanvas* canvasBL = new TCanvas (cname.Data(), ctitle.Data());
      canvasBL->cd();
      gPad->SetTicks(1,1);

      me_h_bowingBaseLinePerStave.at(fileid)->Draw();
      aLine.DrawLine(me_h_bowingBaseLinePerStave.at(fileid)->GetXaxis()->GetXmin(),0.,
		     me_h_bowingBaseLinePerStave.at(fileid)->GetXaxis()->GetXmax(),0.);

      canvasBL->Print(fname.Data());

    }
  }

  return;
}

//////////////////////////////////////////
void ME_writeOutputTextFile()
{
  if (WRITETEXTOUTPUT) {
    // loop on the histogram and extract the values
    //int stave=1; // entire IBL
    for (int stave=0; stave <= NIBLSTAVES; stave++) {
      me_outputTextFile << " Stave " << stave << "\n";
      for (int i=0; i < (int) me_monitoringFileList.size(); i++) {
	if (me_h_bowingMagnitudeEvolution[stave]->GetBinContent(i+1) != 0) { // this bin/run was filled
	  me_outputTextFile << me_runNumber.at(i) <<","
			    << me_h_bowingMagnitudeEvolution[stave]->GetBinContent(i+1) <<","
			    << me_h_bowingMagnitudeEvolution[stave]->GetBinError(i+1)
			    << "\n";
	}
      }
    }
    me_outputTextFile.close();

    if ( LUMIBLOCKANALYSIS ) {
      if (me_h_bowingMagnitudePerStaveAndLB != NULL) {
	TString fname;
	fname.Clear();
	fname.Append("MagnitudeEvolutionByStaveAndLB_Run");
	me_outputTextFile.open (fname+".txt");
	for (int i=1; i <= me_h_bowingMagnitudePerStaveAndLB->GetNbinsX(); i++) {
	  me_outputTextFile << me_h_bowingMagnitudePerStaveAndLB->GetXaxis()->GetBinCenter(i) << ",";
	  // write the entire IBL as stave 0
	  me_outputTextFile << " IBL " 
			    << "," << me_h_bowingMagnitudePerLB[0].at(0)->GetBinContent(i)
			    << "," << me_h_bowingMagnitudePerLB[0].at(0)->GetBinError(i) << " Staves ";
	  for (int j=1; j <= me_h_bowingMagnitudePerStaveAndLB->GetNbinsY(); j++) {
	    me_outputTextFile << me_h_bowingMagnitudePerStaveAndLB->GetYaxis()->GetBinCenter(j) << ","
			      << me_h_bowingMagnitudePerStaveAndLB->GetBinContent(i,j) << ","
			      << me_h_bowingMagnitudePerStaveAndLB->GetBinError(i,j);
	    if (j == me_h_bowingMagnitudePerStaveAndLB->GetNbinsY()) {
	      me_outputTextFile << "\n";
	    }
	    else {
	      me_outputTextFile << ",";
	    }
	  }
	}
	me_outputTextFile.close();
      } 
    } // end of lumiblock analysis
  }
  return;
}

//////////////////////////////////////////
void ME_performLumiBlockAnalysis(int fileid)
{
  bool debug = false;
  TString hname, htitle, cname;
  
  if (debug) {
    std::cout << " ** ME_performLumiBlockAnalysis ** Analysing file id: " << fileid << "  --> " << me_monitoringFileList.at(fileid) << std::endl; 
  }

  // access all IBL vs LB histogram first because it is needed for some initializations (it may be accessed again later)
  TH3F* histo3D =  ME_get3DHistoResidualsVsEtaAndLB (fileid, 0);
  // 
  ME_prepareStaveSummaryHisto (histo3D, fileid);

  // loop on staves 
  for (int stave=0; stave <= NIBLSTAVES; stave++) {
    if (debug) std::cout << " ** ME_performLumiBlockAnalysis ** working on stave " << stave <<std::endl;
    // load 3d histo for staves (stave == 0, all IBL already loaded)
    if (stave > 0) histo3D =  ME_get3DHistoResidualsVsEtaAndLB (fileid, stave);
    ME_prepareMagVsLBhisto (histo3D, fileid, stave);
    me_h_bowingMagnitudePerLB[stave].at(fileid)->Draw();
    
    // lets project the residuals for each stave as a function of the eta-ring and for each lumiblock
    float thisRingMeanValue, thisRingMeanError; 
    for (int lumiblock=1; lumiblock <= histo3D->GetXaxis()->GetNbins(); lumiblock++) {
      // extract lumiblock ID (in case of rebining)
      int lumiblockid = histo3D->GetXaxis()->GetBinCenter(lumiblock);
      
      // reset the bowing magnitude fit results
      me_bowingMagnitudeStave.clear();
      me_bowingMagnitudeValue.clear();
      me_bowingMagnitudeError.clear();
      me_bowingBaseLineValue.clear();
      me_bowingBaseLineError.clear();
      
      hname.Clear();
      hname.Append("h_IBL_ResVsEta_");
      htitle.Clear();
      htitle.Append("IBL mean residual vs ring");
      if (stave == 0) {
	hname.Append("All");
      }
      else {
	hname.Append("stave_"); 
	hname += stave;
	htitle.Append(" ("); htitle += stave; htitle.Append(")");
      }
      hname.Append("_lb_"); hname+= lumiblock;
      htitle.Append(" LumiBlock: "); htitle += lumiblockid;


      TH1F* h_meanResVsRing = new TH1F(hname.Data(), htitle.Data(), histo3D->GetNbinsY(), histo3D->GetYaxis()->GetXmin(), histo3D->GetYaxis()->GetXmax());
      h_meanResVsRing->SetYTitle("mean residual [  #mum]"); 
      h_meanResVsRing->SetXTitle("#eta ring"); 
      
      for (int ring=1; ring <= histo3D->GetNbinsY(); ring++) { 
	// std::cout << " ring -> " << ring << std::endl;
	hname.Clear();
	hname.Append(histo3D->GetName());
	hname.Append("_stave_"); hname += stave;
	hname.Append("_ring_"); hname += ring;
	hname.Append("_lb_"); hname += lumiblockid;
	// std::cout << " --> " << hname.Data() << std::endl;
	if (debug) std::cout << " ** ME_performLumiBlockAnalysis ** analysing stave " << stave << "  ring " << ring << "  lb " << lumiblock << std::endl;
	TH1F* h_residualsAtEta = ME_getHistoResidualsVsEtaAtLB (histo3D, fileid, stave, ring, lumiblock);      
	h_residualsAtEta->SetXTitle("local x residuals [  #mu m]"); 
	
	bool distributionFitted = ME_fitResidualsDistribution(h_residualsAtEta, ring);
	
	if (distributionFitted) {
	  TF1 *fit = h_residualsAtEta->GetFunction("gaus");
	  
	  if (fit != NULL) {
	    thisRingMeanValue = fit->GetParameter(1) * 1000; // --> microns
	    thisRingMeanError = fit->GetParError(1) * 1000; // --> microns
	    if (debug) std::cout << "  ** " << h_residualsAtEta->GetName() 
				 << " ** Residuals Gaussian fit result: mean= " << thisRingMeanValue << " +- " << thisRingMeanError << std::endl;
	  }
	  // store result
	  h_meanResVsRing->SetBinContent(ring, thisRingMeanValue);
	  h_meanResVsRing->SetBinError(ring, thisRingMeanError+0.1); // 0.1 a syst uncertainty of the Gaussian fit
	} 
	if (FULLOUTPUT) {
	  TString cname("IBLXRes_stave_");
	  if (stave == 0) {
	    cname.Append("all");
	  }
	  else {
	    cname += stave;
	  }
	  cname.Append("_ring_");
	  cname += ring;
	  cname.Append("_Run_");
	  cname += me_runNumber.at(fileid);
	  cname.Append("_LB_");
	  cname += lumiblockid;
	  
	  TCanvas thisCanvas(cname.Data(), cname.Data());
	  gPad->SetTicks(1,1);
	  h_residualsAtEta->Draw();
	  // text with result
	  if (distributionFitted) {
	    TString textWithResult("#mu = ");
	    textWithResult += TString::Format("%4.3f",thisRingMeanValue);
	    textWithResult.Append(" +- ");
	    textWithResult += TString::Format("%4.3f",thisRingMeanError);
	    TText aText;
	    aText.SetTextSize(0.03);
	    aText.SetTextColor(kGray+2);
	    aText.DrawTextNDC(0.7,0.5,textWithResult.Data());
	  }
	  thisCanvas.Print(cname+".png");
	}
      } // end of loop on rings
      
      bool bowingMagFitted = ME_fitBowingMagnitude(h_meanResVsRing, stave, fileid, lumiblockid);
      
      if (bowingMagFitted) {
	if (debug) {
	  std::cout << " ** ME_performLumiBlockAnalysis ** bowing mag fitted for stave " << stave << "  and  lb " << lumiblock << std::endl;
	  std::cout << "                                ** bowing mag values vector has size (me_bowingMagnitudeValue.size())= " << me_bowingMagnitudeValue.size() << std::endl;
	}
      }
      //  for (int i=0; i < (int) me_bowingMagnitudeStave.size(); i++) {
      //int stave = me_bowingMagnitudeStave.at(i); // this is because a stave may not be fitted due to stats or whatever
      //me_h_bowingMagnitudeEvolution[stave]->SetBinContent(fileid+1, me_bowingMagnitudeValue.at(i));
      //
      //float total_uncertainty = sqrt(pow(me_bowingMagnitudeError.at(i),2) + pow(me_systUncertaintyBowing,2.));    
      //me_h_bowingMagnitudeEvolution[stave]->SetBinError(fileid+1, total_uncertainty);
      
      if (me_bowingMagnitudeValue.size()>0) {
	//me_h_bowingMagnitudePerLB[stave].at(fileid)->SetBinContent(lumiblock, me_bowingMagnitudeValue.at(stave));
	me_h_bowingMagnitudePerLB[stave].at(fileid)->SetBinContent(lumiblock, me_bowingMagnitudeValue.at(0));
	
	//float total_uncertainty = sqrt(pow(me_bowingMagnitudeError.at(stave),2) + pow(me_systUncertaintyBowing,2.));    
	float total_uncertainty = sqrt(pow(me_bowingMagnitudeError.at(0),2) + pow(me_systUncertaintyBowing,2.));    
	me_h_bowingMagnitudePerLB[stave].at(fileid)->SetBinError(lumiblock, total_uncertainty);

	// fill the summary histogram
	if (me_h_bowingMagnitudePerStaveAndLB != NULL) {
	  me_h_bowingMagnitudePerStaveAndLB->Fill(lumiblockid,stave,me_bowingMagnitudeValue.at(0)); 
	  int thisBin = me_h_bowingMagnitudePerStaveAndLB->FindBin(lumiblockid, stave);
	  me_h_bowingMagnitudePerStaveAndLB->SetBinError(thisBin, total_uncertainty);
	}
      }
      
    } // end of loop in lumiblocks

    
    // open canvas to draw results
    // - canvas name
    cname.Clear();
    cname.Append("BowingMagEvolByLB_Run");
    cname += me_runNumber.at(fileid);
    if (stave == 0){ 
      cname.Append("_IBL");      
    }
    else {
      cname.Append("_stave_");
      cname += stave;
    }
    
    // - open canvas
    TCanvas thisCanvas(cname.Data(), cname.Data());
    
    // draw
    gPad->SetTicks(1,1);
    me_h_bowingMagnitudePerLB[stave].at(fileid)->SetMaximum( me_bowingRangeInPlots/3);
    me_h_bowingMagnitudePerLB[stave].at(fileid)->SetMinimum(-me_bowingRangeInPlots);
    me_h_bowingMagnitudePerLB[stave].at(fileid)->Draw();
    TLine aLine;
    aLine.SetLineColor(kGray+2);
    aLine.SetLineStyle(2);
    
    aLine.DrawLine(me_h_bowingMagnitudePerLB[stave].at(fileid)->GetXaxis()->GetXmin(),0.,
		   me_h_bowingMagnitudePerLB[stave].at(fileid)->GetXaxis()->GetXmax(),0.);
    
    // save plot
    thisCanvas.Print(cname+".png");
  } // end of loop on staves

  if (me_h_bowingMagnitudePerStaveAndLB != NULL) {
    ME_conditioningStaveSummaryHisto ();
    // gStyle->SetPalette(56);
    ME_preparePalette();
    me_h_bowingMagnitudePerStaveAndLB->Draw("colz");
  }
  return;
}

//////////////////////////////////////////
void ME_prepareMagVsLBhisto (TH3F* histo, int fileid, int staveid)
{
  TString hname, htitle;
  hname.Clear();
  hname.Append("hMagVsLumiBlock_Run");
  hname += me_runNumber.at(fileid);
  if (staveid==0) {
    hname.Append("_IBL");
  }
  else{
    hname.Append("_stave");
    hname += staveid;
  }

  TH1F* hmagvslb = new TH1F(hname.Data(),hname.Data(),
			    histo->GetXaxis()->GetNbins(), histo->GetXaxis()->GetXmin(), histo->GetXaxis()->GetXmax());  
  hmagvslb->SetXTitle("Luminosity block");
  hmagvslb->SetYTitle("Bowing magnitude [  #mum]");
  hmagvslb->SetMarkerStyle(kOpenCircle);
  hmagvslb->SetMaximum( me_bowingRangeInPlots);
  //hmagvslb->SetMaximum(  5.);
  hmagvslb->SetMinimum(-me_bowingRangeInPlots);
  hmagvslb->SetStats(kFALSE);
  if (staveid == 0) {
    hmagvslb->SetMarkerColor(me_colorForIBL);
    hmagvslb->SetLineColor(me_colorForIBL);
  }
  if (staveid > 0) {
    hmagvslb->SetMarkerColor(me_colorForStaves);
    hmagvslb->SetLineColor(me_colorForStaves);
  }
  

  me_h_bowingMagnitudePerLB[staveid].push_back(hmagvslb);

  return;
}

//////////////////////////////////////////
TH3F*  ME_get3DHistoResidualsVsEtaAndLB (int fileid, int stave)
{
  bool debug = false; 
  TH3F* h_xresiduals3dLB = NULL;
  // int 0 --> entire IBL
  TString h_path, hname;

  if (me_currentMonitoringFile != NULL) {

    h_path.Clear();
    h_path = "run_"; 
    h_path += me_runNumber.at(fileid); // add run number
    h_path.Append("/IDAlignMon/");    
    h_path += me_trackCollection;

    if (stave == 0) {
          h_path.Append("/Residuals/pix_b0_resXvsetaLumiBlock");
    }
    else {
          h_path.Append("/Residuals/pix_b0_resXvsetaLumiBlock_stave");
      h_path += (stave-1); // the stave number is shifted by 1
    }
    if (debug) std::cout << " ** ME_getHistoResidualsVsEtaAndLB ** analysing file id: " << fileid 
			 << "   stave " << stave 
			 << std::endl
			 << "                          retrieving " << h_path.Data()
			 << std::endl;
    h_xresiduals3dLB = (TH3F*) me_currentMonitoringFile->Get(h_path.Data());

    // WARNING 
    // depending on the InDetAlignMonitoring version h_xresiduals3dLB could be a TProfile2D or a TH3.
    // we are interested in the TH3, so the following check is to verify the histogram is really a TH3
    if (debug && false) std::cout << " ** " << h_xresiduals3dLB->GetName() << " Nbins XYZ "<< h_xresiduals3dLB->GetNbinsX() 
				  << " " <<   h_xresiduals3dLB->GetNbinsY() 
				  << " " <<   h_xresiduals3dLB->GetNbinsZ()
				  << std::endl;
    if (h_xresiduals3dLB->GetNbinsZ() == 1) { // this is not the TH3, so replace it by the TH3
      h_path.Append("_3d");
      if (debug) std::cout << " ** ME_getHistoResidualsVsEtaAndLB ** checking for TH3 " << h_path.Data() << std::endl; 
      h_xresiduals3dLB = NULL;
      h_xresiduals3dLB = (TH3F*) me_currentMonitoringFile->Get(h_path.Data());
      if (debug) std::cout << " ** ME_getHistoResidualsVsEtaAndLB ** replace  h_xresiduals3dLB for the TH3: " << h_xresiduals3dLB->GetName() << std::endl; 
    }

    if (h_xresiduals3dLB == NULL) std::cout << " **  ** ME_getHistoResidualsVsEtaAndLB ** residuals vs LB 3D histo: " << h_path.Data() << " not found" << endl;

    if (h_xresiduals3dLB != NULL) {
      if (debug) std::cout << " **  ** ME_getHistoResidualsVsEtaAndLB ** residuals vs LB 3D histo: " << h_xresiduals3dLB->GetName() << std::endl;
      h_xresiduals3dLB->RebinX(REBINFACTOR);
      h_xresiduals3dLB->Draw("colz");
    }  
  }
  return h_xresiduals3dLB;
}

//////////////////////////////////////////
TH1F*  ME_getHistoResidualsVsEtaAtLB (TH3F* h_xresiduals3dLB, int fileid, int stave, int ring , int lumiblock)
{
  bool debug = false; 
  // int 0 --> entire IBL
  TString hname;
  
  if (debug) std::cout << " ** ME_getHistoResidualsVsEtaAtLB ** analysing file id: " << fileid 
		       << "   stave " << stave 
		       << "   ring " << ring 
		       << "   lb " << lumiblock 
		       << "  on histo: " << h_xresiduals3dLB->GetName() 
		       << std::endl;

  // prepare name
  hname.Clear();
  hname.Append("h_IBL_ResVsEta_");
  if (stave == 0) {
    hname.Append("All");
  }
  else {
    hname.Append("stave_"); 
    hname += stave;
  }
  hname.Append("_run_"); hname += me_runNumber.at(fileid);
  hname.Append("_lb_"); hname+= lumiblock;
  
  TH1F* thisHisto = (TH1F*) h_xresiduals3dLB->ProjectionZ(hname.Data(), lumiblock, lumiblock, ring, ring, ""); 

  if (thisHisto == NULL) std::cout << " ** ME_getHistoResidualsVsEtaAtLB ** WARNING ** file id: " << fileid << " histo with name: " << hname.Data() << " NOT FOUND " << std::endl;
  return thisHisto;
}

//////////////////////////////////////////
void ME_prepareStaveSummaryHisto (TH3F* hinput, int fileid) 
{
  if (NIBLSTAVES > 0) {
    TString hname, htitle;
    hname.Clear();
    hname.Append("hbowingMagnitudePerStaveAndLB");
    hname += me_runNumber.at(fileid);
    htitle.Clear();
    htitle.Append("Bowing magnitude per stave and per LB for Run ");
    htitle += me_runNumber.at(fileid);

    me_h_bowingMagnitudePerStaveAndLB = new TH2F(hname.Data(), htitle.Data(),
						 hinput->GetXaxis()->GetNbins(), hinput->GetXaxis()->GetXmin(), hinput->GetXaxis()->GetXmax(),
						 NIBLSTAVES, 0.5, NIBLSTAVES+0.5);
    me_h_bowingMagnitudePerStaveAndLB->SetStats(kFALSE);
    me_h_bowingMagnitudePerStaveAndLB->SetMaximum( 5.);
    me_h_bowingMagnitudePerStaveAndLB->SetMinimum(-me_bowingRangeInPlots);
    me_h_bowingMagnitudePerStaveAndLB->SetXTitle("Lumiblock");
    me_h_bowingMagnitudePerStaveAndLB->SetYTitle("IBL stave");
  }
  return;
}

//////////////////////////////////////////
void ME_conditioningStaveSummaryHisto ()
{
  // the idea is to make the plot nicer; not filled bins --> outof range --> white
  for (int i=1; i <=  me_h_bowingMagnitudePerStaveAndLB->GetNbinsX(); i++) {
    for (int j=1; j <=  me_h_bowingMagnitudePerStaveAndLB->GetNbinsY(); j++) {
      if (me_h_bowingMagnitudePerStaveAndLB->GetBinContent(i,j) == 0) {
	me_h_bowingMagnitudePerStaveAndLB->SetBinContent(i,j, -1.1*me_bowingRangeInPlots);
      } 
    }
  }
  return;
}

//////////////////////////////////////////
void ME_preparePalette ()
{
  // trafic lights: red -> orange -> green for efficiency plots
  const Int_t NRGBs = 5;
  const Int_t NCont = 99; // 255 originaly but the DrawPannel complaints
  
  Double_t stops[NRGBs] = { 0.70, 0.82, 0.92, 0.97, 1.0};
  Double_t red[NRGBs]   = { 0.80, 0.99, 0.99, 0.05, 0.05};
  Double_t green[NRGBs] = { 0.05, 0.95, 0.90, 0.95, 0.05};
  Double_t blue[NRGBs]  = { 0.05, 0.05, 0.05, 0.05, 0.95};
  
  TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
  gStyle->SetNumberContours(NCont);

  return;
}

//////////////////////////////////////////
void ME_plotDeltaBx ()
{
  TString cname, ctitle, fname;
  TText legend; 
  legend.SetTextColor(kGray+3);
  TText legend_2;
  legend_2.SetTextColor(kGray+2);
  legend_2.SetTextAlign(30);
  legend_2.SetTextAngle(90.);
  legend_2.SetTextSize(0.03);
  TLine aLine;
  aLine.SetLineColor(kGray+2);
  aLine.SetLineStyle(2);
  

  cname.Clear();
  cname.Append("CanDeltaBx");
  ctitle.Clear();
  ctitle.Append("IBL stave ");
  fname.Clear();
  fname.Append("DeltaBxPerStave");
  fname.Append(".png");
  TCanvas* thisCanvas = new TCanvas (cname.Data(), ctitle.Data());
  thisCanvas->cd();
  gPad->SetTicks(1,1);

  // book the histogram
  me_h_deltaBxPerStave = new TH1F ("hDeltaBxPerStave","#Delta Bowing magnitude per IBL stave", NIBLSTAVES, 0.5, NIBLSTAVES+0.5);
  me_h_deltaBxPerStave->SetYTitle("#Delta Bowing magnitude [ #mum]");
  me_h_deltaBxPerStave->SetXTitle("Stave number");
  me_h_deltaBxPerStave->SetStats(kFALSE);
  me_h_deltaBxPerStave->SetMarkerStyle(kFullCircle);
  me_h_deltaBxPerStave->SetLineWidth(2);
  me_h_deltaBxPerStave->SetMarkerColor(me_colorForStaves);
  me_h_deltaBxPerStave->SetLineColor(me_colorForStaves);


  for (int stave=0; stave <= NIBLSTAVES; stave++) {
    ME_computeDeltaBx(stave);
  }

  me_h_deltaBxPerStave->Draw();
  thisCanvas->Print(fname.Data());
  return;
}

//////////////////////////////////////////
void ME_computeDeltaBx (int stave)
{
  if (me_runNumber.size() > 2) { // request at least 2 points
    // computes the DeltaBx between some points and fills the point in the histogram
    int firstPoint = 1; 
    int firstRange = 5; // how many points to use
    int firstPointMax = firstPoint + firstRange;
    if (firstPointMax > (int) me_runNumber.size()) firstPointMax = (int) me_runNumber.size()+1;
    
    int lastPoint = (int) me_runNumber.size();
    int lastRange = 1; // how many points to use
    
    // calculation of initial value
    float initialBx = 0;
    float initialWeight = 0;
    for (int i=firstPoint; i < firstPointMax; i++) {
      //for (int i=firstPoint; i < firstPoint + firstRange; i++) {
      std::cout << " initial use: " << i << std::endl;
      initialBx += me_h_bowingMagnitudeEvolution[stave]->GetBinContent(i);
      initialWeight += pow(1./me_h_bowingMagnitudeEvolution[stave]->GetBinError(i),2);
    }
    initialBx = initialBx / firstRange; // average
    float initialUncertainty = 1./sqrt(initialWeight);
    
    
    float finalBx = 0;
    float finalWeight = 0;
    
    for (int i=lastPoint; i > lastPoint - lastRange; i--) {
      std::cout << " final use: " << i << std::endl;
      finalBx += me_h_bowingMagnitudeEvolution[stave]->GetBinContent(i);
      finalWeight += pow(1./me_h_bowingMagnitudeEvolution[stave]->GetBinError(i),2);
    }
    finalBx = finalBx / lastRange; // average
    float finalUncertainty = 1./sqrt(finalWeight);
    
    float deltaBxVal = finalBx - initialBx; 
    float deltaBxErr = sqrt(pow(initialUncertainty,2)+pow(finalUncertainty,2));
    
    std::cout << " --> initial: " << initialBx << "+-" << initialUncertainty 
	      << "       final: " << finalBx << "+-" << finalUncertainty 
	      << "   DeltaBx: " << deltaBxVal << "+-" << deltaBxErr  << std::endl;
    
    if (stave>0 && me_h_deltaBxPerStave != NULL) {
      me_h_deltaBxPerStave->SetBinContent(stave, deltaBxVal);
      me_h_deltaBxPerStave->SetBinError(stave, deltaBxErr);
    } 
  }
  return;
}
