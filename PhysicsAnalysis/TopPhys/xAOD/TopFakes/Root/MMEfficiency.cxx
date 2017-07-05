/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 ** author: Michele Pinamonti <michele.pinamonti@cern.ch>
 *    * class used by FakesWeights.cxx
*/

#include "TopFakes/MMEfficiency.h"

#define XXX std::cout << "I am here: " << __FILE__ << ":" << __LINE__ << std::endl;

// ------------------------ Implementation ------------------------ //

MMEfficiency::MMEfficiency() :
  fAverage(-1),
  fNvar(0) {

  for (int i = 0; i < MAXvar; ++i) {
    fHist[i] = 0;
    fHist2D[i] = 0;
    fHist3D[i] = 0;
  }
}

MMEfficiency::~MMEfficiency(){
  for (int i = 0; i < MAXvar; ++i) {
    delete fHist[i];
    delete fHist2D[i];
    delete fHist3D[i];
  }
}

// ------------------------ AddHisto ------------------------ //

bool MMEfficiency::AddHisto(TH1F *h){
  bool isDummy = false;
  if(h==0) isDummy = true;
  if(isDummy){
//     std::cout << "  MMEfficiency::INFO: Dummy Hstogram added." << std::endl;
    h = new TH1F("h_dummmy","h_dummy",1,0,1);
    h->SetBinContent(1,1);
  }
  fHist[fNvar] = (TH1F*)h->Clone();
  fHist[fNvar]->SetDirectory(0);
  fHist2D[fNvar] = 0;
  fHist3D[fNvar] = 0;
  if(fNvar>0 && fAverage<=0){
    std::cout << "  MMEfficiency::INFO: No average efficiency set so far. Extracting it from current histogram... " << std::endl;
    if(!isDummy) SetAverageFromHist((TH1F*)fHist[fNvar]->Clone());
    else         fAverage = 1.;
  }
  fDim[fNvar] = 1;
  fNvar += 1;
  if(isDummy) delete h;
  return !isDummy;
}

bool MMEfficiency::AddHisto(TH2F *h){
  bool isDummy = false;
  if(h==0) isDummy = true;
  if(isDummy){
//     std::cout << "  MMEfficiency::INFO: Dummy Hstogram added." << std::endl;
    h = new TH2F("h_dummmy","h_dummy",1,0,1,1,0,1);
    h->SetBinContent(1,1);
  }
  fHist[fNvar] = 0;
  fHist2D[fNvar] = (TH2F*)h->Clone();
  fHist2D[fNvar]->SetDirectory(0);
  fHist3D[fNvar] = 0;
  if(fNvar>0 && fAverage<=0){
    std::cout << "  MMEfficiency::INFO: No average efficiency set so far. Extracting it from current histogram... " << std::endl;
    if(!isDummy) SetAverageFromHist((TH2F*)fHist2D[fNvar]->Clone());
    else         fAverage = 1.;
  }
  fDim[fNvar] = 2;
  fNvar += 2;
  if(isDummy) delete h;
  return !isDummy;
}

bool MMEfficiency::AddHisto(TH3F *h){
  bool isDummy = false;
  if(h==0) isDummy = true;
  if(isDummy){
//     std::cout << "  MMEfficiency::INFO: Dummy Hstogram added." << std::endl;
    h = new TH3F("h_dummmy","h_dummy",1,0,1,1,0,1,1,0,1);
    h->SetBinContent(1,1);
  }
  fHist[fNvar] = 0;
  fHist2D[fNvar] = 0;
  fHist3D[fNvar] = (TH3F*)h->Clone();
  fHist3D[fNvar]->SetDirectory(0);
  if(fNvar>0 && fAverage<=0){
    std::cout << "  MMEfficiency::INFO: No average efficiency set so far. Extracting it from current histogram... " << std::endl;
    if(!isDummy) SetAverageFromHist((TH3F*)fHist3D[fNvar]->Clone());
    else         fAverage = 1.;
  }
  fDim[fNvar] = 3;
  fNvar += 3;
  if(isDummy) delete h;
  return !isDummy;
}

