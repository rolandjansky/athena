/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <plotPositions.h>     

//=====================================================================
//
//  plotPositions
//
//=====================================================================

void plotPositions(Char_t extPath[MaxPathSize]="",Int_t NumIter=-1, bool GlobalPos = true,Char_t Option[MaxPathSize]="WholeID"){
  //gROOT->SetBatch(kTRUE);

  //===================
  // Path
  //===================
  sprintf(path,"/afs/cern.ch/user/l/lacuesta/w0/users/lacuesta/AliEndcaps/");
  Iterations = 5;

  
  bool customLabel = true;
  const char *xBinLabel[11] = {"Nom","Surv","L1","L1","L1.8", "L1.8","L2.7","L2.7","L3","L3","Cog"};
  checkExternals(extPath);
  GraphicsStyle();
  //=================
  // Iteration Number
  //=================
  if (NumIter!=-1) NumberOfIters = NumIter;
  //introduim el num iteracions
  else NumberOfIters = Iterations;
  
  cout<<" Number of Iterations: "<< NumberOfIters<<endl;
   
  if(GlobalPos) sprintf(lg, "Global");
  else          sprintf(lg, "Local");

  
  
  //const char *xBinLabel[11] = {"Nom","L3","L18","L18","L18"};
  //==================
  // Module & stave selection
  //==================
  userDetType = 2;
  userBec = 0;
  userLayer = 2;
  userRing = 0;
  userSector = 0;       // Stave
  
  //=================
  // Main color
  //================
  // kOrange, kPink, kViolet, kAzure, kTeal, kSpring
  mainColor = kAzure;
  

  if(!strcmp(Option, "WholeID")){
    doPIXBarrel = true;
    doPIXEndcaps= true;
    doSCTBarrel = true;
    doSCTEndcaps= true;
  };

  if(!strcmp(Option, "PixelBarrel")){
    doPIXBarrel = true;
  };
  
  if(!strcmp(Option, "SCTBarrel")){
    doSCTBarrel = true;
  };
  
  if(!strcmp(Option, "PixelEndcaps")){
    doPIXEndcaps= true;
  };
  
   if(!strcmp(Option, "SCTEndcaps")){
     doSCTEndcaps= true;
   };
   
  if(!strcmp(Option, "Module")){
    doStave     = false;
  };

  if(!strcmp(Option, "Stave")){
    doStave     = true;
  };
  
  //===================
  // Fill Ntuple 
  //===================
  for (int iter=0; iter<NumberOfIters; iter++) {
    if(GlobalPos){
      sprintf(namePath,"%sIter%d/Iter%d_GlobalPositions.txt",path,iter,iter); 
      // reference positions:
      sprintf(namePath2,"Reference_GlobalPositions.txt",path); 
    } else {
      sprintf(namePath,"%sIter%d/Iter%d_LocalPositions.txt",path,iter,iter);
      // reference positions:      
      sprintf(namePath2,"Reference_LocalPositions.txt",path);
    }
    ifstream filePositions;
    ifstream filePositionsReference;

    if(iter==0){
      ntupleConstantsVsIter = new TNtuple("ntupleConstants_VsIter","data fron txt","Iter:MCode:DetType:Bec:Layer:Ring:Sector:DifTx:DifTy:DifTz:DifRx:DifRy:DifRz");
    }     

    filePositions.open(namePath);
    filePositionsReference.open(namePath2);

    for (int i=0;i<5832;i++){  
      if(GlobalPos){
	filePositions >>  MCode >>  Layer >> Sector >> Ring >> Side >> Tx >> Ty >> Tz >> Rx >> Ry >> Rz >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp; 
       	filePositionsReference >> MCode >> Layer >> Sector >> Ring >> Side >> PTx >>PTy >> PTz >> PRx >> PRy >> PRz >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp;

      } else {
	filePositions >>  MCode >> Tx >> Ty >> Tz >> Rx >> Ry >> Rz;                
	filePositionsReference >> MCode >>  PTx >>PTy >> PTz >> PRx >> PRy >> PRz;    
      } 
      
      ntupleConstantsVsIter->Fill(iter,MCode, invMcode(MCode, "dettype"),invMcode(MCode, "bec"), invMcode(MCode, "layer"), invMcode(MCode, "ring"),invMcode(MCode, "sector"), Tx-PTx,Ty-PTy,Tz-PTz,Rx-PRx,Ry-PRy,Rz-PRz);   
      //cout << MCode << " bec: " << invMcode(MCode, "bec") << " dettype: " << invMcode(MCode, "dettype") << " layer: " << invMcode(MCode, "layer") << " ring: ";
      //cout << invMcode(MCode, "ring") << " sector: " << invMcode(MCode, "sector")<< "Delta Tx: "  <<Tx-PTx << endl;
    }  
    filePositions.close();
    filePositionsReference.close();
  }
  //--------------------------//
  //       Pixel Barrel       //
  //--------------------------// 
  if(doPIXBarrel){        
    for( int layer = 0; layer < 3; layer ++){            
      if(layer == 0) NumSectors = 22;
      if(layer == 1) NumSectors = 38;
      if(layer == 2) NumSectors = 52;
      
      //CreatePalette(NumSectors);
      double maxv[6] = {0,0,0,0,0,0};   
      double minv[6] = {0,0,0,0,0,0};       
      double maxabs[6] = {0,0,0,0,0,0};
      sprintf(canvasname,"cPIX_Ba_L%d",layer);
      cPIX_Ba[layer] = new TCanvas(canvasname,canvasname, CanvasSizeX6[0],CanvasSizeX6[1]);
      cPIX_Ba[layer]->SetFillColor(0);  
      cPIX_Ba[layer]->Divide(3,2);
      int counter = 0;
      cout << "*** Filling Pixel Barrel Layer "<< layer << " Graphs ***" << endl; 
      for(int sector=0 ; sector<NumSectors; sector++){
	for(int ring=-6; ring<=6; ring++){
	
	  sprintf(condition,"DetType==1&&Bec==0&&Layer==%d&&Ring==%d&&Sector==%d",layer,ring,sector);     
	  //cout << "Condition " << condition << endl;
	  for (int k=0; k<6; k++){
	    if(k==0)sprintf(name2,"DifTx:Iter");
	    if(k==1)sprintf(name2,"DifTy:Iter");
	    if(k==2)sprintf(name2,"DifTz:Iter");
	    if(k==3)sprintf(name2,"DifRx:Iter");
	    if(k==4)sprintf(name2,"DifRy:Iter");
	    if(k==5)sprintf(name2,"DifRz:Iter");
	    ntupleConstantsVsIter->Draw(name2,condition,"goff");
	    hGraphPixBa[layer][k][counter]= new TGraph(ntupleConstantsVsIter->GetSelectedRows(), 
						       ntupleConstantsVsIter->GetV2(),
						       ntupleConstantsVsIter->GetV1());
	    hGraphPixBa[layer][k][counter]->SetLineColor(mainColor-9+(19*sector/NumSectors));
	    for(int z = 0; z < NumberOfIters ; z++){
	      maxv[k] = max(ntupleConstantsVsIter->GetV1()[z],maxv[k]);
	      minv[k] = min(ntupleConstantsVsIter->GetV1()[z],minv[k]);
	      maxabs[k] = max(fabs(maxv[k]),fabs(minv[k]));	
	    }
	  }
	  counter ++;
	} 
      }
    
      counter = 0;
      cout << "*** Drawing Pixel Barrel Layer "<< layer << " Graphs ***" << endl; 
      for(int sector=0 ; sector<NumSectors; sector++){
	for(int ring=-6; ring<=6; ring++){  
	  for (int k=0; k<6; k++) {
	    cPIX_Ba[layer]->cd(k+1);
	    if(counter==0){
	      if(k==0){sprintf(title,"PIX Barrel L%d: %s #DeltaTx", layer, lg); sprintf(YAxis,"%s #DeltaTx(mm)",lg);}
	      if(k==1){sprintf(title,"PIX Barrel L%d: %s #DeltaTy", layer, lg); sprintf(YAxis,"%s #DeltaTy(mm)",lg);}
	      if(k==2){sprintf(title,"PIX Barrel L%d: %s #DeltaTz", layer, lg); sprintf(YAxis,"%s #DeltaTz(mm)",lg);}
	      if(k==3){sprintf(title,"PIX Barrel L%d: %s #DeltaRx", layer, lg); sprintf(YAxis,"%s #DeltaRx(mrad)",lg);}
	      if(k==4){sprintf(title,"PIX Barrel L%d: %s #DeltaRy", layer, lg); sprintf(YAxis,"%s #DeltaRy(mrad)",lg);}
	      if(k==5){sprintf(title,"PIX Barrel L%d: %s #DeltaRz", layer, lg); sprintf(YAxis,"%s #DeltaRz(mrad)",lg);}
	      hGraphPixBa[layer][k][0]->SetTitle(title);
	      hGraphPixBa[layer][k][0]->GetYaxis()->SetRangeUser(-maxabs[k],maxabs[k]);
	      hGraphPixBa[layer][k][0]->Draw("ALP");
	      hGraphPixBa[layer][k][0]->GetYaxis()->SetTitle(YAxis);
	      hGraphPixBa[layer][k][0]->SetLineWidth(1);
	      //hGraphPixBa[layer][k][0]->SetLineColor(kBlack);
	      hGraphPixBa[layer][k][0]->GetXaxis()->SetNdivisions(NumberOfIters);
	      if(customLabel) for(int l=0; l<NumberOfIters; l++)  hGraphPixBa[layer][k][0]->GetXaxis()->SetBinLabel(1+((l*100)/NumberOfIters), xBinLabel[l]);
	      else  hGraphPixBa[layer][k][0]->GetXaxis()->SetTitle("Iteration");
	      gPad->SetGridx(); 
	      gPad->SetGridy();
	    }
	    else hGraphPixBa[layer][k][counter]->Draw("LP");
	    cPIX_Ba[layer]->Update();
	  }
	  counter ++;
	}
      }
    }
  }
  
  //--------------------------//
  //       Pixel Endcaps       //
  //--------------------------// 
  if(doPIXEndcaps){   
    char *Endcap[2] = {"A","C"};
    for(int ec=0; ec<2; ec++){
      for( int disk = 0; disk < 3; disk ++){            
	NumSectors = 48;
	
	double maxv[6] = {0,0,0,0,0,0};   
	double minv[6] = {0,0,0,0,0,0};       
	double maxabs[6] = {0,0,0,0,0,0};
	
	sprintf(canvasname,"cPIX_EC%s_D%d",Endcap[ec],disk);
	
	cPIX_EC[ec][disk] = new TCanvas(canvasname,canvasname, CanvasSizeX6[0],CanvasSizeX6[1]);
	cPIX_EC[ec][disk]->SetFillColor(0);  
	cPIX_EC[ec][disk]->Divide(3,2);
	int counter = 0;
	cout << "*** Filling Pixel Endcap "<< Endcap[ec]<< " Disk " << disk << " Graphs ***" << endl; 
	for(int sector=0 ; sector<NumSectors; sector++){
	  if(ec==0) sprintf(condition,"DetType==1&&Bec==1&&Layer==%d&&Sector==%d",disk,sector);     
	  if(ec==1) sprintf(condition,"DetType==1&&Bec==-1&&Layer==%d&&Sector==%d",disk,sector);     
	  cout << "Condition " << condition << endl;
	  for (int k=0; k<6; k++){
	    if(k==0)sprintf(name2,"DifTx:Iter");
	    if(k==1)sprintf(name2,"DifTy:Iter");
	    if(k==2)sprintf(name2,"DifTz:Iter");
	    if(k==3)sprintf(name2,"DifRx:Iter");
	    if(k==4)sprintf(name2,"DifRy:Iter");
	    if(k==5)sprintf(name2,"DifRz:Iter");
	    ntupleConstantsVsIter->Draw(name2,condition,"goff");
	    hGraphPixEC[ec][disk][k][counter]= new TGraph(ntupleConstantsVsIter->GetSelectedRows(), 
							   ntupleConstantsVsIter->GetV2(),
							   ntupleConstantsVsIter->GetV1());
	    hGraphPixEC[ec][disk][k][counter]->SetLineColor(mainColor-9+(19*sector/NumSectors));
	    for(int z = 0; z < NumberOfIters ; z++){
	      maxv[k] = max(ntupleConstantsVsIter->GetV1()[z],maxv[k]);
	      minv[k] = min(ntupleConstantsVsIter->GetV1()[z],minv[k]);
	      maxabs[k] = max(fabs(maxv[k]),fabs(minv[k]));	
	    }
	  }
	  counter ++;
	} 
	
    
   	cout << "*** Drawing Pixel Endcap "<< Endcap[ec]<< " Disk "<< disk << " Graphs ***" << endl; 
	counter = 0;
	for(int sector=0 ; sector<NumSectors; sector++){
	  for (int k=0; k<6; k++) {
	    cPIX_EC[ec][disk]->cd(k+1);
	    if(counter==0){
	      
	      if(k==0){sprintf(title,"PIX EC %s D%d: %s #DeltaTx", Endcap[ec],disk, lg); sprintf(YAxis,"%s #DeltaTx(mm)",lg);}
	      if(k==1){sprintf(title,"PIX EC %s D%d: %s #DeltaTy", Endcap[ec],disk, lg); sprintf(YAxis,"%s #DeltaTy(mm)",lg);}
	      if(k==2){sprintf(title,"PIX EC %s D%d: %s #DeltaTz", Endcap[ec],disk, lg); sprintf(YAxis,"%s #DeltaTz(mm)",lg);}
	      if(k==3){sprintf(title,"PIX EC %s D%d: %s #DeltaRx", Endcap[ec],disk, lg); sprintf(YAxis,"%s #DeltaRx(mrad)",lg);}
	      if(k==4){sprintf(title,"PIX EC %s D%d: %s #DeltaRy", Endcap[ec],disk, lg); sprintf(YAxis,"%s #DeltaRy(mrad)",lg);}
	      if(k==5){sprintf(title,"PIX EC %s D%d: %s #DeltaRz", Endcap[ec],disk, lg); sprintf(YAxis,"%s #DeltaRz(mrad)",lg);}
	      
	      hGraphPixEC[ec][disk][k][0]->SetTitle(title);
	      hGraphPixEC[ec][disk][k][0]->GetYaxis()->SetRangeUser(-maxabs[k],maxabs[k]);
	      hGraphPixEC[ec][disk][k][0]->Draw("ALP");
	      hGraphPixEC[ec][disk][k][0]->GetYaxis()->SetTitle(YAxis);
	      hGraphPixEC[ec][disk][k][0]->SetLineWidth(1);
	      //hGraphPixBa[layer][k][0]->SetLineColor(kBlack);
	      hGraphPixEC[ec][disk][k][0]->GetXaxis()->SetNdivisions(NumberOfIters);
	      if(customLabel) for(int l=0; l<NumberOfIters; l++)  hGraphPixEC[ec][disk][k][0]->GetXaxis()->SetBinLabel(1+((l*100)/NumberOfIters), xBinLabel[l]);
	      else  hGraphPixEC[ec][disk][k][0]->GetXaxis()->SetTitle("Iteration");
	      gPad->SetGridx(); 
	      gPad->SetGridy();
	    }
	    else hGraphPixEC[ec][disk][k][counter]->Draw("LP");
	    cPIX_EC[ec][disk]->Update();
	  }
	  counter ++;
	}
      }
    }
  }
  
  //--------------------------//
  //       SCT Barrel       //
  //--------------------------//   
  if(doSCTBarrel){  
    for( int layer = 0; layer < 4; layer ++){            
      if(layer == 0) NumSectors = 32;
      if(layer == 1) NumSectors = 40;
      if(layer == 2) NumSectors = 48;
      if(layer == 3) NumSectors = 56;
     
      double maxv[6] = {0,0,0,0,0,0};   
      double minv[6] = {0,0,0,0,0,0};       
      double maxabs[6] = {0,0,0,0,0,0};
      
      int counter = 0;
      cout << "*** Filling SCT Barrel Layer "<< layer << " Graphs ***" << endl; 
      for(int sector=0 ; sector<NumSectors; sector++){
	for(int ring=-6; ring<=6; ring++){
	  if(ring == 0) ring++;
	  sprintf(condition,"DetType==2&&Bec==0&&Layer==%d&&Ring==%d&&Sector==%d",layer,ring,sector);     
	  cout << "Condition " << condition << endl;
	  for (int k=0; k<6; k++){
	    if(k==0)sprintf(name2,"DifTx:Iter");
	    if(k==1)sprintf(name2,"DifTy:Iter");
	    if(k==2)sprintf(name2,"DifTz:Iter");
	    if(k==3)sprintf(name2,"DifRx:Iter");
	    if(k==4)sprintf(name2,"DifRy:Iter");
	    if(k==5)sprintf(name2,"DifRz:Iter");
	    ntupleConstantsVsIter->Draw(name2,condition,"goff");
	    hGraphSCTBa[layer][k][counter]= new TGraph(ntupleConstantsVsIter->GetSelectedRows(), 
						       ntupleConstantsVsIter->GetV2(),
						       ntupleConstantsVsIter->GetV1());
	    hGraphSCTBa[layer][k][counter]->SetLineColor(mainColor-9+(19*sector/NumSectors)); 
	    for(int z = 0; z < NumberOfIters ; z++){
	      maxv[k] = max(ntupleConstantsVsIter->GetV1()[z],maxv[k]);
	      minv[k] = min(ntupleConstantsVsIter->GetV1()[z],minv[k]);
	      maxabs[k] = max(fabs(maxv[k]),fabs(minv[k]));	
	    }
	  }
	  counter ++;
	} 
      }
    
      counter = 0;
      cout << "*** Drawing SCT Barrel Layer "<< layer << " Graphs ***" << endl;
      sprintf(canvasname,"cSCT_Ba_L%d",layer);
      cSCT_Ba[layer] = new TCanvas(canvasname,canvasname, CanvasSizeX6[0],CanvasSizeX6[1]);
      cSCT_Ba[layer]->SetFillColor(0);  
      cSCT_Ba[layer]->Divide(3,2);
      for(int sector=0 ; sector<NumSectors; sector++){
	for(int ring=-6; ring<=6; ring++){  
	  if(ring == 0) ring++;
	  for (int k=0; k<6; k++) {
	    cSCT_Ba[layer]->cd(k+1);
	    if(counter==0){
	      if(k==0){sprintf(title,"SCT Barrel L%d: %s #DeltaTx", layer, lg); sprintf(YAxis,"%s #DeltaTx(mm)",lg);}
	      if(k==1){sprintf(title,"SCT Barrel L%d: %s #DeltaTy", layer, lg); sprintf(YAxis,"%s #DeltaTy(mm)",lg);}
	      if(k==2){sprintf(title,"SCT Barrel L%d: %s #DeltaTz", layer, lg); sprintf(YAxis,"%s #DeltaTz(mm)",lg);}
	      if(k==3){sprintf(title,"SCT Barrel L%d: %s #DeltaRx", layer, lg); sprintf(YAxis,"%s #DeltaRx(mrad)",lg);}
	      if(k==4){sprintf(title,"SCT Barrel L%d: %s #DeltaRy", layer, lg); sprintf(YAxis,"%s #DeltaRy(mrad)",lg);}
	      if(k==5){sprintf(title,"SCT Barrel L%d: %s #DeltaRz", layer, lg); sprintf(YAxis,"%s #DeltaRz(mrad)",lg);}
	      hGraphSCTBa[layer][k][0]->SetTitle(title);
	      hGraphSCTBa[layer][k][0]->GetYaxis()->SetRangeUser(-maxabs[k],maxabs[k]);
	      hGraphSCTBa[layer][k][0]->Draw("ALP");
	      hGraphSCTBa[layer][k][0]->GetYaxis()->SetTitle(YAxis);
	      hGraphSCTBa[layer][k][0]->SetLineWidth(1);
	      // hGraphSCTBa[layer][k][0]->SetLineColor(ring);
	      hGraphSCTBa[layer][k][0]->GetXaxis()->SetNdivisions(NumberOfIters);
	      if(customLabel) for(int l = 0; l<NumberOfIters; l++)  hGraphSCTBa[layer][k][0]->GetXaxis()->SetBinLabel(1+((l*100)/NumberOfIters), xBinLabel[l]);
	      else   hGraphSCTBa[layer][k][0]->GetXaxis()->SetTitle("Iteration number");
	      
	      gPad->SetGridx(); 
	      gPad->SetGridy();
	    }
	    else hGraphSCTBa[layer][k][counter]->Draw("LP");
	    cSCT_Ba[layer]->Update();
	  }
	  counter ++;
	}
      }
    }
  }

  //--------------------------//
  //       SCT Endcaps       //
  //--------------------------// 
  if(doSCTEndcaps){   
    char *Endcap[2] = {"A","C"};
    for(int ec=0; ec<2; ec++){    
      for( int disk = 0; disk < 9; disk ++){ 
	if(disk==0) NumRings = 2;
	if(disk==1 || disk == 2 || disk ==3 || disk == 4 || disk == 5) NumRings = 3;
	if(disk==6 || disk ==7) NumRings = 2;
	if(disk==8) NumRings = 1;
	
	double maxv[6] = {0,0,0,0,0,0};   
	double minv[6] = {0,0,0,0,0,0};       
	double maxabs[6] = {0,0,0,0,0,0};
	
	sprintf(canvasname,"cSCT_EC%s_D%d",Endcap[ec],disk);
	
	cSCT_EC[ec][disk] = new TCanvas(canvasname,canvasname, CanvasSizeX6[0],CanvasSizeX6[1]);
	cSCT_EC[ec][disk]->SetFillColor(0);  
	cSCT_EC[ec][disk]->Divide(3,2);
	int counter = 0;
	cout << "*** Filling SCT Endcap "<< Endcap[ec]<< " Disk " << disk << " Graphs ***" << endl; 
	for(int ring = 0; ring < NumRings; ring++){	
	  if(ring==0) NumSectors = 52;
	  else NumSectors = 40;
	  for(int sector=0 ; sector<NumSectors; sector++){
	    if(ec==0) sprintf(condition,"DetType==2&&Bec==1&&Layer==%d&&Ring==%d&&Sector==%d",disk,ring,sector);     
	    if(ec==1) sprintf(condition,"DetType==2&&Bec==-1&&Layer==%d&&Ring==%d&&Sector==%d",disk,ring,sector);     
	    cout << "Condition " << condition << endl;
	    for (int k=0; k<6; k++){
	      if(k==0)sprintf(name2,"DifTx:Iter");
	      if(k==1)sprintf(name2,"DifTy:Iter");
	      if(k==2)sprintf(name2,"DifTz:Iter");
	      if(k==3)sprintf(name2,"DifRx:Iter");
	      if(k==4)sprintf(name2,"DifRy:Iter");
	      if(k==5)sprintf(name2,"DifRz:Iter");
	      ntupleConstantsVsIter->Draw(name2,condition,"goff");
	      hGraphSCTEC[ec][disk][k][counter]= new TGraph(ntupleConstantsVsIter->GetSelectedRows(), 
							    ntupleConstantsVsIter->GetV2(),
							    ntupleConstantsVsIter->GetV1());
	      hGraphSCTEC[ec][disk][k][counter]->SetLineColor(mainColor-9+(19*sector/NumSectors));
	      for(int z = 0; z < NumberOfIters ; z++){
		maxv[k] = max(ntupleConstantsVsIter->GetV1()[z],maxv[k]);
		minv[k] = min(ntupleConstantsVsIter->GetV1()[z],minv[k]);
		maxabs[k] = max(fabs(maxv[k]),fabs(minv[k]));	
	      }
	    }
	    counter ++;
	  }
	} 
	
    
   	cout << "*** Drawing SCT Endcap "<< Endcap[ec]<< " Disk "<< disk << " Graphs ***" << endl; 
	counter = 0;
	for(int ring = 0; ring < NumRings; ring++){
	  if(ring==0) NumSectors = 52;
	  else NumSectors = 40;
	  for(int sector=0 ; sector<NumSectors; sector++){
	    for (int k=0; k<6; k++) {
	      cSCT_EC[ec][disk]->cd(k+1);
	      if(counter==0){
		
		if(k==0){sprintf(title,"SCT EC %s D%d: %s #DeltaTx", Endcap[ec],disk, lg); sprintf(YAxis,"%s #DeltaTx(mm)",lg);}
		if(k==1){sprintf(title,"SCT EC %s D%d: %s #DeltaTy", Endcap[ec],disk, lg); sprintf(YAxis,"%s #DeltaTy(mm)",lg);}
		if(k==2){sprintf(title,"SCT EC %s D%d: %s #DeltaTz", Endcap[ec],disk, lg); sprintf(YAxis,"%s #DeltaTz(mm)",lg);}
		if(k==3){sprintf(title,"SCT EC %s D%d: %s #DeltaRx", Endcap[ec],disk, lg); sprintf(YAxis,"%s #DeltaRx(mrad)",lg);}
		if(k==4){sprintf(title,"SCT EC %s D%d: %s #DeltaRy", Endcap[ec],disk, lg); sprintf(YAxis,"%s #DeltaRy(mrad)",lg);}
		if(k==5){sprintf(title,"SCT EC %s D%d: %s #DeltaRz", Endcap[ec],disk, lg); sprintf(YAxis,"%s #DeltaRz(mrad)",lg);}
		
		hGraphSCTEC[ec][disk][k][0]->SetTitle(title);
		hGraphSCTEC[ec][disk][k][0]->GetYaxis()->SetRangeUser(-maxabs[k],maxabs[k]);
		hGraphSCTEC[ec][disk][k][0]->Draw("ALP");
		hGraphSCTEC[ec][disk][k][0]->GetYaxis()->SetTitle(YAxis);
		hGraphSCTEC[ec][disk][k][0]->SetLineWidth(1);
		//hGraphPixBa[layer][k][0]->SetLineColor(kBlack);
		hGraphSCTEC[ec][disk][k][0]->GetXaxis()->SetNdivisions(NumberOfIters);
		if(customLabel) for(int l=0; l<NumberOfIters; l++)  hGraphSCTEC[ec][disk][k][0]->GetXaxis()->SetBinLabel(1+((l*100)/NumberOfIters), xBinLabel[l]);
		else  hGraphSCTEC[ec][disk][k][0]->GetXaxis()->SetTitle("Iteration");
		gPad->SetGridx(); 
		gPad->SetGridy();
	      }
	      else hGraphSCTEC[ec][disk][k][counter]->Draw("LP");
	      cSCT_EC[ec][disk]->Update();
	    }
	    counter ++;
	  }
	}
      }
    }
  }

  
  //--------------------------//
  //       Module       //
  //--------------------------// 
  if(doModule){
    double maxv[6] = {0,0,0,0,0,0};   
    double minv[6] = {0,0,0,0,0,0};       
    double maxabs[6] = {0,0,0,0,0,0};
    sprintf(condition,"DetType==%d&&Bec==%d&&Layer==%d&&Ring==%d&&Sector==%d",userDetType, userBec, userLayer,userRing, userSector);     
    //cout << "Condition " << condition << endl;
    if(userDetType == 1) {
      if( userBec == -1) sprintf(canvasname,"cModule_PIX_EndcapA_D%d_R%d_S%d",userLayer, userRing, userSector );
      if( userBec == 0) sprintf(canvasname,"cModule_PIX_Barrel_L%d_R%d_S%d",userLayer, userRing, userSector );
      if( userBec == 1) sprintf(canvasname,"cModule_PIX_EndcapC_D%d_R%d_S%d",userLayer, userRing, userSector );
    }
    if(userDetType == 2) {
      if( userBec == -1) sprintf(canvasname,"cModule_SCT_EndcapA_D%d_R%d_S%d",userLayer, userRing, userSector );
      if( userBec == 0) sprintf(canvasname,"cModule_SCT_Barrel_L%d_R%d_S%d",userLayer, userRing, userSector );
      if( userBec == 1) sprintf(canvasname,"cModule_SCT_EndcapC_D%d_R%d_S%d",userLayer, userRing, userSector );
    }
	
    cModule = new TCanvas(canvasname,canvasname, CanvasSizeX6[0],CanvasSizeX6[1]);
    cModule->SetFillColor(0);  
    cModule->Divide(3,2);
    for (int k=0; k<6; k++){
      if(k==0)sprintf(name2,"DifTx:Iter");
      if(k==1)sprintf(name2,"DifTy:Iter");
      if(k==2)sprintf(name2,"DifTz:Iter");
      if(k==3)sprintf(name2,"DifRx:Iter");
      if(k==4)sprintf(name2,"DifRy:Iter");
      if(k==5)sprintf(name2,"DifRz:Iter");
      ntupleConstantsVsIter->Draw(name2,condition,"goff");
      hGraphModule[k]= new TGraph(ntupleConstantsVsIter->GetSelectedRows(), 
						       ntupleConstantsVsIter->GetV2(),
						       ntupleConstantsVsIter->GetV1());
      for(int z = 0; z < NumberOfIters ; z++){
	maxv[k] = max(ntupleConstantsVsIter->GetV1()[z],maxv[k]);
	minv[k] = min(ntupleConstantsVsIter->GetV1()[z],minv[k]);
	maxabs[k] = max(fabs(maxv[k]),fabs(minv[k]));	
      }
      if(k==0){sprintf(title,"Module: %s #DeltaTx", lg); sprintf(YAxis,"%s #DeltaTx(mm)",lg);}
      if(k==1){sprintf(title,"Module: %s #DeltaTy", lg); sprintf(YAxis,"%s #DeltaTy(mm)",lg);}
      if(k==2){sprintf(title,"Module: %s #DeltaTz", lg); sprintf(YAxis,"%s #DeltaTz(mm)",lg);}
      if(k==3){sprintf(title,"Module: %s #DeltaRx", lg); sprintf(YAxis,"%s #DeltaRx(mrad)",lg);}
      if(k==4){sprintf(title,"Module: %s #DeltaRy", lg); sprintf(YAxis,"%s #DeltaRy(mrad)",lg);}
      if(k==5){sprintf(title,"Module: %s #DeltaRz", lg); sprintf(YAxis,"%s #DeltaRz(mrad)",lg);}
      cModule->cd(k+1);
      hGraphModule[k]->SetTitle(title);
      hGraphModule[k]->GetYaxis()->SetRangeUser(-maxabs[k],maxabs[k]);
      hGraphModule[k]->Draw("ALP");
      hGraphModule[k]->GetXaxis()->SetTitle("Iteration number");
      hGraphModule[k]->GetYaxis()->SetTitle(YAxis);
      hGraphModule[k]->SetLineWidth(1);
      hGraphModule[k]->SetLineColor(kBlack);
      hGraphModule[k]->GetXaxis()->SetNdivisions(NumberOfIters);
      gPad->SetGridx(); 
      gPad->SetGridy();   
    }
      
  }
  if(doStave){
    double maxv[6] = {0,0,0,0,0,0};   
      double minv[6] = {0,0,0,0,0,0};       
      double maxabs[6] = {0,0,0,0,0,0};
      
      int counter = 0;
      cout << "*** Filling Stave Graphs ***" << endl; 
      for(int ring=-6; ring<=6; ring++){
	if(userDetType ==2 && ring==0 ) ring++;
	sprintf(condition,"DetType==%d&&Bec==%d&&Layer==%d&&Ring==%d&&Sector==%d",userDetType,userBec, userLayer,ring,userSector);     
	  cout << "Condition " << condition << endl;
	  for (int k=0; k<6; k++){
	    if(k==0)sprintf(name2,"DifTx:Iter");
	    if(k==1)sprintf(name2,"DifTy:Iter");
	    if(k==2)sprintf(name2,"DifTz:Iter");
	    if(k==3)sprintf(name2,"DifRx:Iter");
	    if(k==4)sprintf(name2,"DifRy:Iter");
	    if(k==5)sprintf(name2,"DifRz:Iter");
	    ntupleConstantsVsIter->Draw(name2,condition,"goff");
	    hGraphStave[k][counter]= new TGraph(ntupleConstantsVsIter->GetSelectedRows(), 
						       ntupleConstantsVsIter->GetV2(),
						       ntupleConstantsVsIter->GetV1());
	    hGraphStave[k][counter]->SetLineColor(mainColor+ring); 
	    if(counter == 0 ){
	      if(k==0){sprintf(title,"Stave: #DeltaTx"); sprintf(YAxis,"%s #DeltaTx(mm)",lg);}
	      if(k==1){sprintf(title,"Stave: #DeltaTy"); sprintf(YAxis,"%s #DeltaTy(mm)",lg);}
	      if(k==2){sprintf(title,"Stave: #DeltaTz"); sprintf(YAxis,"%s #DeltaTz(mm)",lg);}
	      if(k==3){sprintf(title,"Stave: #DeltaRx"); sprintf(YAxis,"%s #DeltaRx(mrad)",lg);}
	      if(k==4){sprintf(title,"Stave: #DeltaRy"); sprintf(YAxis,"%s #DeltaRy(mrad)",lg);}
	      if(k==5){sprintf(title,"Stave: #DeltaRz"); sprintf(YAxis,"%s #DeltaRz(mrad)",lg);}
	      hGraphStave[k][0]->SetTitle(title);
	      hGraphStave[k][0]->GetYaxis()->SetTitle(YAxis);
	      hGraphStave[k][0]->SetLineWidth(1);
	      // hGraphStave[k][0]->SetLineColor(ring);
	      hGraphStave[k][0]->GetXaxis()->SetNdivisions(NumberOfIters);
	      if(!customLabel) hGraphStave[k][0]->GetXaxis()->SetTitle("Iteration number");
	      else {
		for(int l=0; l<NumberOfIters; l++)  {
		  hGraphStave[k][0]->GetXaxis()->SetBinLabel(((l*100)/NumberOfIters)+1, xBinLabel[l]);
		  hGraphStave[k][0]->GetXaxis()->SetTitle("");
		};
	      }
	    } 
	    for(int z = 0; z < NumberOfIters ; z++){
	      maxv[k] = max(ntupleConstantsVsIter->GetV1()[z],maxv[k]);
	      minv[k] = min(ntupleConstantsVsIter->GetV1()[z],minv[k]);
	      maxabs[k] = max(fabs(maxv[k]),fabs(minv[k]));	
	    }
	  }
	  counter ++;
	} 
      
    
      counter = 0;
      cout << "*** Drawing Stave Graphs ***" << endl;
      if(userDetType==1) sprintf(canvasname,"cStave_PIX_Barrel_L%d_S%d",userLayer, userSector );
      else               sprintf(canvasname,"cStave_SCT_Barrel_L%d_S%d",userLayer, userSector );
	
      cStave = new TCanvas(canvasname,canvasname, CanvasSizeX6[0],CanvasSizeX6[1]);
      cStave->SetFillColor(0);  
      cStave->Divide(3,2);
      for(int ring=-6; ring<=6; ring++){  
	if(userDetType ==2 && ring==0 ) ring++;
	for (int k=0; k<6; k++) {
	  cStave->cd(k+1);
	  if(counter==0){
	    hGraphStave[k][0]->GetYaxis()->SetRangeUser(-maxabs[k],maxabs[k]);
	    hGraphStave[k][0]->Draw("ALP");
	    gPad->SetGridx(); 
	    gPad->SetGridy();
	    
	  }
	  else {
	    hGraphStave[k][counter]->Draw("LP");
	  }
	  cStave->Update();
	}
	counter ++;
      }
  }
  printPS(GlobalPos); 
}
                 
