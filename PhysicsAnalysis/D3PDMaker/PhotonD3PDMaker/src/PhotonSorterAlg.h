// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file PhotonD3PDMaker/src/PhotonSorterAlg.h
 * @author Mike Hance
 * @date Feb, 2011
 * @brief sort by pt
 */


#ifndef PHOTOND3PDMAKER_PHOTONSORTERALG_H
#define PHOTOND3PDMAKER_PHOTONSORTERALG_H


#include "IN4MSorterAlg.h"
#include "egammaEvent/PhotonContainer.h"


namespace D3PD {


/**
 * @brief Recalculate OQ flags for Photon objects.
 */
class PhotonSorterAlg
  : public IN4MSorterAlg<PhotonContainer>
{
public:
  /**
   * @brief Standard Gaudi algorithm constructor.
   * @param name The algorithm name.
   * @param svcloc The service locator.
   */
  PhotonSorterAlg (const std::string& name,
                     ISvcLocator* svcloc);

  virtual StatusCode addElement(const INavigable4Momentum* ptr);

private:
  
  bool m_remove_TopoCluster_candidates;
};


} // namespace D3PD


#endif // not PHOTOND3PDMAKER_PHOTONSORTERALG_H
