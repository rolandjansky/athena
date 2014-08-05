/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaAnalysisUtils/CaloPointingCorrectionTool.h"

#include "TFile.h"
#include "TH1F.h"
#include "TSystem.h"
#include <iostream>
#include <math.h>

using namespace std;

CaloPointingCorrectionTool::CaloPointingCorrectionTool(TString dataFile, TString mcFile,
  TString dataHistoName, TString mcHistoName) :
  m_hData(0), m_hMC(0)
{
  gSystem->ExpandPathName(dataFile);
  m_hData = getHisto(dataFile, dataHistoName);
  
  gSystem->ExpandPathName(mcFile);
  m_hMC = getHisto(mcFile, mcHistoName);
}

CaloPointingCorrectionTool::~CaloPointingCorrectionTool()
{
  delete m_hData;
  delete m_hMC;
}


float CaloPointingCorrectionTool::getCaloPointingCorrected(float zPointing, float etas2, bool isData)
{
  TH1* h = (isData ? m_hData : m_hMC);
  if (!h || fabs(etas2) < 1.37)
    return zPointing;
  return zPointing - h->Interpolate( etas2 );
}

std::vector<float> CaloPointingCorrectionTool::getCaloPointingCorrected(
  std::vector<float>& zPointing,
  std::vector<float>& etas2, 
  bool isData)
{
  std::vector<float> v;
  for (unsigned int i = 0; i < zPointing.size(); ++i)
    v.push_back( getCaloPointingCorrected(zPointing[i], etas2[i], isData) );
  return v;
}


TH1F* CaloPointingCorrectionTool::getHisto(TString fileName, TString histoName)
{
  TFile *f = TFile::Open(fileName);
  if (!f)
  {
    cout << "Invalid file: " << fileName << endl;
    return 0;
  }
  TH1F *h = dynamic_cast<TH1F*>( f->Get(histoName) );
  if (!h)
  {
    cout << "Invalid histogram name: " << fileName << endl;
    return 0;
  }
  h->SetDirectory(0);
  return h;
}