bool MMEfficiency::AddHisto(TFile *f, std::string histoName, int ndim){
  f->cd();
  bool res = false;
  if     (ndim==1) res = AddHisto((TH1F*)f->Get(histoName.c_str()));
  else if(ndim==2) res = AddHisto((TH2F*)f->Get(histoName.c_str()));
  else if(ndim==3) res = AddHisto((TH3F*)f->Get(histoName.c_str()));
  else{
    std::cout << "  MMEfficiency::ERROR: ndim != 1,2,3" << std::endl;
    return false;
  }
  return res;
}

bool MMEfficiency::AddHisto(std::string fileName, std::string histoName, int ndim){
  bool res = false;
  TFile *f = TFile::Open(fileName.c_str());
  res = AddHisto(f,histoName,ndim);
  delete f;
  return res;
}

// ------------------------ SetAverage ------------------------ //

void MMEfficiency::SetAverage(float value){
  fAverage = value;
}

void MMEfficiency::SetAverageHist(std::string fileName, std::string histoName){
  TFile *f = TFile::Open(fileName.c_str());
  if(!f) return;
  TH1F *h = (TH1F*)f->Get(histoName.c_str());
  if(!h){
    delete f;
    return;
  }
  fAverage = h->GetBinContent(1);
//   cout << "Setting Average Efficiency = " << fAverage << endl;
  delete f;
}

void MMEfficiency::SetAverageHist(TFile *f, std::string histoName){
  TH1F *h = (TH1F*)f->Get(histoName.c_str());
  if(!h) return;
  fAverage = h->GetBinContent(1);
//   cout << "Setting Average Efficiency = " << fAverage << endl;
  delete h;
}

float MMEfficiency::SetAverageFromHist(TH1F *h){
  TF1 *p = new TF1("p","pol0",h->GetXaxis()->GetBinLowEdge(1),h->GetXaxis()->GetBinUpEdge(h->GetNbinsX()));
  h->Fit(p,"I0");
  fAverage = p->GetParameter(0);
  delete p;
  return fAverage;
}

float MMEfficiency::SetAverageFromHist(TH2F *h){
  TF2 *p = new TF2("p","[0]+0*x+0*y",
    h->GetXaxis()->GetBinLowEdge(1),h->GetXaxis()->GetBinUpEdge(h->GetNbinsX()),
    h->GetYaxis()->GetBinLowEdge(1),h->GetYaxis()->GetBinUpEdge(h->GetNbinsY())
  );
  h->Fit(p,"I0");
  fAverage = p->GetParameter(0);
  delete p;
  return fAverage;
}

float MMEfficiency::SetAverageFromHist(TH3F *h){
  TF3 *p = new TF3("p","[0]+0*x+0*y+0*z",
    h->GetXaxis()->GetBinLowEdge(1),h->GetXaxis()->GetBinUpEdge(h->GetNbinsX()),
    h->GetYaxis()->GetBinLowEdge(1),h->GetYaxis()->GetBinUpEdge(h->GetNbinsY()),
    h->GetZaxis()->GetBinLowEdge(1),h->GetZaxis()->GetBinUpEdge(h->GetNbinsZ())
  );
  h->Fit(p,"I0");
  fAverage = p->GetParameter(0);
  delete p;
  return fAverage;
}

float MMEfficiency::SetAverageFromHist(TFile *f, std::string histoName,int ndim){
  float av = -1;
  TH1F *h1;
  TH2F *h2;
  TH3F *h3;
  if(ndim==1){
    h1 = (TH1F*)f->Get(histoName.c_str());
    av = SetAverageFromHist(h1);
  }
  if(ndim==2){
    h2 = (TH2F*)f->Get(histoName.c_str());
    av = SetAverageFromHist(h2);
  }
  if(ndim==3){
    h3 = (TH3F*)f->Get(histoName.c_str());
    av = SetAverageFromHist(h3);
  }
  return av;
}

float MMEfficiency::SetAverageFromHist(std::string fileName, std::string histoName,int ndim){
  float av = -1;
  TFile *f = TFile::Open(fileName.c_str());
  av = SetAverageFromHist(f,histoName,ndim);
  delete f;
  return av;
}

// ------------------------ GetEfficiency ------------------------ //

