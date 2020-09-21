/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

void LarEMSamplingFraction_barrel(std::string filebarrel="LArEM_SF_barrel.root")
{
  TFile* file = TFile::Open(filebarrel.c_str());
  if(!file || !file->IsOpen()) {
    std::cout<<filebarrel<<" not found or corrupt"<<std::endl;
    return;
  }
  TTree* mytree=(TTree*)file->Get("mytree");
  new TCanvas("Etot_barrel","Etot_barrel");
  mytree->Draw("Sum$(energy_active_total+energy_inactive_total)");

  new TCanvas("ELAr_hit_barrel","ELAr_hit_barrel");
  mytree->Draw("(energy_hit[0]+energy_hit[1]+energy_hit[2]+energy_hit[3])");

  TCanvas* c;
  c=new TCanvas("SF_LAr_barrel_calibhit","SF_LAr_barrel_calibhit");
  mytree->Draw("(energy_active_em[1]+energy_active_em[2]+energy_active_em[3]+energy_active_nonem[1]+energy_active_nonem[2]+energy_active_nonem[3] + energy_inactive_em[1]+energy_inactive_em[2]+energy_inactive_em[3]+energy_inactive_nonem[1]+energy_inactive_nonem[2]+energy_inactive_nonem[3])/mc_e:mc_eta>>SF_LAr_barrel_calibhit(56,0,1.4,300,0.7,1.0)","","colz");
  TH2* SF_LAr_barrel_calibhit=(TH2*)gDirectory->Get("SF_LAr_barrel_calibhit");
  //ProfileX(const char* name = "_pfx", Int_t firstybin = 1, Int_t lastybin = -1, Option_t* option = "")
  TProfile* SF_prof_calibhit=SF_LAr_barrel_calibhit->ProfileX();
  
  SF_LAr_barrel_calibhit->GetYaxis()->SetRangeUser(0.96,1.0);
  
  SF_prof_calibhit->SetLineColor(2);
  SF_prof_calibhit->Draw("same");

  c->SetGridx();
  c->SetGridy();
  
  c=new TCanvas("SF_LAr_barrel_hit","SF_LAr_barrel_hit");
  mytree->Draw("(energy_hit[1]+energy_hit[2]+energy_hit[3])/mc_e:mc_eta>>SF_LAr_barrel_hit(56,0,1.4,300,0.13,0.28)","","colz");
  TH2* SF_LAr_barrel_hit=(TH2*)gDirectory->Get("SF_LAr_barrel_hit");
  TProfile* SF_prof_hit=SF_LAr_barrel_hit->ProfileX();
  
  SF_LAr_barrel_hit->GetYaxis()->SetRangeUser(0.96,1.0);
  
  SF_prof_hit->SetLineColor(2);
  SF_prof_hit->Draw("same");

  c->SetGridx();
  c->SetGridy();
  
  c=new TCanvas("SF_LAr_barrel","SF_LAr_barrel");
  mytree->Draw("(energy_hit[1]+energy_hit[2]+energy_hit[3])/(energy_active_em[1]+energy_active_em[2]+energy_active_em[3]+energy_active_nonem[1]+energy_active_nonem[2]+energy_active_nonem[3] + energy_inactive_em[1]+energy_inactive_em[2]+energy_inactive_em[3]+energy_inactive_nonem[1]+energy_inactive_nonem[2]+energy_inactive_nonem[3]):mc_eta>>SF_LAr_barrel(56,0,1.4,300,0.13,0.28)","","colz");
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

  TProfile* SF_prof_noncorr=(TProfile*)(SF_prof_hit->Clone("SF_prof_noncorr"));
  SF_prof_noncorr->Divide(SF_prof_calibhit);
  SF_prof_noncorr->SetLineColor(1);
  SF_prof_noncorr->Draw("samehist");

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
  c->SaveAs(".pdf");
}

