{
// Run trackeff.cpp first

TFile f1("JpsiVal_newL2Star.aan.root");
//TFile f1("JpsiVal.aan.root");

 TFile fout("TrackEff_newL2Star.root","RECREATE");

TH1F *hpt_muplus = (TH1F*)f1.Get("Jpsiphys/hpt_muplus");
TH1F *hpt_muplus_inroi = (TH1F*)f1.Get("Jpsiphys/hpt_muplus_inroi");
TH1F *hpt_muplus_tr = (TH1F*)f1.Get("Jpsiphys/hpt_muplus_tr");
TH1F *heta_muplus = (TH1F*)f1.Get("Jpsiphys/heta_muplus");
TH1F *heta_muplus_inroi = (TH1F*)f1.Get("Jpsiphys/heta_muplus_inroi");
TH1F *heta_muplus_tr = (TH1F*)f1.Get("Jpsiphys/heta_muplus_tr");
TH1F *hphi_muplus = (TH1F*)f1.Get("Jpsiphys/hphi_muplus");
TH1F *hphi_muplus_inroi = (TH1F*)f1.Get("Jpsiphys/hphi_muplus_inroi");
TH1F *hphi_muplus_tr = (TH1F*)f1.Get("Jpsiphys/hphi_muplus_tr");

TH1F *hpt_muminus = (TH1F*)f1.Get("Jpsiphys/hpt_muminus");
TH1F *hpt_muminus_inroi = (TH1F*)f1.Get("Jpsiphys/hpt_muminus_inroi");
TH1F *hpt_muminus_tr = (TH1F*)f1.Get("Jpsiphys/hpt_muminus_tr");
TH1F *heta_muminus = (TH1F*)f1.Get("Jpsiphys/heta_muminus");
TH1F *heta_muminus_inroi = (TH1F*)f1.Get("Jpsiphys/heta_muminus_inroi");
TH1F *heta_muminus_tr = (TH1F*)f1.Get("Jpsiphys/heta_muminus_tr");
TH1F *hphi_muminus = (TH1F*)f1.Get("Jpsiphys/hphi_muminus");
TH1F *hphi_muminus_inroi = (TH1F*)f1.Get("Jpsiphys/hphi_muminus_inroi");
TH1F *hphi_muminus_tr = (TH1F*)f1.Get("Jpsiphys/hphi_muminus_tr");

//hpt_muplus->Draw();
TH1F *hpt_mu = (TH1F*)hpt_muplus->Clone();
hpt_mu->SetName("hpt_mu");
hpt_mu->Add(hpt_muminus);
TH1F *hpt_mu_inroi = (TH1F*)hpt_muplus_inroi->Clone();
hpt_mu_inroi->SetName("hpt_mu_inroi");
hpt_mu_inroi->Add(hpt_muminus_inroi);
TH1F *hpt_mu_tr = (TH1F*)hpt_muplus_tr->Clone();
hpt_mu_tr->SetName("hpt_mu_tr");
hpt_mu_tr->Add(hpt_muminus_tr);
TH1F *heta_mu = (TH1F*)heta_muplus->Clone();
heta_mu->SetName("heta_mu");
heta_mu->Add(heta_muminus);
TH1F *heta_mu_inroi = (TH1F*)heta_muplus_inroi->Clone();
heta_mu_inroi->SetName("heta_mu_inroi");
heta_mu_inroi->Add(heta_muminus_inroi);
TH1F *heta_mu_tr = (TH1F*)heta_muplus_tr->Clone();
heta_mu_tr->SetName("heta_mu_tr");
heta_mu_tr->Add(heta_muminus_tr);
TH1F *hphi_mu = (TH1F*)hphi_muplus->Clone();
hphi_mu->SetName("hphi_mu");
hphi_mu->Add(hphi_muminus);
TH1F *hphi_mu_inroi = (TH1F*)hphi_muplus_inroi->Clone();
hphi_mu_inroi->SetName("hphi_mu_inroi");
hphi_mu_inroi->Add(hphi_muminus_inroi);
TH1F *hphi_mu_tr = (TH1F*)hphi_muplus_tr->Clone();
hphi_mu_tr->SetName("hphi_mu_tr");
hphi_mu_tr->Add(hphi_muminus_tr);


const Int_t NBINS_PT=16;
const Int_t NBINS_ETA=20;
const Int_t NBINS_PHI=20;
Stat_t  Pt_Mu[NBINS_PT],Pt_Mu_tr[NBINS_PT];
Stat_t  Pt_Muplus[NBINS_PT],Pt_Muplus_tr[NBINS_PT];
Stat_t  Pt_Muminus[NBINS_PT],Pt_Muminus_tr[NBINS_PT];
 Stat_t  Pt_Mu0[NBINS_PT];
 Stat_t  Pt_Muplus0[NBINS_PT];
 Stat_t  Pt_Muminus0[NBINS_PT];
Float_t Pt_eff_Mu0[NBINS_PT],Pt_eff_Muplus0[NBINS_PT],Pt_eff_Muminus0[NBINS_PT];
Float_t Pt_eff_Mu[NBINS_PT],Pt_eff_Muplus[NBINS_PT],Pt_eff_Muminus[NBINS_PT];
Stat_t Pt_err_Mu0[NBINS_PT],Pt_err_Muplus0[NBINS_PT],Pt_err_Muminus0[NBINS_PT];
Stat_t Pt_err_Mu[NBINS_PT],Pt_err_Muplus[NBINS_PT],Pt_err_Muminus[NBINS_PT];

for (Int_t i=0; i<NBINS_PT ; i++) {
  Pt_Mu0[i] = hpt_mu->GetBinContent(i+1);
  Pt_Muplus0[i] = hpt_muplus->GetBinContent(i+1);
  Pt_Muminus0[i] = hpt_muminus->GetBinContent(i+1);

  Pt_Mu[i] = hpt_mu_inroi->GetBinContent(i+1);
  Pt_Mu_tr[i] = hpt_mu_tr->GetBinContent(i+1);
  Pt_Muplus[i] = hpt_muplus_inroi->GetBinContent(i+1);
  Pt_Muplus_tr[i] = hpt_muplus_tr->GetBinContent(i+1);
  Pt_Muminus[i] = hpt_muminus_inroi->GetBinContent(i+1);
  Pt_Muminus_tr[i] = hpt_muminus_tr->GetBinContent(i+1);

  if (Pt_Mu0[i] != 0) {
    Pt_eff_Mu0[i] = Pt_Mu[i]/Pt_Mu0[i];
    Pt_err_Mu0[i] = sqrt(Pt_eff_Mu0[i]*(1.-Pt_eff_Mu0[i])/Pt_Mu0[i]);
  } else {
    Pt_eff_Mu0[i] = 0.;
    Pt_err_Mu0[i] = 0.;
  }
  if (Pt_Muplus0[i] != 0) {
    Pt_eff_Muplus0[i] = Pt_Muplus[i]/Pt_Muplus0[i];
    Pt_err_Muplus0[i] = sqrt(Pt_eff_Muplus0[i]*(1.-Pt_eff_Muplus0[i])/Pt_Muplus0[i]);
  } else {
    Pt_eff_Muplus0[i] = 0.;
    Pt_err_Muplus0[i] = 0.;
  }
  if (Pt_Muminus0[i] != 0) {
    Pt_eff_Muminus0[i] = Pt_Muminus[i]/Pt_Muminus0[i];
    Pt_err_Muminus0[i] = sqrt(Pt_eff_Muminus0[i]*(1.-Pt_eff_Muminus0[i])/Pt_Muminus0[i]);
  } else {
    Pt_eff_Muminus0[i] = 0.;
    Pt_err_Muminus0[i] = 0.;
  }

  if (Pt_Mu[i] != 0) {
    Pt_eff_Mu[i] = Pt_Mu_tr[i]/Pt_Mu[i];
    Pt_err_Mu[i] = sqrt(Pt_eff_Mu[i]*(1.-Pt_eff_Mu[i])/Pt_Mu[i]);
  } else {
    Pt_eff_Mu[i] = 0.;
    Pt_err_Mu[i] = 0.;
  }
  if (Pt_Muplus[i] != 0) {
    Pt_eff_Muplus[i] = Pt_Muplus_tr[i]/Pt_Muplus[i];
    Pt_err_Muplus[i] = sqrt(Pt_eff_Muplus[i]*(1.-Pt_eff_Muplus[i])/Pt_Muplus[i]);
  } else {
    Pt_eff_Muplus[i] = 0.;
    Pt_err_Muplus[i] = 0.;
  }
  if (Pt_Muminus[i] != 0) {
    Pt_eff_Muminus[i] = Pt_Muminus_tr[i]/Pt_Muminus[i];
    Pt_err_Muminus[i] = sqrt(Pt_eff_Muminus[i]*(1.-Pt_eff_Muminus[i])/Pt_Muminus[i]);
  } else {
    Pt_eff_Muminus[i] = 0.;
    Pt_err_Muminus[i] = 0.;
  }

 }




// calculate efficiencies

TH1F *hPt_L1eff_Mu = (TH1F*) hpt_mu_inroi->Clone();
hPt_L1eff_Mu->SetName("hPt_L1eff_Mu");
hPt_L1eff_Mu->Divide(hpt_mu);
for (Int_t i=0; i<NBINS_PT ; i++) {
  hPt_L1eff_Mu->SetBinError(i+1,Pt_err_Mu0[i]);  
}
TH1F *hPt_L1eff_Muplus = (TH1F*) hpt_muplus_inroi->Clone();
hPt_L1eff_Muplus->SetName("hPt_L1eff_Muplus");
hPt_L1eff_Muplus->Divide(hpt_muplus);
for (Int_t i=0; i<NBINS_PT ; i++) {
  hPt_L1eff_Muplus->SetBinError(i+1,Pt_err_Muplus0[i]);  
}
TH1F *hPt_L1eff_Muminus = (TH1F*) hpt_muminus_inroi->Clone();
hPt_L1eff_Muminus->SetName("hPt_L1eff_Muminus");
hPt_L1eff_Muminus->Divide(hpt_muminus);
for (Int_t i=0; i<NBINS_PT ; i++) {
  hPt_L1eff_Muminus->SetBinError(i+1,Pt_err_Muminus0[i]);  
}

  

TH1F *hPt_eff_Mu = (TH1F*) hpt_mu_tr->Clone();
hPt_eff_Mu->SetName("hPt_eff_Mu");
hPt_eff_Mu->Divide(hpt_mu_inroi);
for (Int_t i=0; i<NBINS_PT ; i++) {
  hPt_eff_Mu->SetBinError(i+1,Pt_err_Mu[i]);  
}

TH1F *hPt_eff_Muplus = (TH1F*) hpt_muplus_tr->Clone();
hPt_eff_Muplus->SetName("hPt_eff_Muplus");
hPt_eff_Muplus->Divide(hpt_muplus_inroi);
for (Int_t i=0; i<NBINS_PT ; i++) {
  hPt_eff_Muplus->SetBinError(i+1,Pt_err_Muplus[i]);  
}

TH1F *hPt_eff_Muminus = (TH1F*) hpt_muminus_tr->Clone();
hPt_eff_Muminus->SetName("hPt_eff_Muminus");
hPt_eff_Muminus->Divide(hpt_muminus_inroi);
for (Int_t i=0; i<NBINS_PT ; i++) {
  hPt_eff_Muminus->SetBinError(i+1,Pt_err_Muminus[i]);  
}




/////////////////// ETA

Stat_t  Eta_Mu[NBINS_ETA],Eta_Mu_tr[NBINS_ETA];
Stat_t  Eta_Muplus[NBINS_ETA],Eta_Muplus_tr[NBINS_ETA];
Stat_t  Eta_Muminus[NBINS_ETA],Eta_Muminus_tr[NBINS_ETA];
 Stat_t  Eta_Mu0[NBINS_ETA];
 Stat_t  Eta_Muplus0[NBINS_ETA];
 Stat_t  Eta_Muminus0[NBINS_ETA];

Float_t Eta_eff_Mu0[NBINS_ETA],Eta_eff_Muplus0[NBINS_ETA],Eta_eff_Muminus0[NBINS_ETA];
Float_t Eta_eff_Mu[NBINS_ETA],Eta_eff_Muplus[NBINS_ETA],Eta_eff_Muminus[NBINS_ETA];
Stat_t Eta_err_Mu0[NBINS_ETA],Eta_err_Muplus0[NBINS_ETA],Eta_err_Muminus0[NBINS_ETA];
Stat_t Eta_err_Mu[NBINS_ETA],Eta_err_Muplus[NBINS_ETA],Eta_err_Muminus[NBINS_ETA];

for (Int_t i=0; i<NBINS_ETA ; i++) {
  Eta_Mu0[i] = heta_mu->GetBinContent(i+1);
  Eta_Mu[i] = heta_mu_inroi->GetBinContent(i+1);
  Eta_Mu_tr[i] = heta_mu_tr->GetBinContent(i+1);
  Eta_Muplus0[i] = heta_muplus->GetBinContent(i+1);
  Eta_Muplus[i] = heta_muplus_inroi->GetBinContent(i+1);
  Eta_Muplus_tr[i] = heta_muplus_tr->GetBinContent(i+1);
  Eta_Muminus0[i] = heta_muminus->GetBinContent(i+1);
  Eta_Muminus[i] = heta_muminus_inroi->GetBinContent(i+1);
  Eta_Muminus_tr[i] = heta_muminus_tr->GetBinContent(i+1);

  if (Eta_Mu0[i] != 0) {
    Eta_eff_Mu0[i] = Eta_Mu[i]/Eta_Mu0[i];
    Eta_err_Mu0[i] = sqrt(Eta_eff_Mu0[i]*(1.-Eta_eff_Mu0[i])/Eta_Mu0[i]);
  } else {
    Eta_eff_Mu0[i] = 0.;
    Eta_err_Mu0[i] = 0.;
  }
  if (Eta_Muplus0[i] != 0) {
    Eta_eff_Muplus0[i] = Eta_Muplus[i]/Eta_Muplus0[i];
    Eta_err_Muplus0[i] = sqrt(Eta_eff_Muplus0[i]*(1.-Eta_eff_Muplus0[i])/Eta_Muplus0[i]);
  } else {
    Eta_eff_Muplus0[i] = 0.;
    Eta_err_Muplus0[i] = 0.;
  }
  if (Eta_Muminus0[i] != 0) {
    Eta_eff_Muminus0[i] = Eta_Muminus[i]/Eta_Muminus0[i];
    Eta_err_Muminus0[i] = sqrt(Eta_eff_Muminus0[i]*(1.-Eta_eff_Muminus0[i])/Eta_Muminus0[i]);
  } else {
    Eta_eff_Muminus0[i] = 0.;
    Eta_err_Muminus0[i] = 0.;
  }


  if (Eta_Mu[i] != 0) {
    Eta_eff_Mu[i] = Eta_Mu_tr[i]/Eta_Mu[i];
    Eta_err_Mu[i] = sqrt(Eta_eff_Mu[i]*(1.-Eta_eff_Mu[i])/Eta_Mu[i]);
  } else {
    Eta_eff_Mu[i] = 0.;
    Eta_err_Mu[i] = 0.;
  }

  if (Eta_Muplus[i] != 0) {
    Eta_eff_Muplus[i] = Eta_Muplus_tr[i]/Eta_Muplus[i];
    Eta_err_Muplus[i] = sqrt(Eta_eff_Muplus[i]*(1.-Eta_eff_Muplus[i])/Eta_Muplus[i]);
  } else {
    Eta_eff_Muplus[i] = 0.;
    Eta_err_Muplus[i] = 0.;
  }

  if (Eta_Muminus[i] != 0) {
    Eta_eff_Muminus[i] = Eta_Muminus_tr[i]/Eta_Muminus[i];
    Eta_err_Muminus[i] = sqrt(Eta_eff_Muminus[i]*(1.-Eta_eff_Muminus[i])/Eta_Muminus[i]);
  } else {
    Eta_eff_Muminus[i] = 0.;
    Eta_err_Muminus[i] = 0.;
  }
 }

// calculate efficiencies

TH1F *hEta_L1eff_Mu = (TH1F*) heta_mu_inroi->Clone();
hEta_L1eff_Mu->SetName("hEta_L1eff_Mu");
hEta_L1eff_Mu->Divide(heta_mu);
for (Int_t i=0; i<NBINS_ETA ; i++) {
  hEta_L1eff_Mu->SetBinError(i+1,Eta_err_Mu0[i]);  
}
TH1F *hEta_L1eff_Muplus = (TH1F*) heta_muplus_inroi->Clone();
hEta_L1eff_Muplus->SetName("hEta_L1eff_Muplus");
hEta_L1eff_Muplus->Divide(heta_muplus);
for (Int_t i=0; i<NBINS_ETA ; i++) {
  hEta_L1eff_Muplus->SetBinError(i+1,Eta_err_Muplus0[i]);
}
TH1F *hEta_L1eff_Muminus = (TH1F*) heta_muminus_inroi->Clone();
hEta_L1eff_Muminus->SetName("hEta_L1eff_Muminus");
hEta_L1eff_Muminus->Divide(heta_muminus);
for (Int_t i=0; i<NBINS_ETA ; i++) {
  hEta_L1eff_Muminus->SetBinError(i+1,Eta_err_Muminus0[i]);  
}



 TH1F *hEta_eff_Mu = (TH1F*) heta_mu_tr->Clone();
 hEta_eff_Mu->SetName("hEta_eff_Mu");
 hEta_eff_Mu->Divide(heta_mu_inroi);
 for (Int_t i=0; i<NBINS_ETA ; i++) {
   hEta_eff_Mu->SetBinError(i+1,Eta_err_Mu[i]);  
 }
 
 TH1F *hEta_eff_Muplus = (TH1F*) heta_muplus_tr->Clone();
 hEta_eff_Muplus->SetName("hEta_eff_Muplus");
 hEta_eff_Muplus->Divide(heta_muplus_inroi);
 for (Int_t i=0; i<NBINS_ETA ; i++) {
   hEta_eff_Muplus->SetBinError(i+1,Eta_err_Muplus[i]);  
 }
 
 TH1F *hEta_eff_Muminus = (TH1F*) heta_muminus_tr->Clone();
 hEta_eff_Muminus->SetName("hEta_eff_Muminus");
 hEta_eff_Muminus->Divide(heta_muminus_inroi);
 for (Int_t i=0; i<NBINS_ETA ; i++) {
   hEta_eff_Muminus->SetBinError(i+1,Eta_err_Muminus[i]);  
 }
 
 
 ///////////////// PHI
 Stat_t  Phi_Mu[NBINS_PHI],Phi_Mu_tr[NBINS_PHI];
 Stat_t  Phi_Muplus[NBINS_PHI],Phi_Muplus_tr[NBINS_PHI];
 Stat_t  Phi_Muminus[NBINS_PHI],Phi_Muminus_tr[NBINS_PHI];
 Stat_t  Phi_Mu0[NBINS_PHI];
   Stat_t  Phi_Muplus0[NBINS_PHI];
   Stat_t  Phi_Muminus0[NBINS_PHI];

 Float_t Phi_eff_Mu0[NBINS_PHI],Phi_eff_Muplus0[NBINS_PHI],Phi_eff_Muminus0[NBINS_PHI];
 Float_t Phi_eff_Mu[NBINS_PHI],Phi_eff_Muplus[NBINS_PHI],Phi_eff_Muminus[NBINS_PHI];
 Stat_t Phi_err_Mu0[NBINS_PHI],Phi_err_Muplus0[NBINS_PHI],Phi_err_Muminus0[NBINS_PHI];
 Stat_t Phi_err_Mu[NBINS_PHI],Phi_err_Muplus[NBINS_PHI],Phi_err_Muminus[NBINS_PHI];
 
 for (Int_t i=0; i<NBINS_PHI ; i++) {
  Phi_Mu0[i] = hphi_mu->GetBinContent(i+1);
  Phi_Mu[i] = hphi_mu_inroi->GetBinContent(i+1);
  Phi_Mu_tr[i] = hphi_mu_tr->GetBinContent(i+1);
  Phi_Muplus0[i] = hphi_muplus->GetBinContent(i+1);
  Phi_Muplus[i] = hphi_muplus_inroi->GetBinContent(i+1);
  Phi_Muplus_tr[i] = hphi_muplus_tr->GetBinContent(i+1);
  Phi_Muminus0[i] = hphi_muminus->GetBinContent(i+1);
  Phi_Muminus[i] = hphi_muminus_inroi->GetBinContent(i+1);
  Phi_Muminus_tr[i] = hphi_muminus_tr->GetBinContent(i+1);


  if (Phi_Mu0[i] != 0) {
    Phi_eff_Mu0[i] = Phi_Mu[i]/Phi_Mu0[i];
    Phi_err_Mu0[i] = sqrt(Phi_eff_Mu0[i]*(1.-Phi_eff_Mu0[i])/Phi_Mu0[i]);
  } else {
    Phi_eff_Mu0[i] = 0.;
    Phi_err_Mu0[i] = 0.;
  }
  if (Phi_Muplus0[i] != 0) {
    Phi_eff_Muplus0[i] = Phi_Muplus[i]/Phi_Muplus0[i];
    Phi_err_Muplus0[i] = sqrt(Phi_eff_Muplus0[i]*(1.-Phi_eff_Muplus0[i])/Phi_Muplus0[i]);
  } else {
    Phi_eff_Muplus0[i] = 0.;
    Phi_err_Muplus0[i] = 0.;
  }
  if (Phi_Muminus0[i] != 0) {
    Phi_eff_Muminus0[i] = Phi_Muminus[i]/Phi_Muminus0[i];
    Phi_err_Muminus0[i] = sqrt(Phi_eff_Muminus0[i]*(1.-Phi_eff_Muminus0[i])/Phi_Muminus0[i]);
  } else {
    Phi_eff_Muminus0[i] = 0.;
    Phi_err_Muminus0[i] = 0.;
  }




  if (Phi_Mu[i] != 0) {
    Phi_eff_Mu[i] = Phi_Mu_tr[i]/Phi_Mu[i];
    Phi_err_Mu[i] = sqrt(Phi_eff_Mu[i]*(1.-Phi_eff_Mu[i])/Phi_Mu[i]);
  } else {
    Phi_eff_Mu[i] = 0.;
    Phi_err_Mu[i] = 0.;
  }

  if (Phi_Muminus[i] != 0) {
    Phi_eff_Muminus[i] = Phi_Muminus_tr[i]/Phi_Muminus[i];
    Phi_err_Muminus[i] = sqrt(Phi_eff_Muminus[i]*(1.-Phi_eff_Muminus[i])/Phi_Muminus[i]);
  } else {
    Phi_eff_Muminus[i] = 0.;
    Phi_err_Muminus[i] = 0.;
  }

  if (Phi_Muplus[i] != 0) {
    Phi_eff_Muplus[i] = Phi_Muplus_tr[i]/Phi_Muplus[i];
    Phi_err_Muplus[i] = sqrt(Phi_eff_Muplus[i]*(1.-Phi_eff_Muplus[i])/Phi_Muplus[i]);
  } else {
    Phi_eff_Muplus[i] = 0.;
    Phi_err_Muplus[i] = 0.;
  }

}

// calculate efficiencies
TH1F *hPhi_L1eff_Mu = (TH1F*) hphi_mu_inroi->Clone();
hPhi_L1eff_Mu->SetName("hPhi_L1eff_Mu");
hPhi_L1eff_Mu->Divide(hphi_mu);
for (Int_t i=0; i<NBINS_PHI ; i++) {
  hPhi_L1eff_Mu->SetBinError(i+1,Phi_err_Mu0[i]);  
}
TH1F *hPhi_L1eff_Muplus = (TH1F*) hphi_muplus_inroi->Clone();
hPhi_L1eff_Muplus->SetName("hPhi_L1eff_Muplus");
hPhi_L1eff_Muplus->Divide(hphi_muplus);
for (Int_t i=0; i<NBINS_PHI ; i++) {
  hPhi_L1eff_Muplus->SetBinError(i+1,Phi_err_Muplus0[i]);  
}
TH1F *hPhi_L1eff_Muminus = (TH1F*) hphi_muminus_inroi->Clone();
hPhi_L1eff_Muminus->SetName("hPhi_L1eff_Muminus");
hPhi_L1eff_Muminus->Divide(hphi_muminus);
for (Int_t i=0; i<NBINS_PHI ; i++) {
  hPhi_L1eff_Muminus->SetBinError(i+1,Phi_err_Muminus0[i]);  
}



TH1F *hPhi_eff_Mu = (TH1F*) hphi_mu_tr->Clone();
hPhi_eff_Mu->SetName("hPhi_eff_Mu");
hPhi_eff_Mu->Divide(hphi_mu_inroi);
for (Int_t i=0; i<NBINS_PHI ; i++) {
  hPhi_eff_Mu->SetBinError(i+1,Phi_err_Mu[i]);  
}

TH1F *hPhi_eff_Muplus = (TH1F*) hphi_muplus_tr->Clone();
hPhi_eff_Muplus->SetName("hPhi_eff_Muplus");
hPhi_eff_Muplus->Divide(hphi_muplus_inroi);
for (Int_t i=0; i<NBINS_PHI ; i++) {
  hPhi_eff_Muplus->SetBinError(i+1,Phi_err_Muplus[i]);  
}

TH1F *hPhi_eff_Muminus = (TH1F*) hphi_muminus_tr->Clone();
hPhi_eff_Muminus->SetName("hPhi_eff_Muminus");
hPhi_eff_Muminus->Divide(hphi_muminus_inroi);
for (Int_t i=0; i<NBINS_PHI ; i++) {
  hPhi_eff_Muminus->SetBinError(i+1,Phi_err_Muminus[i]);  
}


gStyle->SetOptTitle(0);

gStyle->SetOptStat(true);
TCanvas *MyC2 = new TCanvas("MyC2","Test Canvas 2",0,0,700,800);
MyC2->Divide(3,2);
MyC2->cd(1);
hPt_L1eff_Mu->SetXTitle("pT (MeV)");
hPt_L1eff_Mu->SetYTitle("L1efficiency");
hPt_L1eff_Mu->SetMinimum(0.);
hPt_L1eff_Mu->SetMaximum(1.1);
hPt_L1eff_Mu->SetLineStyle(1);
hPt_L1eff_Mu->Draw();

MyC2->cd(2);
hEta_L1eff_Mu->SetXTitle("Eta");
hEta_L1eff_Mu->SetYTitle("L1efficiency");
hEta_L1eff_Mu->SetMinimum(0.);
hEta_L1eff_Mu->SetMaximum(1.1);
hEta_L1eff_Mu->SetLineStyle(1);
hEta_L1eff_Mu->Draw();

MyC2->cd(3);
hPhi_L1eff_Mu->SetXTitle("Phi");
hPhi_L1eff_Mu->SetYTitle("L1efficiency");
hPhi_L1eff_Mu->SetMinimum(0.);
hPhi_L1eff_Mu->SetMaximum(1.1);
hPhi_L1eff_Mu->Draw();

MyC2->cd(4);
hPt_eff_Mu->SetXTitle("pT (MeV)");
hPt_eff_Mu->SetYTitle("HLTefficiency");
hPt_eff_Mu->SetMinimum(0.8);
hPt_eff_Mu->SetMaximum(1.1);
hPt_eff_Mu->SetLineStyle(1);
hPt_eff_Mu->Draw();

MyC2->cd(5);
hEta_eff_Mu->SetXTitle("Eta");
hEta_eff_Mu->SetYTitle("HLTefficiency");
hEta_eff_Mu->SetMinimum(0.8);
hEta_eff_Mu->SetMaximum(1.1);
hEta_eff_Mu->SetLineStyle(1);
hEta_eff_Mu->Draw();

MyC2->cd(6);
hPhi_eff_Mu->SetXTitle("Phi");
hPhi_eff_Mu->SetYTitle("HLTefficiency");
hPhi_eff_Mu->SetMinimum(0.8);
hPhi_eff_Mu->SetMaximum(1.1);
hPhi_eff_Mu->Draw();

MyC2->SaveAs("muon_eff.eps");
MyC2->SaveAs("muon_eff.jpeg");


TCanvas *MyC3 = new TCanvas("MyC3","muon eff",0,0,700,800);
MyC3->Divide(2,3);
MyC3->cd(1);
hPt_eff_Muplus->SetXTitle("pT (MeV)");
hPt_eff_Muplus->SetYTitle("HLTefficiency");
hPt_eff_Muplus->SetMinimum(0.);
hPt_eff_Muplus->SetMaximum(1.1);
hPt_eff_Muplus->SetLineStyle(1);
hPt_eff_Muplus->Draw();

MyC3->cd(2);
hPt_eff_Muminus->SetXTitle("pT (MeV)");
hPt_eff_Muminus->SetYTitle("HLTefficiency");
hPt_eff_Muminus->SetMinimum(0.);
hPt_eff_Muminus->SetMaximum(1.1);
hPt_eff_Muminus->SetLineStyle(1);
hPt_eff_Muminus->Draw();

MyC3->cd(3);
hEta_eff_Muplus->SetXTitle("Eta");
hEta_eff_Muplus->SetYTitle("HLTefficiency");
hEta_eff_Muplus->SetMinimum(0.);
hEta_eff_Muplus->SetMaximum(1.1);
hEta_eff_Muplus->SetLineStyle(1);
hEta_eff_Muplus->Draw();

MyC3->cd(4);
hEta_eff_Muminus->SetXTitle("Eta");
hEta_eff_Muminus->SetYTitle("HLTefficiency");
hEta_eff_Muminus->SetMinimum(0.);
hEta_eff_Muminus->SetMaximum(1.1);
hEta_eff_Muminus->SetLineStyle(1);
hEta_eff_Muminus->Draw();

MyC3->cd(5);
hPhi_eff_Muplus->SetXTitle("Phi");
hPhi_eff_Muplus->SetYTitle("HLTefficiency");
hPhi_eff_Muplus->SetMinimum(0.);
hPhi_eff_Muplus->SetMaximum(1.1);
hPhi_eff_Muplus->Draw();

MyC3->cd(6);
hPhi_eff_Muminus->SetXTitle("Phi");
hPhi_eff_Muminus->SetYTitle("HLTefficiency");
hPhi_eff_Muminus->SetMinimum(0.);
hPhi_eff_Muminus->SetMaximum(1.1);
hPhi_eff_Muminus->Draw();

MyC3->SaveAs("HLT_eff_muon.eps");
MyC3->SaveAs("HLT_eff_muon.jpeg");
 

TCanvas *MyC4 = new TCanvas("MyC4","L1 muon eff",0,0,700,800);
MyC4->Divide(2,3);
MyC4->cd(1);
hPt_L1eff_Muplus->SetXTitle("pT (MeV)");
hPt_L1eff_Muplus->SetYTitle("L1efficiency");
hPt_L1eff_Muplus->SetMinimum(0.);
hPt_L1eff_Muplus->SetMaximum(1.1);
hPt_L1eff_Muplus->SetLineStyle(1);
hPt_L1eff_Muplus->Draw();

MyC4->cd(2);
hPt_L1eff_Muminus->SetXTitle("pT (MeV)");
 hPt_L1eff_Muminus->SetYTitle("L1efficiency");
hPt_L1eff_Muminus->SetMinimum(0.);
hPt_L1eff_Muminus->SetMaximum(1.1);
hPt_L1eff_Muminus->SetLineStyle(1);
hPt_L1eff_Muminus->Draw();

MyC4->cd(3);
hEta_L1eff_Muplus->SetXTitle("Eta");
hEta_L1eff_Muplus->SetYTitle("L1efficiency");
hEta_L1eff_Muplus->SetMinimum(0.);
hEta_L1eff_Muplus->SetMaximum(1.1);
hEta_L1eff_Muplus->SetLineStyle(1);
hEta_L1eff_Muplus->Draw();

MyC4->cd(4);
hEta_L1eff_Muminus->SetXTitle("Eta");
hEta_L1eff_Muminus->SetYTitle("L1efficiency");
hEta_L1eff_Muminus->SetMinimum(0.);
hEta_L1eff_Muminus->SetMaximum(1.1);
 hEta_L1eff_Muminus->SetLineStyle(1);
hEta_L1eff_Muminus->Draw();

MyC4->cd(5);
hPhi_L1eff_Muplus->SetXTitle("Phi");
hPhi_L1eff_Muplus->SetYTitle("L1efficiency");
hPhi_L1eff_Muplus->SetMinimum(0.);
hPhi_L1eff_Muplus->SetMaximum(1.1);
hPhi_L1eff_Muplus->Draw();

MyC4->cd(6);
hPhi_L1eff_Muminus->SetXTitle("Phi");
hPhi_L1eff_Muminus->SetYTitle("L1efficiency");
hPhi_L1eff_Muminus->SetMinimum(0.);
hPhi_L1eff_Muminus->SetMaximum(1.1);
hPhi_L1eff_Muminus->Draw();

MyC4->SaveAs("L1_eff_muon.eps");
MyC4->SaveAs("L1_eff_muon.jpeg");


hPt_eff_Muplus->Write();
hPt_eff_Muminus->Write();
hEta_eff_Muplus->Write();
hEta_eff_Muminus->Write();
hPhi_eff_Muplus->Write();
hPhi_eff_Muminus->Write();

 fout.Close();
}

