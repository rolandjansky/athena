/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloClusterExtendedCellAssociationTool.cxx 535157 2013-01-28 22:33:25Z mhance $
/**
 * @file CaloD3PDMaker/CaloClusterExtendedCellAssociationTool.cxx
 * @author Mike Hance <mike.hance@cern.ch>
 * @date January, 2013
 * @brief Associate from a CaloCluster to CaloCells in a 7x11 area around centroid
 */


#include "CaloClusterExtendedCellAssociationTool.h"



namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */

CaloClusterExtendedCellAssociationTool::CaloClusterExtendedCellAssociationTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent)
{
  declareProperty ("Getter", m_getter,
                   "ICollectionGetterTool instance defining "
                   "the target collection");

  declareProperty ("DRCut",  m_drcut = 0.7,
                   "DR cut between cluster and cell");

  declareProperty ("PhiRange", m_phirange = 11, 
		   "Number of cells in phi to keep");

  declareProperty ("EtaRange", m_etarange = 7, 
		   "Number of cells in eta to keep");
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode CaloClusterExtendedCellAssociationTool::initialize()
{
  // check the config options, make sure it's clear how to choose the
  // cells to keep

  // Retrieve the getter tool and configure it, saying that we want
  // to get @c CaloCell objects from it.
  CHECK( m_getter.retrieve() );
  CHECK( m_getter->configureD3PD<CaloCell>() );
  return Base::initialize();
}


/**
 * @brief Return the target object.
 * @param p The source object for the association.
 *
 * Return the target of the association, or 0.
 */

StatusCode CaloClusterExtendedCellAssociationTool::reset (const CaloCluster& p)
{
  CHECK(m_getter->reset(true));
  m_cells.clear();

  while (const CaloCell* mycell = m_getter->next<CaloCell>()) {
    float deltaR = sqrt(pow(p.eta()-mycell->eta()           ,2.) +
			pow(acos(cos(p.phi()-mycell->phi())),2.) );
    if(deltaR < m_drcut){
      m_cells.push_back(mycell);
    }
  }

  m_cell_iter = m_cells.begin();

  return StatusCode::SUCCESS;
}

/**
 * @brief Return a pointer to the next element in the association.
 *
 * Return 0 when the association has been exhausted.
 */

const CaloCell* CaloClusterExtendedCellAssociationTool::next()
{ 
  const CaloCell* ret(0);
  if(m_cell_iter != m_cells.end()){
    ret = *m_cell_iter++;
  }
  return ret; 
} 

} // namespace D3PD
