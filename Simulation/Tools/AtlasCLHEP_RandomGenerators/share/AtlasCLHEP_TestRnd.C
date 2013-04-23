void test(TH1D* h,TString type,double maxx=5)
{
  TCanvas* c1=new TCanvas(h->GetName(),h->GetTitle());
  c1->Divide(3);
  TVirtualPad* pad=c1->cd(1);
  h->Sumw2();
  h->SetLineWidth(3);
  TH1D* hc=(TH1D*)h->Clone(TString("res_")+h->GetName());
  TH1D* ht=(TH1D*)h->Clone(TString("tmp_")+h->GetName());
  h->Draw();
  pad->SetLogy();
  h->Fit(type);
  TF1* fit=h->GetFunction(type);
  fit->SetLineColor(2);
  fit->SetLineWidth(1);
  
  TH1D* hn=new TH1D(TString("resn_")+h->GetName(),h->GetTitle(),100,-5,5);
  const double sqrt2=TMath::Sqrt(2);
  ht->Reset();
  ht->SetLineColor(3);
  ht->SetLineWidth(1);
  double ymin=0;
  double ymax=0;
  for(int i=1;i<=hc->GetNbinsX();++i) {
    double x1=hc->GetXaxis()->GetBinLowEdge(i);
    double x2=hc->GetXaxis()->GetBinUpEdge(i);
    double prob=1;
    if(type=="gaus") prob=0.5*hc->GetEntries()*(TMath::Erfc(x1/sqrt2) - TMath::Erfc(x2/sqrt2));
    if(type=="pol0") prob=hc->GetEntries()*(x2-x1);
    if(type=="expo") prob=hc->GetEntries()*(TMath::Exp(-x1)-TMath::Exp(-x2));
    double res=0;
    if(prob>0) res=(hc->GetBinContent(i)-prob)/TMath::Sqrt(prob);
    if(res<ymin) ymin=res;
    if(res>ymax) ymax=res;
    
    //if(i<10) cout<<"x1="<<x1<<" x2="<<x2<<" y="<<hc->GetBinContent(i)<<" prob="<<prob<<" res="<<res<<endl;
    
    hc->SetBinContent(i,res);
    ht->SetBinContent(i,prob);
    if(TMath::Abs(x1)<maxx) hn->Fill(res);
  }
  
  ht->Draw("same");
  h->Draw("same");
  
  //TCanvas* c2=new TCanvas(hc->GetName(),hc->GetTitle());
  c1->cd(2);
  hc->SetLineWidth(0);
  hc->SetLineColor(10);
  hc->SetMarkerColor(1);
  hc->SetMarkerSize(1);
  hc->SetMarkerStyle(6);
  hc->SetTitle("Residual");
  hc->GetYaxis()->SetRangeUser(ymin,ymax);
  hc->Draw("P9");
  //ht->Draw("same");
  //TCanvas* c3=new TCanvas(hn->GetName(),hn->GetTitle());
  c1->cd(3);
  hn->SetTitle(Form("Residual pull for |x|<%3.1f",maxx));
  hn->Draw();
  hn->Fit("gaus");
}

void AtlasCLHEP_TestRnd()
{
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(1100);
  gStyle->SetOptFit(11);

  TFile* f=TFile::Open("random_gen.root");
  TH1D* hflat=(TH1D*)(f->Get("RandFlat"));
  
  TH1D* h1=(TH1D*)(f->Get("RandGauss"));
  TH1D* h2=(TH1D*)(f->Get("RandGaussQ"));
  TH1D* h3=(TH1D*)(f->Get("RandGaussZigurat"));

  TH1D* hexp1=(TH1D*)(f->Get("RandExponential"));
  TH1D* hexp2=(TH1D*)(f->Get("RandExpZiggurat"));
  
  test(hflat,"pol0");
  
  test(h1,"gaus");
  test(h2,"gaus");
  test(h3,"gaus");
  
  test(hexp1,"expo",15);
  test(hexp2,"expo",15);
}
