/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TruthD3PDMaker/src/JetFullTruthTag.h
 * @author Zach Marshall <zach.marshall@cern.ch>
 * @date June, 2013
 * @brief Provides significant info for truth jets.
   Fakes the filling of reco-like branches
 */

#ifndef TRUTHD3PDMAKER_JETFULLTRUTHTAG_H
#define TRUTHD3PDMAKER_JETFULLTRUTHTAG_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include "JetEvent/Jet.h"
#include <vector>


namespace D3PD {

class JetFullTruthTag : public BlockFillerTool<Jet>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  JetFullTruthTag (const std::string& type,
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
  virtual StatusCode fill (const Jet& p);

private:
  float m_hadronMatch_dr,m_partonMatch_dr;
  float m_min_hadron_pt;
  float m_min_parton_pt;
  std::string m_truthCollectionName;

  int *m_partonFlavor;
  float *m_partonDR;
  int *m_hadronFlavor;
  int *m_hadronPDGID;

};


} // namespace D3PD


#endif // not TRUTHD3PDMAKER_JETFULLTRUTHTAG_H
