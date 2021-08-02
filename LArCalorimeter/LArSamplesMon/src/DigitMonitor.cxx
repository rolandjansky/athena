/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArSamplesMon/DigitMonitor.h"

#include "LArSamplesMon/Interface.h"
#include "LArSamplesMon/History.h"
#include "LArSamplesMon/Data.h"
#include "LArSamplesMon/OFC.h"
#include "LArSamplesMon/UniformShapeErrorGetter.h"
#include "LArSamplesMon/CombinedShapeErrorGetter.h"
#include "LArSamplesMon/TreeShapeErrorGetter.h"
#include "LArSamplesMon/ScaledErrorData.h"
#include "LArCafJobs/SimpleShape.h"
#include "LArSamplesMon/Residual.h"
#include "LArCafJobs/Geometry.h"

#include "TH1I.h"
#include "TH2D.h"
#include "TF1.h"
#include "TPaveText.h"
#include "TLatex.h"
#include "TMath.h"
#include "TVectorD.h"
#include "TROOT.h"
#include "TCanvas.h"
#include <map>
#include <vector>

#include <iostream>
using std::cout;
using std::endl;

using namespace LArSamples;


TH1D* DigitMonitor::energyDist(TString name, int nBins, double eMax) const
{return dist(&Data::_energy, DataFuncArgs(), name, nBins, 0, eMax, "Energy distribution", "Energy (MeV)"); }

TH1D* DigitMonitor::timeDist(TString name, int nBins, double tMin, double tMax) const
{ return dist(&Data::_ofcTime, DataFuncArgs(), name, nBins, tMin, tMax, "Time distribution", "Time (ns)"); }

TH1D* DigitMonitor::adcMaxDist(TString name, int nBins, double aMin, double aMax) const
{ return dist(&Data::_adcMax, DataFuncArgs(), name, nBins, aMin, aMax, "ADCMax distribution", "ADCMax (counts)"); }

TH1D* DigitMonitor::noiseDist(TString name, int nBins, double max) const
{ return dist(&Data::_noise, DataFuncArgs(), name, nBins, 0, max, "Noise distribution", "Noise (ADC counts)"); }

TH2D* DigitMonitor::maxValueMap(TString name, PartitionId partition) const
{ return partitionMap(&Data::_maxValue, DataFuncArgs(), name, partition, "Max sample value (ADC counts)", MaxValue); }

TH2D* DigitMonitor::minValueMap(TString name, PartitionId partition) const
{ return partitionMap(&Data::_minValue, DataFuncArgs(), name, partition, "Min sample value (ADC counts)", MinValue); }


TH1D* DigitMonitor::chi2Dist(TString name, int nBins, double max, double kFactor,
                             double fitMax, int lwb, int upb, unsigned int chi2Pars, 
                             ShapeErrorType shapeErrorType, unsigned int nDof) const
{
  TH1D* h = new TH1D(name, "#chi^{2} distribution", nBins, 0, max);
  unsigned int nDofEff = 0, nDofLast = 0;
  UniformShapeErrorGetter* kFactorGetter = 0;
  CombinedShapeErrorGetter* shapeErrorGetter = 0;
  if (kFactor > 0) {
    kFactorGetter = new UniformShapeErrorGetter(kFactor);
    shapeErrorGetter = new CombinedShapeErrorGetter();
    if (interface().shapeErrorGetter()) shapeErrorGetter->add(*interface().shapeErrorGetter());
    shapeErrorGetter->add(*kFactorGetter);
  }
  for (unsigned int i = 0; i < nChannels(); i++) {
    if ((i+1) % 10000 == 0) cout << "Processing entry # " << i+1 << endl;
    const History* history = cellHistory(i);
    if (!history) continue;
    if (shapeErrorGetter) history->setShapeErrorGetter(shapeErrorGetter); // Do it at history level so it doesn't "stick" afterwards,,,
    for (unsigned int j = 0; j < history->nData(); j++) {
      h->Fill(history->chi2(j, lwb, upb, chi2Pars, shapeErrorType, &nDofEff));
      if (nDofLast > 0 && nDofLast != nDofEff) cout << "WARNING @ hash = " << i << ", index = " << j << " : nDof varied from " << nDofLast << " to " << nDofEff << endl;
      nDofLast = nDofEff;
    }
  }
  
  if (nDof > 0 && nDof != nDofEff) cout << "WARNING : fixing nDof = " << nDof << ", but computer nDofEff = " << nDofEff << endl;
  h->Draw();
  if (fitMax < 0) return h;
  TF1* fChi2 = fitChi2(*h, "chi2", 0, max, nDof, 0, fitMax);  

  TPaveText* p = new TPaveText(0.6, 0.85, 0.85, 0.65, "NDC");
  p->AddText(Form("k = %.1f%%", kFactor*100));
  p->AddText(Form("Fit #chi^{2} = %.1f", fChi2->GetChisquare()));
  p->AddText(Form("Fit nDOF = %.1f", fChi2->GetParameter(1)));
  p->Draw();
  
  if (shapeErrorGetter) {
    delete kFactorGetter;
    delete shapeErrorGetter;
  }
  return h;
}


