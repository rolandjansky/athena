#include <fstream>


void JetTagAna_postProcessingHLT(TString sample){

  ofstream fout("out.txt");

  gROOT->Reset();
  gStyle->SetOptStat(0);
  gROOT->ForceStyle();

  Int_t first=1;
  Int_t last=1;

  TString path_bb = "./";
  TString path_uu = "./";
  TString path = "./";

  TString fileName_bb;
  TString fileName_uu;
  TString fileName;

  TH1F* hbbL2   = 0;
  TH1F* hbbEF   = 0;
  TH1F* huuL2   = 0;
  TH1F* huuEF   = 0;
  TH1F* hbbOf1  = 0;
  TH1F* huuOf1  = 0;
  TH1F* hbbOf2  = 0;
  TH1F* huuOf2  = 0;

  if (sample == "5850" || sample == "5851") {

    fileName_bb = "tagana.bb.0";
    fileName_uu = "tagana.uu.0";

  } else if (sample == "5200") {

    fileName = "tagana.0";
    
  }

  if (sample == "5850" || sample == "5851") {

    bool doFirst_u = true;
    bool doFirst_b = true;
    
    for(Int_t i = first; i <= last; i++) {
      
      TString realFileName_bb;
      TString realFileName_uu;
      
      TString insideFileName_bb = fileName_bb;
      TString insideFileName_uu = fileName_uu;
      
      TString number = ""; number+=i;
      
      insideFileName_bb.Resize(insideFileName_bb.Length()-(number.Length()-1));
      insideFileName_uu.Resize(insideFileName_uu.Length()-(number.Length()-1));
      
      realFileName_bb = insideFileName_bb+number+".root";
      realFileName_uu = insideFileName_uu+number+".root";
      
      fout << path_bb << realFileName_bb << endl;
      fout << path_uu << realFileName_uu << endl;    
      
      ifstream fb(path_bb + realFileName_bb);
      ifstream fu(path_uu + realFileName_uu);
      
      TFile* file;
      
      if(fb) {
	
	file = new TFile(path_bb + realFileName_bb);
	
	TH1F* h_bbL2  = (TH1F*)file->Get("perf_l2ip3d_b");
	TH1F* h_bbEF  = (TH1F*)file->Get("perf_ef_afterl2_ip3d_b");
	TH1F* h_bbOf1 = (TH1F*)file->Get("perf_ofip3d_b");
	TH1F* h_bbOf2 = (TH1F*)file->Get("perf_ip3d_b");
      }
      
      if(fu) {
	
	file = new TFile(path_uu + realFileName_uu);
	
	TH1F* h_uuL2  = (TH1F*)file->Get("perf_l2ip3d_u");
	TH1F* h_uuEF  = (TH1F*)file->Get("perf_ef_afterl2_ip3d_u");
	TH1F* h_uuOf1 = (TH1F*)file->Get("perf_ofip3d_u");
	TH1F* h_uuOf2 = (TH1F*)file->Get("perf_ip3d_u");
      }
      
      if (fu && doFirst_u){

	huuL2  = new TH1F(*h_uuL2);
	huuEF  = new TH1F(*h_uuEF);
	huuOf1 = new TH1F(*h_uuOf1);
	huuOf2 = new TH1F(*h_uuOf2);

	doFirst_u = false;
      }
      
      if (fb && doFirst_b){

	hbbL2  = new TH1F(*h_bbL2);
	hbbEF  = new TH1F(*h_bbEF);
	hbbOf1 = new TH1F(*h_bbOf1);
	hbbOf2 = new TH1F(*h_bbOf2);

	doFirst_b = false;
      }

      if(fu) {
	huuL2->Add(h_uuL2);   huuEF->Add(h_uuEF); 
	huuOf1->Add(h_uuOf1); huuOf2->Add(h_uuOf2); 
      }
      
      if(fb) {
	hbbL2->Add(h_bbL2);   hbbEF->Add(h_bbEF); 
	hbbOf1->Add(h_bbOf1); hbbOf2->Add(h_bbOf2); 
      }
    }
  }

  if (sample == "5200") {

    bool doFirst = true;
    
    for(Int_t i = first; i <= last; i++) {
      
      TString realFileName;
      
      TString insideFileName = fileName;
      
      TString number = ""; number+=i;
      
      insideFileName.Resize(insideFileName.Length()-(number.Length()-1));
      
      realFileName = insideFileName+number+".root";
      
      fout << path << realFileName << endl;
      
      ifstream f(path + realFileName);
      
      TFile* file;
      
      if(f) {
	
	file = new TFile(path + realFileName);
	
	TH1F* h_bbL2  = (TH1F*)file->Get("perf_l2_ip3d_b");
	TH1F* h_bbEF  = (TH1F*)file->Get("perf_ef_afterl2_ip3d_b");
	//TH1F* h_bbEF  = (TH1F*)file->Get("perf_ef_ip3d_b");
	TH1F* h_bbOf1 = (TH1F*)file->Get("perf_offline_afterhlt_ip3d_b");
	TH1F* h_bbOf2 = (TH1F*)file->Get("perf_ip3d_b");

	TH1F* h_uuL2  = (TH1F*)file->Get("perf_l2_ip3d_u");
	TH1F* h_uuEF  = (TH1F*)file->Get("perf_ef_afterl2_ip3d_u");
	//TH1F* h_uuEF  = (TH1F*)file->Get("perf_ef_ip3d_u");
	TH1F* h_uuOf1 = (TH1F*)file->Get("perf_offline_afterhlt_ip3d_u");
	TH1F* h_uuOf2 = (TH1F*)file->Get("perf_ip3d_u");

	TH1F* h_bbL2JetProb = (TH1F*)file->Get("perf_l2_jetprob_b");
	TH1F* h_bbEFJetProb = (TH1F*)file->Get("perf_ef_jetprob_b");

	TH1F* h_uuL2JetProb = (TH1F*)file->Get("perf_l2_jetprob_u");
	TH1F* h_uuEFJetProb = (TH1F*)file->Get("perf_ef_jetprob_u");
      }

      if (f && doFirst){

	hbbL2  = new TH1F(*h_bbL2);
	hbbEF  = new TH1F(*h_bbEF);
	hbbOf1 = new TH1F(*h_bbOf1);
	hbbOf2 = new TH1F(*h_bbOf2);

	huuL2  = new TH1F(*h_uuL2);
	huuEF  = new TH1F(*h_uuEF);
	huuOf1 = new TH1F(*h_uuOf1);
	huuOf2 = new TH1F(*h_uuOf2);

	hbbL2JetProb = new TH1F(*h_bbL2JetProb);
	hbbEFJetProb = new TH1F(*h_bbEFJetProb);

	huuL2JetProb = new TH1F(*h_uuL2JetProb);
	huuEFJetProb = new TH1F(*h_uuEFJetProb);

	doFirst = false;

      }

      if(f) {

	hbbL2->Add(h_bbL2);   hbbEF->Add(h_bbEF); 
	hbbOf1->Add(h_bbOf1); hbbOf2->Add(h_bbOf2); 

	huuL2->Add(h_uuL2);   huuEF->Add(h_uuEF); 
	huuOf1->Add(h_uuOf1); huuOf2->Add(h_uuOf2); 

	hbbL2JetProb->Add(h_bbL2JetProb);   hbbEFJetProb->Add(h_bbEFJetProb);
	huuL2JetProb->Add(h_uuL2JetProb);   huuEFJetProb->Add(h_uuEFJetProb);
      }
    }
  }

  TCanvas *c = new TCanvas("c","",10,10,500,500);
  TGraphErrors *grx1 = new TGraphErrors();
  TGraphErrors *grx2 = new TGraphErrors();
  TGraphErrors *grx3 = new TGraphErrors();
  TGraphErrors *grx4 = new TGraphErrors();

  fout << "L2 " << endl;
  plot(fout,hbbL2,huuL2,grx1);
  fout << "EF " << endl;
  plot(fout,hbbEF,huuEF,grx2,1.,1.,"SAME");
  fout << "Offline corr." << endl;
  plot(fout,hbbOf1,huuOf1,grx3,1.,1.,"SAME");
  fout << "Offline " << endl;
  plot(fout,hbbOf2,huuOf2,grx4,1.,1.,"SAME");

  grx1->Draw("AP");
  grx1->GetXaxis()->SetLimits(0.38,1.02);
  grx1->SetMinimum(1);
  grx1->SetMaximum(800);
  grx1->SetMarkerStyle(22);
  grx2->Draw("P");
  grx2->SetMarkerStyle(26);
  grx3->Draw("P");
  grx3->SetMarkerStyle(20);
  grx4->Draw("P");
  grx4->SetMarkerStyle(18);

  gPad->SetLogy();
  gPad->SetGrid();
  gPad->SetLeftMargin(0.15);
  gPad->SetBottomMargin(0.15);
  gPad->SetFillColor(0);
  gPad->SetHighLightColor(0);
  gPad->SetBorderMode(0);

  grx1->GetYaxis()->SetTitle("Light-jet rejection");
  grx1->GetXaxis()->SetTitle("b-jet efficiency");
  grx1->GetYaxis()->SetTitleOffset(1.3);

  TLegend *leg = new TLegend(0.5,0.65,0.88,0.85);
  leg->SetTextSize(0.04);
  leg->AddEntry(grx1,"L2","PL");
  leg->AddEntry(grx2,"EF after L2 ","PL");
  leg->AddEntry(grx3,"Offline after HLT","PL");
  leg->AddEntry(grx4,"Offline","PL");
  leg->Draw();

  TCanvas *c2 = new TCanvas("c2","",10,10,500,500);
  TGraphErrors *grx5 = new TGraphErrors();
  TGraphErrors *grx6 = new TGraphErrors();

  fout << "L2 JetProb" << endl;
  plot(fout,hbbL2JetProb,huuL2JetProb,grx5);
  fout << "EF JetProb" << endl;
  plot(fout,hbbEFJetProb,huuEFJetProb,grx6,1.,1.,"SAME");

  grx5->Draw("AP");
  grx5->GetXaxis()->SetLimits(0.38,1.02);
  grx5->SetMinimum(1);
  grx5->SetMaximum(110);
  grx5->SetMarkerStyle(22);
  grx6->Draw("P");
  grx6->SetMarkerStyle(26);

  gPad->SetLogy();
  gPad->SetGrid();
  gPad->SetLeftMargin(0.15);
  gPad->SetBottomMargin(0.15);
  gPad->SetFillColor(0);
  gPad->SetHighLightColor(0);
  gPad->SetBorderMode(0);

  grx5->GetYaxis()->SetTitle("Light-jet rejection");
  grx5->GetXaxis()->SetTitle("b-jet efficiency");
  grx5->GetYaxis()->SetTitleOffset(1.3);

  TLegend *leg = new TLegend(0.5,0.65,0.88,0.85);
  leg->SetTextSize(0.04);
  leg->AddEntry(grx5,"L2 JetProb","PL");
  leg->AddEntry(grx6,"EF JetProb","PL");
  leg->Draw();
}

