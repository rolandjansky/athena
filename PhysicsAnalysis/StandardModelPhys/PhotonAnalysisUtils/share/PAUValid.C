#include <vector>


/**************************************************************************************************************************************/


void PAUValid(TString path1, TString rel_ref, TString path2, TString rel) {
#define MAXINPUTFILES 2
  TChain* hggreco1 = loadTree(path1);
  TChain* hggreco2 = loadTree(path2);
  TChain* chain[MAXINPUTFILES];
  chain[0] = loadTree(path1);
  chain[1] = loadTree(path2);
  photcalib(chain,rel_ref,rel);  
  mass(chain,rel_ref,rel);
  photeff(chain,rel_ref,rel);
  conveff(chain,rel_ref,rel);
  PhotonsLinResol(chain,rel_ref,rel);
}


/**************************************************************************************************************************************/


TChain* loadTree(TString path) {
  TChain* chain = new TChain("PAUReco","PAUReco");
  chain->Add(path) ;
  return chain ;
}


/**************************************************************************************************************************************/


void photcalib(const TChain* chain[MAXINPUTFILES],TString rel_ref,TString rel) {
  TProfile* hpt[MAXINPUTFILES];
  TProfile* hptc[MAXINPUTFILES];
  TProfile* hptu[MAXINPUTFILES];
  TProfile* hrc[MAXINPUTFILES];
  char hphotcalib_pt[MAXINPUTFILES][100];
  char hphotcalib_pt_c[MAXINPUTFILES][100];
  char hphotcalib_pt_u[MAXINPUTFILES][100];
  char hphotcalib_rc[MAXINPUTFILES][100];
  for (int f=0 ; f<MAXINPUTFILES ; f++) {
    sprintf(hphotcalib_pt[f],"hphotcalib_pt_%d",f);
    hpt[f] = new TProfile(hphotcalib_pt[f],"Photon Energy calibration vs p_{t}",20,0,100);
    sprintf(hphotcalib_pt_c[f],"hphotcalib_pt_c_%d",f);
    hptc[f] = new TProfile(hphotcalib_pt_c[f],"Photon Energy calibration vs p_{t} (conv)",20,0,100);
    sprintf(hphotcalib_pt_u[f],"hphotcalib_pt_u_%d",f);
    hptu[f] = new TProfile(hphotcalib_pt_u[f],"Photon Energy calibration vs p_{t} (unconv)",20,0,100);
    sprintf(hphotcalib_rc[f],"hphotcalib_rc_%d",f);
    hrc[f] = new TProfile(hphotcalib_rc[f],"Photon Energy calibration vs R_{conv}",16,0,800);
  }
  for (int f=0 ; f<MAXINPUTFILES ; f++) {
    hpt[f]->GetXaxis()->SetTitle("photon true p_{t} [GeV]");
    hpt[f]->GetYaxis()->SetTitle("E^{rec}/E^{true}");
    hpt[f]->SetMinimum(0.8);
    hpt[f]->SetMaximum(1.2);
    hptc[f]->SetMinimum(0.8);
    hptc[f]->SetMaximum(1.2);
    hptu[f]->SetMinimum(0.8);
    hptu[f]->SetMaximum(1.2);
    hpt[f]->SetMarkerStyle(23+f);
    hptc[f]->SetMarkerStyle(23+f); hptc[f]->SetMarkerColor(3); 
    hptu[f]->SetMarkerStyle(23+f); hptu[f]->SetMarkerColor(2); 
    hrc[f]->GetXaxis()->SetTitle("photon true R_{conv} [mm]");
    hrc[f]->GetYaxis()->SetTitle("E^{rec}/E^{true}");
    hrc[f]->SetMinimum(0.8);
    hrc[f]->SetMaximum(1.2);
    hrc[f]->SetMarkerStyle(23+f); hrc[f]->SetMarkerColor(3); 
    chain[f]->Project(hphotcalib_pt[f],"ph_E/ph_E_MC:ph_pt_MC/1000","ph_selected&&ph_matchMC&&ph_PDG==22&&ph_pt_MC<100000");
    chain[f]->Project(hphotcalib_pt_c[f],"ph_E/ph_E_MC:ph_pt_MC/1000","ph_selected&&ph_matchMC&&ph_PDG==22&&ph_pt_MC<100000&&ph_isConv_MC==1");
    chain[f]->Project(hphotcalib_pt_u[f],"ph_E/ph_E_MC:ph_pt_MC/1000","ph_selected&&ph_matchMC&&ph_PDG==22&&ph_pt_MC<100000&&ph_isConv_MC==0");
    chain[f]->Project(hphotcalib_rc[f],"ph_E/ph_E_MC:ph_RConv_MC","ph_selected&&ph_matchMC&&ph_PDG==22&&ph_pt_MC<100000&&ph_isConv_MC==1");
  }
  TCanvas* c = gimeCanvas4("cphotcalib_1");
  c->Divide(2,2);
  c->cd(1);
  hpt[0]->Draw();  hpt[1]->Draw("same");
  TLegend* l1 = gimeLegend(0.6,0.6,0.9,0.8) ;
  TString msg1 = "All - " ; msg1 += rel_ref ;
  TString msg2 = "All - " ; msg2 += rel ;
  l1->AddEntry(hpt[0],msg1,"lp");
  l1->AddEntry(hpt[1],msg2,"lp");
  l1->Draw();
  c->cd(2);
  hptc[0]->Draw(); hptc[1]->Draw("same");
  TLegend* l1 = gimeLegend(0.55,0.6,0.9,0.8) ;
  TString msg1 = "Conv. - " ; msg1 += rel_ref ;
  TString msg2 = "Conv. - " ; msg2 += rel ;
  l1->AddEntry(hptc[0],msg1,"lp");
  l1->AddEntry(hptc[1],msg2,"lp");
  l1->Draw();
  c->cd(3);
  hptu[0]->Draw(); hptu[1]->Draw("same");  
  TLegend* l1 = gimeLegend(0.55,0.6,0.9,0.8) ;
  TString msg1 = "Unconv. - " ; msg1 += rel_ref ;
  TString msg2 = "Unconv. - " ; msg2 += rel ;
  l1->AddEntry(hptu[0],msg1,"lp");
  l1->AddEntry(hptu[1],msg2,"lp");
  l1->Draw();
  c->cd(4);
  hrc[0]->Draw(); hrc[1]->Draw("same");
  TLegend* l1 = gimeLegend(0.55,0.6,0.9,0.8) ;
  TString msg1 = "Conv. - " ; msg1 += rel_ref ;
  TString msg2 = "Conv. - " ; msg2 += rel ;
  l1->AddEntry(hrc[0],msg1,"lp");
  l1->AddEntry(hrc[1],msg2,"lp");
  l1->Draw();
  c->SaveAs("photcalib_1.eps");

  TCanvas* c = gimeCanvas2("cphotcalib_2");
  c->Divide(1,2);
  c->cd(1);
  hpt[0]->Draw();  hptu[0]->Draw("same");  hptc[0]->Draw("same");
  TLegend* l1 = gimeLegend(0.55,0.6,0.9,0.8) ;
  TString msg1 = "All - " ; msg1 += rel_ref ;
  TString msg2 = "Unconverted - " ; msg2 += rel_ref ;
  TString msg3 = "Converted - " ; msg3 += rel_ref ;
  l1->AddEntry(hpt[0],msg1,"lp");
  l1->AddEntry(hptu[0],msg2,"lp");
  l1->AddEntry(hptc[0],msg3,"lp");
  l1->Draw();
  c->cd(2);
  hpt[1]->Draw();  hptu[1]->Draw("same");  hptc[1]->Draw("same");
  TLegend* l1 = gimeLegend(0.55,0.6,0.9,0.8) ;
  TString msg1 = "All - " ; msg1 += rel ;
  TString msg2 = "Unconverted - " ; msg2 += rel ;
  TString msg3 = "Converted - " ; msg3 += rel ;
  l1->AddEntry(hpt[0],msg1,"lp");
  l1->AddEntry(hptu[0],msg2,"lp");
  l1->AddEntry(hptc[0],msg3,"lp");
  l1->Draw();
  c->SaveAs("photcalib_2.eps");

}


/**************************************************************************************************************************************/


