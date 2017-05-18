/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TH1.h"
#include "TFile.h"
#include "TCanvas.h"
#include "AtlasUtils.C"
#include "AtlasStyle.C"
#include "AtlasLabels.C"

#include <string>
#include <map>

/////////////////////////////////////////////////////////////////////
// CREATED : 01/02/2016
// AUTHOR : Michele Pinamonti (Univ. Udine), Frederic Derue (LPNHE Paris)
// PURPOSE : ROOT macro to plot a summary of all efficiencies 
//           vs different variables
//           It reads as input the default efficiencies of TopFakes package
//           void EffSummary() is the core of this macro
// MODIFICATION : 
//
/////////////////////////////////////////////////////////////////////

class MMEffComparison {
public:
  MMEffComparison(){
    //       _c = new TCanvas("c","c",600,600);
    _totH = 0;
    _label = "";
    _dummyHist = 0;
    _noYlabels = false;
  };
  ~MMEffComparison(){};
  
  
  void SetIsReal(int isReal=0){
    _isReal = isReal;
  };
  
  void SetXaxis(float min=0,float max=1,string title,int njetbins=0){
    _dummyHist = new TH1F("h","h",1,min,max);
    _dummyHist->GetXaxis()->SetTitle(title.c_str());
    if(njetbins>0){
      _dummyHist->GetXaxis()->SetNdivisions(njetbins);
      _dummyHist->GetXaxis()->CenterLabels();
    }
  };
  
  void SetYaxis(float min=0,float max=1){
    _dummyHist = new TH1F("h","h",1,min,max);
    //_dummyHist->GetYaxis()->SetTitle(title.c_str());
    /*if(njetbins>0){
      _dummyHist->GetYaxis()->SetNdivisions(njetbins);
      _dummyHist->GetYaxis()->CenterLabels();
      }*/
  };
  
  void Add(string filename,string histoname,string label,int color,int style,int color2=-1,int style2=-1){
    TFile *f = new TFile(filename.c_str());
    std::cout << " filename = " << filename << std::endl;
    std::cout << " histoname = " << histoname << std::endl;
    _h[_totH] = (TH1*)f->Get(histoname.c_str())->Clone();
    
    //       cout << "Adding " << filename << ", " << histoname << endl;
    //       cout << _h[_totH]->GetBinContent(1) << endl;
    
    if(!_dummyHist){
      _dummyHist = (TH1F*)_h[_totH]->Clone("h_dummy");
      for(int i_bin=1;i_bin<=_dummyHist->GetNbinsX();i_bin++){
	_dummyHist->SetBinContent(i_bin,0);
	_dummyHist->SetBinError(i_bin,0);
      }
    }
    
    _h[_totH]->SetName(label.c_str());
    _h[_totH]->SetTitle(label.c_str());
    _h[_totH]->SetLineColor(color);
    _h[_totH]->SetLineWidth(1);
    //       _h[_totH]->SetLineStyle(style);
    
    //       _h[_totH]->SetMarkerSize(0);
    _h[_totH]->SetMarkerSize(1.5);
    _h[_totH]->SetMarkerStyle(style);
    _h[_totH]->SetMarkerColor(color);
    
    //       _h_syst_up[_totH] = (TH1*)_h[_totH]->Clone(Form("%s_syst_up",histoname.c_str()));
    //       _h_syst_down[_totH] = (TH1*)_h[_totH]->Clone(Form("%s_syst_down",histoname.c_str()));
    
    _h_syst[_totH] = new TGraphAsymmErrors(_h[_totH]->GetNbinsX());
    for(int i_bin=1;i_bin<=_h[_totH]->GetNbinsX();i_bin++){
      _h_syst[_totH]->SetPoint(i_bin-1,_h[_totH]->GetXaxis()->GetBinCenter(i_bin), _h[_totH]->GetBinContent(i_bin) );
      _h_syst[_totH]->SetPointEYhigh( i_bin-1, _h[_totH]->GetBinError(i_bin) );
      _h_syst[_totH]->SetPointEYlow( i_bin-1, _h[_totH]->GetBinError(i_bin) );
      _h_syst[_totH]->SetPointEXhigh( i_bin-1, _h[_totH]->GetXaxis()->GetBinWidth(i_bin)/2. );
      _h_syst[_totH]->SetPointEXlow( i_bin-1, _h[_totH]->GetXaxis()->GetBinWidth(i_bin)/2. );
      }
    
    _h_syst[_totH]->SetMarkerSize(0);
    _h_syst[_totH]->SetLineColor(color);
    _h_syst[_totH]->SetLineStyle(style);
    
    if(color2>=0) _h_syst[_totH]->SetFillColor(color2);
    else          _h_syst[_totH]->SetFillColor(color);
    if(style2>=0) _h_syst[_totH]->SetFillStyle(style2);//3454);
    else          _h_syst[_totH]->SetFillStyle(3454);
    //       _h_syst[_totH]->SetLineWidth(2);
    
    _index[label] = _totH;
    _totH ++;
  };
  