void LarEMSamplingFraction_endcap(std::string fileendcap="LArEM_SF_endcap.root")
{
  TFile* file = TFile::Open(fileendcap.c_str());
  if(!file || !file->IsOpen()) {
    std::cout<<fileendcap<<" not found or corrupt"<<std::endl;
    return;
  }
  TTree* mytree=(TTree*)file->Get("mytree");

  new TCanvas("ELAr_hit_endcap","ELAr_hit_endcap");
  mytree->Draw("(energy_hit[5]+energy_hit[6]+energy_hit[7])/mc_e:mc_eta>>ELAr_hit_endcap(86,1.35,3.5,150,0,0.15)","","colz");

  TCanvas* c;
  c=new TCanvas("SF_LAr_endcap_calibhit","SF_LAr_endcap_calibhit");
  mytree->Draw("(energy_active_em[5]+energy_active_em[6]+energy_active_em[7]+energy_active_nonem[5]+energy_active_nonem[6]+energy_active_nonem[7] + energy_inactive_em[5]+energy_inactive_em[6]+energy_inactive_em[7]+energy_inactive_nonem[5]+energy_inactive_nonem[6]+energy_inactive_nonem[7])/mc_e:mc_eta>>SF_LAr_endcap_calibhit(86,1.35,3.5,100,0.00,1.00)","","colz");
  TH2* SF_LAr_endcap_calibhit=(TH2*)gDirectory->Get("SF_LAr_endcap_calibhit");
  TProfile* SF_prof_calibhit=SF_LAr_endcap_calibhit->ProfileX();
  SF_prof_calibhit->SetLineColor(2);
  SF_prof_calibhit->Draw("same");

  c->SetGridx();
  c->SetGridy();
  
  c=new TCanvas("SF_LAr_endcap","SF_LAr_endcap");
  mytree->Draw("(energy_hit[5]+energy_hit[6]+energy_hit[7])/(energy_active_em[5]+energy_active_em[6]+energy_active_em[7]+energy_active_nonem[5]+energy_active_nonem[6]+energy_active_nonem[7] + energy_inactive_em[5]+energy_inactive_em[6]+energy_inactive_em[7]+energy_inactive_nonem[5]+energy_inactive_nonem[6]+energy_inactive_nonem[7]):mc_eta>>SF_LAr_endcap(86,1.35,3.5,800,0.00,0.40)","","colz");
  TH2* SF_LAr_endcap=(TH2*)gDirectory->Get("SF_LAr_endcap");
  SF_LAr_endcap->SetStats(0);
  SF_LAr_endcap->SetTitle("Sampling fraction LAr EM endcap");
  SF_LAr_endcap->GetXaxis()->SetTitle("|#eta|");
  SF_LAr_endcap->GetYaxis()->SetTitle("E_{G4hit}/E_{total}");
  SF_LAr_endcap->GetYaxis()->SetRangeUser(0,0.13);
  TProfile* SF_prof=SF_LAr_endcap->ProfileX();
  SF_prof->SetLineColor(2);
  SF_prof->Draw("same");
  
  SF_prof->Fit(new TF1("r1","[0]+(x-1.47)*[1]",1,4),"","",1.45,1.49);
  SF_prof->Fit(new TF1("r2","[0]+(x-1.55)*[1]",1,4),"+","",1.53,1.57);
  SF_prof->Fit(new TF1("r3","[0]+(x-1.70)*[1]",1,4),"+","",1.64,1.76);
  SF_prof->Fit(new TF1("r4","[0]+(x-1.90)*[1]",1,4),"+","",1.84,1.96);
  SF_prof->Fit(new TF1("r5","[0]+(x-2.055)*[1]",1,4),"+","",2.03,2.08);
  SF_prof->Fit(new TF1("r6","[0]+(x-2.20)*[1]",1,4),"+","",2.14,2.26);
  SF_prof->Fit(new TF1("r7","[0]+(x-2.395)*[1]",1,4),"+","",2.34,2.45);
  SF_prof->Fit(new TF1("r8","[0]+(x-2.66)*[1]",1,4),"+","",2.56,2.76);
  SF_prof->Fit(new TF1("r9","[0]+(x-2.995)*[1]",1,4),"+","",2.85,3.14);
  TList* res=SF_prof->GetListOfFunctions();

  TPaveText *pt = new TPaveText(1.45,0.005,3.1,0.075,"");
  pt->SetFillStyle(1001);
  pt->SetFillColor(10);
  pt->SetBorderSize(1);
  pt->AddText("Sampling fractions LAr EM endcap");

  for(int i=0;i<res->GetSize();++i) if(res->At(i)->InheritsFrom(TF1::Class())) {
    TF1* func=(TF1*)res->At(i);
    TString text=Form("%4.2f < #eta < %4.2f = %+7.5f +- %7.5f + (|#eta|-%5.3f)*( %7.5f +- %7.5f )",func->GetXmin(),func->GetXmax(),func->GetParameter(0),func->GetParError(0),0.5*(func->GetXmin()+func->GetXmax()),func->GetParameter(1),func->GetParError(1));
    std::cout<<"Sampling fraction : "<<text<<std::endl;
    pt->AddText(text);
    if(func->GetFormula()) {
    }
  }
  
  pt->Draw();

  c->SetGridx();
  c->SetGridy();
  c->SaveAs(".pdf");
}

void LarEMSamplingFraction_analysis(std::string filebarrel="LArEM_SF_barrel.root", std::string fileendcap="LArEM_SF_endcap.root")
{
  LarEMSamplingFraction_barrel(filebarrel);
  LarEMSamplingFraction_endcap(fileendcap);
}
