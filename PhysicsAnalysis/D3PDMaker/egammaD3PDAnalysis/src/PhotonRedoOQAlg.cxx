/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDAnalysis/src/PhotonRedoOQAlg.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2010
 * @brief Recalculate OQ flags for Photon objects.
 */


#include "PhotonRedoOQAlg.h"


namespace D3PD {


/**
 * @brief Standard Gaudi algorithm constructor.
 * @param name The algorithm name.
 * @param svcloc The service locator.
 */
PhotonRedoOQAlg::PhotonRedoOQAlg (const std::string& name,
                                      ISvcLocator* svcloc)
  : egammaRedoOQAlg<xAOD::PhotonContainer, xAOD::PhotonAuxContainer> (name, svcloc)
{
}


} // namespace D3PD
