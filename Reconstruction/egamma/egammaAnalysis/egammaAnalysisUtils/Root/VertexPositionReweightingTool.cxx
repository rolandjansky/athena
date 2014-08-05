/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <TROOT.h>
#include <TMath.h>

#include "egammaAnalysisUtils/VertexPositionReweightingTool.h"

#include <iostream>
#include <map>

//===============================================================================================================
VertexPositionReweightingTool::VertexPositionReweightingTool(eDataMCBeamspot dataMCsel,
							     std::string weightsFileName)
{
  m_h_weights = 0;
  m_h_mean_nrun = 0;
  m_h_sigma_nrun = 0;

  TFile *m_weightsFile = TFile::Open(weightsFileName.c_str());
  
  if ( !m_weightsFile ) {
    std::cout << "ERROR: could not open file with weights: " << weightsFileName << std::endl;
    m_h_weights = 0;
    return;
  } else {
    std::cout << "Successfully opened file with weights: " << weightsFileName << std::endl;
  }

  switch (dataMCsel) {
  case MC11wide:
    m_h_weights  = (TH1*) m_weightsFile->Get("zvtx_weight_MC11wide");
    break;
  case MC11narrow:
    m_h_weights  = (TH1*) m_weightsFile->Get("zvtx_weight_MC11narrow");
    break;
  case MC12a:
    m_h_weights  = (TH1*) m_weightsFile->Get("zvtx_weight_MC12a");
    m_h_mean_nrun = (TH1*) m_weightsFile->Get("zvtx_mean_nrun_2012");
    m_h_sigma_nrun = (TH1*) m_weightsFile->Get("zvtx_sigma_nrun_2012");
    break;
  default:
    // should never really come here
    std::cout << "ERROR: Unsupported weight selection!" << std::endl;
    break;
  }

  if ( !m_h_weights ) {
    std::cout << "ERROR: Could not get VertexPostition weights histogram from file " << weightsFileName << std::endl;
    std::cout << "Tool will always return weight=1." << std::endl;
  } else {
    std::cout << "Successfully retrieved histogram " <<  m_h_weights->GetName() << std::endl;
    m_h_weights->SetDirectory(gROOT);
    if (m_h_mean_nrun && m_h_sigma_nrun) {
      std::cout << "Successfully retrieved histograms " <<  m_h_mean_nrun->GetName()
		<< " and " << m_h_sigma_nrun->GetName() << std::endl;
      m_h_mean_nrun->SetDirectory(gROOT);
      m_h_sigma_nrun->SetDirectory(gROOT);
    }
  }
  m_weightsFile->Close();
  delete m_weightsFile;
}