//=====================================================================
// checkExternals()
//=====================================================================

void checkExternals(Char_t extPath[MaxPathSize]) {
  
  // check the path
  if (strcmp(extPath,"")) {
    sprintf(path,"%s",extPath);
    cout << "-> using a external path: " << path << endl; 
  }
  else cout << "-> using path: " << path << endl; 
  
  return;
}




int invMcode(int mcode, Char_t parameter[8]){
  int bec, dettype, layer, ring, sector;
  int tmpmcode; 
  bec = 0;
  tmpmcode = mcode;
  if(mcode>10000000){
    bec = 1;
    tmpmcode = (mcode/2)-10000000;
  }
  if(mcode<-10000000){
    bec = -1;
    tmpmcode = (-mcode/2)-10000000;
  }
  dettype = tmpmcode/1000000;
  tmpmcode -= dettype*1000000;
	
  layer = tmpmcode/100000;
  tmpmcode -= layer*100000;

  ring = (tmpmcode/1000)-10;
  tmpmcode -= 1000*(10+ring);
	
  sector = tmpmcode;
	
  if (!strcmp(parameter,"bec")) return bec;
  if (!strcmp(parameter,"dettype")) return dettype;      
  if (!strcmp(parameter,"layer")) return layer;      
  if (!strcmp(parameter,"ring")) return ring;      
  if (!strcmp(parameter,"sector")) return sector;
  return -1;
}

