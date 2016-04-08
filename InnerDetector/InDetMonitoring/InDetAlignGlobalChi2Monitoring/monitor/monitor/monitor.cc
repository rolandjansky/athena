/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <include/monitor.h>
#include <include/utils.cc>
//=====================================================================
//
//  monitor
//
//=====================================================================
void monitor(Char_t extPath[MaxPathSize]="", Int_t extIteration=-1) {

  // Iteration Number (IterNumber=-1 means direct trkana loading)
  IterNumber = 0;

  // InputPath (if externalPath exist, the path will be ignored)
  sprintf(path,"/tmp/%s/",gSystem->GetUserInfo(gSystem->GetUid())->fUser.Data());
  sprintf(path,".");

  gROOT->SetBatch(kTRUE);
  AtlasStyle = false;
  normalizePlots = false;
  reprocessTrkana = true;
  processResults = true;
  showFits = true;
  PrintLevel = 3;
  level = 1; // default is 1
  // ----------------------------------------------------------------------------------------

  // run monitor
  checkExternals(extPath, extIteration);
  runMonitor();
  printPS();

  return;
}


//=====================================================================
// callMacros()
//=====================================================================
void callMacros() {

  // Summary and residuals
  gROOT->ProcessLine(".x macros/common/residuals.cc");
  
  // Hits and Tracks from AlignTrk
  gROOT->ProcessLine(".x macros/common/hits.cc()"); 

  gROOT->ProcessLine(".x macros/common/hitCorrelations.cc()");
  gROOT->ProcessLine(".x macros/common/hitsPerLayer.cc");
  gROOT->ProcessLine(".x macros/common/prochitsPerLayer.cc()");
  
  // Hitmaps
  gROOT->ProcessLine(".x macros/common/hitmaps.cc()");
  // gROOT->ProcessLine(".x macros/common/hitmapsXY.cc()");
  
  // Residuals
  gROOT->ProcessLine(".x macros/common/AllResiduals.cc");
  gROOT->ProcessLine(".x macros/common/AllProcessedResiduals.cc()");
  
  // Pixel Barrel Residuals
  gROOT->ProcessLine(".x macros/common/PIXBarrelRes.cc()");
  gROOT->ProcessLine(".x macros/common/ResVsRecoPtPIXBarrel.cc()");

  // PixelEndcap Residuals
  gROOT->ProcessLine(".x macros/common/PIXEndcapARes.cc()");
  gROOT->ProcessLine(".x macros/common/PIXEndcapCRes.cc()");
  gROOT->ProcessLine(".x macros/common/ResVsRecoPtPIXEndCapA.cc()");
  gROOT->ProcessLine(".x macros/common/ResVsRecoPtPIXEndCapC.cc()");

  // SCT Barrel Residuals
  gROOT->ProcessLine(".x macros/common/SCTBarrelRes.cc()");
  gROOT->ProcessLine(".x macros/common/ResVsRecoPtSCTBarrel.cc()");
  
  //SCT Endcap Residuals
  gROOT->ProcessLine(".x macros/common/SCTEndcapARes.cc()");
  gROOT->ProcessLine(".x macros/common/SCTEndcapCRes.cc()");
  gROOT->ProcessLine(".x macros/common/ResVsRecoPtSCTEndCapA.cc()");
  gROOT->ProcessLine(".x macros/common/ResVsRecoPtSCTEndCapC.cc()");

  // Track Matching
  gROOT->ProcessLine(".x macros/common/TrackMatching_Parameters.cc()");
  gROOT->ProcessLine(".x macros/common/TrackMatching_ParametersVsD0.cc()");
  gROOT->ProcessLine(".x macros/common/TrackMatching_ParametersVsZ.cc()");
  gROOT->ProcessLine(".x macros/common/TrackMatching_ParametersVsPhi0.cc()");
  gROOT->ProcessLine(".x macros/common/TrackMatching_ParametersVsEta.cc()");
  gROOT->ProcessLine(".x macros/common/TrackMatching_ParametersVsQoverPt.cc()");

  // Silicon Clusters for Barrels
  gROOT->ProcessLine(".x macros/common/SiCluster.cc()");
  
  // Residuals per Layer (means)
  gROOT->ProcessLine(".x macros/common/ResPerLayer.cc");
  gROOT->ProcessLine(".x macros/common/ProcResPerLayer.cc()");
  gROOT->ProcessLine(".x macros/common/resErrors.cc()");

  // Resmaps
  gROOT->ProcessLine(".x macros/common/resmaps.cc()");

  // AlignTrk stuff
  gROOT->ProcessLine(".x macros/common/trk_params.cc");
  // gROOT->ProcessLine(".x macros/common/truth_trk_params.cc");
  // gROOT->ProcessLine(".x macros/common/trkdifftruth.cc");
  gROOT->ProcessLine(".x macros/common/Used_trk_params.cc()");
  gROOT->ProcessLine(".x macros/common/CompTrkParams.cc()");
  
  // AlignTrk momentum
  gROOT->ProcessLine(".x macros/common/momentum.cc");
  gROOT->ProcessLine(".x macros/common/momentum2.cc()");
  gROOT->ProcessLine(".x macros/common/CompMomentum.cc()");
  
  // AlignTrk properties and correlations
  gROOT->ProcessLine(".x macros/common/TrkParCorrProfNoTruth.cc()");
  gROOT->ProcessLine(".x macros/common/d0Error.cc()");
  gROOT->ProcessLine(".x macros/common/z0Error.cc()");
  gROOT->ProcessLine(".x macros/common/TrkPtVsEta.cc()");
  gROOT->ProcessLine(".x macros/common/TrkParCorrProf2NoTruth.cc()");
  gROOT->ProcessLine(".x macros/common/D0bsCorrelations.cc()"); 

  // Beam spot plots
  gROOT->ProcessLine(".x macros/common/BeamSpotHistos.cc()"); 

  //Trk::Track parameters
  gROOT->ProcessLine(".x macros/common/ntracks.cc()");
  gROOT->ProcessLine(".x macros/common/trkTrack_hits.cc()");  
  gROOT->ProcessLine(".x macros/common/trkTrack_params.cc");
  gROOT->ProcessLine(".x macros/common/trkTrack_momentum.cc");
  gROOT->ProcessLine(".x macros/common/trkTrack_truth.cc");
  gROOT->ProcessLine(".x macros/common/trkTrack_recotruthdiff.cc");
  gROOT->ProcessLine(".x macros/common/trkTrack_genVtx.cc");
  gROOT->ProcessLine(".x macros/common/trkTrack_genParticle.cc");

  // Pixels properties
  gROOT->ProcessLine(".x macros/common/Pixel_trkProp.cc"); // delzr
  gROOT->ProcessLine(".x macros/common/Pixel_usedtrkProp.cc()"); // delzr
  
  // SCT properties
  gROOT->ProcessLine(".x macros/common/SCT_trkProp.cc"); // delzr
  gROOT->ProcessLine(".x macros/common/SCT_usedtrkProp.cc()"); // delzr
    
  // Chi2
  gROOT->ProcessLine(".x macros/common/chi2.cc");

  // Vertex Candidate
  gROOT->ProcessLine(".x macros/common/VtxCandidatePos.cc");
  
  // ----------------------------------------------------------------------------------------

  // Silicon Modules positions
  gROOT->ProcessLine(".x macros/extras/PixelBarrelIniLocalPos.cc");
  gROOT->ProcessLine(".x macros/extras/SCTBarrelIniLocalPos.cc");

  // ----------------------------------------------------------------------------------------

  // EigenModes directly from the bigmatrix and the bigvector
  gROOT->ProcessLine(".x macros/common/EigenModes.cc");

  // ----------------------------------------------------------------------------------------

  // Summary and Diagonalization information
  gROOT->ProcessLine(".x macros/solve/align_dia.cc");
   
  //gROOT->ProcessLine(".x macros/common/bigmatrix.cc");
  
  // Alignment Pixel Corrections (Errors) (Vs Hits)
  gROOT->ProcessLine(".x macros/solve/PixelConstants.cc");
  gROOT->ProcessLine(".x macros/solve/corrVsPixelHits.cc");
  gROOT->ProcessLine(".x macros/solve/PixelCorrErrors.cc");   
  gROOT->ProcessLine(".x macros/solve/errorsVsPixelHits.cc");

  // Alignment SCT Corrections (Errors) (Vs Hits)
  gROOT->ProcessLine(".x macros/solve/SCTConstants.cc");
  gROOT->ProcessLine(".x macros/solve/corrVsSCTHits.cc");
  gROOT->ProcessLine(".x macros/solve/SCTCorrErrors.cc");   
  gROOT->ProcessLine(".x macros/solve/errorsVsSCTHits.cc");

  // Alignment Corrections Maps
  gROOT->ProcessLine(".x macros/solve/PIXConstantsMap.cc");
  gROOT->ProcessLine(".x macros/solve/SCTConstantsMap.cc");

  // results.root (to be complete)
    //  gROOT->ProcessLine(".x macros/common/VtxFitPos.cc");
    //gROOT->ProcessLine(".x macros/common/show_constantsPixelPulls.cc");
    //gROOT->ProcessLine(".x macros/common/show_constantsSCTPulls.cc");

  return;       
}


