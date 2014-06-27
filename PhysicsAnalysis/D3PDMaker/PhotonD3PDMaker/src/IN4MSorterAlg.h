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


#ifndef PHOTOND3PDMAKER_IN4MSORTERALG_H
#define PHOTOND3PDMAKER_IN4MSORTERALG_H


#include "AthenaBaseComps/AthAlgorithm.h"
#include "D3PDMakerInterfaces/ICollectionGetterTool.h"
#include "GaudiKernel/ToolHandle.h"
#include <string>


class INavigable4Momentum;

namespace D3PD {


class ICollectionGetterTool;

class IN4MEtSort{
public:
  inline IN4MEtSort () {};
  bool operator() (const INavigable4Momentum* a,
		   const INavigable4Momentum* b);
};


/**
 * @brief Helper, to factor out non-template dependent code.
 *        Don't use this directly.
 */
class IN4MSorterAlgBase
  : public AthAlgorithm
{
public:
  /**
   * @brief Standard Gaudi algorithm constructor.
   * @param name The algorithm name.
   * @param svcloc The service locator.
   */
  IN4MSorterAlgBase (const std::string& name,
		     ISvcLocator* svcloc);


  /// Standard Gaudi initialize method.
  StatusCode initialize();


  /// Standard Gaudi execute method.
  StatusCode execute();

protected:
  /// Store the output list in SG.
  virtual StatusCode commit () = 0;


  /// Property: SG key for output container.
  std::string m_outputkey;

  /// Store the element in the output list
  virtual StatusCode addElement(const INavigable4Momentum* ptr) = 0;

private:
  /// Property: Getter for input egamma objects.
  ToolHandle<ICollectionGetterTool> m_getter;
};


/**
 * @brief Recalculate OQ flags for Electron objects.
 *        This is a template class, and can't be used directly as an Algorithm.
 *        Derive a non-templated class from this to actually use.
 */
template <typename CONT>
class IN4MSorterAlg
  : public IN4MSorterAlgBase
{
public:
  /**
   * @brief Standard Gaudi algorithm constructor.
   * @param name The algorithm name.
   * @param svcloc The service locator.
   */
  IN4MSorterAlg (const std::string& name,
                   ISvcLocator* svcloc);


protected:
  typedef typename CONT::base_value_type Element;


  /// Store the output list in SG.
  virtual StatusCode commit ();

  /// Store the element in the output list
  virtual StatusCode addElement(const INavigable4Momentum* ptr);

private:
  /// The output list being accumulated.
  CONT* m_outlist;
};

}


#include "IN4MSorterAlg.icc"

#endif // not PHOTOND3PDMAKER_IN4MSORTERALG_H
