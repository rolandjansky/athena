/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDMaker/src/xAODElectronObjectDecisionFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2010
 * @brief Record which triggers are passed by an Electron HLT object.
 */


#include "xAODElectronObjectDecisionFillerTool.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
xAODElectronObjectDecisionFillerTool::xAODElectronObjectDecisionFillerTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : TriggerObjectDecisionFillerTool<xAOD::Electron> (type, name, parent)
{
}


} // namespace D3PD
