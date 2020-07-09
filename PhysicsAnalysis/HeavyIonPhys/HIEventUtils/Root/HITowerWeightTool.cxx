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
							     m_h3W(nullptr),
							     m_h3Eta(nullptr),
							     m_h3Phi(nullptr),
							     m_h3Mag(nullptr),
							     m_h3EtaPhiResponse(nullptr),
							     m_h3EtaPhiOffset(nullptr),
							     m_runNumber(0),
							     m_runIndex(0)

{
  declareProperty("ApplyCorrection",m_applycorrection=true,"If false unit weigts are applied");
  declareProperty("InputFile",m_inputFile="cluster.geo.HIJING_2018.root","File containing cluster geometric moments.");
  declareProperty("ConfigDir",m_configDir="HIJetCorrection/","Directory containing configuration file.");
}


float HITowerWeightTool::getWeight(float eta, float phi, int sample) const
{
  return m_h3W->GetBinContent(m_h3W->FindBin(eta,phi,sample));
}
float HITowerWeightTool::getWeightEta(float eta, float phi, int sample) const
{
  return m_h3Eta->GetBinContent(m_h3Eta->FindBin(eta,phi,sample));
}
float HITowerWeightTool::getWeightPhi(float eta, float phi, int sample) const
{
  return m_h3Phi->GetBinContent(m_h3Phi->FindBin(eta,phi,sample));
}
float HITowerWeightTool::getWeightMag(float eta, float phi, int sample) const
{
  return m_h3Mag->GetBinContent(m_h3Mag->FindBin(eta,phi,sample));
}

float HITowerWeightTool::getEtaPhiResponse(float eta, float phi) const
{
  if(m_runIndex==0)  return 1;

  int eb=m_h3EtaPhiResponse->GetXaxis()->FindBin(eta);
  int pb=m_h3EtaPhiResponse->GetYaxis()->FindBin(phi);
  float rv=m_h3EtaPhiResponse->GetBinContent(eb,pb,m_runIndex);
  return rv;
}

float HITowerWeightTool::getEtaPhiOffset(float eta, float phi) const
{
  if(m_runIndex==0) return 0;
  int eb=m_h3EtaPhiOffset->GetXaxis()->FindBin(eta);
  int pb=m_h3EtaPhiOffset->GetYaxis()->FindBin(phi);
  return m_h3EtaPhiOffset->GetBinContent(eb,pb,m_runIndex)*std::cosh(eta);
}

StatusCode HITowerWeightTool::configureEvent()
{
  if (!m_applycorrection){
    m_runIndex=0;
    ATH_MSG_DEBUG("Using unit weights and doing no eta-phi correction.");
    return StatusCode::SUCCESS;
  }

  const xAOD::EventInfo* ei=nullptr;
  if(evtStore()->retrieve(ei,"EventInfo").isFailure())
  {
    ATH_MSG_ERROR("Could not retrieve EventInfo");
    return StatusCode::FAILURE;
  }
  unsigned int run_number=ei->runNumber();

  if(m_runNumber!=run_number)
  {
    auto itr=m_runMap.find(run_number);
    if(itr==m_runMap.end())
    {
			//trying generic run number <=> no run dependence
		        run_number = 226000;
		        auto itrg=m_runMap.find(run_number);
		        if(itrg==m_runMap.end()){
		            m_runIndex=0;
		            ATH_MSG_WARNING("No generic calibration or calibration for " << run_number << " is avaliable. Doing no eta-phi correction.");
		        }
		        else {
		            m_runIndex=itrg->second;
		            ATH_MSG_DEBUG("Using generic calibration for eta-phi correction.");
		        }
    }
    else m_runIndex=itr->second;
    m_runNumber=run_number;
  }
  return StatusCode::SUCCESS;
}

StatusCode HITowerWeightTool::initialize()
{
  if(m_init) return StatusCode::SUCCESS;
  std::string local_path=m_configDir+m_inputFile;
  std::string full_path=PathResolverFindCalibFile(local_path);
  ATH_MSG_INFO("Reading input file "<< m_inputFile << " from " << full_path);
  TFile* f=TFile::Open(full_path.c_str());
  if(f==nullptr)
  {
    ATH_MSG_FATAL("Cannot read config file " << full_path );
    return StatusCode::FAILURE;
  }

  m_h3W=(TH3F*)f->GetObjectChecked("h3_w","TH3F");
  if(m_h3W==nullptr)
  {
    ATH_MSG_FATAL("Cannot find TH3F m_h3W in config file " << full_path );
    return StatusCode::FAILURE;
  }

  m_h3Eta=(TH3F*)f->GetObjectChecked("h3_eta","TH3F");
  if(m_h3Eta==nullptr)
  {
    ATH_MSG_FATAL("Cannot find TH3F m_h3Eta in config file " << full_path );
    return StatusCode::FAILURE;
  }

  m_h3Phi=(TH3F*)f->GetObjectChecked("h3_phi","TH3F");
  if(m_h3Phi==nullptr)
  {
    ATH_MSG_FATAL("Cannot find TH3F m_h3Phi in config file " << full_path );
    return StatusCode::FAILURE;
  }

  m_h3Mag=(TH3F*)f->GetObjectChecked("h3_R","TH3F");
  if(m_h3Mag==nullptr)
  {
    ATH_MSG_FATAL("Cannot find TH3F m_h3Mag in config file " << full_path );
    return StatusCode::FAILURE;
  }

  m_h3W->SetDirectory(0);
  m_h3Eta->SetDirectory(0);
  m_h3Phi->SetDirectory(0);
  m_h3Mag->SetDirectory(0);

  m_h3EtaPhiResponse=(TH3F*)f->GetObjectChecked("h3_eta_phi_response","TH3F");
  if(m_h3EtaPhiResponse==nullptr)
  {
    ATH_MSG_FATAL("Cannot find TH3F h3_eta_phi_response in config file " << full_path );
    return StatusCode::FAILURE;
  }
  m_h3EtaPhiResponse->SetDirectory(0);
  m_h3EtaPhiOffset=(TH3F*)f->GetObjectChecked("h3_eta_phi_offset","TH3F");
  if(m_h3EtaPhiOffset==nullptr)
  {
    ATH_MSG_FATAL("Cannot find TH3F h3_eta_phi_offset in config file " << full_path );
    return StatusCode::FAILURE;
  }
  m_h3EtaPhiOffset->SetDirectory(0);

  TH1I* h1_run_index=(TH1I*)f->GetObjectChecked("h1_run_index","TH1I");
  if(h1_run_index==nullptr)
  {
    ATH_MSG_FATAL("Cannot find TH3F h1_run_index in config file " << full_path );
    return StatusCode::FAILURE;
  }
  for(int xbin=1; xbin<=h1_run_index->GetNbinsX(); xbin++) {
		m_runMap.emplace_hint(m_runMap.end(),std::make_pair(h1_run_index->GetBinContent(xbin),xbin));
	}
  f->Close();
  m_init=true;
  return StatusCode::SUCCESS;

}