//=====================================================================
// printPS()
//=====================================================================
void printPS() {

  cout << "\nStoring the plots in a ps file..." << endl;

  sprintf(filename,"%s/Iter%d_plots.ps",path,IterNumber);

  TCanvas c0;
  sprintf(command,"%s[",filename);
  c0.Print(command);

  // title and residuals
  if (cInfoRes)             cInfoRes->Print(filename);

  // hits and tracks
  if (cHits)                cHits->Print(filename);
  if (cHitCorr)             cHitCorr->Print(filename);
  if (cHitsPerLayer)        cHitsPerLayer->Print(filename);
  if (cProcHitsPerLayer)    cProcHitsPerLayer->Print(filename);

  // Hitmaps
  if (cPIXHitMap)           cPIXHitMap->Print(filename);
  if (cSCTHitMapEc[0])      cSCTHitMapEc[0]->Print(filename);
  if (cSCTHitMapBa)         cSCTHitMapBa->Print(filename);
  if (cSCTHitMapEc[1])      cSCTHitMapEc[1]->Print(filename);

  // Hitmaps XY
  // if (cHitmapsXY)           cHitmapsXY->Print(filename);

  // Silicon Cluster
  if (cSiClusSize)          cSiClusSize->Print(filename);
  if (cSiClusSizeIncAng)    cSiClusSizeIncAng->Print(filename);

  // Residuals
  if (cAllRes)              cAllRes->Print(filename);
  if (cProcAllRes)          cProcAllRes->Print(filename);

  // PIX Barrel/Endcap Residuals
  if (cPIXEndcapARes)        cPIXEndcapARes->Print(filename);
  if (cPIXBarrelRes)        cPIXBarrelRes->Print(filename);
  if (cPIXEndcapCRes)        cPIXEndcapCRes->Print(filename);
  
  // SCT Barrel/Endcap Residuals
  if (cSCTEndcapARes)        cSCTEndcapARes->Print(filename);
  if (cSCTBarrelRes)        cSCTBarrelRes->Print(filename);
  if (cSCTEndcapCRes)        cSCTEndcapCRes->Print(filename);
  
  // Pixel Residuals Vs pT
  if (cPIXEndCapAResVspT)   cPIXEndCapAResVspT->Print(filename);
  if (cPIXBarrelResVspT)    cPIXBarrelResVspT->Print(filename);
  if (cPIXEndCapCResVspT)   cPIXEndCapCResVspT->Print(filename); 
  
  // SCT Residuals Vs pT
  if (cSCTEndCapAResVspT)   cSCTEndCapAResVspT->Print(filename);
  if (cSCTBarrelResVspT)    cSCTBarrelResVspT->Print(filename);
  if (cSCTEndCapCResVspT)   cSCTEndCapCResVspT->Print(filename);
  
  // Residuals per Layer (means)
  if (cResPerLayer)         cResPerLayer->Print(filename);
  if (cProcResPerLayer)     cProcResPerLayer->Print(filename);

  // Residuals Errors
  if (cPixResErrors)        cPixResErrors->Print(filename);
  if (cSCTResErrors)        cSCTResErrors->Print(filename);

  // Resmaps (means)
  if (cResMapMeanPix)          cResMapMeanPix->Print(filename);
  if (cEtaResMapMeanPix)       cEtaResMapMeanPix->Print(filename);
  if (cResMapMeanSCTEcASide0)  cResMapMeanSCTEcASide0->Print(filename);
  if (cResMapMeanSCTEcASide1)  cResMapMeanSCTEcASide1->Print(filename);
  if (cResMapMeanSCTSide0)     cResMapMeanSCTSide0->Print(filename);
  if (cResMapMeanSCTSide1)     cResMapMeanSCTSide1->Print(filename);
  if (cResMapMeanSCTEcCSide0)  cResMapMeanSCTEcCSide0->Print(filename);
  if (cResMapMeanSCTEcCSide0)  cResMapMeanSCTEcCSide1->Print(filename);

  // Resmaps (sigmas)
  if (cResMapSigmaPix)         cResMapSigmaPix->Print(filename);
  if (cEtaResMapSigmaPix)      cEtaResMapSigmaPix->Print(filename);
  if (cResMapSigmaSCTEcASide0) cResMapSigmaSCTEcASide0->Print(filename);
  if (cResMapSigmaSCTEcASide0) cResMapSigmaSCTEcASide1->Print(filename);
  if (cResMapSigmaSCTSide0)    cResMapSigmaSCTSide0->Print(filename);
  if (cResMapSigmaSCTSide1)    cResMapSigmaSCTSide1->Print(filename);
  if (cResMapSigmaSCTEcCSide0) cResMapSigmaSCTEcCSide0->Print(filename);
  if (cResMapSigmaSCTEcCSide0) cResMapSigmaSCTEcCSide1->Print(filename);

  // AlignTrk parameters
  if (cParams)              cParams->Print(filename);
  if (cTruthParams)         cTruthParams->Print(filename);
  if (cTruthDiffParams)     cTruthDiffParams->Print(filename);
  if (cUsedParams)          cUsedParams->Print(filename);  
  if (cCompParams)          cCompParams->Print(filename);

  // AlignTrk momentum
  if (cMomentum)            cMomentum->Print(filename);
  if (cMomentum2)           cMomentum2->Print(filename);
  if (cCompMomentum)        cCompMomentum->Print(filename);

  // AlignTrk Correlations
  if (cParCorr)             cParCorr->Print(filename);
  if (cD0bsCorr)            cD0bsCorr->Print(filename);
  if (cd0Error)             cd0Error->Print(filename);
  if (cz0Error)             cz0Error->Print(filename);
  if (cpTvsEta)             cpTvsEta->Print(filename);
  if (cParCorr2)            cParCorr2->Print(filename);
  
  // Track Matching
  if (cCompTrackMatchingParams) cCompTrackMatchingParams->Print(filename);
  if (cCompTrackMatchingParamsVsD0) cCompTrackMatchingParamsVsD0->Print(filename);
  if (cCompTrackMatchingParamsVsZ) cCompTrackMatchingParamsVsZ->Print(filename);
  if (cCompTrackMatchingParamsVsPhi0) cCompTrackMatchingParamsVsPhi0->Print(filename);
  if (cCompTrackMatchingParamsVsEta) cCompTrackMatchingParamsVsEta->Print(filename);
  if (cCompTrackMatchingParamsVsQoverPt) cCompTrackMatchingParamsVsQoverPt->Print(filename);

  // Trk::Track parameters
  if (cNTracks)             cNTracks->Print(filename);
  if (cTrackhits)           cTrackhits->Print(filename);  
  if (cTrackParams)         cTrackParams->Print(filename);
  if (cTrkMomentum)         cTrkMomentum->Print(filename);
  if (cTruthTrackParams)    cTruthTrackParams->Print(filename);
  if (cTrkTruthDiffParams)  cTrkTruthDiffParams->Print(filename);
  if (cTruthVtx)            cTruthVtx->Print(filename);
  if (cGenParticle)         cGenParticle->Print(filename);

  // Silicon modules properties
  if (cTrkPixelProp)        cTrkPixelProp->Print(filename);
  if (cUsedTrkPixelProp)    cUsedTrkPixelProp->Print(filename);
  if (cTrkSCTProp)          cTrkSCTProp->Print(filename);
  if (cUsedTrkSCTProp)      cUsedTrkSCTProp->Print(filename);

  // Extras
  if (cChi2)                cChi2->Print(filename);
  if (cVtxCandidatePos)     cVtxCandidatePos->Print(filename);
  if (cBeamSpot)            cBeamSpot->Print(filename);

  // Initial module positions
  if (cPixelBarrelPos)      cPixelBarrelPos->Print(filename);
  if (cPixelBarrelMeanPos)  cPixelBarrelMeanPos->Print(filename);
  if (cSCTBarrelPos)        cSCTBarrelPos->Print(filename);
  if (cSCTBarrelMeanPos)    cSCTBarrelMeanPos->Print(filename);

  // Diagonalization info
  if (cDia)                  cDia->Print(filename);
  if (cBigMatrix)            cBigMatrix->Print(filename);

  // Alignment Pixel Corrections (Errors) (Vs Hits)
  if (cPixelConsts)         cPixelConsts->Print(filename);
  if (cCorrVsPixelHits)     cCorrVsPixelHits->Print(filename);
  if (cPixelErrorConsts)    cPixelErrorConsts->Print(filename);
  if (cErrorsVsPixelHits)   cErrorsVsPixelHits->Print(filename);

  // Alignment SCT Corrections (Errors) (Vs Hits)
  if (cSCTConsts)           cSCTConsts->Print(filename);
  if (cCorrVsSCTHits)       cCorrVsSCTHits->Print(filename);
  if (cSCTErrorConsts)      cSCTErrorConsts->Print(filename);
  if (cErrorsVsSCTHits)     cErrorsVsSCTHits->Print(filename);

  // Alignment Pixel Correction Maps
  for (int j=0; j<6; j++) {
    if(cPIXConstantsMap[j]) cPIXConstantsMap[j]->Print(filename);
  }

  // Alignment SCT Correction Maps
  for (int j=0; j<6; j++) {
    if(cSCTConstantsMap[j]) cSCTConstantsMap[j]->Print(filename);
  }

  sprintf(command,"%s]",filename);
  c0.Print(command);

  // Compress it!
  sprintf(name,".!gzip -f %s",filename);
  gROOT->ProcessLine(name);
  
  cout << " - Plots stored successfully!" << endl;

  return;
}



