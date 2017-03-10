/*
 *  Macro to evaluate Higgs theory uncertainties produced using the HiggsWeightTool
 *  Dag Gillberg, March 3, 2017
 */
typedef TString Str;
typedef std::vector<TH1*> HistV;
typedef std::vector<TString> StrV;
typedef std::map<TString,HistV> HistMap;
typedef std::vector<double> NumV;
void fatal(Str msg) { printf("\n\nFATAL\n  %s\n\n",msg.Data()); abort(); }

// methods to access hists
TFile *openFile(Str fn);
TH1 *getHist(TFile *f, Str hn, int rebin=1);
TH1 *getHist(Str fn, Str hn, int rebin=1) { return getHist(openFile(fn),hn,rebin); }

// methods to draw stuff
void drawLine(double x1, double y1, double x2, double y2) {
  static TLine *line = new TLine(); line->DrawLine(x1,y1,x2,y2);
}
TH1* drawHist(TH1 *h, Str opt, int col=kBlack, int ls=1, int lw=2) {
  h->SetStats(0); h->SetLineColor(col); h->SetLineWidth(lw); h->SetLineStyle(ls); h->Draw(opt); return h;
}
void drawText(double x, double y, Str txt, int col=kBlack) {
  static TLatex *t = new TLatex(); t->SetNDC(); t->SetTextFont(42); t->SetTextColor(col); t->DrawLatex(x,y,txt);
}

// methods to calculate errors
double addInQuad(const NumV &v, double nom=0) {
  double V=0; for (auto var:v) V+=pow(var-nom,2); return sqrt(V);
}
double addInQuadRel(const NumV &v, double nom) {
  double V=0; for (auto var:v) V+=pow((var-nom)/nom,2); return sqrt(V);
}
double envelopeRel(const NumV &vec, double n, bool ignoreZero=true) {
  double max=0;
  for (auto v:vec) {
    double u=std::abs(v-n)/n; if (u>max&&!(ignoreZero&&v==0)) max=u;
  } return max;
}
// assumes uncorrelated systematic sources (Hessian)
double getTotUnc(HistV sysVar, int bin) {
  double nom=sysVar[0]->GetBinContent(bin), V=0;
  for (int i=1;i<sysVar.size();++i)
    V+=pow((sysVar[i]->GetBinContent(bin)-nom)/nom,2);
  return sqrt(V);
}

void drawRatio(TH1 *h, HistV pdf, HistV aS, HistV qcd, bool qcdEnv=true);
void addInQuad(TH1 *tot, TH1 *unc);

