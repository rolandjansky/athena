
TFile *f;
TFile *r;

void plot1D(TString s,
	    TString hn1,
	    TString hn2,
	    TString hn3,
	    TString hn4,
	    bool end=false)
{
  TCanvas *dummydet=new TCanvas("dummydet","dummydet",600,400);
  TCanvas *canvasdet=new TCanvas("JetRec_RTT_det","JetRec RTT detailed",800,600);
  TPad *padd1 = new TPad("pdad1","number of jets",0.03,0.64,0.47,0.92,0);
  TPad *padd1b= new TPad("pdd1b","number of jets",0.03,0.49,0.47,0.64,0);
  TPad *padd2 = new TPad("pdad2","jet energy    ",0.53,0.64,0.93,0.92,0);
  TPad *padd2b= new TPad("pdd2b","jet energy    ",0.53,0.49,0.93,0.64,0);
  TPad *padd3 = new TPad("pdad3","jet eta       ",0.03,0.18,0.47,0.46,0);
  TPad *padd3b= new TPad("pdd3b","jet eta       ",0.03,0.03,0.47,0.18,0);
  TPad *padd4 = new TPad("pdad4","jet mass      ",0.53,0.18,0.93,0.46,0);
  TPad *padd4b= new TPad("pdd4b","jet mass      ",0.53,0.03,0.93,0.18,0);
  padd1->SetBottomMargin(0.001);
  padd1b->SetTopMargin(0.0);
  padd2->SetBottomMargin(0.001);
  padd2b->SetTopMargin(0.0);
  padd3->SetBottomMargin(0.001);
  padd3b->SetTopMargin(0.0);
  padd4->SetBottomMargin(0.001);
  padd4b->SetTopMargin(0.0);
  padd1->Draw();
  padd2->Draw();
  padd3->Draw();
  padd4->Draw();
  padd1b->Draw();
  padd2b->Draw();
  padd3b->Draw();
  padd4b->Draw();
  
  TPaveLabel *titled = new TPaveLabel(0.1,0.94,0.9,0.98, s);
  titled->Draw();
  
  if ( ! f->cd(s) ) return;
  TH1 *hd1=(TH1*)gDirectory->Get(hn1);
  TH1 *hd2=(TH1*)gDirectory->Get(hn2);
  TH1 *hd3=(TH1*)gDirectory->Get(hn3);
  TH1 *hd4=(TH1*)gDirectory->Get(hn4);
  if ( ! r->cd(s) ) return;
  TH1 *gd1=(TH1*)gDirectory->Get(hn1);
  TH1 *gd2=(TH1*)gDirectory->Get(hn2);
  TH1 *gd3=(TH1*)gDirectory->Get(hn3);
  TH1 *gd4=(TH1*)gDirectory->Get(hn4);
  gd1->SetLineColor(kRed);
  gd2->SetLineColor(kRed);
  gd3->SetLineColor(kRed);
  gd4->SetLineColor(kRed);
  
  TH1 *sd1 = hd1->Clone();
  TH1 *sd2 = hd2->Clone();
  TH1 *sd3 = hd3->Clone();
  TH1 *sd4 = hd4->Clone();
  TH1 *dd1 = hd1->Clone();
  TH1 *dd2 = hd2->Clone();
  TH1 *dd3 = hd3->Clone();
  TH1 *dd4 = hd4->Clone();
  
  sd1->Add(gd1);
  sd2->Add(gd2);
  sd3->Add(gd3);
  sd4->Add(gd4);
  dd1->Add(gd1,-1);
  dd2->Add(gd2,-1);
  dd3->Add(gd3,-1);
  dd4->Add(gd4,-1);
  dd1->Divide(sd1);
  dd2->Divide(sd2);
  dd3->Divide(sd3);
  dd4->Divide(sd4);
  dd1->SetStats(0);
  dd2->SetStats(0);
  dd3->SetStats(0);
  dd4->SetStats(0);
  dd1->SetTitle("");
  dd2->SetTitle("");
  dd3->SetTitle("");
  dd4->SetTitle("");
  
  hd1->GetXaxis()->SetLabelOffset(3);
  hd2->GetXaxis()->SetLabelOffset(3);
  hd3->GetXaxis()->SetLabelOffset(3);
  hd4->GetXaxis()->SetLabelOffset(3);
  
  dd1->GetXaxis()->SetLabelSize(0.1);
  dd2->GetXaxis()->SetLabelSize(0.1);
  dd3->GetXaxis()->SetLabelSize(0.1);
  dd4->GetXaxis()->SetLabelSize(0.1);
  dd1->GetYaxis()->SetLabelSize(0.1);
  dd2->GetYaxis()->SetLabelSize(0.1);
  dd3->GetYaxis()->SetLabelSize(0.1);
  dd4->GetYaxis()->SetLabelSize(0.1);
  
  dd1->SetMaximum( 1.2);
  dd2->SetMaximum( 1.2);
  dd3->SetMaximum( 1.2);
  dd4->SetMaximum( 1.2);
  dd1->SetMinimum(-1.2);
  dd2->SetMinimum(-1.2);
  dd3->SetMinimum(-1.2);
  dd4->SetMinimum(-1.2);
  
  padd1->cd();
  if(gd1->GetMaximum()>hd1->GetMaximum())
    { gd1->Draw(); hd1->Draw("same"); } else { hd1->Draw(); gd1->Draw("same"); }
  padd2->cd();
  if(gd2->GetMaximum()>hd2->GetMaximum())
    { gd2->Draw(); hd2->Draw("same"); } else { hd2->Draw(); gd2->Draw("same"); }
  padd3->cd();
  if(gd3->GetMaximum()>hd3->GetMaximum())
    { gd3->Draw(); hd3->Draw("same"); } else { hd3->Draw(); gd3->Draw("same"); }
  padd4->cd();
  if(gd4->GetMaximum()>hd4->GetMaximum())
    { gd4->Draw(); hd4->Draw("same"); } else { hd4->Draw(); gd4->Draw("same"); }
  padd1b->cd();
  dd1->Draw();
  padd2b->cd();
  dd2->Draw();
  padd3b->cd();
  dd3->Draw();
  padd4b->cd();
  dd4->Draw();
  
  TString ps=s+".ps";
  canvasdet->Update();
  if(end)
    ps=s+".ps)";
  canvasdet->Print("JetRec_Detailed_"+ps);	
}

