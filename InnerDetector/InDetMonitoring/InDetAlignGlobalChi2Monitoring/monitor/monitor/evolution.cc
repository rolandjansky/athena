/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <include/evolution.h>


//=====================================================================
//
//  evolution
//
//=====================================================================
void evolution(Char_t extPath[MaxPathSize]="") {
  
  // InputPath (if externalPath exist, the path will be ignored)
  sprintf(path,"/tmp/%s/",gSystem->GetUserInfo(gSystem->GetUid())->fUser.Data());

  // paths for internal testing
  // sprintf(path,"/home/cescobar/work/alignment/results/calibstream/fdr2/caf_tests/nominal_ES04_Level1_WithCosmics/");
  sprintf(path,"/home/cescobar/work/alignment/results/calibstream/fdr2/fdr2_week/day1/");
  //sprintf(path,"/afs/cern.ch/user/m/moles/home/Dades_Monitor/");

  // if NumberOfIters = -1, will plot all the available iterations
  NumberOfIters = -1;

  // gROOT->SetBatch(kTRUE);
  AtlasStyle = false;
  PrintLevel = 3;

  // ----------------------------------------------------------------------------------------

  init();
  printPS();

  return;
}


//=====================================================================
// callMacros()
//=====================================================================
void callMacros() {

  // residual means/sigmas Vs iteration
  gROOT->ProcessLine(".x macros/flow/BarrelResVsIters.cc(1)");

  // constants Vs iteration
  // gROOT->ProcessLine(".x macros/flow/ConstantsVsIters.cc()");

}


//=====================================================================
// printPS()
//=====================================================================
void printPS() {
  
  cout << "\nStoring the plots in a ps file..." << endl;
  
  sprintf(filename,"%s/evolution.ps",path);
 
  TCanvas c0;
  sprintf(command,"%s[",filename);
  c0.Print(command);
  
  //---------------------------------------------------------------------

  if (cBarrelResVsIters) cBarrelResVsIters->Print(filename);
  if (cPIXPositionVsIter)cPIXPositionVsIter ->Print(filename);

  //---------------------------------------------------------------------

  sprintf(command,"%s]",filename);
  c0.Print(command);
  
  // Compress it!
  sprintf(name,".!gzip -f %s",filename);
  gROOT->ProcessLine(name);
  
  cout << " - Plots stored successfully!" << endl;
  
  return;
}



//=====================================================================
// init()
//=====================================================================
void init() {

  setStyle();
  gROOT->ProcessLine(".L include/utils.cc");

  // if NumberOfIters = -1, will plot all the available iterations
  // so, the NumberOfIters will be a big number
  if (NumberOfIters==-1) NumberOfIters = 200;

  for (int iter=0; iter<NumberOfIters; iter++) {
    checkIfMixing(iter);
    accuFiles(iter);
  }

  PlotNtuple = true;  
  callMacros();

  return;
}


//=====================================================================
// checkIfMixing()
//=====================================================================
void checkIfMixing(Int_t iter) {

  // cout << " -------> path: " << path << endl; 
  void *dirp = gSystem->OpenDirectory(path);
  Char_t *afile;

  sprintf(filename,"Iter%d_trkana.root",iter);
  while ((afile = const_cast<Char_t *>(gSystem->GetDirEntry(dirp)))) {
    // cout << " -------> afile: " << afile << endl; 
    if (!strcmp(afile,filename)) { 
      isMixed = false; 
      sprintf(pathTemp,"%s",path);
      return; }
  }

  sprintf(path2,"%s/Iter%d/",path,iter);
  // cout << " -------> path2: " << path2 << endl; 

  dirp = gSystem->OpenDirectory(path2);
  while ((afile = const_cast<Char_t *>(gSystem->GetDirEntry(dirp)))) {
    if (!strcmp(afile,filename)) { 
      isMixed = false;
      sprintf(pathTemp,"%s",path2);
      return; }
  }

  dirp = gSystem->OpenDirectory(path);
  while ((afile = const_cast<Char_t *>(gSystem->GetDirEntry(dirp)))) {
    // cout << " -------> afile: " << afile << endl; 
    if (!strcmp(afile,"Collision")) {
      isMixed = true; 
      // cout << " --> Collision: " << afile << endl; 
    }
    else if (!strcmp(afile,"Cosmics")) {
      isMixed = true;
      // cout << " --> Cosmics: " << afile << endl;
      sprintf(pathTemp,"%s",path);
      return;
    }
  }

  dirp = gSystem->OpenDirectory(path2);
  while ((afile = const_cast<Char_t *>(gSystem->GetDirEntry(dirp)))) {
    // cout << " -------> afile: " << afile << endl;
    if (!strcmp(afile,"Collision")) {
      isMixed = true; 
      // cout << " --> Collision dir: " << afile << endl; 
    }
    else if (!strcmp(afile,"Cosmics")) {
      isMixed = true;
      // cout << " --> Cosmics: " << afile << endl;
      sprintf(pathTemp,"%s",path2);
      return;
    }
  }

  return;
}


//=====================================================================
// accuFiles()
//=====================================================================
void accuFiles(Int_t iter) {

  // cout << "isMixed: " << isMixed << endl;

  sprintf(inputpath,"%s",pathTemp);
  // cout << " -------> pathTemp: " << pathTemp << endl; 

  if (isMixed) sprintf(filename,"%s/Collision/Iter%d_trkana.root",inputpath,iter);
  else sprintf(filename,"%s/Iter%d_trkana.root",inputpath,iter);
  if(checkFile(filename)!=0) {
    if (PrintLevel < 2) {
      if (isMixed) cout << " ERROR :: problem reading " << inputpath << "Collision/Iter" << iter << "_trkana.root" << endl;
      else cout << " ERROR :: problem reading " << inputpath << "/Iter" << iter << "_trkana.root" << endl;
    }
    if (IterCounter==0) gROOT->ProcessLine(".q");
    else return;
  }

  TFile *fileTemp = new TFile(filename);
  TDirectory *trkanaDir = fileTemp->GetDirectory("trkana");
  if (trkanaDir==NULL) {
    return;
  }
  else {
    file = fileTemp;
    cout << "- Opening file: " << filename << endl;
    callMacros();
    IterCounter++;
  }

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

  gROOT->ForceStyle();

  return;
}