  void AddSyst(string filename,string histoname,string label){
    TFile *f = new TFile(filename.c_str());   
    //       cout << "Adding syst to " << filename << ", " << histoname << " -> " << _index[label] << endl;
    AddSystToHist( (TH1*)f->Get(histoname.c_str()), _h_syst[_index[label]] );
  };
  
  void SetLabel(string label){
    _label = label;
  };
  
  void Draw(){
    bool xaxisSet = false;
    if(_dummyHist!=0) _dummyHist->Draw();
    for(int i=0;i<_totH;i++){
      //         if(i==0 && _dummyHist==0) _h_syst[i]->Draw("E2 same");
      //         else     
      _h_syst[i]->Draw("E2 same");
      //         if(i==0 && _dummyHist==0) 
      //           _h[i]->Draw("E same");
      //         else     
      _h[i]->Draw("E same");
      //         if(i>0 && !xaxisSet) _h[0]->GetXaxis()->SetTitle( _h[i]->GetXaxis()->GetTitle() );
      string s = _h[i]->GetXaxis()->GetTitle();
      if( s != "" ) xaxisSet = true;
    }
    
    if(_noYlabels){
      _dummyHist->GetYaxis()->SetTitle("");
      _dummyHist->GetYaxis()->SetLabelSize(0);
    }
    else{
      if(_isReal==0) _dummyHist->GetYaxis()->SetTitle("#epsilon_{fake}");
      if(_isReal==1) _dummyHist->GetYaxis()->SetTitle("#epsilon_{real}");
      if(_isReal==2) _dummyHist->GetYaxis()->SetTitle("Efficiency");        
    }
    
    //       if(_isReal) _dummyHist->SetMaximum(1.65);
    if(_isReal) _dummyHist->SetMaximum(1.3);
    else        _dummyHist->SetMaximum(1.00);
    _dummyHist->SetMinimum(0);
    
    // fix empty bins and error bars...
    for(int i=0;i<_totH;i++){
      for(int i_bin=1;i_bin<=_h[i]->GetNbinsX();i_bin++){
	//           _h[i]->SetBinError(i_bin,0.0001);
	if( _h[i]->GetBinContent(i_bin)<=0 ) _h_syst[i]->SetPointEYhigh(i_bin-1,0);
	if( _h[i]->GetBinContent(i_bin)<=0 ) _h_syst[i]->SetPointEYlow(i_bin-1,0);
	//           if( _h[i]->GetTitle() )
      }
    }
    
    gPad->RedrawAxis();
  };
  
  void DrawLegend(float x0=0.2, float y0=0.7, float x1=0.9, float y1=0.9){
    TLegend *l;
    if(_isReal==2) l = new TLegend(x0,y0,x1,y1);
    if(_isReal==2) l->SetNColumns(3);
    l->SetFillStyle(0);
    l->SetBorderSize(0);
    l->SetTextSize(0.06);
    for(int i=0;i<_totH;i++){
      //         l->AddEntry(_h_syst[i],_h[i]->GetTitle(),"lf");
      _h_forLegend[i] = (TH1F*)_h[i]->Clone(Form("histForLegend[%d]",i));
      _h_forLegend[i]->SetFillColor(_h_syst[i]->GetFillColor());
      _h_forLegend[i]->SetFillStyle(_h_syst[i]->GetFillStyle());
      l->AddEntry(_h_forLegend[i],_h[i]->GetTitle(),"lpf");
    }
    l->Draw();
  }
  