//=====================================================================
// runMonitor()
//=====================================================================
void runMonitor() {

  checkIfSplittedJob();

  cout <<filename << endl;

  SystemInfo();

  if (!openFile()) return;
  readSummary();

  if (processResults) openResultsFile();
  readResultsSummary();

  checks();

  if (iPixelHits==0) SCTOnly   = true;
  if (iSCTHits==0)   PixelOnly = true;

  if(iCosmics==1 && iSetup==5) level = 0;
  else level = 1;

  callMacros();
  cleanSymLinks();

  return;
}


//=====================================================================
// checkExternals()
//=====================================================================
void checkExternals(Char_t *extPath, Int_t extIteration) {

  setStyle();

  Int_t status = 0;
  if (status!=0) {
    cout << " ** monitor ** Problems compiling macros. >> ABORTING << " << endl;
    return;
  }

  // cout << extenalPath << endl;

  // direct file loading
  if (extIteration==-1) {
    cout << "-> direct file loading" << endl; 
    if (strcmp(extPath,"")) {
      IterNumber = extIteration;
      sprintf(filename,"%s",extPath);
    }
    else {
      IterNumber = extIteration;
      sprintf(filename,"%s",path);
    }
    return;
  }

  // check the path
  if (strcmp(extPath,"")) {
    sprintf(path,"%s",extPath);
    cout << "-> using a external path: " << path << endl; 
  }
  else
    cout << "-> using path: " << path << endl; 

  if (checkDir(path)!=0) {
    cout << "ERROR :: path not found!" << endl;
    gROOT->ProcessLine(".q");
  }

  // checkIteration
  if (extIteration>-1) {
    IterNumber = extIteration;
    cout << "-> using a external Iteration: " << IterNumber << endl; 
  }
  else
    cout << "-> using Iteration: " << IterNumber << endl; 
  
  return;
}