TF1* DigitMonitor::chi2Func(const char* name, double xMin, double xMax)
{
  TF1* fChi2 = new TF1(name, "[0]*TMath::Power(x/2, [1]/2)/x*TMath::Exp(-x/2)/TMath::Gamma([1]/2)", xMin, xMax);
  fChi2->SetParameter(0, 100);
  fChi2->SetParameter(1, 3);
  fChi2->SetParLimits(1, 1, 7);
  return fChi2;
}


TF1* DigitMonitor::fitChi2(TH1D& h, const char* name, double xMin, double xMax, 
				       double nDof, double fitMin, double fitMax)
{
  if (fitMin == Definitions::none) fitMin = xMin;
  if (fitMax == Definitions::none) fitMin = xMax;

  TF1* fChi2 = chi2Func(name, xMin, xMax);
  fChi2->SetParameter(0, 4*h.GetMaximum());
  if (nDof > 0) fChi2->FixParameter(1, nDof);
  
  h.Fit(fChi2, "", "", fitMin, fitMax);
  fChi2->SetRange(xMin, xMax); 
  return fChi2;
}


TH1D* DigitMonitor::bestChi2Dist(TString name, int nBins, double max, 
                                             double refErrMin, double refErrMax, unsigned int refErrNBins,
                                             int lwb, int upb, unsigned int chi2Pars, unsigned int nDof) const
{
  TF1* fChi2 = chi2Func("fChi2", 0, max);
  if (nDof > 0) fChi2->FixParameter(1, nDof);

  double delta = (refErrMax - refErrMin)/refErrNBins;
  TH1D* fitResults = new TH1D("fitResults", "", refErrNBins, refErrMin - delta/2, refErrMax - delta/2);
  
  for (unsigned int k = 0; k < refErrNBins; k++) {
    double refErr = refErrMin + delta*k;
    TH1D* h = new TH1D(name, "#chi^{2} distribution", nBins, 0, max);
    UniformShapeErrorGetter kFactorGetter(refErr);
    CombinedShapeErrorGetter shapeErrorGetter;
    if (interface().shapeErrorGetter()) shapeErrorGetter.add(*interface().shapeErrorGetter());
    shapeErrorGetter.add(kFactorGetter);
    for (unsigned int i = 0; i < nChannels(); i++) {
      const History* history = cellHistory(i);
      if (!history) continue;
      history->setShapeErrorGetter(&shapeErrorGetter);
      for (unsigned int j = 0; j < history->nData(); j++) {
        h->Fill(history->chi2(j, lwb, upb, chi2Pars));
      }
    }
    h->Fit(fChi2); 
    fitResults->SetBinContent(k+1, fChi2->GetChisquare());
    delete h;
  }
      
  TF1* fPol2 = new TF1("fPol2", "[0]*(x - [1])*(x - [1]) + [2]", refErrMin, refErrMax);
  
  double center = fitResults->GetBinCenter(fitResults->GetMinimumBin());
  fPol2->SetParameter(0, (fitResults->GetMaximum() - fitResults->GetMinimum())/center/center);
  fPol2->SetParameter(1, center);
  fPol2->SetParameter(2, fitResults->GetMinimum());
  
  fitResults->Draw();
  fitResults->Fit(fPol2);
  
  TPaveText* p = new TPaveText(0.6, 0.85, 0.85, 0.65, "NDC");
  p->AddText(Form("Fit k_{best} = %.1f%%", fPol2->GetParameter(1)*100));
  p->AddText(Form("Fit #delta k_{best} = %.2f%%", 1/TMath::Sqrt(fPol2->GetParameter(0))*100));
  p->AddText(Form("Fit #chi^{2}_{min} = %.2f", fPol2->GetParameter(2)));
  p->Draw();
  
  return fitResults;
}



