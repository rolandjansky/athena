/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigEgammaD3PDMaker/src/TrigPhotonObjectDecisionFillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2010
 * @brief Record which triggers are passed by a L2 photon object.
 */


#include "TrigPhotonObjectDecisionFillerTool.h"
#include "TrigParticle/TrigPhoton.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TrigPhotonObjectDecisionFillerTool::TrigPhotonObjectDecisionFillerTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : TriggerObjectDecisionFillerTool<TrigPhoton> (type, name, parent)
{
}


} // namespace D3PD
