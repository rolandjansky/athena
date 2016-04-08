// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MissingETD3PDMaker/src/MissingETContainerFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2014
 * @brief Simple filler for xAOD missing et.
 */


#ifndef MISSINGETD3PDMAKER_MISSINGETCONTAINERFILLERTOOL_H
#define MISSINGETD3PDMAKER_MISSINGETCONTAINERFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include "xAODMissingET/MissingETContainer.h"
#include "GaudiKernel/ToolHandle.h"
#include <unordered_map>


namespace D3PD {


class IObjGetterTool;


class MissingETContainerFillerTool
  : public BlockFillerTool<xAOD::MissingETContainer>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  MissingETContainerFillerTool (const std::string& type,
                                const std::string& name,
                                const IInterface* parent);


  /// @brief Standard Gaudi initialize method.
  virtual StatusCode initialize() override;


  /// @brief Book variables for this block.
  virtual StatusCode book() override;


  /**
   * @brief Fill one block --- type-safe version.
   * @param p The input object.
   *
   * This is called once per object.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   */
  virtual StatusCode fill (const xAOD::MissingETContainer& p) override;


private:
  /// Variables to fill for a single @c MissingET instance.
  struct Vars
  {
    float* m_etx;
    float* m_ety;
    float* m_sumet;
  };

  /// Map from MET component name to variables.
  std::unordered_map<std::string, Vars> m_vars;


  /// Property: Getter for the MissingETContainer object from which
  ///           to find the variables to fill.
  ToolHandle<IObjGetterTool> m_getter;
};


} // namespace D3PD



#endif // not MISSINGETD3PDMAKER_MISSINGETCONTAINERFILLERTOOL_H
