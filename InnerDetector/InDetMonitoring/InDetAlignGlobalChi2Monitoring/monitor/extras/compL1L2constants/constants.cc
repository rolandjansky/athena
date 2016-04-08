/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <constants.h>


//=====================================================================
//
//  constants
//
// Usage: root constants.cc++
//
//=====================================================================
void constants(Char_t extPath[MaxPathSize]="", 
 	       Bool_t extBarrelOnly=false, 
 	       Bool_t extCustomComp=true ) {
  
  // InputPath (if externalPath exist, the path will be ignored)
  // sprintf(path,"/tmp/%s/",gSystem->GetUserInfo(gSystem->GetUid())->fUser.Data());
  //  sprintf(path,"/home/cescobar/work/alignment/results/calibstream/fdr2/fdr2_week/day1/");
  sprintf(path,"/l/cescobar/work/alignment/results/csc/multimuons/sets/Pawel_setI/multimuons/");

  // gROOT->SetBatch(kTRUE);
  AtlasStyle = false;
  PrintLevel = 3;
  BarrelOnly = extBarrelOnly;
  customComp = extCustomComp;
  showNominal = true;
  doLevel2ForRef = true; 

  // ----------------------------------------------------------------------------------------

  init(extPath);
  if (!customComp) {
    for (int iter=0; iter<NumberOfIters; iter++) {
      if (NumberOfIters==1) sprintf(filename,"%s/Iter%d/Iter%d_AlignmentConstants.txt",path,iter,iter);
      else sprintf(filename,"%s/Iter%d/Iter%d_AlignmentConstants.txt",path,iter,iter);
      read(filename);
      sprintf(filename,"Iteration %d",iter);
      if (NumberOfIters==1) PlotConstants(2,0.9,kBlue,filename);
      else PlotConstants(2,0.9,iColor[iter],filename);
    }
  }
  else {

    // *************************************************************
    // Colors: kYellow, kOrange, kRed, kPink, kMagenta, kViolet, 
    //         kBlue, kAzure, kCyan, kTeal, kGreen, kSpring, kGray
    // ************************************************************
    
    //    read("reference/OFLCOND-CSC-00-01-04.txt");
    //    PlotConstants(5,0.9,kBlue,"OFLCOND-CSC-00-01-04");
    
    //     read("reference/OFLCOND-CSC-00-01-05.txt");
    //     PlotConstants(5,0.9,kGreen,"OFLCOND-CSC-00-01-05");

    /*
    read("/afs/cern.ch/user/a/atlidali/public/cescobar/fdr2/day1/Iter0_AlignmentConstants.txt");
    PlotConstants(25,0.9,kGreen,"Iter0 - FDR2 day1");

    read("/afs/cern.ch/user/a/atlidali/public/cescobar/fdr2/day1/Iter1_AlignmentConstants.txt");
    PlotConstants(25,0.9,kRed,"Iter1 - FDR2 day1");

    read("/afs/cern.ch/user/a/atlidali/public/cescobar/fdr2/day1/Iter2_AlignmentConstants.txt");
    PlotConstants(25,0.9,kOrange,"Iter2 - FDR2 day1");
    */
    
//     read("/afs/cern.ch/user/a/atlidali/public/cescobar/fdr2/day1/Iter3_AlignmentConstants.old");
//     PlotConstants(25,0.9,kBlue,"Iter3 - FDR2 day1");

/*
  read("/afs/cern.ch/user/a/atlidali/public/cescobar/fdr2/day1/Iter3_AlignmentConstants.txt");
    PlotConstants(26,0.9,kRed,"Iter3 - FDR2 day1");

    read("/afs/cern.ch/user/a/atlidali/public/cescobar/fdr2/day1/Iter4_AlignmentConstants.txt");
    PlotConstants(25,0.9,kBlue,"Iter4 - FDR2 day1");

    read("/afs/cern.ch/user/a/atlidali/public/cescobar/fdr2/day1/Iter5_AlignmentConstants.txt");
    PlotConstants(25,0.9,kOrange,"Iter5 - FDR2 day1");
*/
/*
    read("/afs/cern.ch/user/a/atlidali/public/cescobar/fdr2/day1/Iter6_AlignmentConstantsPreCog.txt");
    PlotConstants(25,0.9,kGreen,"Iter6 PreCoG - FDR2 day1");

    read("/afs/cern.ch/user/a/atlidali/public/cescobar/fdr2/day1/Iter6_AlignmentConstants.txt");
    PlotConstants(25,0.9,kBlue,"Iter6 - FDR2 day1");

    read("/afs/cern.ch/user/a/atlidali/public/cescobar/fdr2/day1/InDetCog_Constants.txt");
    PlotConstants(26,0.9,kRed,"ID CoG - FDR2 day1");
*/


    read("/l/cescobar/work/alignment/results/csc/multimuons/sets/Pawel_setI/multimuons/Iter0_CSC_AlignmentConstants.txt");
    PlotConstants(25,0.9,kGreen,"Iter0 - CSC Multimuons");
    read("/l/cescobar/work/alignment/results/csc/multimuons/sets/Pawel_setI/multimuons/Iter7_CSC_AlignmentConstants.txt");
    PlotConstants(26,0.9,kRed,"Iter7 - CSC Multimuons");
  }

  // PlotHists();
  PrintCanvas();

  return;
}


//=====================================================================
// init()
//=====================================================================
void init(Char_t extPath[MaxPathSize]) {
   
  checkExternals(extPath);

  // reserve pointers
  radiusLy = new Float_t[31];
  mcode = new Int_t[31];
  modsLy = new Int_t[31];
  radiusLy = new Float_t[31];
  cog  = new TVector3[31];

  // Resert the vectors
  TVector3 clear(0.0,0.0,0.0);
  for(int m=0; m<31; m++) { 
    mcode[m]=0;
    modsLy[m]=0; 
    radiusLy[m]=0.0; 
    cog[m]=clear;
  }

  // Define the ntuple
  ntuple = new TNtuple("ntuple","ntuple","level:bec:radius:Tx:Ty:Tz:Rx:Ry:Rz");  
  ntuple2 = new TNtuple("ntuple2","ntuple2","iter:level:PixSCT:BEc:layer:sector:ring:Tx:Ty:Tz:Rx:Ry:Rz:reference");  

  if (!customComp)
    checkNumberOfIters();

  // Reference Points (CSC values from DB)
  read("reference/OFLCOND-CSC-00-01-00.txt");
  problem = good;
  if (doLevel2ForRef) sprintf(filename,"CSC Reference");
  else sprintf(filename,"CSC Reference (just L2)");
  PlotConstants(4,0.9,kBlack,filename);

  if (showNominal) {
    // Reference Points (CSC values from DB)
    read("reference/OFLCOND-CSC-00-00-00.txt");
    PlotConstants(5,0.9,kBlack,"Nominal");
  }

  // checkNCPUs();

  return;
}


//=====================================================================
// checkExternals()
//=====================================================================
void checkExternals(Char_t extPath[MaxPathSize]) {

  // cout << extenalPath << endl;
  if(!customComp) {
    // check the path
    if (strcmp(extPath,"")) {
      sprintf(path,"%s",extPath);
      cout << "-> using a external path: " << path << endl; 
    }
    else
      cout << "-> using path: " << path << endl; 
  }
  else
    cout << "-> using custom paths" << endl; 
  
  return;
}