float MMEfficiency::GetEfficiency(){
  float eff = 1;
  float eff1 = 0;
  float v0, v1, v2;
  float xmin, ymin, zmin;
  float xmax, ymax, zmax;
  for(int i=0;i<fNvar;i+=fDim[i]){
    // ensure that x,y,z are inside hist range
    if(fDim[i]==3){
      v0 = fVars[i];
      xmin = fHist3D[i]->GetXaxis()->GetXmin();
      xmax = fHist3D[i]->GetXaxis()->GetXmax();
      if(v0<xmin) v0 = xmin;
      if(v0>xmax) v0 = xmax-0.001;
      v1 = fVars[i+1];
      ymin = fHist3D[i]->GetYaxis()->GetXmin();
      ymax = fHist3D[i]->GetYaxis()->GetXmax();
      if(v1<ymin) v1 = ymin;
      if(v1>ymax) v1 = ymax-0.001;
      v2 = fVars[i+2];
      zmin = fHist3D[i]->GetZaxis()->GetXmin();
      zmax = fHist3D[i]->GetZaxis()->GetXmax();
      if(v2<zmin) v2 = zmin;
      if(v2>zmax) v2 = zmax-0.001;
      eff1 = fHist3D[i]->GetBinContent(fHist3D[i]->FindBin(v0,v1,v2));
    }
    if(fDim[i]==2){
      v0 = fVars[i];
      xmin = fHist2D[i]->GetXaxis()->GetXmin();
      xmax = fHist2D[i]->GetXaxis()->GetXmax();
      if(v0<xmin) v0 = xmin;
      if(v0>xmax) v0 = xmax-0.001;
      v1 = fVars[i+1];
      ymin = fHist2D[i]->GetYaxis()->GetXmin();
      ymax = fHist2D[i]->GetYaxis()->GetXmax();
      if(v1<ymin) v1 = ymin;
      if(v1>ymax) v1 = ymax-0.001;
      eff1 = fHist2D[i]->GetBinContent(fHist2D[i]->FindBin(v0,v1));
    }
    if(fDim[i]==1){
      v0 = fVars[i];
      xmin = fHist[i]->GetXaxis()->GetXmin();
      xmax = fHist[i]->GetXaxis()->GetXmax();
      if(v0<xmin) v0 = xmin;
      if(v0>xmax) v0 = xmax-0.001;
      // fix by Timothée Theveneaux-Pelzer
      if ( fHist[i]->GetXaxis()->FindBin(v0) < fHist[i]->FindFirstBinAbove(0) ) {
        int bin=fHist[i]->FindFirstBinAbove(0);
        eff1 = fHist[i]->GetBinContent(bin);
      }
      else if ( fHist[i]->GetXaxis()->FindBin(v0) > fHist[i]->FindLastBinAbove(0) ) {
        int bin=fHist[i]->FindLastBinAbove(0);
        eff1 = fHist[i]->GetBinContent(bin);
      }
      else eff1 = fHist[i]->GetBinContent(fHist[i]->FindBin(v0));
// cout << fHist[i]->GetTitle() << endl;
    }
    //
    if(eff1>0 && eff1<1) eff *= eff1;  // this is just to avoid crazy eff values going in
    else eff *= fAverage;              // ... and if the value is non-physica, just give the average eff
    //
    if(i>0) eff /= fAverage;           // divide by average if it's an additional parametrization
  }
  return eff;
}

float MMEfficiency::GetAverageEfficiency(){
  return fAverage;
}

float MMEfficiency::GetEfficiency(float v0,float v1,float v2,float v3,float v4,float v5,float v6,float v7,float v8,float v9){
  SetParVars(v0,v1,v2,v3,v4,v5,v6,v7,v8,v9);
  return GetEfficiency();
}

// ------------------------ SetParVar(s) ------------------------ //

void MMEfficiency::SetParVars(float v0,float v1,float v2,float v3,float v4,float v5,float v6,float v7,float v8,float v9){
  SetParVar(0,v0);
  if(fNvar>=2) SetParVar(1,v1);
  if(fNvar>=3) SetParVar(2,v2);
  if(fNvar>=4) SetParVar(3,v3);
  if(fNvar>=5) SetParVar(4,v4);
  if(fNvar>=6) SetParVar(5,v5);
  if(fNvar>=7) SetParVar(6,v6);
  if(fNvar>=8) SetParVar(7,v7);
  if(fNvar>=9) SetParVar(8,v8);
  if(fNvar>=10) SetParVar(9,v9);
}

void MMEfficiency::SetParVar(int idx,float value){
  fVars[idx] = value;
}

// ------------------------ Other ------------------------ //

int MMEfficiency::GetNvars(){
  return fNvar;
}
