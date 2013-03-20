// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MissingETD3PDMaker/src/MissingETCaloFillerTool.h
 * @author jet goodson <jgoodson@cern.ch>
 * @date 24 Sep, 2009
 * @brief Block filler tool for Missing ET Calo objects.
 */


#ifndef MISSINGETD3PPDMAKER_MISSINGETCALOSAMPLINGFILLERTOOL_H
#define MISSINGETD3PPDMAKER_MISSINGETCALOSAMPLINGFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include "MissingETEvent/MissingEtCalo.h"


namespace D3PD {


/**
 * @brief Block filler tool for Missing ET objects.
 */
  class MissingETCaloSamplingFillerTool
    : public BlockFillerTool<MissingEtCalo>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  MissingETCaloSamplingFillerTool (const std::string& type,
                       const std::string& name,
                       const IInterface* parent);


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
  virtual StatusCode fill (const MissingEtCalo& p);


private:
  struct Vars {
    float* etx;
    float* ety;
    float* sumet;
    float* phi;
    float* nCell;
  };
 
  Vars m_vars[MissingEtCalo::Size];
};


} // namespace D3PD


#endif // not MISSINGETD3PPDMAKER_MISSINGETCALOSAMPLINGFILLERTOOL_H