//=====================================================================
// checkNumberOfIters()
//=====================================================================
void checkNumberOfIters() {

  Char_t dir[MaxPathSize];
  Char_t dir2[MaxPathSize];
  sprintf(dir,"%s",path);
  // cout << "dir: " << dir << endl;
  // cout << endl;

  if(checkFile(dir)==1) 
    NumberOfIters = 1;
  else {
    // if the job has been processed in parallel, derived the number of CPUs used
    NumberOfIters = 0;
    
    void *dirp = gSystem->OpenDirectory(dir);
    Char_t *afile;
    while ((afile = const_cast<Char_t *>(gSystem->GetDirEntry(dirp)))) {
      
      // cout << " -------> afile: " << afile << endl;
      if (!strcmp(afile,".")) continue;
      else if (!strcmp(afile,"..")) continue;
      else if (!strcmp(afile,"logs")) continue;
      else if (!strcmp(afile,"FirstBs")) continue;
      //      if (!strcmp(afile,".") || !strcmp(afile,"..")) {
      
      sprintf(dir2,"%s/%s",dir,afile);
      // cout << dir2 << endl;

      if (checkDir(dir2)==3)
	NumberOfIters++;
    }
  }

  if (NumberOfIters==0) {
    sprintf(dir,"%s/Iter0_AlignmentConstants.txt",path);
    if(checkFile(dir)!=1)
      NumberOfIters = 1;
  }

  cout << endl;
  cout << " --------------------------------------------" << endl;
  cout << "  Number of Iterations: " << NumberOfIters << endl;
  cout << " --------------------------------------------" << endl;
  cout << endl;
  
  return;  
}


//=====================================================================
// checkFile()
//=====================================================================
Long_t checkFile(Char_t fileName[MaxPathSize]) {

  Long_t id, size, flag, modtime;
  Int_t status = gSystem->GetPathInfo(fileName,&id,&size,&flag,&modtime);

  if (PrintLevel<1) {
    cout << "checking file: " << fileName << endl;
    cout << " * status: " << status << endl;
    cout << " * id: " << id << endl;
    cout << " * size: " << size << " " << endl;
    cout << " * flag: " << flag << endl;
    cout << " * modtime: " << modtime << endl;
  }

  return status;
}


//=====================================================================
// checkDir()
//=====================================================================
Long_t checkDir(Char_t file[MaxPathSize]) {

  Long_t id, size, flag, modtime;
  gSystem->GetPathInfo(file,&id,&size,&flag,&modtime);

  // cout << "checking: " << file << endl;
  // cout << " * id: " << id << endl;
  // cout << " * size: " << size << " " << endl;
  // cout << " * flag: " << flag << endl;
  // cout << " * modtime: " << modtime << endl;

  return flag;
}


//=====================================================================
// PrintCanvas()
//=====================================================================
void PrintCanvas() {
  if (problem) {

    if (PrintLevel<1) cout << " In PrintCanvas()..." << endl;

    PrintLegend();

    cout << endl;
    cout << "Creating plots..." << endl;

    sprintf(filename,"plots.ps");

    TCanvas c0;
    sprintf(command,"%s[",filename);
    c0.Print(command);

    if(cConstsBarrel)  cConstsBarrel->Print(filename);
    if(cConstsEndCapA) cConstsEndCapA->Print(filename);
    if(cConstsEndCapC) cConstsEndCapC->Print(filename);

    sprintf(command,"%s]",filename);
    c0.Print(command);

    // Compress it!
    sprintf(command,".!gzip -f %s",filename);
    gROOT->ProcessLine(command);
  
    cout << " - Plots stored successfully!" << endl;
  }
  return;
}


//=====================================================================
// initGraphics()
//=====================================================================
void initGraphics(Int_t option) {
  if (PrintLevel<1) cout << " In initGraphics()..." << endl;

  if (!graphics) SetStyle();
  SetStyle();

  if (option==0) {

    cConstsBarrel = new TCanvas("cConstsBarrel","cConstsBarrel",1150,700);
    cConstsBarrel->Divide(3,2);

    if (!BarrelOnly) {
      cConstsEndCapA = new TCanvas("cConstsEndCapA","cConstsEndCapA",1150,700);
      cConstsEndCapA->Divide(3,2);
      
      cConstsEndCapC = new TCanvas("cConstsEndCapC","cConstsEndCapC",1150,700);
      cConstsEndCapC->Divide(3,2);
    }

    // Define the Legend
    float lowerLimit = 0.0;
    if (!customComp) {
      switch(NumberOfIters) {
      case 1:
	lowerLimit = 0.87; break;
      case 2:
	lowerLimit = 0.81; break;
      case 3:
	lowerLimit = 0.78; break;
      case 4:
	lowerLimit = 0.80; break;
      case 5:
	lowerLimit = 0.70; break;
      case 6:
	lowerLimit = 0.70; break;
      case 7:
	lowerLimit = 0.70; break;
      case 8:
	lowerLimit = 0.70; break;
      case 9:
	lowerLimit = 0.70; break;
      case 10:
	lowerLimit = 0.70; break;
      default:
	lowerLimit = 0.90; break;
      }
    }
    else lowerLimit = 0.80;
    leg = new TLegend(0.55,lowerLimit,0.99,1.0);
    
  }
  else if (option==1) {
    cHistsCorrVsIter = new TCanvas("cHistsCorrVsIter","cHistsCorrVsIter",1150,700);
    cHistsCorrVsIter->Divide(3,2);
  }

  graphics = true;

  return;
}


