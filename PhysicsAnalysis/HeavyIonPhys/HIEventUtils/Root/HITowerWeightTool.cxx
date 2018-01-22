/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIEventUtils/HITowerWeightTool.h"
#include "PathResolver/PathResolver.h"
#include "xAODEventInfo/EventInfo.h"
#include <TH3F.h>
#include <TFile.h>
#include <sstream>
#include <iostream>
#include <iomanip>

HITowerWeightTool::HITowerWeightTool(const std::string& n) : asg::AsgTool(n),
							     m_init(false),
							     m_h3_w(nullptr),
							     m_h3_eta(nullptr),
							     m_h3_phi(nullptr),
							     m_h3_mag(nullptr),
							     m_h3_eta_phi_response(nullptr),
							     m_h3_eta_phi_offset(nullptr),
							     m_run_number(0),
							     m_run_index(0)

{
  declareProperty("InputFile",m_input_file="cluster.geo.root","File containing cluster geometric moments.");
  declareProperty("ConfigDir",m_config_dir="HIEventUtils/","Directory containing configuration file.");
  if(initialize().isFailure()) ATH_MSG_WARNING("Initial configuration of tool failed");

}


float HITowerWeightTool::getWeight(float eta, float phi, int sample) const
{
  return m_h3_w->GetBinContent(m_h3_w->FindBin(eta,phi,sample));
}
float HITowerWeightTool::getWeightEta(float eta, float phi, int sample) const
{
  return m_h3_eta->GetBinContent(m_h3_eta->FindBin(eta,phi,sample));
}
float HITowerWeightTool::getWeightPhi(float eta, float phi, int sample) const
{
  return m_h3_phi->GetBinContent(m_h3_phi->FindBin(eta,phi,sample));
}
float HITowerWeightTool::getWeightMag(float eta, float phi, int sample) const
{
  return m_h3_mag->GetBinContent(m_h3_mag->FindBin(eta,phi,sample));
}

float HITowerWeightTool::getEtaPhiResponse(float eta, float phi) const
{
  if(m_run_index==0)  return 1;

  int eb=m_h3_eta_phi_response->GetXaxis()->FindBin(eta);
  int pb=m_h3_eta_phi_response->GetYaxis()->FindBin(phi);
  float rv=m_h3_eta_phi_response->GetBinContent(eb,pb,m_run_index);
  return rv;
}

float HITowerWeightTool::getEtaPhiOffset(float eta, float phi) const
{
  if(m_run_index==0) return 0;
  int eb=m_h3_eta_phi_offset->GetXaxis()->FindBin(eta);
  int pb=m_h3_eta_phi_offset->GetYaxis()->FindBin(phi);
  return m_h3_eta_phi_offset->GetBinContent(eb,pb,m_run_index)*std::cosh(eta);
}

StatusCode HITowerWeightTool::configureEvent()
{
  const xAOD::EventInfo* ei=nullptr;
  if(evtStore()->retrieve(ei,"EventInfo").isFailure())
  {
    ATH_MSG_ERROR("Could not retrieve EventInfo");
    return StatusCode::FAILURE;
  }
  unsigned int run_number=ei->runNumber();
  if(m_run_number!=run_number)
  {
    auto itr=m_run_map.find(run_number);
    if(itr==m_run_map.end())
    {
      m_run_index=0;
      ATH_MSG_WARNING("No calibration avaliable for " << run_number << ". Doing no eta-phi correction.");
    }
    else m_run_index=itr->second;
    m_run_number=run_number;
  }
  return StatusCode::SUCCESS;
}

StatusCode HITowerWeightTool::initialize()
{
  if(m_init) return StatusCode::SUCCESS;
  std::string local_path=m_config_dir+m_input_file;
  std::string full_path=PathResolverFindCalibFile(local_path);
  TFile* f=TFile::Open(full_path.c_str());
  if(f==nullptr) 
  {
    ATH_MSG_FATAL("Cannot read config file " << full_path ); 
    return StatusCode::FAILURE;
  }

  m_h3_w=(TH3F*)f->GetObjectChecked("h3_w","TH3F");
  if(m_h3_w==nullptr) 
  {
    ATH_MSG_FATAL("Cannot find TH3F m_h3_w in config file " << full_path ); 
    return StatusCode::FAILURE;
  }

  m_h3_eta=(TH3F*)f->GetObjectChecked("h3_eta","TH3F");
  if(m_h3_eta==nullptr) 
  {
    ATH_MSG_FATAL("Cannot find TH3F m_h3_eta in config file " << full_path ); 
    return StatusCode::FAILURE;
  }

  m_h3_phi=(TH3F*)f->GetObjectChecked("h3_phi","TH3F");
  if(m_h3_phi==nullptr) 
  {
    ATH_MSG_FATAL("Cannot find TH3F m_h3_phi in config file " << full_path ); 
    return StatusCode::FAILURE;
  }

  m_h3_mag=(TH3F*)f->GetObjectChecked("h3_R","TH3F");
  if(m_h3_mag==nullptr) 
  {
    ATH_MSG_FATAL("Cannot find TH3F m_h3_mag in config file " << full_path ); 
    return StatusCode::FAILURE;
  }

  m_h3_w->SetDirectory(0);
  m_h3_eta->SetDirectory(0);
  m_h3_phi->SetDirectory(0);
  m_h3_mag->SetDirectory(0);

  m_h3_eta_phi_response=(TH3F*)f->GetObjectChecked("h3_eta_phi_response","TH3F");
  if(m_h3_eta_phi_response==nullptr) 
  {
    ATH_MSG_FATAL("Cannot find TH3F h3_eta_phi_response in config file " << full_path ); 
    return StatusCode::FAILURE;
  }
  m_h3_eta_phi_response->SetDirectory(0);
  m_h3_eta_phi_offset=(TH3F*)f->GetObjectChecked("h3_eta_phi_offset","TH3F");
  if(m_h3_eta_phi_offset==nullptr) 
  {
    ATH_MSG_FATAL("Cannot find TH3F h3_eta_phi_offset in config file " << full_path ); 
    return StatusCode::FAILURE;
  }
  m_h3_eta_phi_offset->SetDirectory(0);
  
  TH1I* h1_run_index=(TH1I*)f->GetObjectChecked("h1_run_index","TH1I");
  if(h1_run_index==nullptr) 
  {
    ATH_MSG_FATAL("Cannot find TH3F h1_run_index in config file " << full_path ); 
    return StatusCode::FAILURE;
  }
  for(int xbin=1; xbin<h1_run_index->GetNbinsX(); xbin++) m_run_map.emplace_hint(m_run_map.end(),std::make_pair(h1_run_index->GetBinContent(xbin),xbin));
  f->Close();
  m_init=true;
  return StatusCode::SUCCESS;

}