TH1D* DigitMonitor::gainDist(TString name) const
{
  TH1D* h = new TH1D(name, "gain distribution", 3, -0.5, 2.5);
  h->GetXaxis()->SetBinLabel(1, "Low");
  h->GetXaxis()->SetBinLabel(2, "Medium");
  h->GetXaxis()->SetBinLabel(3, "High");
  for (unsigned int i = 0; i < nChannels(); i++) {
    const History* history = cellHistory(i);
    if (!history) continue;
    for (unsigned int j = 0; j < history->nData(); j++) h->Fill(history->data(j)->gain());
  }
  return h;
}


TH1D* DigitMonitor::layerDist(TString name) const
{
  TH1D* h = new TH1D(name, "layer distribution", 4, -0.5, 3.5);
  h->GetXaxis()->SetBinLabel(1, "PS");
  h->GetXaxis()->SetBinLabel(2, "Strips");
  h->GetXaxis()->SetBinLabel(3, "Middle");
  h->GetXaxis()->SetBinLabel(4, "Back");
  for (unsigned int i = 0; i < nChannels(); i++) {
    const History* history = cellHistory(i);
    if (!history) continue;
    for (unsigned int j = 0; j < history->nData(); j++) h->Fill(history->cellInfo()->layer());
  }
  return h;
}


TH1D* DigitMonitor::residualDist(unsigned int k, TString name, int nBins, double rMin, double rMax, bool norm) const
{
  TH1D* h = new TH1D(name, "Residual distribution", nBins, rMin, rMax);
  h->GetXaxis()->SetTitle(norm ? "Residual/ADCMax (%)" : "Residual (ADC counts)");
    
  for (unsigned int i = 0; i < nChannels(); i++) {
    const History* history = cellHistory(i);
    if (!history) continue;
    for (unsigned int j = 0; j < history->nData(); j++) 
      h->Fill(history->data(j)->delta(k)*(norm ? 100/history->data(j)->adcMax() : 1));
  }
  return h;
}