//=====================================================================
// PlotConstants()
//=====================================================================
void PlotConstants(Int_t MarkerStyle, Float_t MarkerSize, 
		   Color_t MarkerColor, Char_t label[MaxPathSize]) {

  if (problem) { 

    if (PrintLevel<1) cout << " In PlotConstants()..." << endl;

    if (!graphics) initGraphics(0);

    Char_t name[MaxPathSize];

    TCut cutBarrel = "level==2 && bec==0";
    TCut cutEndCapA = "level==2 && bec==2";
    TCut cutEndCapC = "level==2 && bec==-2";

    cConstsBarrel->cd(1);
    ntuple->Draw("Tx:radius",cutBarrel,"goff");
    TGraph *hBarrelResTx = new TGraph(ntuple->GetSelectedRows(), ntuple->GetV2(), ntuple->GetV1());
    sprintf(name,"Barrel - Global #DeltaTx");
    hBarrelResTx->SetTitle(name);
    sprintf(name,"Global #Deltax (mm)");
    hBarrelResTx->GetYaxis()->SetTitle(name);
    hBarrelResTx->GetXaxis()->SetTitle("radius (mm)");
    if (doLevel2ForRef) hBarrelResTx->GetYaxis()->SetRangeUser(-0.1,1.4);
    else hBarrelResTx->GetYaxis()->SetRangeUser(-0.2,0.2);
    hBarrelResTx->GetYaxis()->SetLabelSize(0.025);
    hBarrelResTx->GetXaxis()->SetLabelSize(0.03);
    hBarrelResTx->SetMarkerStyle(MarkerStyle);
    hBarrelResTx->SetMarkerSize(MarkerSize);
    hBarrelResTx->SetMarkerColor(MarkerColor);
    if (!AtlasStyle) { gPad->SetGridx(); gPad->SetGridy(); }
    if (first) hBarrelResTx->Draw("AP");
    else hBarrelResTx->Draw("P");
    setTitleFont();
    drawHorizontalLine(hBarrelResTx);
    
    // Add legend
    leg->AddEntry(hBarrelResTx,label,"p"); 
    
    //
    // ----------------------------------------------------------------------------------------------
    //

    cConstsBarrel->cd(2);
    ntuple->Draw("Ty:radius",cutBarrel,"goff");
    TGraph *hBarrelResTy = new TGraph(ntuple->GetSelectedRows(), ntuple->GetV2(), ntuple->GetV1());
    sprintf(name,"Barrel - Global #DeltaTy");
    hBarrelResTy->SetTitle(name);
    sprintf(name,"Global #Deltay (mm)");
    hBarrelResTy->GetYaxis()->SetTitle(name);
    hBarrelResTy->GetXaxis()->SetTitle("radius (mm)");
    if (doLevel2ForRef) hBarrelResTy->GetYaxis()->SetRangeUser(-0.3,1.9);
    else hBarrelResTy->GetYaxis()->SetRangeUser(-0.2,0.2);
    hBarrelResTy->GetYaxis()->SetLabelSize(0.025);
    hBarrelResTy->GetXaxis()->SetLabelSize(0.03);
    hBarrelResTy->SetMarkerStyle(MarkerStyle);
    hBarrelResTy->SetMarkerSize(MarkerSize);
    hBarrelResTy->SetMarkerColor(MarkerColor);
    if (!AtlasStyle) { gPad->SetGridx(); gPad->SetGridy(); }
    if (first) hBarrelResTy->Draw("AP");
    else hBarrelResTy->Draw("P");
    setTitleFont();
    drawHorizontalLine(hBarrelResTy);

    //
    // ----------------------------------------------------------------------------------------------
    //

    cConstsBarrel->cd(3);
    ntuple->Draw("Tz:radius",cutBarrel,"goff");
    TGraph *hBarrelResTz = new TGraph(ntuple->GetSelectedRows(), ntuple->GetV2(), ntuple->GetV1());
    sprintf(name,"Barrel - Global #DeltaTz");
    hBarrelResTz->SetTitle(name);
    sprintf(name,"Global #Deltaz (mm)");
    hBarrelResTz->GetYaxis()->SetTitle(name);
    hBarrelResTz->GetXaxis()->SetTitle("radius (mm)");
    if (doLevel2ForRef) hBarrelResTz->GetYaxis()->SetRangeUser(-0.3,2.1);
    else hBarrelResTz->GetYaxis()->SetRangeUser(-0.1,0.1);
    hBarrelResTz->GetYaxis()->SetLabelSize(0.025);
    hBarrelResTz->GetXaxis()->SetLabelSize(0.03);
    hBarrelResTz->SetMarkerStyle(MarkerStyle);
    hBarrelResTz->SetMarkerSize(MarkerSize);
    hBarrelResTz->SetMarkerColor(MarkerColor);
    if (!AtlasStyle) { gPad->SetGridx(); gPad->SetGridy(); }
    if (first) hBarrelResTz->Draw("AP");
    else hBarrelResTz->Draw("P");
    setTitleFont();
    drawHorizontalLine(hBarrelResTz);

    //
    // ----------------------------------------------------------------------------------------------
    //

    cConstsBarrel->cd(4);
    ntuple->Draw("Rx:radius",cutBarrel,"goff");
    TGraph *hBarrelResRx = new TGraph(ntuple->GetSelectedRows(), ntuple->GetV2(), ntuple->GetV1());
    sprintf(name,"Barrel - Global #DeltaRx");
    hBarrelResRx->SetTitle(name);
    sprintf(name,"Global #Delta#alpha (mrad)");
    hBarrelResRx->GetYaxis()->SetTitle(name);
    hBarrelResRx->GetXaxis()->SetTitle("radius (mm)");
    if (doLevel2ForRef) hBarrelResRx->GetYaxis()->SetRangeUser(-2.2,0.8);
    else hBarrelResRx->GetYaxis()->SetRangeUser(-0.3,0.3);
    hBarrelResRx->GetYaxis()->SetLabelSize(0.025);
    hBarrelResRx->GetXaxis()->SetLabelSize(0.03);
    hBarrelResRx->SetMarkerStyle(MarkerStyle);
    hBarrelResRx->SetMarkerSize(MarkerSize);
    hBarrelResRx->SetMarkerColor(MarkerColor);
    if (!AtlasStyle) { gPad->SetGridx(); gPad->SetGridy(); }
    if (first) hBarrelResRx->Draw("AP");
    else hBarrelResRx->Draw("P");
    setTitleFont();
    drawHorizontalLine(hBarrelResRx);

    //
    // ----------------------------------------------------------------------------------------------
    //

    cConstsBarrel->cd(5);
    ntuple->Draw("Ry:radius",cutBarrel,"goff");
    TGraph *hBarrelResRy = new TGraph(ntuple->GetSelectedRows(), ntuple->GetV2(), ntuple->GetV1());
    sprintf(name,"Barrel - Global #DeltaRy");
    hBarrelResRy->SetTitle(name);
    sprintf(name,"Global #Delta#beta (mrad)");
    hBarrelResRy->GetYaxis()->SetTitle(name);
    hBarrelResRy->GetXaxis()->SetTitle("radius (mm)");
    if (doLevel2ForRef) hBarrelResRy->GetYaxis()->SetRangeUser(-1.5,1.5);
    else hBarrelResRy->GetYaxis()->SetRangeUser(-0.5,0.5);
    hBarrelResRy->GetYaxis()->SetLabelSize(0.025);
    hBarrelResRy->GetXaxis()->SetLabelSize(0.03);
    hBarrelResRy->SetMarkerStyle(MarkerStyle);
    hBarrelResRy->SetMarkerSize(MarkerSize);
    hBarrelResRy->SetMarkerColor(MarkerColor);
    if (!AtlasStyle) { gPad->SetGridx(); gPad->SetGridy(); }
    if (first) hBarrelResRy->Draw("AP");
    else hBarrelResRy->Draw("P");
    setTitleFont();
    drawHorizontalLine(hBarrelResRy);

    //
    // ----------------------------------------------------------------------------------------------
    //

    cConstsBarrel->cd(6);
    ntuple->Draw("Rz:radius",cutBarrel,"goff");
    TGraph *hBarrelResRz = new TGraph(ntuple->GetSelectedRows(), ntuple->GetV2(), ntuple->GetV1());
    sprintf(name,"Barrel - Global #DeltaRz");
    hBarrelResRz->SetTitle(name);
    sprintf(name,"Global #Delta#gamma (mrad)");
    hBarrelResRz->GetYaxis()->SetTitle(name);
    hBarrelResRz->GetXaxis()->SetTitle("radius (mm)");
    if (doLevel2ForRef) hBarrelResRz->GetYaxis()->SetRangeUser(-2.0,2.8);
    else hBarrelResRz->GetYaxis()->SetRangeUser(-1.5,2.0);
    hBarrelResRz->GetYaxis()->SetLabelSize(0.025);
    hBarrelResRz->GetXaxis()->SetLabelSize(0.03);
    hBarrelResRz->SetMarkerStyle(MarkerStyle);
    hBarrelResRz->SetMarkerSize(MarkerSize);
    hBarrelResRz->SetMarkerColor(MarkerColor);
    if (!AtlasStyle) { gPad->SetGridx(); gPad->SetGridy(); }
    if (first) hBarrelResRz->Draw("AP");
    else hBarrelResRz->Draw("P");
    setTitleFont();
    drawHorizontalLine(hBarrelResRz);

    //
    // ----------------------------------------------------------------------------------------------
    //

    if (!BarrelOnly) {
  
      cConstsEndCapA->cd(1);
      ntuple->Draw("Tx:radius",cutEndCapA,"goff");
      TGraph *hEndCapAResTx = new TGraph(ntuple->GetSelectedRows(), ntuple->GetV2(), ntuple->GetV1());
      sprintf(name,"EndCap A - Global #DeltaTx");
      hEndCapAResTx->SetTitle(name);
      sprintf(name,"Global #Deltax (mm)");
      hEndCapAResTx->GetYaxis()->SetTitle(name);
      hEndCapAResTx->GetXaxis()->SetTitle("Z (mm)");
      hEndCapAResTx->GetYaxis()->SetRangeUser(-3.5,3.5);
      hEndCapAResTx->GetYaxis()->SetLabelSize(0.025);
      hEndCapAResTx->GetXaxis()->SetLabelSize(0.03);
      hEndCapAResTx->SetMarkerStyle(MarkerStyle);
      hEndCapAResTx->SetMarkerSize(MarkerSize);
      hEndCapAResTx->SetMarkerColor(MarkerColor);
      if (!AtlasStyle) {
	gPad->SetGridx();
	gPad->SetGridy();
      }
      if (first) hEndCapAResTx->Draw("AP");
      else hEndCapAResTx->Draw("P");
      setTitleFont();
      drawHorizontalLine(hEndCapAResTx);

      cConstsEndCapC->cd(1);  
      ntuple->Draw("Tx:(-radius)",cutEndCapC,"goff");
      TGraph *hEndCapCResTx = new TGraph(ntuple->GetSelectedRows(), ntuple->GetV2(), ntuple->GetV1());
      sprintf(name,"EndCap C - Global #DeltaTx");
      hEndCapCResTx->SetTitle(name);
      sprintf(name,"Global #Deltax (mm)");
      hEndCapCResTx->GetYaxis()->SetTitle(name);
      hEndCapCResTx->GetXaxis()->SetTitle("-Z (mm)");
      hEndCapCResTx->GetYaxis()->SetRangeUser(-3.5,3.5);
      hEndCapCResTx->GetYaxis()->SetLabelSize(0.025);
      hEndCapCResTx->GetXaxis()->SetLabelSize(0.03);
      hEndCapCResTx->SetMarkerStyle(MarkerStyle);
      hEndCapCResTx->SetMarkerSize(MarkerSize);
      hEndCapCResTx->SetMarkerColor(MarkerColor);
      if (!AtlasStyle) {
	gPad->SetGridx();
	gPad->SetGridy();
      }
      if (first) hEndCapCResTx->Draw("AP");
      else hEndCapCResTx->Draw("P");
      setTitleFont();
      drawHorizontalLine(hEndCapCResTx);

      //
      // ----------------------------------------------------------------------------------------------
      //

      cConstsEndCapA->cd(2);
      ntuple->Draw("Ty:radius",cutEndCapA,"goff");
      TGraph *hEndCapAResTy = new TGraph(ntuple->GetSelectedRows(), ntuple->GetV2(), ntuple->GetV1());
      sprintf(name,"EndCap A - Global #DeltaTy");
      hEndCapAResTy->SetTitle(name);
      sprintf(name,"Global #Deltay (mm)");
      hEndCapAResTy->GetYaxis()->SetTitle(name);
      hEndCapAResTy->GetXaxis()->SetTitle("Z (mm)");
      hEndCapAResTy->GetYaxis()->SetRangeUser(-3.5,3.5);
      hEndCapAResTy->GetYaxis()->SetLabelSize(0.025);
      hEndCapAResTy->GetXaxis()->SetLabelSize(0.03);
      hEndCapAResTy->SetMarkerStyle(MarkerStyle);
      hEndCapAResTy->SetMarkerSize(MarkerSize);
      hEndCapAResTy->SetMarkerColor(MarkerColor);
      if (!AtlasStyle) {
	gPad->SetGridx();
	gPad->SetGridy();
      }
      if (first) hEndCapAResTy->Draw("AP");
      else hEndCapAResTy->Draw("P");
      setTitleFont();
      drawHorizontalLine(hEndCapAResTy);

      cConstsEndCapC->cd(2);  
      ntuple->Draw("Ty:(-radius)",cutEndCapC,"goff");
      TGraph *hEndCapCResTy = new TGraph(ntuple->GetSelectedRows(), ntuple->GetV2(), ntuple->GetV1());
      sprintf(name,"EndCap C - Global #DeltaTy");
      hEndCapCResTy->SetTitle(name);
      sprintf(name,"Global #DeltaTy (mm)");
      hEndCapCResTy->GetYaxis()->SetTitle(name);
      hEndCapCResTy->GetXaxis()->SetTitle("-Z (mm)");
      hEndCapCResTy->GetYaxis()->SetRangeUser(-3.5,3.5);
      hEndCapCResTy->GetYaxis()->SetLabelSize(0.025);
      hEndCapCResTy->GetXaxis()->SetLabelSize(0.03);
      hEndCapCResTy->SetMarkerStyle(MarkerStyle);
      hEndCapCResTy->SetMarkerSize(MarkerSize);
      hEndCapCResTy->SetMarkerColor(MarkerColor);
      if (!AtlasStyle) {
	gPad->SetGridx();
	gPad->SetGridy();
      }
      if (first) hEndCapCResTy->Draw("AP");
      else hEndCapCResTy->Draw("P");
      setTitleFont();
      drawHorizontalLine(hEndCapCResTy);

      //
      // ----------------------------------------------------------------------------------------------
      //


      cConstsEndCapA->cd(3);
      ntuple->Draw("Tz:radius",cutEndCapA,"goff");
      TGraph *hEndCapAResTz = new TGraph(ntuple->GetSelectedRows(), ntuple->GetV2(), ntuple->GetV1());
      sprintf(name,"EndCap A - Global #DeltaTz");
      hEndCapAResTz->SetTitle(name);
      sprintf(name,"Global #Deltaz (mm)");
      hEndCapAResTz->GetYaxis()->SetTitle(name);
      hEndCapAResTz->GetXaxis()->SetTitle("Z (mm)");
      hEndCapAResTz->GetYaxis()->SetRangeUser(-4.1,4.1);
      hEndCapAResTz->GetYaxis()->SetLabelSize(0.025);
      hEndCapAResTz->GetXaxis()->SetLabelSize(0.03);
      hEndCapAResTz->SetMarkerStyle(MarkerStyle);
      hEndCapAResTz->SetMarkerSize(MarkerSize);
      hEndCapAResTz->SetMarkerColor(MarkerColor);
      if (!AtlasStyle) {
	gPad->SetGridx();
	gPad->SetGridy();
      }
      if (first) hEndCapAResTz->Draw("AP");
      else hEndCapAResTz->Draw("P");
      setTitleFont();
      drawHorizontalLine(hEndCapAResTz);

      cConstsEndCapC->cd(3);  
      ntuple->Draw("Tz:(-radius)",cutEndCapC,"goff");
      TGraph *hEndCapCResTz = new TGraph(ntuple->GetSelectedRows(), ntuple->GetV2(), ntuple->GetV1());
      sprintf(name,"EndCap C - Global #DeltaTz");
      hEndCapCResTz->SetTitle(name);
      sprintf(name,"Global #Deltaz (mm)");
      hEndCapCResTz->GetYaxis()->SetTitle(name);
      hEndCapCResTz->GetXaxis()->SetTitle("-Z (mm)");
      hEndCapCResTz->GetYaxis()->SetRangeUser(-5.1,3.1);
      hEndCapCResTz->GetYaxis()->SetLabelSize(0.025);
      hEndCapCResTz->GetXaxis()->SetLabelSize(0.03);
      hEndCapCResTz->SetMarkerStyle(MarkerStyle);
      hEndCapCResTz->SetMarkerSize(MarkerSize);
      hEndCapCResTz->SetMarkerColor(MarkerColor);
      if (!AtlasStyle) {
	gPad->SetGridx();
	gPad->SetGridy();
      }
      if (first) hEndCapCResTz->Draw("AP");
      else hEndCapCResTz->Draw("P");
      setTitleFont();
      drawHorizontalLine(hEndCapCResTz);

      //
      // ----------------------------------------------------------------------------------------------
      //

      cConstsEndCapA->cd(4);
      ntuple->Draw("Rx:radius",cutEndCapA,"goff");
      TGraph *hEndCapAResRx = new TGraph(ntuple->GetSelectedRows(), ntuple->GetV2(), ntuple->GetV1());
      sprintf(name,"EndCap A - Global #DeltaRx");
      hEndCapAResRx->SetTitle(name);
      sprintf(name,"Global #Delta#alpha (mrad)");
      hEndCapAResRx->GetYaxis()->SetTitle(name);
      hEndCapAResRx->GetXaxis()->SetTitle("Z (mm)");
      hEndCapAResRx->GetYaxis()->SetRangeUser(-1.5,1.5);
      hEndCapAResRx->GetYaxis()->SetLabelSize(0.025);
      hEndCapAResRx->GetXaxis()->SetLabelSize(0.03);
      hEndCapAResRx->SetMarkerStyle(MarkerStyle);
      hEndCapAResRx->SetMarkerSize(MarkerSize);
      hEndCapAResRx->SetMarkerColor(MarkerColor);
      if (!AtlasStyle) {
	gPad->SetGridx();
	gPad->SetGridy();
      }
      if (first) hEndCapAResRx->Draw("AP");
      else hEndCapAResRx->Draw("P");
      setTitleFont();
      drawHorizontalLine(hEndCapAResRx);

      cConstsEndCapC->cd(4);  
      ntuple->Draw("Rx:(-radius)",cutEndCapC,"goff");
      TGraph *hEndCapCResRx = new TGraph(ntuple->GetSelectedRows(), ntuple->GetV2(), ntuple->GetV1());
      sprintf(name,"EndCap C - Global #DeltaRx");
      hEndCapCResRx->SetTitle(name);
      sprintf(name,"Global #Delta#alpha (mrad)");
      hEndCapCResRx->GetYaxis()->SetTitle(name);
      hEndCapCResRx->GetXaxis()->SetTitle("-Z (mm)");
      hEndCapCResRx->GetYaxis()->SetRangeUser(-1.5,1.5);
      hEndCapCResRx->GetYaxis()->SetLabelSize(0.025);
      hEndCapCResRx->GetXaxis()->SetLabelSize(0.03);
      hEndCapCResRx->SetMarkerStyle(MarkerStyle);
      hEndCapCResRx->SetMarkerSize(MarkerSize);
      hEndCapCResRx->SetMarkerColor(MarkerColor);
      if (!AtlasStyle) {
	gPad->SetGridx();
	gPad->SetGridy();
      }
      if (first) hEndCapCResRx->Draw("AP");
      else hEndCapCResRx->Draw("P");
      setTitleFont();
      drawHorizontalLine(hEndCapCResRx);

      //
      // ----------------------------------------------------------------------------------------------
      //

      cConstsEndCapA->cd(5);
      ntuple->Draw("Ry:radius",cutEndCapA,"goff");
      TGraph *hEndCapAResRy = new TGraph(ntuple->GetSelectedRows(), ntuple->GetV2(), ntuple->GetV1());
      sprintf(name,"EndCap A - Global #DeltaRy");
      hEndCapAResRy->SetTitle(name);
      sprintf(name,"Global #Delta#beta (mrad)");
      hEndCapAResRy->GetYaxis()->SetTitle(name);
      hEndCapAResRy->GetXaxis()->SetTitle("Z (mm)");
      hEndCapAResRy->GetYaxis()->SetRangeUser(-2.0,2.0);
      hEndCapAResRy->GetYaxis()->SetLabelSize(0.025);
      hEndCapAResRy->GetXaxis()->SetLabelSize(0.03);
      hEndCapAResRy->SetMarkerStyle(MarkerStyle);
      hEndCapAResRy->SetMarkerSize(MarkerSize);
      hEndCapAResRy->SetMarkerColor(MarkerColor);
      if (!AtlasStyle) {
	gPad->SetGridx();
	gPad->SetGridy();
      }
      if (first) hEndCapAResRy->Draw("AP");
      else hEndCapAResRy->Draw("P");
      setTitleFont();
      drawHorizontalLine(hEndCapAResRy);

      cConstsEndCapC->cd(5);  
      ntuple->Draw("Ry:(-radius)",cutEndCapC,"goff");
      TGraph *hEndCapCResRy = new TGraph(ntuple->GetSelectedRows(), ntuple->GetV2(), ntuple->GetV1());
      sprintf(name,"EndCap C - Global #DeltaRy");
      hEndCapCResRy->SetTitle(name);
      sprintf(name,"Global #Delta#beta (mrad)");
      hEndCapCResRy->GetYaxis()->SetTitle(name);
      hEndCapCResRy->GetXaxis()->SetTitle("-Z (mm)");
      hEndCapCResRy->GetYaxis()->SetRangeUser(-2.0,2.0);
      hEndCapCResRy->GetYaxis()->SetLabelSize(0.025);
      hEndCapCResRy->GetXaxis()->SetLabelSize(0.03);
      hEndCapCResRy->SetMarkerStyle(MarkerStyle);
      hEndCapCResRy->SetMarkerSize(MarkerSize);
      hEndCapCResRy->SetMarkerColor(MarkerColor);
      if (!AtlasStyle) {
	gPad->SetGridx();
	gPad->SetGridy();
      }
      if (first) hEndCapCResRy->Draw("AP");
      else hEndCapCResRy->Draw("P");
      setTitleFont();
      drawHorizontalLine(hEndCapCResRy);

      //
      // ----------------------------------------------------------------------------------------------
      //

      cConstsEndCapA->cd(6);
      ntuple->Draw("Rz:radius",cutEndCapA,"goff");
      TGraph *hEndCapAResRz = new TGraph(ntuple->GetSelectedRows(), ntuple->GetV2(), ntuple->GetV1());
      sprintf(name,"EndCap A - Global #DeltaRz");
      hEndCapAResRz->SetTitle(name);
      sprintf(name,"Global #Delta#gamma (mrad)");
      hEndCapAResRz->GetYaxis()->SetTitle(name);
      hEndCapAResRz->GetXaxis()->SetTitle("Z (mm)");
      hEndCapAResRz->GetYaxis()->SetRangeUser(-2.5,2.5);
      hEndCapAResRz->GetYaxis()->SetLabelSize(0.025);
      hEndCapAResRz->GetXaxis()->SetLabelSize(0.03);
      hEndCapAResRz->SetMarkerStyle(MarkerStyle);
      hEndCapAResRz->SetMarkerSize(MarkerSize);
      hEndCapAResRz->SetMarkerColor(MarkerColor);
      if (!AtlasStyle) {
	gPad->SetGridx();
	gPad->SetGridy();
      }
      if (first) hEndCapAResRz->Draw("AP");
      else hEndCapAResRz->Draw("P");
      setTitleFont();
      drawHorizontalLine(hEndCapAResRz);

      cConstsEndCapC->cd(6);  
      ntuple->Draw("Rz:(-radius)",cutEndCapC,"goff");
      TGraph *hEndCapCResRz = new TGraph(ntuple->GetSelectedRows(), ntuple->GetV2(), ntuple->GetV1());
      sprintf(name,"EndCap C - Global #DeltaRz");
      hEndCapCResRz->SetTitle(name);
      sprintf(name,"Global #Delta#gamma (mrad)");
      hEndCapCResRz->GetYaxis()->SetTitle(name);
      hEndCapCResRz->GetXaxis()->SetTitle("-Z (mm)");
      hEndCapCResRz->GetYaxis()->SetRangeUser(-2.5,2.5);
      hEndCapCResRz->GetYaxis()->SetLabelSize(0.025);
      hEndCapCResRz->GetXaxis()->SetLabelSize(0.03);
      hEndCapCResRz->SetMarkerStyle(MarkerStyle);
      hEndCapCResRz->SetMarkerSize(MarkerSize);
      hEndCapCResRz->SetMarkerColor(MarkerColor);
      if (!AtlasStyle) {
	gPad->SetGridx();
	gPad->SetGridy();
      }
      if (first) hEndCapCResRz->Draw("AP");
      else hEndCapCResRz->Draw("P");
      setTitleFont();
      drawHorizontalLine(hEndCapCResRz);
    }

    ntuple->Reset();

    first = false;
    counter++;
  }

  return; 
}


