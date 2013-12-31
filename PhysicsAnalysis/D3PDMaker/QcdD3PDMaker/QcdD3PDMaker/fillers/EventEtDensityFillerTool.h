// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file EventEtDensityFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2012
 * @brief 
 */


#ifndef EVENTETDENSITYFILLERTOOL_H
#define EVENTETDENSITYFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h" 
#include "GaudiKernel/ToolHandle.h"
class IEventEtDensityProvider;


namespace QcdD3PD {


class EventEtDensityFillerTool
  : public D3PD::BlockFillerTool<void>
{
 public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
    EventEtDensityFillerTool (const std::string& type,
                              const std::string& name,
                              const IInterface* parent);


  /// Standard Gaudi initialize method.
  virtual StatusCode initialize ();


  /**
   * @brief Book variables for this block.
   */
  virtual StatusCode book ();


  /**
   * @brief Fill one block --- type-safe version.
   * @param p The input object.
   */
  virtual StatusCode fill ();


private:
  /// Property: Event density tool.
  ToolHandle<IEventEtDensityProvider> m_tool;

  /// Property: Suffix for variable names.
  std::string m_variableSuffix;


  /// Variable: The event Et density.
  float *m_rho;
};


} // namespace QcdD3PD


#endif // not EVENTETDENSITYFILLERTOOL_H
