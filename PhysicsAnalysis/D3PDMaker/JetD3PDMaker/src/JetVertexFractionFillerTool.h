// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file JetD3PDMaker/src/JetVertexFractionFillerTool.h
 * @author scott snyder
 * @date Feb, 2011
 * @brief Block filler tool for the Jet Vertex Fraction
 */

#ifndef JETD3PDMAKER_JETVERTEXFRACTIONFILLERTOOL_H
#define JETD3PDMAKER_JETVERTEXFRACTIONFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "GaudiKernel/ServiceHandle.h"


class Jet;
class IUserDataSvc;


namespace D3PD {


/**
 * @brief Block filler tool for the Jet Vertex Fraction
 */
class JetVertexFractionFillerTool
  : public BlockFillerTool<Jet>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  JetVertexFractionFillerTool (const std::string& type,
                               const std::string& name,
                               const IInterface* parent);
  
  /// Standard Gaudi @c initialize method.
  virtual StatusCode initialize();

  /// Book variables for this block.
  virtual StatusCode book();


  /**
   * @brief Fill one block --- type-safe version.
   * @param p The input object.
   *
   * This is called once per object.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   */
  virtual StatusCode fill (const Jet& p);
  
  
private:
  /// Property: The UserDataSvc.
  ServiceHandle<IUserDataSvc> m_userDataSvc;

  /// Property: Take the JVF from UserData?
  ///           (Note: this is always the case if full JVF info is requested.)
  bool m_fromUD;

  /// Property: Fill JVF info for all vertices?
  //            (Note: JVF is always taken from UD in this case.)
  bool m_fillFullJVF;

  /// Property: Prefix to add to UD labels.
  std::string m_udprefix;

  /// Variable: jet vertex fraction (for first vertex).
  float* m_jvtxf;

  /// Verable: Full jet vertex fractions for all the vertices.
  std::vector<float>* m_fullJVF;
};


} // namespace D3PD


#endif // not JETD3PDMAKER_JETVERTEXFRACTIONFILLERTOOL_H
