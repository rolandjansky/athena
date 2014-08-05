// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDAnalysis/src/PhotonTruthAlg.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2009
 * @brief Analyze @c GenParticle's matching photons and make UD decorations.
 */


#ifndef EGAMMAD3PDANALYSIS_PHOTONTRUTHALG_H
#define EGAMMAD3PDANALYSIS_PHOTONTRUTHALG_H


#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"


namespace D3PD {


class PhotonTruthTool;
class ICollectionGetterTool;


/**
 * @brief Analyze @a GenParticle's matching photons and made UD decorations.
 */
class PhotonTruthAlg
  : public AthAlgorithm
{
public:
  /**
   * @brief Standard Gaudi algorithm constructor.
   * @param name The algorithm name.
   * @param svcloc The service locator.
   */
  PhotonTruthAlg (const std::string& name,
                  ISvcLocator* svcloc);


  /// Standard Gaudi @c initialize method.
  virtual StatusCode initialize();


  /// Standard Gaudi @c execute method.
  virtual StatusCode execute();


private:
  /// Property: Prefix to add to aux data items.
  std::string m_auxPrefix;

  /// Property: Truth analysis tool.
  ToolHandle<PhotonTruthTool> m_truthTool;

  /// Property: Getter for input photon objects.
  ToolHandle<ICollectionGetterTool> m_photonGetter;

  /// Property: If true, don't complain if input objects are missing.
  bool m_allowMissing;
};


} // namespace D3PD


#endif // EGAMMAD3PDANALYSIS_PHOTONTRUTHALG_H
