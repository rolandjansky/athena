// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDAnalysis/src/ElectronRedoOQAlg.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2010
 * @brief Recalculate OQ flags for Electron objects.
 */


#ifndef EGAMMAD3PDANALYSIS_ELECTRONREDOOQALG_H
#define EGAMMAD3PDANALYSIS_ELECTRONREDOOQALG_H


#include "egammaRedoOQAlg.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/ElectronAuxContainer.h"


namespace D3PD {


/**
 * @brief Recalculate OQ flags for Electron objects.
 */
class ElectronRedoOQAlg
  : public egammaRedoOQAlg<xAOD::ElectronContainer, xAOD::ElectronAuxContainer>
{
public:
  /**
   * @brief Standard Gaudi algorithm constructor.
   * @param name The algorithm name.
   * @param svcloc The service locator.
   */
  ElectronRedoOQAlg (const std::string& name,
                     ISvcLocator* svcloc);
};


} // namespace D3PD


#endif // not EGAMMAD3PDANALYSIS_ELECTRONREDOOQALG_H
