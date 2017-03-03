/*
 *  Macro to evaluate Higgs theory uncertainties produced using the HiggsWeightTool
 *  Dag Gillberg, March 3, 2017
 */
typedef TString Str;
typedef std::vector<TH1F*> HistV;
typedef std::vector<TString> StrV;
typedef std::map<TString,HistV> HistMap;
typedef std::vector<double> NumV;
void fatal(Str msg) { printf("\n\nFATAL\n  %s\n\n",msg.Data()); abort(); }
TFile *openFile(Str fn);
double addInQuad(const NumV &v, double nom=0) {
  double V=0; for (auto var:v) V+=pow(var-nom,2); return sqrt(V);
}
double addInQuadRel(const NumV &v, double nom) {
  double V=0; for (auto var:v) V+=pow((var-nom)/nom,2); return sqrt(V);
}
TH1 *getHist(TFile *f, Str hn, int rebin=1);
TH1 *getHist(Str fn, Str hn, int rebin=1) { return getHist(openFile(fn),hn,rebin); }

void drawLine(double x1, double y1, double x2, double y2) {
  static TLine *line = new TLine(); line->DrawLine(x1,y1,x2,y2);
}

TH1F *makeHist(Str name, int Nbins, double xmin, double xmax, Str tit="") {
  TH1F *h = new TH1F(name,tit,Nbins,xmin,xmax); return h;
}

TH1* drawHist(TH1 *h, Str opt, int col=kRed, int ls=1, int lw=2) {
  h->SetStats(0); h->SetLineColor(col); h->SetLineWidth(lw); h->SetLineStyle(ls); h->Draw(opt); return h;
}

double getTotUnc(HistV sysVar, int bin) {
  double nom=sysVar[0]->GetBinContent(bin), V=0;
  for (int i=1;i<sysVar.size();++i)
    V+=pow((sysVar[i]->GetBinContent(bin)-nom)/nom,2);
  return sqrt(V);
}

double hxswg(Str p) {
  //https://twiki.cern.ch/twiki/bin/view/LHCPhysics/CERNYellowReportPageAt13TeV
  if (p=="ggF") return 4.858E+01;
  if (p=="VBF") return 3.782E+00;
  if (p=="WpH") return 8.400E-01;
  if (p=="WmH") return 5.328E-01;
  if (p=="ZH" ) return 8.839E-01-1.227E-01;
  return 0.0;
}
char *per(double var, double nom) { return Form("%.2f%%",(var-nom)/nom*100); }
void evaluateHiggsTheoryUncert() {
  StrV prods({"ggF","VBF","WpH","WmH","ZH"});
  Str pdf("HiggsTheoryUnc.pdf");
  TCanvas *can = new TCanvas();
  can->Print(pdf+"[");

  std::map<Str,TFile*> files;
  for (auto p:prods) files[p]=openFile(p+".root");
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
  printf("%8s%10s%10s%10s\n","ProdMode","PDF4LHC","aS(up)","aS(dn)");
  for (auto p:prods) {
    double n=getHist(files[p],"pTH")->Integral(0,-1);
    double au=getHist(files[p],"pTH_aSup")->Integral(0,-1);
    double ad=getHist(files[p],"pTH_aSdn")->Integral(0,-1);
    NumV pdfV;
    for (int i=1;i<=30;++i)
      pdfV.push_back(getHist(files[p],Form("pTH_pdf4lhc%i",i))->Integral(0,-1));
    double pdf=addInQuadRel(pdfV,n);
    printf("%8s%10s%10s%10s\n",p.Data(),per(pdf+1,1),per(au,n),per(ad,n));
  }
  
  
  can->Print(pdf+"]");
  printf("\nProduced %s\n\n",pdf.Data());
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
