// ============================================
// ROOT script to read JpsiExample.root
// which is the n-tuple made by JpsiExample alg
// D. Scheirich
// ============================================

void mass(char* file="../JpsiExample.root") {

  TFile* f = new TFile(file);
  TTree* t = (TTree*)f->Get("AUX");

  TCanvas* c = new TCanvas("c1","c1");
  c->Divide(2,1);
  c->cd(1);
  t->Draw("jpsiMass >> h1");
  TH1F *h1 = (TH1F*)gPad->GetPrimitive("h1"); // 1D
  h1->Fit("gaus","","",3000,3200);
  c->cd(2);
  t->Draw("jpsiMassError>>h2");


  t->Print();

}

void pt(char* file="../JpsiExample.root") {

  TFile* f = new TFile(file);
  TTree* t = (TTree*)f->Get("AUX");

  TCanvas* c = new TCanvas("c1","c1");
  c->Divide(2,2);
  c->cd(1);
  t->Draw("sqrt(trkRefitPx1*trkRefitPx1+trkRefitPy1*trkRefitPy1) >> h1");
  c->cd(2);
  t->Draw("sqrt(trkRefitPx2*trkRefitPx1+trkRefitPy2*trkRefitPy2) >> h2");
  c->cd(3);
  t->Draw("sqrt(trkOrigPx1*trkOrigPx1+trkOrigPy1*trkOrigPy1) >> h3");
  c->cd(4);
  t->Draw("sqrt(trkOrigPx2*trkOrigPx1+trkOrigPy2*trkOrigPy2) >> h4");

}

void p(char* file="../JpsiExample.root") {

  TFile* f = new TFile(file);
  TTree* t = (TTree*)f->Get("AUX");

  TCanvas* c = new TCanvas("c1","c1");
  c->Divide(2,2);
  c->cd(1);
  t->Draw("sqrt(trkRefitPx1*trkRefitPx1+trkRefitPy1*trkRefitPy1+trkRefitPz1*trkRefitPz1) >> h1");
  c->cd(2);
  t->Draw("sqrt(trkRefitPx2*trkRefitPx1+trkRefitPy2*trkRefitPy2+trkRefitPz2*trkRefitPz2) >> h2");
  c->cd(3);
  t->Draw("sqrt(trkOrigPx1*trkOrigPx1+trkOrigPy1*trkOrigPy1+trkOrigPz1*trkOrigPz1) >> h3");
  c->cd(4);
  t->Draw("sqrt(trkOrigPx2*trkOrigPx1+trkOrigPy2*trkOrigPy2+trkOrigPz2*trkOrigPz2) >> h4");

  TCanvas* c2 = new TCanvas("c2","c2");
  c2->Divide(2,1);
  c2->cd(1);
  t->Draw("trkRefitCharge1*trkRefitCharge2 >> h5");
  c2->cd(2);
  t->Draw("trkOrigCharge1*trkOrigCharge2 >> h6");

}

void vx(char* file="../JpsiExample.root") {

  TFile* f = new TFile(file);
  TTree* t = (TTree*)f->Get("AUX");

  TCanvas* c = new TCanvas("c1","c1");
  c->Divide(2,1);
  c->cd(1);
  t->Draw("sqrt(vx*vx+vy*vy) >> h1");
  c->cd(2);
  t->Draw("rxyError >> h2");
  TCanvas* c2 = new TCanvas("c2","c2");
  t->Draw("vz >> h3");

}
