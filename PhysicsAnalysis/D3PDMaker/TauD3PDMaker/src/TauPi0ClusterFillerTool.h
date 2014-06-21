// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TauD3PDMaker/src/TauPi0ClusterFillerTool.h
 * @author Dugan O'Neil <doneil@sfu.ca>
 * @author Ryan Reece <ryan.reece@cern.ch>
 * @date September, 2009
 * @brief Block filler tool for vector sum of pi-0 clusters.
 */

#ifndef TAUD3PDMAKER_TAUSUMPI0FILLERTOOL_H
#define TAUD3PDMAKER_TAUSUMPI0FILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
namespace Analysis { class TauJet; }


namespace D3PD {


/**
 * @brief Block filler tool for taujet.
 *
 */
class TauPi0ClusterFillerTool
  : public BlockFillerTool<Analysis::TauJet>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TauPi0ClusterFillerTool (const std::string& type,
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
  virtual StatusCode fill (const Analysis::TauJet& p);


private:
  /// basic 4-vector info. Multiple pi-0 per tau possible.
  std::vector<float>* m_pt;
  std::vector<float>* m_eta;
  std::vector<float>* m_phi;

};


} // namespace D3PD


#endif // not TAUD3PDMAKER_TAUJETFILLERTOOL_H