void photeff(const TChain* chain[MAXINPUTFILES],TString rel_ref,TString rel) {
  TProfile* hpt[MAXINPUTFILES];
  TProfile* hptc[MAXINPUTFILES];
  TProfile* hptu[MAXINPUTFILES];
  TProfile* heta[MAXINPUTFILES];
  TProfile* hetac[MAXINPUTFILES];
  TProfile* hetau[MAXINPUTFILES];
  char hphoteff_pt[MAXINPUTFILES][100];
  char hphoteff_pt_c[MAXINPUTFILES][100];
  char hphoteff_pt_u[MAXINPUTFILES][100];
  char hphoteff_eta[MAXINPUTFILES][100];
  char hphoteff_eta_c[MAXINPUTFILES][100];
  char hphoteff_eta_u[MAXINPUTFILES][100];
  for (int f=0 ; f<MAXINPUTFILES ; f++) {
    sprintf(hphoteff_pt[f],"hphoteff_pt_%d",f);
    hpt[f] = new TProfile(hphoteff_pt[f],"Photon ID efficiency vs p_{t}",20,0,100);
    sprintf(hphoteff_pt_c[f],"hphoteff_pt_c_%d",f);
    hptc[f] = new TProfile(hphoteff_pt_c[f],"Photon ID efficiency vs p_{t} (conv)",20,0,100);
    sprintf(hphoteff_pt_u[f],"hphoteff_pt_u_%d",f);
    hptu[f] = new TProfile(hphoteff_pt_u[f],"Photon ID efficiency vs p_{t} (unconv)",20,0,100);
    sprintf(hphoteff_eta[f],"hphoteff_eta_%d",f);
    heta[f] = new TProfile(hphoteff_eta[f],"Photon ID efficiency vs #eta (p_{t}>25 GeV)",25,-2.5,2.5);
    sprintf(hphoteff_eta_c[f],"hphoteff_eta_c_%d",f);
    hetac[f] = new TProfile(hphoteff_eta_c[f],"Photon ID efficiency vs #eta (p_{t}>25 GeV) (conv)",25,-2.5,2.5);
    sprintf(hphoteff_eta_u[f],"hphoteff_eta_u_%d",f);
    hetau[f] = new TProfile(hphoteff_eta_u[f],"Photon ID efficiency vs #eta (p_{t}>25 GeV) (unconv)",25,-2.5,2.5);
  }
  for (int f=0 ; f<MAXINPUTFILES ; f++) {
    hpt[f]->GetXaxis()->SetTitle("photon true p_{t} [GeV]");
    hpt[f]->GetYaxis()->SetTitle("efficiency");
    hpt[f]->SetMinimum(0);
    hpt[f]->SetMaximum(1);
    hpt[f]->SetMarkerStyle(23+f);
    hptc[f]->SetMarkerStyle(23+f); hptc[f]->SetMarkerColor(3); 
    hptu[f]->SetMarkerStyle(23+f); hptu[f]->SetMarkerColor(2); 
    chain[f]->Project(hphoteff_pt[f],"(truth_ph_iRecPhoton>=0&&ph_selected[truth_ph_iRecPhoton]):truth_ph_pt/1000");
    chain[f]->Project(hphoteff_pt_c[f],"(truth_ph_iRecPhoton>=0&&ph_selected[truth_ph_iRecPhoton]):truth_ph_pt/1000","truth_ph_isConv==1");
    chain[f]->Project(hphoteff_pt_u[f],"(truth_ph_iRecPhoton>=0&&ph_selected[truth_ph_iRecPhoton]):truth_ph_pt/1000","truth_ph_isConv==0");
    heta[f]->GetXaxis()->SetTitle("photon true #eta");
    heta[f]->GetYaxis()->SetTitle("efficiency");
    heta[f]->SetMinimum(0.4);
    heta[f]->SetMaximum(1);
    heta[f]->SetMarkerStyle(23+f);
    hetac[f]->SetMarkerStyle(23+f); hetac[f]->SetMarkerColor(3); 
    hetau[f]->SetMarkerStyle(23+f); hetau[f]->SetMarkerColor(2); 
    chain[f]->Project(hphoteff_eta[f],"(truth_ph_iRecPhoton>=0&&ph_selected[truth_ph_iRecPhoton]):truth_ph_eta","truth_ph_pt>25000");
    chain[f]->Project(hphoteff_eta_c[f],"(truth_ph_iRecPhoton>=0&&ph_selected[truth_ph_iRecPhoton]):truth_ph_eta","truth_ph_pt>25000&&truth_ph_isConv==1");
    chain[f]->Project(hphoteff_eta_u[f],"(truth_ph_iRecPhoton>=0&&ph_selected[truth_ph_iRecPhoton]):truth_ph_eta","truth_ph_pt>25000&&truth_ph_isConv==0");
  }

  TCanvas* c = gimeCanvas3("cphoteff_1");
  c->Divide(3,1);
  c->cd(1);
  if (hpt[0]->GetBinContent(hpt[0]->GetMaximumBin())>hpt[1]->GetBinContent(hpt[1]->GetMaximumBin())) {hpt[0]->Draw();  hpt[1]->Draw("same");}
  else {hpt[1]->Draw();  hpt[0]->Draw("same");}
  TLegend* l1 = gimeLegend(0.55,0.2,0.9,0.4) ;
  TString msg1 = "All - " ; msg1 += rel_ref ;
  TString msg2 = "All - " ; msg2 += rel ;
  l1->AddEntry(hpt[0],msg1,"lp");
  l1->AddEntry(hpt[1],msg2,"lp");
  l1->Draw();
  c->cd(2);
  if (hptc[0]->GetBinContent(hptc[0]->GetMaximumBin())>hptc[1]->GetBinContent(hptc[1]->GetMaximumBin())) {hptc[0]->Draw();  hptc[1]->Draw("same");}
  //if (hptc[0]->GetMaximum()>hptc[1]->GetMaximum()) {hptc[0]->Draw();  hptc[1]->Draw("same");}
  else {hptc[1]->Draw();  hptc[0]->Draw("same");}
  TLegend* l1 = gimeLegend(0.55,0.2,0.9,0.4) ;
  TString msg1 = "Conv. - " ; msg1 += rel_ref ;
  TString msg2 = "Conv. - " ; msg2 += rel ;
  l1->AddEntry(hptc[0],msg1,"lp");
  l1->AddEntry(hptc[1],msg2,"lp");
  l1->Draw();
  c->cd(3);
  if (hptu[0]->GetBinContent(hptu[0]->GetMaximumBin())>hptu[1]->GetBinContent(hptu[1]->GetMaximumBin())) {hptu[0]->Draw();  hptu[1]->Draw("same");}
  //if (hptu[0]->GetMaximum()>hptu[1]->GetMaximum()) {hptu[0]->Draw();  hptu[1]->Draw("same");}
  else {hptu[1]->Draw();  hptu[0]->Draw("same");}
  TLegend* l1 = gimeLegend(0.55,0.2,0.9,0.4) ;
  TString msg1 = "Unconv. - " ; msg1 += rel_ref ;
  TString msg2 = "Unconv. - " ; msg2 += rel ;
  l1->AddEntry(hptu[0],msg1,"lp");
  l1->AddEntry(hptu[1],msg2,"lp");
  l1->Draw();
  c->SaveAs("photeff_1.eps");

  TCanvas* c = gimeCanvas3("cphoteff_2");
  c->Divide(3,1);
  c->cd(1); 
  if (heta[0]->GetMaximum()>heta[1]->GetMaximum()) {heta[0]->Draw();  heta[1]->Draw("same");}
  else {heta[1]->Draw();  heta[0]->Draw("same");}
  TLegend* l1 = gimeLegend(0.55,0.2,0.9,0.4) ;
  TString msg1 = "All - " ; msg1 += rel_ref ;
  TString msg2 = "All - " ; msg2 += rel ;
  l1->AddEntry(heta[0],msg1,"lp");
  l1->AddEntry(heta[1],msg2,"lp");
  l1->Draw();
  c->cd(2); 
  if (hetac[0]->GetMaximum()>hetac[1]->GetMaximum()) {hetac[0]->Draw();  hetac[1]->Draw("same");}
  else {hetac[1]->Draw();  hetac[0]->Draw("same");}
  TLegend* l1 = gimeLegend(0.55,0.2,0.9,0.4) ;
  TString msg1 = "Conv. - " ; msg1 += rel_ref ;
  TString msg2 = "Conv. - " ; msg2 += rel ;
  l1->AddEntry(hetac[0],msg1,"lp");
  l1->AddEntry(hetac[1],msg2,"lp");
  l1->Draw();
  c->cd(3); 
  if (hetau[0]->GetMaximum()>hetau[1]->GetMaximum()) {hetau[0]->Draw();  hetau[1]->Draw("same");}
  else {hetau[1]->Draw();  hetau[0]->Draw("same");}
  TLegend* l1 = gimeLegend(0.55,0.2,0.9,0.4) ;
  TString msg1 = "Unconv. - " ; msg1 += rel_ref ;
  TString msg2 = "Unconv. - " ; msg2 += rel ;
  l1->AddEntry(hetau[0],msg1,"lp");
  l1->AddEntry(hetau[1],msg2,"lp");
  l1->Draw();
  c->SaveAs("photeff_2.eps");

  TCanvas* c = gimeCanvas4("cphoteff_3");
  c->Divide(2,2);
  c->cd(1) ; hpt[0]->Draw();  hptu[0]->Draw("same");  hptc[0]->Draw("same");
  TLegend* l1 = gimeLegend(0.55,0.2,0.9,0.4) ;
  TString msg1 = "All - " ; msg1 += rel_ref ;
  TString msg2 = "Unconverted - " ; msg2 += rel_ref ;
  TString msg3 = "Converted - " ; msg3 += rel_ref ;
  l1->AddEntry(hpt[0],msg1,"lp");
  l1->AddEntry(hptu[0],msg2,"lp");
  l1->AddEntry(hptc[0],msg3,"lp");
  l1->Draw();
  c->cd(3) ; hpt[1]->Draw();  hptu[1]->Draw("same");  hptc[1]->Draw("same");
  TLegend* l1 = gimeLegend(0.55,0.2,0.9,0.4) ;
  TString msg1 = "All - " ; msg1 += rel ;
  TString msg2 = "Unconverted - " ; msg2 += rel ;
  TString msg3 = "Converted - " ; msg3 += rel ;
  l1->AddEntry(hpt[0],msg1,"lp");
  l1->AddEntry(hptu[0],msg2,"lp");
  l1->AddEntry(hptc[0],msg3,"lp");
  l1->Draw();
  c->cd(2) ; heta[0]->Draw();  hetau[0]->Draw("same");  hetac[0]->Draw("same");
  TLegend* l1 = gimeLegend(0.55,0.2,0.9,0.4) ;
  TString msg1 = "All - " ; msg1 += rel_ref ;
  TString msg2 = "Unconverted - " ; msg2 += rel_ref ;
  TString msg3 = "Converted - " ; msg3 += rel_ref ;
  l1->AddEntry(heta[0],msg1,"lp");
  l1->AddEntry(hetau[0],msg2,"lp");
  l1->AddEntry(hetac[0],msg3,"lp");
  l1->Draw();
  c->cd(4) ; heta[1]->Draw();  hetau[1]->Draw("same");  hetac[1]->Draw("same");
  TLegend* l1 = gimeLegend(0.55,0.2,0.9,0.4) ;
  TString msg1 = "All - " ; msg1 += rel ;
  TString msg2 = "Unconverted - " ; msg2 += rel ;
  TString msg3 = "Converted - " ; msg3 += rel ;
  l1->AddEntry(heta[0],msg1,"lp");
  l1->AddEntry(hetau[0],msg2,"lp");
  l1->AddEntry(hetac[0],msg3,"lp");
  l1->Draw();

  c->SaveAs("photeff_3.eps");
}


/**************************************************************************************************************************************/


