/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigTauD3PDMaker/src/TrigEFTauObjectDecisionFillerTool.cxx
 * @author E. Ptacek <eptacek@uoregon.edu>
 * @date Apr, 2010
 * @brief Record which triggers are passed by a EF tau object.
 */


#include "TrigEFTauObjectDecisionFillerTool.h"
#include "tauEvent/TauJet.h"
#include "tauEvent/TauJetContainer.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TrigEFTauObjectDecisionFillerTool::TrigEFTauObjectDecisionFillerTool
   (const std::string& type,
   const std::string& name,
   const IInterface* parent)
     : TriggerObjectDecisionFillerTool<Analysis::TauJet> (type, name, parent)
{
}


} // namespace D3PD
