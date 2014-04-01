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
  const int Struc = 4;
  const int rel=7;
  //Constants values
  double X,pTx,pTy,pTz,pRx,pRy,pRz;
  
  //Definim els vectors
  float Tx[rel][Struc],Ty[rel][Struc],Tz[rel][Struc],Rx[rel][Struc],Ry[rel][Struc],Rz[rel][Struc];
  
  for (int j=0; j<rel;j++){
    for (int i=0; i<Struc;i++){
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
		Tx[i][j]=pTx;
		Ty[i][j]=pTy;
		Tz[i][j]=pTz;
		Rx[i][j]=pRx;
		Ry[i][j]=pRy; 
		Rz[i][j]=pRz;
		cout << " Tx: "<< Tx[i][j] << " Ty: "<< Ty[i][j] << " Tz: "<< Tz[i][j] << " Rx: "<< Rx[i][j] <<" Ry: "<< Ry[i][j] <<" Rz: "<< Rz[i][j] <<endl;
     }
    relX.close();
    } 
    else cout<<" **** OutputsiAlignment.txt file doesn't exist in rel"<< i << endl;
  }      
   
  //HISTOGRAM:
  const char *Structures[4]={"PIX","SCTECC","SCTBA","SCTECA"};
  
  //Definim els histogrames:
  TH1F *hTx[rel];
 
  TH1F *hTy[rel];
  TH1F *hTz[rel];
  TH1F *hRx[rel];
  TH1F *hRy[rel];
  TH1F *hRz[rel];
    
  char name_Tx[200],name_Ty[200],name_Tz[200],name_Rx[200],name_Ry[200],name_Rz[200] ; 

  for (int i=0;i<rel;i++){  
    sprintf(name_Tx,"hTx_rel%d",i);
    sprintf(name_Ty,"hTy_rel%d",i);
    sprintf(name_Tz,"hTz_rel%d",i);
    sprintf(name_Rx,"hRx_rel%d",i);
    sprintf(name_Ry,"hRy_rel%d",i);
    sprintf(name_Rz,"hRz_rel%d",i);
    
    hTx[i]=new TH1F(name_Tx,name_Tx,4,0,4);
    hTy[i]=new TH1F(name_Ty,name_Ty,4,0,4);
    hTz[i]=new TH1F(name_Tz,name_Tz,4,0,4);
    hRx[i]=new TH1F(name_Rx,name_Ry,4,0,4);      
    hRy[i]=new TH1F(name_Ry,name_Rx,4,0,4);
    hRz[i]=new TH1F(name_Rz,name_Rz,4,0,4);  
    
    float Tx_MAX=0,Ty_MAX=0,Tz_MAX=0,Rx_MAX=0,Ry_MAX=0,Rz_MAX=0;

    for (int j=0;j<Struc;j++){       
      hTx[i]->SetBinContent(j+1,Tx[i][j]);
      hTx[i]->GetXaxis()->SetBinLabel(j+1,Structures[j]);
      hTx[i]->SetTitle("Alignment Constants Tx");
      if(fabs(Tx[i][j])>Tx_MAX) Tx_MAX=fabs(Tx[i][j]);
      hTy[i]->SetBinContent (j+1,Ty[i][j]);
      hTy[i] ->GetXaxis()->SetBinLabel(j+1,Structures[j]);
      hTy[i]->SetTitle("Alignment Constants Ty");
      if(fabs(Ty[i][j])>Ty_MAX) Ty_MAX=fabs(Ty[i][j]);
      hTz[i]->SetBinContent (j+1,Tz[i][j]);
      hTz[i] ->GetXaxis()->SetBinLabel(j+1,Structures[j]);
      hTz[i]->SetTitle("Alignment Constants Tz");
      if(fabs(Tz[i][j])>Tz_MAX) Tz_MAX=fabs(Tz[i][j]);
      hRx[i]->SetBinContent (j+1,Rx[i][j]);
      hRx[i] ->GetXaxis()->SetBinLabel(j+1,Structures[j]);
      hRx[i]->SetTitle("Alignment Constants Rx");
      if(fabs(Rx[i][j])>Rx_MAX) Rx_MAX=fabs(Rx[i][j]);
      hRy[i]->SetBinContent (j+1,Ry[i][j]);
      hRy[i] ->GetXaxis()->SetBinLabel(j+1,Structures[j]);
      hRy[i]->SetTitle("Alignment Constants Ry");
      if(fabs(Ry[i][j])>Ry_MAX) Ry_MAX=fabs(Ry[i][j]);
      hRz[i]->SetBinContent (j+1,Rz[i][j]);
      hRz[i] ->GetXaxis()->SetBinLabel(j+1,Structures[j]);
      hRz[i]->SetTitle("Alignment Constants Rz");
      if(fabs(Rz[i][j])>Rz_MAX) Rz_MAX=fabs(Rz[i][j]);
    }
    hTx[i]->GetYaxis()->SetRangeUser(-(Tx_MAX+0.1),Tx_MAX+0.1);
    hTy[i]->GetYaxis()->SetRangeUser(-(Ty_MAX+0.1),Ty_MAX+0.1);
    hTz[i]->GetYaxis()->SetRangeUser(-(Tz_MAX+0.1),Tz_MAX+0.1);
    hRx[i]->GetYaxis()->SetRangeUser(-(Rx_MAX+0.1),Rx_MAX+0.1);
    hRy[i]->GetYaxis()->SetRangeUser(-(Ry_MAX+0.1),Ry_MAX+0.1);
    hRz[i]->GetYaxis()->SetRangeUser(-(Rz_MAX+0.1),Rz_MAX+0.1);
  }  
  
  //Draw 
  TCanvas *Constants_L1=new TCanvas("Constants_L1","Constants_L1",600,400);
  Constants_L1->SetFillColor(0);  
  Constants_L1->Divide(3,2);

  Constants_L1->cd(1); // Tx  

  TLegend *legTx = new TLegend(0.84, 0.72, 0.99, 0.99);
  legTx->SetMargin(0.45);
  legTx->SetFillColor(10);
  
  gPad->SetFrameBorderMode(0);
  gPad->SetFillColor(kGreen-3);
  gPad->SetFillStyle(3002);   
  gPad->SetGridx();
  gPad->SetGridy(); 
  gPad->SetFrameFillColor(10);  
  
  
  
    
  char legend[200];  
  for (int i=0; i<rel;i++){
    hTx[i]->SetStats(kFALSE);
    hTx[i]->SetLineColor(i+1);
    hTx[i]->SetLineWidth(2);
    hTx[i]->SetMarkerColor(i+1);
    hTx[i]->SetMarkerStyle(7);   
    hTx[i]->SetMarkerSize(3);
   
    sprintf(legend,"rel%d",i);
      if(i==0){
	hTx[i]->Draw("LP");
	legTx->AddEntry(hTx[i], legend , "L");
      }
      else{ 
	hTx[i]->DrawCopy("same,LP");
	legTx->AddEntry(hTx[i], legend, "L");  
      }
      legTx->Draw("same");
  }

  Constants_L1->cd(2);
  gPad->SetFrameBorderMode(0);
  gPad->SetFillColor(kGreen-3);
  gPad->SetFillStyle(3002);   
  gPad->SetGridx();
  gPad->SetGridy(); 
  gPad->SetFrameFillColor(10);  
  
  TLegend *legTy = new TLegend(0.84, 0.72, 0.99, 0.99);   
  legTy->SetMargin(0.45);
  legTy->SetFillColor(10);
  
  for (int i=0; i<rel;i++){
    hTy[i]->SetStats(kFALSE);
    hTy[i]->SetLineColor(i+1);
    hTy[i]->SetLineWidth(2);
    hTy[i]->SetMarkerColor(i+1);
    hTy[i]->SetMarkerSize(3);
    hTy[i]->SetMarkerStyle(7);
    sprintf(legend,"rel%d",i);
    if(i==0){
      hTy[i]->Draw("LP");
      legTy->AddEntry(hTy[i], legend, "L");
    }
    else{ 
      hTy[i]->DrawCopy("same,LP");
      legTy->AddEntry(hTy[i], legend, "L");
    }    
    legTy->Draw("same");
  }

  Constants_L1->cd(3);
  gPad->SetFrameBorderMode(0);
  gPad->SetFillColor(kGreen-3);
  gPad->SetFillStyle(3002);   
  gPad->SetGridx();
  gPad->SetGridy(); 
  gPad->SetFrameFillColor(10);  

   TLegend *legTz = new TLegend(0.84, 0.72, 0.99, 0.99);   
  legTz->SetMargin(0.45);
  legTz->SetFillColor(10);
 
  for (int i=0; i<rel;i++){
    hTz[i]->SetStats(kFALSE);
    hTz[i]->SetLineColor(i+1);
    hTz[i]->SetLineWidth(2);
    hTz[i]->SetMarkerColor(i+1);
    hTz[i]->SetMarkerSize(3);
    hTz[i]->SetMarkerStyle(7);
    sprintf(legend,"rel%d",i); 
    if(i==0){
      hTz[i]->Draw("LP");
      legTz->AddEntry(hTz[i], legend, "L");
      
    }
    else{ 
      hTz[i]->DrawCopy("same,LP");
      legTz->AddEntry(hTz[i],legend, "L"); 
    } 
    legTz->Draw("same");
  }

  Constants_L1->cd(4);
  gPad->SetFrameBorderMode(0);
  gPad->SetFillColor(kGreen-3);
  gPad->SetFillStyle(3002);   
  gPad->SetGridx();
  gPad->SetGridy(); 
  gPad->SetFrameFillColor(10);  
  
  TLegend *legRx = new TLegend(0.84, 0.72, 0.99, 0.99);   
  legRx->SetMargin(0.45);  
  legRx->SetFillColor(10);

  for (int i=0; i<rel;i++){
    hRx[i]->SetStats(kFALSE);
    hRx[i]->SetLineColor(i+1);
    hRx[i]->SetLineWidth(2);
    hRx[i]->SetMarkerColor(i+1);
    hRx[i]->SetMarkerSize(3);
    hRx[i]->SetMarkerStyle(7);
    sprintf(legend,"rel%d",i);
    if(i==0){
      hRx[i]->Draw("LP");
      legRx->AddEntry(hRx[i], legend, "L");
    }
    else{ 
      hRx[i]->DrawCopy("same,LP");
      legRx->AddEntry(hRx[i], legend, "L");
    }
    legRx->Draw("same");
  }

  
  Constants_L1->cd(5);
  gPad->SetFrameBorderMode(0);
  gPad->SetFillColor(kGreen-3);
  gPad->SetFillStyle(3002);   
  gPad->SetGridx();
  gPad->SetGridy(); 
  gPad->SetFrameFillColor(10);  
  
  TLegend *legRy = new TLegend(0.84, 0.72, 0.99, 0.99);   
  legRy->SetMargin(0.45);
  legRy->SetFillColor(10);
   
  for (int i=0; i<rel;i++){
    hRy[i]->SetStats(kFALSE);
    hRy[i]->SetLineColor(i+1);
    hRy[i]->SetLineWidth(2);
    hRy[i]->SetMarkerColor(i+1);
    hRy[i]->SetMarkerSize(3);
    hRy[i]->SetMarkerStyle(7);
    sprintf(legend,"rel%d",i);
    if(i==0){
      hRy[i]->Draw("LP");
      legRy->AddEntry(hRy[i], legend, "L");
    }
    else{ 
      hRy[i]->DrawCopy("same,LP");
      legRy->AddEntry(hRy[i], legend, "L");
    }
    legRy->Draw("same");
  }

  Constants_L1->cd(6);
  gPad->SetFrameBorderMode(0);
  gPad->SetFillColor(kGreen-3);
  gPad->SetFillStyle(3002);   
  gPad->SetGridx();
  gPad->SetGridy(); 
  gPad->SetFrameFillColor(10);  
  
  TLegend *legRz = new TLegend(0.84, 0.72, 0.99, 0.99);   
  legRz->SetMargin(0.45);
  legRz->SetFillColor(10);

   
  for (int i=0; i<rel;i++){
    hRz[i]->SetStats(kFALSE);
    hRz[i]->SetLineColor(i+1);
    hRz[i]->SetLineWidth(2);
    hRz[i]->SetMarkerColor(i+1);
    hRz[i]->SetMarkerSize(3);
    hRz[i]->SetMarkerStyle(7);
    sprintf(legend,"rel%d",i);
    if(i==0){
      hRz[i]->Draw("LP");
      legRz->AddEntry(hRz[i], legend, "L");
    }
    else{ 
      hRz[i]->DrawCopy("same,LP");
      legRz->AddEntry(hRz[i], legend, "L");
    }
    legRz->Draw("same");
  }

  Constants_L1->Update();
  Constants_L1->Print("Constants_RelX.eps", "eps");
  Constants_L1->Print("Constants_RelX.png", "png");
  
}

//   hRz->SetMarkerColor(2);
//   hRz->SetMarkerSize(1);
//   hRz->SetMarkerStyle(24);
//   gPad->SetGridx();
//   gPad->SetGridy();
//   hrefRz->Draw("P");
//   hRz->Draw ("same,P");
//   hRz2->Draw ("same,P");

//   TLegend *leg = new TLegend(0.6, 0.8, 0.9, 0.95);
//   leg->AddEntry(hrefRz, "CSC Reference", "P");
//   leg->AddEntry(hRz, "Level18 IT0", "P");
//   leg->AddEntry(hRz2, "Level18 IT1", "P");
//   leg->Draw("same");
//   gPad->SetGridx();
//   gPad->SetGridy();