void mass(const TChain* chain[MAXINPUTFILES],TString rel_ref,TString rel) {
  TH1F* h[MAXINPUTFILES];
  TH1F* hnc[MAXINPUTFILES];
  TH1F* hni[MAXINPUTFILES];
  TH1F* hc[MAXINPUTFILES];
  TH1F* hu[MAXINPUTFILES];
  TH1F* hc0[MAXINPUTFILES];
  TH1F* hc1[MAXINPUTFILES];
  TH1F* hc2[MAXINPUTFILES];
  char hmass[MAXINPUTFILES][100];
  char hmassnocut[MAXINPUTFILES][100];
  char hmassnoisol[MAXINPUTFILES][100];
  char hmassc[MAXINPUTFILES][100];
  char hmassu[MAXINPUTFILES][100];
  char hmassc0[MAXINPUTFILES][100];
  char hmassc1[MAXINPUTFILES][100];
  char hmassc2[MAXINPUTFILES][100];

  //cout << chain[0]->GetEntries() << " - " << chain[1]->GetEntries() << endl;

  for (int f=0 ; f<MAXINPUTFILES ; f++) {
    sprintf(hmass[f],"hmass_%d",f);
    h[f]  = new TH1F(hmass[f] ,"Higgs mass",100,90,140);
    sprintf(hmassnocut[f],"hmass_nocut_%d",f);
    hnc[f] = new TH1F(hmassnocut[f], "Higgs mass - Only pT cut",100,90,140);
    sprintf(hmassnoisol[f],"hmass_noisol_%d",f);
    hni[f] = new TH1F(hmassnoisol[f], "Higgs mass - No track isolation",100,90,140);
    sprintf(hmassc[f],"hmassc_%d",f);
    hc[f] = new TH1F(hmassc[f],"Higgs mass, at least 1 true conv",100,90,140);
    sprintf(hmassu[f],"hmassu_%d",f);
    hu[f] = new TH1F(hmassu[f],"Higgs mass, no true conv",100,90,140);
    h[f]->GetXaxis()->SetTitle("mass [GeV]");
    chain[f]->Project(hmass[f],"hgg_mgg/1000","hgg_okInclusive");
    sprintf(hmassc0[f],"hmassc0_%d",f);
    hc0[f] = new TH1F(hmassc0[f],"Higgs mass, unconv",100,90,140);
    sprintf(hmassc1[f],"hmassc1_%d",f);
    hc1[f] = new TH1F(hmassc1[f],"Higgs mass, one track conv.",100,90,140);
    sprintf(hmassc2[f],"hmassc2_%d",f);
    hc2[f] = new TH1F(hmassc2[f],"Higgs mass, double track conv.",100,90,140);
    //TCanvas* c = new TCanvas();
    //h[f]->Fit("gaus","","",117,125);
    chain[f]->Project(hmassu[f],"hgg_mgg/1000","hgg_okInclusive&&ph_isConv_MC[hgg_iPhotonLead]==0&&ph_isConv_MC[hgg_iPhotonSublead]==0");
    chain[f]->Project(hmassc[f],"hgg_mgg/1000","hgg_okInclusive&&(ph_isConv_MC[hgg_iPhotonLead]==1||ph_isConv_MC[hgg_iPhotonSublead]==1)");
    chain[f]->Project(hmassc0[f],"hgg_mgg/1000","hgg_okInclusive&&(ph_convFlag[hgg_iPhotonLead]==0&&ph_convFlag[hgg_iPhotonSublead]==0)");
    chain[f]->Project(hmassc1[f],"hgg_mgg/1000","hgg_okInclusive&&((ph_convFlag[hgg_iPhotonLead]==1&&ph_convFlag[hgg_iPhotonSublead]==1)||(ph_convFlag[hgg_iPhotonLead]==1&&ph_convFlag[hgg_iPhotonSublead]==0)||(ph_convFlag[hgg_iPhotonLead]==0&&ph_convFlag[hgg_iPhotonSublead]==1)||(ph_convFlag[hgg_iPhotonLead]==1&&ph_convFlag[hgg_iPhotonSublead]==2)||(ph_convFlag[hgg_iPhotonLead]==2&&ph_convFlag[hgg_iPhotonSublead]==1))");
    chain[f]->Project(hmassc2[f],"hgg_mgg/1000","hgg_okInclusive&&((ph_convFlag[hgg_iPhotonLead]==2&&ph_convFlag[hgg_iPhotonSublead]==2)||(ph_convFlag[hgg_iPhotonLead]==2&&ph_convFlag[hgg_iPhotonSublead]==0)||(ph_convFlag[hgg_iPhotonLead]==0&&ph_convFlag[hgg_iPhotonSublead]==2))");
    chain[f]->Project(hmassnocut[f],"sqrt((ph_E[0]+ph_E[1])**2-(ph_pt[0]*cos(ph_phi[0])+ph_pt[1]*cos(ph_phi[1]))**2-(ph_pt[0]*sin(ph_phi[0])+ph_pt[1]*sin(ph_phi[1]))**2-(ph_pt[0]*sinh(ph_eta[0])+ph_pt[1]*sinh(ph_eta[1]))**2)/1000","ph_pt[0]>40000 && ph_pt[1]>25000");

    char ch01[300],ch02[300],ch03[300],ch04[300],ch05[300],ch06[300],ch07[300],ch08[300]; 
    char ch12[300],ch13[300],ch14[300],ch15[300],ch16[300],ch17[300],ch18[300];
    char ch23[300],ch24[300],ch25[300],ch26[300],ch27[300],ch28[300]; 
    char ch34[300],ch35[300],ch36[300],ch37[300],ch38[300]; 
    char ch45[300],ch46[300],ch47[300],ch48[300]; 
    char ch56[300],ch57[300],ch58[300]; 
    char ch67[300],ch68[300]; 
    char ch78[300]; 
    sprintf(ch01,"sqrt((ph_E[0]+ph_E[1])**2-(ph_pt[0]*cos(ph_phi[0])+ph_pt[1]*cos(ph_phi[1]))**2-(ph_pt[0]*sin(ph_phi[0])+ph_pt[1]*sin(ph_phi[1]))**2-(ph_pt[0]*sinh(ph_eta[0])+ph_pt[1]*sinh(ph_eta[1]))**2)/1000>>%s",hmassnoisol[f]);
    chain[f]->Draw(ch01,"ph_pt[0]>40000 && ph_pt[1]>25000 && ph_itune2[0]==15 && ph_itune2[1]==15","goff");
    sprintf(ch02,"sqrt((ph_E[0]+ph_E[2])**2-(ph_pt[0]*cos(ph_phi[0])+ph_pt[2]*cos(ph_phi[2]))**2-(ph_pt[0]*sin(ph_phi[0])+ph_pt[2]*sin(ph_phi[2]))**2-(ph_pt[0]*sinh(ph_eta[0])+ph_pt[2]*sinh(ph_eta[2]))**2)/1000>>+%s",hmassnoisol[f]);
    chain[f]->Draw(ch02,"ph_pt[0]>40000 && ph_pt[2]>25000 && ph_itune2[0]==15 && ph_itune2[2]==15 && ph_itune2[1]!=15","goff");
    sprintf(ch03,"sqrt((ph_E[0]+ph_E[3])**2-(ph_pt[0]*cos(ph_phi[0])+ph_pt[3]*cos(ph_phi[3]))**2-(ph_pt[0]*sin(ph_phi[0])+ph_pt[3]*sin(ph_phi[3]))**2-(ph_pt[0]*sinh(ph_eta[0])+ph_pt[3]*sinh(ph_eta[3]))**2)/1000>>+%s",hmassnoisol[f]);
    chain[f]->Draw(ch03,"ph_pt[0]>40000 && ph_pt[3]>25000 && ph_itune2[0]==15 && ph_itune2[3]==15 && ph_itune2[1]!=15 && ph_itune2[2]!=15","goff");
    sprintf(ch04,"sqrt((ph_E[0]+ph_E[4])**2-(ph_pt[0]*cos(ph_phi[0])+ph_pt[4]*cos(ph_phi[4]))**2-(ph_pt[0]*sin(ph_phi[0])+ph_pt[4]*sin(ph_phi[4]))**2-(ph_pt[0]*sinh(ph_eta[0])+ph_pt[4]*sinh(ph_eta[4]))**2)/1000>>+%s",hmassnoisol[f]);
    chain[f]->Draw(ch04,"ph_pt[0]>40000 && ph_pt[4]>25000 && ph_itune2[0]==15 && ph_itune2[4]==15 && ph_itune2[1]!=15 && ph_itune2[2]!=15 && ph_itune2[3]!=15","goff");
    sprintf(ch05,"sqrt((ph_E[0]+ph_E[5])**2-(ph_pt[0]*cos(ph_phi[0])+ph_pt[5]*cos(ph_phi[5]))**2-(ph_pt[0]*sin(ph_phi[0])+ph_pt[5]*sin(ph_phi[5]))**2-(ph_pt[0]*sinh(ph_eta[0])+ph_pt[5]*sinh(ph_eta[5]))**2)/1000>>+%s",hmassnoisol[f]);
    chain[f]->Draw(ch05,"ph_pt[0]>40000 && ph_pt[5]>25000 && ph_itune2[0]==15 && ph_itune2[5]==15 && ph_itune2[1]!=15 && ph_itune2[2]!=15 && ph_itune2[3]!=15 && ph_itune2[4]!=15","goff");
    sprintf(ch06,"sqrt((ph_E[0]+ph_E[6])**2-(ph_pt[0]*cos(ph_phi[0])+ph_pt[6]*cos(ph_phi[6]))**2-(ph_pt[0]*sin(ph_phi[0])+ph_pt[6]*sin(ph_phi[6]))**2-(ph_pt[0]*sinh(ph_eta[0])+ph_pt[6]*sinh(ph_eta[6]))**2)/1000>>+%s",hmassnoisol[f]);
    chain[f]->Draw(ch06,"ph_pt[0]>40000 && ph_pt[6]>25000 && ph_itune2[0]==15 && ph_itune2[6]==15 && ph_itune2[1]!=15 && ph_itune2[2]!=15 && ph_itune2[3]!=15 && ph_itune2[4]!=15 && ph_itune2[5]!=15","goff");
    sprintf(ch07,"sqrt((ph_E[0]+ph_E[7])**2-(ph_pt[0]*cos(ph_phi[0])+ph_pt[7]*cos(ph_phi[7]))**2-(ph_pt[0]*sin(ph_phi[0])+ph_pt[7]*sin(ph_phi[7]))**2-(ph_pt[0]*sinh(ph_eta[0])+ph_pt[7]*sinh(ph_eta[7]))**2)/1000>>+%s",hmassnoisol[f]);
    chain[f]->Draw(ch07,"ph_pt[0]>40000 && ph_pt[7]>25000 && ph_itune2[0]==15 && ph_itune2[7]==15 && ph_itune2[1]!=15 && ph_itune2[2]!=15 && ph_itune2[3]!=15 && ph_itune2[4]!=15 && ph_itune2[5]!=15 && ph_itune2[6]!=15","goff");
    sprintf(ch08,"sqrt((ph_E[0]+ph_E[8])**2-(ph_pt[0]*cos(ph_phi[0])+ph_pt[8]*cos(ph_phi[8]))**2-(ph_pt[0]*sin(ph_phi[0])+ph_pt[8]*sin(ph_phi[8]))**2-(ph_pt[0]*sinh(ph_eta[0])+ph_pt[8]*sinh(ph_eta[8]))**2)/1000>>+%s",hmassnoisol[f]);
    chain[f]->Draw(ch08,"ph_pt[0]>40000 && ph_pt[8]>25000 && ph_itune2[0]==15 && ph_itune2[8]==15 && ph_itune2[1]!=15 && ph_itune2[2]!=15 && ph_itune2[3]!=15 && ph_itune2[4]!=15 && ph_itune2[5]!=15 && ph_itune2[6]!=15 && ph_itune2[7]!=15","goff");

    sprintf(ch12,"sqrt((ph_E[1]+ph_E[2])**2-(ph_pt[1]*cos(ph_phi[1])+ph_pt[2]*cos(ph_phi[2]))**2-(ph_pt[1]*sin(ph_phi[1])+ph_pt[2]*sin(ph_phi[2]))**2-(ph_pt[1]*sinh(ph_eta[1])+ph_pt[2]*sinh(ph_eta[2]))**2)/1000>>+%s",hmassnoisol[f]);
    chain[f]->Draw(ch12,"ph_pt[1]>40000 && ph_pt[2]>25000 && ph_itune2[0]!=15 && ph_itune2[1]==15 && ph_itune2[2]==15","goff");
    sprintf(ch13,"sqrt((ph_E[1]+ph_E[3])**2-(ph_pt[1]*cos(ph_phi[1])+ph_pt[3]*cos(ph_phi[3]))**2-(ph_pt[1]*sin(ph_phi[1])+ph_pt[3]*sin(ph_phi[3]))**2-(ph_pt[1]*sinh(ph_eta[1])+ph_pt[3]*sinh(ph_eta[3]))**2)/1000>>+%d",hmassnoisol[f]);
    chain[f]->Draw(ch13,"ph_pt[1]>40000 && ph_pt[3]>25000 && ph_itune2[0]!=15 && ph_itune2[1]==15 && ph_itune2[3]==15  && ph_itune2[2]!=15","goff");
    sprintf(ch14,"sqrt((ph_E[1]+ph_E[4])**2-(ph_pt[1]*cos(ph_phi[1])+ph_pt[4]*cos(ph_phi[4]))**2-(ph_pt[1]*sin(ph_phi[1])+ph_pt[4]*sin(ph_phi[4]))**2-(ph_pt[1]*sinh(ph_eta[1])+ph_pt[4]*sinh(ph_eta[4]))**2)/1000>>+%d",hmassnoisol[f]);
    chain[f]->Draw(ch14,"ph_pt[1]>40000 && ph_pt[4]>25000 && ph_itune2[0]!=15 && ph_itune2[1]==15 && ph_itune2[4]==15  && ph_itune2[2]!=15 && ph_itune2[3]!=15","goff");
    sprintf(ch15,"sqrt((ph_E[1]+ph_E[5])**2-(ph_pt[1]*cos(ph_phi[1])+ph_pt[5]*cos(ph_phi[5]))**2-(ph_pt[1]*sin(ph_phi[1])+ph_pt[5]*sin(ph_phi[5]))**2-(ph_pt[1]*sinh(ph_eta[1])+ph_pt[5]*sinh(ph_eta[5]))**2)/1000>>+%d",hmassnoisol[f]);
    chain[f]->Draw(ch15,"ph_pt[1]>40000 && ph_pt[5]>25000 && ph_itune2[0]!=15 && ph_itune2[1]==15 && ph_itune2[5]==15  && ph_itune2[2]!=15 && ph_itune2[3]!=15 && ph_itune2[4]!=15","goff");
    sprintf(ch16,"sqrt((ph_E[1]+ph_E[6])**2-(ph_pt[1]*cos(ph_phi[1])+ph_pt[6]*cos(ph_phi[6]))**2-(ph_pt[1]*sin(ph_phi[1])+ph_pt[6]*sin(ph_phi[6]))**2-(ph_pt[1]*sinh(ph_eta[1])+ph_pt[6]*sinh(ph_eta[6]))**2)/1000>>+%d",hmassnoisol[f]);
    chain[f]->Draw(ch16,"ph_pt[1]>40000 && ph_pt[6]>25000 && ph_itune2[0]!=15 && ph_itune2[1]==15 && ph_itune2[6]==15  && ph_itune2[2]!=15 && ph_itune2[3]!=15 && ph_itune2[4]!=15 && ph_itune2[5]!=15","goff");
    sprintf(ch17,"sqrt((ph_E[1]+ph_E[7])**2-(ph_pt[1]*cos(ph_phi[1])+ph_pt[7]*cos(ph_phi[7]))**2-(ph_pt[1]*sin(ph_phi[1])+ph_pt[7]*sin(ph_phi[7]))**2-(ph_pt[1]*sinh(ph_eta[1])+ph_pt[7]*sinh(ph_eta[7]))**2)/1000>>+%d",hmassnoisol[f]);
    chain[f]->Draw(ch17,"ph_pt[1]>40000 && ph_pt[7]>25000 && ph_itune2[0]!=15 && ph_itune2[1]==15 && ph_itune2[7]==15  && ph_itune2[2]!=15 && ph_itune2[3]!=15 && ph_itune2[4]!=15 && ph_itune2[5]!=15 && ph_itune2[6]!=15","goff");
    sprintf(ch18,"sqrt((ph_E[1]+ph_E[8])**2-(ph_pt[1]*cos(ph_phi[1])+ph_pt[8]*cos(ph_phi[8]))**2-(ph_pt[1]*sin(ph_phi[1])+ph_pt[8]*sin(ph_phi[8]))**2-(ph_pt[1]*sinh(ph_eta[1])+ph_pt[8]*sinh(ph_eta[8]))**2)/1000>>+%d",hmassnoisol[f]);
    chain[f]->Draw(ch18,"ph_pt[1]>40000 && ph_pt[8]>25000 && ph_itune2[0]!=15 && ph_itune2[1]==15 && ph_itune2[8]==15 && ph_itune2[2]!=15 && ph_itune2[3]!=15 && ph_itune2[4]!=15 && ph_itune2[5]!=15 && ph_itune2[6]!=15 && ph_itune2[7]!=15","goff");

    sprintf(ch23,"sqrt((ph_E[2]+ph_E[3])**2-(ph_pt[2]*cos(ph_phi[2])+ph_pt[3]*cos(ph_phi[3]))**2-(ph_pt[2]*sin(ph_phi[2])+ph_pt[3]*sin(ph_phi[3]))**2-(ph_pt[2]*sinh(ph_eta[2])+ph_pt[3]*sinh(ph_eta[3]))**2)/1000>>+%d",hmassnoisol[f]);
    chain[f]->Draw(ch23,"ph_pt[2]>40000 && ph_pt[3]>25000 && ph_itune2[0]!=15 && ph_itune2[1]!=15 && ph_itune2[2]==15 && ph_itune2[3]==15","goff");
    sprintf(ch24,"sqrt((ph_E[2]+ph_E[4])**2-(ph_pt[2]*cos(ph_phi[2])+ph_pt[4]*cos(ph_phi[4]))**2-(ph_pt[2]*sin(ph_phi[2])+ph_pt[4]*sin(ph_phi[4]))**2-(ph_pt[2]*sinh(ph_eta[2])+ph_pt[4]*sinh(ph_eta[4]))**2)/1000>>+%d",hmassnoisol[f]);
    chain[f]->Draw(ch24,"ph_pt[2]>40000 && ph_pt[4]>25000 && ph_itune2[0]!=15 && ph_itune2[1]!=15 && ph_itune2[2]==15 && ph_itune2[4]==15  && ph_itune2[3]!=15","goff");
    sprintf(ch25,"sqrt((ph_E[2]+ph_E[5])**2-(ph_pt[2]*cos(ph_phi[2])+ph_pt[5]*cos(ph_phi[5]))**2-(ph_pt[2]*sin(ph_phi[2])+ph_pt[5]*sin(ph_phi[5]))**2-(ph_pt[2]*sinh(ph_eta[2])+ph_pt[5]*sinh(ph_eta[5]))**2)/1000>>+%d",hmassnoisol[f]);
    chain[f]->Draw(ch25,"ph_pt[2]>40000 && ph_pt[5]>25000 && ph_itune2[0]!=15 && ph_itune2[1]!=15 && ph_itune2[2]==15 && ph_itune2[5]==15  && ph_itune2[3]!=15 && ph_itune2[4]!=15","goff");
    sprintf(ch26,"sqrt((ph_E[2]+ph_E[6])**2-(ph_pt[2]*cos(ph_phi[2])+ph_pt[6]*cos(ph_phi[6]))**2-(ph_pt[2]*sin(ph_phi[2])+ph_pt[6]*sin(ph_phi[6]))**2-(ph_pt[2]*sinh(ph_eta[2])+ph_pt[6]*sinh(ph_eta[6]))**2)/1000>>+%d",hmassnoisol[f]);
    chain[f]->Draw(ch26,"ph_pt[2]>40000 && ph_pt[6]>25000 && ph_itune2[0]!=15 && ph_itune2[1]!=15 && ph_itune2[2]==15 && ph_itune2[6]==15  && ph_itune2[3]!=15 && ph_itune2[4]!=15 && ph_itune2[5]!=15","goff");
    sprintf(ch27,"sqrt((ph_E[2]+ph_E[7])**2-(ph_pt[2]*cos(ph_phi[2])+ph_pt[7]*cos(ph_phi[7]))**2-(ph_pt[2]*sin(ph_phi[2])+ph_pt[7]*sin(ph_phi[7]))**2-(ph_pt[2]*sinh(ph_eta[2])+ph_pt[7]*sinh(ph_eta[7]))**2)/1000>>+%d",hmassnoisol[f]);
    chain[f]->Draw(ch27,"ph_pt[2]>40000 && ph_pt[7]>25000 && ph_itune2[0]!=15 && ph_itune2[1]!=15 && ph_itune2[2]==15 && ph_itune2[7]==15  && ph_itune2[3]!=15 && ph_itune2[4]!=15 && ph_itune2[5]!=15 && ph_itune2[6]!=15","goff");
    sprintf(ch28,"sqrt((ph_E[2]+ph_E[8])**2-(ph_pt[2]*cos(ph_phi[2])+ph_pt[8]*cos(ph_phi[8]))**2-(ph_pt[2]*sin(ph_phi[2])+ph_pt[8]*sin(ph_phi[8]))**2-(ph_pt[2]*sinh(ph_eta[2])+ph_pt[8]*sinh(ph_eta[8]))**2)/1000>>+%d",hmassnoisol[f]);
    chain[f]->Draw(ch28,"ph_pt[2]>40000 && ph_pt[8]>25000 && ph_itune2[0]!=15 && ph_itune2[1]!=15 && ph_itune2[2]==15 && ph_itune2[8]==15 && ph_itune2[3]!=15 && ph_itune2[4]!=15 && ph_itune2[5]!=15 && ph_itune2[6]!=15 && ph_itune2[7]!=15","goff");

    sprintf(ch34,"sqrt((ph_E[3]+ph_E[4])**2-(ph_pt[3]*cos(ph_phi[3])+ph_pt[4]*cos(ph_phi[4]))**2-(ph_pt[3]*sin(ph_phi[3])+ph_pt[4]*sin(ph_phi[4]))**2-(ph_pt[3]*sinh(ph_eta[3])+ph_pt[4]*sinh(ph_eta[4]))**2)/1000>>+%d",hmassnoisol[f]);
    chain[f]->Draw(ch34,"ph_pt[3]>40000 && ph_pt[4]>25000 && ph_itune2[0]!=15 && ph_itune2[1]!=15 && ph_itune2[2]!=15 && ph_itune2[3]==15 && ph_itune2[4]==15","goff");
    sprintf(ch35,"sqrt((ph_E[3]+ph_E[5])**2-(ph_pt[3]*cos(ph_phi[3])+ph_pt[5]*cos(ph_phi[5]))**2-(ph_pt[3]*sin(ph_phi[3])+ph_pt[5]*sin(ph_phi[5]))**2-(ph_pt[3]*sinh(ph_eta[3])+ph_pt[5]*sinh(ph_eta[5]))**2)/1000>>+%d",hmassnoisol[f]);
    chain[f]->Draw(ch35,"ph_pt[3]>40000 && ph_pt[5]>25000 && ph_itune2[0]!=15 && ph_itune2[1]!=15 && ph_itune2[2]!=15 && ph_itune2[3]==15 && ph_itune2[5]==15 && ph_itune2[4]!=15","goff");
    sprintf(ch36,"sqrt((ph_E[3]+ph_E[6])**2-(ph_pt[3]*cos(ph_phi[3])+ph_pt[6]*cos(ph_phi[6]))**2-(ph_pt[3]*sin(ph_phi[3])+ph_pt[6]*sin(ph_phi[6]))**2-(ph_pt[3]*sinh(ph_eta[3])+ph_pt[6]*sinh(ph_eta[6]))**2)/1000>>+%d",hmassnoisol[f]);
    chain[f]->Draw(ch36,"ph_pt[3]>40000 && ph_pt[6]>25000 && ph_itune2[0]!=15 && ph_itune2[1]!=15 && ph_itune2[2]!=15 && ph_itune2[3]==15 && ph_itune2[6]==15 && ph_itune2[4]!=15 && ph_itune2[5]!=15","goff");
    sprintf(ch37,"sqrt((ph_E[3]+ph_E[7])**2-(ph_pt[3]*cos(ph_phi[3])+ph_pt[7]*cos(ph_phi[7]))**2-(ph_pt[3]*sin(ph_phi[3])+ph_pt[7]*sin(ph_phi[7]))**2-(ph_pt[3]*sinh(ph_eta[3])+ph_pt[7]*sinh(ph_eta[7]))**2)/1000>>+%d",hmassnoisol[f]);
    chain[f]->Draw(ch37,"ph_pt[3]>40000 && ph_pt[7]>25000 && ph_itune2[0]!=15 && ph_itune2[1]!=15 && ph_itune2[2]!=15 && ph_itune2[3]==15 && ph_itune2[7]==15 && ph_itune2[4]!=15 && ph_itune2[5]!=15 && ph_itune2[6]!=15","goff");
    sprintf(ch38,"sqrt((ph_E[3]+ph_E[8])**2-(ph_pt[3]*cos(ph_phi[3])+ph_pt[8]*cos(ph_phi[8]))**2-(ph_pt[3]*sin(ph_phi[3])+ph_pt[8]*sin(ph_phi[8]))**2-(ph_pt[3]*sinh(ph_eta[3])+ph_pt[8]*sinh(ph_eta[8]))**2)/1000>>+%d",hmassnoisol[f]);
    chain[f]->Draw(ch38,"ph_pt[3]>40000 && ph_pt[8]>25000 && ph_itune2[0]!=15 && ph_itune2[1]!=15 && ph_itune2[2]!=15 && ph_itune2[3]==15 && ph_itune2[8]==15 && ph_itune2[4]!=15 && ph_itune2[5]!=15 && ph_itune2[6]!=15 && ph_itune2[7]!=15","goff");

    sprintf(ch45,"sqrt((ph_E[4]+ph_E[5])**2-(ph_pt[4]*cos(ph_phi[4])+ph_pt[5]*cos(ph_phi[5]))**2-(ph_pt[4]*sin(ph_phi[4])+ph_pt[5]*sin(ph_phi[5]))**2-(ph_pt[4]*sinh(ph_eta[4])+ph_pt[5]*sinh(ph_eta[5]))**2)/1000>>+%d",hmassnoisol[f]);
    chain[f]->Draw(ch45,"ph_pt[4]>40000 && ph_pt[5]>25000 && ph_itune2[0]!=15 && ph_itune2[1]!=15 && ph_itune2[2]!=15 && ph_itune2[3]!=15 && ph_itune2[4]==15 && ph_itune2[5]==15","goff");
    sprintf(ch46,"sqrt((ph_E[4]+ph_E[6])**2-(ph_pt[4]*cos(ph_phi[4])+ph_pt[6]*cos(ph_phi[6]))**2-(ph_pt[4]*sin(ph_phi[4])+ph_pt[6]*sin(ph_phi[6]))**2-(ph_pt[4]*sinh(ph_eta[4])+ph_pt[6]*sinh(ph_eta[6]))**2)/1000>>+%d",hmassnoisol[f]);
    chain[f]->Draw(ch46,"ph_pt[4]>40000 && ph_pt[6]>25000 && ph_itune2[0]!=15 && ph_itune2[1]!=15 && ph_itune2[2]!=15 && ph_itune2[3]!=15 && ph_itune2[4]==15 && ph_itune2[6]==15 && ph_itune2[5]!=15","goff");
    sprintf(ch47,"sqrt((ph_E[4]+ph_E[7])**2-(ph_pt[4]*cos(ph_phi[4])+ph_pt[7]*cos(ph_phi[7]))**2-(ph_pt[4]*sin(ph_phi[4])+ph_pt[7]*sin(ph_phi[7]))**2-(ph_pt[4]*sinh(ph_eta[4])+ph_pt[7]*sinh(ph_eta[7]))**2)/1000>>+%d",hmassnoisol[f]);
    chain[f]->Draw(ch47,"ph_pt[4]>40000 && ph_pt[7]>25000 && ph_itune2[0]!=15 && ph_itune2[1]!=15 && ph_itune2[2]!=15 && ph_itune2[3]!=15 && ph_itune2[4]==15 && ph_itune2[7]==15 && ph_itune2[5]!=15 && ph_itune2[6]!=15","goff");
    sprintf(ch48,"sqrt((ph_E[4]+ph_E[8])**2-(ph_pt[4]*cos(ph_phi[4])+ph_pt[8]*cos(ph_phi[8]))**2-(ph_pt[4]*sin(ph_phi[4])+ph_pt[8]*sin(ph_phi[8]))**2-(ph_pt[4]*sinh(ph_eta[4])+ph_pt[8]*sinh(ph_eta[8]))**2)/1000>>+%d",hmassnoisol[f]);
    chain[f]->Draw(ch48,"ph_pt[4]>40000 && ph_pt[8]>25000 && ph_itune2[0]!=15 && ph_itune2[1]!=15 && ph_itune2[2]!=15 && ph_itune2[3]!=15 && ph_itune2[4]==15 && ph_itune2[8]==15 && ph_itune2[5]!=15 && ph_itune2[6]!=15 && ph_itune2[7]!=15","goff");

    sprintf(ch56,"sqrt((ph_E[5]+ph_E[6])**2-(ph_pt[5]*cos(ph_phi[5])+ph_pt[6]*cos(ph_phi[6]))**2-(ph_pt[5]*sin(ph_phi[5])+ph_pt[6]*sin(ph_phi[6]))**2-(ph_pt[5]*sinh(ph_eta[5])+ph_pt[6]*sinh(ph_eta[6]))**2)/1000>>+%d",hmassnoisol[f]);
    chain[f]->Draw(ch56,"ph_pt[5]>40000 && ph_pt[6]>25000 && ph_itune2[0]!=15 && ph_itune2[1]!=15 && ph_itune2[2]!=15 && ph_itune2[3]!=15 && ph_itune2[4]!=15 && ph_itune2[5]==15 && ph_itune2[6]==15","goff");
    sprintf(ch57,"sqrt((ph_E[5]+ph_E[7])**2-(ph_pt[5]*cos(ph_phi[5])+ph_pt[7]*cos(ph_phi[7]))**2-(ph_pt[5]*sin(ph_phi[5])+ph_pt[7]*sin(ph_phi[7]))**2-(ph_pt[5]*sinh(ph_eta[5])+ph_pt[7]*sinh(ph_eta[7]))**2)/1000>>+%d",hmassnoisol[f]);
    chain[f]->Draw(ch57,"ph_pt[5]>40000 && ph_pt[7]>25000 && ph_itune2[0]!=15 && ph_itune2[1]!=15 && ph_itune2[2]!=15 && ph_itune2[3]!=15 && ph_itune2[4]!=15 && ph_itune2[5]==15 && ph_itune2[7]==15 && ph_itune2[6]!=15","goff");
    sprintf(ch58,"sqrt((ph_E[5]+ph_E[8])**2-(ph_pt[5]*cos(ph_phi[5])+ph_pt[8]*cos(ph_phi[8]))**2-(ph_pt[5]*sin(ph_phi[5])+ph_pt[8]*sin(ph_phi[8]))**2-(ph_pt[5]*sinh(ph_eta[5])+ph_pt[8]*sinh(ph_eta[8]))**2)/1000>>+%d",hmassnoisol[f]);
    chain[f]->Draw(ch58,"ph_pt[5]>40000 && ph_pt[8]>25000 && ph_itune2[0]!=15 && ph_itune2[1]!=15 && ph_itune2[2]!=15 && ph_itune2[3]!=15 && ph_itune2[4]!=15 && ph_itune2[5]==15 && ph_itune2[8]==15 && ph_itune2[6]!=15 && ph_itune2[7]!=15","goff");

    sprintf(ch67,"sqrt((ph_E[6]+ph_E[7])**2-(ph_pt[6]*cos(ph_phi[6])+ph_pt[7]*cos(ph_phi[7]))**2-(ph_pt[6]*sin(ph_phi[6])+ph_pt[7]*sin(ph_phi[7]))**2-(ph_pt[6]*sinh(ph_eta[6])+ph_pt[7]*sinh(ph_eta[7]))**2)/1000>>+%d",hmassnoisol[f]);
    chain[f]->Draw(ch67,"ph_pt[6]>40000 && ph_pt[7]>25000 && ph_itune2[0]!=15 && ph_itune2[1]!=15 && ph_itune2[2]!=15 && ph_itune2[3]!=15 && ph_itune2[4]!=15 && ph_itune2[5]!=15 && ph_itune2[6]==15 && ph_itune2[7]==15","goff");
    sprintf(ch68,"sqrt((ph_E[6]+ph_E[8])**2-(ph_pt[6]*cos(ph_phi[6])+ph_pt[8]*cos(ph_phi[8]))**2-(ph_pt[6]*sin(ph_phi[6])+ph_pt[8]*sin(ph_phi[8]))**2-(ph_pt[6]*sinh(ph_eta[6])+ph_pt[8]*sinh(ph_eta[8]))**2)/1000>>+%d",hmassnoisol[f]);
    chain[f]->Draw(ch68,"ph_pt[6]>40000 && ph_pt[8]>25000 && ph_itune2[0]!=15 && ph_itune2[1]!=15 && ph_itune2[2]!=15 && ph_itune2[3]!=15 && ph_itune2[4]!=15 && ph_itune2[5]!=15 && ph_itune2[6]==15 && ph_itune2[8]==15 && ph_itune2[7]!=15","goff");

    sprintf(ch78,"sqrt((ph_E[7]+ph_E[8])**2-(ph_pt[7]*cos(ph_phi[7])+ph_pt[8]*cos(ph_phi[8]))**2-(ph_pt[7]*sin(ph_phi[7])+ph_pt[8]*sin(ph_phi[8]))**2-(ph_pt[7]*sinh(ph_eta[7])+ph_pt[8]*sinh(ph_eta[8]))**2)/1000>>+%d",hmassnoisol[f]);
    chain[f]->Draw(ch78,"ph_pt[7]>40000 && ph_pt[8]>25000 && ph_itune2[0]!=15 && ph_itune2[1]!=15 && ph_itune2[2]!=15 && ph_itune2[3]!=15 && ph_itune2[4]!=15 && ph_itune2[5]!=15 && ph_itune2[7]==15 && ph_itune2[8]==15 && ph_itune2[6]!=15","goff");
 
  }

  float scale1 = 1.;
  if (chain[0]->GetEntries()!=chain[1]->GetEntries()) {
    scale1 = chain[0]->GetEntries()*1./chain[1]->GetEntries();
    h[1]->Scale(chain[0]->GetEntries()*1./chain[1]->GetEntries());
    hnc[1]->Scale(chain[0]->GetEntries()*1./chain[1]->GetEntries());
    hni[1]->Scale(chain[0]->GetEntries()*1./chain[1]->GetEntries());
    hc[1]->Scale(chain[0]->GetEntries()*1./chain[1]->GetEntries());
    hu[1]->Scale(chain[0]->GetEntries()*1./chain[1]->GetEntries());
    hc0[1]->Scale(chain[0]->GetEntries()*1./chain[1]->GetEntries());
    hc1[1]->Scale(chain[0]->GetEntries()*1./chain[1]->GetEntries());
    hc2[1]->Scale(chain[0]->GetEntries()*1./chain[1]->GetEntries());
  }

  TCanvas* c = gimeCanvas3("cmass_1");
  c->Divide(3,1);
  c->cd(1);
  hc0[1]->SetLineStyle(2);
  hc0[0]->Fit("gaus","0","goff",117,125);
  hc0[1]->Fit("gaus","0","goff",117,125);
  if (hc0[0]->GetMaximum()>hc0[1]->GetMaximum()) {hc0[0]->Draw(); hc0[1]->Draw("same");}
  else {hc0[1]->Draw(); hc0[0]->Draw("same");}
  float nbevent0 = hc0[0]->GetEntries() ;
  float nbevent1 = hc0[1]->GetEntries()*scale1 ;
  float avg00 = hc0[0]->GetFunction("gaus")->GetParameter(1) ;
  float sig00 = hc0[0]->GetFunction("gaus")->GetParameter(2) ;
  float avg01 = hc0[1]->GetFunction("gaus")->GetParameter(1) ;
  float sig01 = hc0[1]->GetFunction("gaus")->GetParameter(2) ;
  TString avg_msg00 = "< > = " ; avg_msg00 += avg00 ; avg_msg00 += " - " ; avg_msg00 += rel_ref;
  TString sig_msg00 = "sigma = " ; sig_msg00 += sig00 ; sig_msg00 += " - " ; sig_msg00 += rel_ref;
  TString avg_msg01 = "< > = " ; avg_msg01 += avg01 ; avg_msg01 += " - " ; avg_msg01 += rel;
  TString sig_msg01 = "sigma = " ; sig_msg01 += sig01 ; sig_msg01 += " - " ; sig_msg01 += rel;
  TString nbevent0_msg = "NB events = " ; nbevent0_msg += nbevent0 ; nbevent0_msg += " - " ; nbevent0_msg += rel_ref;
  TString nbevent1_msg = "NB events = " ; nbevent1_msg += nbevent1 ; nbevent1_msg += " - " ; nbevent1_msg += rel;
  TText txt1;
  txt1.DrawTextNDC(0.2,0.7,nbevent0_msg);
  txt1.DrawTextNDC(0.2,0.65,avg_msg00);
  txt1.DrawTextNDC(0.2,0.6,sig_msg00);
  txt1.DrawTextNDC(0.2,0.5,nbevent1_msg);
  txt1.DrawTextNDC(0.2,0.45,avg_msg01);
  txt1.DrawTextNDC(0.2,0.4,sig_msg01);
  TLegend* l1 = gimeLegend(0.6,0.6,0.9,0.8) ;
  TString msg1 = "Unconv. photons - " ; msg1 += rel_ref;
  TString msg2 = "Unconv. photons - " ; msg2 += rel;
  l1->AddEntry(hc0[0],msg1,"l");
  l1->AddEntry(hc0[1],msg2,"lf");
  l1->Draw();
  c->cd(2);
  hc1[0]->Fit("gaus","0","goff",117,125);
  hc1[1]->Fit("gaus","0","goff",117,125);
  hc1[1]->SetLineStyle(2); 
  if (hc1[0]->GetMaximum()>hc1[1]->GetMaximum()) {hc1[0]->Draw(); hc1[1]->Draw("same");}
  else {hc1[1]->Draw(); hc1[0]->Draw("same");}
  float nbevent0 = hc1[0]->GetEntries() ;
  float nbevent1 = hc1[1]->GetEntries()*scale1 ;
  float avg10 = hc1[0]->GetFunction("gaus")->GetParameter(1) ;
  float sig10 = hc1[0]->GetFunction("gaus")->GetParameter(2) ;
  float avg11 = hc1[1]->GetFunction("gaus")->GetParameter(1) ;
  float sig11 = hc1[1]->GetFunction("gaus")->GetParameter(2) ;
  TString avg_msg10 = "< > = " ; avg_msg10 += avg10 ; avg_msg10 += " - " ; avg_msg10 += rel_ref;
  TString sig_msg10 = "sigma = " ; sig_msg10 += sig10 ; sig_msg10 += " - " ; sig_msg10 += rel_ref;
  TString avg_msg11 = "< > = " ; avg_msg11 += avg11 ; avg_msg11 += " - " ; avg_msg11 += rel;
  TString sig_msg11 = "sigma = " ; sig_msg11 += sig11 ;sig_msg11 += " - " ; sig_msg11 += rel;
  TString nbevent0_msg = "NB events = " ; nbevent0_msg += nbevent0 ; nbevent0_msg += " - " ; nbevent0_msg += rel_ref;
  TString nbevent1_msg = "NB events = " ; nbevent1_msg += nbevent1 ; nbevent1_msg += " - " ; nbevent1_msg += rel;
  TText txt1;
  txt1.DrawTextNDC(0.2,0.7,nbevent0_msg);
  txt1.DrawTextNDC(0.2,0.65,avg_msg10);
  txt1.DrawTextNDC(0.2,0.6,sig_msg10);
  txt1.DrawTextNDC(0.2,0.5,nbevent1_msg);
  txt1.DrawTextNDC(0.2,0.45,avg_msg11);
  txt1.DrawTextNDC(0.2,0.4,sig_msg11);
  TLegend* l1 = gimeLegend(0.6,0.6,0.9,0.8) ;
  TString msg1 = "Single track conv. - " ; msg1 += rel_ref;
  TString msg2 = "Single track conv. - " ; msg2 += rel;
  l1->AddEntry(hc1[0],msg1,"l");
  l1->AddEntry(hc1[1],msg2,"lf");
  l1->Draw();
  c->cd(3);
  hc2[1]->SetLineStyle(2);
  hc2[0]->Fit("gaus","0","",117,125);
  hc2[1]->Fit("gaus","0","",117,125);
  if (hc2[0]->GetMaximum()>hc2[1]->GetMaximum()) {hc2[0]->Draw(); hc2[1]->Draw("same");}
  else {hc2[1]->Draw(); hc2[0]->Draw("same");}
  float nbevent0 = hc2[0]->GetEntries() ;
  float nbevent1 = hc2[1]->GetEntries()*scale1 ;
  float avg20 = hc2[0]->GetFunction("gaus")->GetParameter(1) ;
  float sig20 = hc2[0]->GetFunction("gaus")->GetParameter(2) ;
  float avg21 = hc2[1]->GetFunction("gaus")->GetParameter(1) ;
  float sig21 = hc2[1]->GetFunction("gaus")->GetParameter(2) ;
  TString avg_msg20 = "< > = " ; avg_msg20 += avg20 ; avg_msg20 += " - " ; avg_msg20 += rel_ref;
  TString sig_msg20 = "sigma = " ; sig_msg20 += sig20 ; sig_msg20 += " - " ; sig_msg20 += rel_ref;
  TString avg_msg21 = "< > = " ; avg_msg21 += avg21 ; avg_msg21 += " - " ; avg_msg21 += rel;
  TString sig_msg21 = "sigma = " ; sig_msg21 += sig21 ; sig_msg21 += " - " ; sig_msg21 += rel;
  TString nbevent0_msg = "NB events = " ; nbevent0_msg += nbevent0 ; nbevent0_msg += " - " ; nbevent0_msg += rel_ref;
  TString nbevent1_msg = "NB events = " ; nbevent1_msg += nbevent1 ; nbevent1_msg += " - " ; nbevent1_msg += rel;
  TText txt1;
  txt1.DrawTextNDC(0.2,0.7,nbevent0_msg);
  txt1.DrawTextNDC(0.2,0.65,avg_msg20);
  txt1.DrawTextNDC(0.2,0.6,sig_msg20);
  txt1.DrawTextNDC(0.2,0.5,nbevent1_msg);
  txt1.DrawTextNDC(0.2,0.45,avg_msg21);
  txt1.DrawTextNDC(0.2,0.4,sig_msg21);
  TLegend* l1 = gimeLegend(0.6,0.6,0.9,0.8) ;
  TString msg1 = "Double track conv. - " ; msg1 += rel_ref;
  TString msg2 = "Double track conv. - " ; msg2 += rel;
  l1->AddEntry(hc2[0],msg1,"l");
  l1->AddEntry(hc2[1],msg2,"lf");
  l1->Draw();
  c->SaveAs("mass_1.eps");

  TCanvas* c = gimeCanvas2("cmass_2");
  c->Divide(1,2);
  c->cd(1);
  h[0]->Fit("gaus","","",117,125); h[1]->Fit("gaus","","",117,125);
  if (h[0]->GetMaximum()>h[1]->GetMaximum()) {h[0]->Draw(); h[1]->SetLineStyle(2); h[1]->Draw("same");}
  else {h[1]->SetLineStyle(2); h[1]->Draw(); h[0]->Draw("same");}
  hc[0]->SetFillStyle(3006); hc[0]->SetFillColor(5); hc[0]->Draw("same"); 
  hc[1]->SetFillColor(5); hc[1]->SetFillStyle(3007); hc[1]->Draw("same");
  float nbevent0 = h[0]->GetEntries() ;
  float nbevent1 = h[1]->GetEntries()*scale1 ;
  float avg10 = h[0]->GetFunction("gaus")->GetParameter(1) ;
  float sig10 = h[0]->GetFunction("gaus")->GetParameter(2) ;
  float avg11 = h[1]->GetFunction("gaus")->GetParameter(1) ;
  float sig11 = h[1]->GetFunction("gaus")->GetParameter(2) ;
  TString avg_msg10 = "< > = " ; avg_msg10 += avg10 ; avg_msg10 += " - " ; avg_msg10 += rel_ref;
  TString sig_msg10 = "sigma = " ; sig_msg10 += sig10 ; sig_msg10 += " - " ; sig_msg10 += rel_ref;
  TString avg_msg11 = "< > = " ; avg_msg11 += avg11 ; avg_msg11 += " - " ; avg_msg11 += rel;
  TString sig_msg11 = "sigma = " ; sig_msg11 += sig11 ;sig_msg11 += " - " ; sig_msg11 += rel;
  TString nbevent0_msg = "NB events = " ; nbevent0_msg += nbevent0 ; nbevent0_msg += " - " ; nbevent0_msg += rel_ref;
  TString nbevent1_msg = "NB events = " ; nbevent1_msg += nbevent1 ; nbevent1_msg += " - " ; nbevent1_msg += rel;
  TText txt1;
  txt1.DrawTextNDC(0.2,0.7,nbevent0_msg);
  txt1.DrawTextNDC(0.2,0.65,avg_msg10);
  txt1.DrawTextNDC(0.2,0.6,sig_msg10);
  txt1.DrawTextNDC(0.2,0.5,nbevent1_msg);
  txt1.DrawTextNDC(0.2,0.45,avg_msg11);
  txt1.DrawTextNDC(0.2,0.4,sig_msg11);
  TLegend* l1 = gimeLegend(0.6,0.6,0.9,0.8) ;
  TString msg1 = "All events - " ; msg1 += rel_ref;
  TString msg2 = "All events - " ; msg2 += rel;
  l1->AddEntry(h[0],msg1,"l");
  l1->AddEntry(hc[0],msg2,"lf");
  l1->Draw();

  c->cd(2);
  hu[0]->Fit("gaus","","",117,125);
  hu[1]->Fit("gaus","","",117,125);
  if (hu[0]->GetMaximum()>hu[1]->GetMaximum()) {hu[0]->Draw(); hu[1]->SetLineStyle(2); hu[1]->Draw("same");}
  else {hu[1]->SetLineStyle(2); hu[1]->Draw(); hu[0]->Draw("same");}
  float nbevent0 = hu[0]->GetEntries() ;
  float nbevent1 = hu[1]->GetEntries()*scale1 ;
  float avg10 = hu[0]->GetFunction("gaus")->GetParameter(1) ;
  float sig10 = hu[0]->GetFunction("gaus")->GetParameter(2) ;
  float avg11 = hu[1]->GetFunction("gaus")->GetParameter(1) ;
  float sig11 = hu[1]->GetFunction("gaus")->GetParameter(2) ;
  TString avg_msg10 = "< > = " ; avg_msg10 += avg10 ; avg_msg10 += " - " ; avg_msg10 += rel_ref;
  TString sig_msg10 = "sigma = " ; sig_msg10 += sig10 ; sig_msg10 += " - " ; sig_msg10 += rel_ref;
  TString avg_msg11 = "< > = " ; avg_msg11 += avg11 ; avg_msg11 += " - " ; avg_msg11 += rel;
  TString sig_msg11 = "sigma = " ; sig_msg11 += sig11 ;sig_msg11 += " - " ; sig_msg11 += rel;
  TString nbevent0_msg = "NB events = " ; nbevent0_msg += nbevent0 ; nbevent0_msg += " - " ; nbevent0_msg += rel_ref;
  TString nbevent1_msg = "NB events = " ; nbevent1_msg += nbevent1 ; nbevent1_msg += " - " ; nbevent1_msg += rel;
  TText txt1;
  txt1.DrawTextNDC(0.2,0.7,nbevent0_msg);
  txt1.DrawTextNDC(0.2,0.65,avg_msg10);
  txt1.DrawTextNDC(0.2,0.6,sig_msg10);
  txt1.DrawTextNDC(0.2,0.5,nbevent1_msg);
  txt1.DrawTextNDC(0.2,0.45,avg_msg11);
  txt1.DrawTextNDC(0.2,0.4,sig_msg11);
  TLegend* l2 = gimeLegend(0.6,0.6,0.9,0.8) ;
  TString msg1 = "No true conv. - " ; msg1 += rel_ref;
  TString msg2 = "No true conv. - " ; msg2 += rel;
  l2->AddEntry(hu[0],msg1,"l");
  l2->AddEntry(hu[1],msg2,"l");
  l2->Draw();

  c->SaveAs("mass_2.eps");



  TCanvas* c = gimeCanvas4("cmass_3");
  c->Divide(2,2);
  c->cd(1);
  h[0]->Draw();
  hc[0]->SetFillColor(5);
  hc[0]->Draw("same");
  float avg = h[0]->GetFunction("gaus")->GetParameter(1) ;
  float sig = h[0]->GetFunction("gaus")->GetParameter(2) ;
  TString avg_msg = "< > = " ; avg_msg += avg ; avg_msg += " - " ; avg_msg += rel_ref;
  TString sig_msg = "sigma = " ; sig_msg += sig ; sig_msg += " - " ; sig_msg += rel_ref;
  TText txt;
  txt.DrawTextNDC(0.2,0.8,avg_msg);
  txt.DrawTextNDC(0.2,0.75,sig_msg);
  TLegend* l1 = gimeLegend(0.6,0.6,0.9,0.8) ;
  TString msg1 = "All events - " ; msg1 += rel_ref;
  TString msg2 = "At least one true conv. - " ; msg2 += rel_ref;
  l1->AddEntry(h[0],msg1,"l");
  l1->AddEntry(hc[0],msg2,"lf");
  l1->Draw();
  c->cd(3);
  hu[0]->Fit("gaus","","",117,125);
  hu[0]->Draw();
  avg = hu[0]->GetFunction("gaus")->GetParameter(1) ;
  sig = hu[0]->GetFunction("gaus")->GetParameter(2) ;
  avg_msg = "< > = " ; avg_msg += avg ; avg_msg += " - " ; avg_msg += rel_ref;
  sig_msg = "sigma = " ; sig_msg += sig ; sig_msg += " - " ; sig_msg += rel_ref;
  txt.DrawTextNDC(0.2,0.8,avg_msg);
  txt.DrawTextNDC(0.2,0.75,sig_msg);  
  TLegend* l2 = gimeLegend(0.6,0.6,0.9,0.8) ;
  TString msg1 = "No true conv. - " ; msg1 += rel_ref;
  l2->AddEntry(hu[0],msg1,"l");
  l2->Draw();

  c->cd(2);
  h[1]->Draw();
  hc[1]->SetFillColor(5);
  hc[1]->Draw("same");
  float avg = h[1]->GetFunction("gaus")->GetParameter(1) ;
  float sig = h[1]->GetFunction("gaus")->GetParameter(2) ;
  TString avg_msg = "< > = " ; avg_msg += avg ; avg_msg += " - " ; avg_msg += rel;
  TString sig_msg = "sigma = " ; sig_msg += sig ; sig_msg += " - " ; sig_msg += rel;
  TText txt;
  txt.DrawTextNDC(0.2,0.8,avg_msg);
  txt.DrawTextNDC(0.2,0.75,sig_msg);
  TLegend* l1 = gimeLegend(0.6,0.6,0.9,0.8) ;
  TString msg1 = "All events - " ; msg1 += rel;
  TString msg2 = "At least one true conv. - " ; msg2 += rel;
  l1->AddEntry(h[1],msg1,"l");
  l1->AddEntry(hc[1],msg2,"lf");
  l1->Draw();
  c->cd(4);
  hu[1]->Fit("gaus","","",117,125);
  hu[1]->Draw();
  avg = hu[1]->GetFunction("gaus")->GetParameter(1) ;
  sig = hu[1]->GetFunction("gaus")->GetParameter(2) ;
  avg_msg = "< > = " ; avg_msg += avg ; avg_msg += " - " ; avg_msg += rel;
  sig_msg = "sigma = " ; sig_msg += sig ; sig_msg += " - " ; sig_msg += rel;
  txt.DrawTextNDC(0.2,0.8,avg_msg);
  txt.DrawTextNDC(0.2,0.75,sig_msg);  
  TLegend* l2 = gimeLegend(0.6,0.6,0.9,0.8) ;
  TString msg1 = "No true conv. - " ; msg1 += rel;
  l2->AddEntry(hu[1],msg1,"l");
  l2->Draw();

  c->SaveAs("mass_3.eps");

  TCanvas* c = gimeCanvas3("cmass_4");
  c->Divide(3,1);
  c->cd(1);
  h[1]->SetLineStyle(2);
  if (h[0]->GetMaximum()>h[1]->GetMaximum()) {h[0]->Draw(); h[1]->Draw("Same");}
  else {h[1]->Draw(); h[0]->Draw("Same");}
  TLegend* l1 = gimeLegend(0.6,0.6,0.9,0.8) ;
  TString msg1 = "All - no track isol. - " ; msg1 += rel_ref;
  TString msg2 = "All - no track isol. - " ; msg2 += rel;
  l1->AddEntry(h[0],msg1,"l");
  l1->AddEntry(h[1],msg2,"lf");
  l1->Draw();
  float nbevent0 = h[0]->GetEntries() ;
  float nbevent1 = h[1]->GetEntries()*scale1 ;
  float avg20 = h[0]->GetFunction("gaus")->GetParameter(1) ;
  float sig20 = h[0]->GetFunction("gaus")->GetParameter(2) ;
  float avg21 = h[1]->GetFunction("gaus")->GetParameter(1) ;
  float sig21 = h[1]->GetFunction("gaus")->GetParameter(2) ;
  TString avg_msg20 = "< > = " ; avg_msg20 += avg20 ; avg_msg20 += " - " ; avg_msg20 += rel_ref;
  TString sig_msg20 = "sigma = " ; sig_msg20 += sig20 ; sig_msg20 += " - " ; sig_msg20 += rel_ref;
  TString avg_msg21 = "< > = " ; avg_msg21 += avg21 ; avg_msg21 += " - " ; avg_msg21 += rel;
  TString sig_msg21 = "sigma = " ; sig_msg21 += sig21 ; sig_msg21 += " - " ; sig_msg21 += rel;
  TString nbevent0_msg = "NB events = " ; nbevent0_msg += nbevent0 ; nbevent0_msg += " - " ; nbevent0_msg += rel_ref;
  TString nbevent1_msg = "NB events = " ; nbevent1_msg += nbevent1 ; nbevent1_msg += " - " ; nbevent1_msg += rel;
  TText txt1;
  txt1.DrawTextNDC(0.2,0.7,nbevent0_msg);
  txt1.DrawTextNDC(0.2,0.65,avg_msg20);
  txt1.DrawTextNDC(0.2,0.6,sig_msg20);
  txt1.DrawTextNDC(0.2,0.5,nbevent1_msg);
  txt1.DrawTextNDC(0.2,0.45,avg_msg21);
  txt1.DrawTextNDC(0.2,0.4,sig_msg21);

  c->cd(2);
  hni[0]->Fit("gaus","","",117,125); hni[1]->Fit("gaus","","",117,125);
  hni[1]->SetLineStyle(2);
  if (hni[0]->GetMaximum()>hni[1]->GetMaximum()) {hni[0]->Draw(); hni[1]->Draw("Same");}
  else {hni[1]->Draw(); hni[0]->Draw("Same");}
  TLegend* l1 = gimeLegend(0.6,0.6,0.9,0.8) ;
  TString msg1 = "All - no track isol. - " ; msg1 += rel_ref;
  TString msg2 = "All - no track isol. - " ; msg2 += rel;
  l1->AddEntry(hni[0],msg1,"l");
  l1->AddEntry(hni[1],msg2,"lf");
  l1->Draw();
  float nbevent0 = hni[0]->GetEntries() ;
  float nbevent1 = hni[1]->GetEntries()*scale1 ;
  float avg20 = hni[0]->GetFunction("gaus")->GetParameter(1) ;
  float sig20 = hni[0]->GetFunction("gaus")->GetParameter(2) ;
  float avg21 = hni[1]->GetFunction("gaus")->GetParameter(1) ;
  float sig21 = hni[1]->GetFunction("gaus")->GetParameter(2) ;
  TString avg_msg20 = "< > = " ; avg_msg20 += avg20 ; avg_msg20 += " - " ; avg_msg20 += rel_ref;
  TString sig_msg20 = "sigma = " ; sig_msg20 += sig20 ; sig_msg20 += " - " ; sig_msg20 += rel_ref;
  TString avg_msg21 = "< > = " ; avg_msg21 += avg21 ; avg_msg21 += " - " ; avg_msg21 += rel;
  TString sig_msg21 = "sigma = " ; sig_msg21 += sig21 ; sig_msg21 += " - " ; sig_msg21 += rel;
  TString nbevent0_msg = "NB events = " ; nbevent0_msg += nbevent0 ; nbevent0_msg += " - " ; nbevent0_msg += rel_ref;
  TString nbevent1_msg = "NB events = " ; nbevent1_msg += nbevent1 ; nbevent1_msg += " - " ; nbevent1_msg += rel;
  TText txt1;
  txt1.DrawTextNDC(0.2,0.7,nbevent0_msg);
  txt1.DrawTextNDC(0.2,0.65,avg_msg20);
  txt1.DrawTextNDC(0.2,0.6,sig_msg20);
  txt1.DrawTextNDC(0.2,0.5,nbevent1_msg);
  txt1.DrawTextNDC(0.2,0.45,avg_msg21);
  txt1.DrawTextNDC(0.2,0.4,sig_msg21);

  c->cd(3);
  hnc[0]->Fit("gaus","","",117,125); hnc[1]->Fit("gaus","","",117,125);
  hnc[1]->SetLineStyle(2);
  if (hnc[0]->GetMaximum()>hnc[1]->GetMaximum()) {hnc[0]->Draw(); hnc[1]->Draw("Same");}
  else {hnc[1]->Draw(); hnc[0]->Draw("Same");}
  TLegend* l1 = gimeLegend(0.6,0.6,0.9,0.8) ;
  TString msg1 = "All - no track isol. - " ; msg1 += rel_ref;
  TString msg2 = "All - no track isol. - " ; msg2 += rel;
  l1->AddEntry(hnc[0],msg1,"l");
  l1->AddEntry(hnc[1],msg2,"lf");
  l1->Draw();
  float nbevent0 = hnc[0]->GetEntries() ;
  float nbevent1 = hnc[1]->GetEntries()*scale1 ;
  float avg20 = hnc[0]->GetFunction("gaus")->GetParameter(1) ;
  float sig20 = hnc[0]->GetFunction("gaus")->GetParameter(2) ;
  float avg21 = hnc[1]->GetFunction("gaus")->GetParameter(1) ;
  float sig21 = hnc[1]->GetFunction("gaus")->GetParameter(2) ;
  TString avg_msg20 = "< > = " ; avg_msg20 += avg20 ; avg_msg20 += " - " ; avg_msg20 += rel_ref;
  TString sig_msg20 = "sigma = " ; sig_msg20 += sig20 ; sig_msg20 += " - " ; sig_msg20 += rel_ref;
  TString avg_msg21 = "< > = " ; avg_msg21 += avg21 ; avg_msg21 += " - " ; avg_msg21 += rel;
  TString sig_msg21 = "sigma = " ; sig_msg21 += sig21 ; sig_msg21 += " - " ; sig_msg21 += rel;
  TString nbevent0_msg = "NB events = " ; nbevent0_msg += nbevent0 ; nbevent0_msg += " - " ; nbevent0_msg += rel_ref;
  TString nbevent1_msg = "NB events = " ; nbevent1_msg += nbevent1 ; nbevent1_msg += " - " ; nbevent1_msg += rel;
  TText txt1;
  txt1.DrawTextNDC(0.2,0.7,nbevent0_msg);
  txt1.DrawTextNDC(0.2,0.65,avg_msg20);
  txt1.DrawTextNDC(0.2,0.6,sig_msg20);
  txt1.DrawTextNDC(0.2,0.5,nbevent1_msg);
  txt1.DrawTextNDC(0.2,0.45,avg_msg21);
  txt1.DrawTextNDC(0.2,0.4,sig_msg21);
  c->SaveAs("mass_4.eps");

}