//=====================================================================
// PrintLegend()
//=====================================================================
void PrintLegend() {

  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);
  leg->SetTextFont(42);
  
  for (int i=0; i<6; i++) {
    if (cConstsBarrel) {
      cConstsBarrel->cd(i+1);
      leg->Draw();
    }
    if(cConstsEndCapA) {
      cConstsEndCapA->cd(i+1);
      leg->Draw();
    }
    if(cConstsEndCapC) {
      cConstsEndCapC->cd(i+1);
      leg->Draw();
    }
  }

  return;
}


//=====================================================================
// read()
//====================================================================
// read data from an ascii file
Bool_t read(Char_t filename[MaxPathSize]) {
  if (PrintLevel<1) cout << " In read()..." << endl;

  bool doLevel2 = true;

  if (!graphics) {
    cout << endl;
    isReference = 1;
    doLevel2 = doLevel2ForRef;
  }
  else {
    isReference = 0;
    doLevel2 = true;
  }
  
  if (checkFile(filename)==1) {
    cout << " - ERROR:: " << filename << " doesn't exist!" << endl;
    good = false;
    return good;
  }
  else {

    cout << "- Reading constants from " << filename << " file..." << endl;

    // file exist!
    good = true;

    readRadius("reference/structures.txt");

    ifstream in(filename);
  
    Int_t Extra, PixSCT, BarrelEC, Layer, Eta, Phi, Side;
    Float_t Tx, Ty, Tz, Rx, Ry, Rz;

    Int_t nlines = 0;

    Char_t str[MaxPathSize];

    Float_t PixL1[6]={0.0};
    Float_t SCTL1[6][3]={{0.0},{0.0}};

    for (int i=0;i<31;i++) {
      modsLy[i] = 0;
    }
  
    while (1) {
     
      Char_t c;
      in >> c;
      if (!in.good()) break;
      in.putback (c);
      c = in.peek();
      // cout << "Peek in " << c << endl;

      Bool_t SuperModule = false;
      Int_t Level = 0;
      Float_t radius = 0.0;

      if ( (c >= '0') && (c <= '9') ) {

	in >> Extra >> PixSCT >> BarrelEC >> Layer >> Eta >> Phi >> Side >> Tx >> Ty >> Tz >> Rx >> Ry >> Rz;
      
	// ----------------------------------------------------------------------------------
	//
	//         -      Level 1     -
	//
	// ----------------------------------------------------------------------------------
	if(!strcmp(str,"/Indet/Align/ID")) {
	  if(PixSCT==1) {
	    PixL1[0] = Tx; PixL1[1] = Ty; PixL1[2] = Tz;
	    PixL1[3] = Rx; PixL1[4] = Ry; PixL1[5] = Rz;
	  }
	  if(PixSCT==2) {
	    if(BarrelEC==-2) { 
	      SCTL1[0][0] = Tx; SCTL1[1][0] = Ty; SCTL1[2][0] = Tz;
	      SCTL1[3][0] = Rx; SCTL1[4][0] = Ry; SCTL1[5][0] = Rz;
	    }
	    else if(BarrelEC== 0) { 
	      SCTL1[0][1] = Tx; SCTL1[1][1] = Ty; SCTL1[2][1] = Tz;
	      SCTL1[3][1] = Rx; SCTL1[4][1] = Ry; SCTL1[5][1] = Rz;
	    }
	    else if(BarrelEC== 2) {
	      SCTL1[0][2] = Tx; SCTL1[1][2] = Ty; SCTL1[2][2] = Tz;
	      SCTL1[3][2] = Rx; SCTL1[4][2] = Ry; SCTL1[5][2] = Rz;
	    }
	  }
	  if (PrintLevel<1) {
	    cout << " - Level 1: " << " DetType: " << PixSCT
		 << " - BEc " << BarrelEC << "  (" << Tx <<  "," << Ty <<  "," << Tz
		 << "," << Rx <<  "," << Ry <<  "," << Rz << ")"
		 << endl;
	  }

	  SuperModule = true;
	  Level = 1;
	}

	// ----------------------------------------------------------------------------------
	//
	//         -      Level 2     -
	//
	// ----------------------------------------------------------------------------------
	if(!strcmp(str,"/Indet/Align/PIX") || !strcmp(str,"/Indet/Align/SCT")) {
	  if (PrintLevel<1) {
	    cout << " - Level 2: " << " DetType: " << PixSCT
		 << " - BEc " << BarrelEC << " - Layer " << Layer <<"  (" << Tx <<  "," << Ty <<  "," << Tz 
		 << "," << Rx <<  "," << Ry <<  "," << Rz << ")"
		 << endl;
	  }
	  if (PixSCT==1) {

	    if (doLevel2) {
	      Tx += PixL1[0]; Ty += PixL1[1]; Tz += PixL1[2];
	      Rx += PixL1[3]; Ry += PixL1[4]; Rz += PixL1[5];
	    }

	    if (PrintLevel<1) {
	      cout << " Level 1+2: " << " DetType: " << PixSCT
		   << " - BEc " << BarrelEC << "  (" << Tx <<  "," << Ty <<  "," << Tz 
		   << "," << Rx <<  "," << Ry <<  "," << Rz << ")"
		   << endl;
	    }
	  }
	  else {
	    if(BarrelEC== -2) {
	      if (doLevel2) {
		Tx += SCTL1[0][0]; Ty += SCTL1[1][0]; Tz += SCTL1[2][0];
		Rx += SCTL1[3][0]; Ry += SCTL1[4][0]; Rz += SCTL1[5][0];
	      }
	    }
	    else if(BarrelEC== 0) {
	      if (doLevel2) {
		Tx += SCTL1[0][1]; Ty += SCTL1[1][1]; Tz += SCTL1[2][1];
		Rx += SCTL1[3][1]; Ry += SCTL1[4][1]; Rz += SCTL1[5][1];
	      }
	    }
	    else if(BarrelEC== 2) {
	      if (doLevel2) {
		Tx += SCTL1[0][2]; Ty += SCTL1[1][2]; Tz += SCTL1[2][2];
		Rx += SCTL1[3][2]; Ry += SCTL1[4][2]; Rz += SCTL1[5][2];
	      }
	    }
	    if (PrintLevel<1) {
	      cout << " --> Level 1 + Level 2: " << " DetType: " << PixSCT
		   << " - BEc " << BarrelEC << "   (" << Tx <<  "," << Ty <<  "," << Tz 
		   << "," << Rx <<  "," << Ry <<  "," << Rz << ")"
		   << endl;
	    }
	  }

	  SuperModule = true;
	  Level = 2;
	  radius = GetRadius(PixSCT,BarrelEC,Layer);
	}

	// ----------------------------------------------------------------------------------
	//
	//         -      Level 3     -
	//
	// ----------------------------------------------------------------------------------
	// Pixels
	if(!strcmp(str,"/Indet/Align/PIXEC1")) modsLy[1]++;
	if(!strcmp(str,"/Indet/Align/PIXEC2")) modsLy[2]++;
	if(!strcmp(str,"/Indet/Align/PIXEC3")) modsLy[3]++;

	if(!strcmp(str,"/Indet/Align/PIXB1"))  modsLy[4]++;
	if(!strcmp(str,"/Indet/Align/PIXB2"))  modsLy[5]++;
	if(!strcmp(str,"/Indet/Align/PIXB3"))  modsLy[6]++;

	if(!strcmp(str,"/Indet/Align/PIXEA1")) modsLy[7]++;
	if(!strcmp(str,"/Indet/Align/PIXEA2")) modsLy[8]++;
	if(!strcmp(str,"/Indet/Align/PIXEA3")) modsLy[9]++;
      
	// SCT
	if(!strcmp(str,"/Indet/Align/SCTEC1")) modsLy[10]++;
	if(!strcmp(str,"/Indet/Align/SCTEC2")) modsLy[11]++;
	if(!strcmp(str,"/Indet/Align/SCTEC3")) modsLy[12]++;
	if(!strcmp(str,"/Indet/Align/SCTEC4")) modsLy[13]++;
	if(!strcmp(str,"/Indet/Align/SCTEC5")) modsLy[14]++;
	if(!strcmp(str,"/Indet/Align/SCTEC6")) modsLy[15]++;
	if(!strcmp(str,"/Indet/Align/SCTEC7")) modsLy[16]++;
	if(!strcmp(str,"/Indet/Align/SCTEC8")) modsLy[17]++;
	if(!strcmp(str,"/Indet/Align/SCTEC9")) modsLy[18]++;

	if(!strcmp(str,"/Indet/Align/SCTB1"))  modsLy[19]++;
	if(!strcmp(str,"/Indet/Align/SCTB2"))  modsLy[20]++;
	if(!strcmp(str,"/Indet/Align/SCTB3"))  modsLy[21]++;
	if(!strcmp(str,"/Indet/Align/SCTB4"))  modsLy[22]++;

	if(!strcmp(str,"/Indet/Align/SCTEA1")) modsLy[23]++;
	if(!strcmp(str,"/Indet/Align/SCTEA2")) modsLy[24]++;
	if(!strcmp(str,"/Indet/Align/SCTEA3")) modsLy[25]++;
	if(!strcmp(str,"/Indet/Align/SCTEA4")) modsLy[26]++;
	if(!strcmp(str,"/Indet/Align/SCTEA5")) modsLy[27]++;
	if(!strcmp(str,"/Indet/Align/SCTEA6")) modsLy[28]++;
	if(!strcmp(str,"/Indet/Align/SCTEA7")) modsLy[29]++;
	if(!strcmp(str,"/Indet/Align/SCTEA8")) modsLy[30]++;
	if(!strcmp(str,"/Indet/Align/SCTEA9")) modsLy[31]++;

	if (!in.good()) break;
	if (SuperModule) {
	  ntuple->Fill(Level, BarrelEC, radius, Tx, Ty, Tz, Rx, Ry, Rz);
	  ntuple2->Fill(Iteration, Level, PixSCT, BarrelEC, Layer, Phi, Eta, Tx, Ty, Tz, Rx, Ry, Rz, isReference);
	}

	nlines++;

      }
      else {
	in >> str;
	if(!strcmp(str,"/Indet/Align/ID") || !strcmp(str,"/Indet/Align/PIX") || !strcmp(str,"/Indet/Align/SCT"))
	  if (PrintLevel<1) cout << " reading block " << str << endl;
	if (!in.good()) break;
      }
    }
    cout << " Found " << nlines << " rows of floats" << endl;

    if (PrintLevel<2) {
      ntuple->Scan("radius:Tx:Ty:Tz:Rx:Ry:Rz","level==2 && bec==0");
      ntuple->Scan("radius:Tx:Ty:Tz:Rx:Ry:Rz","level==2 && bec==-2");
      ntuple->Scan("radius:Tx:Ty:Tz:Rx:Ry:Rz","level==2 && bec==2");
    }

    in.close();

    Iteration++;
   
  }

  cout << "file read successfully!" << endl;

  return good;
}


