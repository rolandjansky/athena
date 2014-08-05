/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: egammaNbCellsGainAlg.cxx 605541 2014-07-09 04:46:56Z ssnyder $
/**
 * @file egammaD3PDAnalysis/src/egammaNbCellsGainAlg.cxx
 * @date Aug. 2013
 * @brief Number of cells in a given gain
 */


#include "egammaNbCellsGainAlg.h"
#include "D3PDMakerInterfaces/ICollectionGetterTool.h"
#include "xAODEgamma/Egamma.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "AthenaKernel/errorcheck.h"



namespace D3PD {


egammaNbCellsGainAlg::egammaNbCellsGainAlg (const std::string& name,
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
StatusCode egammaNbCellsGainAlg::initialize()
{
  msg() << MSG::INFO <<" Starting egammaNbCellsGainAlg" << endreq;
  
  CHECK( AthAlgorithm::initialize() );
  CHECK( m_getter.retrieve() );
  CHECK( m_getter->configureD3PD<xAOD::Egamma>() ); 
  return StatusCode::SUCCESS;
}


/**
 * @brief Standard Gaudi @c execute method.
 */
StatusCode egammaNbCellsGainAlg::execute()
{
  StatusCode sc = StatusCode::SUCCESS;

  msg()<<MSG::DEBUG << " In execute: Getting egamma Objects  " << endreq;
  
#define DECOR(TYPE,N) xAOD::Egamma::Decorator<TYPE> N (m_auxPrefix + #N)
  DECOR(int, nbCells_s0LowGain);
  DECOR(int, nbCells_s0MedGain);
  DECOR(int, nbCells_s0HighGain);

  DECOR(int, nbCells_s1LowGain);
  DECOR(int, nbCells_s1MedGain);
  DECOR(int, nbCells_s1HighGain);

  DECOR(int, nbCells_s2LowGain);
  DECOR(int, nbCells_s2MedGain);
  DECOR(int, nbCells_s2HighGain);

  DECOR(int, nbCells_s3LowGain);
  DECOR(int, nbCells_s3MedGain);
  DECOR(int, nbCells_s3HighGain);
#undef DECOR

  CHECK( m_getter->reset (m_allowMissing) );
  while (const xAOD::Egamma* eg = m_getter->next<xAOD::Egamma>()){
    const xAOD::CaloCluster *cluster = eg->caloCluster();

    int cell_nbcells_gainlow[4];
    int cell_nbcells_gainmed[4];
    int cell_nbcells_gainhgh[4];

    for(int i=0; i<4; i++){
      cell_nbcells_gainlow[i]=0;
      cell_nbcells_gainmed[i]=0;
      cell_nbcells_gainhgh[i]=0;
    }

    msg()<<MSG::DEBUG <<" Checking Cluster  " << endreq;
    
    if(cluster){
      if(cluster->getCellLinks()){

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
	  case 0: cell_nbcells_gainhgh[layer]++; break;
	  case 1: cell_nbcells_gainmed[layer]++; break;
	  case 2: cell_nbcells_gainlow[layer]++; break;
	  default: break;
	  }
	}
      }
    }

    nbCells_s0LowGain(*eg)  = cell_nbcells_gainlow[0];
    nbCells_s0MedGain(*eg)  = cell_nbcells_gainmed[0];
    nbCells_s0HighGain(*eg) = cell_nbcells_gainhgh[0];

    nbCells_s1LowGain(*eg)  = cell_nbcells_gainlow[1];
    nbCells_s1MedGain(*eg)  = cell_nbcells_gainmed[1];
    nbCells_s1HighGain(*eg) = cell_nbcells_gainhgh[1];

    nbCells_s2LowGain(*eg)  = cell_nbcells_gainlow[2];
    nbCells_s2MedGain(*eg)  = cell_nbcells_gainmed[2];
    nbCells_s2HighGain(*eg) = cell_nbcells_gainhgh[2];

    nbCells_s3LowGain(*eg)  = cell_nbcells_gainlow[3];
    nbCells_s3MedGain(*eg)  = cell_nbcells_gainmed[3];
    nbCells_s3HighGain(*eg) = cell_nbcells_gainhgh[3];

    m_getter->releaseElement (eg);
  }
  
  return sc;
}
}
