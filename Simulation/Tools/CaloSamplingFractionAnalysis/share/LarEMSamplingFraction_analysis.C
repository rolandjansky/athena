/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

void LarEMSamplingFraction_barrel()
{
  TFile* file = TFile::Open("LArEM_SF_barrel.root");
  TTree* mytree=(TTree*)file->Get("mytree");
  new TCanvas("Etot_barrel","Etot_barrel");
  mytree->Draw("Sum$(energy_active_total+energy_inactive_total)");

  new TCanvas("ELAr_hit_barrel","ELAr_hit_barrel");
  mytree->Draw("(energy_hit[0]+energy_hit[1]+energy_hit[2]+energy_hit[3])");

  TCanvas* c;
  c=new TCanvas("SF_LAr_barrel_calibhit","SF_LAr_barrel_calibhit");
  mytree->Draw("(energy_active_total[0]+energy_active_total[1]+energy_active_total[2]+energy_active_total[3])/(energy_active_total[0]+energy_active_total[1]+energy_active_total[2]+energy_active_total[3]+energy_inactive_total[0]+energy_inactive_total[1]+energy_inactive_total[2]+energy_inactive_total[3]):mc_eta>>SF_LAr_barrel_calibhit(56,0,1.4,140,0.18,0.25)","","colz");
  TH2* SF_LAr_barrel_calibhit=(TH2*)gDirectory->Get("SF_LAr_barrel_calibhit");
  //ProfileX(const char* name = "_pfx", Int_t firstybin = 1, Int_t lastybin = -1, Option_t* option = "")
  TProfile* SF_prof_calibhit=SF_LAr_barrel_calibhit->ProfileX();
  SF_prof_calibhit->SetLineColor(2);
  SF_prof_calibhit->Draw("same");
  //TFitResultPtr* fitres=
  //SF_prof_calibhit->Fit("pol0","","",0.1,0.75);
  //SF_prof_calibhit->Fit("pol0","+","",0.85,1.35);
  c->SetGridx();
  c->SetGridy();
  
  c=new TCanvas("SF_LAr_barrel","SF_LAr_barrel");
  mytree->Draw("(energy_hit[0]+energy_hit[1]+energy_hit[2]+energy_hit[3])/(energy_active_total[0]+energy_active_total[1]+energy_active_total[2]+energy_active_total[3]+energy_inactive_total[0]+energy_inactive_total[1]+energy_inactive_total[2]+energy_inactive_total[3]):mc_eta>>SF_LAr_barrel(56,0,1.4,140,0.16,0.23)","","colz");
  TH2* SF_LAr_barrel=(TH2*)gDirectory->Get("SF_LAr_barrel");
  SF_LAr_barrel->SetStats(0);
  SF_LAr_barrel->SetTitle("Sampling fraction LAr EM barrel");
  SF_LAr_barrel->GetXaxis()->SetTitle("|#eta|");
  SF_LAr_barrel->GetYaxis()->SetTitle("E_{G4hit}/E_{total}");
  //ProfileX(const char* name = "_pfx", Int_t firstybin = 1, Int_t lastybin = -1, Option_t* option = "")
  TProfile* SF_prof=SF_LAr_barrel->ProfileX();
  SF_prof->SetLineColor(2);
  SF_prof->Draw("same");
  SF_prof->Fit("pol0","Q","",0.1,0.75);
  SF_prof->Fit("pol0","Q+","",0.85,1.35);
  TList* res=SF_prof->GetListOfFunctions();

  TPaveText *pt = new TPaveText(.05,.2,.75,.23,"");
  pt->SetFillStyle(1001);
  pt->SetFillColor(10);
  pt->SetBorderSize(1);
  pt->AddText("Sampling fractions LAr EM barrel");

  for(int i=0;i<res->GetSize();++i) if(res->At(i)->InheritsFrom(TF1::Class())) {
    TF1* func=(TF1*)res->At(i);
    TString text=Form("%4.2f < #eta < %4.2f = %7.5f +- %7.5f",func->GetXmin(),func->GetXmax(),func->GetParameter(0),func->GetParError(0));
    //cout<<"i="<<i<<" "<<res->At(i)->ClassName()<<" : "<<res->At(i)->GetName()<<endl;
    std::cout<<"Sampling fraction : "<<text<<std::endl;
    pt->AddText(text);
  }
  
  pt->Draw();
   
  
  c->SetGridx();
  c->SetGridy();
}

