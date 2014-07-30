// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file EventCommonD3PDMaker/src/ChargeFillerTool.h
 * @author Serhan Mete, scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Block filler tool for @c IParticle::charge().
 */

#ifndef EVENTCOMMOND3PDMAKER_CHARGEFILLERTOOL_H
#define EVENTCOMMOND3PDMAKER_CHARGEFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
class IParticle;


namespace D3PD {


/**
 * @brief Block filler tool for @c IParticle::charge().
 */
class ChargeFillerTool
  : public BlockFillerTool<IParticle>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  ChargeFillerTool (const std::string& type,
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
  virtual StatusCode fill (const IParticle& p);


private:
  /// Variable: Particle charge.
  float* m_charge;
  
};


} // namespace D3PD


#endif // not EVENTCOMMOND3PDMAKER_CHARGEFILLERTOOL_H
