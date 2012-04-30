/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void BeamDetectors(){

  gROOT->Reset();

   gStyle->SetOptStat(111111);     

   SC = new TCanvas("scint", "Scintillators", 0, 0, 800, 800);
   SC->Divide(2,3);
            
   SC->cd(1); t->Draw("S1cou", Phys);      
   SC->cd(2); t->Draw("S2cou", Phys);   
   SC->cd(3); t->Draw("S3cou", Phys);      
   SC->cd(4); t->Draw("MuTag", Phys);      
   SC->cd(5); t->Draw("Trig");

   bc = new TCanvas("Beam_Chambers", "Beam Chambers", 10 ,10, 810, 810);
   bc->Divide(2,2);
   
   bc->cd(1);
   t->Draw("Xcha0:Ycha0", Phys); htemp->SetTitle(title);
   bc->cd(2);
   t->Draw("Xcha1:Ycha1", Phys); htemp->SetTitle("BC1");
   bc->cd(3);
   t->Draw("Xcha2:Ycha2", Phys); htemp->SetTitle("BC2");   
   bc->cd(4);
   t->Draw("XchN1:YchN1", Phys); htemp->SetTitle("BC-1");   
  
}
