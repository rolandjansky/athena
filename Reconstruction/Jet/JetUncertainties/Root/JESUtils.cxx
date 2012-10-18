/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Useful utility functions for use when testing JES-related code

#include "JetUncertainties/JESUtils.h"

#include <cstdlib>
#include "TMath.h"
#include "TObjArray.h"

namespace JESUtils{

// Vectorizing functions
StrV Vectorize(TString str, TString sep=" ")
{
  StrV result; TIter istr(str.Tokenize(sep));
  while (TObjString* os=(TObjString*)istr())
    result.push_back(os->GetString());
  return result;
}

VecD VectorizeD(TString str, TString sep=" ")
{
  VecD result; TIter istr(str.Tokenize(sep));
  while (TObjString* os=(TObjString*)istr())
    result.push_back(atof(os->GetString()));
  return result;
}


// Empty histogram creation functions
VecD GetUniformBins(int numBins, double lowBound, double highBound)
{
  VecD uniformBins;

  double dx = (highBound-lowBound)/numBins;
  for (int i = 0; i <= numBins; ++i)
    uniformBins.push_back(lowBound + i*dx);
  
  return uniformBins;
}

VecD GetLogBins(int numBins, double lowBound, double highBound)
{
  VecD logBins;

  double dx = (log(highBound) - log(lowBound))/numBins;
  for (int i = 0; i <= numBins; ++i)
    logBins.push_back(exp(log(lowBound)+i*dx));

  return logBins;
}

VecD GetDefaultPtBins()
{
  return GetLogBins(100,15,2500);
}

VecD GetDefaultEtaBins()
{
  int numEtaValues   = 14;
  double etaValues[] = {0,0.15,0.3,0.8,1.0,1.2,1.65,2.1,2.45,2.8,3.0,3.2,3.6,4.5};

  VecD etaValuesVec;
  for (int i = numEtaValues-1; i > 0; i--) // don't include zero
    etaValuesVec.push_back(-1*etaValues[i]);
  for (int i = 0; i < numEtaValues; i++) // include zero
    etaValuesVec.push_back(etaValues[i]);
  
  return etaValuesVec; 
}

TH2D* MakeEmptyPtEtaHisto(TString hname, VecD ptBins, VecD etaBins)
{
  return new TH2D(hname,"",ptBins.size()-1,&ptBins[0],etaBins.size()-1,&etaBins[0]);
}

TH2D* MakeEmptyPtEtaHisto(TString hname, VecD ptBins)
{
  return MakeEmptyPtEtaHisto(hname,ptBins,GetDefaultEtaBins());
}

TH2D* MakeEmptyPtEtaHisto(TString hname)
{
  return MakeEmptyPtEtaHisto(hname,GetDefaultPtBins(),GetDefaultEtaBins());
}


// Histogram interpolation functions
void CheckPtEtaRanges(TH2D* histo1, TH2D* histo2)
{
  // Ranges for histogram 1
  double minPt1  = histo1->GetXaxis()->GetBinLowEdge(1); // Low edge of first actual bin (not underflow)
  double maxPt1  = histo1->GetXaxis()->GetBinLowEdge(histo1->GetNbinsX()+1); // Low edge of overflow (upper edge of last actual bin)
  double minEta1 = histo1->GetYaxis()->GetBinLowEdge(1); 
  double maxEta1 = histo1->GetYaxis()->GetBinLowEdge(histo1->GetNbinsY()+1);

  // Ranges for histogram 2
  double minPt2  = histo2->GetXaxis()->GetBinLowEdge(1);
  double maxPt2  = histo2->GetXaxis()->GetBinLowEdge(histo2->GetNbinsX()+1);
  double minEta2 = histo2->GetYaxis()->GetBinLowEdge(1);
  double maxEta2 = histo2->GetYaxis()->GetBinLowEdge(histo2->GetNbinsY()+1);
  
  // Compare
  const double tolerance = 1e-5;
  if ( (fabs(minPt1 - minPt2) > tolerance) || (fabs(maxPt1 - maxPt2) > tolerance) )
    Fatal("CheckPtEtaRanges","Histograms differ in pt ranges. \"%s\" is [%f,%f], \"%s\" is [%f,%f]",
          histo1->GetName(),minPt1,maxPt1,histo2->GetName(),minPt2,maxPt2);
  if ( (fabs(minEta1 - minEta2) > tolerance) || (fabs(maxEta1 - maxEta2) > tolerance) )
    Fatal("CheckPtEtaRanges","Histograms differ in eta ranges. \"%s\" is [%f,%f], \"%s\" is [%f,%f]",
          histo1->GetName(),minEta1,maxEta1,histo2->GetName(),minEta2,maxEta2);
  
  // If reaches here, then they agree
  return;
}
TH2D* MakeInterpolatedPtEtaHisto(TH2D* oldHisto, TString newName, VecD newPtBins, VecD newEtaBins)
{
  // Make the histogram
  TH2D* newHisto = MakeEmptyPtEtaHisto(newName,newPtBins,newEtaBins);
  
  // Ensure the histograms cover the same phase space ranges
  CheckPtEtaRanges(oldHisto,newHisto);

  // Fill the histogram with interpolated values
  for (int ipt=1; ipt <= newHisto->GetNbinsX(); ++ipt)
  {
    for (int ieta=1; ieta <= newHisto->GetNbinsY(); ++ieta)
    {
      double pt  = newHisto->GetXaxis()->GetBinCenter(ipt);
      double eta = newHisto->GetYaxis()->GetBinCenter(ieta);
      newHisto->SetBinContent(ipt,ieta,oldHisto->Interpolate(pt,eta));
    }
  }
  
  // Done, return the new histogram
  return newHisto;
}
TH2D* MakeInterpolatedPtEtaHisto(TH2D* oldHisto, TString newName)
{
  return MakeInterpolatedPtEtaHisto(oldHisto,newName,GetDefaultPtBins(),GetDefaultEtaBins());
}

}