//=====================================================================
// readRadius()
//=====================================================================
void readRadius(Char_t filename[MaxPathSize]) {
  if (PrintLevel<1) cout << " In readRadius()..." << endl;

  if (PrintLevel<2)
    cout << "Reading radius from the reference file: structures.txt" << endl;

  ifstream in(filename);

  int i=0;
  while (1) {
    if (!in.good()) break;

    Float_t cogX, cogY, cogZ;
    in >> mcode[i] >> modsLy[i] >> radiusLy[i] >> cogX >> cogY >> cogZ;

    cog[i].SetXYZ(cogX,cogY,cogZ);

    i++;
  }

  if (PrintLevel<2) {
    for (i=0; i<31; i++) {
      printf("%12i  %5i  %5.3f  (%3.3f,%3.3f,%3.3f)\n",mcode[i],modsLy[i],radiusLy[i],
	     cog[i].X(),cog[i].Y(),cog[i].Z());
    }
  }

  return;
}


//=====================================================================
// GetRadius()
//=====================================================================
Float_t GetRadius(Int_t PixSCT, Int_t BarrelEC, Int_t Layer) {
  if (PrintLevel<1) cout << " In GetRadius()..." << endl;

  Int_t iLayer = 0;
  
  if (PixSCT==1) {
    if (BarrelEC==-2) iLayer = Layer;
    else if (BarrelEC==0) iLayer = Layer + 3;
    else iLayer = Layer + 6;
  }
  else {
    if (BarrelEC==-2) iLayer = Layer + 9;
    else if (BarrelEC==0) iLayer = Layer + 18;
    else iLayer = Layer + 22;
  }
  
  Float_t radius=0.0;
  if (BarrelEC==0) radius = radiusLy[iLayer];
  else radius = cog[iLayer].Z();

  return radius;
}