/**************************************************************************************************************************************/


void conveff(const TChain* chain[MAXINPUTFILES],TString rel_ref,TString rel) {
  TProfile* h2[MAXINPUTFILES];
  TProfile* h12[MAXINPUTFILES];
  char hconveff_2[MAXINPUTFILES][100];
  char hconveff_12[MAXINPUTFILES][100];
  for (int f=0 ; f<MAXINPUTFILES ; f++) {
    sprintf(hconveff_2[f],"hconveff_2_%d",f);
    h2[f] = new TProfile(hconveff_2[f],"Conversion efficiency vs radius (conv2)",16,0,800);
    sprintf(hconveff_12[f],"hconveff_12_%d",f);
    h12[f] = new TProfile(hconveff_12[f],"Conversion efficiency vs radius (conv1+conv2)",16,0,800);
    h2[f]->GetXaxis()->SetTitle("true conv radius [mm]");
    h2[f]->GetYaxis()->SetTitle("efficiency");
    h2[f]->SetMinimum(0);
    h2[f]->SetMaximum(1);
    h2[f]->SetMarkerStyle(23+f); h2[f]->SetMarkerColor(2); 
    h12[f]->SetMarkerStyle(23+f); h12[f]->SetMarkerColor(1); 
    chain[f]->Project(hconveff_2[f],"(ph_convFlag==2):ph_RConv_MC","ph_selected&&ph_matchMC&&ph_isConv_MC");
    chain[f]->Project(hconveff_12[f],"(ph_convFlag>0):ph_RConv_MC","ph_selected&&ph_matchMC&&ph_isConv_MC");
  }
  TCanvas* c = gimeCanvas4("cconveff");
  c->Divide(2,2);
  c->cd(1);
  h2[0]->Draw();
  h12[0]->Draw("same");
  TLegend* l1 = gimeLegend(0.55,0.6,0.9,0.8) ;
  TString msg1 = "Conv2 - " ; msg1 += rel_ref;
  TString msg2 = "Conv1+Conv2 - " ; msg2 += rel_ref;
  l1->AddEntry(h2[0],msg1,"lp");
  l1->AddEntry(h12[0],msg2,"lp");
  l1->Draw();

  c->cd(2);
  h2[1]->Draw();
  h12[1]->Draw("same");
  TLegend* l1 = gimeLegend(0.55,0.6,0.9,0.8) ;
  TString msg1 = "Conv2 - " ; msg1 += rel;
  TString msg2 = "Conv1+Conv2 - " ; msg2 += rel;
  l1->AddEntry(h2[1],msg1,"lp");
  l1->AddEntry(h12[1],msg2,"lp");
  l1->Draw();

  c->cd(3);
  h2[0]->Draw(); h2[1]->Draw("same");
  TLegend* l1 = gimeLegend(0.55,0.6,0.9,0.8) ;
  TString msg1 = "Conv2 - " ; msg1 += rel_ref;
  TString msg2 = "Conv2 - " ; msg2 += rel;
  l1->AddEntry(h2[0],msg1,"lp");
  l1->AddEntry(h2[1],msg2,"lp");
  l1->Draw();

  c->cd(4);
  h12[0]->Draw(); h12[1]->Draw("same");
  TLegend* l1 = gimeLegend(0.55,0.6,0.9,0.8) ;
  TString msg1 = "Conv1+Conv2 - " ; msg1 += rel_ref;
  TString msg2 = "Conv1+Conv2 - " ; msg2 += rel;
  l1->AddEntry(h12[0],msg1,"lp");
  l1->AddEntry(h12[1],msg2,"lp");
  l1->Draw();

  c->SaveAs("conveff.eps");
}


