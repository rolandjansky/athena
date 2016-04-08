/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: egammaSumCellsGainAlg.cxx 605541 2014-07-09 04:46:56Z ssnyder $
/**
 * @file egammaD3PDAnalysis/src/egammaMaxECellAlg.cxx
 * @author Mike Hance
 * @date Aug. 2013
 * @brief Sum cell energies with given gain
 */


#include "egammaSumCellsGainAlg.h"
#include "D3PDMakerInterfaces/ICollectionGetterTool.h"
#include "xAODEgamma/Egamma.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


egammaSumCellsGainAlg::egammaSumCellsGainAlg (const std::string& name,
					      ISvcLocator* svcloc)
  : AthAlgorithm (name, svcloc),
    m_getter (this)
{
  declareProperty ("AuxPrefix", m_auxPrefix,
                   "Prefix to add to aux data items.");
  declareProperty ("AllowMissing", m_allowMissing = false,
		     "If true, don't complain if input objects are missing.");
  declareProperty ("Getter", m_getter,
		     "Getter instance for the input egamma objects.");
}


/**
 * @brief Standard Gaudi @c initialize method.
 */
StatusCode egammaSumCellsGainAlg::initialize()
{
  msg() << MSG::INFO <<" Starting egammaSumCellsGainAlg" << endreq;
  
  CHECK( AthAlgorithm::initialize() );
  CHECK( m_getter.retrieve() );
  CHECK( m_getter->configureD3PD<xAOD::Egamma>() ); 
  return StatusCode::SUCCESS;
}


/**
 * @brief Standard Gaudi @c execute method.
 */
StatusCode egammaSumCellsGainAlg::execute()
{
  StatusCode sc = StatusCode::SUCCESS;

  msg()<<MSG::DEBUG << " In execute: Getting egamma Objects  " << endreq;
  
#define DECOR(TYPE,N) xAOD::Egamma::Decorator<TYPE> N (m_auxPrefix + #N)
  DECOR (float, Es0LowGain);
  DECOR (float, Es0MedGain);
  DECOR (float, Es0HighGain);

  DECOR (float, Es1LowGain);
  DECOR (float, Es1MedGain);
  DECOR (float, Es1HighGain);

  DECOR (float, Es2LowGain);
  DECOR (float, Es2MedGain);
  DECOR (float, Es2HighGain);

  DECOR (float, Es3LowGain);
  DECOR (float, Es3MedGain);
  DECOR (float, Es3HighGain);
#undef DECOR 

  CHECK( m_getter->reset (m_allowMissing) );
  while (const xAOD::Egamma* eg = m_getter->next<xAOD::Egamma>()){
    const xAOD::CaloCluster *cluster = eg->caloCluster();
    
    float cell_sum_energy_gainlow[4];
    float cell_sum_energy_gainmed[4];
    float cell_sum_energy_gainhgh[4];
    for(int i=0; i<4; i++){
      cell_sum_energy_gainlow[i]=0;
      cell_sum_energy_gainmed[i]=0;
      cell_sum_energy_gainhgh[i]=0;
    }
    
    msg()<<MSG::DEBUG <<" Checking Cluster  " << endreq;
    
    if(cluster) {
      if(cluster->getCellLinks()) {
	
        for (const CaloCell* cell : *cluster) {
	  int sampling = cell->caloDDE()->getSampling();
	  int layer = -1;
	  switch(sampling){
	  case CaloCell_ID::PreSamplerB: 
	  case CaloCell_ID::PreSamplerE:
	    layer=0;
	    break;
	  case CaloCell_ID::EMB1: 
	  case CaloCell_ID::EME1:
	    layer=1;
	    break;
	  case CaloCell_ID::EMB2: 
	  case CaloCell_ID::EME2:
	    layer=2;
	    break;
	  case CaloCell_ID::EMB3: 
	  case CaloCell_ID::EME3:
	    layer=3;
	    break;
	  default:
	    break;
	  }

	  if(layer < 0) continue;

	  switch(cell->gain()){
	  case 0: cell_sum_energy_gainhgh[layer] += cell->energy(); break;
	  case 1: cell_sum_energy_gainmed[layer] += cell->energy(); break;
	  case 2: cell_sum_energy_gainlow[layer] += cell->energy(); break;
	  default: break;
	  }
	}
      }
    }

    Es0LowGain(*eg)  = cell_sum_energy_gainlow[0];
    Es0MedGain(*eg)  = cell_sum_energy_gainmed[0];
    Es0HighGain(*eg) = cell_sum_energy_gainhgh[0];

    Es1LowGain(*eg)  = cell_sum_energy_gainlow[1];
    Es1MedGain(*eg)  = cell_sum_energy_gainmed[1];
    Es1HighGain(*eg) = cell_sum_energy_gainhgh[1];

    Es2LowGain(*eg)  = cell_sum_energy_gainlow[2];
    Es2MedGain(*eg)  = cell_sum_energy_gainmed[2];
    Es2HighGain(*eg) = cell_sum_energy_gainhgh[2];

    Es3LowGain(*eg)  = cell_sum_energy_gainlow[3];
    Es3MedGain(*eg)  = cell_sum_energy_gainmed[3];
    Es3HighGain(*eg) = cell_sum_energy_gainhgh[3];

    m_getter->releaseElement (eg);
  }
  
  return sc;
}


} // namespace D3PD