bool DigitMonitor::residualPlots(CaloId calo, unsigned int layer, CaloGain::CaloGain gain, bool xip, bool ring, 
                                 double lo, double hi, const TString& fileName)
{
  TH1D* h1[5];
  TH2D* hEtaPhi[5];
  TString globalTitle =  TString(xip ? "Derivative" : "Offset") + " correction";
  for (unsigned short k = 0; k < 5; k++) {
    TString title =  globalTitle + " for sampling " + Form("%d", k);
    h1[k] = new TH1D(Form("h%d", k), title, 100, lo, hi);
    h1[k]->GetXaxis()->SetTitle("correction");
    hEtaPhi[k] = Geo::etaPhiHist(calo, layer, Form("hEtaPhi%d", k), title);
    hEtaPhi[k]->SetMinimum(lo);
    hEtaPhi[k]->SetMaximum(hi);
  }
  FilterParams f;
  f.addCalo(calo);
  f.addLayer(layer);
  TH2D* hAll = new TH2D("hAll", globalTitle + " vs. sampling", 5, 0, 5, 100, lo, hi);
  hAll->GetXaxis()->SetTitle("sampling");
  hAll->GetYaxis()->SetTitle("correction");    
  for (unsigned int i = 0; i < nChannels(); i++) {
    if ((i+1) % 10000 == 0) cout << "Cell # " << i+1 << "/" << nChannels() << endl;
    const History* history = pass(i, f);
    if (!history || history->nData() == 0) continue;
    for (unsigned short k = 0; k < 5; k++) {
      ShapeErrorType type = (ring ? RingShapeError : CellShapeError);
      double xi = history->data(0)->xi(k, type, gain, xip);
      h1[k]->Fill(xi);
      hEtaPhi[k]->Fill(history->cellInfo()->eta(), history->cellInfo()->phi(), xi);
      if (Id::matchCalo(calo, HEC) && history->cellInfo()->region() == 1) // For HEC, take care of larger phi size in region 1
        hEtaPhi[k]->Fill(history->cellInfo()->eta(), history->cellInfo()->phi() - Geo::phiSize(HEC_A, 1, 1), xi);      
    }
  }
  for (unsigned int i = 1; i <= 100; i++) 
    for (unsigned short k = 0; k < 5; k++) 
      hAll->SetBinContent(k+1, i, h1[k]->GetBinContent(i));
  TFile* ff = TFile::Open(fileName + ".root", "RECREATE");
  for (unsigned short k = 0; k < 5; k++) {
    h1[k]->Write();
    hEtaPhi[k]->Write();
  }
  hAll->Write();
  ff->Close();
  TCanvas* c1 = new TCanvas("c1", Id::str(calo) + " layer " + Form("%d", layer), 1600, 2000);
  c1->Divide(2,3);
  for (unsigned short k = 0; k < 5; k++) {
    c1->cd(k+1); 
    hEtaPhi[k]->Draw("COLZ");
  }
  c1->cd(6);
  hAll->Draw();
  c1->Print(fileName + ".png");
  c1->Print(fileName + ".eps");
  return true;
}


bool DigitMonitor::residualPlotsRingComp(CaloId calo, unsigned int layer, CaloGain::CaloGain gain, bool xip,
                                  double lo, double hi, const TString& fileName)
{
  unsigned int nBins = 100;
  TH1D* h[5][5];
  TString globalTitle =  TString(xip ? "Derivative" : "Offset") + " correction";
  for (unsigned short l = 0; l < 5; l++)
    for (unsigned short k = 0; k < 5; k++)
      h[l][k] = new TH1D(Form("h%d%d", l, k), "", nBins, l < 2 ? -15 : lo, l < 2 ? 15 : hi);
  FilterParams f;
  f.addCalo(calo);
  f.addLayer(layer);
  TH2D* hAll[5];
  hAll[0] = new TH2D("h0All", "#xi_{cell}/#delta#xi vs. sampling", 5, 0, 5, nBins, -15, 15);
  hAll[1] = new TH2D("hDAll", "(#xi_{cell} - #xi_{ring})/#delta#xi vs. sampling", 5, 0, 5, nBins, -15, 15);
  hAll[2] = new TH2D("hEAll", globalTitle + " per-cell value vs. sampling", 5, 0, 5, nBins, 0, hi);
  hAll[3] = new TH2D("hRAll", globalTitle + " per-ring value vs. sampling", 5, 0, 5, nBins, lo, hi);
  hAll[4] = new TH2D("hCAll", globalTitle + " per-cell error vs. sampling", 5, 0, 5, nBins, lo, hi);
  TH2D* hCor = new TH2D("hCor", globalTitle + " per-ring vs. per-cell values", nBins, lo, hi, nBins, lo, hi);
  hCor->GetXaxis()->SetTitle("#xi_{cell}");
  hCor->GetYaxis()->SetTitle("#xi_{ring}");    
  for (unsigned short l = 0; l < 4; l++) {
    hAll[l]->GetXaxis()->SetTitle("sampling");
    hAll[l]->GetYaxis()->SetTitle("significance");    
  }
  for (unsigned int i = 0; i < nChannels(); i++) {
    if ((i+1) % 10000 == 0) cout << "Cell # " << i+1 << "/" << nChannels() << endl;
    const History* history = pass(i, f);
    if (!history || history->nData() == 0) continue;
    for (unsigned short k = 0; k < 5; k++) {
      const ShapeErrorData* cellSED = history->shapeErrorData(gain, CellShapeError);
      const ShapeErrorData* ringSED = history->shapeErrorData(gain, RingShapeError);
      if (!cellSED || !ringSED) {
        if (cellSED) delete cellSED;
        if (ringSED) delete ringSED;
        continue;
      }
      double cellVal = (xip ? cellSED->xi()(k) : cellSED->xip()(k));
      double ringVal = (xip ? ringSED->xi()(k) : ringSED->xip()(k));
      double cellErr = TMath::Sqrt(xip ? cellSED->xiErr()(k,k) : cellSED->xipErr()(k,k));
      delete cellSED;
      delete ringSED;
      h[0][k]->Fill(cellVal/cellErr);
      h[1][k]->Fill((cellVal - ringVal)/cellErr);
      h[2][k]->Fill(cellVal);
      h[3][k]->Fill(ringVal);
      h[4][k]->Fill(cellErr);
      hCor->Fill(cellVal, ringVal);
    }
  }
  for (unsigned int i = 1; i <= nBins; i++) 
    for (unsigned short l = 0; l < 5; l++)
      for (unsigned short k = 0; k < 5; k++)
        hAll[l]->SetBinContent(k+1, i, h[l][k]->GetBinContent(i));
  TFile* ff = TFile::Open(fileName + ".root", "RECREATE");
  for (unsigned short l = 0; l < 5; l++) {
    for (unsigned short k = 0; k < 5; k++)
      h[l][k]->Write();
    hAll[l]->Write();
  }
  ff->Close();
  TCanvas* c1 = new TCanvas("c1", Id::str(calo) + " layer " + Form("%d", layer), 1600, 500);
  c1->Divide(3,1);
  c1->cd(1); hCor->Draw();
  TLatex l; l.SetNDC();
  TString corrStr = Form("Correlation : %.1f%%", 100*hCor->GetCorrelationFactor());
  l.DrawLatex(0.2, 0.8, corrStr);
  c1->cd(2); hAll[0]->Draw("COL");
  c1->cd(3); hAll[1]->Draw("COL");
  c1->Print(fileName + ".png");
  c1->Print(fileName + ".eps");
  return true;
}