//=====================================================================
// PlotHists()
//=====================================================================
void PlotHists() {
  cout << "In PlotHists()" << endl;

  if (!cHistsCorrVsIter) initGraphics(1);

  Int_t setPixSCT = 2;
  Int_t setBEc = -2;
  Int_t setLayer = 0;

  Char_t CorrName[MaxPathSize];
  Char_t UnitsName[MaxPathSize];
  Char_t m_histName[MaxPathSize];
  Char_t yAxisName[MaxPathSize];
  Char_t TitleName[MaxPathSize];
  Char_t m_extra[MaxPathSize];
  Char_t PixSCTName[MaxPathSize];
  Char_t BEcName[MaxPathSize];
  Char_t LayerName[MaxPathSize];

  sprintf(m_extra,"PixSCT==%d",setPixSCT);
  TCut cutPixSCT = m_extra;
  if (setPixSCT==1) sprintf(PixSCTName,"Pixel");
  else if (setPixSCT==2) sprintf(PixSCTName,"SCT");

  sprintf(m_extra,"BEc==%d",setBEc);
  TCut cutBEc = m_extra;
  if (setBEc==0) sprintf(BEcName,"Barrel");
  else if (setBEc==-2) sprintf(BEcName,"EndCapC");
  else if (setBEc==2) sprintf(BEcName,"EndCapA");
  if (setBEc==0) sprintf(LayerName,"Layer");
  else sprintf(LayerName,"Disk");

  sprintf(m_extra,"layer==%d",setLayer);
  TCut cutLayer = m_extra;

  TCut cutLevel = "level==2";
  TCut cutIter = "iter>-2";

  for (int j=0; j<6; j++) {

    switch(j) {
    case 0:
      sprintf(CorrName,"Tx");
      sprintf(UnitsName,"mm");
      break;
    case 1:
      sprintf(CorrName,"Ty");
      sprintf(UnitsName,"mm");
      break;
    case 2:
      sprintf(CorrName,"Tz");
      sprintf(UnitsName,"mm");
      break;
    case 3:
      sprintf(CorrName,"Rx");
      sprintf(UnitsName,"mrad");
      break;
    case 4:
      sprintf(CorrName,"Ry");
      sprintf(UnitsName,"mrad");
      break;
    case 5:
      sprintf(CorrName,"Rz");
      sprintf(UnitsName,"mrad");
      break;
    }
 
    sprintf(m_histName,"%s:iter>>h%sCorrEvol(%d,0,%d)",
	    CorrName,CorrName,NumberOfIters+1,NumberOfIters+1);
    sprintf(TitleName,"Global #Delta%s - %s %s (%s %d)",
	    CorrName, PixSCTName, BEcName, LayerName, setLayer);
    sprintf(yAxisName,"Global #Delta%s (%s)",CorrName,UnitsName);

    cHistsCorrVsIter->cd(j+1);
    TCut cutFinal = cutPixSCT+cutBEc+cutLayer+cutLevel+cutIter;
    ntuple2->Draw(m_histName,cutFinal,"goff");
    TGraph *HD = new TGraph(ntuple2->GetSelectedRows(), ntuple2->GetV2(), ntuple2->GetV1());
    HD = new TGraph(ntuple2->GetSelectedRows(), ntuple2->GetV2(), ntuple2->GetV1());
    HD->SetTitle(TitleName);
    HD->GetXaxis()->SetTitle("Iteration Number");
    HD->GetYaxis()->SetTitle(yAxisName);
    HD->GetYaxis()->SetLabelSize(0.03);
    HD->GetXaxis()->SetLabelSize(0.03);
    HD->SetMarkerStyle(7);
    HD->SetMarkerSize(0.5);
    HD->SetMarkerColor(kBlue);
    HD->SetLineColor(kBlue);
    if (!AtlasStyle) {
      gPad->SetGridx();
      gPad->SetGridy();
    }
    //    HD->GetXaxis()->Set(8,-1,7);
    //     sprintf(m_extra,"h%sCorrEvol",CorrName);
    //     TH2F *histoTH2F = (TH2F*)gDirectory->FindObject(m_extra);
    //     SetXAxisLabels(HD, GetNumberOfBinsX(histoTH2F));
    HD->Draw("APL");
    setTitleFont();
    drawHorizontalLine(HD);
  }

  return;
}


