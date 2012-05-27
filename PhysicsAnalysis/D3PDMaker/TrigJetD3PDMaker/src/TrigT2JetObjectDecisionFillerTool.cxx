/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigT2JetObjectDecisionFillerTool.cxx 307763 2010-06-29 14:25:58Z krasznaa $
/**
 * @file TrigEgammaD3PDMaker/src/TrigT2JetObjectDecisionFillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2010
 * @brief Record which triggers are passed by a L2 jet object.
 */


#include "TrigT2JetObjectDecisionFillerTool.h"
#include "TrigCaloEvent/TrigT2Jet.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TrigT2JetObjectDecisionFillerTool::TrigT2JetObjectDecisionFillerTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : TriggerObjectDecisionFillerTool<TrigT2Jet> (type, name, parent)
{
}


} // namespace D3PD
