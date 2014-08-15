// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloClusterExtendedCellAssociationTool.h 533711 2013-01-18 16:33:30Z mhance $
/**
 * @file CaloD3PDMaker/CaloClusterExtendedCellAssociationTool.h
 * @author Mike Hance <mike.hance@cern.ch>
 * @date January, 2013
 * @brief Associate from a CaloCluster to CaloCells in a 7x11 area around centroid
 */


#ifndef CALOD3PDMAKER_CALOCLUSTEREXTENDEDCELLASSOCIATIONTOOL_H
#define CALOD3PDMAKER_CALOCLUSTEREXTENDEDCELLASSOCIATIONTOOL_H

#include "D3PDMakerUtils/MultiAssociationTool.h"
#include "CaloEvent/CaloCluster.h"
#include "CaloEvent/CaloCell.h"
#include "D3PDMakerInterfaces/ICollectionGetterTool.h"
#include "GaudiKernel/ToolHandle.h"



namespace D3PD {


/**
 * @brief Associate from an egamma to its CaloCluster.
 */
class CaloClusterExtendedCellAssociationTool
  : public MultiAssociationTool<CaloCluster,CaloCell>
{
public:
  typedef MultiAssociationTool<CaloCluster,CaloCell> Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  CaloClusterExtendedCellAssociationTool (const std::string& type,
					  const std::string& name,
					  const IInterface* parent);


  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */

  virtual StatusCode reset (const CaloCluster& p);

  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */

  virtual StatusCode initialize ();

  /**
   * @brief Return a pointer to the next element in the association.
   *
   * Return 0 when the association has been exhausted.
   */
  virtual const  CaloCell* next();


private:

  ToolHandle<ICollectionGetterTool> m_getter;

  std::vector<const CaloCell*> m_cells;
  std::vector<const CaloCell*>::iterator m_cell_iter;

  float m_drcut;
  int m_phirange;
  int m_etarange;

};

} // namespace D3PD



#endif // not EGAMMAD3PDMAKER_EGAMMACLUSTERASSOCIATIONTOOL_H
