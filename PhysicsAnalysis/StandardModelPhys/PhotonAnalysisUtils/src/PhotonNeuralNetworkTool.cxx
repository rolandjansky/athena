/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id $
/* 
 * @file PhotonNeuralNetworkTool.cxx
 * @author Mike Hance <mike.hance@cern.ch>
 * @date April, 2012
 */

#include "PhotonNeuralNetworkTool.h"

#include "egammaEvent/Photon.h"

#include "egammaEvent/EMShower.h"
#include "CaloEvent/CaloCluster.h"

#include "PhotonAnalysisUtils/NeuralPhoton.h"
#include "FourMomUtils/P4Helpers.h"
#include "PathResolver/PathResolver.h"

#include "egammaAnalysisUtils/EnergyRescaler.h"

#include "AthenaKernel/errorcheck.h"

PhotonNeuralNetworkTool::PhotonNeuralNetworkTool(const std::string& type, const std::string& name, const IInterface* parent) 
  : AthAlgTool(type, name, parent)
{
  declareProperty ("Weights", m_weights="NeuralPhoton_TMVA");
  declareInterface<IPhotonNeuralNetworkTool>(this);

  m_cachedResults = std::make_pair(-1,false);
}

PhotonNeuralNetworkTool::~PhotonNeuralNetworkTool(){
}


StatusCode PhotonNeuralNetworkTool::initialize() {
  m_neuralPhoton = new NeuralPhoton(PathResolver::find_directory (m_weights, "DATAPATH"), 2, 2);

  m_eRescaler = new eg2011::EnergyRescaler;
  m_eRescaler->useDefaultCalibConstants("2011");

  return StatusCode::SUCCESS;
}


StatusCode PhotonNeuralNetworkTool::finalize() {
  delete m_neuralPhoton;
  delete m_eRescaler;

  return StatusCode::SUCCESS;
}

bool PhotonNeuralNetworkTool::Passes(const Analysis::Photon* ph){
  if(ph == m_cachedPhoton)
    return m_cachedResults.second;
  else
    return GetResults(ph).second;
}


float PhotonNeuralNetworkTool::Discriminant(const Analysis::Photon* ph){
  if(ph == m_cachedPhoton)
    return m_cachedResults.first;
  else
    return GetResults(ph).first;
}


std::pair<float,bool> PhotonNeuralNetworkTool::GetResults(const Analysis::Photon* g){
  if(g == m_cachedPhoton) return m_cachedResults;

  float eta,pt,e233,e237,e277,reta,rphi,deltaEs,eratio,e2tsts1,emins1,emaxs1;
  double e_corrected;
  int convFlag;
  std::string particle_type;

  const EMShower* shower = g->detail<EMShower>();
  if(!shower){
    m_cachedResults.second=false;
    m_cachedResults.first=-1;
    return m_cachedResults;
  }
    
  // This is a total hack.  This needs to go into some tool in PAU that
  // will do the computation centrally.  To be done when we restructure PAU.
  convFlag = 0;
  if ( g->conversion() )
    convFlag = g->conversion()->vxTrackAtVertex()->size() ;
  if ( g->author()&egammaParameters::AuthorRConv ) 
    convFlag += 10;

  eta = g->cluster()->etaBE(2);
  particle_type = ((convFlag%10) == 0) ? "UNCONVERTED_PHOTON" : "CONVERTED_PHOTON";
  e_corrected = m_eRescaler->applyEnergyCorrectionGeV(g->cluster()->eta(), 
						      g->cluster()->phi(), 
						      g->cluster()->e()/1000., 
						      g->cluster()->pt()/1000., 
						      eg2011::EnergyRescaler::NOMINAL,
						      particle_type) * 1000.;
  if(fabs(eta)<5)
    pt = cosh(eta) != 0 ? e_corrected/cosh(eta):0;
  else
    pt = 0;

  if(pt < 25000 || fabs(eta) > 2.37 || (fabs(eta)>1.37 && fabs(eta)<1.52)){
    m_cachedResults.second=false;
    m_cachedResults.first=-1;
    return m_cachedResults;
  }
    
  e233 = shower->parameter(egammaParameters::e233);
  e237 = shower->parameter(egammaParameters::e237);
  e277 = shower->parameter(egammaParameters::e277);
	  
  rphi = e237 ? e233 / e237 : 0;
  reta = e277 ? e237 / e277 : 0;

  e2tsts1 = shower->parameter(egammaParameters::e2tsts1);
  emins1  = shower->parameter(egammaParameters::emins1 );
  emaxs1  = shower->parameter(egammaParameters::emaxs1 );

  deltaEs = e2tsts1 - emins1;
  eratio  = (emaxs1+e2tsts1) != 0 ? (emaxs1 - e2tsts1)/(emaxs1 + e2tsts1) : -1;
    

  m_cachedResults.second = m_neuralPhoton->GetPassNetwork(eta, 
							  pt,
							  convFlag,
							  shower->parameter(egammaParameters::ethad)/pt,
							  shower->parameter(egammaParameters::ethad1)/pt,
							  reta,
							  shower->parameter(egammaParameters::weta2),
							  rphi, 
							  shower->parameter(egammaParameters::weta1), 
							  shower->parameter(egammaParameters::wtots1), 
							  deltaEs, 
							  eratio, 
							  shower->parameter(egammaParameters::fracs1), 
							  shower->parameter(egammaParameters::f1), 
							  shower->parameter(egammaParameters::e277),
							  &m_cachedResults.first);

  return m_cachedResults;
}