bool DigitMonitor::residualPlotsGainComp(CaloId calo, unsigned int layer, bool ring, bool xip,
                                         double lo, double hi, const TString& fileName)
{
  unsigned int nBins = 100;
  TH1D* h[5][5];
  TString globalTitle =  TString(xip ? "Derivative" : "Offset") + " correction";
  for (unsigned short l = 0; l < 5; l++)
    for (unsigned short k = 0; k < 5; k++)
      h[l][k] = new TH1D(Form("h%d%d", l, k), "", nBins, l < 2 ? -15 : lo, l < 2 ? 15 : hi);
  FilterParams f;
  f.addCalo(calo);
  f.addLayer(layer);
  TH2D* hAll[5];
  hAll[0] = new TH2D("h0All", "#xi_{high}/#delta#xi vs. sampling", 5, 0, 5, nBins, -15, 15);
  hAll[1] = new TH2D("hDAll", "(#xi_{high} - #xi_{low})/#delta#xi vs. sampling", 5, 0, 5, nBins, -15, 15);
  hAll[2] = new TH2D("hEAll", globalTitle + " higher-gain value vs. sampling", 5, 0, 5, nBins, 0, hi);
  hAll[3] = new TH2D("hRAll", globalTitle + " lower-gain value vs. sampling", 5, 0, 5, nBins, lo, hi);
  hAll[4] = new TH2D("hCAll", globalTitle + " higher-gain error vs. sampling", 5, 0, 5, nBins, lo, hi);
  TH2D* hCor = new TH2D("hCor", globalTitle + " lower-gain vs. higher-gain values", nBins, lo, hi, nBins, lo, hi);
  hCor->GetXaxis()->SetTitle("#xi_{high}");
  hCor->GetYaxis()->SetTitle("#xi_{low}");    
  for (unsigned short l = 0; l < 4; l++) {
    hAll[l]->GetXaxis()->SetTitle("sampling");
    hAll[l]->GetYaxis()->SetTitle("significance");    
  }
  
  CaloGain::CaloGain hiGain = (Id::matchCalo(calo, HEC) || Id::matchCalo(calo, FCAL) ? CaloGain::LARMEDIUMGAIN : CaloGain::LARHIGHGAIN);
  CaloGain::CaloGain loGain = (Id::matchCalo(calo, HEC) || Id::matchCalo(calo, FCAL) ? CaloGain::LARLOWGAIN : CaloGain::LARMEDIUMGAIN);

  for (unsigned int i = 0; i < nChannels(); i++) {
    if ((i+1) % 10000 == 0) cout << "Cell # " << i+1 << "/" << nChannels() << endl;
    const History* history = pass(i, f);
    if (!history || history->nData() == 0) continue;
    for (unsigned short k = 0; k < 5; k++) {
      const ShapeErrorData* hgSED = history->shapeErrorData(hiGain, ring ? RingShapeError : CellShapeError);
      const ShapeErrorData* lgSED = history->shapeErrorData(loGain, ring ? RingShapeError : CellShapeError);
      if (!hgSED || !lgSED) {
        if (hgSED) delete hgSED;
        if (lgSED) delete lgSED;
        continue;
      }
      double hgVal = (xip ? hgSED->xi()(k) : hgSED->xip()(k));
      double lgVal = (xip ? lgSED->xi()(k) : lgSED->xip()(k));
      double hgErr = TMath::Sqrt(xip ? hgSED->xiErr()(k,k) : hgSED->xipErr()(k,k));
      double error = TMath::Sqrt(xip ? hgSED->xiErr()(k,k) + lgSED->xiErr()(k,k) : hgSED->xipErr()(k,k) + lgSED->xipErr()(k,k));
      delete hgSED;
      delete lgSED;
      h[0][k]->Fill(hgVal/hgErr);
      h[1][k]->Fill((hgVal - lgVal)/error);
      h[2][k]->Fill(hgVal);
      h[3][k]->Fill(lgVal);
      h[4][k]->Fill(hgErr);
      hCor->Fill(hgVal, lgVal);
    }
  }
  for (unsigned int i = 1; i <= nBins; i++) 
    for (unsigned short l = 0; l < 5; l++)
      for (unsigned short k = 0; k < 5; k++)
        hAll[l]->SetBinContent(k+1, i, h[l][k]->GetBinContent(i));
  TFile* ff = TFile::Open(fileName + ".root", "RECREATE");
  for (unsigned short l = 0; l < 5; l++) {
    for (unsigned short k = 0; k < 5; k++)
      h[l][k]->Write();
    hAll[l]->Write();
  }
  ff->Close();
  TCanvas* c1 = new TCanvas("c1", Id::str(calo) + " layer " + Form("%d", layer), 1600, 500);
  c1->Divide(3,1);
  c1->cd(1); hCor->Draw();
  TLatex l; l.SetNDC();
  TString corrStr = Form("Correlation : %.1f%%", 100*hCor->GetCorrelationFactor());
  l.DrawLatex(0.2, 0.8, corrStr);
  c1->cd(2); hAll[0]->Draw("COL");
  c1->cd(3); hAll[1]->Draw("COL");
  c1->Print(fileName + ".png");
  c1->Print(fileName + ".eps");
  return true;
}


