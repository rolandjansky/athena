/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
.L print.C
print("ttbar20_newbank.root","ttbar20")
print("ttbar40_newbank.root","ttbar40")
 */

TString ifile, out;
TCanvas *c1;
TFile *f;
TF1 *func_nocut, *func_cut;

Bool_t reject;
Double_t fline(Double_t *x, Double_t *par) {
  if (reject && abs(x[0]) > 0.8 && abs(x[0]) < 1.2) {
    TF1::RejectPoint();
    return 0;
  } 
  return par[0];
}

void doitG(TString name, bool fit = false) {
   TGraphAsymmErrors *g = (TGraphAsymmErrors*)f->Get(name);
   cerr << "Title = " << g->GetXaxis()->GetTitle() << std::endl;
   c1->cd();
   g->Draw("AP");
   g->SetTitle(out);  
   if (fit) {
     reject = 1;
     g->Fit("func_nocut","R");
     reject = 0;
     g->Fit("func_cut","R");
     g->Draw("AP");
     reject = 1;
     func_nocut->Draw("same");
     reject = 0;
     func_cut->Draw("same");
     TString title = out+" eff overall = ";
     title += Form("%.3f",func_nocut->GetParameter(0));
     title += " and w/o transition region = ";
     title += Form("%.3f",func_cut->GetParameter(0));
     g->SetTitle(title);
   }
   TString newname = name; 
   newname += "_";
   newname += out;
   newname += ".pdf";
   c1->Print(newname);
}

void doitH(TString name) {
   TH1F *h = (TH1F*)f->Get(name);
   c1->cd();
   h->Draw();
   h->SetTitle(out);
   TString newname = name; 
   newname += "_";
   newname += out;
   newname += ".pdf";
   c1->Print(newname);
}


void doit2D(TString name) {
   TH2F *h = (TH2F*)f->Get(name);
   c1->cd();
   h->Draw("COLZ");
   h->SetTitle(out);
   TString newname = name; 
   newname += "_";
   newname += out;
   newname += ".pdf";
   c1->Print(newname);
}

void print(TString infile="ttbar20_newbank.root",TString outstring="ttbar20") {
   gStyle->SetOptStat(00000000);
   ifile = infile;
   out = outstring;
   c1 = new TCanvas("c1","c1",800,600);
   reject = kTRUE;
   func_nocut = new TF1("func_nocut",fline,-2.5,2.5,1);
   func_cut = new TF1("func_cut",fline,-2.5,2.5,1);
   func_nocut->SetLineColor(kRed);
   func_cut->SetLineColor(kGreen+3);
   f = new TFile(infile,"READ");
   doitG("greffd0");
   doitG("greffz0");
   doitG("greffcurv");
   doitG("greffeta",true);
   doitG("greffphi"); 
   doitG("greffpt"); 
   doit2D("histophid0_ftk"); 
   doit2D("histoetaphi_ftk"); 
   gStyle->SetOptStat(11111111);
   doitH("histod0res");
   doitH("histoz0res");
   doitH("histocurvres");
   doitH("histoetares");
   doitH("histophires");
   doitH("historelptres");
   return;
}
