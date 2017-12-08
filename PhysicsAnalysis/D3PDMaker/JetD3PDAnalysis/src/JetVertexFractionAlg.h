// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file JetD3PDAnalysis/src/JetVertexFraction.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2011
 * @brief Store in UserData the JVF vector for a jet.
 */


#ifndef JETD3PDANALYSIS_JETVERTEXFRACTIONALG_H
#define JETD3PDANALYSIS_JETVERTEXFRACTIONALG_H


#include "D3PDMakerUtils/UDAlgBase.h"
#include "GaudiKernel/ToolHandle.h"
#include <string>


class JetVertexAssociationTool;


namespace D3PD {


class ICollectionGetterTool;


/**
 * @brief Store in UserData the JVF vector for a jet.
 */
class JetVertexFractionAlg
  : public UDAlgBase
{
public:
  /**
   * @brief Standard Gaudi algorithm constructor.
   * @param name The algorithm name.
   * @param svcloc The service locator.
   */
  JetVertexFractionAlg (const std::string& name,
                        ISvcLocator* svcloc);


  /// Standard Gaudi @c initialize method.
  virtual StatusCode initialize();


  /// Standard Gaudi @c execute method.
  virtual StatusCode execute();


private:
  /// Property: Getter for input jet objects.
  ToolHandle<ICollectionGetterTool> m_getter;

  /// Property: Tool to calculate the JVF.
  ToolHandle<JetVertexAssociationTool> m_jvfTool;
};


} // namespace D3PD


#endif // not JETD3PDANALYSIS_JETVERTEXFRACTIONALG_H