//=====================================================================
// checkIfcompiled()
//=====================================================================
Long_t checkIfcompiled(char file[280]) {

  // cout << "-----------------> " << file << endl;

  if(checkFile(file) != 0) {
//     char ifile[180];

    cout << endl;
    cout << " - first compilation" << endl;
    if (!strcmp(file,"../trkana/rntu_C.so"))
      gROOT->ProcessLine(".x ../trkana/rntu.C++(filename,true)");
    else if (!strcmp(file,"../trkana/cntu_C.so"))
      gROOT->ProcessLine(".x ../trkana/cntu.C++(path,NumberOfCPUs,IterNumber)");    
    else if (!strcmp(file,"../trkana/trkana_C.so"))
      gROOT->ProcessLine(".x ../trkana/trkana.C++(0,0,true,IterNumber,name,NumberOfCPUs)");
    firstCompiled = true;
  }

  Long_t id, size, flag, modtime;
  gSystem->GetPathInfo(file,&id,&size,&flag,&modtime);
  
  // cout << "checking: " << file << endl;
  // cout << " * id: " << id << endl;
  // cout << " * size: " << size << " " << endl;
  // cout << " * flag: " << flag << endl;
  // cout << " * modtime: " << modtime << endl;    

  return modtime;
}


//=====================================================================
// checkIfSplittedJob()
//=====================================================================
void checkIfSplittedJob() {
  // cout << "In checkIfSplittedJob()..." << endl;

  Char_t dir[MaxPathSize];
  Char_t dir2[MaxPathSize];

  Char_t filenameTemp[MaxPathSize]; 
  
  sprintf(bigmatrixpath,"%s/../Iter%d_bigmatrix.bin",path,IterNumber);
  
  sprintf(filenameTemp,"%s/../../info.txt",path);
  if (checkFile(filenameTemp)!=0) {
    sprintf(filenameTemp,"%s/../info.txt",path);
    if (checkFile(filenameTemp)!=0) {  
      sprintf(filenameTemp,"%s/info.txt",path);
      if (checkFile(filenameTemp)!=0) {     
	NumberOfCPUs = 1;
	return;
      }
      else sprintf(dir,"%s/Iter%d/",path,IterNumber);
    }
    else sprintf(dir,"%s",path);
    
  }
  else sprintf(dir,"%s",path);
  
  cout << "dir: " << dir << endl;
  cout << endl;

  // if the job has been processed in parallel, derived the number of CPUs used
  NumberOfCPUs = 0;
  
  void *dirp = gSystem->OpenDirectory(dir);
  Char_t *afile;
  while ((afile = const_cast<Char_t *>(gSystem->GetDirEntry(dirp)))) {
    
    // cout << " -------> afile: " << afile << endl;
    if (!strcmp(afile,".")) continue;
    else if (!strcmp(afile,"..")) continue;
    else if (!strcmp(afile,"logs")) continue;
    
    sprintf(dir2,"%s/%s",dir,afile);
    // cout << dir2 << endl;

    if (isDir(dir2)==3)
      NumberOfCPUs++;
  }
  
  //   cout << endl;
  //   cout << " --------------------------------------------------------------" << endl;
  //   cout << "  Number of CPUs used to process the sample: " << NumberOfCPUs << endl;
  //   cout << " --------------------------------------------------------------" << endl;
  //   cout << endl;
  
  //   cout << dir << endl;
  //   cout << endl;

  if (NumberOfCPUs<=0) NumberOfCPUs=1;

  sprintf(path,"%s",dir);

  return;  
}


//=====================================================================
// openFile()
//=====================================================================
bool openFile() {
  // cout << "In openFile()..." << endl;

  createSymLinks();

  if (IterNumber==-1) {
    cout << " -> filename:" << filename << endl;
    file = new TFile(filename);
    TDirectory *trkanaDir = file->GetDirectory("trkana");
    if (trkanaDir==NULL) {
      if (processNtuple()>0) return false;     
    }
    else return true;     
  } 

  // remove trkana.root
  if(reprocessTrkana) {
    sprintf(filename,"%s/Iter%d/Iter%d_trkana.root",path,IterNumber,IterNumber);
    if(checkFile(filename)==0) {
      cout << " removing " << path << "/Iter" << IterNumber << "/Iter" << IterNumber << "_trkana.root" << endl;
      sprintf(command,".!rm %s",filename);
      gROOT->ProcessLine(command);
      sprintf(command,".!ls ../");
      gROOT->ProcessLine(command);
//       sprintf(command,".!rm ../trkana/*.so",filename);
      gROOT->ProcessLine(".!rm ../trkana/*.so");
      firstCompiled = true;
    }
    else {
      sprintf(filename,"%s/Iter%d_trkana.root",path,IterNumber);
      if(checkFile(filename)==0) {
	cout << " removing " << path << "/Iter" << IterNumber << "_trkana.root" << endl;
	sprintf(command,".!rm %s",filename);
	gROOT->ProcessLine(command);
      }
    }
    cout << endl;
  }

  // ask for the trkana.root file
  sprintf(filename,"%s/Iter%d_trkana.root",path,IterNumber);
  if (checkFile(filename)!=0) {

    // if not in path, ask if it is in path/IterX
    sprintf(filename,"%s/Iter%d/Iter%d_trkana.root",path,IterNumber,IterNumber);
    if (checkFile(filename)!=0) {

      // if not let's try with the monitor.root
      if (processNtuple()>0) return false; 
    }
    else sprintf(path,"%s/Iter%d/",path,IterNumber);
  }

  file = new TFile(filename);
  TDirectory *trkanaDir = file->GetDirectory("trkana");
  if (trkanaDir==NULL) {
    if (processNtuple()>0) return false;     
  }
    
  cout << "- Selected file: " << filename << endl;
  
  
  return true;
}


