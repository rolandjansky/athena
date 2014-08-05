// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDAnalysis/src/PhotonRedoOQAlg.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2010
 * @brief Recalculate OQ flags for Photon objects.
 */


#ifndef EGAMMAD3PDANALYSIS_PHOTONREDOOQALG_H
#define EGAMMAD3PDANALYSIS_PHOTONREDOOQALG_H


#include "egammaRedoOQAlg.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/PhotonAuxContainer.h"


namespace D3PD {


/**
 * @brief Recalculate OQ flags for Photon objects.
 */
class PhotonRedoOQAlg
  : public egammaRedoOQAlg<xAOD::PhotonContainer, xAOD::PhotonAuxContainer>
{
public:
  /**
   * @brief Standard Gaudi algorithm constructor.
   * @param name The algorithm name.
   * @param svcloc The service locator.
   */
  PhotonRedoOQAlg (const std::string& name,
                   ISvcLocator* svcloc);
};


} // namespace D3PD


#endif // not EGAMMAD3PDANALYSIS_PHOTONREDOOQALG_H
