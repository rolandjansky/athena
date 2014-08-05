// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: GenParticleBremFillerTool.h 348274 2011-02-28 16:25:06Z krasznaa $
/**
 * @file EventCommonD3PDMaker/src/GenParticleBremFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2009
 * @brief Fill the hasHardBrem flag.
 */

#ifndef EVENTCOMMOND3PDMAKER_TRUTHPARTICLEBREMFILLERTOOL_H
#define EVENTCOMMOND3PDMAKER_TRUTHPARTICLEBREMFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include "xAODTruth/TruthParticle.h"


namespace D3PD {


/**
 * @brief Fill the hasHardBrem flag.
 */
class TruthParticleBremFillerTool
  : public BlockFillerTool<xAOD::TruthParticle>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TruthParticleBremFillerTool (const std::string& type,
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
  virtual StatusCode fill (const xAOD::TruthParticle& p);


private:
  /// Variable: Does this particle have a brem?
  bool* m_hasHardBrem;
};


} // namespace D3PD


#endif // not EVENTCOMMOND3PDMAKER_TRUTHPARTICLEBREMFILLERTOOL_H