void plot2D(TString s,
	    TString hn,
	    bool end=false)
{
  TCanvas *dummydet=new TCanvas("dummydet","dummydet",600,400);
  TCanvas *canvasdet=new TCanvas("JetRec_RTT_det","JetRec RTT detailed",800,600);
  TPad *padd1 = new TPad("pdad1","number of jets",0.03,0.62,0.93,0.92,0);
  TPad *padd2 = new TPad("pdad2","jet energy    ",0.03,0.32,0.93,0.62,0);
  TPad *padd3 = new TPad("pdad3","jet eta       ",0.03,0.02,0.93,0.32,0);
  padd1->Draw();
  padd2->Draw();
  padd3->Draw();
  
  TPaveLabel *titled = new TPaveLabel(0.1,0.94,0.9,0.98, s);
  titled->Draw();
  
  if ( ! f->cd(s) ) return;
  TH2 *hd=(TH2*)gDirectory->Get(hn);
  if ( ! r->cd(s) ) return;
  TH2 *gd=(TH2*)gDirectory->Get(hn);
  
  TH1 *sd = hd->Clone();
  TH1 *dd = hd->Clone();
  
  sd->Add(gd);
  dd->Add(gd,-1);
  dd->Divide(sd);
  hd->SetStats(0);
  gd->SetStats(0);
  dd->SetStats(0);
  dd->SetTitle("Difference / Sum");
  
  hd->GetXaxis()->SetLabelOffset(3);
  dd->GetXaxis()->SetLabelSize(0.1);
  dd->SetMaximum( 1.2);
  dd->SetMinimum(-1.2);
  
  padd1->cd();
  hd->Draw("surf");
  padd2->cd();
  gd->Draw("surf");
  padd3->cd();
  dd->Draw("surf");
  
  TString ps=s+".ps";
  canvasdet->Update();
  if(end)
    ps=s+".ps)";
  canvasdet->Print("JetRec_Detailed_"+ps);	
}