TH1D* DigitMonitor::shapeErrorDist(unsigned int k, TString name, int nBins, double rMin, double rMax, double mean) const
{
  TH1D* h = new TH1D(name, "Shape error", nBins, rMin, rMax);
  h->GetXaxis()->SetTitle("#epsilon^{2}");
  
  for (unsigned int i = 0; i < nChannels(); i++) {
    const History* history = cellHistory(i);
    if (!history) continue;
    for (unsigned int j = 0; j < history->nData(); j++) { 
      OFC* ofc = history->ofc(j);
      double delta = history->data(j)->delta(k) - mean;
      double adcMax = history->data(j)->adcMax();
      h->Fill((delta*delta - ofc->Gamma()(k, k))/(adcMax*adcMax));
    }
  }
  cout << sqrt(h->GetMean())*100 << " +/- " << sqrt(h->GetMeanError())*100 << endl;
  return h;
}


double DigitMonitor::residualCorr(unsigned int k1, unsigned int k2) const
{
  double sum = 0;
  int n = 0;
  
  for (unsigned int i = 0; i < nChannels(); i++) {
    const History* history = cellHistory(i);
    if (!history) continue;
    for (unsigned int j = 0; j < history->nData(); j++) {
      double residual1 = history->data(j)->delta(k1);
      double residual2 = history->data(j)->delta(k2);
      sum += residual1*residual2;
      n++;
    }  
  }
  
  return sum/n;
}