  TH1* GetHist(int idx){
    return _h[idx];
  };
  
  TH1F* GetDummyHist(){
    return _dummyHist;
  };
  
  bool _noYlabels;
  
private:
  TH1* _h[100];
  TH1* _h_forLegend[100];
  //     TH1* _h_syst[100];
  TGraphAsymmErrors* _h_syst[100];
  //     TH1* _h_syst_up[100];
  //     TH1* _h_syst_down[100];
  int _totH;
  string _label;
  int _isReal;
  TH1F* _dummyHist;
  map<string,int> _index;
};

// --------------------------------------------------

class MMEffComparisonSet {
public:
  MMEffComparisonSet(int w=600,int h=600){
    _c = new TCanvas("c","c",w,h);
    _N = 0;
    _idx = 0;
    _ec[0] = new MMEffComparison();
    Cd(0);
  };
  ~MMEffComparisonSet(){};
  
  void Divide(int a=2, int b=1){
    _c->Divide(a,b, 0,0 );
    _N=a*b;
    for(int i=0;i<_N;i++){
      _ec[i] = new MMEffComparison();
      if(i>0) _ec[i]->_noYlabels = true;
    }
    Cd(0);
  };
  
  void Cd(int i){
    _c->cd(i+1);
    _idx = i;
  };
  
  void SaveAs(string name){
    _c->SaveAs(name.c_str());
  };
  
  
  // methods from MMEffComparison:
  void SetIsReal(int isReal=0){
    _ec[_idx]->SetIsReal(isReal);
  };
  
  void SetXaxis(float min=0,float max=1,string title,int njetbins=0){
    _ec[_idx]->SetXaxis(min,max,title,njetbins);
  };
  
  void SetYaxis(float min=0,float max=1){
    _ec[_idx]->SetYaxis(min,max);
  };
  
  void Add(string filename,string histoname,string label,int color,int style,int color2=-1,int style2=-1){
    _ec[_idx]->Add(filename,histoname,label,color,style,color2,style2);
  };
  
  void AddSyst(string filename,string histoname,string label){
    _ec[_idx]->AddSyst(filename,histoname,label);
  };
  
  void SetLabel(string label){
    _ec[_idx]->SetLabel(label);
  };
  
  void Draw(){
    _ec[_idx]->Draw();
  };
  
  void DrawLegend(float x0=0.2, float y0=0.7, float x1=0.9, float y1=0.9){
    _ec[_idx]->DrawLegend(x0,y0,x1,y1);
  }
  
  TH1* GetHist(int idx){
    return _ec[_idx]->GetHist(idx);
  };
  
  TCanvas* GetCanvas(){
    return _c;
  };
  
  TH1F* GetDummyHist(){
    return _ec[_idx]->GetDummyHist();
  };
  
private:
  TCanvas *_c;
  
  MMEffComparison *_ec[10];
  int _N;
  int _idx;
  
};

// --------------------------------------------------

void AddSystToHist(TH1* h, TGraphAsymmErrors* h_syst){
  float x;
  int j;
  float diff;
  for(int i=0;i<h_syst->GetN();i++){
    x = h_syst->GetX()[i];
    j = h->GetXaxis()->FindBin(x);
    diff = h->GetBinContent(j) - h_syst->GetY()[i];
    // syst cutoff ;)
    //if(diff>0.2) diff = 0.2;
    //if(diff<-0.2) diff = -0.2;
    //
//     if(x<h_syst->GetXaxis()->GetXmax()  &&  j<=h_syst->GetN() && h_syst->GetY[j]>0 ){
      if(diff>0) h_syst->SetPointEYhigh( i, SqSum( h_syst->GetErrorYhigh(i), diff ) );
      if(diff<0) h_syst->SetPointEYlow( i, SqSum( h_syst->GetErrorYlow(i), diff ) );
//     }
  }
};

float SqSum(float a,float b){
  return sqrt( pow(a,2) + pow(b,2) );
};
    
// --------------------------------------------------