//=====================================================================
// SetStyle()
//=====================================================================
void SetStyle() {

  if (AtlasStyle) {
    gROOT->SetStyle("ATLAS");
    
    // Extra options to the ATLAS Style
    Float_t jsize = 0.03;
    
    gStyle->SetLabelSize(jsize,"x");
    gStyle->SetLabelSize(jsize,"y");
    gStyle->SetLabelSize(jsize-0.01,"z");
    
    gStyle->SetTitleSize(jsize,"x");  
    gStyle->SetTitleSize(jsize,"y");
    gStyle->SetTitleSize(jsize,"z");
    gStyle->SetTitleYOffset(1.3);
    
    gStyle->SetTextFont(42);
    
    gStyle->SetStripDecimals(false);    
    TGaxis::SetMaxDigits(4);
    gStyle->SetPalette(1);
    
    // gStyle->SetPadLeftMargin(0.4);
    
  }
  else
    gROOT->ProcessLine(".x ../../monitor/include/style.cc");   

  gROOT->ForceStyle();
  return;
}


//=====================================================================
//  setTitleFont()
//=====================================================================
void setTitleFont() {
  if (!AtlasStyle) {
    gPad->Update();
    TPaveText *pt = (TPaveText*)(gPad->GetPrimitive("title"));
    pt->SetTextFont(font);
    pt->SetTextSize(0.04);
    gPad->Modified();
  }
  return;
}


