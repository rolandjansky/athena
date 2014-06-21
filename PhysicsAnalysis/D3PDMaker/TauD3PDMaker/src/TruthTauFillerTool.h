/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TauD3PDMaker/src/TruthTauFillerTool.h
 * @author Ryan Reece <ryan.reece@cern.ch>
 * @date December, 2009
 * @brief Block filler tool for true taujet variables.
 */

#ifndef TAUD3PDMAKER_TRUTHTAUFILLERTOOL_H
#define TAUD3PDMAKER_TRUTHTAUFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include "TauTools/TruthHandler.h"

class TruthParticle;

namespace D3PD {


/**
 * @brief Block filler tool for taujet.
 *
 */
class TruthTauFillerTool : public BlockFillerTool<TruthParticle>
{
public:
  typedef BlockFillerTool<TruthParticle> Base;

  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TruthTauFillerTool (const std::string& type,
                      const std::string& name,
                      const IInterface* parent);

  StatusCode initialize();

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
  virtual StatusCode fill (const TruthParticle& p);


    private:
        // Variables from each stored
        float* m_vis_m;
        float* m_vis_Et;
        float* m_vis_eta;
        float* m_vis_phi;
        int* m_nProng;
        int* m_nPi0;
        float* m_charge;

        // other private
        TruthHandler m_truth_handler;
};


} // namespace D3PD


#endif // not TAUD3PDMAKER_TRUTHTAUFILLERTOOL_H