/**************************************************************************************************************************************/


TCanvas* gimeCanvas4(const char* name) {
  TCanvas* c = new TCanvas(name,name,0,0,1600,800);
  c->SetBorderMode(0);
  c->SetFillColor(0);
  return c;
}


/**************************************************************************************************************************************/


TCanvas* gimeCanvas3(const char* name) {
  TCanvas* c = new TCanvas(name,name,0,0,1600,400);
  c->SetBorderMode(0);
  c->SetFillColor(0);
  return c;
}


/**************************************************************************************************************************************/


TCanvas* gimeCanvas2(const char* name) {
  TCanvas* c = new TCanvas(name,name,0,0,800,800);
  c->SetBorderMode(0);
  c->SetFillColor(0);
  return c;
}


/**************************************************************************************************************************************/


TLegend* gimeLegend(float x1, float y1, float x2, float y2) {
  TLegend* leg = new TLegend(x1,y1,x2,y2) ;
  leg->SetBorderSize(0);
  leg->SetFillColor(0);
  leg->SetFillStyle(0);
  leg->SetTextSize(0.04);
  return leg;
}


/**************************************************************************************************************************************/


void PhotonsLinResol(const TChain* chain[MAXINPUTFILES],TString rel_ref,TString rel) {
#define GEV     0.001
#define ETABINS 14
#define PHIBINS ETABINS
#define MINETA  0.0
#define MAXETA  2.5
#define ENEBINS 20
#define MINENE  20.0
#define MAXENE  100.0
 
  //* Initialize Step Parameters 
  float STEP_ETA   =(MAXETA-MINETA)/ETABINS;
  float START_ETA  =MINETA;
  float STEP_ENE   =(MAXENE-MINENE)/ENEBINS;
  float START_ENE  =MINENE;
  float EneBinSIZE[ENEBINS];   
  for (int i=0; i<ENEBINS; i++) {
    EneBinSIZE[i]=0.;
  }
  
  float EneEtaBinSIZE[ETABINS];   
  for (int i=0; i<ETABINS; i++) {
    EneEtaBinSIZE[i]=0.;
  }

  TH1F* Eresol[ENEBINS][MAXINPUTFILES];
  TH1F* EresEta[ETABINS][MAXINPUTFILES];
  char title1[ENEBINS][MAXINPUTFILES][100];
  char title2[ETABINS][MAXINPUTFILES][100];
  for (int f ; f<MAXINPUTFILES ; f++) {
    for (int i=0 ; i<ENEBINS ; i++) {
      sprintf(title1[i][f],"Eresol%d_%d",i,f);
      Eresol[i][f] = new TH1F(title1[i][f],title1[i][f],200,-1.,1.);
    }
    for (int i=0 ; i<ETABINS ; i++) {
      sprintf(title2[i][f],"EresEta%d_%d",i,f);
      EresEta[i][f] = new TH1F(title2[i][f],title2[i][f],200,-1.,1.);
    }
  }

  for (int f=0 ; f<MAXINPUTFILES ; f++) {
    Long64_t nentries = chain[f]->GetEntries();
    Long64_t nbytes = 0, nb = 0;
    vector<float> *ph_E;
    vector<float> *ph_eta;
    vector<float> *ph_E_MC;
    vector<float> *ph_isConv;
    vector<int> *ph_matchMC;
    TBranch *b_ph_E;
    TBranch *b_ph_eta;
    TBranch *b_ph_E_MC;
    TBranch *b_ph_isConv;
    TBranch *b_ph_matchMC;  
    ph_E = 0;
    ph_eta = 0;
    ph_E_MC = 0;
    ph_isConv = 0;
    ph_matchMC = 0;
    chain[f]->SetBranchAddress("ph_E", &ph_E, &b_ph_E);
    chain[f]->SetBranchAddress("ph_eta", &ph_eta, &b_ph_eta);
    chain[f]->SetBranchAddress("ph_E_MC", &ph_E_MC, &b_ph_E_MC);
    chain[f]->SetBranchAddress("ph_isConv", &ph_isConv, &b_ph_isConv);
    chain[f]->SetBranchAddress("ph_matchMC", &ph_matchMC, &b_ph_matchMC);
    for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = chain[f]->LoadTree(jentry);
      if (ientry < 0) break;
      //cout << jentry << endl;
      nb = chain[f]->GetEntry(jentry);   nbytes += nb;
      for (int j=0 ; j<ph_E->size() ; j++) {
	if ((*ph_isConv)[j]==0 && (*ph_matchMC)[j]==1) {
	  for (int i=0 ; i<ENEBINS ; i++) {
	    if ((*ph_E)[j]>(MINENE+i*((MAXENE-MINENE)/ENEBINS))*1000 && (*ph_E)[j]<(MINENE+(i+1)*((MAXENE-MINENE)/ENEBINS))*1000) Eresol[i][f]->Fill(((*ph_E)[j]-(*ph_E_MC)[j])/(*ph_E_MC)[j]);
	  }
	  for (int i=0 ; i<ETABINS ; i++) {
	    float abseta = fabs((*ph_eta)[j]);
	    if ( abseta>(MINETA+i*((MAXETA-MINETA)/ETABINS)) && abseta<(MINETA+(i+1)*((MAXETA-MINETA)/ETABINS))) EresEta[i][f]->Fill(((*ph_E)[j]-(*ph_E_MC)[j])/(*ph_E_MC)[j]);
	  }
	}
      }
    }
  }
  
  //* Plotting variables
  TGraphErrors *Linearity_error[MAXINPUTFILES];
  TGraphErrors *Resolution_error[MAXINPUTFILES];
  char BlaBla[100];
  float minGraph    = -0.04;
  float maxGraph    =  0.04;
  float minGraphEta  = -0.05;
  float maxGraphEta  =  0.05;
  float minGraphRes =  0.0;
  float maxGraphRes =  0.05;

  //* Energy Resolution/Linearity Plots:
  //TCanvas* cEresol = gimeCanvas4("cEresol");
  //TCanvas* cElinear = gimeCanvas4("cElinear");
  //TCanvas* cEresolvEne = gimeCanvas4("cEresolvEne");
  TCanvas cEresol;
  TCanvas cElinear;
  TCanvas cEresolvEne;
  cEresol.Update();
  cElinear.Update();
  float resol    = 0.05;
  float fitstart =-2.5*resol;
  float fitend   = 3.0*resol;
  float meanEne   [ENEBINS];
  float meanEneErr[ENEBINS];
  float meanEneBin[ENEBINS];
  float sigmEne   [ENEBINS];
  float sigmEneErr[ENEBINS];
  float Amplit    [ENEBINS][MAXINPUTFILES];
  
  for (int f=0; f<MAXINPUTFILES; f++) {
    cEresol.cd();
    for (int i=0; i<ENEBINS; i++) {
      float minene =START_ENE+i*STEP_ENE;
      float maxene =START_ENE+(i+1)*STEP_ENE;
      float meanene=(maxene+minene)/2.;
      Eresol[i][f]->Fit("gaus","","",-3.0*resol,3.0*resol);
      float mpv = gaus->GetParameter(1);
      float msig = gaus->GetParameter(2);
      //msig=0.11/sqrt(meanene);
      //mpv=0.0;
      fitstart=mpv-2.5*msig;
      fitend  =mpv+2.5*msig;
      Eresol[i][f]->Fit("gaus","","",fitstart,fitend);

      //* Cache the fit results:
      meanEne[i]   =gaus->GetParameter(1);
      sigmEne[i]   =gaus->GetParameter(2);
      meanEneErr[i]=gaus->GetParError(2);
      sigmEneErr[i]=gaus->GetParError(2);
      meanEneBin[i]=meanene;
      Amplit [i][f]=Eresol[i][f]->Integral();

      //* Sumperimpose the histos for checks:
      if(f>0) {
        Eresol[i][0]->SetMarkerStyle(22);
        if (Amplit[i][0]>0) {
          Eresol[i][0]->Scale(Amplit[i][f]/Amplit[i][0]);
        }
        Eresol[i][0]->Draw("sameerrors");
      }
      TLatex *legen = new TLatex(1.,1.,"legen"); 
      legen->SetNDC();
      legen->SetTextSize(0.07);
      sprintf(BlaBla,"%3.0f < E < %3.0fGeV",minene,maxene);
      legen->DrawLatex(0.12, 0.7,BlaBla);
      cEresol.Update();
      if(f==MAXINPUTFILES-1){
        char plotName[100];
        sprintf(plotName,"PhotonEresol_v_ene_%4.2f_%4.2f.eps",minene,maxene);
        cEresol.Print(plotName);
      }
    }

    //* Make Linearity vs ene Graph:
    cElinear.cd();
    Linearity_error[f] = new TGraphErrors(ENEBINS,meanEneBin,meanEne,EneBinSIZE,meanEneErr);
    Linearity_error[f]->SetTitle(0);
    //Linearity_error[f]->SetMarkerSize(2.0);
    Linearity_error[f]->SetMarkerStyle(22+f+1);
    Linearity_error[f]->SetMaximum(maxGraph);
    Linearity_error[f]->SetMinimum(minGraph);
    
    if(f==0)
      Linearity_error[0]->Draw("AP");
    Linearity_error[f]->GetXaxis()->SetTitle("Energy (GeV)");
    Linearity_error[f]->GetYaxis()->SetTitle("<(E-Etrue)/Etrue>");
    Linearity_error[f]->GetYaxis()->SetTitleOffset(1.30);
    
    Linearity_error[f]->Draw("P");
    
    cElinear.Update();
    if(f==MAXINPUTFILES-1){
      char plotName[100];
      sprintf(plotName,"PhotonELinearity.eps");
      TLegend* l1 = gimeLegend(0.55,0.6,0.9,0.8) ;
      TString msg1 = rel_ref;
      TString msg2 = rel;
      l1->AddEntry(Linearity_error[0],msg1,"lp");
      l1->AddEntry(Linearity_error[1],msg2,"lp");
      l1->Draw();
      cElinear.SaveAs(plotName);//Print(plotName);
    }

    //* Make Resolution vs ene Graph:
    cEresolvEne.cd();
    Resolution_error[f] = new TGraphErrors(ENEBINS,meanEneBin,sigmEne,EneBinSIZE,sigmEneErr);
    Resolution_error[f]->SetTitle(0);
    //Resolution_error[f]->SetMarkerSize(2.0);
    Resolution_error[f]->SetMarkerStyle(22+f+1);
    Resolution_error[f]->SetMaximum(maxGraphRes);
    Resolution_error[f]->SetMinimum(minGraphRes);
    
    if(f==0)
      Resolution_error[0]->Draw("AP");
    Resolution_error[f]->GetXaxis()->SetTitle("Energy (GeV)");
    Resolution_error[f]->GetYaxis()->SetTitle("#sigma_{(E-Etrue)/Etrue}");
    Resolution_error[f]->GetYaxis()->SetTitleOffset(1.30);
    
    Resolution_error[f]->Draw("P");
    
    cEresolvEne.Update();
    if(f==MAXINPUTFILES-1){
      char plotName[100];
      sprintf(plotName,"PhotonEResolution.eps");
      TLegend* l1 = gimeLegend(0.55,0.6,0.9,0.8) ;
      TString msg1 = rel_ref;
      TString msg2 = rel;
      l1->AddEntry(Resolution_error[0],msg1,"lp");
      l1->AddEntry(Resolution_error[1],msg2,"lp");
      l1->Draw();
      cEresolvEne.SaveAs(plotName);//Print(plotName);
    }

  }// MAXINPUTFILES loop

  //* Energy Uniformity in eta/Resolution in eta:
  float meanEneEta   [ETABINS];
  float meanEneEtaErr[ETABINS];
  float meanEneEtaBin[ETABINS];
  float sigmEneEta   [ETABINS];
  float sigmEneEtaErr[ETABINS];
  float AmplitEta    [ETABINS][MAXINPUTFILES];

  for (int f=0; f<MAXINPUTFILES; f++) {
    cEresol.cd();
    for (int i=0; i<ETABINS; i++) {
      float mineta =START_ETA+i*STEP_ETA;
      float maxeta =START_ETA+(i+1)*STEP_ETA;
      float meaneta=(maxeta+mineta)/2.;
      if (i!=8) EresEta[i][f]->Fit("gaus","","",-3.0*resol,3.0*resol);
      else EresEta[i][f]->Fit("gaus","","",-10.0*resol,10.0*resol);
      float mpv = gaus->GetParameter(1);
      float msig = gaus->GetParameter(2);
      //msig=0.12/sqrt(50.0);
      //mpv=0.0;
      fitstart=mpv-2.5*msig;
      fitend  =mpv+2.5*msig;
      EresEta[i][f]->Fit("gaus","","",fitstart,fitend);

      //* Cache the fit results:
      meanEneEta[i]   =gaus->GetParameter(1);
      sigmEneEta[i]   =gaus->GetParameter(2);
      meanEneEtaErr[i]=gaus->GetParError(2);
      sigmEneEtaErr[i]=gaus->GetParError(2);
      meanEneEtaBin[i]=meaneta;
      AmplitEta [i][f]=EresEta[i][f]->Integral();

      //* Sumperimpose the histos for checks:
      if(f>0) {
        EresEta[i][0]->SetMarkerStyle(22);
        if (AmplitEta[i][0]>0) {
          EresEta[i][0]->Scale(AmplitEta[i][f]/AmplitEta[i][0]);
        }
        EresEta[i][0]->Draw("sameerrors");
      }
      TLatex *legen = new TLatex(1.,1.,"legen"); 
      legen->SetNDC();
      legen->SetTextSize(0.07);
      sprintf(BlaBla,"%3.2f < #eta < %3.2f",mineta,maxeta);
      legen->DrawLatex(0.12, 0.7,BlaBla);
      cEresol.Update();
      if(f==MAXINPUTFILES-1){
        char plotName[100];
        sprintf(plotName,"PhotonEresol_v_eta_%4.2f_%4.2f.eps",mineta,maxeta);
        cEresol.Print(plotName);
      }

    }

    //* Make Linearity vs ene Graph:
    cElinear.cd();
    Linearity_error[f] = new TGraphErrors(ETABINS,meanEneEtaBin,meanEneEta,EneEtaBinSIZE,meanEneEtaErr);
    Linearity_error[f]->SetTitle(0);
    //Linearity_error[f]->SetMarkerSize(2.0);
    Linearity_error[f]->SetMarkerStyle(22+f+1);
    Linearity_error[f]->SetMaximum(maxGraphEta);
    Linearity_error[f]->SetMinimum(minGraphEta);
    
    if(f==0)
      Linearity_error[0]->Draw("AP");
    Linearity_error[f]->GetXaxis()->SetTitle("|#eta|");
    Linearity_error[f]->GetYaxis()->SetTitle("<(E-Etrue)/Etrue>");
    Linearity_error[f]->GetYaxis()->SetTitleOffset(1.30);
    
    Linearity_error[f]->Draw("P");
    
    cElinear.Update();
    if(f==MAXINPUTFILES-1){
      char plotName[100];
      sprintf(plotName,"PhotonEUniformity_vs_Eta.eps");
      TLegend* l1 = gimeLegend(0.55,0.6,0.9,0.8) ;
      TString msg1 = rel_ref;
      TString msg2 = rel;
      l1->AddEntry(Linearity_error[0],msg1,"lp");
      l1->AddEntry(Linearity_error[1],msg2,"lp");
      l1->Draw();
      cElinear.SaveAs(plotName);//Print(plotName);
    }

    //* Make Resolution vs eta Graph:
    cEresolvEne.cd();
    Resolution_error[f] = new TGraphErrors(ETABINS,meanEneEtaBin,sigmEneEta,EneEtaBinSIZE,sigmEneEtaErr);
    Resolution_error[f]->SetTitle(0);
    //Resolution_error[f]->SetMarkerSize(2.0);
    Resolution_error[f]->SetMarkerStyle(22+f+1);
    Resolution_error[f]->SetMaximum(maxGraphRes);
    Resolution_error[f]->SetMinimum(minGraphRes);
    
    if(f==0)
      Resolution_error[0]->Draw("AP");
    Resolution_error[f]->GetXaxis()->SetTitle("|#eta|");
    Resolution_error[f]->GetYaxis()->SetTitle("#sigma_{(E-Etrue)/Etrue}");
    Resolution_error[f]->GetYaxis()->SetTitleOffset(1.30);
    Resolution_error[f]->Draw("P");
    
    cEresolvEne.Update();
    if(f==MAXINPUTFILES-1){
      char plotName[100];
      sprintf(plotName,"PhotonEResolution_vs_Eta.eps");
      TLegend* l1 = gimeLegend(0.55,0.6,0.9,0.8) ;
      TString msg1 = rel_ref;
      TString msg2 = rel;
      l1->AddEntry(Resolution_error[0],msg1,"lp");
      l1->AddEntry(Resolution_error[1],msg2,"lp");
      l1->Draw();
      cEresolvEne.SaveAs(plotName);//Print(plotName);
    }

  }// MAXINPUTFILES loop


}
