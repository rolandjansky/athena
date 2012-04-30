/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <TileDef.h>

void energy_in_tile(){

   tile->AddFriend("TB/tree","../run/ntuple.root");
   
   TCut Phys = "Trig==1";   
   c1 = new TCanvas("energy","Total energy in tile (pi & e)",20,20,820,820);
   c1->Divide(1,2);
   c1->cd(1); 
   if( tile->Draw("ecell/1000", Phys && "MuTag<500")!=0) { 
      htemp->SetTitle("Energy in tile #pi and e");
      htemp->SetFillColor(4);
   }
   c1->cd(2); 
   if( tile->Draw("ecell/1000", Phys && "MuTag>500")!=0) { 
      htemp->SetTitle("Energy in tile #mu");
      htemp->SetFillColor(4);
   }  
}    