//=====================================================================
// processNtuple()
//=====================================================================
int processNtuple() {
  // cout << "In processNtuple()..." << endl;

//   StatusCode status = SUCCESS;

  if (NumberOfCPUs==1) {

    // ask for the monitor.root file
    sprintf(filename,"%s/Iter%d_monitor.root",path,IterNumber);
    if (checkFile(filename)!=0) {
      
      // if not in path, ask if it is in path/IterX
      sprintf(filename,"%s/Iter%d/Iter%d_monitor.root",path,IterNumber,IterNumber);
      if (checkFile(filename)!=0) {
	cout << " ERROR :: no Iter" << IterNumber << "_monitor.root nor trkana.root nor info.txt files" << endl;
	cleanSymLinks();
	gROOT->ProcessLine(".q");
      }
      else sprintf(path,"%s/Iter%d/",path,IterNumber);
    }
    
    TFile *f1 = new TFile(filename);
    if (f1->IsZombie()) {
      cout << " ERROR :: no Iter" << IterNumber << "_monitor.root nor trkana.root nor info.txt files" << endl;
      cout << " Maybe you wanted to run over a parallel job... if yes, set the right Number of CPUs" << endl;
      gROOT->ProcessLine(".q");
    }
      
    cout << "compiling rntu.C..." << endl;
    if (checkIfcompiled("monitor.cc") > checkIfcompiled("../trkana/rntu_C.so"))
      gROOT->ProcessLine(".x ../trkana/rntu.C++(filename,true)");
    else {
      if (!firstCompiled) {
	cout << " -> no compilation is needed\n" << endl;
	gROOT->ProcessLine(".L ../trkana/rntu_C.so");
	gROOT->ProcessLine("rntu(filename,true)");
      }
    }
  }
  else {
    cout << endl;
    cout << " --------------------------------------------------------------" << endl;
    cout << "  Number of CPUs used to process the sample: " << NumberOfCPUs << endl;
    cout << " --------------------------------------------------------------" << endl;
    cout << endl;
  
    cout << "path: " << path << endl;
  
    cout << "compiling cntu.C..." << endl;
    if (checkIfcompiled("monitor.cc") > checkIfcompiled("../trkana/cntu_C.so"))
      gROOT->ProcessLine(".x ../trkana/cntu.C++(path,NumberOfCPUs,IterNumber)");
    else {
      if (!firstCompiled) {
	cout << " -> no compilation is needed\n" << endl;
	gROOT->ProcessLine(".L ../trkana/cntu_C.so");
	gROOT->ProcessLine("cntu(path,NumberOfCPUs,IterNumber)");
      }
    }
  }

  cout << "\nIter" << IterNumber << "_trkana.root will be copied to: " << path;
  
  if (NumberOfCPUs>1) sprintf(name,"%s/",path);
  else {
    sprintf(name,"%s/Iter%d_trkana.root",path,IterNumber);
    strcpy(name, path);
  }
  
  cout << endl;
  cout << "compiling trkana.C..." << endl;
  if (checkIfcompiled("monitor.cc") > checkIfcompiled("../trkana/trkana_C.so"))
    gROOT->ProcessLine(".x ../trkana/trkana.C++(0,0,true,IterNumber,name,NumberOfCPUs)");
  else {
    if (!firstCompiled) {
      cout << " -> no compilation is needed\n" << endl;
      gROOT->ProcessLine(".L ../trkana/trkana_C.so");
      gROOT->ProcessLine("trkana(0,0,true,IterNumber,name,NumberOfCPUs)");
    }
  }
  
  sprintf(filename,"%s/Iter%d_trkana.root",path,IterNumber);
  file = new TFile(filename);
  if (file->IsZombie()) {
    cout << " - problem processing the ntuple" << endl;
    return(2);
  }
  
  cleanSymLinks();
    
  return(0);
}


//=====================================================================
// readSummary()
//=====================================================================
void readSummary() {

  treeSummary = (TTree*)file->Get("trkana/Summary");
  // treeSummary->Show(0);

  if (treeSummary->GetBranchStatus("events"))
    treeSummary->SetBranchAddress("events",       &iEvents);
  if (treeSummary->GetBranchStatus("tracks"))
    treeSummary->SetBranchAddress("tracks",       &iTracks);
  if (treeSummary->GetBranchStatus("usedTracks"))
    treeSummary->SetBranchAddress("usedTracks",   &iUsedTracks);
  if (treeSummary->GetBranchStatus("hits"))
    treeSummary->SetBranchAddress("hits",         &iHits);
  if (treeSummary->GetBranchStatus("Pixelhits"))
    treeSummary->SetBranchAddress("Pixelhits",    &iPixelHits);
  if (treeSummary->GetBranchStatus("SCThits"))
    treeSummary->SetBranchAddress("SCThits",      &iSCTHits);
  if (treeSummary->GetBranchStatus("usedHits"))
    treeSummary->SetBranchAddress("usedHits",     &iUsedHits); 
  if (treeSummary->GetBranchStatus("AlignOpt"))
    treeSummary->SetBranchAddress("AlignOpt",     &iAlignOpt);
  if (treeSummary->GetBranchStatus("setup"))
    treeSummary->SetBranchAddress("setup",        &iSetup);
  if (treeSummary->GetBranchStatus("Cosmics"))
    treeSummary->SetBranchAddress("Cosmics",      &iCosmics);
  if (treeSummary->GetBranchStatus("BField"))
    treeSummary->SetBranchAddress("BField",       &iBField);
  if (treeSummary->GetBranchStatus("simulation"))
    treeSummary->SetBranchAddress("simulation",   &iSimulation);
  if (treeSummary->GetBranchStatus("misalignment"))
    treeSummary->SetBranchAddress("misalignment", &iMisalignment);
  if (treeSummary->GetBranchStatus("MCS"))
    treeSummary->SetBranchAddress("MCS",          &iNoMCS);
  if (treeSummary->GetBranchStatus("NoMCSCosmics"))
    treeSummary->SetBranchAddress("NoMCSCosmics", &iNoMCSCosmics);
  if (treeSummary->GetBranchStatus("GlobalTag"))
    treeSummary->SetBranchAddress("GlobalTag",    &iGlobalTag);
  if (treeSummary->GetBranchStatus("NtupleVersion"))
    treeSummary->SetBranchAddress("NtupleVersion",     &iNtupleVersion);
  if (treeSummary->GetBranchStatus("AlignProcessLevel"))
    treeSummary->SetBranchAddress("AlignProcessLevel", &iAlignProcessLevel);
  if (treeSummary->GetBranchStatus("VtxCandidate"))
    treeSummary->SetBranchAddress("VtxCandidate", &iVtxCandidate);
  if (treeSummary->GetBranchStatus("BeamSpot"))
    treeSummary->SetBranchAddress("BeamSpot", &iBeamSpot);
  if (treeSummary->GetBranchStatus("TrkCons"))
    treeSummary->SetBranchAddress("TrkCons", &iTrkCons);

  // Track Cut
  if (treeSummary->GetBranchStatus("MinimumHitsPerTrack"))
    treeSummary->SetBranchAddress("MinimumHitsPerTrack",      &iMinimumHitsPerTrack);
  if (treeSummary->GetBranchStatus("MinimumPixelHitsPerTrack"))
    treeSummary->SetBranchAddress("MinimumPixelHitsPerTrack", &iMinimumPixelHitsPerTrack);
  if (treeSummary->GetBranchStatus("MinimumSCTHitsPerTrack"))
    treeSummary->SetBranchAddress("MinimumSCTHitsPerTrack",   &iMinimumSCTHitsPerTrack);

  treeSummary->GetEntry(0);

  if (iGlobalTag!=999) iMisalignment = iGlobalTag;
  if (iNoMCSCosmics!=999) iNoMCS = iNoMCSCosmics;

  if (iSimulation) doTruth = true;
  if (iMisalignment == 0) isNominal = true;

  return;
}




//=====================================================================
// openResultsFile()
//=====================================================================
void openResultsFile() {
  cout << "In openResultsFile()..." << endl;

  sprintf(filename,"%s/Iter%d_results.root",path,IterNumber);
  if (checkFile(filename)!=0)
    sprintf(filename,"%s/../Iter%d_results.root",path,IterNumber);
  if (checkFile(filename)!=0)
    sprintf(filename,"%s/Iter%d/Iter%d_results.root",path,IterNumber,IterNumber);
  if (checkFile(filename)!=0) {
    cout << "no compatible file found in " << filename << endl;
    return;
  }
  else {
    fileResults = new TFile(filename);
      
    treeDia = (TTree*)fileResults->Get("GlobalChi2/Dia");    
    treeConsts = (TTree*)fileResults->Get("GlobalChi2/Constants");
    treeResultsSummary = (TTree*)fileResults->Get("GlobalChi2/Summary");
  }
  
  cout << "- Selected file: " << filename << endl;

}


