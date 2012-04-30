/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <TileDef.h>


void drawers(){
   
   tile->AddFriend("TB/tree","../run/ntuple.root");   
   TCut Phys = "Trig==1";

   Float_t EtotDrawers[9]={0}, Drawers[9]={0,1,2,3,4,5,6,7,8};  
   c1 = new TCanvas("drawers");
   
   if (tile->Draw(A0, Phys && a0) !=0 ) EtotDrawers[0]=htemp->GetMean()/1000; 
   if (tile->Draw(A1, Phys && a1) !=0 ) EtotDrawers[1]=htemp->GetMean()/1000; 
   if (tile->Draw(A2, Phys && a2) !=0 ) EtotDrawers[2]=htemp->GetMean()/1000;   
   if (tile->Draw(C0, Phys) !=0 ) EtotDrawers[3]=htemp->GetMean()/1000; 
   if (tile->Draw(C1, Phys) !=0 ) EtotDrawers[4]=htemp->GetMean()/1000; 
   if (tile->Draw(C2, Phys) !=0 ) EtotDrawers[5]=htemp->GetMean()/1000;  
   if (tile->Draw(E0, Phys && e0) !=0 ) EtotDrawers[6]=htemp->GetMean()/1000; 
   if (tile->Draw(E1, Phys && e1) !=0 ) EtotDrawers[7]=htemp->GetMean()/1000; 
   if (tile->Draw(E2, Phys && e2) !=0 ) EtotDrawers[8]=htemp->GetMean()/1000;  
 
   c1 = new TCanvas("drawers","Mean energy in drawers",20,20,820,820);
   c1->SetGrid();
   char title1[200]={0};
   sprintf(title1,"Mean energy in drawers (GeV)");
   Float_t drawer[9]={0};   
   draw = new TGraph(9,Drawers, EtotDrawers);
   draw->Draw("AP");
   draw->SetTitle(title1);
   draw->SetMarkerStyle(21);
   draw->SetMarkerColor(4);    
   c1->Update(); 

}
