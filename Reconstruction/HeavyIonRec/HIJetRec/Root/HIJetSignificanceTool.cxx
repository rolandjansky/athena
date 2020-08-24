/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "StoreGate/WriteDecorHandle.h"

#include "HIJetSignificanceTool.h"
#include "xAODCaloEvent/CaloCluster.h"

HIJetSignificanceTool::HIJetSignificanceTool(const std::string& name)
  : asg::AsgTool(name)
{
  declareInterface<IJetDecorator>(this);
}


StatusCode HIJetSignificanceTool::initialize()
{
  //First we chekc that jet container name is defined
  if( m_jetContainerName.empty() ){
    ATH_MSG_ERROR("HIJetSignificanceTool needs to have its input jet container name configured!");
    return StatusCode::FAILURE;
  }
  //Preped Jet Container Name
  m_jetSignificanceKey = m_jetContainerName + "." + m_jetSignificanceKey.key();
  m_jetCellSignificanceKey = m_jetContainerName + "." + m_jetCellSignificanceKey.key();
  m_jetCellSigSamplingKey = m_jetContainerName + "." + m_jetCellSigSamplingKey.key();
  m_jetNMaxSigTileKey = m_jetContainerName + "." + m_jetNMaxSigTileKey.key();
  m_jetSignificanceTileKey = m_jetContainerName + "." + m_jetSignificanceTileKey.key();

  //Keys Initialization
  ATH_CHECK( m_jetSignificanceKey.initialize() );
  ATH_CHECK( m_jetCellSignificanceKey.initialize() );
  ATH_CHECK( m_jetCellSigSamplingKey.initialize() );
  ATH_CHECK( m_jetNMaxSigTileKey.initialize() );
  ATH_CHECK( m_jetSignificanceTileKey.initialize() );

  return StatusCode::SUCCESS;
}


StatusCode HIJetSignificanceTool::decorate( const xAOD::JetContainer& jets ) const
{
  //From decor keys to handlers
  SG::WriteDecorHandle<xAOD::JetContainer, float> jetSignificanceDecorHandle( m_jetSignificanceKey );
  SG::WriteDecorHandle<xAOD::JetContainer, float> jetCellSignificanceDecorHandle ( m_jetCellSignificanceKey );
  SG::WriteDecorHandle<xAOD::JetContainer, float> jetCellSigSamplingDecorHandle ( m_jetCellSigSamplingKey );
  SG::WriteDecorHandle<xAOD::JetContainer, int  > jetNMaxSigTileDecorHandle ( m_jetNMaxSigTileKey );
  SG::WriteDecorHandle<xAOD::JetContainer, float> jetSignificanceTileDecorHandle ( m_jetSignificanceTileKey );

  //Here we were passing through JetConstituentVector. Now we access this directly and in a loop of jets, not for single instances
  //const xAOD::JetConstituentVector constituents = jets.getConstituents();
  //for(xAOD::JetConstituentVector::iterator itr = constituents.begin(); itr != constituents.end(); ++itr)
  for(const xAOD::Jet* jet : jets)
  {
    float significance=0;
    float cell_sig=0;
    int cell_sig_sampling=-1;
    int n_sig_max_tile=0;
    float sig_tile=0;
    float E_tile=0;
    // loop over raw constituents. Look for clusters
    size_t num = jet->numConstituents();
    for(size_t i = 0; i < num; i++)
    {
      const xAOD::CaloCluster* cl=static_cast<const xAOD::CaloCluster*>( (jet->rawConstituent(i)) );
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
    significance=jet->jetP4(xAOD::JetEMScaleMomentum).E()/std::sqrt(significance);
    sig_tile=E_tile/std::sqrt(sig_tile);

    // set the attributes
    jetSignificanceDecorHandle(*jet) = significance;
    jetCellSignificanceDecorHandle(*jet) = cell_sig;
    jetCellSigSamplingDecorHandle(*jet) = cell_sig_sampling;
    jetNMaxSigTileDecorHandle(*jet) = n_sig_max_tile;
    jetSignificanceTileDecorHandle(*jet) = sig_tile;

  }//End of loop over jets

  return StatusCode::SUCCESS;
}
