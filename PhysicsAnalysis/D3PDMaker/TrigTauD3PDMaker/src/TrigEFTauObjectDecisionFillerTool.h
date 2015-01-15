// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigTauD3PDMaker/src/TrigEFTauObjectDecisionFillerTool.h
 * @author E. Ptacek <eptacek@uoregon.edu>
 * @date Apr, 2010
 * @brief Record which triggers are passed by a EF tau object.
 */


#ifndef TRIGTAUD3PDMAKER_TRIGEFTAUOBJECTDECISIONFILLERTOOL_H
#define TRIGTAUD3PDMAKER_TRIGEFTAUOBJECTDECISIONFILLERTOOL_H


#include "TriggerD3PDMaker/TriggerObjectDecisionFillerTool.h"
namespace Analysis { class TauJet; }


namespace D3PD {


/**
 * @brief Record which triggers are passed by a EF tau object.
 */
class TrigEFTauObjectDecisionFillerTool
  : public TriggerObjectDecisionFillerTool<Analysis::TauJet>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TrigEFTauObjectDecisionFillerTool (const std::string& type,
				     const std::string& name,
				     const IInterface* parent);
};


} // namespace D3PD


#endif // not TRIGTAUD3PDMAKER_TRIGEFTAUOBJECTDECISIONFILLERTOOL_H