void ValgrindRTTJobs_postJob(TString fstr = "none")
{
  // r=new TFile("/atlas/home/seuster/JetRTT/run/12.5.0_J5/monitor.root");
  r=new TFile(fstr);
  f=new TFile("monitor.root");
  
  gROOT->SetStyle("Plain");
  
  TObjArray *dirs=new TObjArray(50);
  TIter next(f->GetListOfKeys());
  TKey *key=(TKey*)next();
  TString s=key->GetClassName();
  
  int n0=0;
  if(s=="TDirectory") {
    TIter next(gDirectory->GetListOfKeys());
    while ((key=(TKey*)next())) {
      s=key->GetClassName();
      if(s=="TDirectory") {
	s=key->GetName();
	dirs->Add(new TObjString(s));
	n0++;
      }
    }
  }else{
    cout << "NO\n";
  }
  dirs->Print();
  TCanvas *dummy=new TCanvas("dummy","dummy",600,400);
  
  TString ps="(";
  
  int n1=0;
  TIter next2(dirs);
  TObjString *str;
  while (str=(TObjString *)next2()) {
    
    TCanvas *canvas=new TCanvas("JetRec_RTT","JetRec RTT",800,600);
    TPad *pad1 = new TPad("pad1","number of jets",0.03,0.64,0.47,0.92,0);
    TPad *pad1b= new TPad("pd1b","number of jets",0.03,0.49,0.47,0.64,0);
    TPad *pad2 = new TPad("pad2","jet energy    ",0.53,0.64,0.93,0.92,0);
    TPad *pad2b= new TPad("pd2b","jet energy    ",0.53,0.49,0.93,0.64,0);
    TPad *pad3 = new TPad("pad3","jet eta       ",0.03,0.18,0.47,0.46,0);
    TPad *pad3b= new TPad("pd3b","jet eta       ",0.03,0.03,0.47,0.18,0);
    TPad *pad4 = new TPad("pad4","jet mass      ",0.53,0.18,0.93,0.46,0);
    TPad *pad4b= new TPad("pd4b","jet mass      ",0.53,0.03,0.93,0.18,0);
    pad1->SetBottomMargin(0.001);
    pad1b->SetTopMargin(0.0);
    pad2->SetBottomMargin(0.001);
    pad2b->SetTopMargin(0.0);
    pad3->SetBottomMargin(0.001);
    pad3b->SetTopMargin(0.0);
    pad4->SetBottomMargin(0.001);
    pad4b->SetTopMargin(0.0);
    pad1->Draw();
    pad2->Draw();
    pad3->Draw();
    pad4->Draw();
    pad1b->Draw();
    pad2b->Draw();
    pad3b->Draw();
    pad4b->Draw();
    
    TString s = str->GetName();
    std::cout << s << "\n";
    
    TPaveLabel *title = new TPaveLabel(0.1,0.94,0.9,0.98, s);
    title->Draw();
    
    if ( ! f->cd(str->GetName()) ) continue;
    TH1D *h1=(TH1D*)gDirectory->Get("NoJets");
    TH1D *h2=(TH1D*)gDirectory->Get("JetE");
    TH1D *h3=(TH1D*)gDirectory->Get("JetEta");
    TH1D *h4=(TH1D*)gDirectory->Get("JetMass");
    if ( ! r->cd(str->GetName()) ) continue;
    TH1D *g1=(TH1D*)gDirectory->Get("NoJets");
    TH1D *g2=(TH1D*)gDirectory->Get("JetE");
    TH1D *g3=(TH1D*)gDirectory->Get("JetEta");
    TH1D *g4=(TH1D*)gDirectory->Get("JetMass");
    g1->SetLineColor(kRed);
    g2->SetLineColor(kRed);
    g3->SetLineColor(kRed);
    g4->SetLineColor(kRed);
    
    TH1D *s1=h1->Clone();
    TH1D *s2=h2->Clone();
    TH1D *s3=h3->Clone();
    TH1D *s4=h4->Clone();
    TH1D *d1=h1->Clone();
    TH1D *d2=h2->Clone();
    TH1D *d3=h3->Clone();
    TH1D *d4=h4->Clone();
    s1->Add(g1);
    s2->Add(g2);
    s3->Add(g3);
    s4->Add(g4);
    d1->Add(g1,-1);
    d2->Add(g2,-1);
    d3->Add(g3,-1);
    d4->Add(g4,-1);
    d1->Divide(s1);
    d2->Divide(s2);
    d3->Divide(s3);
    d4->Divide(s4);
    d1->SetStats(0);
    d2->SetStats(0);
    d3->SetStats(0);
    d4->SetStats(0);
    d1->SetTitle("");
    d2->SetTitle("");
    d3->SetTitle("");
    d4->SetTitle("");
    
    h1->GetXaxis()->SetLabelOffset(3);
    h2->GetXaxis()->SetLabelOffset(3);
    h3->GetXaxis()->SetLabelOffset(3);
    h4->GetXaxis()->SetLabelOffset(3);
    
    d1->GetXaxis()->SetLabelSize(0.1);
    d2->GetXaxis()->SetLabelSize(0.1);
    d3->GetXaxis()->SetLabelSize(0.1);
    d4->GetXaxis()->SetLabelSize(0.1);
    d1->GetYaxis()->SetLabelSize(0.1);
    d2->GetYaxis()->SetLabelSize(0.1);
    d3->GetYaxis()->SetLabelSize(0.1);
    d4->GetYaxis()->SetLabelSize(0.1);
    
    d1->SetMaximum( 1.2);
    d2->SetMaximum( 1.2);
    d3->SetMaximum( 1.2);
    d4->SetMaximum( 1.2);
    d1->SetMinimum(-1.2);
    d2->SetMinimum(-1.2);
    d3->SetMinimum(-1.2);
    d4->SetMinimum(-1.2);
    
    pad1->cd();
    if(g1->GetMaximum()>h1->GetMaximum())
      { g1->Draw(); h1->Draw("same"); } else { h1->Draw(); g1->Draw("same"); }
    pad2->cd();
    if(g2->GetMaximum()>h2->GetMaximum())
      { g2->Draw(); h2->Draw("same"); } else { h2->Draw(); g2->Draw("same"); }
    pad3->cd();
    if(g3->GetMaximum()>h3->GetMaximum())
      { g3->Draw(); h3->Draw("same"); } else { h3->Draw(); g3->Draw("same"); }
    pad4->cd();
    if(g4->GetMaximum()>h4->GetMaximum())
      { g4->Draw(); h4->Draw("same"); } else { h4->Draw(); g4->Draw("same"); }
    pad1b->cd();
    d1->Draw();
    pad2b->cd();
    d2->Draw();
    pad3b->cd();
    d3->Draw();
    pad4b->cd();
    d4->Draw();
    dummy->cd();
    
    n1++;
    if(n0==n1) ps=")";
    
    canvas->Update();
    canvas->Print("JetRec_RTT.ps"+ps);
    ps="";
    
    bool ok = (d1->GetSum()==0) && ( h1->GetSum() > 0 );

    ok=false;
    // make detailed plots if differences found
    if( ! ok )
      {
	canvas->Print("JetRec_Detailed_"+s+".ps(");
	
	plot1D(s, "JetE", "JetP", "JetPt", "JetPhi");
	plot1D(s, "JetDeltaR", "JetDeltaPhi", "JetDeltaEta", "JetDeltaCos");
	
	plot1D(s, "AveJetEvsEta", "AveJetEtvsEta", "AveJetPtvsEta", "AveJetMvsEta");
	plot1D(s, "AveJetEvsPhi", "AveJetEtvsPhi", "AveJetPtvsPhi", "AveJetMvsPhi");
	plot1D(s, "JetNormProf", "JetIntgProf", "AveJetPvsEta", "AveJetPvsPhi");
	
	plot2D(s, "JetEvsEta");
	plot2D(s, "JetPvsEta");
	plot2D(s, "JetEtvsEta");
	plot2D(s, "JetPtvsEta");
	plot2D(s, "JetMvsEta");

	plot2D(s, "AveJetEinEtaPhi");
	plot2D(s, "AveJetPinEtaPhi");
	plot2D(s, "AveJetEtinEtaPhi");
	plot2D(s, "AveJetPtinEtaPhi");
	plot2D(s, "AveJetMinEtaPhi", true);
	
      }
  }
}
