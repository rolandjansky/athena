// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloD3PDMaker/CaloClusterCellAssociationTool.h
 * @author Hong Ma <hma@bnl.gov>
 * @date April, 2010
 * @brief Associate from a CaloCluster to CaloCell
 */


#ifndef CALOD3PDMAKER_CALOCLUSTERCELLASSOCIATIONTOOL_H
#define CALOD3PDMAKER_CALOCLUSTERCELLASSOCIATIONTOOL_H

#include "D3PDMakerUtils/MultiAssociationTool.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "CaloEvent/CaloCluster.h"
#include "CaloEvent/CaloCell.h"



namespace D3PD {


/**
 * @brief Associate from a CaloCluster to CaloCell
 */
class CaloClusterCellAssociationTool
  : public MultiAssociationTool<Types<CaloCluster, xAOD::CaloCluster>, CaloCell>
{
public:
  typedef MultiAssociationTool<Types<CaloCluster, xAOD::CaloCluster>, CaloCell> Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  CaloClusterCellAssociationTool (const std::string& type,
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

  virtual StatusCode reset (const xAOD::CaloCluster& p);


  /**
   * @brief Return a pointer to the next element in the association.
   *
   * Return 0 when the association has been exhausted.
   */
  virtual const  CaloCell* next();


private:
  std::vector<const CaloCell*> m_cells;
  size_t m_icell;
};

} // namespace D3PD



#endif // not EGAMMAD3PDMAKER_EGAMMACLUSTERASSOCIATIONTOOL_H
