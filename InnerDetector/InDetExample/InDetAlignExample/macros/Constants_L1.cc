/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************//
//    RTT Constants at Level1 for rel_X ; X= [0,6]         //
//*********************************************************//
#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <TH1F.h>
#include <TCanvas.h>
#include <TLegend.h>
void Constants_L1(){
 
  ifstream relX;
  const int struc = 4;
  const int rel=7;
  //Constants values
  double X,pTx,pTy,pTz,pRx,pRy,pRz;
  
  //Definim els vectors
  float Tx[struc][rel],Ty[struc][rel],Tz[struc][rel],Rx[struc][rel],Ry[struc][rel],Rz[struc][rel];
  
  for (int j=0; j<struc;j++){
    for (int i=0; i<rel;i++){
      Tx[j][i]=0;
      Ty[j][i]=0;
      Tz[j][i]=0;
      Rx[j][i]=0;
      Ry[j][i]=0;
      Rz[j][i]=0;
    } 
  }
  
  for (int i=0; i<rel;i++){
    if(i==0) relX.open("/afs/cern.ch/atlas/project/RTT/prod/Results/rtt/rel_0/devval/build/x86_64-slc5-gcc43-opt/offline/InDetAlignExample/NewInDetSiLevel1Alignment/Iter0/OutputSiAlignment.txt");
    if(i==1) relX.open("/afs/cern.ch/atlas/project/RTT/prod/Results/rtt/rel_1/devval/build/x86_64-slc5-gcc43-opt/offline/InDetAlignExample/NewInDetSiLevel1Alignment/Iter0/OutputSiAlignment.txt");
    if(i==2) relX.open("/afs/cern.ch/atlas/project/RTT/prod/Results/rtt/rel_2/devval/build/x86_64-slc5-gcc43-opt/offline/InDetAlignExample/NewInDetSiLevel1Alignment/Iter0/OutputSiAlignment.txt");
    if(i==3) relX.open("/afs/cern.ch/atlas/project/RTT/prod/Results/rtt/rel_3/devval/build/x86_64-slc5-gcc43-opt/offline/InDetAlignExample/NewInDetSiLevel1Alignment/Iter0/OutputSiAlignment.txt");
    if(i==4) relX.open("/afs/cern.ch/atlas/project/RTT/prod/Results/rtt/rel_4/devval/build/x86_64-slc5-gcc43-opt/offline/InDetAlignExample/NewInDetSiLevel1Alignment/Iter0/OutputSiAlignment.txt");
    if(i==5) relX.open("/afs/cern.ch/atlas/project/RTT/prod/Results/rtt/rel_5/devval/build/x86_64-slc5-gcc43-opt/offline/InDetAlignExample/NewInDetSiLevel1Alignment/Iter0/OutputSiAlignment.txt");
    if(i==6) relX.open("/afs/cern.ch/atlas/project/RTT/prod/Results/rtt/rel_6/devval/build/x86_64-slc5-gcc43-opt/offline/InDetAlignExample/NewInDetSiLevel1Alignment/Iter0/OutputSiAlignment.txt");
 	

   if(relX.is_open()) {
      cout <<"****  OutputsiAlignment.txt file opened in rel"<< i << endl;   
      char line[200];
      // First line
      relX>>line;
	  for (int j=0; j<4; j++){
		relX >> X >> X >> X >> X >> X >> X >> X >> pTx >> pTy >> pTz >> pRx >> pRy >> pRz;          
		Tx[j][i]=pTx;
		Ty[j][i]=pTy;
		Tz[j][i]=pTz;
		Rx[j][i]=pRx;
		Ry[j][i]=pRy; 
		Rz[j][i]=pRz;
		cout << " Tx: "<< Tx[j][i] << " Ty: "<< Ty[j][i] << " Tz: "<< Tz[j][i] << " Rx: "<< Rx[j][i] <<" Ry: "<< Ry[j][i] <<" Rz: "<< Rz[j][i] <<endl;
     }
    relX.close();
    } 
    else cout<<" **** OutputsiAlignment.txt file doesn't exist in rel"<< i << endl;
  }      
   


  //HISTOGRAM:
  const char *Nightlies[7]={"rel0","rel1","rel2","rel3","rel4","rel5","rel6"};
  const char *Structures[4]={"PIX","SCTECC","SCTBA","SCTECA"};

  //Definim els histogrames:
  TH1F *hTx[struc];
  TH1F *hTy[struc];
  TH1F *hTz[struc];
  TH1F *hRx[struc];
  TH1F *hRy[struc];
  TH1F *hRz[struc];
    
  char name_Tx[200],name_Ty[200],name_Tz[200],name_Rx[200],name_Ry[200],name_Rz[200] ; 

  for (int i=0;i<struc;i++){  
    sprintf(name_Tx,"hTx_struc%d",i);
    sprintf(name_Ty,"hTy_struc%d",i);
    sprintf(name_Tz,"hTz_struc%d",i);
    sprintf(name_Rx,"hRx_struc%d",i);
    sprintf(name_Ry,"hRy_struc%d",i);
    sprintf(name_Rz,"hRz_struc%d",i);
    
    hTx[i]=new TH1F(name_Tx,name_Tx,7,0,7);
    hTy[i]=new TH1F(name_Ty,name_Ty,7,0,7);
    hTz[i]=new TH1F(name_Tz,name_Tz,7,0,7);
    hRx[i]=new TH1F(name_Rx,name_Ry,7,0,7);      
    hRy[i]=new TH1F(name_Ry,name_Rx,7,0,7);
    hRz[i]=new TH1F(name_Rz,name_Rz,7,0,7);  
    
    for (int j=0;j<rel;j++){        
      hTx[i]->SetBinContent(j+1,Tx[i][j]);
      hTx[i]->GetYaxis()->SetTitleOffset(1.3);
      hTx[i]->GetYaxis()->SetLabelSize(0.03);
      hTx[i]->GetYaxis()->SetTitle(" Alignment corrections (mm)");
      hTx[i]->GetXaxis()->SetTitle("Nightlies");
      hTx[i]->GetXaxis()->SetBinLabel(j+1,Nightlies[j]);
      hTx[i]->SetTitle("Alignment Constants Tx");
     
      hTy[i]->GetYaxis()->SetLabelSize(0.03);
      hTy[i]->GetYaxis()->SetTitleOffset(1.3);
      hTy[i]->SetBinContent (j+1,Ty[i][j]);
      hTy[i]->GetYaxis()->SetTitle(" Alignment corrections (mm)");
      hTy[i]->GetXaxis()->SetTitle("Nightlies");
      hTy[i] ->GetXaxis()->SetBinLabel(j+1,Nightlies[j]);
      hTy[i]->SetTitle("Alignment Constants Ty");
      
      hTz[i]->GetYaxis()->SetLabelSize(0.03);
      hTz[i]->GetYaxis()->SetTitleOffset(1.3);
      hTz[i]->SetBinContent (j+1,Tz[i][j]);
      hTz[i]->GetYaxis()->SetTitle(" Alignment corrections (mm)");
      hTz[i]->GetXaxis()->SetTitle("Nightlies");
      hTz[i] ->GetXaxis()->SetBinLabel(j+1,Nightlies[j]);
      hTz[i]->SetTitle("Alignment Constants Tz");
      
      hRx[i]->GetYaxis()->SetLabelSize(0.03);
      hRx[i]->GetYaxis()->SetTitleOffset(1.3);
      hRx[i]->SetBinContent (j+1,Rx[i][j]);
      hRx[i] ->GetXaxis()->SetBinLabel(j+1,Nightlies[j]);
      hRx[i]->SetTitle("Alignment Constants Rx");
      hRx[i]->GetXaxis()->SetTitle("Nightlies");
      hRx[i]->GetYaxis()->SetTitle(" Alignment corrections (mrad)");
      
      hRy[i]->GetYaxis()->SetLabelSize(0.03); 
      hRy[i]->GetYaxis()->SetTitleOffset(1.3);  
      hRy[i]->SetBinContent (j+1,Ry[i][j]);
      hRy[i] ->GetXaxis()->SetBinLabel(j+1,Nightlies[j]);
      hRy[i]->SetTitle("Alignment Constants Ry");
      hRy[i]->GetXaxis()->SetTitle("Nightlies");
      hRy[i]->GetYaxis()->SetTitle(" Alignment corrections (mrad)");
      
      hRz[i]->GetYaxis()->SetLabelSize(0.03);
      hRz[i]->GetYaxis()->SetTitleOffset(1.3);
      hRz[i]->SetBinContent (j+1,Rz[i][j]);
      hRz[i] ->GetXaxis()->SetBinLabel(j+1,Nightlies[j]);
      hRz[i]->GetXaxis()->SetTitle("Nightlies");
      hRz[i]->SetTitle("Alignment Constants Rz");
      hRz[i]->GetYaxis()->SetTitle(" Alignment corrections (mrad)");
    }

    hTx[i]->GetYaxis()->SetRangeUser(-0.02,0.02);
    hTy[i]->GetYaxis()->SetRangeUser(-0.02,0.02);
    hTz[i]->GetYaxis()->SetRangeUser(-0.02,0.02);
    hRx[i]->GetYaxis()->SetRangeUser(-0.02,0.02);
    hRy[i]->GetYaxis()->SetRangeUser(-0.02,0.02);
    hRz[i]->GetYaxis()->SetRangeUser(-0.02,0.02);
  }  


  //Draw 
  TCanvas *Constants_L1=new TCanvas("Constants_L1","Constants_L1",1200,800);
  Constants_L1->SetFillColor(0);  
  Constants_L1->Divide(3,2);
   
  const int Colors[4]={2,3,4,6};
  const char Marker[4]={kStar,kFullCircle,kOpenSquare,kOpenCross};
  
  char colorTest = kStar;
  TLegend *legend = new TLegend(0.70, 0.75, 0.88, 0.89);
  legend->SetFillColor(10);
    
  for(int AlignmentConstant=1; AlignmentConstant<7; AlignmentConstant++){
    Constants_L1->cd(AlignmentConstant); // Tx  
    gPad->SetGridx();
    gPad->SetGridy();
    gStyle->SetOptStat(0);
   
    for (int j=0; j<struc;j++){
      if(AlignmentConstant==1)legend->AddEntry(hTx[j], Structures[j] , "P"); 
      gStyle->SetMarkerSize(0.8);
      hTx[j]->SetMarkerStyle(Marker[j]); hTy[j]->SetMarkerStyle(Marker[j]); hTz[j]->SetMarkerStyle(Marker[j]);
      hRx[j]->SetMarkerStyle(Marker[j]); hRy[j]->SetMarkerStyle(Marker[j]); hRz[j]->SetMarkerStyle(Marker[j]);
      hTx[j]->SetMarkerColor(Colors[j]);hTy[j]->SetMarkerColor(Colors[j]);hTz[j]->SetMarkerColor(Colors[j]);
      hRx[j]->SetMarkerColor(Colors[j]);hRy[j]->SetMarkerColor(Colors[j]);hRz[j]->SetMarkerColor(Colors[j]);
          
      if(AlignmentConstant==1 && j==0) hTx[j]->Draw("P"); legend->Draw();
      if(AlignmentConstant==1 && j!=0) hTx[j]->DrawCopy("same,P");
      if(AlignmentConstant==2 && j==0) hTy[j]->Draw("P");legend->Draw();
      if(AlignmentConstant==2 && j!=0) hTy[j]->DrawCopy("same,P");
      if(AlignmentConstant==3 && j==0) hTz[j]->Draw("P");legend->Draw();
      if(AlignmentConstant==3 && j!=0) hTz[j]->DrawCopy("same,P");
      if(AlignmentConstant==4 && j==0) hRx[j]->Draw("P");legend->Draw();
      if(AlignmentConstant==4 && j!=0) hRx[j]->DrawCopy("same,P");
      if(AlignmentConstant==5 && j==0) hRy[j]->Draw("P");legend->Draw();
      if(AlignmentConstant==5 && j!=0) hRy[j]->DrawCopy("same,P");
      if(AlignmentConstant==6 && j==0) hRz[j]->Draw("P");legend->Draw();
      if(AlignmentConstant==6 && j!=0) hRz[j]->DrawCopy("same,P");
    }
  }
  

  Constants_L1->Update();
  Constants_L1->Print("Constants_RelX.eps", "eps");
  Constants_L1->Print("Constants_RelX.png", "png");
  
}