VertexPositionReweightingTool::VertexPositionReweightingTool(std::string mcProductionTag,
							     std::string weightsFileName)
{
  m_h_weights = 0;
  m_h_mean_nrun = 0;
  m_h_sigma_nrun = 0;

  // https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/AtlasProductionGroupMC11c
  // true = wide MC
  std::map<std::string, eDataMCBeamspot> tagMC;
  // MC11 tags
  // primary sim tags
  tagMC["s1272"] = MC11wide;
  tagMC["s1273"] = MC11wide;
  tagMC["s1299"] = MC11narrow;
  tagMC["s1310"] = MC11narrow;
  tagMC["s1309"] = MC11narrow;
  tagMC["s1372"] = MC11narrow;
  tagMC["s1378"] = MC11narrow;
  tagMC["s1662"] = MC11narrow;
  // merge tags are probably also clear
  tagMC["s1274"] = MC11wide;
  tagMC["s1300"] = MC11narrow;
  tagMC["s1353"] = MC11narrow;
  tagMC["s1370"] = MC11narrow;
  tagMC["s1704"] = MC11narrow;
  // AFII is always narrow?
  tagMC["a131"] = MC11narrow;

  // MC12 tags
  // primary sim tags
  tagMC["s1468"] = MC12a;
  tagMC["s1472"] = MC12a;
  tagMC["s1469"] = MC12a;
  tagMC["s1479"] = MC12a;
  tagMC["s1482"] = MC12a;
  tagMC["s1484"] = MC12a;
  tagMC["s1485"] = MC12a;
  tagMC["s1486"] = MC12a;
  tagMC["s1499"] = MC12a;
  // merge tags are probably also clear
  tagMC["s1470"] = MC12a;
  tagMC["s1504"] = MC12a;
  // AFII
  tagMC["a159"] = MC12a;
  tagMC["a165"] = MC12a;
  tagMC["a169"] = MC12a;
  tagMC["a170"] = MC12a;
  tagMC["a171"] = MC12a;
  tagMC["a172"] = MC12a;
  tagMC["a173"] = MC12a;


  TFile *m_weightsFile = TFile::Open(weightsFileName.c_str());
  m_h_weights = 0;
  
  if ( !m_weightsFile ) {
    std::cout << "ERROR: could not open file with weights: "<<weightsFileName << std::endl;
    return;
  } else {
    std::cout << "Successfully opened file with weights: "<<weightsFileName << std::endl;
  }

  
  std::map<std::string, eDataMCBeamspot>::iterator it;
  for (it = tagMC.begin() ; it != tagMC.end(); it++) {
    // std::cout << (*it).first << " => " << (*it).second << std::endl;
    if (mcProductionTag.find((*it).first) != std::string::npos) {
      std::cout << "Found tag " << (*it).first << "  => ";
      switch ((*it).second) {
      case MC11wide:
	m_h_weights  = (TH1*) m_weightsFile->Get("zvtx_weight_MC11wide");
	std::cout << " wide MC11 zvtx distribution." << std::endl;
	break;
      case MC11narrow:
	std::cout << " narrow MC11 zvtx distribution." << std::endl;
	m_h_weights  = (TH1*) m_weightsFile->Get("zvtx_weight_MC11narrow");
	break;
      case MC12a:
	std::cout << " MC12a zvtx distribution." << std::endl;
	m_h_weights  = (TH1*) m_weightsFile->Get("zvtx_weight_MC12a");
	m_h_mean_nrun = (TH1*) m_weightsFile->Get("zvtx_mean_nrun_2012");
	m_h_sigma_nrun = (TH1*) m_weightsFile->Get("zvtx_sigma_nrun_2012");
	break;
      default:
	// should never really come here
	std::cout << "ERROR: Unsupported weight selection!" << std::endl;
	break;
      }
      break;
    }
  }

  if ( !m_h_weights ) {
    std::cout << "ERROR: could not interpret tag " << mcProductionTag
	      << " or could not get VertexPostition weight histogram." << std::endl;
  } else {
    std::cout << "Successfully retrieved histogram " <<  m_h_weights->GetName() << std::endl;
    m_h_weights->SetDirectory(gROOT);
    if (m_h_mean_nrun && m_h_sigma_nrun) {
      std::cout << "Successfully retrieved histograms " <<  m_h_mean_nrun->GetName()
		<< " and " << m_h_sigma_nrun->GetName() << std::endl;
      m_h_mean_nrun->SetDirectory(gROOT);
      m_h_sigma_nrun->SetDirectory(gROOT);
    }
  }
  m_weightsFile->Close();
  delete m_weightsFile;
}


//===============================================================================================================
VertexPositionReweightingTool::~VertexPositionReweightingTool()
{
  if (m_h_weights != 0)
    delete m_h_weights;

  if (m_h_mean_nrun != 0)
    delete m_h_mean_nrun;

  if (m_h_sigma_nrun != 0)
    delete m_h_sigma_nrun;
}


//===============================================================================================================
float VertexPositionReweightingTool::GetWeight( float vxp_z )
{
  if (!m_h_weights)
    return 1.;

  int bin = m_h_weights->FindBin( vxp_z );
  float weight = m_h_weights->GetBinContent(bin);
  return weight;
}

//===============================================================================================================
float VertexPositionReweightingTool::GetWeight( float vxp_z, int runnumber )
{
  // run dependent retrieval of zv weight
  if (!m_h_mean_nrun || !m_h_sigma_nrun)
    return 1.;

  // first check we actually have the run number
  int bin = m_h_mean_nrun->FindBin(runnumber);
  if (bin == 0 || bin > m_h_mean_nrun->GetNbinsX())
    return 1.;

  // now retrieve MC and data mean and sigma of Gaussians
  // MC is "hidden" in bin 0
  double mean[2], sigma[2], gaus[2];
  mean[0] = m_h_mean_nrun->GetBinContent(0);
  sigma[0] = m_h_sigma_nrun->GetBinContent(0);
  mean[1] = m_h_mean_nrun->GetBinContent(bin);
  sigma[1] = m_h_sigma_nrun->GetBinContent(bin);

  // now calculate the Gaussians
  for (int i = 0; i < 2; i++) {
    double x = (vxp_z - mean[i])/sigma[i];
    x *= -0.5*x;
    gaus[i] = 1/sigma[i]*TMath::Exp(x);
  }

  return gaus[1]/gaus[0];
}
