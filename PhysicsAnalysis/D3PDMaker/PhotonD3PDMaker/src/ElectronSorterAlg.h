// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file PhotonD3PDMaker/src/ElectronSorterAlg.h
 * @author Mike Hance
 * @date Feb, 2011
 * @brief sort by pt
 */


#ifndef PHOTOND3PDMAKER_ELECTRONSORTERALG_H
#define PHOTOND3PDMAKER_ELECTRONSORTERALG_H


#include "IN4MSorterAlg.h"
#include "egammaEvent/ElectronContainer.h"


namespace D3PD {


/**
 * @brief Recalculate OQ flags for Electron objects.
 */
class ElectronSorterAlg
  : public IN4MSorterAlg<ElectronContainer>
{
public:
  /**
   * @brief Standard Gaudi algorithm constructor.
   * @param name The algorithm name.
   * @param svcloc The service locator.
   */
  ElectronSorterAlg (const std::string& name,
                     ISvcLocator* svcloc);

};


} // namespace D3PD


#endif // not PHOTOND3PDMAKER_ELECTRONSORTERALG_H