//=====================================================================
// readResultsSummary()
//=====================================================================
void readResultsSummary() {

  if (treeResultsSummary==NULL) {
    iAlignPars = 6;
    return;
  }
  else {
    // treeResultsSummary->Show(0);
    
    if (treeResultsSummary->GetBranchStatus("AlignLevel"))
      treeResultsSummary->SetBranchAddress("AlignLevel",   &iAlignSolveLevel);
    if (treeResultsSummary->GetBranchStatus("AlignPars"))
      treeResultsSummary->SetBranchAddress("AlignPars",    &iAlignPars);
    if (treeResultsSummary->GetBranchStatus("DoF"))
      treeResultsSummary->SetBranchAddress("DoF",          &iDoF);
    if (treeResultsSummary->GetBranchStatus("ModCut"))
      treeResultsSummary->SetBranchAddress("ModCut",       &iModCut);
    if (treeResultsSummary->GetBranchStatus("NhitsCut"))
      treeResultsSummary->SetBranchAddress("NhitsCut",     &iHitCut);

    if (treeResultsSummary->GetBranchStatus("SoftScaled")) {
      treeResultsSummary->SetBranchAddress("SoftScaled", &iSoftScaled);
      treeResultsSummary->SetBranchAddress("SoftCut", &iSoftCut);
      treeResultsSummary->SetBranchAddress("SoftPIXTXerror", &iSoftPIXTXerror);
      treeResultsSummary->SetBranchAddress("SoftPIXTYerror", &iSoftPIXTYerror);
      treeResultsSummary->SetBranchAddress("SoftPIXTZerror", &iSoftPIXTZerror);
      treeResultsSummary->SetBranchAddress("SoftPIXRXerror", &iSoftPIXRXerror);
      treeResultsSummary->SetBranchAddress("SoftPIXRYerror", &iSoftPIXRYerror);
      treeResultsSummary->SetBranchAddress("SoftPIXRZerror", &iSoftPIXRZerror);
      treeResultsSummary->SetBranchAddress("SoftSCTTXerror", &iSoftSCTTXerror);
      treeResultsSummary->SetBranchAddress("SoftSCTTYerror", &iSoftSCTTYerror);
      treeResultsSummary->SetBranchAddress("SoftSCTTZerror", &iSoftSCTTZerror);
      treeResultsSummary->SetBranchAddress("SoftSCTRXerror", &iSoftSCTRXerror);
      treeResultsSummary->SetBranchAddress("SoftSCTRYerror", &iSoftSCTRYerror);
      treeResultsSummary->SetBranchAddress("SoftSCTRZerror", &iSoftSCTRZerror);
      treeResultsSummary->SetBranchAddress("SoftPIXECTXerror", &iSoftPIXECTXerror);
      treeResultsSummary->SetBranchAddress("SoftPIXECTYerror", &iSoftPIXECTYerror);
      treeResultsSummary->SetBranchAddress("SoftPIXECTZerror", &iSoftPIXECTZerror);
      treeResultsSummary->SetBranchAddress("SoftPIXECRXerror", &iSoftPIXECRXerror);
      treeResultsSummary->SetBranchAddress("SoftPIXECRYerror", &iSoftPIXECRYerror);
      treeResultsSummary->SetBranchAddress("SoftPIXECRZerror", &iSoftPIXECRZerror);
      treeResultsSummary->SetBranchAddress("SoftSCTECTXerror", &iSoftSCTECTXerror);
      treeResultsSummary->SetBranchAddress("SoftSCTECTYerror", &iSoftSCTECTYerror);
      treeResultsSummary->SetBranchAddress("SoftSCTECTZerror", &iSoftSCTECTZerror);
      treeResultsSummary->SetBranchAddress("SoftSCTECRXerror", &iSoftSCTECRXerror);
      treeResultsSummary->SetBranchAddress("SoftSCTECRYerror", &iSoftSCTECRYerror);
      treeResultsSummary->SetBranchAddress("SoftSCTECRZerror", &iSoftSCTECRZerror);
    }
    
    treeResultsSummary->GetEntry(0);

    // extras - CANNOT WORK since htemp is never defined
//     treeConsts->Draw("Tx","","goff");
//     alignedModules = htemp->GetEntries();
//     treeConsts->Draw("Tx","dettype==1","goff");
//     alignedPIXModules = htemp->GetEntries();
//     treeConsts->Draw("Tx","dettype==2","goff");
//     alignedSCTModules = htemp->GetEntries();
  }

  return;
}