void EffSummary(string ch="el",string var="all_1",
		string jet="",string bjet="_ge1b",int isReal=2) 
{  
  // ch : "el" for electrons, "mu" for muons
  // var: "all_1" for eta,pt,dR
  //      "all_2" for ptjet,njet,nbtag and dphi
  // jet: jet bin
  // bjet: b-jet bin
  // isReal = 2 means both fake and real

  SetAtlasStyle();
  gROOT->Reset();
  gROOT->SetStyle("ATLAS");
  gROOT->ForceStyle();
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);
  
  MMEffComparisonSet *c = new MMEffComparisonSet(1800,600*1800/1400);
  
  if(ch=="el"){
    string trig[] = {""}; // no trigger splitting
  }
  else{
    string trig[] = {"mu20i","mu50","mu20"}
  }

  // define the variables
  if(ch=="el"){
    if(var=="all_1"){
      // e channel
      int Nvar = 3;
      string vari[] =  {"eta",        "pt",              "dR"};
      string label[] = {"|#eta(elec)|  ","p_{T}(elec) [GeV]  ","#DeltaR(elec,jet)  "};
      float xmin[] =   {0,            21,                0.};
      float xmax[] =   {2.6,          149,               5.9};
    }
    if(var=="all_2"){
      int Nvar = 4;
      string vari[] =  {"jetpt",                      "njet",        "nbtag",             "dPhi"};
      string label[] = {"p_{T}(leading jet) [GeV]  ", "n_{jet}  ",  "n_{b-jet}  ",      "#Delta#phi(e,E_{T}^{miss})  " };
      float xmin[] =   {0,                           0.9,             -0.1,                   -0.1};
      float xmax[] =   {530,                          6.1,             3.1,                   3.3};
    }
  }
  
  if(ch=="mu"){
    // mu channel
    if(var=="all_1"){
      int Nvar = 3;
      string vari[] =  {"eta",        "pt",              "dR"};
      string label[] = {"|#eta(#mu)| ","p_{T}(#mu) [GeV] ","#DeltaR(#mu,jet) "};
      float xmin[] =   {0,            21,                0.};
      float xmax[] =   {2.6,          149,               5.9};
    }
    if(var=="all_2"){
      int Nvar = 4;
      string vari[] =  {"jetpt",                      "njet",        "nbtag",             "dPhi"};
      string label[] = {"p_{T}(leading jet) [GeV]  ", "n_{jet}  ",  "n_{b-jet}  ",      "#Delta#phi(#mu,E_{T}^{miss})  " };
      float xmin[] =   {0,                           0.9,             -0.1,                   -0.1};
      float xmax[] =   {530,                          6.1,             3.1,                   3.3};
    }
  }
    

  // ----
  
  c->Divide(Nvar);
  //   c->GetCanvas()->GetPad(1)->SetRightMargin(0.3);
  //   c->GetCanvas()->GetPad(2)->SetRightMargin(0.5);
  if(Nvar==4){
    c->GetCanvas()->GetPad(1)->SetPad(0, 0, 0.367672, 0.942544);
    c->GetCanvas()->GetPad(2)->SetPad(0.367672,0, 0.57,0.942544);    // w = 0.57 - 0.367672; origw = 0.983166 - 0.675419
    c->GetCanvas()->GetPad(3)->SetPad(0.57,0, 0.675419,0.942544);    // w = 0.675419 - 0.57; origw = 0.983166 - 0.675419
    c->GetCanvas()->GetPad(4)->SetPad(0.675419, 0, 0.983166, 0.942544);
    //     0.367672, 0, 0.675419, 0.942544
  }
  else{
    cout << c->GetCanvas()->GetPad(1)->GetXlowNDC() << ", ";
    cout << c->GetCanvas()->GetPad(1)->GetYlowNDC() << ", ";
    cout << c->GetCanvas()->GetPad(1)->GetXlowNDC()+c->GetCanvas()->GetPad(1)->GetAbsWNDC() << ", ";
    cout << c->GetCanvas()->GetPad(1)->GetYlowNDC()+c->GetCanvas()->GetPad(1)->GetAbsHNDC();
    cout << endl;

    cout << c->GetCanvas()->GetPad(2)->GetXlowNDC() << ", ";
    cout << c->GetCanvas()->GetPad(2)->GetYlowNDC() << ", ";
    cout << c->GetCanvas()->GetPad(2)->GetXlowNDC()+c->GetCanvas()->GetPad(2)->GetAbsWNDC() << ", ";
    cout << c->GetCanvas()->GetPad(2)->GetYlowNDC()+c->GetCanvas()->GetPad(2)->GetAbsHNDC();
    cout << endl;
    
    cout << c->GetCanvas()->GetPad(3)->GetXlowNDC() << ", ";
    cout << c->GetCanvas()->GetPad(3)->GetYlowNDC() << ", ";
    cout << c->GetCanvas()->GetPad(3)->GetXlowNDC()+c->GetCanvas()->GetPad(3)->GetAbsWNDC() << ", ";
    cout << c->GetCanvas()->GetPad(3)->GetYlowNDC()+c->GetCanvas()->GetPad(3)->GetAbsHNDC();
    cout << endl;
  }
  
  for(int i=0;i<Nvar;i++){
    c->Cd(i);
    c->SetYaxis(0.,1.5);
    c->SetXaxis(xmin[i],xmax[i],label[i]);
    if(vari[i]=="nbtag")     c->SetXaxis(xmin[i],xmax[i],label[i],5);
    if(vari[i]=="njet")      c->SetXaxis(xmin[i],xmax[i],label[i],7);

    c->SetIsReal(isReal);

    // ==================================================
    // case for electrons
    if(ch=="el") {
      //std::cout << " add real efficiencies " << std::endl;
      // real efficiencies
      c->Add("../data/R_"+ch+"_Default.root","eff_"+vari[i]+jet,"#varepsilon_{r} ",kBlack,kOpenTriangleUp,kBlack,3445);
      // add systematics (CRreal)
      //std::cout << " add CRreal " << std::endl;
      c->AddSyst("../data/R_"+ch+"_Default_CRreal.root","eff_"+vari[i]+bjet+jet,"#varepsilon_{r} ");

      //std::cout << " add fake efficiencies " << std::endl;
      // fake efficiencies
      c->Add("../data/F_"+ch+"_Default.root","eff_"+vari[i]+bjet+jet,"#varepsilon_{f} ",kBlack,kFullTriangleUp,kBlack,3445);      
      // add systematics MCup
      //std::cout << " add MCup " << std::endl;
      c->AddSyst("../data/F_"+ch+"_Default_MCup.root","eff_"+vari[i]+bjet+jet,"#varepsilon_{f} ");      
      // add systematics MCdown
      //std::cout << " add MCdown " << std::endl;
      c->AddSyst("../data/F_"+ch+"_Default_MCdown.root","eff_"+vari[i]+bjet+jet,"#varepsilon_{f} ");
      //std::cout << " add CRfake " << std::endl;
      // add systematics CRfake
      c->AddSyst("../data/F_"+ch+"_Default_CRfake.root","eff_"+vari[i]+bjet+jet,"#varepsilon_{f} ");
    }
   // ==================================================
    // case for muons
    if(ch=="mu") {
      //std::cout << " add real efficiencies T&P" << std::endl;
      // real efficiencies
      c->Add("../data/R_"+ch+"_Default.root","eff_"+vari[i]+jet+"_"+trig[0],"#varepsilon_{r} "+trig[0],kRed,kOpenSquare,kRed-10,1001);
      c->Add("../data/R_"+ch+"_Default.root","eff_"+vari[i]+jet+"_"+trig[1],"#varepsilon_{r} "+trig[1],kBlack,kOpenTriangleUp,kBlack,3445);
      c->Add("../data/R_"+ch+"_Default.root","eff_"+vari[i]+jet+"_"+trig[2],"#varepsilon_{r} "+trig[2],kBlue,kOpenCircle,kBlue-7);//kAzure);
      // add systematics (CRreal)
      //std::cout << " add real efficiencies high MTW" << std::endl;
      //std::cout << " file = " << "../data/R_"+ch+"_Default_CRreal.root" << std::endl;
      c->AddSyst("../data/R_"+ch+"_Default_CRreal.root","eff_"+vari[i]+bjet+jet+"_"+trig[0],"#varepsilon_{r} "+trig[0]);
      c->AddSyst("../data/R_"+ch+"_Default_CRreal.root","eff_"+vari[i]+bjet+jet+"_"+trig[1],"#varepsilon_{r} "+trig[1]);
      c->AddSyst("../data/R_"+ch+"_Default_CRreal.root","eff_"+vari[i]+bjet+jet+"_"+trig[2],"#varepsilon_{r} "+trig[2]);
      
      //std::cout << " add fake efficiencies " << std::endl;
      // fake efficiencies
      c->Add("../data/F_"+ch+"_Default.root","eff_"+vari[i]+bjet+jet+"_"+trig[0],"#varepsilon_{f} "+trig[0],kRed,kFullSquare,kRed-10,1001);
      c->Add("../data/F_"+ch+"_Default.root","eff_"+vari[i]+bjet+jet+"_"+trig[1],"#varepsilon_{f} "+trig[1],kBlack,kFullTriangleUp,kBlack,3445);
      c->Add("../data/F_"+ch+"_Default.root","eff_"+vari[i]+bjet+jet+"_"+trig[2],"#varepsilon_{f} "+trig[2],kBlue,kFullCircle,kBlue-7);
      
      // add systematics MCup
      c->AddSyst("../data/F_"+ch+"_Default_MCup.root","eff_"+vari[i]+bjet+jet+"_"+trig[0],"#varepsilon_{f} "+trig[0]);
      c->AddSyst("../data/F_"+ch+"_Default_MCup.root","eff_"+vari[i]+bjet+jet+"_"+trig[1],"#varepsilon_{f} "+trig[1]);
      c->AddSyst("../data/F_"+ch+"_Default_MCup.root","eff_"+vari[i]+bjet+jet+"_"+trig[2],"#varepsilon_{f} "+trig[2]);

      // add systematics MCdown
      c->AddSyst("../data/F_"+ch+"_Default_MCdown.root","eff_"+vari[i]+bjet+jet+"_"+trig[0],"#varepsilon_{f} "+trig[0]);
      c->AddSyst("../data/F_"+ch+"_Default_MCdown.root","eff_"+vari[i]+bjet+jet+"_"+trig[1],"#varepsilon_{f} "+trig[1]);
      c->AddSyst("../data/F_"+ch+"_Default_MCdown.root","eff_"+vari[i]+bjet+jet+"_"+trig[2],"#varepsilon_{f} "+trig[2]);
      
      // add systematics CRfake
      c->AddSyst("../data/F_"+ch+"_Default_CRfake.root","eff_"+vari[i]+bjet+jet+"_"+trig[0],"#varepsilon_{f} "+trig[0]);
      c->AddSyst("../data/F_"+ch+"_Default_CRfake.root","eff_"+vari[i]+bjet+jet+"_"+trig[1],"#varepsilon_{f} "+trig[1]);
      c->AddSyst("../data/F_"+ch+"_Default_CRfake.root","eff_"+vari[i]+bjet+jet+"_"+trig[2],"#varepsilon_{f} "+trig[2]);
    }
    c->Draw();
    
    if(i==Nvar-1){
      if(ch=="el") c->DrawLegend(0.05,0.8, 0.95,0.95);
      if(ch=="mu") c->DrawLegend(0.05,0.8, 0.95,0.95);
    }
  }
  
  // ------

  if(Nvar==4){
    c->Cd(0);
    c->GetDummyHist()->GetXaxis()->SetNdivisions(506);
    // w = 0.57 - 0.367672; origw = 0.942544 - 0.367672
    c->Cd(1);
    c->GetDummyHist()->GetXaxis()->SetLabelSize( ((0.983166 - 0.675419)/(0.57 - 0.367672)) * c->GetDummyHist()->GetXaxis()->GetLabelSize() );
    c->GetDummyHist()->GetXaxis()->SetTitleSize( ((0.983166 - 0.675419)/(0.57 - 0.367672)) * c->GetDummyHist()->GetXaxis()->GetTitleSize() );
    c->GetDummyHist()->GetXaxis()->SetLabelOffset(-0.015);
    c->GetDummyHist()->GetXaxis()->SetTitleOffset(0.9);
    //
    c->Cd(2);
    c->GetDummyHist()->GetXaxis()->SetLabelSize( ((0.983166 - 0.675419)/(0.675419 - 0.57)) * c->GetDummyHist()->GetXaxis()->GetLabelSize() );
    c->GetDummyHist()->GetXaxis()->SetTitleSize( ((0.983166 - 0.675419)/(0.675419 - 0.57)) * c->GetDummyHist()->GetXaxis()->GetTitleSize() );
    c->GetDummyHist()->GetXaxis()->SetLabelOffset(-0.065);
    c->GetDummyHist()->GetXaxis()->SetTitleOffset(0.46);
  }
  
  
  // increase x-axis size
  c->Cd(0);
  c->GetDummyHist()->GetXaxis()->SetLabelSize( 0.85 * c->GetDummyHist()->GetXaxis()->GetLabelSize() );
  c->GetDummyHist()->GetXaxis()->SetTitleSize( 0.85 * c->GetDummyHist()->GetXaxis()->GetTitleSize() );
  c->GetDummyHist()->GetXaxis()->SetLabelOffset(0.012);
  c->GetDummyHist()->GetXaxis()->SetTitleOffset(1.6);
  for(int i=0;i<Nvar;i++){
    c->Cd(i);
    c->GetDummyHist()->GetXaxis()->SetLabelSize( 1.2 * c->GetDummyHist()->GetXaxis()->GetLabelSize() );
    c->GetDummyHist()->GetXaxis()->SetTitleSize( 1.2 * c->GetDummyHist()->GetXaxis()->GetTitleSize() );
  }
  
  
  c->Cd(0);
  if(ch=="el"){
    ATLASLabel(0.25,0.93,"Work in progress");
    myText(0.25,0.87,1,"#sqrt{s}=13 TeV, 3.2 fb^{-1}",0.045);
  }
  if(ch=="mu"){
    ATLASLabel(0.25,0.93,"Work in progress");
    myText(0.25,0.87,1,"#sqrt{s}=13 TeV, 3.2 fb^{-1}",0.045);
  }

  // adapt to your case
  // for electrons
  if (ch=="el") {
    if (jet=="_1j") {
      if (bjet=="_0b")
	myText(0.65,0.87,1,"e+1 jet, 0 b-jet",0.045);
      if (bjet=="_ge1b")
	myText(0.65,0.87,1,"e+1 jet, #geq1 b-jet",0.045);
    }
    if (jet=="") {
      if (bjet=="_0b")
      myText(0.65,0.87,1,"e+#geq 1 jet, 0 b-jet",0.045);
      if (bjet=="_ge1b")
	myText(0.65,0.87,1,"e+#geq 1 jet, #geq1 b-jet",0.045);
    }
    if (jet=="_ge2j") {
      if (bjet=="_0b")
	myText(0.65,0.87,1,"e+#geq 2 jets, 0 b-jet",0.045);
      if (bjet=="_ge1b")
	myText(0.65,0.87,1,"e+#geq 2 jets, #geq1 b-jet",0.045);
    }
  }
  // for muons
  if (ch=="mu") {
    if (jet=="_1j") {
      if (bjet=="_0b")
	myText(0.65,0.87,1,"#mu+1 jet, 0 b-jet",0.045);
      if (bjet=="_ge1b")
	myText(0.65,0.87,1,"#mu+1 jet, #geq1 b-jet",0.045);
    }
    if (jet=="") {
      if (bjet=="_0b")
      myText(0.65,0.87,1,"#mu+#geq 1 jet, 0 b-jet",0.045);
      if (bjet=="_ge1b")
	myText(0.65,0.87,1,"#mu+#geq 1 jet, #geq1 b-jet",0.045);
    }
    if (jet=="_ge2j") {
      if (bjet=="_0b")
	myText(0.65,0.87,1,"#mu+#geq 2 jets, 0 b-jet",0.045);
      if (bjet=="_ge1b")
	myText(0.65,0.87,1,"#mu+#geq 2 jets, #geq1 b-jet",0.045);
    }
  }

  string name;
  if(isReal==0) name = "FakeEffComp_"+ch+"_"+var+bjet+jet;
  if(isReal==1) name = "RealEffComp_"+ch+"_"+var+bjet+jet;
  if(isReal==2) name = "EffComp_"+ch+"_"+var+bjet+jet;
  c->SaveAs(("fig/"+name+".png").c_str());
  c->SaveAs(("fig/"+name+".eps").c_str());
  c->SaveAs(("fig/"+name+".pdf").c_str());
  
  TFile *fNew = new TFile("Test.root","RECREATE");
  gPad->Write("",TObject::kOverwrite);

}

