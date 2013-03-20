/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigTauD3PDMaker/src/TrigL2TauObjectDecisionFillerTool.cxx
 * @author E. Ptacek <eptacek@uoregon.edu>
 * @date Apr, 2010
 * @brief Record which triggers are passed by a L2 tau object.
 */


#include "TrigL2TauObjectDecisionFillerTool.h"
#include "TrigParticle/TrigTau.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TrigL2TauObjectDecisionFillerTool::TrigL2TauObjectDecisionFillerTool
   (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : TriggerObjectDecisionFillerTool<TrigTau> (type, name, parent)
{
}


} // namespace D3PD