bool DigitMonitor::residualParams(int lwb, int upb, CovMatrix& k, TVectorD& means) const
{
  CovMatrix sum2N(lwb, upb), sumCN(lwb, upb);
  TVectorD sum(lwb, upb), sumN(lwb, upb);
  double sumA = 0, n = 0;
  for (unsigned int i = 0; i < nChannels(); i++) {
    const History* history = cellHistory(i);
    if (!history) continue;
    for (unsigned int j = 0; j < history->nData(); j++) {
      if (history->data(j)->adcMax() < 0) continue;
      OFC* ofc = history->ofc(j);
      if (!ofc || ofc->hasSameRange(lwb, upb)) { cout << "Invalid index bounds!" << endl; return false; }
      for (int i1 = lwb; i1 <= upb; i1++) {
        sum(i1)   = sum(i1)  + history->data(j)->delta(i1);
        sumN(i1)  = sumN(i1) + history->data(j)->delta(i1)/TMath::Power(history->data(j)->adcMax(), 2);
        for (int i2 = lwb; i2 <= upb; i2++) {
          sumCN(i1, i2) = sumCN(i1, i2) + ofc->Gamma()(i1, i2)/TMath::Power(history->data(j)->adcMax(), 2);
          sum2N(i1, i2) = sum2N(i1, i2) + history->data(j)->delta(i1)*history->data(j)->delta(i2)/TMath::Power(history->data(j)->adcMax(),2);
        }
      }
      sumA +=  1/TMath::Power(history->data(j)->adcMax(), 2);
      n++;
    }
  }

  sumA /= n;
  k.ResizeTo(lwb, upb);
  means.ResizeTo(lwb, upb);
  
  for (int i1 = lwb; i1 <= upb; i1++) {
    sum(i1)   = sum(i1)/n;    
    sumN(i1)  = sumN(i1)/n;
    means(i1) = sum(i1);
  }

  for (int i1 = lwb; i1 <= upb; i1++) {
    for (int i2 = lwb; i2 <= upb; i2++) {
      sumCN(i1, i2) = sumCN(i1, i2)/n;
      sum2N(i1, i2) = sum2N(i1, i2)/n;
      k(i1, i2) = sum2N(i1, i2) - sumN(i1)*sum(i2) - sum(i1)*sumN(i2)
                + sum(i1)*sum(i2)*sumA - sumCN(i1,i2);
      k(i1, i2) = (k(i1, i2) >= 0 ? sqrt(k(i1, i2)) : -sqrt(-k(i1, i2)));
    }
  }
  return true;
}


int DigitMonitor::combine(SimpleShape*& shape, SimpleShape*& ref, TString selection, bool timeAligned) const
{
  FilterParams f;
  if (!f.set(selection)) return 0;
  int n = 0;
  shape = ref = 0;
  
  double maxSum = 0;
  
  for (unsigned int i = 0; i < nChannels(); i++) {  
    if ((i+1) % 50000 == 0) cout << "Cell # " << i+1 << "/" << nChannels() << endl;
    const History* history = pass(i, f);
    if (!history) continue;
    for (unsigned int j = 0; j < history->nData(); j++) {
      if (!f.passEvent(*history->data(j))) continue;
      maxSum += history->data(j)->maxValue();
      cout << "Adding pulse (" << n+1 << ") at hash " << i << ", index " << j << ", max = " << maxSum/(n+1) << endl;
      //cout << i << " " << j << endl;
      SimpleShape* thisData = new SimpleShape(*history->data(j));
      SimpleShape* thisRef =  history->referenceShape(j);
      if (timeAligned) {
        if (!SimpleShape::scaleAndShift(thisData, 1, -history->data(j)->ofcTime())) return -1;
        if (!SimpleShape::scaleAndShift(thisRef,  1, -history->data(j)->ofcTime())) return -1;
      }
      if (!SimpleShape::add(shape, thisData)) return -1;
      delete thisData;
      if (!SimpleShape::add(ref, thisRef)) return -1;      
      delete thisRef;
      n++;
    }
  }
  if (!SimpleShape::scaleAndShift(shape, 1.0/n)) return -1;
  if (!SimpleShape::scaleAndShift(ref,   1.0/n)) return -1;
  return n;
}