double hxswg(Str p) {
  //https://twiki.cern.ch/twiki/bin/view/LHCPhysics/CERNYellowReportPageAt13TeV
  if (p=="ggF") return 4.858E+01;
  if (p=="VBF") return 3.782E+00;
  if (p=="WpH") return 8.400E-01;
  if (p=="WmH") return 5.328E-01;
  if (p=="ZH" ) return 8.839E-01-1.227E-01; // sig(qq->ZH) = sig(pp->ZH) - sig(gg->ZH)
  return 0.0;
}
// print relative deviation in percent between var and nom
char *per(double relDev) { return relDev?Form("%.2f%%",relDev*100):Form("N/A"); }
char *per(double var, double nom) { return per((var-nom)/nom); }
void evaluateHiggsTheoryUncert() {
  StrV prods({"ggF","VBF","WpH","WmH","ZH"});
  gErrorIgnoreLevel=2001; // turn off those TCanvas print messages
  std::map<Str,TFile*> files;
  for (auto p:prods) files[p]=openFile(p+".root");
  // 0. Check the MC stats (how much is the stats effectively reduced due to nominal weight)
  printf("\n%8s%12s%12s%12s\n","ProdMode","Nevts","Neff","ratio");
  for (auto p:prods) {
    auto h=getHist(files[p],"pTH"); double Neff = h->GetEffectiveEntries();
    printf("%8s%12.0f%12.1f%12.4f\n",p.Data(),
	   h->GetEntries(),Neff,Neff/ h->GetEntries());
  }

  /// 1. Check the inclusive cross section
  printf("\n%8s%13s%13s%12s\n","ProdMode","sig(MC)","sig(HXSWG)","ratio");
  for (auto p:prods) {
    auto h=getHist(files[p],"pTH");
    // Average weight is the Powheg cross section
    double sigPow  = h->Integral(0,-1)/h->GetEntries();
    double sigHXWG = hxswg(p);
    printf("%8s%10.3f pb%10.3f pb%12.4f\n",p.Data(),sigPow,sigHXWG,sigHXWG/sigPow);
  }

  /// 2. Check the uncertainty on the inclusive cross section ...
  printf("\nTotal uncertainties on inclusive predictions\n");
  printf("%8s%10s%10s%10s%10s%10s%10s%10s\n",
	 "ProdMode","PDF4LHC","aS(up)","aS(dn)","QCDnlo","QCDnnlo","QCDnnlo2","QCDwg1");
  for (auto p:prods) {
    double n=getHist(files[p],"pTH")->Integral(0,-1);
    double au=getHist(files[p],"pTH_aSup")->Integral(0,-1);
    double ad=getHist(files[p],"pTH_aSdn")->Integral(0,-1);
    NumV pdfV, qcd, qcd_nnlo, qcd_nnlo2, qcdWG1;
    for (int i=1;i<=30;++i)
      pdfV.push_back(getHist(files[p],Form("pTH_pdf4lhc%i",i))->Integral(0,-1));
    for (int i=1;i<=8;++i) qcd.push_back(getHist(files[p],Form("pTH_qcd%i",i))->Integral(0,-1));
    for (int i=1;i<=26;++i) qcd_nnlo.push_back(getHist(files[p],Form("pTH_nnlops_qcd%i",i))->Integral(0,-1));
    for (int i=1;i<=2;++i) qcd_nnlo2.push_back(getHist(files[p],Form("pTH_nnlo_qcd%i",i))->Integral(0,-1));
    for (int i=1;i<=6;++i) qcdWG1.push_back(getHist(files[p],Form("pTH_wg1qcd%i",i))->Integral(0,-1));
    double pdf=addInQuadRel(pdfV,n), nnlops=addInQuadRel(qcd_nnlo2,n), wg1=addInQuadRel(qcdWG1,n);
    if (p!="ggF") wg1=0;
    printf("%8s%10s%10s%10s%10s%10s%10s%10s\n",
	   p.Data(),per(pdf),per(au,n),per(ad,n),
	   per(envelopeRel(qcd,n)),per(envelopeRel(qcd_nnlo,n)),
	   per(nnlops),per(wg1));
  }

  Str pdf("HiggsTheoryUnc.pdf");
  TCanvas *can = new TCanvas();
  can->SetMargin(0.12,0.04,0.12,0.04);
  can->Print(pdf+"[");

  for (auto p:prods) {
    TFile *f=files[p];
    for (TString var:{"pTH","Njets30","yH","STXS"}) {
      auto nom=getHist(f,var);
      nom->SetName(p+var);

      // Read in histograms with theory variations
      HistV pdfV, qcd, qcd_nnlo, qcd_nnlo2, qcd_wg1;
      for (int i=1;i<=30;++i) pdfV.push_back(getHist(f,var+Form("_pdf4lhc%i",i)));
      HistV aS({getHist(f,var+"_aSup"),getHist(f,var+"_aSdn")});
      for (int i=1;i<=8;++i) qcd.push_back(getHist(f,var+Form("_qcd%i",i)));
      for (int i=1;i<=26;++i) qcd_nnlo.push_back(getHist(f,var+Form("_nnlops_qcd%i",i)));
      for (int i=1;i<=2;++i) qcd_nnlo2.push_back(getHist(f,var+Form("_nnlo_qcd%i",i)));
      for (int i=1;i<=6;++i) qcd_wg1.push_back(getHist(f,var+Form("_wg1qcd%i",i)));

      // Draw uncertainty variations
      nom->SetMaximum(1.3*nom->GetMaximum());
      if (p=="ggF"&&var=="STXS") nom->GetXaxis()->SetRangeUser(0,15);
      drawHist(nom,""); 
      if (p=="ggF") for (auto h:qcd_wg1) drawHist(h,"hist same",kRed);
      else for (auto h:qcd) drawHist(h,"hist same",kRed);
      for (auto h:pdfV) drawHist(h,"hist same",kGray);
      for (auto h:aS) drawHist(h,"hist same",kBlue);
      drawHist(nom,"same");
      drawText(0.7,0.88,p,kBlack);
      drawText(0.7,0.83,"PDF variations",kGray);
      drawText(0.7,0.78,"#it{#alpha}_{s} variations",kBlue);
      drawText(0.7,0.73,"QCD variations",kRed);
      can->Print(pdf);

      drawRatio(nom,pdfV,aS,p=="ggF"?qcd_wg1:qcd,false);
      drawText(0.2,0.88,p,kBlack);
      drawText(0.2,0.83,"MC stat err.",kBlack);
      drawText(0.4,0.88,"PDF uncert.",kGray);
      drawText(0.4,0.83,"#it{#alpha}_{s} variations",kBlue);
      drawText(0.6,0.88,"QCD uncert.",kRed);
      can->Print(pdf);
    }
    auto w1=getHist(f,"w_nom1");
    auto w2=getHist(f,"w_nom2");
    if (w1->GetMean()<10) w2->Draw();
    else w1->Draw();
    can->Print(pdf);
  }
  
  can->Print(pdf+"]");
  printf("\nProduced %s\n\n",pdf.Data());
}