//=====================================================================
// ShowStatistics()
//=====================================================================
void ShowStatistics() {

  switch(iSetup)
    {
    case 0:
      sprintf(aSetup,"Cone Geometry");
      sprintf(psSetup,"Cone");
      break;
      
    case 1:
      sprintf(aSetup,"DoubleCone Geometry");
      sprintf(psSetup,"DoubleCone");
      break;
      
    case 2:
      sprintf(aSetup," Barrel Geometry");
      sprintf(psSetup,"Barrel");
      SCTBarrelOnly = true;
      PixelBarrelOnly = true;
      break;
      
    case 3:
      sprintf(aSetup," EndCap Geometry");
      sprintf(psSetup,"EndCap");
      break;
	
    case 4:
      sprintf(aSetup," Full Geometry");
      sprintf(psSetup,"Full");
      break;   
	
    case 5:
      iCosmics = 1;
      sprintf(aSetup,"SR1 Geometry");
      sprintf(psSetup,"SR1");
      break;      
	
    } // end switch statement 

  // More Options
  if (iCosmics && iSetup!=5) {
    sprintf(aSetup,"%s - %s",aSetup,"Cosmics in the Pit");
    sprintf(psSetup,"%s_%s",psSetup,"Cosmics");
  }

  // SR1 Cosmics labels    
  if (iSetup==5) {
    if (iSimulation) {
      if (iMisalignment == 0) {
	sprintf(aSetup,"%s - %s",aSetup,"Sim Data - Nominal");
	sprintf(psSetup,"%s_%s",psSetup,"SimData_Nominal");
      }
      else if (iMisalignment == 1) {
	sprintf(aSetup,"%s - %s",aSetup,"Sim Data - sctlocal1");
	sprintf(psSetup,"%s_%s",psSetup,"SimData_sctlocal1");
      }
      else if (iMisalignment == 2) {
	sprintf(aSetup,"%s - %s",aSetup,"Sim Data - sctdisp1");
	sprintf(psSetup,"%s_%s",psSetup,"SimData_sctdisp1");
      }
    }
    else {
      sprintf(aSetup,"%s - %s",aSetup,"Real Data");
      sprintf(psSetup,"%s_%s",psSetup,"RealData");
    }
  }
  // Collisions and Cosmics in the Pit
  else {
    if (iMisalignment == 0) {
      sprintf(aSetup,"%s (%s)",aSetup,"Nominal");
      sprintf(psSetup,"%s_%s",psSetup,"Nominal");
    }
    else if (iMisalignment == 1) {
      sprintf(aSetup,"%s (%s)",aSetup,"Perfect");
      sprintf(psSetup,"%s_%s",psSetup,"Perfect");
    }
    else if (iMisalignment == 2) {
      sprintf(aSetup,"%s (%s)",aSetup,"Aligned");
      sprintf(psSetup,"%s_%s",psSetup,"Aligned");
    }
  }
    
  if (iAlignOpt && !iCosmics && iSetup!=5) sprintf(psSetup,"%s_%s",psSetup,"VtxFit");    

  // Simulation or RealData?
  if (iSimulation) 
    sprintf(aSim,"%s","Simulation");
  else 
    sprintf(aSim,"%s","Real Data");

  // -------------------------------------------------------------------------
  //
  // Show Statistics
  //
  // -------------------------------------------------------------------------
  cout << endl;
  cout << "\t--------------------------------------------------------"<< endl;
  cout << "\t             Statistics" << endl;
  cout << "\t--------------------------------------------------------"<< endl;
  cout << "\t" << aSetup << endl;
  cout << "\t - Running over " << aSim << endl;
  cout << "\t - Iteration: " << IterNumber << endl;
  cout << "\t - Events: " << iEvents << endl;
  cout << "\t - Total AlignTrks: " << iTracks << endl;
  cout << "\t  - Total Hits: " << iHits << endl;
  cout << "\t   - Pixel Hits: " << iPixelHits << endl;
  cout << "\t   - SCT Hits: " << iSCTHits << endl;
  cout << "\t - Processed AlignTrks: " << iUsedTracks << endl;
  cout << "\t  - Processed Hits: " << iUsedHits << endl;
  cout << "\t--------------------------------------------------------"<< endl;

  if (doTrkTrack) {
    cout << "\t - Total Trk::Tracks: " << iTrkTracks << endl;
    if (isTRT) cout << "\t  -- Silicon+TRT Tracks --" << endl;
    else  cout << "\t  -- Silicon Only Tracks --" << endl;
    cout << "\t--------------------------------------------------------"<< endl;
  }

  if (iAlignOpt) cout << "\t - Using VtxFit" << endl;
  else cout << "\t - No using VtxFit" << endl;

  if (iBField!=999) cout << "\t - B Field: " << iBField << " Tesla"  << endl;

  if (iMinimumHitsPerTrack!=999) {
    cout << "\t--------------------------------------------------------"<< endl;
    cout << "\t - Internal Track Cut " << endl;
    cout << "\t    - Minimum required hits: " << iMinimumHitsPerTrack << endl;
    cout << "\t    - Minimum required Pixel hits: " << iMinimumPixelHitsPerTrack << endl;
    cout << "\t    - Minimum required SCT hits: " << iMinimumSCTHitsPerTrack << endl;
    cout << "\t--------------------------------------------------------"<< endl;
  }

  if (iAlignProcessLevel!=999)
    cout << "\t - AlignProcessLevel: " << iAlignProcessLevel << endl;
       
  if (iNtupleVersion!=999)
    cout << "\t - Ntuple version: " << iNtupleVersion << endl;

  if (doTruth)
    cout << "\t - Truth is available in the ntuple " << endl;
  else
    cout << "\t - No Truth is available in the ntuple " << endl;

  cout << "\t--------------------------------------------------------"<< endl;
  cout << endl;

  if (fileResults!=NULL) {
    cout << "\t--------------------------------------------------------"<< endl;
    cout << "\t          Solve Summary" << endl;
    cout << "\t--------------------------------------------------------"<< endl;
    cout << "\t - AlignSolveLevel: " << iAlignSolveLevel << endl;
    cout << "\t - HitCut: " << iHitCut << endl;
    cout << "\t - Number of aligned parameters : " << iAlignPars << endl;
    cout << "\t - Number of aligned modules: " << alignedModules << endl;
    cout << "\t    - Pixel modules: " << alignedPIXModules << endl;
    cout << "\t    - SCT modules: " << alignedSCTModules << endl;
    cout << "\t - Number of DoFs: " << iDoF << endl;
    cout << "\t - ModCut: " << iModCut << endl;
    cout << "\t--------------------------------------------------------"<< endl;
    cout << endl;
  }

  return;
}

//=====================================================================
// StoreStatistics()
//=====================================================================
void StoreStatistics() {

  sprintf(filename,"%s/Iter%d_summary.txt",path,IterNumber);
  std::string summaryfilename = filename;
  std::fstream sumFile(summaryfilename.c_str(), std::ios::out);
  sumFile.setf(std::ios::fixed);
  sumFile.setf(std::ios::showpoint);
  sumFile.precision(6);

  sumFile << " --------------------------------------------------------------" << endl;
  sumFile << "  Number of CPUs used to process the sample: " << NumberOfCPUs << endl;
  sumFile << " --------------------------------------------------------------" << endl;
  
  sumFile << endl;
  sumFile << "\t--------------------------------------------------------"<< endl;
  sumFile << "\t             Statistics" << endl;
  sumFile << "\t--------------------------------------------------------"<< endl;
  sumFile << "\t" << aSetup << endl;
  sumFile << "\t - Running over " << aSim << endl;
  sumFile << "\t - Iteration: " << IterNumber << endl;
  sumFile << "\t - Events: " << iEvents << endl;
  sumFile << "\t - Total AlignTrks: " << iTracks << endl;
  sumFile << "\t  - Total Hits: " << iHits << endl;
  sumFile << "\t   - Pixel Hits: " << iPixelHits << endl;
  sumFile << "\t   - SCT Hits: " << iSCTHits << endl;
  sumFile << "\t - Processed AlignTrks: " << iUsedTracks << endl;
  sumFile << "\t  - Processed Hits: " << iUsedHits << endl;
  sumFile << "\t--------------------------------------------------------"<< endl;
  
  if (doTrkTrack) {
    sumFile << "\t - Total Trk::Tracks: " << iTrkTracks << endl;
    if (isTRT) sumFile << "\t  -- Silicon+TRT Tracks --" << endl;
    else  sumFile << "\t  -- Silicon Only Tracks --" << endl;
    sumFile << "\t--------------------------------------------------------"<< endl;
  }
  
  if (iAlignOpt) sumFile << "\t - Using VtxFit" << endl;
  else sumFile << "\t - No using VtxFit" << endl;

  if (iBField!=999) sumFile << "\t - B Field: " << iBField << " Tesla"  << endl;

  if (iMinimumHitsPerTrack!=999) {
    sumFile << "\t--------------------------------------------------------"<< endl;
    sumFile << "\t - Internal Track Cut " << endl;
    sumFile << "\t    - Minimum required hits: " << iMinimumHitsPerTrack << endl;
    sumFile << "\t    - Minimum required Pixel hits: " << iMinimumPixelHitsPerTrack << endl;
    sumFile << "\t    - Minimum required SCT hits: " << iMinimumSCTHitsPerTrack << endl;
    sumFile << "\t--------------------------------------------------------"<< endl;
  }

  if (iAlignProcessLevel!=999)
    sumFile << "\t - AlignProcessLevel: " << iAlignProcessLevel << endl;
       
  if (iNtupleVersion!=999)
    sumFile << "\t - Ntuple version: " << iNtupleVersion << endl;

  if (doTruth)
    sumFile << "\t - Truth is available in the ntuple " << endl;
  else
    sumFile << "\t - No Truth is available in the ntuple " << endl;

  sumFile << "\t--------------------------------------------------------"<< endl;
  sumFile << endl;

  if (fileResults!=NULL) {
    sumFile << "\t--------------------------------------------------------"<< endl;
    sumFile << "\t          Solve Summary" << endl;
    sumFile << "\t--------------------------------------------------------"<< endl;
    sumFile << "\t - AlignSolveLevel: " << iAlignSolveLevel << endl;
    sumFile << "\t - HitCut: " << iHitCut << endl;
    sumFile << "\t - Number of aligned parameters : " << iAlignPars << endl;
    sumFile << "\t - Number of aligned modules: " << alignedModules << endl;
    sumFile << "\t    - Pixel modules: " << alignedPIXModules << endl;
    sumFile << "\t    - SCT modules: " << alignedSCTModules << endl;
    sumFile << "\t - Number of DoFs: " << iDoF << endl;
    sumFile << "\t - ModCut: " << iModCut << endl;
    sumFile << "\t--------------------------------------------------------"<< endl;
    sumFile << endl;
  }
  sumFile.close();

  return;
}


