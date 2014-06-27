/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file PhotonD3PDMaker/src/PhotonSorterAlg.cxx
 * @author Mike Hance
 * @date Feb, 2010
 * @brief sorting by pt
 */


#include "PhotonSorterAlg.h"


namespace D3PD {


/**
 * @brief Standard Gaudi algorithm constructor.
 * @param name The algorithm name.
 * @param svcloc The service locator.
 */
PhotonSorterAlg::PhotonSorterAlg (const std::string& name,
                                      ISvcLocator* svcloc)
  : IN4MSorterAlg<PhotonContainer> (name, svcloc)
{
  declareProperty("RemoveTopoClusterCandidates",   m_remove_TopoCluster_candidates);
}

/**
 * @brief Store the element in the output list
 */
StatusCode PhotonSorterAlg::addElement (const INavigable4Momentum* ptr)
{

  // apply any filters
  if(m_remove_TopoCluster_candidates){
    const Analysis::Photon* phot = dynamic_cast<const Analysis::Photon*>(ptr);
    if(phot && phot->author() == 128) return StatusCode::SUCCESS;
  }

  return IN4MSorterAlg<PhotonContainer>::addElement(ptr);
}

} // namespace D3PD