void plot(ofstream &fout, TH1F* hb, TH1F* hu,TGraphErrors *gr,double off_eps=1, double off_R=1, TString opt="AP"){
  
  double totb = 0;
  double totu = 0;
  for (int i=0;i<hb->GetNbinsX();i++){
    totb += hb->GetBinContent(i+1);
    totu += hu->GetBinContent(i+1);
  }
  
  fout << "totb = " << totb << "; totu = " << totu << endl;

  double sumu=totu,sumb=totb;
  double effu,effb,seffu,seffb;
  int ind=0;

  for (int i=0;i<hb->GetNbinsX()-1;i++){
    if (i>=0){
      sumb -= hb->GetBinContent(i+1);
      sumu -= hu->GetBinContent(i+1);
    }

    //fout << "GetBinContent " << i+1 << " = " << hb->GetBinContent(i+1) << " " << totb << endl;
    //fout << "GetBinCenter  " << i   << " = " << hu->GetBinCenter(i) << endl;
    //fout << "sumb = " << sumb << "; sumu = " << sumu << endl;

    effb  = sumb/totb*off_eps;
    effu  = sumu/totu*(1/off_R);
    seffb = sqrt((effb)*(1-effb)/totb);
    seffu = sqrt((effu)*(1-effu)/totu);

    fout << "cut " << hb->GetBinCenter(i+1)+hb->GetBinWidth(i+1) << "  effb = " << effb << "; effu = " << effu << endl;
    
    if (effu!=0 && effb!=0){ 
      seffu = seffu/pow(effu,2);
      gr->SetPoint(ind,effb,1./effu);
      gr->SetPointError(ind,seffb,seffu);
      ind++;
    }
  }
}