//=====================================================================
//  drawHorizontalLine()
//=====================================================================
void drawHorizontalLine(TGraph* hgraf) {
  // Draw horizontal Line around y=0.0
  drawHorizontalLine(hgraf,0.0);
  return;
}


//=====================================================================
//  drawHorizontalLine()
//=====================================================================
void drawHorizontalLine(TGraph* hgraf, Double_t y) {
  double x1 = hgraf->GetXaxis()->GetXmin();
  double x2 = hgraf->GetXaxis()->GetXmax();
  TLine *l0x = new TLine(x1,y,x2,y);
  l0x->SetLineColor(kRed);
  l0x->SetLineStyle(2);
  l0x->Draw();
  return;
}


//=====================================================================
//  SetXAxisLabels()
//=====================================================================
void SetXAxisLabels(TGraph *histo, Int_t Nbins) {
  // cout << " <TGraph> Setting labels for histogram " << histo->GetTitle() << endl;
  
  // Int_t Nbins = histo->GetXaxis()->GetNbins();
  //  cout << Nbins << endl;

  Char_t m_name[MaxPathSize];

  for (int i=1; i<=Nbins; i++) {
    if (i==1) histo->GetXaxis()->SetBinLabel(i,"Nom");
    else {
      sprintf(m_name,"%d",i-2);
      histo->GetXaxis()->SetBinLabel(i,m_name);
      //      histo->GetXaxis()->SetOption('R');
    }
  }
  return;
}


//=====================================================================
//  GetNumberOfBinsX()
//=====================================================================
Int_t GetNumberOfBinsX(TH2F *histo) {
  // cout << " <TH2F> Setting labels for histogram " << histo->GetTitle() << endl;

  Int_t Nbins = histo->GetNbinsX();
  // cout << Nbins << endl;

  return Nbins;
}


/*
//=====================================================================
// checkNCPUs()
//=====================================================================
void checkNCPUs() {
  // cout << "In checkNCPUs()..." << endl;

  Char_t dir[MaxPathSize];
  Char_t dir2[MaxPathSize];

  sprintf(filename,"%s/../../info.txt",path);
  if (checkFile(filename)!=0) {
    sprintf(filename,"%s/../info.txt",path);
    if (checkFile(filename)!=0) {  
      sprintf(filename,"%s/info.txt",path);
      if (checkFile(filename)!=0) {     
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
*/
