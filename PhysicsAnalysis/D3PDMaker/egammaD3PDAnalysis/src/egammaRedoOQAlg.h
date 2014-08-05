// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDAnalysis/src/egammaRedoOQAlg.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2010
 * @brief Recalculate OQ flags for Electron objects.
 *        This is a template class, and can't be used directly as an Algorithm.
 *        Derive a non-templated class from this to actually use.
 */


#ifndef EGAMMAD3PDANALYSIS_EGAMMAREDOOQALG_H
#define EGAMMAD3PDANALYSIS_EGAMMAREDOOQALG_H


#include "AthenaBaseComps/AthAlgorithm.h"
#include "D3PDMakerInterfaces/ICollectionGetterTool.h"
#include "egammaInterfaces/IegammaBaseTool.h"
#include "xAODEgamma/Egamma.h"
#include "GaudiKernel/ToolHandle.h"
#include <string>


namespace D3PD {


class ICollectionGetterTool;


/**
 * @brief Helper, to factor out non-template dependent code.
 *        Don't use this directly.
 */
class egammaRedoOQAlgBase
  : public AthAlgorithm
{
public:
  /**
   * @brief Standard Gaudi algorithm constructor.
   * @param name The algorithm name.
   * @param svcloc The service locator.
   */
  egammaRedoOQAlgBase (const std::string& name,
                       ISvcLocator* svcloc);


  /// Standard Gaudi initialize method.
  StatusCode initialize();


  /// Standard Gaudi execute method.
  StatusCode execute();


protected:
  /// Clone EG and add it to the output list.
  virtual xAOD::Egamma* cloneEG (const xAOD::Egamma* eg) = 0;

  /// Store the output list in SG.
  virtual StatusCode commit () = 0;


  /// Property: SG key for output container.
  std::string m_outputkey;


private:
  /// Property: Getter for input egamma objects.
  ToolHandle<ICollectionGetterTool> m_getter;

  /// Property: Tool to recalculate OQ flags.
  ToolHandle<IegammaBaseTool> m_egammaOQFlagsBuilder;
};


/**
 * @brief Recalculate OQ flags for Electron objects.
 *        This is a template class, and can't be used directly as an Algorithm.
 *        Derive a non-templated class from this to actually use.
 */
template <typename CONT, typename AUXSTORE>
class egammaRedoOQAlg
  : public egammaRedoOQAlgBase
{
public:
  /**
   * @brief Standard Gaudi algorithm constructor.
   * @param name The algorithm name.
   * @param svcloc The service locator.
   */
  egammaRedoOQAlg (const std::string& name,
                   ISvcLocator* svcloc);


protected:
  typedef typename CONT::base_value_type Element;

  /// Create the container if we haven't done so yet.
  void checkCont();

  /// Clone EG and add it to the output list.
  virtual xAOD::Egamma* cloneEG (const xAOD::Egamma* eg);

  /// Store the output list in SG.
  virtual StatusCode commit ();


private:
  /// The output list being accumulated.
  CONT* m_outlist;

  /// The corresponding aux store.
  AUXSTORE* m_auxstore;
};


} // namespace D3PD


#include "egammaRedoOQAlg.icc"


#endif // not EGAMMAD3PDANALYSIS_EGAMMAREDOOQALG_H
