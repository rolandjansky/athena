// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDAnalysis/src/egammaDeltaEmax2.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2009
 * @brief Calculate deltaEmax2 for egamma objects and store as UD.
 */


#ifndef EGAMMAD3PDANALYSIS_EGAMMADELTAEMAX2_H
#define EGAMMAD3PDANALYSIS_EGAMMADELTAEMAX2_H


#include "AthenaBaseComps/AthAlgorithm.h"
#include "xAODEgamma/Egamma.h"
#include "GaudiKernel/ToolHandle.h"


namespace D3PD {


class ICollectionGetterTool;


/**
 * @brief Calculate deltaEmax2 for egamma objects and store as UD.
 */
class egammaDeltaEmax2Alg
  : public AthAlgorithm
{
public:
  /**
   * @brief Standard Gaudi algorithm constructor.
   * @param name The algorithm name.
   * @param svcloc The service locator.
   */
  egammaDeltaEmax2Alg (const std::string& name,
                       ISvcLocator* svcloc);


  /// Standard Gaudi @c initialize method.
  virtual StatusCode initialize() override;


  /// Standard Gaudi @c execute method.
  virtual StatusCode execute() override;


private:
  /// Property: Prefix to add to aux data items.
  std::string m_auxPrefix;

  /// Property: Getter for input photon objects.
  ToolHandle<ICollectionGetterTool> m_getter;

  /// Property: If true, don't complain if input objects are missing.
  bool m_allowMissing;

  /// Property: True if we should use the high-lum definition of deltaEmax2.
  bool m_highLum;
};


} // namespace D3PD


#endif // EGAMMAD3PDANALYSIS_EGAMMADELTAEMAX2_H
