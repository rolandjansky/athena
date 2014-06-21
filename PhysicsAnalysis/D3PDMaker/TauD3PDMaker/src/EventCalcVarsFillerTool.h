// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TauD3PDMaker/src/EventCalcVarsFillerTool.h
 * @author Michel Trottier-McDonald <mtm@cern.ch>
 * @date December, 2010
 * @brief Block filler tool for event-based calculated variables. These
 * are variables which are needed for MV discrimination
 * but are not (yet?) in EDM.
 */

#ifndef TAUD3PDMAKER_EVENTCALCVARSFILLERTOOL_H
#define TAUD3PDMAKER_EVENTCALCVARSFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include "TauDiscriminant/TauDetailsManager.h"

namespace D3PD {

/**
 * @brief Block filler tool for taujet.
 *
 */
class EventCalcVarsFillerTool
  : public BlockFillerTool<void>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  EventCalcVarsFillerTool (const std::string& type,
                     const std::string& name,
                     const IInterface* parent);


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
  virtual StatusCode fill ();


private:
  TauDetailsManager* m_detailsManager;  //!< A pointer to the @c TauDetailsManager.

  int*   m_numGoodVertices;
};


} // namespace D3PD


#endif // not TAUD3PDMAKER_EVENTCALCVARSFILLERTOOL_H
