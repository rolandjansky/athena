/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TruthD3PDMaker/src/TruthEtIsolationFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2011
 * @brief Block filler tool for truth isolation information.
 */

#ifndef TRUTHD3PDMAKER_TRUTHETISOLATIONFILLERTOOL_H
#define TRUTHD3PDMAKER_TRUTHETISOLATIONFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include "McParticleEvent/TruthParticleParamDefs.h"


namespace D3PD {

class TruthEtIsolationFillerTool : public BlockFillerTool<McAod::EtIsolations>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TruthEtIsolationFillerTool (const std::string& type,
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
  virtual StatusCode fill (const McAod::EtIsolations& p);


private:
  /// Parameter: Desired cone size.
  float m_coneSize;

  /// Index of the cone to fill.
  unsigned int m_coneIndex;

  /// Variable: Isolation transverse energy.
  float* m_etcone;
};


} // namespace D3PD


#endif // not TRUTHD3PDMAKER_TRUTHETISOLATIONFILLERTOOL_H
