/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: egammaMaxECellAlg.cxx 775884 2016-09-29 15:57:53Z ssnyder $
/**
 * @file SUSYD3PDMAker/src/egammaMaxECellAlg.cxx
 * @author Nikiforos K. Nikiforou <nikiforo@cern.ch> adapted from various tools by Scott Snyder and others
 * @date Apr, 2012
 * @brief Find max E cell in middle layer for an egamma object EM Cluster and store in UserData.
 */


#include "egammaMaxECellAlg.h"
#include "D3PDMakerInterfaces/ICollectionGetterTool.h"
#include "xAODEgamma/Egamma.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


egammaMaxECellAlg::egammaMaxECellAlg (const std::string& name,
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
StatusCode egammaMaxECellAlg::initialize()
{
  ATH_MSG_INFO(" Starting egammaMaxECellAlg"  );
    
  CHECK( AthAlgorithm::initialize() );
  CHECK( m_cablingKey.initialize() );
  CHECK( m_getter.retrieve() );
  CHECK( m_getter->configureD3PD<xAOD::Egamma>() ); 
  return StatusCode::SUCCESS;
}


/**
 * @brief Standard Gaudi @c execute method.
 */
StatusCode egammaMaxECellAlg::execute()
{
  StatusCode sc = StatusCode::SUCCESS;

#define DECOR(TYPE,N) xAOD::Egamma::Decorator<TYPE> N (m_auxPrefix + #N)
  DECOR(float,        maxEcell_time);
  DECOR(float,        maxEcell_energy);
  DECOR(int,          maxEcell_gain);
  DECOR(unsigned int, maxEcell_onlId);
  DECOR(float,        maxEcell_x);
  DECOR(float,        maxEcell_y);
  DECOR(float,        maxEcell_z);
#undef DECOR

  CHECK( m_getter->reset (m_allowMissing) );
  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
  const LArOnOffIdMapping* cabling{*cablingHdl};
  if(!cabling){
     ATH_MSG_ERROR("Do not have mapping object " << m_cablingKey.key() );
     return StatusCode::FAILURE;
  }
  while (const xAOD::Egamma* eg = m_getter->next<xAOD::Egamma>())
  {
    const xAOD::CaloCluster *cluster = eg->caloCluster();
        
    maxEcell_time(*eg)   = -9898.9;
    maxEcell_energy(*eg) = -9898.9;
    maxEcell_gain(*eg)   = -1;
    maxEcell_onlId(*eg)  = 0;
        
    maxEcell_x(*eg) = -9898.9;
    maxEcell_y(*eg) = -9898.9;
    maxEcell_z(*eg) = -9898.9;
        
    if(cluster){
      if(cluster->getCellLinks()){
                
        float emax=-9999.;

        const CaloCell* cell_maxE=0;
        for (const CaloCell* cell : *cluster) {
          int sampling = cell->caloDDE()->getSampling();
          if (sampling== CaloCell_ID::EMB2 || sampling== CaloCell_ID::EME2) {
            if ( (cell->provenance() & 0x2000) ) {
              if (cell->energy() > emax) {
                emax=cell->energy();
                cell_maxE=cell;
              }
            }
          }
        }
                
        if(cell_maxE){
          const CaloDetDescrElement* caloDDEl = cell_maxE->caloDDE();
                    
          maxEcell_time(*eg)   = cell_maxE->time();
          maxEcell_energy(*eg) = cell_maxE->energy();
          maxEcell_gain(*eg)   = (int) cell_maxE->gain();
          maxEcell_onlId(*eg)  = (unsigned int) (cabling->createSignalChannelID(caloDDEl->identify())).get_compact();
          
          maxEcell_x(*eg) = caloDDEl->x();
          maxEcell_y(*eg) = caloDDEl->y();
          maxEcell_z(*eg) = caloDDEl->z();
        }
                
                
      }
    }

    m_getter->releaseElement (eg);
  }

  return sc;
}


} // namespace D3PD
