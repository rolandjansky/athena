/////////////////////////////////////////
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
  ME_prepareOutputFolder();
  ME_prepareBeforeAlignmentHistosFolder();
  ME_prepareAfterAlignmentHistosFolder();
  ME_getMonitoringFileList(); 
  ME_prepareEvolutionHistos();
  ME_prepareHistosPerStave();
  ME_prepareOutputTextFile();
  
  return;
}

//////////////////////////////////////////
void ME_loop()
{
  std::cout << " ** Going to loop on " << me_iter0MonitoringFileList.size() << " files " << std::endl;
  
  // loop on monitoring files
  for (int thisfile=0; thisfile < (int) me_iter0MonitoringFileList.size(); thisfile++) {
    ME_prepareLoop();
    std::cout << std::endl
	      << " ** processing file " << thisfile 
	      << " " << me_iter0MonitoringFileList.at(thisfile).Data() 
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
  bool debug = false;
  if (debug) std::cout << " ** ME_getMonitoringFileList() ** retrieving monitoring file list ** START ** " << std::endl;
 
  // first: define the folers where to look
  std::vector<TString> listOfFolders;

  //me_listOfDataProjects.push_back("data15_13TeV");
  //me_listOfDataStreams.push_back("calibration_IDTracks");

  me_listOfDataProjects.push_back("data16_13TeV");
  me_listOfDataStreams.push_back("calibration_IDTracks");

  me_listOfDataProjects.push_back("data16_13TeV");
  me_listOfDataStreams.push_back("calibration_IDTracks");
  
  TString tempString;
  for (int iproject=0; iproject < (int) me_listOfDataProjects.size(); iproject++) {
    tempString.Clear();
    if (iproject == 0) tempString.Append("/eos/atlas/atlastier0/tzero/caf/IDAlignment/"); 
    if (iproject == 1) tempString.Append("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/"); 
    tempString.Append(me_listOfDataProjects.at(iproject).Data());
    tempString.Append("/");
    tempString.Append(me_listOfDataStreams.at(iproject).Data());
    tempString.Append("/");
    listOfFolders.push_back(tempString.Data());
  }

  if (debug || true) {
    std::cout << " ** ME_getMonitoringFileList() ** list of folders with size: " << listOfFolders.size() << std::endl;
    for (int ifolder=0; ifolder < (int) listOfFolders.size(); ifolder++) {
      std::cout << "   - folder: " << ifolder << " = " <<  listOfFolders.at(ifolder).Data() << std::endl;
    }
    //return;
  }



  // Loop on all folders and get the list of runs
  TString oslsCommand, oslsOutput, oslsRunNumber, iter0Folder, iter0MonFile;
  int theRunNumber;
  for (int ifolder=0; ifolder < (int) listOfFolders.size(); ifolder++) {
    std::cout << " ** checking runs on folder (" << ifolder <<"): " << listOfFolders.at(ifolder).Data() << std::endl;
    oslsCommand.Clear();
    oslsCommand.Append("ls ");
    oslsCommand.Append(listOfFolders.at(ifolder).Data());
    oslsOutput = gSystem->GetFromPipe(oslsCommand.Data());
    Ssiz_t fromV = 0;
    // loop on the output --> extract run numbers
    while (oslsOutput.Tokenize(oslsRunNumber, fromV,"\n")){
      theRunNumber =  oslsRunNumber.Atoi();
      if (debug && true) std::cout << "  --> testing run: string " << oslsRunNumber.Data() << "   integer: " << theRunNumber << std::endl;
      iter0Folder = listOfFolders.at(ifolder);
      iter0Folder.Append(oslsRunNumber.Data());
      iter0Folder.Append("/");
      iter0Folder.Append(me_listOfDataProjects.at(ifolder).Data());
      iter0Folder.Append(".");
      iter0Folder.Append(oslsRunNumber.Data());
      iter0Folder.Append(".");
      iter0Folder.Append(me_listOfDataStreams.at(ifolder).Data());
      iter0Folder.Append(".idalignmerge.ROOT_MON.Iter0.c");  // some runs have .c1 
      int t0passID = ME_getLatestTier0ProcessingID(listOfFolders.at(ifolder), iter0Folder);
      iter0Folder += t0passID;
      iter0Folder.Append("/");
      if (debug && true) std::cout << "  --> iter0 folder: " << iter0Folder.Data() << std::endl;
      

      iter0MonFile = iter0Folder;
      iter0MonFile.Append(me_listOfDataProjects.at(ifolder).Data());
      iter0MonFile.Append(".");
      iter0MonFile.Append(oslsRunNumber.Data());
      iter0MonFile.Append(".");
      iter0MonFile.Append(me_listOfDataStreams.at(ifolder).Data());
      iter0MonFile.Append(".idalignmerge.ROOT_MON.Iter0.c");
      iter0MonFile += t0passID;
      iter0MonFile.Append("._0001.1");  
      if (debug && true) std::cout << "  --> iter0 file: " << iter0MonFile.Data() << std::endl;

      if (debug && false) {
	std::cout << " ** ME_getMonitoringFileList() ** status of gSystem->Which() for run: " << theRunNumber << " and tier0 proc: " << t0passID 
		  << " --> " << gSystem->Which(iter0Folder.Data(), iter0MonFile.Data()) << " <-- " 
		  << std::endl;
      }

      bool iter0MonitoringOK = false;
      if (ME_runIsGood(theRunNumber)) {
	if (gSystem->Which(iter0Folder.Data(), iter0MonFile.Data()) && ME_runIsGood(theRunNumber)) {
	  if (debug) std::cout << " ** ME_getMonitoringFileList() ** Monitoring file "<< iter0MonFile.Data() <<" is accesible :) " << std::endl;
	  iter0MonitoringOK = true;
	}
	else {
	  if (debug) std::cout << " ** ME_getMonitoringFileList ** Problems retrieving iter0 monitoring file: "<< iter0MonFile.Data() << std::endl;
	  // This may be a consequence of the new dynamic alignment scheme at tier0 where the Iter0 files are split in lumiblocks 
	  iter0MonFile = ME_getIter0MonitoringFilesChain(iter0Folder, theRunNumber);
	  if (debug) std::cout << " ** ME_getMonitoringFileList ** chained Iter0: "<< iter0MonFile.Data() << std::endl;
	  iter0MonitoringOK = true;
	}
      }

      if (iter0MonitoringOK){ // if file exists and run is not flagged as bad
	// So far the iter0 files are fine
	// before registering the iter0 files, let's check what happens with Iter3
	
	TString iter3Folder = ME_iter3FolderName (listOfFolders.at(ifolder), oslsRunNumber, me_listOfDataProjects.at(ifolder), me_listOfDataStreams.at(ifolder), t0passID);
	if (debug) std::cout << " ** ME_getMonitoringFileList() ** iter3Folder = " << iter3Folder.Data() << std::endl;
	if (iter3Folder.Length()>0 || !ITER3ANALYSIS) {  // when no AfterAlignment is wanted, initial files are then used
	//	if ((iter3Folder.Length()>0 || !ITER3ANALYSIS) && theRunNumber <= 303560) {  // when no AfterAlignment is wanted, initial files are then used
	  me_iter0MonitoringFileList.push_back(iter0MonFile);
	  me_runNumber.push_back(theRunNumber);
	  me_runDateTime.push_back(ME_getRunDateTime(iter0Folder));
	  if (iter3Folder.Length()>0) { // use AfterAlignment file is they exist
	    ME_getIter3MonitoringFiles(iter3Folder, theRunNumber);
	  }
	}
      }
    }
  }
  

  // OLD STUFF
  //me_iter0MonitoringFileList.push_back("/afs/cern.ch/work/m/martis/public/athena/BowTest/runTest/Iter0_284213_BX_25NS/collisions/TotalMonitoring.root"); me_runNumber.push_back(284213);
  /*
  // now, for all files, extract run number... this method will depend on the way the file name is given  
  for (int thisfile=0; thisfile < (int) me_iter0MonitoringFileList.size(); thisfile++) {
    me_runNumber.push_back(ME_extractRunNumber(thisfile));
  }
  */

  // OLD STUFF
  //me_iter0MonitoringFileList.push_back("/Users/martis/Downloads/Run302872_beforeAlignment_2.root"); me_runNumber.push_back(302872);

  /*
  me_iter0MonitoringFileList.push_back("/afs/cern.ch/user/m/martis/mywork/data16_13TeV_20.7.5.7/InnerDetector/InDetExample/InDetAlignExample/L1byLB/Iter0_LB_181_190/collisions/TotalMonitoring.root"); me_runNumber.push_back(302872);
 
  me_iter0MonitoringFileList.push_back("/afs/cern.ch/user/m/martis/mywork/data16_13TeV_20.7.5.7/InnerDetector/InDetExample/InDetAlignExample/L1byLB/Iter0_LB_2291_2300/collisions/TotalMonitoring.root"); me_runNumber.push_back(302872);
  */
  /*
  me_iter0MonitoringFileList.push_back("/afs/cern.ch/user/m/martis/mywork/data16_13TeV_20.7.5.7/InnerDetector/InDetExample/InDetAlignExample/L1_Run303560/Iter0_LB_131_180/collisions/TotalMonitoring.root"); me_runNumber.push_back(303560);
  me_iter0MonitoringFileList.push_back("/afs/cern.ch/user/m/martis/mywork/data16_13TeV_20.7.5.7/InnerDetector/InDetExample/InDetAlignExample/L1_Run303560/Iter0_LB_181_230/collisions/TotalMonitoring.root"); me_runNumber.push_back(303560);
  me_iter0MonitoringFileList.push_back("/afs/cern.ch/user/m/martis/mywork/data16_13TeV_20.7.5.7/InnerDetector/InDetExample/InDetAlignExample/L1_Run303560/Iter0_LB_231_280/collisions/TotalMonitoring.root"); me_runNumber.push_back(303560);
  me_iter0MonitoringFileList.push_back("/afs/cern.ch/user/m/martis/mywork/data16_13TeV_20.7.5.7/InnerDetector/InDetExample/InDetAlignExample/L1_Run303560/Iter0_LB_281_330/collisions/TotalMonitoring.root"); me_runNumber.push_back(303560);
  me_iter0MonitoringFileList.push_back("/afs/cern.ch/user/m/martis/mywork/data16_13TeV_20.7.5.7/InnerDetector/InDetExample/InDetAlignExample/L1_Run303560/Iter0_LB_331_380/collisions/TotalMonitoring.root"); me_runNumber.push_back(303560);
  me_iter0MonitoringFileList.push_back("/afs/cern.ch/user/m/martis/mywork/data16_13TeV_20.7.5.7/InnerDetector/InDetExample/InDetAlignExample/L1_Run303560/Iter0_LB_381_430/collisions/TotalMonitoring.root"); me_runNumber.push_back(303560);
  me_iter0MonitoringFileList.push_back("/afs/cern.ch/user/m/martis/mywork/data16_13TeV_20.7.5.7/InnerDetector/InDetExample/InDetAlignExample/L1_Run303560/Iter0_LB_431_480/collisions/TotalMonitoring.root"); me_runNumber.push_back(303560);
  me_iter0MonitoringFileList.push_back("/afs/cern.ch/user/m/martis/mywork/data16_13TeV_20.7.5.7/InnerDetector/InDetExample/InDetAlignExample/L1_Run303560/Iter0_LB_481_530/collisions/TotalMonitoring.root"); me_runNumber.push_back(303560);
  me_iter0MonitoringFileList.push_back("/afs/cern.ch/user/m/martis/mywork/data16_13TeV_20.7.5.7/InnerDetector/InDetExample/InDetAlignExample/L1_Run303560/Iter0_LB_531_580/collisions/TotalMonitoring.root"); me_runNumber.push_back(303560);
  me_iter0MonitoringFileList.push_back("/afs/cern.ch/user/m/martis/mywork/data16_13TeV_20.7.5.7/InnerDetector/InDetExample/InDetAlignExample/L1_Run303560/Iter0_LB_581_630/collisions/TotalMonitoring.root"); me_runNumber.push_back(303560);
  me_iter0MonitoringFileList.push_back("/afs/cern.ch/user/m/martis/mywork/data16_13TeV_20.7.5.7/InnerDetector/InDetExample/InDetAlignExample/L1_Run303560/Iter0_LB_631_680/collisions/TotalMonitoring.root"); me_runNumber.push_back(303560);
  me_iter0MonitoringFileList.push_back("/afs/cern.ch/user/m/martis/mywork/data16_13TeV_20.7.5.7/InnerDetector/InDetExample/InDetAlignExample/L1_Run303560/Iter0_LB_681_730/collisions/TotalMonitoring.root"); me_runNumber.push_back(303560);
  me_iter0MonitoringFileList.push_back("/afs/cern.ch/user/m/martis/mywork/data16_13TeV_20.7.5.7/InnerDetector/InDetExample/InDetAlignExample/L1_Run303560/Iter0_LB_731_780/collisions/TotalMonitoring.root"); me_runNumber.push_back(303560);
  me_iter0MonitoringFileList.push_back("/afs/cern.ch/user/m/martis/mywork/data16_13TeV_20.7.5.7/InnerDetector/InDetExample/InDetAlignExample/L1_Run303560/Iter0_LB_781_830/collisions/TotalMonitoring.root"); me_runNumber.push_back(303560);
  me_iter0MonitoringFileList.push_back("/afs/cern.ch/user/m/martis/mywork/data16_13TeV_20.7.5.7/InnerDetector/InDetExample/InDetAlignExample/L1_Run303560/Iter0_LB_831_880/collisions/TotalMonitoring.root"); me_runNumber.push_back(303560);
  me_iter0MonitoringFileList.push_back("/afs/cern.ch/user/m/martis/mywork/data16_13TeV_20.7.5.7/InnerDetector/InDetExample/InDetAlignExample/L1_Run303560/Iter0_LB_881_930/collisions/TotalMonitoring.root"); me_runNumber.push_back(303560);
  me_iter0MonitoringFileList.push_back("/afs/cern.ch/user/m/martis/mywork/data16_13TeV_20.7.5.7/InnerDetector/InDetExample/InDetAlignExample/L1_Run303560/Iter0_LB_931_980/collisions/TotalMonitoring.root"); me_runNumber.push_back(303560);
  */
  //me_iter0MonitoringFileList.push_back("/afs/cern.ch/user/m/martis/mywork/data16_13TeV_20.7.5.7/InnerDetector/InDetExample/InDetAlignExample/L11_Run303846/Run_303846_Iter1_TotalMonitoring.root"); me_runNumber.push_back(303846);

  if (debug) std::cout << "  ** ME_getMonitoringFileList() ** monitoring file list contains " << me_iter0MonitoringFileList.size() << " files " << std::endl;
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
  std::cout << "          >  NIBLSTAVES : " <<  NIBLSTAVES << std::endl;
  std::cout << "          >  USEPLANARONLY : " <<  USEPLANARONLY << std::endl;
  std::cout << "          >  LUMIBLOCKANALYSIS : " <<  LUMIBLOCKANALYSIS << std::endl;
  std::cout << "          >  ITER3ANALYSIS : " <<  ITER3ANALYSIS << std::endl;
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
  
  // check if hostname is aiatla012
  if (me_HOSTNAME.Index("aiatlas012") >= 0) {
    std::cout << " -- executing in aiatlas012 -> exit() -- " << std::endl << std::endl;
    exit(0);
  }

  return;
}

//////////////////////////////////////////
void ME_clear()
{
  // clear the vectors data (just in case) 
  me_iter0MonitoringFileList.clear();
  me_iter3MonitoringFileList.clear();
  me_runNumber.clear();
  me_iter3runNumber.clear();
  

  return;
}

//////////////////////////////////////////
void ME_prepareOutputTextFile()
{
  if (WRITETEXTOUTPUT) {
    // open output text file (overwrite existing file)
    me_outputTextFile.open ("MagnitudeEvolutionByRun.txt");

    // in time format
    me_outputTextFileTimeFormat.open("MagnitudeEvolutionByRunAndTime.txt");
  }
  return;
}

//////////////////////////////////////////
/*
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
    startingpoint = me_iter0MonitoringFileList.at(fileid).Index("Iter0_");
    myrunnumber = ((TString) me_iter0MonitoringFileList.at(fileid)(startingpoint+6,6)).Atoi();
    if (debug) std::cout << " ** starting point for run number: " << startingpoint << " --> " << ((TString) me_iter0MonitoringFileList.at(fileid)(startingpoint+6,6)) << std::endl;
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
*/
//////////////////////////////////////////
void ME_processFile(int fileid)
{
  bool debug = false;

  // open the monitoring file
  me_currentMonitoringFile = TFile::Open(me_iter0MonitoringFileList.at(fileid).Data());

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
    thisCanvas.Print(me_outputFolder + cname+".png");
  }

  // now histogram has to be projected in order to get a residual average vs module eta
  //h_xresiduals3d->ProjectionZ("",0,-1,0,-1,"ed");
  // h_xresiduals3d->Project3DProfile("yx");

  for (int stave = 0; stave <= NIBLSTAVES; stave++) {
    me_h_IBLmeanResPerRing[stave] = ME_getIBLMeanResPerRing(h_xresiduals3d, stave, fileid); 
    ME_fitBowingMagnitude(me_h_IBLmeanResPerRing[stave], stave, fileid, -1, "");
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
      thisCanvas.Print(me_outputFolder + cname+".png");
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

  // in case Iter3 analysis (final iteration) is wanted
  if ( ITER3ANALYSIS) {
    ME_performIter3Analysis(fileid);
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
    if (debug && h_xresiduals3d != NULL) std::cout << " ** method 2 works ** 3D histogram with residuals captured: " << h_xresiduals3d->GetName() << std::endl; 
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
    if (debug && h_xresiduals3d != NULL) std::cout << " ** method 3 works ** 3D histogram with residuals captured: " << h_xresiduals3d->GetName() << std::endl; 
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
    if (debug && h_xresiduals3d != NULL) std::cout << " ** method 4 works ** 3D histogram with residuals captured: " << h_xresiduals3d->GetName() << std::endl; 
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
    h_residualsAtEta->SetXTitle("local x residuals [ #mum]"); 
    bool distributionFitted = ME_fitResidualsDistribution(h_residualsAtEta, ring);
    float thisRingMeanValue = 0.0; 
    float thisRingMeanError = 0.0; 
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
	cname.Append("All");
      }
      else {
	// cname += staveid;
	cname += TString::Format("%02d",staveid);
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
      thisCanvas.Print(me_outputFolder + cname+".png");
    }
  }
  
  h_meanResVsRing->SetYTitle("mean residual [ #mum]"); 
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
bool ME_fitBowingMagnitude (TH1F* h_input, int staveid, int fileid, int lumiblockid, TString theSuffix)
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
    double magValue = 0, magError = 0;
    double baseLineValue = 0, baseLineError = 0;
    double tiltValue = 0, tiltError= 0; bool tiltFitted = false;
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
	if (FITTYPE == 5) { // the fit values have been replaced in MakeStaveFit by the residulas at the centre of the stave
	  baseLineValue =  fitFunction->GetParameter(1);
	  baseLineError =  fitFunction->GetParError(1);
	  magValue =  fitFunction->GetParameter(2);
	  magError =  fitFunction->GetParError(2); 
	}
	if (FITTYPE == 6) {
	  baseLineValue =  fitFunction->GetParameter(1);
	  baseLineError =  fitFunction->GetParError(1);
	  magValue =  fitFunction->GetParameter(2);
	  magError =  fitFunction->GetParError(2);
	  tiltFitted = true;
	  tiltValue = fitFunction->GetParameter(3);
	  tiltError = fitFunction->GetParError(3);
	}

	if (debug) std::cout << " ** ME_fitBowingMagnitude ** fit ok ** MAGNITUDE: " << magValue << " +- " << magError << std::endl;
	if (debug) std::cout << " ** ME_fitBowingMagnitude ** fit ok ** BASELINE:  " << baseLineValue << " +- " << baseLineError << std::endl;

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
	  cname.Append("All");
	}
	else {
	  //cname += staveid;
	  cname += TString::Format("%02d",staveid);
	}
	cname.Append(theSuffix.Data());
	cname.Append("_Run_");
	cname += me_runNumber.at(fileid);
	if (lumiblockid >= 0) {
	  cname.Append("_LB_");
	  //cname += lumiblockid;
	  cname += TString::Format("%04d", lumiblockid);
	}
	TCanvas thisCanvas(cname.Data(), cname.Data());
	thisGraph->GetXaxis()->SetLimits(-365.,365.); // full stave size
	thisGraph->Draw();
	//
	// now draw the fitting function in the entire stave length
	fitFunction->SetRange(-365.,365.);
	fitFunction->Draw("same");
	//

	float theTextY = 0.86; if (tiltFitted) theTextY = 0.87;
	float theTextGap = -0.06; if (tiltFitted) theTextGap = -0.055;
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
	aText.DrawTextNDC(0.5, theTextY,textWithResult.Data());

	if (baseLineValue != 0) {
	  textWithResult.Clear();
	  textWithResult.Append("base line = ");
	  textWithResult += TString::Format("%4.3f",baseLineValue);
	  textWithResult.Append(" +- ");
	  textWithResult += TString::Format("%4.3f",baseLineError);
	  textWithResult.Append(" microns ");
	  theTextY += theTextGap;
	  aText.DrawTextNDC(0.5, theTextY,textWithResult.Data());
	}
	if (tiltFitted) {
	  textWithResult.Clear();
	  textWithResult.Append("tilt = ");
	  textWithResult += TString::Format("%4.3f",tiltValue*1000); // multiply by 1000 -> move from milli-radians to micro-radians
	  textWithResult.Append(" +- ");
	  textWithResult += TString::Format("%4.3f",tiltError*1000);
	  textWithResult.Append(" microradians ");
	  theTextY += theTextGap;
	  aText.DrawTextNDC(0.5, theTextY, textWithResult.Data());
	}

	// coordinates of the baseLine line (with tilt) 
	double lineXlow = -z_fix; double lineXupper = z_fix;
	double lineYlow = baseLineValue + lineXlow * tiltValue;
	double lineYupper = baseLineValue + lineXupper * tiltValue;

	// aLine.DrawLine(-365.,baseLineValue, 365., baseLineValue);
	aLine.DrawLine(lineXlow, lineYlow, lineXupper, lineYupper);
	anArrow.DrawArrow(0., baseLineValue, 0., baseLineValue+magValue);

	thisCanvas.Print(me_outputFolder + cname+".png");
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
  xmax =  me_bowingRangeInPlots/2;
  graph->GetYaxis()->SetRangeUser(xmin,xmax);
  graph->GetXaxis()->SetRangeUser(-z_fix,z_fix);
  graph->GetYaxis()->SetTitle("Local X Residual mean [  #mum]");
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
  if (type==2) {
      if (debug) std::cout<<"FIT HIDE FORMULA"<< std::endl;
      fit = new TF1("fitBowing", "[0]-[1]*(x+[2]) * (4.0*[2]*(x+[2])**2 - (x+[2])**3 - (2.0*[2])**3)", -z_fix, z_fix );
      fit->FixParameter(2, z_fix);
      fit->SetParameter(1, 5.e-12);
  }
  else if (type == 1) {
      fit = new TF1("fitBowing", "-[1]*[0]-[2]*[0]*[0] +[1]*x+[2]*x*x",-z_fix,z_fix); 
  }
  else if (type == 3 | type == 6) {
      if (debug) std::cout<<"Fitting SH formula"<<std::endl;
      fit = new TF1("fitBowing", "[1] - ([2]*(x*x-[0]*[0]))/([0]*[0]) + [3]*x",-z_fix,z_fix);
      fit->FixParameter(0,z_fix);
      if (type == 3) fit->FixParameter(3, 0);
  }
  else if (type ==4) {
      fit = new TF1("fitBowing", "[0]+[1]*( (x+[2]/2.0)**2 ) * ( ([2]-(x+[2]/2.0))**2 )", -z_fix, z_fix );
      fit->FixParameter(2, z_fix*2);      
  }
  else if (type == 5) {
    // this is as type == 3 
    // but returned fit values are going to be replaced by the residuals at the center of the stave
    if (debug) std::cout<<"Fitting SH formula"<<std::endl;
    fit = new TF1("fitBowing", "[1] - ([2]*(x*x-[0]*[0]))/([0]*[0])",-z_fix,z_fix);
    fit->FixParameter(0,z_fix);
  }

  else {
        fit = new TF1("fitBowing","[0]+[1]+[2]",-z_fix,z_fix);
	std::cout<<"No fit will be made"<<std::endl;
        fixbase=1;
        fit->FixParameter(2,0);
  }
  
 
  /*
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
  */

  //  TFitResultPtr r; 
  if (debug) std::cout<<" ** MakeStaveFit ** FITTING... " << std::endl;
  TFitResultPtr r = inputGraph->Fit(fit,"QEX0S");

  if (type == 5) {
    // this replaces the fit values by the residuals at the center of the stave
    if (debug || true) std::cout << "  ** MakeStaveFit ** TYPE 5 " << std::endl;
    Double_t zofpoint;
    Double_t rofpoint;
    std::vector<double> validResidual;
    std::vector<double> weightOfResidual;
    
    for (int i=0; i < inputGraph->GetN(); i++) {
      inputGraph->GetPoint(i, zofpoint, rofpoint);
      if (fabs(zofpoint)< 25.) {
	validResidual.push_back(rofpoint);
	weightOfResidual.push_back(pow(1./ inputGraph->GetErrorY(i), 2));
	// std::cout << "  ** MakeStaveFit ** z " << zofpoint << "  r: " << rofpoint << std::endl;
      }
    } 
    // now compute the weighted residual
    double weightedResidual = 0;
    double sumOfWeights = 0;
    for (int i=0; i < (int) validResidual.size(); i++) {
      weightedResidual += validResidual.at(i) * weightOfResidual.at(i);
      sumOfWeights +=  weightOfResidual.at(i);
    }
    inputGraph->GetFunction("fitBowing")->SetParameter(2, weightedResidual / sumOfWeights); // Magnitude --> average residual
    inputGraph->GetFunction("fitBowing")->SetParError(2, 1./sqrt(sumOfWeights));
    inputGraph->GetFunction("fitBowing")->SetParameter(1, 0.); // baseline
    inputGraph->GetFunction("fitBowing")->SetParError(1, 1./sqrt(sumOfWeights)); // set same error as for magnitude 
  }
     
  return graphGoodForFit;
}  

