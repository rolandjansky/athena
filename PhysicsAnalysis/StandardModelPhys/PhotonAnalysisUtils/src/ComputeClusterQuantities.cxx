/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
********************************************************************

NAME:     ComputeClusterQuantities.cxx
PACKAGE:  PhotonAnalysisUtils

AUTHOR:  JF Marchand 
CREATED:  Dec 2011

PURPOSE:  Tool to compute cluster related quantities

Updated:  

********************************************************************/

#include "PhotonAnalysisUtils/ComputeClusterQuantities.h"

#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloEvent/CaloCluster.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCell.h"

#define UNKNOWN -99999

ComputeClusterQuantities::ComputeClusterQuantities(const std::string& type,
						   const std::string& name,
						   const IInterface* parent)
  : AthAlgTool(type, name, parent)
{
  declareInterface<IComputeClusterQuantities>(this);
  
}

ComputeClusterQuantities::~ComputeClusterQuantities() {}

/////////////////////////////////////////////////////////////////

//========================================       
StatusCode ComputeClusterQuantities::initialize()
{   

  ATH_MSG_DEBUG(" Initializing ComputeClusterQuantities");

  StoreGateSvc* detStore;
  StatusCode sc = service("DetectorStore",detStore);
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Unable to get pointer to Detector Store Service");
    return sc;
  }
  sc = detStore->retrieve(m_calo_helper);
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve calo helper from DetectorStore");
    return sc;
  }

  return StatusCode::SUCCESS;
}
//========================================
StatusCode ComputeClusterQuantities::finalize()
{
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////
// ATHENA EXECUTE METHOD:

StatusCode ComputeClusterQuantities::execute(const egamma *eg)
{
  m_eS0_real  = 0;
  m_eS1_real  = 0;
  m_eS2_real  = 0;
  m_eS3_real  = 0;
  m_EcellS0   = UNKNOWN;
  m_etacellS0 = UNKNOWN;

  if (!eg->cluster()) return StatusCode::FAILURE;
  
  if(eg->cluster() && eg->cluster()->getNumberOfCells()){
    CaloCluster::cell_iterator iterator_current_cell_of_cluster = eg->cluster()->cell_begin();
    CaloCluster::cell_iterator iterator_last_cell_of_cluster    = eg->cluster()->cell_end();
    
    for (; iterator_current_cell_of_cluster != iterator_last_cell_of_cluster; ++iterator_current_cell_of_cluster) {
      const CaloCell* current_cell = (*iterator_current_cell_of_cluster) ;
      if(!current_cell) continue;
      Identifier id  = current_cell->ID();
      switch (m_calo_helper->calo_sample(id)) {
      case CaloSampling::PreSamplerB: 
      case CaloSampling::PreSamplerE:
	m_eS0_real += (*iterator_current_cell_of_cluster)->energy();
	break;
      case CaloSampling::EMB1:
      case CaloSampling::EME1:
	m_eS1_real += (*iterator_current_cell_of_cluster)->energy();
	break;
      case CaloSampling::EMB2:
      case CaloSampling::EME2:
	m_eS2_real += (*iterator_current_cell_of_cluster)->energy();
	break;
      case CaloSampling::EMB3:
      case CaloSampling::EME3:
	m_eS3_real += (*iterator_current_cell_of_cluster)->energy();
	break;
      }
    }
  }

  double EcellPSBarrel = eg->cluster()->getVariable(CaloVariableType::MAX_ENERGY, CaloSampling::PreSamplerB) ;
  if ( EcellPSBarrel > 0 ) {
    m_EcellS0   = EcellPSBarrel;
    m_etacellS0 = eg->cluster()->getVariable(CaloVariableType::MAX_ETA, CaloSampling::PreSamplerB);
  } else {
    m_EcellS0   = eg->cluster()->getVariable(CaloVariableType::MAX_ENERGY, CaloSampling::PreSamplerE) ;
    m_etacellS0 = eg->cluster()->getVariable(CaloVariableType::MAX_ETA, CaloSampling::PreSamplerE) ;
  }


  return StatusCode::SUCCESS; 
}
