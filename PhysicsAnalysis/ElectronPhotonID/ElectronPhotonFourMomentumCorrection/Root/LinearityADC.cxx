/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "ElectronPhotonFourMomentumCorrection/LinearityADC.h"

#include "TFile.h"

LinearityADC::LinearityADC(const std::string& filename, const std::string& name) :
  asg::AsgMessaging(name) 
{
  ATH_MSG_INFO("Initialize LinearityADC with file " << filename);
  std::unique_ptr<TFile> adcFile(TFile::Open(filename.c_str(), "READ"));
  static const std::map<TString,PATCore::ParticleType::Type> pN = {
    { "elec", PATCore::ParticleType::Electron },
    { "unco", PATCore::ParticleType::UnconvertedPhoton },
    { "conv", PATCore::ParticleType::ConvertedPhoton } };
  for (auto it : pN) {
    m_hcorr[it.second].reset(dynamic_cast<TProfile2D*>(adcFile->Get(Form("linearity_%s",it.first.Data()))));
    m_hcorr[it.second]->SetDirectory(nullptr);
  }
  adcFile->Close();
}

//===============================================================================
float LinearityADC::getCorr(float etaCalo, float et, PATCore::ParticleType::Type pType) const
{
  float corr=0.;
  float etGeV = et/1000.;
  if (etGeV<6.) etGeV=6.;
  if (etGeV>2999.) etGeV=2999.;

  auto it = m_hcorr.find(pType);
  if (it == m_hcorr.end()) {
    ATH_MSG_ERROR("unknow particle type " << pType);
    return 1.;
  }
  TProfile2D* hh = it->second.get();
 
  float aeta = std::abs(etaCalo);
  if (aeta>2.49) aeta=2.49;
  
  int ix = hh->GetXaxis()->FindBin(aeta);
  int iy = hh->GetYaxis()->FindBin(etGeV);
  float y = hh->GetYaxis()->GetBinCenter(iy);
  int iy0,iy1;
  if (y>etGeV) {
    iy0 = iy-1;
    iy1 = iy;
  }
  else {
    iy0=iy;
    iy1=iy+1;
  }
  
  int iyc = hh->GetYaxis()->FindBin(40.);
  
  if (iy0<1)                           corr = hh->GetBinContent(ix,iy1);
  if (iy1>hh->GetYaxis()->GetNbins())  corr = hh->GetBinContent(ix,iy0);
  
  float y0 = hh->GetYaxis()->GetBinCenter(iy0);
  float y1 = hh->GetYaxis()->GetBinCenter(iy1);
  // subtract effect for 40 GeV Et electrons
  it = m_hcorr.find(PATCore::ParticleType::Electron);
  float c0 = hh->GetBinContent(ix,iy0) - it->second->GetBinContent(ix,iyc);
  float c1 = hh->GetBinContent(ix,iy1) - it->second->GetBinContent(ix,iyc);
  
  corr = (c1*(etGeV-y0) + c0*(y1-etGeV)) / (y1-y0);
  ATH_MSG_VERBOSE("correction from ADC non linearity = " << corr);

  return (1.+corr);
}

