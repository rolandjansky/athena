/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDAnalysis/src/ElectronRedoOQAlg.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2010
 * @brief Recalculate OQ flags for Electron objects.
 */


#include "ElectronRedoOQAlg.h"


namespace D3PD {


/**
 * @brief Standard Gaudi algorithm constructor.
 * @param name The algorithm name.
 * @param svcloc The service locator.
 */
ElectronRedoOQAlg::ElectronRedoOQAlg (const std::string& name,
                                      ISvcLocator* svcloc)
  : egammaRedoOQAlg<xAOD::ElectronContainer, xAOD::ElectronAuxContainer> (name, svcloc)
{
}


} // namespace D3PD
