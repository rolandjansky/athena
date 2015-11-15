/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "HIJetRec/HIJetSignificanceTool.h"
#include "xAODCaloEvent/CaloCluster.h"

HIJetSignificanceTool::HIJetSignificanceTool(const std::string& name)
  : JetModifierBase(name)
{
}


StatusCode HIJetSignificanceTool::initialize()
{
  return StatusCode::SUCCESS;
}


int HIJetSignificanceTool::modifyJet( xAOD::Jet& jet ) const 
{
  float significance=0;
  float cell_sig=0;
  int cell_sig_sampling=-1;
  int n_sig_max_tile=0;
  float sig_tile=0;
  float E_tile=0;
  const xAOD::JetConstituentVector constituents = jet.getConstituents();

  for(xAOD::JetConstituentVector::iterator itr = constituents.begin(); itr != constituents.end(); ++itr) 

  {
    const xAOD::CaloCluster* cl=static_cast<const xAOD::CaloCluster*>( itr->rawConstituent() );
    if( !cl) continue;

    double m=0;
    double m2=0;
    unsigned int samp_max=CaloSampling::Unknown;


    double cl_E=cl->altE(); //signifiance moments were computed from unsubtracted cell energies
    double sigma_cl=0;

    for(unsigned int isample=12; isample<21; isample++) E_tile+=cl->eSample( (CaloSampling::CaloSample) isample);

    if(cl->retrieveMoment(xAOD::CaloCluster::SIGNIFICANCE,m) ) 
    {
      sigma_cl=cl_E/m;
      significance+=sigma_cl*sigma_cl;
    }
    if(cl->retrieveMoment(xAOD::CaloCluster::CELL_SIG_SAMPLING,m2))
    {
      samp_max=static_cast<unsigned int>(m2);
      if(CaloSampling::getSamplingName(samp_max).find("Tile")!=std::string::npos) 
      {
	n_sig_max_tile++;
	sig_tile+=sigma_cl*sigma_cl;
      }
    }
    
    if( cl->retrieveMoment(xAOD::CaloCluster::CELL_SIGNIFICANCE,m) )
    {
      if( m > cell_sig )
      {
	cell_sig=m;
	if(samp_max!=CaloSampling::Unknown) cell_sig_sampling=samp_max;
      }
    }
  }
  significance=jet.jetP4(xAOD::JetEMScaleMomentum).E()/std::sqrt(significance);
  sig_tile=E_tile/std::sqrt(sig_tile);

  // set the attributes
  jet.setAttribute<float>("SIGNIFICANCE",significance);
  jet.setAttribute<float>("CELL_SIGNIFICANCE",cell_sig);
  jet.setAttribute<int>("CELL_SIG_SAMPLING", cell_sig_sampling);
  jet.setAttribute<int>("N_MAX_SIG_TILE", n_sig_max_tile);
  jet.setAttribute<float>("SIGNIFICANCE_TILE", sig_tile);
    
  return 0;
}


