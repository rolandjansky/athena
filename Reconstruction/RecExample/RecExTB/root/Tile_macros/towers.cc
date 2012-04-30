/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <TileDef.h>


void towers(){

   tile->AddFriend("TB/tree","../run/ntuple.root");        
   TCut Phys = "Trig==1 ";

   Float_t EtotTowers[27]={0}, Towers[27]={0};  
   c1 = new TCanvas("towers");
   if (tile->Draw(T_10, Phys && (a0||a1||a2)) !=0 ) EtotTowers[0]=htemp->GetMean()/1000; 
   if (tile->Draw(T_9, Phys && (a0||a1||a2)) !=0 ) EtotTowers[1]=htemp->GetMean()/1000; 
   if (tile->Draw(T_8, Phys && (a0||a1||a2)) !=0 ) EtotTowers[2]=htemp->GetMean()/1000;   
   if (tile->Draw(T_7, Phys && (a0||a1||a2)) !=0 ) EtotTowers[3]=htemp->GetMean()/1000; 
   if (tile->Draw(T_6, Phys && (a0||a1||a2)) !=0 ) EtotTowers[4]=htemp->GetMean()/1000; 
   if (tile->Draw(T_5, Phys && (a0||a1||a2)) !=0 ) EtotTowers[5]=htemp->GetMean()/1000;  
   if (tile->Draw(T_4, Phys && (a0||a1||a2)) !=0 ) EtotTowers[6]=htemp->GetMean()/1000; 
   if (tile->Draw(T_3, Phys && (a0||a1||a2)) !=0 ) EtotTowers[7]=htemp->GetMean()/1000; 
   if (tile->Draw(T_2, Phys && (a0||a1||a2)) !=0 ) EtotTowers[8]=htemp->GetMean()/1000;  
   if (tile->Draw(T_1, Phys && (a0||a1||a2)) !=0 ) EtotTowers[9]=htemp->GetMean()/1000;  
   if (tile->Draw(T1, Phys) !=0 ) EtotTowers[10]=htemp->GetMean()/1000;
   if (tile->Draw(T2, Phys) !=0 ) EtotTowers[11]=htemp->GetMean()/1000;
   if (tile->Draw(T3, Phys) !=0 ) EtotTowers[12]=htemp->GetMean()/1000;   
   if (tile->Draw(T4, Phys) !=0 ) EtotTowers[13]=htemp->GetMean()/1000; 
   if (tile->Draw(T5, Phys) !=0 ) EtotTowers[14]=htemp->GetMean()/1000; 
   if (tile->Draw(T6, Phys) !=0 ) EtotTowers[15]=htemp->GetMean()/1000;  
   if (tile->Draw(T7, Phys) !=0 ) EtotTowers[16]=htemp->GetMean()/1000; 
   if (tile->Draw(T8, Phys) !=0 ) EtotTowers[17]=htemp->GetMean()/1000; 
   if (tile->Draw(T9, Phys) !=0 ) EtotTowers[18]=htemp->GetMean()/1000;  
   if (tile->Draw(T10, Phys) !=0 ) EtotTowers[19]=htemp->GetMean()/1000;  
   if (tile->Draw(T11, Phys && (e0||e1||e2)) !=0 ) EtotTowers[20]=htemp->GetMean()/1000;
   if (tile->Draw(T12, Phys && (e0||e1||e2)) !=0 ) EtotTowers[21]=htemp->GetMean()/1000;
   if (tile->Draw(T13, Phys && (e0||e1||e2)) !=0 ) EtotTowers[22]=htemp->GetMean()/1000;  
   if (tile->Draw(T14, Phys && (e0||e1||e2)) !=0 ) EtotTowers[23]=htemp->GetMean()/1000;
   if (tile->Draw(T15, Phys && (e0||e1||e2)) !=0 ) EtotTowers[24]=htemp->GetMean()/1000;
   if (tile->Draw(T16, Phys && (e0||e1||e2)) !=0 ) EtotTowers[25]=htemp->GetMean()/1000;
   
   c1 = new TCanvas("towers","Mean energy in towers",20,20,820,820);
   c1->SetGrid();
   char title1[200]={0};
   sprintf(title1,"Mean energy in eta (GeV)");

   for (Int_t i=0; i<27; i++){ Towers[i]= 0.1*(i-10) +0.05;}
   
   draw = new TGraph(27,Towers, EtotTowers);
   draw->Draw("APL");
   draw->SetTitle(title1);
   draw->SetMarkerStyle(21);
   draw->SetMarkerColor(4);    
   c1->Update(); 

}