//////////////////////////////////////////
void ME_prepareEvolutionHistos()
{
  std::cout << " ** preparing evolution histograms for " << me_iter0MonitoringFileList.size() << " runs " << std::endl;
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
						    me_iter0MonitoringFileList.size(), -0.5, me_iter0MonitoringFileList.size()-0.5);

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
						    me_iter0MonitoringFileList.size(), -0.5, me_iter0MonitoringFileList.size()-0.5);


    // use run number as x-axis bin label
    for (int bin=1; bin <= (int) me_iter0MonitoringFileList.size(); bin++) {
      binLabel.Clear();
      binLabel += me_runNumber.at(bin-1);
      me_h_bowingMagnitudeEvolution[stave]->GetXaxis()->SetBinLabel(bin, binLabel.Data());
      me_h_bowingBaseLineEvolution[stave]->GetXaxis()->SetBinLabel(bin, binLabel.Data());
    }
    // style
    me_h_bowingMagnitudeEvolution[stave]->SetStats(kFALSE);
    me_h_bowingMagnitudeEvolution[stave]->SetYTitle("Bowing magnitude [#mum]");
    me_h_bowingMagnitudeEvolution[stave]->SetXTitle("Run number");
    me_h_bowingMagnitudeEvolution[stave]->SetMarkerStyle(kFullCircle);
    me_h_bowingMagnitudeEvolution[stave]->SetLineWidth(2);

    me_h_bowingBaseLineEvolution[stave]->SetStats(kFALSE);
    me_h_bowingBaseLineEvolution[stave]->SetYTitle("Base line [ #mum]");
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
  bool debug = false;
  if (debug) std::cout << " ** preparing histograms per stave. There are in total " << me_iter0MonitoringFileList.size() << " runs " << std::endl;
  if (NIBLSTAVES > 0 ) {
    // this histograms are created if user wants to perform an stave by stave analysis
    TString hname, htitle, binLabel;
  
    
    for (int file = 0; file < (int)  me_iter0MonitoringFileList.size(); file++) {
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
      h_bowingMagnitudePerStave->SetYTitle("Bowing magnitude [ #mum]");
      h_bowingMagnitudePerStave->SetXTitle("Stave number");
      h_bowingMagnitudePerStave->SetMarkerStyle(kFullCircle);
      h_bowingMagnitudePerStave->SetLineWidth(2);
      h_bowingMagnitudePerStave->SetMarkerColor(me_colorForStaves);
      h_bowingMagnitudePerStave->SetLineColor(me_colorForStaves);

      h_baseLinePerStave->SetStats(kFALSE);
      h_baseLinePerStave->SetYTitle("Base line [ #mum]");
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

  me_bowingMagnitudeValue_AfterAlignment.clear();
  return;
}

//////////////////////////////////////////
void ME_plotEvolutionHistos()
{
  bool debug = false;
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
      //cname += stave;
      cname += TString::Format("%02d",stave);
      ctitle += stave;
      ctitle.Append("bowing magnitude evolution vs run");
      //fname += stave;
      fname += TString::Format("%02d",stave);
    }
    fname.Append(".png");
    TCanvas* thisCanvas = new TCanvas (cname.Data(), ctitle.Data());
    thisCanvas->cd();
    gPad->SetTicks(1,1);
    me_h_bowingMagnitudeEvolution[stave]->SetMaximum( me_bowingRangeInPlots/1.); // used to be /3
    me_h_bowingMagnitudeEvolution[stave]->SetMinimum(-me_bowingRangeInPlots);
    me_h_bowingMagnitudeEvolution[stave]->Draw("");
    aLine.DrawLine(me_h_bowingMagnitudeEvolution[stave]->GetXaxis()->GetXmin(),0.,
		   me_h_bowingMagnitudeEvolution[stave]->GetXaxis()->GetXmax(),0.);
    //    legend.DrawText(0.15*me_iter0MonitoringFileList.size(),17.,"Period E");
    //legend.DrawText(0.56*me_iter0MonitoringFileList.size(),17.,"Period F");
    //legend.DrawText(0.77*me_iter0MonitoringFileList.size(),17.,"Period G");
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
    thisCanvas->Print(me_outputFolder + fname.Data());

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
      //fname += stave;
      fname += TString::Format("%02d",stave); 
    }
    fname.Append(".png");
    TCanvas* thisCanvas2 = new TCanvas (cname.Data(), ctitle.Data());
    thisCanvas2->cd();
    gPad->SetTicks(1,1);

    me_h_bowingBaseLineEvolution[stave]->SetMaximum( me_bowingRangeInPlots*3/4); // used to be /3
    me_h_bowingBaseLineEvolution[stave]->SetMinimum(-me_bowingRangeInPlots);
    me_h_bowingBaseLineEvolution[stave]->Draw("");
    aLine.DrawLine(me_h_bowingBaseLineEvolution[stave]->GetXaxis()->GetXmin(),0.,
		   me_h_bowingBaseLineEvolution[stave]->GetXaxis()->GetXmax(),0.);
    aLine.DrawLine(16.5, -me_bowingRangeInPlots, 16.5, me_bowingRangeInPlots/3);
    aLine.DrawLine(20.5, -me_bowingRangeInPlots, 20.5, me_bowingRangeInPlots/3);
    thisCanvas2->Print(me_outputFolder + fname.Data());

  }

  //  
  // Repeat the plots for AfterAlignment results (Iter3)
  //
  if (debug) std::cout << " ** ME_plotEvolutionHistos ** plot after alignment histos? " <<  ITER3ANALYSIS << std::endl;

  for (int stave=0; stave <= NIBLSTAVES && ITER3ANALYSIS; stave++) {
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

    cname.Append("_AfterAlignment");
    fname.Append("_AfterAlignment");
    ctitle.Append(" (after alignment");

    fname.Append(".png");
    if (debug) std::cout << " ** ME_plotEvolutionHistos ** plot after alignment output file name for bowing magnitude evolution: " << fname.Data() 
				 << "  (stave= " << stave << ") "
				 << std::endl;

    TCanvas* thisCanvas = new TCanvas (cname.Data(), ctitle.Data());
    thisCanvas->cd();
    gPad->SetTicks(1,1);
    if (debug) std::cout << " ** ME_plotEvolutionHistos ** gPad->SetTicks(1,1) " << std::endl;
 
    if (me_h_bowingMagnitudeEvolution_AfterAlignment[stave] != NULL) {
      std::cout << " ** ME_plotEvolutionHistos ** me_h_bowingMagnitudeEvolution_AfterAlignment[" << stave << "]= " << me_h_bowingMagnitudeEvolution_AfterAlignment[stave] << std::endl;
      std::cout << " ** ME_plotEvolutionHistos ** me_h_bowingMagnitudeEvolution_AfterAlignment[" << stave << "] is ? " << std::endl;
      std::cout << " ** ME_plotEvolutionHistos ** me_h_bowingMagnitudeEvolution_AfterAlignment[" << stave << "]->GetEntries()= " 
		<< me_h_bowingMagnitudeEvolution_AfterAlignment[stave]->GetEntries() << std::endl;
      me_h_bowingMagnitudeEvolution_AfterAlignment[stave]->SetMaximum( me_bowingRangeInPlots/1); // used to be /3
      me_h_bowingMagnitudeEvolution_AfterAlignment[stave]->SetMinimum(-me_bowingRangeInPlots);
      me_h_bowingMagnitudeEvolution_AfterAlignment[stave]->Draw("");
      aLine.DrawLine(me_h_bowingMagnitudeEvolution_AfterAlignment[stave]->GetXaxis()->GetXmin(),0.,
		     me_h_bowingMagnitudeEvolution_AfterAlignment[stave]->GetXaxis()->GetXmax(),0.);
    }
    else {
      std::cout << std::endl;
      std::cout << " ** ME_plotEvolutionHistos ** WARNING me_h_bowingMagnitudeEvolution_AfterAlignment[" << stave << "] = NULL ** WARNING ** " << std::endl;
      std::cout << std::endl;
    }

    thisCanvas->Print(me_outputFolder + fname.Data());

  }

  if (debug) std::cout << " ** ME_plotEvolutionHistos ** calling drawEvolInTimeFormat() " << std::endl;
  ME_drawEvolInTimeFormat();

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

      canvasBM->Print(me_outputFolder + fname.Data());

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

      canvasBL->Print(me_outputFolder + fname.Data());

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
      if (stave == 0) {
	me_outputTextFile << " - Stave - ALL - (" << stave  << ") \n";
      }
      else {
	me_outputTextFile << " - Stave - " << stave << "\n";
      }
      for (int i=0; i < (int) me_iter0MonitoringFileList.size(); i++) {
	if (me_h_bowingMagnitudeEvolution[stave]->GetBinContent(i+1) != 0) { // this bin/run was filled
	  me_outputTextFile << me_runNumber.at(i) <<","
			    << me_h_bowingMagnitudeEvolution[stave]->GetBinContent(i+1) <<","
			    << me_h_bowingMagnitudeEvolution[stave]->GetBinError(i+1)
			    << "\n";
	}
      }
    }
    me_outputTextFile.close();

    // time evolution
    for (int stave=0; stave <= NIBLSTAVES; stave++) {
      if (stave == 0) {
	me_outputTextFileTimeFormat << " - Stave - ALL - (" << stave  << ") \n";
      }
      else {
	me_outputTextFileTimeFormat << " - Stave - " << stave << "\n";
      }
      for (int i=0; i < (int) me_iter0MonitoringFileList.size(); i++) {
	if (me_h_bowingMagnitudeEvolution[stave]->GetBinContent(i+1) != 0) { // this bin/run was filled
	  me_outputTextFileTimeFormat << me_runNumber.at(i) <<", "
				      << me_runDateTime.at(i).GetDay() << "/" << me_runDateTime.at(i).GetMonth() << "/" << me_runDateTime.at(i).GetYear() <<", "  
				      << me_h_bowingMagnitudeEvolution[stave]->GetBinContent(i+1) <<","
				      << me_h_bowingMagnitudeEvolution[stave]->GetBinError(i+1)
				      << "\n";
	}
      }
    }
    me_outputTextFileTimeFormat.close();

    //
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
	      me_outputTextFile << ","; //","
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
    std::cout << " ** ME_performLumiBlockAnalysis ** Analysing file id: " << fileid << "  --> " << me_iter0MonitoringFileList.at(fileid) << std::endl; 
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
    float thisRingMeanValue = 0.0;
    float thisRingMeanError = 0.0; 
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
      h_meanResVsRing->SetYTitle("mean residual [ #mum]"); 
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
	h_residualsAtEta->SetXTitle("local x residuals [ #mum]"); 
	
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
	  cname.Clear();
	  cname.Append("IBLXRes_stave_");
	  if (stave == 0) {
	    cname.Append("All");
	  }
	  else {
	    //cname += stave;
	    cname += TString::Format("%02d",stave);
	  }
	  cname.Append("_ring_");
	  cname += ring;
	  cname.Append("_Run_");
	  cname += me_runNumber.at(fileid);
	  cname.Append("_LB_");
	  //cname += lumiblockid;
	  cname += TString::Format("%04d",lumiblockid);
	  
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
	  thisCanvas.Print( me_outputFolder + cname+".png");
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
      //cname += stave;
      cname += TString::Format("%02d",stave);
    }
    
    // - open canvas
    TCanvas thisCanvas(cname.Data(), cname.Data());
    
    // draw
    gPad->SetTicks(1,1);
    me_h_bowingMagnitudePerLB[stave].at(fileid)->SetMaximum( me_bowingRangeInPlots/3.); // used to be /3
    me_h_bowingMagnitudePerLB[stave].at(fileid)->SetMinimum(-me_bowingRangeInPlots);
    me_h_bowingMagnitudePerLB[stave].at(fileid)->Draw();
    TLine aLine;
    aLine.SetLineColor(kGray+2);
    aLine.SetLineStyle(2);
    
    aLine.DrawLine(me_h_bowingMagnitudePerLB[stave].at(fileid)->GetXaxis()->GetXmin(),0.,
		   me_h_bowingMagnitudePerLB[stave].at(fileid)->GetXaxis()->GetXmax(),0.);
    
    // save plot
    thisCanvas.Print(me_outputFolder + cname+".png");
  } // end of loop on staves

  if (me_h_bowingMagnitudePerStaveAndLB != NULL) {
    ME_conditioningStaveSummaryHisto ();
    // gStyle->SetPalette(56);
    ME_preparePalette(me_h_bowingMagnitudePerStaveAndLB->GetMinimum(), me_h_bowingMagnitudePerStaveAndLB->GetMaximum()); 
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
  hmagvslb->SetYTitle("Bowing magnitude [ #mum]");
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
    float ColorStep = 50./13;
    int   theColor = int(51+(staveid-1)*ColorStep);
    if (theColor>100) theColor=100;
    if (staveid == 2) theColor=kBlack;
    std::cout << " -- color -- stave " << staveid << " theColor " << theColor << std::endl;
    hmagvslb->SetMarkerColor(theColor);
    hmagvslb->SetLineColor(theColor);
    hmagvslb->SetMarkerStyle(kFullCircle);
    hmagvslb->SetLineWidth(2);
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
    //me_h_bowingMagnitudePerStaveAndLB->SetMaximum( 7.5);
    me_h_bowingMagnitudePerStaveAndLB->SetMaximum( me_bowingRangeInPlots);
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
void ME_preparePalette (float theMin, float theMax)
{
  // trafic lights: red -> orange -> green for efficiency plots
  const Int_t NRGBs = 7;
  const Int_t NCont = 99; // 255 originaly but the DrawPannel complaints

  bool debug = false;
  if (debug) std::cout << " ** ME_preparePalette ** range: " << theMin << " --> " << theMax << std::endl;
  float theGap = theMax - theMin;
  float theLevel0 = -theMin/theGap;
  float theLower = theLevel0/2; if (theLower < 0.25) theLower = 0.25;
  float theBottom = theLevel0/4; if (theBottom < 0.10) theBottom = 0.10;
  float theUpper = 3*theLevel0/2; if (theUpper < 0.85) theUpper = 0.85;
  
  //Double_t stops[NRGBs] = { 0.70, 0.82, 0.85, theLevel0, 1.0};
  //Double_t red[NRGBs]   = { 0.80, 0.99, 0.99, 0.05, 0.05};
  //Double_t green[NRGBs] = { 0.05, 0.95, 0.90, 0.95, 0.05};
  //Double_t blue[NRGBs]  = { 0.05, 0.05, 0.05, 0.05, 0.95};
  
  Double_t stops[NRGBs] = { theBottom, theLower, (theLower+theLevel0)/2, theLevel0, (theLevel0+theUpper)/2, theUpper, 1.0};
  Double_t red[NRGBs]   = { 0.99,      0.90,     0.80,                   0.02,      0.05,                   0.05,    0.02};
  Double_t green[NRGBs] = { 0.02,      0.85,     0.90,                   0.99,      0.80,                   0.85,    0.02};
  Double_t blue[NRGBs]  = { 0.02,      0.05,     0.05,                   0.02,      0.90,                   0.90,    0.99};
  


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
  thisCanvas->Print(me_outputFolder + fname.Data());
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

//////////////////////////////////////////
int ME_getLatestTier0ProcessingID(TString searchPath, TString fileName)
{
  // the different tier 0 processings are identified with the suffix c0 (fisrt), c1 (second), etc.
  bool debug = false;
  if (debug) std::cout << " ** ME_getLatestTier0ProcessingID ** START ** " << std::endl;

  int t0process = 0;
  TString tempFileName;
  tempFileName.Clear();
  tempFileName.Append(fileName.Data());
  tempFileName += t0process; 
  
  if (debug) {
    std::cout << "  --> searchPath: " << searchPath.Data() << std::endl;
    std::cout << "  --> tempFileName: " << tempFileName.Data() << std::endl;
  }

  bool testNextFolder = true;
  Long_t id;
  Long_t size;
  Long_t flags;
  Long_t modtime;
  
  while (testNextFolder){
    //testNextFolder = gSystem->Which(searchPath.Data(), tempFileName.Data());
    if (debug)  std::cout << " ** ME_getLatestTier0ProcessingID ** going for gSystem->GetPathInfo(...)" << std::endl;
    if ( gSystem->GetPathInfo(tempFileName.Data(),&id, &size, &flags, &modtime) ) {
      // file does not exist
      testNextFolder = false;
      if (debug) std::cout << "   --> " << tempFileName.Data() << " DOES NOT EXIST " << std::endl;
    }
    else {
      // file exists
      testNextFolder = true;
      if (debug) std::cout << "   --> " << tempFileName.Data() << " DOES EXIST " << std::endl;
      t0process ++;
      //std::cout << " process: " << t0process << std::endl;
      tempFileName.Clear();
      tempFileName.Append(fileName.Data());
      tempFileName += t0process; 
    }
  }
  t0process--; // subtract 1 unit

  if (debug) std::cout << std::endl << " ** ME_getLatestTier0ProcessingID ** return Tier0 process: " << t0process << std::endl << std::endl;
  return t0process;
}

//////////////////////////////////////////
bool ME_runIsGood(int runnumber)
{
  std::vector<int>   badRunList; // this is a private list of runs with some problems. Run is good unless it is in the list
  badRunList.push_back(281130);
  badRunList.push_back(298591);
  badRunList.push_back(301918);

  bool isthisrungood = true;

  // if (runnumber != 302872) isthisrungood = false;
  int irun=0;
  while (irun < (int) badRunList.size() && isthisrungood) {
    if (runnumber == badRunList.at(irun)) isthisrungood = false;
    irun++;
  }
  // runs before september 2015, IBL was stable. Run 278727 first valid run in September 2015
  if (runnumber < 278727) isthisrungood = false;

  //if (runnumber < 307000) isthisrungood = false; // TEST
  return isthisrungood;
}

//////////////////////////////////////////
TString ME_iter3FolderName (TString dataFolder, TString dataRun, TString dataProject, TString dataStream, int t0passID)
{
  TString iter3Folder = dataFolder;
  iter3Folder.Append(dataRun.Data());
  iter3Folder.Append("/");
  iter3Folder.Append(dataProject.Data());
  iter3Folder.Append(".");
  iter3Folder.Append(dataRun.Data());
  iter3Folder.Append(".");
  iter3Folder.Append(dataStream.Data());
  iter3Folder.Append(".idalignmerge.ROOT_MON.Iter3.c");  // some runs have .c1 
  iter3Folder += t0passID;
  iter3Folder.Append("/");

  // check the folder exists
  Long_t id;
  Long_t size;
  Long_t flags;
  Long_t modtime;
  if ( gSystem->GetPathInfo(iter3Folder.Data(), &id, &size, &flags, &modtime) ) {
    // file does not exist
    iter3Folder.Clear();
    //std::cout << " -- iter3 folder -- WARNING -- does not exist for run " << dataRun.Data() << std::endl;
  }
  //else{
  //  std::cout << " -- iter3 folder -- EXISTS " << std::endl;    
  //}

  return iter3Folder;
}

//////////////////////////////////////////
void ME_getIter3MonitoringFiles (TString iter3Folder, int runNumber)
{
  //std::cout << " -- going to check Iter3 -- for run: " << runNumber << std::endl;
  //std::cout << " -- " << iter3Folder.Data() << std::endl; 

  TString oslsCommand, oslsOutput, oslsFile, fullFileName;
  oslsCommand.Clear();
  oslsCommand.Append("ls ");
  oslsCommand.Append(iter3Folder.Data());
  oslsOutput = gSystem->GetFromPipe(oslsCommand.Data());
  Ssiz_t fromV = 0;
  // loop on the output --> extract run numbers
  while (oslsOutput.Tokenize(oslsFile, fromV,"\n")){
    // std::cout << " run: string " << oslsFile.Data() << std::endl;
    fullFileName = iter3Folder;
    fullFileName.Append(oslsFile.Data());
    me_iter3MonitoringFileList.push_back(fullFileName.Data());
    me_iter3runNumber.push_back(runNumber);
  }
  
  return;
}

//////////////////////////////////////////
TString ME_getIter0MonitoringFilesChain (TString iter0Folder, int runNumber)
{
  int nfileschained = 0;
  bool debug = false;
  TString theChainName;
  TString oshaddCommand = "";

  if (debug) std::cout << " -- ME_getIter0MonitoringFilesChain -- going to check Iter0 monitoring files (dynamic alignment/ LB group split) for run: " << runNumber << std::endl;

  TString oslsCommand, oslsOutput, oslsFile, fullFileName;
  oslsCommand.Clear();
  oslsCommand.Append("ls ");
  oslsCommand.Append(iter0Folder.Data());
  oslsOutput = gSystem->GetFromPipe(oslsCommand.Data());
  Ssiz_t fromV = 0;
  // loop on the output --> extract run numbers
  while (oslsOutput.Tokenize(oslsFile, fromV,"\n")){
    // std::cout << " run: string " << oslsFile.Data() << std::endl;
      fullFileName = iter0Folder;
      fullFileName.Append(oslsFile.Data());
      if (debug) std::cout << " -- ME_getIter0MonitoringFilesChain -- " << oslsFile.Data() << std::endl;
      if (nfileschained == 0) {
	theChainName.Clear();
	theChainName.Append(me_beforeAlignmentHistosFolder.Data());
	theChainName.Append("run_");
	theChainName += runNumber;
	theChainName.Append("_BeforeAlignment_AllLB.root");
	
	oshaddCommand.Append("hadd "); 
	oshaddCommand.Append(theChainName.Data());
      }
      // add files to the hadd command
      nfileschained++;
      oshaddCommand.Append(" "); 
      oshaddCommand.Append(fullFileName.Data());
  }


  // check if the chained file already exists
  TString stringRunNumber;
  stringRunNumber += runNumber;

  oslsCommand.Clear();
  oslsCommand.Append("ls ");
  oslsCommand.Append(me_beforeAlignmentHistosFolder.Data());
  oslsOutput = gSystem->GetFromPipe(oslsCommand.Data());
  if (debug) std::cout << " -- ME_getIter0MonitoringFilesChain -- run " << runNumber << std::endl 
		       << " oslsCommand:   " << oslsCommand.Data() << std::endl
		       << " output:   " << oslsOutput.Data() << std::endl;

  if (oslsOutput.Index(stringRunNumber.Data()) < 0 ||  REGENERATEAFTERALIGMENTMONITORINGFILE) {
    gSystem->Exec(oshaddCommand.Data());
    if (debug) std::cout << " -- ME_getIter0MonitoringFilesChain -- run " << runNumber << " has " << nfileschained << " files " << std::endl;  
  }
  else {
    if (debug) std::cout << " -- ME_getIter0MonitoringFilesChain -- no need to regenerate: "<< theChainName.Data() << std::endl;
  }


  return theChainName;
}


//////////////////////////////////////////
void ME_performIter3Analysis(int fileid)
{
  bool debug = false;
  TString hname, htitle, cname;
  TString h_path;
  
  if (debug) {
    std::cout << std::endl;
    std::cout << " ** ME_performIter3Analysis ** Analysing file id: " << fileid << "    run number --> " << me_runNumber.at(fileid) << std::endl; 
  }

  // first time prepare histos
  if (me_h_bowingMagnitudeEvolution_AfterAlignment[0]==NULL) ME_iter3Analysis_PrepareHistos();

  // Chain the AfterAlignment (Iter3) monitoring files belonging to this run
  TString monitoringFileAfterAlignment = ME_chainAfterAlignmentMonitoringFiles(me_runNumber.at(fileid));
  if (debug) {
    std::cout << " ** ME_performIter3Analysis ** monitoring file after alignment " <<  monitoringFileAfterAlignment.Data() << std::endl;
  } 

  // check if the current monitoring file is open. If so, close.
  //if (me_currentMonitoringFile->IsOpen()) me_currentMonitoringFile->Close();

  // open the total monitoring file after the alignment
  me_currentMonitoringFile = TFile::Open(monitoringFileAfterAlignment.Data());

  /*
  // retrieve the IBL 3D residuals histo: residuals (z) vs module phi (y) and module eta (x)
  TString h_path = "run_"; 
  h_path += me_runNumber.at(fileid); // add run number

  h_path.Clear(); // --> for monitoring files without run number in the path 

  //h_path.Append("/IDAlignMon/InDetTrackParticles_AlignSel/Residuals/pix_b0_xresvsmodetaphi_3d");
  //h_path.Append("/IDAlignMon/AlignTracks_all/Residuals/pix_b0_xresvsmodetaphi_3d");
  */
  h_path.Append("/IDAlignMon/");
  h_path += me_trackCollection;
  h_path.Append("/Residuals/pix_b0_xresvsmodetaphi_3d");
  if (debug) std::cout << " ** going to fetch TH3 histo: " << h_path.Data() << std::endl;
  //TH3F* h_xresiduals3d = (TH3F*) me_currentMonitoringFile->Get(h_path.Data());

  TH3F* h_xresiduals3d = ME_getIBLResiduals3DHisto (fileid);
  if (h_xresiduals3d != NULL) {
    if (debug) std::cout << "  ** ME_performIter3Analysis ** th3->GetEntries() " << h_xresiduals3d->GetEntries() << std::endl;

    // reset the bowing results
    //me_bowingMagnitudeValue.clear();
    //me_bowingMagnitudeError.clear();

    if (debug) std::cout << " ** ME_performIter3Analysis ** going to fit bowing magnitudes ... size of me_bowingMagnitudeValue:" << me_bowingMagnitudeValue.size() << std::endl;
    for (int stave = 0; stave <= NIBLSTAVES; stave++) {
      me_h_IBLmeanResPerRing[stave] = ME_getIBLMeanResPerRing(h_xresiduals3d, stave, fileid); 
      ME_fitBowingMagnitude(me_h_IBLmeanResPerRing[stave], stave, fileid, -1, "_AfterAlignment");
      if (FULLOUTPUT) {
	cname.Clear();
	cname.Append("IBLXmeanResVsRing_stave_");
	if (stave == 0) {
	  cname.Append("all");
	}
	else {
	  cname += stave;
	}
	cname.Append("_AfterAlignment_");
	cname.Append("_Run_");
	cname += me_runNumber.at(fileid);
	TCanvas thisCanvas(cname.Data(), cname.Data());
	me_h_IBLmeanResPerRing[stave]->Draw();
	thisCanvas.Print(me_outputFolder + cname + ".png");
      }
    }
    if (debug) std::cout << " ** ME_performIter3Analysis ** fit of bowing magnitudes completed!   size of me_bowingMagnitudeValue:" << me_bowingMagnitudeValue.size()  << std::endl;

    // now loop on fit results and fill evolution histograms 
    for (int i=0; i < (int) me_bowingMagnitudeStave.size(); i++) {
      int stave = me_bowingMagnitudeStave.at(i); // this is because a stave may not be fitted due to stats or whatever
      me_h_bowingMagnitudeEvolution_AfterAlignment[stave]->SetBinContent(fileid+1, me_bowingMagnitudeValue.at(i));

      float total_uncertainty = sqrt(pow(me_bowingMagnitudeError.at(i),2) + pow(me_systUncertaintyBowing,2.));    
      me_h_bowingMagnitudeEvolution_AfterAlignment[stave]->SetBinError(fileid+1, total_uncertainty);
      
      // // std::cout << " ** Filling evoution histograms: stave " << stave << "  entries = " << me_h_bowingMagnitudeEvolution[stave]->GetEntries() << std::endl; 
      //me_h_bowingBaseLineEvolution[stave]->SetBinContent(fileid+1, me_bowingBaseLineValue.at(i));
      //me_h_bowingBaseLineEvolution[stave]->SetBinError(fileid+1, me_bowingBaseLineError.at(i));
    
      // // fill the bowing magnitude and base line histo per stave
      //if (me_h_bowingMagnitudePerStave.size()>0) {
      // std::cout << " me_h_bowingMagnitudePerStave.at(" << fileid << ")->SetBinContent(" << stave <<",  me_bowingMagnitudeValue.at(" << i << "));" << std::endl;
      // me_h_bowingMagnitudePerStave.at(fileid)->SetBinContent(stave, me_bowingMagnitudeValue.at(i));
      // me_h_bowingMagnitudePerStave.at(fileid)->SetBinError(stave, total_uncertainty);  
	
      // me_h_bowingBaseLinePerStave.at(fileid)->SetBinContent(stave,  me_bowingBaseLineValue.at(i));
      // me_h_bowingBaseLinePerStave.at(fileid)->SetBinError(stave,  me_bowingBaseLineError.at(i));
      //}
    }
    
  }

  return;
}

//////////////////////////////////////////
void ME_iter3Analysis_PrepareHistos ()
{
  bool debug = false;
  
  if (debug) std::cout << " ** ME_iter3Analysis_PrepareHistos ** START " << std::endl;

  TString hname, htitle;
  for (int istave=0; istave <= NIBLSTAVES; istave++) {
    hname = me_h_bowingMagnitudeEvolution[istave]->GetName();
    hname.Append("_AfterAlignment");
    htitle = me_h_bowingMagnitudeEvolution[istave]->GetTitle();
    htitle.Append(" (after alignment)");

    me_h_bowingMagnitudeEvolution_AfterAlignment[istave] = new TH1F(hname.Data(), htitle.Data(), me_h_bowingMagnitudeEvolution[istave]->GetNbinsX(),
									  me_h_bowingMagnitudeEvolution[istave]->GetXaxis()->GetXmin(),
									  me_h_bowingMagnitudeEvolution[istave]->GetXaxis()->GetXmax());

    me_h_bowingMagnitudeEvolution_AfterAlignment[istave]->SetXTitle(me_h_bowingMagnitudeEvolution[istave]->GetXaxis()->GetTitle());
    me_h_bowingMagnitudeEvolution_AfterAlignment[istave]->SetYTitle(me_h_bowingMagnitudeEvolution[istave]->GetYaxis()->GetTitle());
    me_h_bowingMagnitudeEvolution_AfterAlignment[istave]->SetMarkerStyle(kOpenCircle);
    me_h_bowingMagnitudeEvolution_AfterAlignment[istave]->SetMarkerColor(kRed+1);
    me_h_bowingMagnitudeEvolution_AfterAlignment[istave]->SetLineColor(kRed+1);
    me_h_bowingMagnitudeEvolution_AfterAlignment[istave]->SetStats(kFALSE);
    
    for (int ibin = 1; ibin <= me_h_bowingMagnitudeEvolution_AfterAlignment[istave]->GetNbinsX(); ibin++) {
      me_h_bowingMagnitudeEvolution_AfterAlignment[istave]->GetXaxis()->SetBinLabel(ibin, me_h_bowingMagnitudeEvolution[istave]->GetXaxis()->GetBinLabel(ibin));
    }

    if (debug){
      std::cout << " ** ME_iter3Analysis_PrepareHistos ** histo:  me_h_bowingMagnitudeEvolution_AfterAlignment["<< istave << "] created with name: " 
		<< me_h_bowingMagnitudeEvolution_AfterAlignment[istave]->GetName()
		<< " addres: " << me_h_bowingMagnitudeEvolution_AfterAlignment[istave]
		<< std::endl;
    }
  }
  if (debug) std::cout << " ** ME_iter3Analysis_PrepareHistos ** COMPLETED " << std::endl;
  return;
}


//////////////////////////////////////////
TString ME_chainAfterAlignmentMonitoringFiles(int targetrun)
{
  // returning the number of chained files
  int nfileschained = 0;
  bool debug = false;

  TString stringRunNumber = "";
  stringRunNumber += targetrun;
  int theIndex;
  TString theChainName;
  TString oshaddCommand = "";

  
  for (int ifile=0; ifile < (int) me_iter3MonitoringFileList.size(); ifile++) {
    if (me_iter3runNumber.at(ifile) == targetrun) {
      // verify that the run number is in the filename
      theIndex = me_iter3MonitoringFileList.at(ifile).Index(stringRunNumber.Data());
      if (debug) std::cout << " ** ME_chainAfterAlignmentMonitoringFiles ** Index of " << stringRunNumber << " in iter3MonitoringFilename("<<ifile << ")= " << theIndex << std::endl;  
      if (theIndex>=0) { // run number is in the file name
	if (nfileschained == 0) {
	  theChainName.Clear();
	  theChainName.Append(me_afterAlignmentHistosFolder.Data());
	  theChainName.Append("run_");
	  theChainName += targetrun;
	  theChainName.Append("_AfterAlignment_AllLB.root");

	  oshaddCommand.Append("hadd "); 
	  oshaddCommand.Append(theChainName.Data());
	}
	// add files to the hadd command
	nfileschained++;
	oshaddCommand.Append(" "); 
	oshaddCommand.Append(me_iter3MonitoringFileList.at(ifile).Data());
      } 
    }
  }
  
  // check if file already exists
  TString oslsCommand, oslsOutput;
  oslsCommand.Clear();
  oslsCommand.Append("ls ");
  oslsCommand.Append(theChainName.Data());
  oslsOutput = gSystem->GetFromPipe(oslsCommand.Data());

  if (oslsOutput.Index(stringRunNumber.Data()) < 0 || REGENERATEAFTERALIGMENTMONITORINGFILE) {
    gSystem->Exec(oshaddCommand.Data());
    if (debug) std::cout << " ** ME_chainAfterAlignmentMonitoringFiles ** run " << stringRunNumber << " has " << nfileschained << " files " << std::endl;  
  }
  else {
    if (debug) std::cout << " ** ME_chainAfterAlignmentMonitoringFiles ** no need to regenerate: "<< theChainName.Data() << std::endl;
  }

  return theChainName;
}

////////////////////////////////////////
void ME_drawEvolInTimeFormat()
{
  // draw the bowing evolution with a time line axis

  bool debug = false;
  if (debug) std::cout << " ** ME_drawEvolInTimeFormat ** START " << std::endl;
  //SetAtlasStyle();

  // check the size of the dates vector
  if (me_runDateTime.size() == 0) {
    if (debug) std::cout << " ** ME_drawEvolInTimeFormat ** size of dates vector == 0 " << std::endl;
    return;
  }

  bool manyYearsUsed = false;
  if (me_listOfDataProjects.size() > 1) manyYearsUsed = true;
  
  TGraphErrors *graphBowMagEvol_time = new TGraphErrors;
  TGraphErrors *graphBowMagEvolAfterAlignment_time = new TGraphErrors;
  TDatime theDate(2015,01,01,0,0,0);
  
  graphBowMagEvol_time->SetMarkerStyle( me_h_bowingMagnitudeEvolution[0]->GetMarkerStyle());
  graphBowMagEvol_time->SetMarkerColor( me_h_bowingMagnitudeEvolution[0]->GetMarkerColor());
  graphBowMagEvol_time->SetLineColor( me_h_bowingMagnitudeEvolution[0]->GetMarkerColor());

  if (ITER3ANALYSIS && me_h_bowingMagnitudeEvolution_AfterAlignment[0] != NULL) {
    graphBowMagEvolAfterAlignment_time->SetMarkerStyle( me_h_bowingMagnitudeEvolution_AfterAlignment[0]->GetMarkerStyle());
    graphBowMagEvolAfterAlignment_time->SetMarkerColor( me_h_bowingMagnitudeEvolution_AfterAlignment[0]->GetMarkerColor());
    graphBowMagEvolAfterAlignment_time->SetLineColor( me_h_bowingMagnitudeEvolution_AfterAlignment[0]->GetMarkerColor());
  }

  if (debug){
    std::cout << " ** ME_drawEvolInTimeFormat ** TGrapErrors already defined " << std::endl;
    std::cout << " ** ME_drawEvolInTimeFormat ** me_runDateTime.size() = " << me_runDateTime.size() << std::endl;
  }

  //

  if (me_runDateTime.size()>0) {
    for (int idate=0; idate < (int) me_runDateTime.size(); idate++) {
      // fill points with initial bowing 
      theDate = me_runDateTime.at(idate);
      graphBowMagEvol_time->SetPoint(idate, theDate.Convert(), me_h_bowingMagnitudeEvolution[0]->GetBinContent(idate+1)); 
      graphBowMagEvol_time->SetPointError(idate, 0., me_h_bowingMagnitudeEvolution[0]->GetBinError(idate+1) );

      if (debug && false) std::cout << " ** ME_drawEvolInTimeFormat ** point: " << idate 
				    << "  run " << me_runNumber.at(idate)
				    << "  date-time: " << theDate.GetDate() 
				    << std::endl;

      // fill points with bowing computed after alignment
      if (ITER3ANALYSIS && me_h_bowingMagnitudeEvolution_AfterAlignment[0] != NULL ) {
	graphBowMagEvolAfterAlignment_time->SetPoint(idate, theDate.Convert(), me_h_bowingMagnitudeEvolution_AfterAlignment[0]->GetBinContent(idate+1)); 
	graphBowMagEvolAfterAlignment_time->SetPointError(idate, 0., me_h_bowingMagnitudeEvolution_AfterAlignment[0]->GetBinError(idate+1) );
	// 
		// special treatment for 2015 runs that were realigned out of the calibration loop
	if (ME_runIsInSpecialTreatment2015RunList(me_runNumber.at(idate))) {
	  graphBowMagEvolAfterAlignment_time->SetPoint(idate, theDate.Convert(), ME_getBowingMagFor2015SpecialRuns(me_runNumber.at(idate)));
	}
      }
    }
  }

  TCanvas *cTimeAxisCan = new TCanvas("cTimeAxisCan","time evolution", 0., 0., 900, 600);
  cTimeAxisCan->cd();
  gPad->SetTicks(1,1);

  TMultiGraph *theGraph = new TMultiGraph();
  theGraph->Add(graphBowMagEvol_time,"p");
  if (ITER3ANALYSIS) theGraph->Add(graphBowMagEvolAfterAlignment_time,"p");
  theGraph->Draw("a");

  TH1F *hist = theGraph->GetHistogram();
  TAxis *xaxis = hist->GetXaxis();
  xaxis->SetTimeDisplay(1);
  

  xaxis->SetTimeFormat("%d/%m %F1970-01-01-00:00:00");
  if (manyYearsUsed) xaxis->SetTimeFormat("%d/%m/%Y %F1970-01-01-00:00:00");

  //xaxis->SetNdivisions(204,false);
  xaxis->SetNdivisions(404,false); // was 408
  xaxis->SetTitle("Day in 2016");
  if (manyYearsUsed) xaxis->SetTitle("Day");

  // hist->GetYaxis()->SetTitle(me_h_bowingMagnitudeEvolution[0]->GetYaxis()->GetTitle());
  hist->GetYaxis()->SetTitle("Average IBL distortion magnitude [#mum]");
  hist->SetMaximum(me_h_bowingMagnitudeEvolution[0]->GetMaximum());
  hist->SetMinimum(me_h_bowingMagnitudeEvolution[0]->GetMinimum());
  hist->SetTitle(me_h_bowingMagnitudeEvolution[0]->GetTitle());
  hist->GetXaxis()->SetTitleOffset(1.3);

  // Line at 0
  TLine aLine;
  aLine.SetLineColor(kGray+2);
  aLine.SetLineStyle(2);
  aLine.DrawLine( hist->GetXaxis()->GetXmin(),0.,
		  hist->GetXaxis()->GetXmax(),0.);

  double xLegend = 0.59;
  double yLegend = 0.85;

  //ATLASLabel(0.12, 0.82, "ID Alignment calibration loop", kBlack);
  myTextMatthias(xLegend, yLegend, 0.035, kBlack, "ID Alignment calibration loop");
  if (manyYearsUsed) {
    myTextMatthias(xLegend, yLegend-0.05, 0.035, kBlack, "Data 2015 / 2016");
  }
  else {
    myTextMatthias(xLegend, yLegend-0.05, 0.035, kBlack, "Data 2016");
  }
  myMarkerText(xLegend+0.03, yLegend-0.09, graphBowMagEvol_time->GetMarkerColor(), graphBowMagEvol_time->GetMarkerStyle(), "Default alignment", graphBowMagEvol_time->GetMarkerSize());
  if (ITER3ANALYSIS && me_h_bowingMagnitudeEvolution_AfterAlignment[0] != NULL) {
    myMarkerText(xLegend+0.03, yLegend-0.13, graphBowMagEvolAfterAlignment_time->GetMarkerColor(), graphBowMagEvolAfterAlignment_time->GetMarkerStyle(), "After CalibLoop alignment" ,graphBowMagEvolAfterAlignment_time->GetMarkerSize());
  }
  
  TString lastDataText = "Latest update with run: ";
  lastDataText += me_runNumber.back();
  lastDataText.Append(" at ");
  lastDataText += me_runDateTime.back().GetDay();
  lastDataText.Append("/");
  lastDataText += me_runDateTime.back().GetMonth();
  lastDataText.Append("/");
  lastDataText += me_runDateTime.back().GetYear();
  myTextMatthias(0.59, 0.91, 0.025, kBlack, lastDataText.Data());

  // PF
  // PF 
  // PF

  // Save the plot 
  cTimeAxisCan->Print(me_outputFolder + "BowingMagnitude_TimeEvolution.png");

  // Save the Multigraph
  TFile file_TGraph("tgraph_bow_15_16.root","RECREATE");
  theGraph->Write();
  file_TGraph.Close();

  if (debug) std::cout << " ** ME_drawEvolInTimeFormat ** COMPLETED " << std::endl;
  return;
}

////////////////////////////////////////
TDatime ME_getRunDateTime (TString iter0Folder){
  bool debug = false;

  TDatime theDate(2015,01,01,12,00,00);

  if (debug) std::cout << " ** ME_getRunDateTime ** finding date and time for run: " << iter0Folder.Data() << std::endl;
  TString oslsCommand = "stat "; oslsCommand.Append(iter0Folder.Data()); oslsCommand.Append(" | grep Modify | awk '{printf $2 \" \" $3}'");
  TString oslsOutput = gSystem->GetFromPipe(oslsCommand.Data());
  if (debug) std::cout << " ** ME_getRunDateTime ** " << oslsOutput.Data() << std::endl;
  theDate.Set(oslsOutput.Data());
  if (debug) std::cout << " ** ME_getRunDateTime ** ouput:" << oslsOutput.Data() 
		       << "    Date  yyyy-mm-dd: " << theDate.GetYear() << "-" << theDate.GetMonth() << "-" << theDate.GetDay() << std::endl; 
  
  // special treatment for run 284484 (2015) as it was reprocessed (c2) in 2016. use the original 2015 date
  if (iter0Folder.Index("284484") > 0) {
    theDate.Set(2015,11,3, 12,00,00);
  }


  return theDate;
}

////////////////////////////////////////////
bool ME_runIsInSpecialTreatment2015RunList (int theRun)
{
  bool thisRunIsSpecial = false;

  if (theRun == 280500) thisRunIsSpecial = true;
  if (theRun == 280520) thisRunIsSpecial = true;
  if (theRun == 280614) thisRunIsSpecial = true;
  if (theRun == 280673) thisRunIsSpecial = true;

  if (theRun == 280753) thisRunIsSpecial = true;
  if (theRun == 280853) thisRunIsSpecial = true;
  if (theRun == 280862) thisRunIsSpecial = true;
  if (theRun == 280950) thisRunIsSpecial = true;

  if (theRun == 280977) thisRunIsSpecial = true;
  if (theRun == 281070) thisRunIsSpecial = true;
  if (theRun == 281074) thisRunIsSpecial = true;
  if (theRun == 281075) thisRunIsSpecial = true;

  if (theRun == 281130) thisRunIsSpecial = true;
  if (theRun == 281317) thisRunIsSpecial = true;
  if (theRun == 281381) thisRunIsSpecial = true;
  if (theRun == 281385) thisRunIsSpecial = true;

  if (theRun == 281411) thisRunIsSpecial = true;
  if (theRun == 282625) thisRunIsSpecial = true;
  if (theRun == 282631) thisRunIsSpecial = true;
  if (theRun == 282712) thisRunIsSpecial = true;

  if (theRun == 282784) thisRunIsSpecial = true;
  if (theRun == 282992) thisRunIsSpecial = true;
  if (theRun == 283074) thisRunIsSpecial = true;
  if (theRun == 283155) thisRunIsSpecial = true;

  if (theRun == 283270) thisRunIsSpecial = true;
  if (theRun == 283429) thisRunIsSpecial = true;
  if (theRun == 283608) thisRunIsSpecial = true;
  if (theRun == 283780) thisRunIsSpecial = true;

  if (theRun == 284006) thisRunIsSpecial = true;
  if (theRun == 284154) thisRunIsSpecial = true;
  if (theRun == 284213) thisRunIsSpecial = true;
  if (theRun == 284285) thisRunIsSpecial = true;

  if (theRun == 284420) thisRunIsSpecial = true;
  if (theRun == 284427) thisRunIsSpecial = true;
  if (theRun == 284473) thisRunIsSpecial = true;
  if (theRun == 284484) thisRunIsSpecial = true;

  return thisRunIsSpecial;
}

////////////////////////////////////////////
float ME_getBowingMagFor2015SpecialRuns (int theRun)
{
  float theBowing = 25.;

  if (theRun == 280500) theBowing = 1.803407;
  if (theRun == 280520) theBowing = 1.306839;
  if (theRun == 280614) theBowing = 0.07100698;
  if (theRun == 280673) theBowing = -0.2115438;

  if (theRun == 280753) theBowing = 0.836698;
  if (theRun == 280853) theBowing = -0.182584;
  if (theRun == 280862) theBowing = 0.3558431;
  if (theRun == 280950) theBowing = 1.392249;

  if (theRun == 280977) theBowing = 0.2004839;
  if (theRun == 281070) theBowing = 2.141745;
  if (theRun == 281074) theBowing = 2.141745;
  if (theRun == 281075) theBowing = 2.141745;

  if (theRun == 281130) theBowing = 20.;
  if (theRun == 281317) theBowing = 0.6531002;
  if (theRun == 281381) theBowing = 1.13957;
  if (theRun == 281385) theBowing = 1.261269;

  if (theRun == 281411) theBowing = 1.603248;
  if (theRun == 282625) theBowing = 1.718212;
  if (theRun == 282631) theBowing = 2.02957;
  if (theRun == 282712) theBowing = 2.820183;

  if (theRun == 282784) theBowing = 0.2423804;
  if (theRun == 282992) theBowing = 2.116418;
  if (theRun == 283074) theBowing = 2.073665;
  if (theRun == 283155) theBowing = 1.970335;

  if (theRun == 283270) theBowing = 2.560627;
  if (theRun == 283429) theBowing = 2.358727;
  if (theRun == 283608) theBowing = 2.634164;
  if (theRun == 283780) theBowing = 2.276105;

  if (theRun == 284006) theBowing = 2.131346;
  if (theRun == 284154) theBowing = 2.073411;
  if (theRun == 284213) theBowing = 2.441031;
  if (theRun == 284285) theBowing = 3.6162;

  if (theRun == 284420) theBowing = 2.446549;
  if (theRun == 284427) theBowing = 2.629768;
  if (theRun == 284473) theBowing = 1.866875;
  if (theRun == 284484) theBowing = 2.685501;

  return theBowing;
}

////////////////////////////////////////////
void ME_prepareOutputFolder()
{
  bool debug = false;
  me_outputFolder.Clear();
  me_outputFolder.Append("./");

  me_HOSTNAME = gSystem->GetFromPipe("echo $HOSTNAME");
  if (debug) std::cout << " ** ME_prepareOutputFolder ** HOSTNAME = " << me_HOSTNAME.Data() << std::endl;
  
  // check if hostname is aiatla012
  if (me_HOSTNAME.Index("aiatlas012") > 0) {
    me_outputFolder.Clear();
    me_outputFolder.Append("/var/vhost/atlas-alignment/secure/detailed_plots");    
  }
  
  // check if this is a local test of the web monitoring
  TString commandOutput = gSystem->GetFromPipe("echo $RUNLOCALTEST");
  if (debug) std::cout << " ** ME_prepareOutputFolder ** RUNLOCALTEST = " << commandOutput.Data() << std::endl;
  if (commandOutput.Index("True") >= 0) {
    // this is a local test of the web monitoring
    // -> fetch the ALIGNWEBROOT
    commandOutput = gSystem->GetFromPipe("echo $ALIGNWEBROOT");
    if (debug) std::cout << " ** ME_prepareOutputFolder ** ALIGNWEBROOT = " << commandOutput.Data() << "  length: " << commandOutput.Length() << std::endl;
    if (commandOutput.Length() > 0) { 
      me_outputFolder.Clear();
      me_outputFolder.Append(commandOutput.Data());
      std::cout << " ** ME_prepareOutputFolder ** commandOutput.Last(/) = " <<  me_outputFolder.Last('/') << "  length: " << me_outputFolder.Length() << std::endl;
      if ((me_outputFolder.Last('/')+1) != me_outputFolder.Length())  me_outputFolder.Append("/"); // check last character to be '/'
      me_outputFolder.Append("WebPage/detailed_plots");
    }
  }
  
  // if "detailed_plots" is in the outputFolder --> this is being run under the web monitoring --> add year
  if (me_outputFolder.Index("detailed_plots") >= 0) {
    commandOutput = gSystem->GetFromPipe("echo $ALIGNWEBYEAR");
    if (debug) std::cout << " ** ME_prepareOutputFolder ** ALIGNWEBYEAR = " << commandOutput.Data() << "  length: " << commandOutput.Length() << std::endl;
    if (commandOutput.Length() > 0) {
      me_outputFolder.Append("/");
      me_outputFolder.Append(commandOutput.Data());
    }
    else {
      // default year: 2016
      me_outputFolder.Append("/2016");
    } 

    // add path to IBL bowing folder
    me_outputFolder.Append("/IBLbowing/");
  }

  // check if output folder exists
  TString execCommand = "stat "; 
  execCommand.Append(me_outputFolder.Data()); 
  execCommand.Append(" | grep Modify ");
  
  commandOutput = gSystem->GetFromPipe(execCommand.Data());
  if (debug) std::cout << " ** ME_prepareOutputFolder ** gSystem->GetFromPipe(execCommand.Data()) = " << commandOutput.Data() << endl;
  if (commandOutput.Index("Modify") >= 0) {
    if (debug) std::cout << " ** ME_prepareOutputFolder ** output folder EXISTS " << std::endl;
  }
  else {
    if (debug) std::cout << " ** ME_prepareOutputFolder ** output folder DOES NOT EXIST " << std::endl;
    // create the folder
    execCommand.Clear();
    execCommand.Append("mkdir ");
    execCommand.Append(me_outputFolder.Data());
    commandOutput = gSystem->GetFromPipe(execCommand.Data());
    if (debug) std::cout << " ** ME_prepareOutputFolder ** result from mkdir " << commandOutput.Data() << std::endl;
  }


  if (debug) std::cout << " ** ME_prepareOutputFolder ** me_outputFolder = " << me_outputFolder.Data() << std::endl;
  
  return;
}

////////////////////////////////////////////
void  ME_prepareAfterAlignmentHistosFolder()
{
  bool debug = false;
  me_afterAlignmentHistosFolder.Clear();
  me_afterAlignmentHistosFolder.Append("./MagnitudeEvolution_AfterAlignmentHistos/");

  // check if output folder exists
  TString execCommand = "stat "; 
  execCommand.Append(me_afterAlignmentHistosFolder.Data()); 
  execCommand.Append(" | grep Modify ");
  
  TString commandOutput = gSystem->GetFromPipe(execCommand.Data());
  if (debug) std::cout << " ** ME_prepareAfterAlignmentHistosFolder ** gSystem->GetFromPipe(execCommand.Data()) = " << commandOutput.Data() << endl;
  if (commandOutput.Index("Modify") >= 0) {
    if (debug) std::cout << " ** ME_prepareAfterAlignmentHistosFolder ** output folder EXISTS " << std::endl;
  }
  else {
    if (debug) std::cout << " ** ME_prepareAfterAlignmentHistosFolder ** output folder DOES NOT EXIST " << std::endl;
    // create the folder
    execCommand.Clear();
    execCommand.Append("mkdir ");
    execCommand.Append(me_afterAlignmentHistosFolder.Data());
    commandOutput = gSystem->GetFromPipe(execCommand.Data());
    if (debug) std::cout << " ** ME_prepareAfterAlignmentHistosFolder ** result from mkdir " << commandOutput.Data() << std::endl;
  }

  if (debug)  std::cout << " ** ME_prepareAfterAlignmentHistosFolder ** me_afterAlignmentHistosFolder: " << me_afterAlignmentHistosFolder.Data() << std::endl;
  return;
}

////////////////////////////////////////////
void  ME_prepareBeforeAlignmentHistosFolder()
{
  bool debug = false;
  me_beforeAlignmentHistosFolder.Clear();
  me_beforeAlignmentHistosFolder.Append("./MagnitudeEvolution_BeforeAlignmentHistos/");

  // check if output folder exists
  TString execCommand = "stat "; 
  execCommand.Append(me_beforeAlignmentHistosFolder.Data()); 
  execCommand.Append(" | grep Modify ");
  
  TString commandOutput = gSystem->GetFromPipe(execCommand.Data());
  if (debug) std::cout << " ** ME_prepareBeforeAlignmentHistosFolder ** gSystem->GetFromPipe(execCommand.Data()) = " << commandOutput.Data() << endl;
  if (commandOutput.Index("Modify") >= 0) {
    if (debug) std::cout << " ** ME_prepareBeforeAlignmentHistosFolder ** output folder EXISTS " << std::endl;
  }
  else {
    if (debug) std::cout << " ** ME_prepareBeforeAlignmentHistosFolder ** output folder DOES NOT EXIST " << std::endl;
    // create the folder
    execCommand.Clear();
    execCommand.Append("mkdir ");
    execCommand.Append(me_beforeAlignmentHistosFolder.Data());
    commandOutput = gSystem->GetFromPipe(execCommand.Data());
    if (debug) std::cout << " ** ME_prepareBeforeAlignmentHistosFolder ** result from mkdir " << commandOutput.Data() << std::endl;
  }

  if (debug)  std::cout << " ** ME_prepareBeforeAlignmentHistosFolder ** me_beforeAlignmentHistosFolder: " << me_beforeAlignmentHistosFolder.Data() << std::endl;
  return;
}

