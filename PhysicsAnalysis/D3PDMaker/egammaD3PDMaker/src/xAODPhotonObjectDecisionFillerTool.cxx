/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDMaker/src/xAODPhotonObjectDecisionFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2010
 * @brief Record which triggers are passed by an Photon HLT object.
 */


#include "xAODPhotonObjectDecisionFillerTool.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
xAODPhotonObjectDecisionFillerTool::xAODPhotonObjectDecisionFillerTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : TriggerObjectDecisionFillerTool<xAOD::Photon> (type, name, parent)
{
}


} // namespace D3PD