//=====================================================================
// printPS()
//=====================================================================
void printPS(bool GlobalPos) {
  
  cout << "\nStoring the plots in a ps file..." << endl;
  
  if(GlobalPos) sprintf(filename,"%s/GlobalPositionEvolution.ps",path);
  else          sprintf(filename,"%s/LocalPositionEvolution.ps",path); 
  TCanvas c0;
  sprintf(command,"%s[",filename);
  c0.Print(command);
  
  //---------------------------------------------------------------------
  for(int layer = 0; layer <3; layer++){
    if (cPIX_Ba[layer]) cPIX_Ba[layer]->Print(filename);
  }
  
  for(int ec=0; ec<2 ; ec++){
    for(int disk = 0; disk <3; disk++){
      if (cPIX_EC[ec][disk]) cPIX_EC[ec][disk]->Print(filename);
    }
  }

  for(int ec=0; ec<2 ; ec++){
    for(int disk = 0; disk <9; disk++){
      if (cSCT_EC[ec][disk]) cSCT_EC[ec][disk]->Print(filename);
    }
  }
  
  for(int layer = 0; layer <4; layer++){
    if (cSCT_Ba[layer]) cSCT_Ba[layer]->Print(filename);
  }
  if (cModule) cModule->Print(filename);
  if (cStave) cStave->Print(filename);
 
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
// GraphicsStyle()
//=====================================================================  
void GraphicsStyle(){
  gStyle->SetCanvasColor(0);
  gStyle->SetPadColor(kWhite);
  //gStyle->SetPalette(1);  
  // TGAxis::SetMaxDigits(2);  
  return;
}


