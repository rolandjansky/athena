/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

{
  TFile* file = TFile::Open("LArEM_SF.root");
  new TCanvas("Etot","Etot");
  mytree->Draw("Sum$(energy_active_total+energy_inactive_total)");

  new TCanvas("ELAr_hit","ELAr_hit");
  mytree->Draw("(energy_hit[0]+energy_hit[1]+energy_hit[2]+energy_hit[3])");

  //new TCanvas("SF","SF");
  //mytree->Draw("Sum$(energy_active_total)/Sum$(energy_active_total+energy_inactive_total):mc_eta>>SF(14,0,1.4,20,0.15,0.25)","","colz");

  //new TCanvas("LAr_barrel","LAr_barrel");
  //mytree->Draw("(energy_active_total[0]+energy_active_total[1]+energy_active_total[2]+energy_active_total[3]):mc_eta>>LAr_barrel(56,0,1.4,50,7000,12000)","","colz");

  new TCanvas("SF_LAr_barrel_calibhit","SF_LAr_barrel_calibhit");
  mytree->Draw("(energy_active_total[0]+energy_active_total[1]+energy_active_total[2]+energy_active_total[3])/(energy_active_total[0]+energy_active_total[1]+energy_active_total[2]+energy_active_total[3]+energy_inactive_total[0]+energy_inactive_total[1]+energy_inactive_total[2]+energy_inactive_total[3]):mc_eta>>SF_LAr_barrel_calibhit(56,0,1.4,70,0.18,0.25)","","colz");
  TH2* SF_LAr_barrel_calibhit=(TH2*)gDirectory->Get("SF_LAr_barrel_calibhit");
  //ProfileX(const char* name = "_pfx", Int_t firstybin = 1, Int_t lastybin = -1, Option_t* option = "")
  TProfile* SF_prof_calibhit=SF_LAr_barrel_calibhit->ProfileX();
  SF_prof_calibhit->SetLineColor(2);
  SF_prof_calibhit->Draw("same");
  //TFitResultPtr* fitres=
  SF_prof_calibhit->Fit("pol0","","",0.1,0.75);
  SF_prof_calibhit->Fit("pol0","+","",0.85,1.35);
  
  new TCanvas("SF_LAr_barrel","SF_LAr_barrel");
  mytree->Draw("(energy_hit[0]+energy_hit[1]+energy_hit[2]+energy_hit[3])/(energy_active_total[0]+energy_active_total[1]+energy_active_total[2]+energy_active_total[3]+energy_inactive_total[0]+energy_inactive_total[1]+energy_inactive_total[2]+energy_inactive_total[3]):mc_eta>>SF_LAr_barrel(56,0,1.4,70,0.16,0.23)","","colz");
  TH2* SF_LAr_barrel=(TH2*)gDirectory->Get("SF_LAr_barrel");
  //ProfileX(const char* name = "_pfx", Int_t firstybin = 1, Int_t lastybin = -1, Option_t* option = "")
  TProfile* SF_prof=SF_LAr_barrel->ProfileX();
  SF_prof->SetLineColor(2);
  SF_prof->Draw("same");
  //TFitResultPtr* fitres=
  SF_prof->Fit("pol0","","",0.1,0.75);
  SF_prof->Fit("pol0","+","",0.85,1.35);
  
  //mytree->Print();
  
}