//=====================================================================
// setStyle()
//=====================================================================
void setStyle() {

  if (AtlasStyle) {
    gROOT->SetStyle("ATLAS");

    // Extra options to the ATLAS Style
    Float_t jsize = 0.04;

    gStyle->SetLabelSize(jsize,"x");
    gStyle->SetLabelSize(jsize,"y");
    gStyle->SetLabelSize(jsize-0.01,"z");

    gStyle->SetTitleSize(jsize,"x");  
    gStyle->SetTitleSize(jsize,"y");
    gStyle->SetTitleSize(jsize,"z");
    gStyle->SetTextFont(42);

    gStyle->SetStripDecimals(false);    
    TGaxis::SetMaxDigits(4);
    gStyle->SetPalette(1);
  }
  else
    gROOT->ProcessLine(".x include/style.cc");   

//   gROOT->ProcessLine(".L include/utils.cc");

  gROOT->ForceStyle();

}


//=====================================================================
// checks()
//=====================================================================
void checks() {

  // if (doTruth) cout << "it seems that truth exists" << endl;
  // else cout << "no truth" << endl;

  // check for truth
  TH1F *h1temp = (TH1F*)file->Get("trkana/Track_Parameters/hTruzD0Zoom_0");
  if (h1temp!=NULL) {
    doTruth = true;
    // cout << "it seems that truth exists because the truth histos exist!" << endl;
  }
  else
    doTruth = false;

  // second check!
  if (h1temp->GetMean()==0.0 && !iCosmics)
    doTruth = false;

  if (!iSimulation) doTruth = false;

  // cout << "  ---> doTruth: " << doTruth << endl;

  // -----------------------------------------------------------------

  // check for SiCluster branch
  TTree *treeSiCluster = (TTree*)file->Get("trkana/SiCluster");
  if (treeSiCluster!=NULL) doSiCluster = true;
  
  // check for Trk::Track branch
  TTree *treeTrkTrack = (TTree*)file->Get("trkana/TrkTrack");
  if (treeTrkTrack!=NULL) doTrkTrack = true;

  if (doTrkTrack) {
    // Fill iTrkTracks
    TH1F *hTrkTrackTemp_d0 = (TH1F*)file->Get("trkana/TrkTrack/hTRecoD0_0");
    iTrkTracks = (int)hTrkTrackTemp_d0->GetEntries();
    delete hTrkTrackTemp_d0;
  }

  // -----------------------------------------------------------------

  // Check if EndCaps exists
  TH1F *hSCTEndCapTemp = (TH1F*)file->Get("trkana/SCTRes/hSCTResEndcap_0");
  if (hSCTEndCapTemp->GetMean()==0.0) SCTBarrelOnly = true;

  // -----------------------------------------------------------------

  // Check if Pixels exists
  TH1F *hPixelHitsTemp = (TH1F*)file->Get("trkana/PIXHits/hPIXhits_0");
  if (hPixelHitsTemp->GetMean()==0.0) SCTOnly = true;
  
  // -----------------------------------------------------------------

  // Check if SCT exists
  TH1F *hSCTHitsTemp = (TH1F*)file->Get("trkana/SCTHits/hSCThits_0");
  if (hSCTHitsTemp->GetMean()==0.0) PixelOnly = true;
  
  // -----------------------------------------------------------------

  // -------------------------------------------------------------------------------
  // Check if the tree exists
  TTree *TrkMatchTree = (TTree*)file->Get("trkana/TrkMatching");
  if (TrkMatchTree!=NULL) TrackMatching = true;
  // -------------------------------------------------------------------------------

  // Check if it is an old ntuple (SR1, DC3, etc..)
  TTree *OldSetupTree = (TTree*)file->Get("trkana/OldSetup");
  if (OldSetupTree!=NULL) {
    // OldSetupTree->Show(0);
    Int_t setup = 0;
    OldSetupTree->SetBranchAddress("setup",&setup);
    OldSetupTree->GetEntry(0);
    // cout << "setup: " << setup << endl;

    // SR1 Setup
    if (setup==5) {
      // cout << "SR1 ntuple" << endl;
      iSetup = setup;      
      SR1Ntuple = true;
      doTruth = false;
      SCTBarrelOnly = true;
      iBField = 0.0;
      iNtupleVersion = 0.8;
      SCTOnly = true;
    }
  }

  // -----------------------------------------------------------------

  // Check if there is a VtxCandidate
  if (!iCosmics) {
//     TH1F *hSCTHitsTemp = (TH1F*)file->Get("trkana/SCTHits/hSCThits_0");
    TH1F *hVtxTemp = (TH1F*)file->Get("trkana/Track_Parameters/hVtxX");
    if (hVtxTemp->GetEntries()==0) doVtxCandidate = false;
  }

  if(iVtxCandidate==0) doVtxCandidate = false;

  // -----------------------------------------------------------------

  // Check if Silicon only tracks
  TH1F *hTrknTRThitsTemp = (TH1F*)file->Get("trkana/TrkTrack/hTrkTRThits_0");
  if (hTrknTRThitsTemp->GetMean()!=0.0) isTRT = true;

  // Show Statistics
  ShowStatistics();
  StoreStatistics();

}

//=====================================================================
// SystemInfo()
//=====================================================================
void SystemInfo() {

  SysInfo_t system;
  gSystem->GetSysInfo(&system);
  
  cout << endl;
  cout << " ----------------------------------------------------------------------------" << endl;
  cout << "  System Summary" << endl;
  cout << "   - OS: " << system.fOS << endl;
  cout << "   - computer model: " << system.fModel << endl;
  cout << "   - " << system.fCpus << "x " << system.fCpuType 
       << " CPU(s) running at " << (system.fCpuSpeed)/1000 << " GHz" << endl;
  cout << "   - memory: " << (system.fPhysRam)/1000 << " GB" << endl;
  cout << " ----------------------------------------------------------------------------" << endl;
  cout << endl;

}
