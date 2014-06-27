/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file PhotonD3PDMaker/src/ElectronSorterAlg.cxx
 * @author Mike Hance
 * @date Feb, 2010
 * @brief sorting by pt
 */


#include "ElectronSorterAlg.h"


namespace D3PD {


/**
 * @brief Standard Gaudi algorithm constructor.
 * @param name The algorithm name.
 * @param svcloc The service locator.
 */
ElectronSorterAlg::ElectronSorterAlg (const std::string& name,
                                      ISvcLocator* svcloc)
  : IN4MSorterAlg<ElectronContainer> (name, svcloc)
{
}

} // namespace D3PD