Residuals* DigitMonitor::getResiduals(unsigned int hash, CaloGain::CaloGain gain, double absResTrunc, bool adjust, bool zeroTime) const
{
  const History* history = cellHistory(hash);
  if (!history) return 0;
  if (adjust) history = history->adjust();
  Residuals* residuals = history->residuals(gain, absResTrunc, false, zeroTime);
  if (adjust) delete history;
  return residuals;
}


bool DigitMonitor::makeResidualCorrections(TString outputFile, short resTrunc, short timeTrunc, double absResTrunc,
                                           unsigned int minSize, bool weigh, bool adjust, bool zeroTime) const
{
  TreeShapeErrorGetter* shapeError = new TreeShapeErrorGetter(outputFile, true);
  unsigned int nProcessed = 0;
  std::vector< std::vector<ResidualCalculator> > ringCalcs;
  std::vector<ResidualCalculator> gainRingCalcs(Geo::nPhiRings());
  for (unsigned int g = 0; g < CaloGain::LARNGAIN; g++) ringCalcs.push_back(gainRingCalcs);
  
  cout << "Processing cells" << endl;
  
  for (unsigned int i = 0; i < nChannels(); i++) {
    if ((i+1) % 10000 == 0) cout << "Processing hash = " << (i+1) << endl;
    for (unsigned int g = 0; g < CaloGain::LARNGAIN; g++) {
      Residuals* residuals = getResiduals(i, (CaloGain::CaloGain)g, absResTrunc, adjust, zeroTime);
      if (residuals && (resTrunc > 0 || timeTrunc > 0)) {
        Residuals* truncated = residuals->truncate(resTrunc, timeTrunc);
        delete residuals;
        residuals = truncated;
      }

      if (residuals && residuals->size() < minSize) {
        delete residuals;
        residuals = 0;
      }

      ResidualCalculator* resCalc = 0;
      if (residuals) {
        resCalc = residuals->calculator(weigh);
        delete residuals;
      }     

      if (!resCalc) {
        shapeError->addCell(ResidualCalculator(), (CaloGain::CaloGain)g);
        continue;
      }

      nProcessed++;
      cout << i << " : Phi ring " << cellHistory(i)->cellInfo()->globalPhiRing() << " : adding " << resCalc->size() << endl;
      shapeError->addCell(*resCalc, (CaloGain::CaloGain)g);
      ringCalcs[g][cellHistory(i)->cellInfo()->globalPhiRing()].append(*resCalc);
      if (cellHistory(i)->cellInfo()->globalPhiRing() == 1142) cout << ringCalcs[g][cellHistory(i)->cellInfo()->globalPhiRing()].regresser()->mean(0) << endl;
      delete resCalc;
    }
  }
  
  cout << "Processing phi-symmetric data" << endl;
  
  for (unsigned int g = 0; g < CaloGain::LARNGAIN; g++) {
    for (short i = 0; i < Geo::nPhiRings(); i++) {
      if (ringCalcs[g][i].size() > 0) 
        cout << "Saving data for gain " << g << ", phi ring " << i << ", n = " << ringCalcs[g][i].size() << ", range = " << ringCalcs[g][i].rangeStr() << endl;
      shapeError->addRing(ringCalcs[g][i], (CaloGain::CaloGain)g);
    }
  }
  
  cout << "Done!" << endl;
  
  delete shapeError;
  return true;
}
