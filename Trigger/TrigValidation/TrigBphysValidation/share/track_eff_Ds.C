{
// Run trackeff.cpp first

TFile f1("DsVal.aan.root");

/*
TH1F *hpt_k = (TH1F*)f1.Get("Bphys/hpt_k");
TH1F *hpt_k_L2_inroi = (TH1F*)f1.Get("Bphys/hpt_k_L2_inroi");
TH1F *hpt_k_inroi = (TH1F*)f1.Get("Bphys/hpt_k_inroi");
TH1F *hpt_k_tr = (TH1F*)f1.Get("Bphys/hpt_k_tr");
TH1F *hpt_k_EFtr = (TH1F*)f1.Get("Bphys/hpt_k_EFtr");
TH1F *hpt_k_L2_EFtr = (TH1F*)f1.Get("Bphys/hpt_k_L2_EFtr");
TH1F *heta_k = (TH1F*)f1.Get("Bphys/heta_k");
TH1F *heta_k_inroi = (TH1F*)f1.Get("Bphys/heta_k_inroi");
TH1F *heta_k_tr = (TH1F*)f1.Get("Bphys/heta_k_tr");
TH1F *heta_k_EFtr = (TH1F*)f1.Get("Bphys/heta_k_EFtr");
TH1F *hphi_k = (TH1F*)f1.Get("Bphys/hphi_k");
TH1F *hphi_k_inroi = (TH1F*)f1.Get("Bphys/hphi_k_inroi");
TH1F *hphi_k_tr = (TH1F*)f1.Get("Bphys/hphi_k_tr");
TH1F *hphi_k_EFtr = (TH1F*)f1.Get("Bphys/hphi_k_EFtr");
*/
TH1F *hpt_kplus = (TH1F*)f1.Get("Bphys/hpt_kplus");
TH1F *hpt_kplus_L2_inroi = (TH1F*)f1.Get("Bphys/hpt_kplus_L2_inroi");
TH1F *hpt_kplus_inroi = (TH1F*)f1.Get("Bphys/hpt_kplus_inroi");
TH1F *hpt_kplus_tr = (TH1F*)f1.Get("Bphys/hpt_kplus_tr");
TH1F *hpt_kplus_EFtr = (TH1F*)f1.Get("Bphys/hpt_kplus_EFtr");
TH1F *hpt_kplus_L2_EFtr = (TH1F*)f1.Get("Bphys/hpt_kplus_L2_EFtr");
TH1F *heta_kplus = (TH1F*)f1.Get("Bphys/heta_kplus");
TH1F *heta_kplus_inroi = (TH1F*)f1.Get("Bphys/heta_kplus_inroi");
TH1F *heta_kplus_tr = (TH1F*)f1.Get("Bphys/heta_kplus_tr");
TH1F *heta_kplus_EFtr = (TH1F*)f1.Get("Bphys/heta_kplus_EFtr");
TH1F *hphi_kplus = (TH1F*)f1.Get("Bphys/hphi_kplus");
TH1F *hphi_kplus_inroi = (TH1F*)f1.Get("Bphys/hphi_kplus_inroi");
TH1F *hphi_kplus_tr = (TH1F*)f1.Get("Bphys/hphi_kplus_tr");
TH1F *hphi_kplus_EFtr = (TH1F*)f1.Get("Bphys/hphi_kplus_EFtr");

TH1F *hpt_kminus = (TH1F*)f1.Get("Bphys/hpt_kminus");
TH1F *hpt_kminus_L2_inroi = (TH1F*)f1.Get("Bphys/hpt_kminus_L2_inroi");
TH1F *hpt_kminus_inroi = (TH1F*)f1.Get("Bphys/hpt_kminus_inroi");
TH1F *hpt_kminus_tr = (TH1F*)f1.Get("Bphys/hpt_kminus_tr");
TH1F *hpt_kminus_EFtr = (TH1F*)f1.Get("Bphys/hpt_kminus_EFtr");
TH1F *hpt_kminus_L2_EFtr = (TH1F*)f1.Get("Bphys/hpt_kminus_L2_EFtr");
TH1F *heta_kminus = (TH1F*)f1.Get("Bphys/heta_kminus");
TH1F *heta_kminus_inroi = (TH1F*)f1.Get("Bphys/heta_kminus_inroi");
TH1F *heta_kminus_tr = (TH1F*)f1.Get("Bphys/heta_kminus_tr");
TH1F *heta_kminus_EFtr = (TH1F*)f1.Get("Bphys/heta_kminus_EFtr");
TH1F *hphi_kminus = (TH1F*)f1.Get("Bphys/hphi_kminus");
TH1F *hphi_kminus_inroi = (TH1F*)f1.Get("Bphys/hphi_kminus_inroi");
TH1F *hphi_kminus_tr = (TH1F*)f1.Get("Bphys/hphi_kminus_tr");
TH1F *hphi_kminus_EFtr = (TH1F*)f1.Get("Bphys/hphi_kminus_EFtr");

TH1F *hpt_pion = (TH1F*)f1.Get("Bphys/hpt_pion");
TH1F *hpt_pion_inroi = (TH1F*)f1.Get("Bphys/hpt_pion_inroi");
TH1F *hpt_pion_L2_inroi = (TH1F*)f1.Get("Bphys/hpt_pion_L2_inroi");
TH1F *hpt_pion_tr = (TH1F*)f1.Get("Bphys/hpt_pion_tr");
TH1F *hpt_pion_EFtr = (TH1F*)f1.Get("Bphys/hpt_pion_EFtr");
TH1F *hpt_pion_L2_EFtr = (TH1F*)f1.Get("Bphys/hpt_pion_L2_EFtr");
TH1F *heta_pion = (TH1F*)f1.Get("Bphys/heta_pion");
TH1F *heta_pion_inroi = (TH1F*)f1.Get("Bphys/heta_pion_inroi");
TH1F *heta_pion_tr = (TH1F*)f1.Get("Bphys/heta_pion_tr");
TH1F *heta_pion_EFtr = (TH1F*)f1.Get("Bphys/heta_pion_EFtr");
TH1F *hphi_pion = (TH1F*)f1.Get("Bphys/hphi_pion");
TH1F *hphi_pion_inroi = (TH1F*)f1.Get("Bphys/hphi_pion_inroi");
TH1F *hphi_pion_tr = (TH1F*)f1.Get("Bphys/hphi_pion_tr");
TH1F *hphi_pion_EFtr = (TH1F*)f1.Get("Bphys/hphi_pion_EFtr");

hpt_kplus->Draw();
TH1F *hpt_k = (TH1F*)hpt_kplus->Clone();
hpt_k->SetName("hpt_k");
hpt_k->Add(hpt_kminus);
TH1F *hpt_k_inroi = (TH1F*)hpt_kplus_inroi->Clone();
hpt_k_inroi->SetName("hpt_k_inroi");
hpt_k_inroi->Add(hpt_kminus_inroi);
//TH1F *hpt_k_L2_inroi = (TH1F*)hpt_kplus_L2_inroi->Clone();
//hpt_k_L2_inroi->SetName("hpt_k_L2_inroi");
//hpt_k_L2_inroi->Add(hpt_kminus_L2_inroi);
TH1F *hpt_k_tr = (TH1F*)hpt_kplus_tr->Clone();
hpt_k_tr->SetName("hpt_k_tr");
hpt_k_tr->Add(hpt_kminus_tr);

TH1F *heta_k = (TH1F*)heta_kplus->Clone();
heta_k->SetName("heta_k");
heta_k->Add(heta_kminus);
TH1F *heta_k_inroi = (TH1F*)heta_kplus_inroi->Clone();
heta_k_inroi->SetName("heta_k_inroi");
heta_k_inroi->Add(heta_kminus_inroi);
TH1F *heta_k_tr = (TH1F*)heta_kplus_tr->Clone();
heta_k_tr->SetName("heta_k_tr");
heta_k_tr->Add(heta_kminus_tr);

TH1F *hphi_k = (TH1F*)hphi_kplus->Clone();
hphi_k->SetName("hphi_k");
hphi_k->Add(hphi_kminus);
TH1F *hphi_k_inroi = (TH1F*)hphi_kplus_inroi->Clone();
hphi_k_inroi->SetName("hphi_k_inroi");
hphi_k_inroi->Add(hphi_kminus_inroi);
TH1F *hphi_k_tr = (TH1F*)hphi_kplus_tr->Clone();
hphi_k_tr->SetName("hphi_k_tr");
hphi_k_tr->Add(hphi_kminus_tr);

/*
TH1F *hpt_k_EFtr = (TH1F*)hpt_kplus_EFtr->Clone();
hpt_k_EFtr->SetName("hpt_k_EFtr");
hpt_k_EFtr->Add(hpt_kminus_EFtr);
//TH1F *hpt_k_L2_EFtr = (TH1F*)hpt_kplus_L2_EFtr->Clone();
//hpt_k_L2_EFtr->SetName("hpt_k_L2_EFtr");
//hpt_k_L2_EFtr->Add(hpt_kminus_L2_EFtr);
TH1F *heta_k_EFtr = (TH1F*)heta_kplus_EFtr->Clone();
heta_k_EFtr->SetName("heta_k_EFtr");
heta_k_EFtr->Add(heta_kminus_EFtr);
TH1F *hphi_k_EFtr = (TH1F*)hphi_kplus_EFtr->Clone();
hphi_k_EFtr->SetName("hphi_k_EFtr");
hphi_k_EFtr->Add(hphi_kminus_EFtr);
*/

const Int_t NBINS_PT=16;
const Int_t NBINS_ETA=20;
const Int_t NBINS_PHI=20;
Stat_t  Pt_K[NBINS_PT],Pt_L2_K[NBINS_PT],Pt_K_tr[NBINS_PT],Pt_K_EFtr[NBINS_PT],Pt_K_L2_EFtr[NBINS_PT];
Stat_t  Pt_Kplus[NBINS_PT],Pt_L2_Kplus[NBINS_PT],Pt_Kplus_tr[NBINS_PT],Pt_Kplus_EFtr[NBINS_PT],Pt_Kplus_L2_EFtr[NBINS_PT];
Stat_t  Pt_Kminus[NBINS_PT],Pt_L2_Kminus[NBINS_PT],Pt_Kminus_tr[NBINS_PT],Pt_Kminus_EFtr[NBINS_PT],Pt_Kminus_L2_EFtr[NBINS_PT];
Stat_t  Pt_pi[NBINS_PT],Pt_L2_pi[NBINS_PT],Pt_pi_tr[NBINS_PT],Pt_pi_EFtr[NBINS_PT],Pt_pi_L2_EFtr[NBINS_PT];
Float_t Pt_eff_K[NBINS_PT],Pt_eff_Kplus[NBINS_PT],Pt_eff_Kminus[NBINS_PT],Pt_eff_pi[NBINS_PT];
Stat_t Pt_err_K[NBINS_PT],Pt_err_Kplus[NBINS_PT],Pt_err_Kminus[NBINS_PT],Pt_err_pi[NBINS_PT];
Float_t Pt_EFeff_K[NBINS_PT],Pt_EFeff_Kplus[NBINS_PT],Pt_EFeff_Kminus[NBINS_PT],Pt_EFeff_pi[NBINS_PT];
Stat_t Pt_EFerr_K[NBINS_PT],Pt_EFerr_Kplus[NBINS_PT],Pt_EFerr_Kminus[NBINS_PT],Pt_EFerr_pi[NBINS_PT];
Float_t Pt_L2_EFeff_K[NBINS_PT],Pt_L2_EFeff_Kplus[NBINS_PT],Pt_L2_EFeff_Kminus[NBINS_PT],Pt_L2_EFeff_pi[NBINS_PT];
Stat_t Pt_L2_EFerr_K[NBINS_PT],Pt_L2_EFerr_Kplus[NBINS_PT],Pt_L2_EFerr_Kminus[NBINS_PT],Pt_L2_EFerr_pi[NBINS_PT];

for (Int_t i=0; i<NBINS_PT ; i++) {
  Pt_K[i] = hpt_k_inroi->GetBinContent(i+1);
  // Pt_L2_K[i] = hpt_k_L2_inroi->GetBinContent(i+1);
  Pt_K_tr[i] = hpt_k_tr->GetBinContent(i+1);
  //  Pt_K_EFtr[i] = hpt_k_EFtr->GetBinContent(i+1);
  // Pt_K_L2_EFtr[i] = hpt_k_L2_EFtr->GetBinContent(i+1);
  Pt_Kplus[i] = hpt_kplus_inroi->GetBinContent(i+1);
  // Pt_L2_Kplus[i] = hpt_kplus_L2_inroi->GetBinContent(i+1);
  Pt_Kplus_tr[i] = hpt_kplus_tr->GetBinContent(i+1);
  //Pt_Kplus_EFtr[i] = hpt_kplus_EFtr->GetBinContent(i+1);
  //  Pt_Kplus_L2_EFtr[i] = hpt_kplus_L2_EFtr->GetBinContent(i+1);
  Pt_Kminus[i] = hpt_kminus_inroi->GetBinContent(i+1);
  // Pt_L2_Kminus[i] = hpt_kminus_L2_inroi->GetBinContent(i+1);
  Pt_Kminus_tr[i] = hpt_kminus_tr->GetBinContent(i+1);
  //Pt_Kminus_EFtr[i] = hpt_kminus_EFtr->GetBinContent(i+1);
  // Pt_Kminus_L2_EFtr[i] = hpt_kminus_L2_EFtr->GetBinContent(i+1);
  Pt_pi[i] = hpt_pion_inroi->GetBinContent(i+1);
  //Pt_L2_pi[i] = hpt_pion_L2_inroi->GetBinContent(i+1);
  Pt_pi_tr[i] = hpt_pion_tr->GetBinContent(i+1);
  //Pt_pi_EFtr[i] = hpt_pion_EFtr->GetBinContent(i+1);
  //  Pt_pi_L2_EFtr[i] = hpt_pion_L2_EFtr->GetBinContent(i+1);

  if (Pt_K[i] != 0) {
    Pt_eff_K[i] = Pt_K_tr[i]/Pt_K[i];
    Pt_err_K[i] = sqrt(Pt_eff_K[i]*(1.-Pt_eff_K[i])/Pt_K[i]);
    Pt_EFeff_K[i] = Pt_K_EFtr[i]/Pt_K[i];
    Pt_EFerr_K[i] = sqrt(Pt_EFeff_K[i]*(1.-Pt_EFeff_K[i])/Pt_K[i]);
  } else {
    Pt_eff_K[i] = 0.;
    Pt_err_K[i] = 0.;
    Pt_EFeff_K[i] = 0.;
    Pt_EFerr_K[i] = 0.;
  }
  cout << Pt_K[i] << " " << Pt_K_tr[i] << Pt_K_EFtr[i] <<endl;
  if (Pt_Kplus[i] != 0) {
    Pt_eff_Kplus[i] = Pt_Kplus_tr[i]/Pt_Kplus[i];
    Pt_err_Kplus[i] = sqrt(Pt_eff_Kplus[i]*(1.-Pt_eff_Kplus[i])/Pt_Kplus[i]);
    Pt_EFeff_Kplus[i] = Pt_Kplus_EFtr[i]/Pt_Kplus[i];
    Pt_EFerr_Kplus[i] = sqrt(Pt_EFeff_Kplus[i]*(1.-Pt_EFeff_Kplus[i])/Pt_Kplus[i]);
  } else {
    Pt_eff_Kplus[i] = 0.;
    Pt_err_Kplus[i] = 0.;
    Pt_EFeff_Kplus[i] = 0.;
    Pt_EFerr_Kplus[i] = 0.;
  }
  cout << Pt_Kplus[i] << " " << Pt_Kplus_tr[i] << Pt_Kplus_EFtr[i] <<endl;
  if (Pt_Kminus[i] != 0) {
    Pt_eff_Kminus[i] = Pt_Kminus_tr[i]/Pt_Kminus[i];
    Pt_err_Kminus[i] = sqrt(Pt_eff_Kminus[i]*(1.-Pt_eff_Kminus[i])/Pt_Kminus[i]);
    Pt_EFeff_Kminus[i] = Pt_Kminus_EFtr[i]/Pt_Kminus[i];
    Pt_EFerr_Kminus[i] = sqrt(Pt_EFeff_Kminus[i]*(1.-Pt_EFeff_Kminus[i])/Pt_Kminus[i]);
  } else {
    Pt_eff_Kminus[i] = 0.;
    Pt_err_Kminus[i] = 0.;
    Pt_EFeff_Kminus[i] = 0.;
    Pt_EFerr_Kminus[i] = 0.;
  }
  cout << Pt_Kminus[i] << " " << Pt_Kminus_tr[i] << Pt_Kminus_EFtr[i] <<endl;

  /*
  if (Pt_L2_K[i] != 0) {
    Pt_L2_EFeff_K[i] = Pt_K_EFtr[i]/Pt_L2_K[i];
    Pt_L2_EFerr_K[i] = sqrt(Pt_EFeff_K[i]*(1.-Pt_EFeff_K[i])/Pt_L2_K[i]);
  } else {
    Pt_L2_EFeff_K[i] = 0.;
    Pt_L2_EFerr_K[i] = 0.;
  }

  if (Pt_L2_Kplus[i] != 0) {
    Pt_L2_EFeff_Kplus[i] = Pt_Kplus_EFtr[i]/Pt_L2_Kplus[i];
    Pt_L2_EFerr_Kplus[i] = sqrt(Pt_EFeff_Kplus[i]*(1.-Pt_EFeff_Kplus[i])/Pt_L2_Kplus[i]);
  } else {
    Pt_L2_EFeff_Kplus[i] = 0.;
    Pt_L2_EFerr_Kplus[i] = 0.;
  }

  if (Pt_L2_Kminus[i] != 0) {
    Pt_L2_EFeff_Kminus[i] = Pt_Kminus_EFtr[i]/Pt_L2_Kminus[i];
    Pt_L2_EFerr_Kminus[i] = sqrt(Pt_EFeff_Kminus[i]*(1.-Pt_EFeff_Kminus[i])/Pt_L2_Kminus[i]);
  } else {
    Pt_L2_EFeff_Kminus[i] = 0.;
    Pt_L2_EFerr_Kminus[i] = 0.;
  }
  */
  if (Pt_pi[i] != 0) {
    Pt_eff_pi[i] = Pt_pi_tr[i]/Pt_pi[i];
    Pt_err_pi[i] = sqrt(Pt_eff_pi[i]*(1.-Pt_eff_pi[i])/Pt_pi[i]);
    Pt_EFeff_pi[i] = Pt_pi_EFtr[i]/Pt_pi[i];
    Pt_EFerr_pi[i] = sqrt(Pt_EFeff_pi[i]*(1.-Pt_EFeff_pi[i])/Pt_pi[i]);
  } else {
    Pt_eff_pi[i] = 0.;
    Pt_err_pi[i] = 0.;
    Pt_EFeff_pi[i] = 0.;
    Pt_EFerr_pi[i] = 0.;
  }
  cout << Pt_pi[i] << " " << Pt_pi_tr[i] << Pt_pi_EFtr[i] <<endl;
  /*
  if (Pt_L2_pi[i] != 0) {
    Pt_L2_EFeff_pi[i] = Pt_pi_EFtr[i]/Pt_L2_pi[i];
    Pt_L2_EFerr_pi[i] = sqrt(Pt_EFeff_pi[i]*(1.-Pt_EFeff_pi[i])/Pt_L2_pi[i]);
  } else {
    Pt_L2_EFeff_pi[i] = 0.;
    Pt_L2_EFerr_pi[i] = 0.;
  }
  */
}
for (Int_t i=0; i<NBINS_PT ; i++) {
  cout << "IDSCAN  eff " << Pt_eff_K[i] << "+/-" << Pt_err_K[i] << endl;
  cout << "EF  eff " << Pt_eff_K[i] << "+/-" << Pt_err_K[i] << endl;
}
// calculate efficiencies

TH1F *hPt_eff_K = (TH1F*) hpt_k_tr->Clone();
hPt_eff_K->SetName("hPt_eff_K");
hPt_eff_K->Divide(hpt_k_inroi);
for (Int_t i=0; i<NBINS_PT ; i++) {
  hPt_eff_K->SetBinError(i+1,Pt_err_K[i]);  
}

TH1F *hPt_eff_Kplus = (TH1F*) hpt_kplus_tr->Clone();
hPt_eff_Kplus->SetName("hPt_eff_Kplus");
hPt_eff_Kplus->Divide(hpt_kplus_inroi);
for (Int_t i=0; i<NBINS_PT ; i++) {
  hPt_eff_Kplus->SetBinError(i+1,Pt_err_Kplus[i]);  
}

TH1F *hPt_eff_Kminus = (TH1F*) hpt_kminus_tr->Clone();
hPt_eff_Kminus->SetName("hPt_eff_Kminus");
hPt_eff_Kminus->Divide(hpt_kminus_inroi);
for (Int_t i=0; i<NBINS_PT ; i++) {
  hPt_eff_Kminus->SetBinError(i+1,Pt_err_Kminus[i]);  
}

TH1F *hPt_eff_pi = (TH1F*) hpt_pion_tr->Clone();
hPt_eff_pi->SetName("hPt_eff_pi");
hPt_eff_pi->Divide(hpt_pion_inroi);
for (Int_t i=0; i<NBINS_PT ; i++) {
  hPt_eff_pi->SetBinError(i+1,Pt_err_pi[i]);  
}

/*
TH1F *hPt_EFeff_K = (TH1F*) hpt_k_EFtr->Clone();
hPt_EFeff_K->SetName("hPt_EFeff_K");
hPt_EFeff_K->Divide(hpt_k_inroi);
for (Int_t i=0; i<NBINS_PT ; i++) {
  hPt_EFeff_K->SetBinError(i+1,Pt_EFerr_K[i]);  
}
//TH1F *hPt_L2_EFeff_K = (TH1F*) hpt_k_L2_EFtr->Clone();
//hPt_L2_EFeff_K->SetName("hPt_L2_EFeff_K");
//hPt_L2_EFeff_K->Divide(hpt_k_L2_inroi);
//for (Int_t i=0; i<NBINS_PT ; i++) {
//  hPt_L2_EFeff_K->SetBinError(i+1,Pt_L2_EFerr_K[i]);  
//}

TH1F *hPt_EFeff_Kplus = (TH1F*) hpt_kplus_EFtr->Clone();
hPt_EFeff_Kplus->SetName("hPt_EFeff_Kplus");
hPt_EFeff_Kplus->Divide(hpt_kplus_inroi);
for (Int_t i=0; i<NBINS_PT ; i++) {
  hPt_EFeff_Kplus->SetBinError(i+1,Pt_EFerr_Kplus[i]);  
}
*/
/*
TH1F *hPt_L2_EFeff_Kplus = (TH1F*) hpt_kplus_L2_EFtr->Clone();
hPt_L2_EFeff_Kplus->SetName("hPt_L2_EFeff_Kplus");
hPt_L2_EFeff_Kplus->Divide(hpt_kplus_L2_inroi);
for (Int_t i=0; i<NBINS_PT ; i++) {
  hPt_L2_EFeff_Kplus->SetBinError(i+1,Pt_L2_EFerr_Kplus[i]);  
}
*/
/*
TH1F *hPt_EFeff_Kminus = (TH1F*) hpt_kminus_EFtr->Clone();
hPt_EFeff_Kminus->SetName("hPt_EFeff_Kminus");
hPt_EFeff_Kminus->Divide(hpt_kminus_inroi);
for (Int_t i=0; i<NBINS_PT ; i++) {
  hPt_EFeff_Kminus->SetBinError(i+1,Pt_EFerr_Kminus[i]);  
}
*/
/*
TH1F *hPt_L2_EFeff_Kminus = (TH1F*) hpt_kminus_L2_EFtr->Clone();
hPt_L2_EFeff_Kminus->SetName("hPt_L2_EFeff_Kminus");
hPt_L2_EFeff_Kminus->Divide(hpt_kminus_L2_inroi);
for (Int_t i=0; i<NBINS_PT ; i++) {
  hPt_L2_EFeff_Kminus->SetBinError(i+1,Pt_L2_EFerr_Kminus[i]);  
}
*/
/*
TH1F *hPt_EFeff_pi = (TH1F*) hpt_pion_EFtr->Clone();
hPt_EFeff_pi->SetName("hPt_EFeff_pi");
hPt_EFeff_pi->Divide(hpt_pion_inroi);
for (Int_t i=0; i<NBINS_PT ; i++) {
  hPt_EFeff_pi->SetBinError(i+1,Pt_EFerr_pi[i]);  
}
*/
/*
TH1F *hPt_L2_EFeff_pi = (TH1F*) hpt_pion_L2_EFtr->Clone();
hPt_L2_EFeff_pi->SetName("hPt_L2_EFeff_pi");
hPt_L2_EFeff_pi->Divide(hpt_pion_L2_inroi);
for (Int_t i=0; i<NBINS_PT ; i++) {
  hPt_L2_EFeff_pi->SetBinError(i+1,Pt_L2_EFerr_pi[i]);  
}
*/
Stat_t  Eta_K[NBINS_ETA],Eta_K_tr[NBINS_ETA],Eta_K_EFtr[NBINS_ETA];
Stat_t  Eta_Kplus[NBINS_ETA],Eta_Kplus_tr[NBINS_ETA],Eta_Kplus_EFtr[NBINS_ETA];
Stat_t  Eta_Kminus[NBINS_ETA],Eta_Kminus_tr[NBINS_ETA],Eta_Kminus_EFtr[NBINS_ETA];
Stat_t  Eta_pi[NBINS_ETA],Eta_pi_tr[NBINS_ETA],Eta_pi_EFtr[NBINS_ETA];
Float_t Eta_eff_K[NBINS_ETA],Eta_eff_Kplus[NBINS_ETA],Eta_eff_Kminus[NBINS_ETA],Eta_eff_pi[NBINS_ETA];
Stat_t Eta_err_K[NBINS_ETA],Eta_err_Kplus[NBINS_ETA],Eta_err_Kminus[NBINS_ETA],Eta_err_pi[NBINS_ETA];
Float_t Eta_EFeff_K[NBINS_ETA],Eta_EFeff_Kplus[NBINS_ETA],Eta_EFeff_Kminus[NBINS_ETA],Eta_EFeff_pi[NBINS_ETA];
Stat_t Eta_EFerr_K[NBINS_ETA],Eta_EFerr_Kplus[NBINS_ETA],Eta_EFerr_Kminus[NBINS_ETA],Eta_EFerr_pi[NBINS_ETA];

for (Int_t i=0; i<NBINS_ETA ; i++) {
  Eta_K[i] = heta_k_inroi->GetBinContent(i+1);
  Eta_K_tr[i] = heta_k_tr->GetBinContent(i+1);
  //  Eta_K_EFtr[i] = heta_k_EFtr->GetBinContent(i+1);
  Eta_Kplus[i] = heta_kplus_inroi->GetBinContent(i+1);
  Eta_Kplus_tr[i] = heta_kplus_tr->GetBinContent(i+1);
  //  Eta_Kplus_EFtr[i] = heta_kplus_EFtr->GetBinContent(i+1);
  Eta_Kminus[i] = heta_kminus_inroi->GetBinContent(i+1);
  Eta_Kminus_tr[i] = heta_kminus_tr->GetBinContent(i+1);
  //  Eta_Kminus_EFtr[i] = heta_kminus_EFtr->GetBinContent(i+1);
  Eta_pi[i] = heta_pion_inroi->GetBinContent(i+1);
  Eta_pi_tr[i] = heta_pion_tr->GetBinContent(i+1);
  //  Eta_pi_EFtr[i] = heta_pion_EFtr->GetBinContent(i+1);

  if (Eta_K[i] != 0) {
    Eta_eff_K[i] = Eta_K_tr[i]/Eta_K[i];
    Eta_err_K[i] = sqrt(Eta_eff_K[i]*(1.-Eta_eff_K[i])/Eta_K[i]);
    Eta_EFeff_K[i] = Eta_K_EFtr[i]/Eta_K[i];
    Eta_EFerr_K[i] = sqrt(Eta_EFeff_K[i]*(1.-Eta_EFeff_K[i])/Eta_K[i]);
  } else {
    Eta_eff_K[i] = 0.;
    Eta_err_K[i] = 0.;
    Eta_EFeff_K[i] = 0.;
    Eta_EFerr_K[i] = 0.;
  }
  cout << " kaons eta , bin " << i << " " << Eta_K[i] << " " << Eta_K_tr[i] << endl;
  if (Eta_Kplus[i] != 0) {
    Eta_eff_Kplus[i] = Eta_Kplus_tr[i]/Eta_Kplus[i];
    Eta_err_Kplus[i] = sqrt(Eta_eff_Kplus[i]*(1.-Eta_eff_Kplus[i])/Eta_Kplus[i]);
    Eta_EFeff_Kplus[i] = Eta_Kplus_EFtr[i]/Eta_Kplus[i];
    Eta_EFerr_Kplus[i] = sqrt(Eta_EFeff_Kplus[i]*(1.-Eta_EFeff_Kplus[i])/Eta_Kplus[i]);
  } else {
    Eta_eff_Kplus[i] = 0.;
    Eta_err_Kplus[i] = 0.;
    Eta_EFeff_Kplus[i] = 0.;
    Eta_EFerr_Kplus[i] = 0.;
  }
  cout << " kplus eta , bin " << i << " " << Eta_Kplus[i] << " " << Eta_Kplus_tr[i] << endl;
  if (Eta_Kminus[i] != 0) {
    Eta_eff_Kminus[i] = Eta_Kminus_tr[i]/Eta_Kminus[i];
    Eta_err_Kminus[i] = sqrt(Eta_eff_Kminus[i]*(1.-Eta_eff_Kminus[i])/Eta_Kminus[i]);
    Eta_EFeff_Kminus[i] = Eta_Kminus_EFtr[i]/Eta_Kminus[i];
    Eta_EFerr_Kminus[i] = sqrt(Eta_EFeff_Kminus[i]*(1.-Eta_EFeff_Kminus[i])/Eta_Kminus[i]);
  } else {
    Eta_eff_Kminus[i] = 0.;
    Eta_err_Kminus[i] = 0.;
    Eta_EFeff_Kminus[i] = 0.;
    Eta_EFerr_Kminus[i] = 0.;
  }
  cout << " kminus eta , bin " << i << " " << Eta_Kminus[i] << " " << Eta_Kminus_tr[i] << endl;

  if (Eta_pi[i] != 0) {
    Eta_eff_pi[i] = Eta_pi_tr[i]/Eta_pi[i];
    Eta_err_pi[i] = sqrt(Eta_eff_pi[i]*(1.-Eta_eff_pi[i])/Eta_pi[i]);
    Eta_EFeff_pi[i] = Eta_pi_EFtr[i]/Eta_pi[i];
    Eta_EFerr_pi[i] = sqrt(Eta_EFeff_pi[i]*(1.-Eta_EFeff_pi[i])/Eta_pi[i]);
  } else {
    Eta_eff_pi[i] = 0.;
    Eta_err_pi[i] = 0.;
    Eta_EFeff_pi[i] = 0.;
    Eta_EFerr_pi[i] = 0.;
  }
  cout << " pions eta bin " << i << " " << Eta_pi[i] << " " << Eta_pi_tr[i] << " eff: " << Eta_eff_pi[i] << endl;
}
for (Int_t i=0; i<NBINS_ETA ; i++) {
  cout << "pion  eff, bin " << i << " " << Eta_eff_pi[i] << "+/-" << Eta_err_pi[i] << endl;

}
// calculate efficiencies

TH1F *hEta_eff_K = (TH1F*) heta_k_tr->Clone();
hEta_eff_K->SetName("hEta_eff_K");
hEta_eff_K->Divide(heta_k_inroi);
for (Int_t i=0; i<NBINS_ETA ; i++) {
  hEta_eff_K->SetBinError(i+1,Eta_err_K[i]);  
}

TH1F *hEta_eff_Kplus = (TH1F*) heta_kplus_tr->Clone();
hEta_eff_Kplus->SetName("hEta_eff_Kplus");
hEta_eff_Kplus->Divide(heta_kplus_inroi);
for (Int_t i=0; i<NBINS_ETA ; i++) {
  hEta_eff_Kplus->SetBinError(i+1,Eta_err_Kplus[i]);  
}

TH1F *hEta_eff_Kminus = (TH1F*) heta_kminus_tr->Clone();
hEta_eff_Kminus->SetName("hEta_eff_Kminus");
hEta_eff_Kminus->Divide(heta_kminus_inroi);
for (Int_t i=0; i<NBINS_ETA ; i++) {
  hEta_eff_Kminus->SetBinError(i+1,Eta_err_Kminus[i]);  
}

TH1F *hEta_eff_pi = (TH1F*) heta_pion_tr->Clone();
hEta_eff_pi->SetName("hEta_eff_pi");
hEta_eff_pi->Divide(heta_pion_inroi);
for (Int_t i=0; i<NBINS_ETA ; i++) {
  //  cout << " Set bin error " << i << " " << Eta_err_pi[i] << endl;
  hEta_eff_pi->SetBinError(i+1,Eta_err_pi[i]);  
}
/*
TH1F *hEta_EFeff_K = (TH1F*) heta_k_EFtr->Clone();
hEta_EFeff_K->SetName("hEta_EFeff_K");
hEta_EFeff_K->Divide(heta_k_inroi);
for (Int_t i=0; i<NBINS_ETA ; i++) {
  hEta_EFeff_K->SetBinError(i+1,Eta_EFerr_K[i]);  
}

TH1F *hEta_EFeff_Kplus = (TH1F*) heta_kplus_EFtr->Clone();
hEta_EFeff_Kplus->SetName("hEta_EFeff_Kplus");
hEta_EFeff_Kplus->Divide(heta_kplus_inroi);
for (Int_t i=0; i<NBINS_ETA ; i++) {
  hEta_EFeff_Kplus->SetBinError(i+1,Eta_EFerr_Kplus[i]);  
}

TH1F *hEta_EFeff_Kminus = (TH1F*) heta_kminus_EFtr->Clone();
hEta_EFeff_Kminus->SetName("hEta_EFeff_Kminus");
hEta_EFeff_Kminus->Divide(heta_kminus_inroi);
for (Int_t i=0; i<NBINS_ETA ; i++) {
  hEta_EFeff_Kminus->SetBinError(i+1,Eta_EFerr_Kminus[i]);  
}

TH1F *hEta_EFeff_pi = (TH1F*) heta_pion_EFtr->Clone();
hEta_EFeff_pi->SetName("hEta_EFeff_pi");
hEta_EFeff_pi->Divide(heta_pion_inroi);
for (Int_t i=0; i<NBINS_ETA ; i++) {
  //  cout << " Set bin error " << i << " " << Eta_EFerr_pi[i] << endl;
  hEta_EFeff_pi->SetBinError(i+1,Eta_EFerr_pi[i]);  
}
*/

cout << " L2: number K tracks in RoI " << heta_k_inroi->GetEntries() << endl;
cout << " L2: number K tracks found in RoI " << heta_k_tr->GetEntries() << "  eff: " <<
 heta_k_tr->GetEntries()/heta_k_inroi->GetEntries() << endl;
cout << " L2: number Kplus tracks in RoI " << heta_kplus_inroi->GetEntries() << endl;
cout << " L2: number K plustracks found in RoI " << heta_kplus_tr->GetEntries() << "  eff: " <<
 heta_kplus_tr->GetEntries()/heta_kplus_inroi->GetEntries() << endl;
cout << " L2: number Kminus tracks in RoI " << heta_kminus_inroi->GetEntries() << endl;
cout << " L2: number Kminus tracks found in RoI " << heta_kminus_tr->GetEntries() << "  eff: " <<
 heta_kminus_tr->GetEntries()/heta_kminus_inroi->GetEntries() << endl;
cout << " L2: number pi tracks in RoI " << heta_pion_inroi->GetEntries() << endl;
cout << " L2: number pi tracks found in RoI " << heta_pion_tr->GetEntries() <<  "  eff: " <<
 heta_pion_tr->GetEntries()/heta_pion_inroi->GetEntries() << endl;
/*
cout << " EF: number K tracks in RoI " << heta_k_inroi->GetEntries() << endl;
cout << " EF: number K tracks found in RoI " << heta_k_EFtr->GetEntries() << "  eff: " <<
 heta_k_EFtr->GetEntries()/heta_k_inroi->GetEntries() << endl;
cout << " EF: number Kplus tracks in RoI " << heta_kplus_inroi->GetEntries() << endl;
cout << " EF: number Kplus tracks found in RoI " << heta_kplus_EFtr->GetEntries() << "  eff: " <<
 heta_kplus_EFtr->GetEntries()/heta_kplus_inroi->GetEntries() << endl;
cout << " EF: number Kminus tracks in RoI " << heta_kminus_inroi->GetEntries() << endl;
cout << " EF: number Kminus tracks found in RoI " << heta_kminus_EFtr->GetEntries() << "  eff: " <<
 heta_kminus_EFtr->GetEntries()/heta_kminus_inroi->GetEntries() << endl;
cout << " EF: number pi tracks in RoI " << heta_pion_inroi->GetEntries() << endl;
cout << " EF: number pi tracks found in RoI " << heta_pion_EFtr->GetEntries() <<  "  eff: " <<
 heta_pion_EFtr->GetEntries()/heta_pion_inroi->GetEntries() << endl;
//cout << " EF after L2 (ALL PT): number K tracks in RoI " << hpt_k_L2_inroi->GetEntries() << endl;
//cout << " EF after L2: number K tracks found in RoI " << hpt_k_L2_EFtr->GetEntries() << "  eff: " <<
// hpt_k_L2_EFtr->GetEntries()/hpt_k_L2_inroi->GetEntries() << endl;
//cout << " EF after L2: number pi tracks in RoI " << hpt_pion_L2_inroi->GetEntries() << endl;
//cout << " EF after L2: number pi tracks found in RoI " << hpt_pion_L2_EFtr->GetEntries() <<  "  eff: " <<
// hpt_pion_L2_EFtr->GetEntries()/hpt_pion_L2_inroi->GetEntries() << endl;
*/
// phi
Stat_t  Phi_K[NBINS_PHI],Phi_K_tr[NBINS_PHI],Phi_K_EFtr[NBINS_PHI];
Stat_t  Phi_Kplus[NBINS_PHI],Phi_Kplus_tr[NBINS_PHI],Phi_Kplus_EFtr[NBINS_PHI];
Stat_t  Phi_Kminus[NBINS_PHI],Phi_Kminus_tr[NBINS_PHI],Phi_Kminus_EFtr[NBINS_PHI];
Stat_t  Phi_pi[NBINS_PHI],Phi_pi_tr[NBINS_PHI],Phi_pi_EFtr[NBINS_PHI];
Float_t Phi_eff_K[NBINS_PHI],Phi_eff_Kplus[NBINS_PHI],Phi_eff_Kminus[NBINS_PHI],Phi_eff_pi[NBINS_PHI];
Stat_t Phi_err_K[NBINS_PHI],Phi_err_Kplus[NBINS_PHI],Phi_err_Kminus[NBINS_PHI],Phi_err_pi[NBINS_PHI];
Float_t Phi_EFeff_K[NBINS_PHI],Phi_EFeff_Kplus[NBINS_PHI],Phi_EFeff_Kminus[NBINS_PHI],Phi_EFeff_pi[NBINS_PHI];
Stat_t Phi_EFerr_K[NBINS_PHI],Phi_EFerr_Kplus[NBINS_PHI],Phi_EFerr_Kminus[NBINS_PHI],Phi_EFerr_pi[NBINS_PHI];

for (Int_t i=0; i<NBINS_PHI ; i++) {
  Phi_K[i] = hphi_k_inroi->GetBinContent(i+1);
  Phi_K_tr[i] = hphi_k_tr->GetBinContent(i+1);
  //  Phi_K_EFtr[i] = hphi_k_EFtr->GetBinContent(i+1);
  Phi_Kplus[i] = hphi_kplus_inroi->GetBinContent(i+1);
  Phi_Kplus_tr[i] = hphi_kplus_tr->GetBinContent(i+1);
  //  Phi_Kplus_EFtr[i] = hphi_kplus_EFtr->GetBinContent(i+1);
  Phi_Kminus[i] = hphi_kminus_inroi->GetBinContent(i+1);
  Phi_Kminus_tr[i] = hphi_kminus_tr->GetBinContent(i+1);
  //  Phi_Kminus_EFtr[i] = hphi_kminus_EFtr->GetBinContent(i+1);
  Phi_pi[i] = hphi_pion_inroi->GetBinContent(i+1);
  Phi_pi_tr[i] = hphi_pion_tr->GetBinContent(i+1);
  //  Phi_pi_EFtr[i] = hphi_pion_EFtr->GetBinContent(i+1);

  if (Phi_K[i] != 0) {
    Phi_eff_K[i] = Phi_K_tr[i]/Phi_K[i];
    Phi_err_K[i] = sqrt(Phi_eff_K[i]*(1.-Phi_eff_K[i])/Phi_K[i]);
    Phi_EFeff_K[i] = Phi_K_EFtr[i]/Phi_K[i];
    Phi_EFerr_K[i] = sqrt(Phi_EFeff_K[i]*(1.-Phi_EFeff_K[i])/Phi_K[i]);
  } else {
    Phi_eff_K[i] = 0.;
    Phi_err_K[i] = 0.;
    Phi_EFeff_K[i] = 0.;
    Phi_EFerr_K[i] = 0.;
  }
  //  cout << " kaons eta , bin " << i << " " << Phi_K[i] << " " << Phi_K_tr[i] << endl;
  if (Phi_Kminus[i] != 0) {
    Phi_eff_Kminus[i] = Phi_Kminus_tr[i]/Phi_Kminus[i];
    Phi_err_Kminus[i] = sqrt(Phi_eff_Kminus[i]*(1.-Phi_eff_Kminus[i])/Phi_Kminus[i]);
    Phi_EFeff_Kminus[i] = Phi_Kminus_EFtr[i]/Phi_Kminus[i];
    Phi_EFerr_Kminus[i] = sqrt(Phi_EFeff_Kminus[i]*(1.-Phi_EFeff_Kminus[i])/Phi_Kminus[i]);
  } else {
    Phi_eff_Kminus[i] = 0.;
    Phi_err_Kminus[i] = 0.;
    Phi_EFeff_Kminus[i] = 0.;
    Phi_EFerr_Kminus[i] = 0.;
  }
  //  cout << " kaons eta , bin " << i << " " << Phi_Kminus[i] << " " << Phi_Kminus_tr[i] << endl;
  if (Phi_Kplus[i] != 0) {
    Phi_eff_Kplus[i] = Phi_Kplus_tr[i]/Phi_Kplus[i];
    Phi_err_Kplus[i] = sqrt(Phi_eff_Kplus[i]*(1.-Phi_eff_Kplus[i])/Phi_Kplus[i]);
    Phi_EFeff_Kplus[i] = Phi_Kplus_EFtr[i]/Phi_Kplus[i];
    Phi_EFerr_Kplus[i] = sqrt(Phi_EFeff_Kplus[i]*(1.-Phi_EFeff_Kplus[i])/Phi_Kplus[i]);
  } else {
    Phi_eff_Kplus[i] = 0.;
    Phi_err_Kplus[i] = 0.;
    Phi_EFeff_Kplus[i] = 0.;
    Phi_EFerr_Kplus[i] = 0.;
  }
  //  cout << " kaons eta , bin " << i << " " << Phi_Kplus[i] << " " << Phi_Kplus_tr[i] << endl;

  if (Phi_pi[i] != 0) {
    Phi_eff_pi[i] = Phi_pi_tr[i]/Phi_pi[i];
    Phi_err_pi[i] = sqrt(Phi_eff_pi[i]*(1.-Phi_eff_pi[i])/Phi_pi[i]);
    Phi_EFeff_pi[i] = Phi_pi_EFtr[i]/Phi_pi[i];
    Phi_EFerr_pi[i] = sqrt(Phi_EFeff_pi[i]*(1.-Phi_EFeff_pi[i])/Phi_pi[i]);
  } else {
    Phi_eff_pi[i] = 0.;
    Phi_err_pi[i] = 0.;
    Phi_EFeff_pi[i] = 0.;
    Phi_EFerr_pi[i] = 0.;
  }
  //  cout << " pions eta bin " << i << " " << Phi_pi[i] << " " << Phi_pi_tr[i] << " eff: " << Phi_eff_pi[i] << endl;
}
for (Int_t i=0; i<NBINS_PHI ; i++) {
  //  cout << "pion  eff, bin " << i << " " << Phi_eff_pi[i] << "+/-" << Phi_err_pi[i] << endl;

}
// calculate efficiencies

TH1F *hPhi_eff_K = (TH1F*) hphi_k_tr->Clone();
hPhi_eff_K->SetName("hPhi_eff_K");
hPhi_eff_K->Divide(hphi_k_inroi);
for (Int_t i=0; i<NBINS_PHI ; i++) {
  hPhi_eff_K->SetBinError(i+1,Phi_err_K[i]);  
}

TH1F *hPhi_eff_Kplus = (TH1F*) hphi_kplus_tr->Clone();
hPhi_eff_Kplus->SetName("hPhi_eff_Kplus");
hPhi_eff_Kplus->Divide(hphi_kplus_inroi);
for (Int_t i=0; i<NBINS_PHI ; i++) {
  hPhi_eff_Kplus->SetBinError(i+1,Phi_err_Kplus[i]);  
}

TH1F *hPhi_eff_Kminus = (TH1F*) hphi_kminus_tr->Clone();
hPhi_eff_Kminus->SetName("hPhi_eff_Kminus");
hPhi_eff_Kminus->Divide(hphi_kminus_inroi);
for (Int_t i=0; i<NBINS_PHI ; i++) {
  hPhi_eff_Kminus->SetBinError(i+1,Phi_err_Kminus[i]);  
}

TH1F *hPhi_eff_pi = (TH1F*) hphi_pion_tr->Clone();
hPhi_eff_pi->SetName("hPhi_eff_pi");
hPhi_eff_pi->Divide(hphi_pion_inroi);
for (Int_t i=0; i<NBINS_PHI ; i++) {
  //  cout << " Set bin error " << i << " " << Phi_err_pi[i] << endl;
  hPhi_eff_pi->SetBinError(i+1,Phi_err_pi[i]);  
}
/*
TH1F *hPhi_EFeff_K = (TH1F*) hphi_k_EFtr->Clone();
hPhi_EFeff_K->SetName("hPhi_EFeff_K");
hPhi_EFeff_K->Divide(hphi_k_inroi);
for (Int_t i=0; i<NBINS_PHI ; i++) {
  hPhi_EFeff_K->SetBinError(i+1,Phi_EFerr_K[i]);  
}

TH1F *hPhi_EFeff_Kplus = (TH1F*) hphi_kplus_EFtr->Clone();
hPhi_EFeff_Kplus->SetName("hPhi_EFeff_Kplus");
hPhi_EFeff_Kplus->Divide(hphi_kplus_inroi);
for (Int_t i=0; i<NBINS_PHI ; i++) {
  hPhi_EFeff_Kplus->SetBinError(i+1,Phi_EFerr_Kplus[i]);  
}

TH1F *hPhi_EFeff_Kminus = (TH1F*) hphi_kminus_EFtr->Clone();
hPhi_EFeff_Kminus->SetName("hPhi_EFeff_Kminus");
hPhi_EFeff_Kminus->Divide(hphi_kminus_inroi);
for (Int_t i=0; i<NBINS_PHI ; i++) {
  hPhi_EFeff_Kminus->SetBinError(i+1,Phi_EFerr_Kminus[i]);  
}

TH1F *hPhi_EFeff_pi = (TH1F*) hphi_pion_EFtr->Clone();
hPhi_EFeff_pi->SetName("hPhi_EFeff_pi");
hPhi_EFeff_pi->Divide(hphi_pion_inroi);
for (Int_t i=0; i<NBINS_PHI ; i++) {
  //  cout << " Set bin error " << i << " " << Phi_EFerr_pi[i] << endl;
  hPhi_EFeff_pi->SetBinError(i+1,Phi_EFerr_pi[i]);  
}
*/
gStyle->SetOptTitle(0);

gStyle->SetOptStat(true);
TCanvas *MyC2 = new TCanvas("MyC2","Test Canvas 2",0,0,700,800);
MyC2->Divide(2,3);
MyC2->cd(1);
hPt_eff_K->SetXTitle("pT (MeV)");
hPt_eff_K->SetYTitle("efficiency");
hPt_eff_K->SetMinimum(0.);
hPt_eff_K->SetMaximum(1.1);
hPt_eff_K->SetLineStyle(1);
hPt_eff_K->Draw();
/*
hPt_EFeff_K->SetLineColor(2);
hPt_EFeff_K->SetLineStyle(1);
hPt_EFeff_K->Draw("SAME");
hPt_L2_EFeff_K->SetLineColor(4);
hPt_L2_EFeff_K->SetLineStyle(1);
hPt_L2_EFeff_K->Draw("SAME");
*/
MyC2->cd(2);
hPt_eff_pi->SetXTitle("pT (MeV)");
hPt_eff_pi->SetYTitle("efficiency");
hPt_eff_pi->SetMinimum(0.);
hPt_eff_pi->SetMaximum(1.1);
hPt_eff_pi->SetLineStyle(1);
hPt_eff_pi->Draw();
/*
hPt_EFeff_pi->SetLineColor(2);
hPt_EFeff_pi->SetLineStyle(1);
hPt_EFeff_pi->Draw("SAME");
hPt_L2_EFeff_pi->SetLineColor(4);
hPt_L2_EFeff_pi->SetLineStyle(1);
hPt_L2_EFeff_pi->Draw("SAME");
*/
MyC2->cd(3);
hEta_eff_K->SetXTitle("Eta");
hEta_eff_K->SetYTitle("efficiency");
hEta_eff_K->SetMinimum(0.);
hEta_eff_K->SetMaximum(1.1);
hEta_eff_K->SetLineStyle(1);
hEta_eff_K->Draw();
//hEta_EFeff_K->SetLineColor(2);
//hEta_EFeff_K->SetLineStyle(1);
//hEta_EFeff_K->Draw("SAME");
MyC2->cd(4);
hEta_eff_pi->SetXTitle("Eta");
hEta_eff_pi->SetYTitle("efficiency");
hEta_eff_pi->SetMinimum(0.);
hEta_eff_pi->SetMaximum(1.1);
hEta_eff_pi->SetLineStyle(1);
hEta_eff_pi->Draw();
//hEta_EFeff_pi->SetLineColor(2);
//hEta_EFeff_pi->SetLineStyle(1);
//hEta_EFeff_pi->Draw("SAME");
MyC2->cd(5);
hPhi_eff_K->SetXTitle("Phi");
hPhi_eff_K->SetYTitle("efficiency");
hPhi_eff_K->SetMinimum(0.);
hPhi_eff_K->SetMaximum(1.1);
hPhi_eff_K->Draw();
//hPhi_EFeff_K->SetLineColor(2);
//hPhi_EFeff_K->Draw("SAME");
MyC2->cd(6);
hPhi_eff_pi->SetXTitle("Phi");
hPhi_eff_pi->SetYTitle("efficiency");
hPhi_eff_pi->SetMinimum(0.);
hPhi_eff_pi->SetMaximum(1.1);
hPhi_eff_pi->Draw();
//hPhi_EFeff_pi->SetLineColor(2);
//hPhi_EFeff_pi->Draw("SAME");

MyC2->SaveAs("track_eff.png");


TCanvas *MyC3 = new TCanvas("MyC3","kaon eff",0,0,700,800);
MyC3->Divide(2,3);
MyC3->cd(1);
hPt_eff_Kplus->SetXTitle("pT (MeV)");
hPt_eff_Kplus->SetYTitle("efficiency");
hPt_eff_Kplus->SetMinimum(0.);
hPt_eff_Kplus->SetMaximum(1.1);
hPt_eff_Kplus->SetLineStyle(1);
hPt_eff_Kplus->Draw();
/*
hPt_EFeff_Kplus->SetLineColor(2);
hPt_EFeff_Kplus->SetLineStyle(1);
hPt_EFeff_Kplus->Draw("SAME");
hPt_L2_EFeff_Kplus->SetLineColor(4);
hPt_L2_EFeff_Kplus->SetLineStyle(1);
hPt_L2_EFeff_Kplus->Draw("SAME");
*/
MyC3->cd(2);
hPt_eff_Kminus->SetXTitle("pT (MeV)");
hPt_eff_Kminus->SetYTitle("efficiency");
hPt_eff_Kminus->SetMinimum(0.);
hPt_eff_Kminus->SetMaximum(1.1);
hPt_eff_Kminus->SetLineStyle(1);
hPt_eff_Kminus->Draw();
/*
hPt_EFeff_Kminus->SetLineColor(2);
hPt_EFeff_Kminus->SetLineStyle(1);
hPt_EFeff_Kminus->Draw("SAME");
hPt_L2_EFeff_Kminus->SetLineColor(4);
hPt_L2_EFeff_Kminus->SetLineStyle(1);
hPt_L2_EFeff_Kminus->Draw("SAME");
*/
MyC3->cd(3);
hEta_eff_Kplus->SetXTitle("Eta");
hEta_eff_Kplus->SetYTitle("efficiency");
hEta_eff_Kplus->SetMinimum(0.);
hEta_eff_Kplus->SetMaximum(1.1);
hEta_eff_Kplus->SetLineStyle(1);
hEta_eff_Kplus->Draw();
/*
hEta_EFeff_Kplus->SetLineColor(2);
hEta_EFeff_Kplus->SetLineStyle(1);
hEta_EFeff_Kplus->Draw("SAME");
*/
MyC3->cd(4);
hEta_eff_Kminus->SetXTitle("Eta");
hEta_eff_Kminus->SetYTitle("efficiency");
hEta_eff_Kminus->SetMinimum(0.);
hEta_eff_Kminus->SetMaximum(1.1);
hEta_eff_Kminus->SetLineStyle(1);
hEta_eff_Kminus->Draw();
/*
hEta_EFeff_Kminus->SetLineColor(2);
hEta_EFeff_Kminus->SetLineStyle(1);
hEta_EFeff_Kminus->Draw("SAME");
*/
MyC3->cd(5);
hPhi_eff_Kplus->SetXTitle("Phi");
hPhi_eff_Kplus->SetYTitle("efficiency");
hPhi_eff_Kplus->SetMinimum(0.);
hPhi_eff_Kplus->SetMaximum(1.1);
hPhi_eff_Kplus->Draw();
/*
hPhi_EFeff_Kplus->SetLineColor(2);
hPhi_EFeff_Kplus->Draw("SAME");
*/
MyC3->cd(6);
hPhi_eff_Kminus->SetXTitle("Phi");
hPhi_eff_Kminus->SetYTitle("efficiency");
hPhi_eff_Kminus->SetMinimum(0.);
hPhi_eff_Kminus->SetMaximum(1.1);
hPhi_eff_Kminus->Draw();
/*
hPhi_EFeff_Kminus->SetLineColor(2);
hPhi_EFeff_Kminus->Draw("SAME");
*/
MyC3->SaveAs("track_eff_kaon.png");
}

