// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDMaker/src/egammaClusterAssociationTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Associate from an egamma to its CaloCluster.
 */


#ifndef EGAMMAD3PDMAKER_EGAMMACLUSTERASSOCIATIONTOOL_H
#define EGAMMAD3PDMAKER_EGAMMACLUSTERASSOCIATIONTOOL_H


#include "D3PDMakerUtils/SingleAssociationTool.h"
#include "xAODEgamma/Egamma.h"
#include "xAODCaloEvent/CaloCluster.h"


namespace D3PD {


/**
 * @brief Associate from an egamma to its CaloCluster.
 */
class egammaClusterAssociationTool
  : public SingleAssociationTool<xAOD::Egamma, xAOD::CaloCluster>
{
public:
  typedef SingleAssociationTool<xAOD::Egamma, xAOD::CaloCluster> Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  egammaClusterAssociationTool (const std::string& type,
                                const std::string& name,
                                const IInterface* parent);


  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */
  virtual const xAOD::CaloCluster* get (const xAOD::Egamma& p);
};


} // namespace D3PD



#endif // not EGAMMAD3PDMAKER_EGAMMACLUSTERASSOCIATIONTOOL_H