void LarEMSamplingFraction_endcap()
{
  TFile* file = TFile::Open("LArEM_SF_endcap.root");
  TTree* mytree=(TTree*)file->Get("mytree");
  new TCanvas("Etot_endcap","Etot_endcap");
  mytree->Draw("Sum$(energy_active_total+energy_inactive_total)");

  new TCanvas("ELAr_hit_endcap","ELAr_hit_endcap");
  mytree->Draw("(energy_hit[4]+energy_hit[5]+energy_hit[6]+energy_hit[7])");

  TCanvas* c;
  c=new TCanvas("SF_LAr_endcap_calibhit","SF_LAr_endcap_calibhit");
  mytree->Draw("(energy_active_total[4]+energy_active_total[5]+energy_active_total[6]+energy_active_total[7])/(energy_active_total[4]+energy_active_total[5]+energy_active_total[6]+energy_active_total[7]+energy_inactive_total[4]+energy_inactive_total[5]+energy_inactive_total[6]+energy_inactive_total[7]):mc_eta>>SF_LAr_endcap_calibhit(86,1.35,3.5,400,0.00,0.40)","","colz");
  TH2* SF_LAr_endcap_calibhit=(TH2*)gDirectory->Get("SF_LAr_endcap_calibhit");
  //ProfileX(const char* name = "_pfx", Int_t firstybin = 1, Int_t lastybin = -1, Option_t* option = "")
  TProfile* SF_prof_calibhit=SF_LAr_endcap_calibhit->ProfileX();
  SF_prof_calibhit->SetLineColor(2);
  SF_prof_calibhit->Draw("same");
  //TFitResultPtr* fitres=
  //SF_prof_calibhit->Fit("pol0","","",0.1,0.75);
  //SF_prof_calibhit->Fit("pol0","+","",0.85,1.35);
  c->SetGridx();
  c->SetGridy();
  
  c=new TCanvas("SF_LAr_endcap","SF_LAr_endcap");
  mytree->Draw("(energy_hit[4]+energy_hit[5]+energy_hit[6]+energy_hit[7])/(energy_active_total[4]+energy_active_total[5]+energy_active_total[6]+energy_active_total[7]+energy_inactive_total[4]+energy_inactive_total[5]+energy_inactive_total[6]+energy_inactive_total[7]):mc_eta>>SF_LAr_endcap(86,1.35,3.5,800,0.00,0.40)","","colz");
  TH2* SF_LAr_endcap=(TH2*)gDirectory->Get("SF_LAr_endcap");
  SF_LAr_endcap->SetStats(0);
  SF_LAr_endcap->SetTitle("Sampling fraction LAr EM endcap");
  SF_LAr_endcap->GetXaxis()->SetTitle("|#eta|");
  SF_LAr_endcap->GetYaxis()->SetTitle("E_{G4hit}/E_{total}");
  SF_LAr_endcap->GetYaxis()->SetRangeUser(0,0.15);
  //ProfileX(const char* name = "_pfx", Int_t firstybin = 1, Int_t lastybin = -1, Option_t* option = "")
  TProfile* SF_prof=SF_LAr_endcap->ProfileX();
  SF_prof->SetLineColor(2);
  SF_prof->Draw("same");
  //TFitResultPtr* fitres=
  //SF_prof->Fit("pol0","","",0.1,0.75);
  //SF_prof->Fit("pol0","+","",0.85,1.35);
  c->SetGridx();
  c->SetGridy();
}

void LarEMSamplingFraction_analysis()
{
  LarEMSamplingFraction_barrel();
  LarEMSamplingFraction_endcap();
}