TH1* drawRatioAxis(TH1 *nom) {
  TH1D *axis = (TH1D*)nom->Clone();
  axis->GetYaxis()->SetRangeUser(0.6,1.4);
  for (int bin=1;bin<=axis->GetNbinsX();++bin) {
    double y=axis->GetBinContent(bin);
    axis->SetBinError(bin,std::abs(y)<1e-5?0:axis->GetBinError(bin)/y); axis->SetBinContent(bin,1.0);
  }
  axis->SetYTitle("Impact of systematic variation relative to nominal");
  axis->Draw(); return axis;
}

void drawRatio(TH1 *nom, HistV pdf, HistV aS, HistV qcd, bool qcdEnv) {
  drawRatioAxis(nom);
  TH1D *pdfUnc = (TH1D*)nom->Clone(); pdfUnc->Reset();
  for (int bin=1;bin<=pdfUnc->GetNbinsX();++bin) pdfUnc->SetBinContent(bin,1.0);
  TH1D *qcdUnc = (TH1D*)pdfUnc->Clone();
  for (auto h:pdf) { h->Divide(nom); addInQuad(pdfUnc,h); }
  for (auto h:qcd) { h->Divide(nom); addInQuad(qcdUnc,h); }
  qcdUnc->SetFillColor(kRed-9);
  if (qcdEnv) 
    for (int bin=1;bin<=qcdUnc->GetNbinsX();++bin) {
      double max=0;
      for (auto h:qcd)
	if (std::abs(h->GetBinContent(bin)-1)>max)
	  max=std::abs(h->GetBinContent(bin)-1);
      qcdUnc->SetBinError(bin,max==1?0:max);
    }
 
  if (Str(nom->GetName()).Contains("ggF")) qcdUnc->Draw("e2 same");
  pdfUnc->SetFillColor(kGray);
  pdfUnc->Draw("e2 same");
  for (auto h:qcd) drawHist(h,"hist same",kRed); 
  for (auto h:aS) { h->Divide(nom); drawHist(h,"hist same",kBlue); }
}


TFile *openFile(Str fn) {
  TFile *f=TFile::Open(fn);
  if (f==nullptr||f->IsZombie()) fatal("Cannot open "+fn);
  return f;
}

TH1 *getHist(TFile *f, Str hn, int rebin) {
  TH1 *h = (TH1*)f->Get(hn);
  if (h==nullptr) fatal("Cannot access histo "+hn+" in file "+f->GetName());
  h->Rebin(rebin); return h;
}

// add uncertainty source unc to total
// unc is relative to 1
void addInQuad(TH1 *tot, TH1 *uncVar) {
  for (int bin=1;bin<=tot->GetNbinsX();++bin) {
    double err1 = tot->GetBinError(bin);
    double err2 = uncVar->GetBinContent(bin)-1;
    if (err2!=-1)
      tot->SetBinError(bin,sqrt(err1*err1+err2*err2));
  }
}
