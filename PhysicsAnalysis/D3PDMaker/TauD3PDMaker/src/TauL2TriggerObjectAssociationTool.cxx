// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TauD3PDMaker/src/TauL2TriggerObjectAssociationTool.cxx
 * @date Nov, 2009
 * @brief Associate from an Tau to its matching L2 trigger object.
 */


#include "TauL2TriggerObjectAssociationTool.h"
#include "SGTools/BaseInfo.h"


SG_ADD_BASE(TrigTau, SG_VIRTUAL(INavigable4Momentum));


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TauL2TriggerObjectAssociationTool::TauL2TriggerObjectAssociationTool
    (const std::string& type,
     const std::string& name,
     const IInterface* parent)
      : Base (type, name, parent)
{
  if (setProperty ("ChainPattern", "L2_tau.*").isFailure()) {
    REPORT_MESSAGE (MSG::ERROR) << "Can't set ChainPattern property for tool "
                                << type << "/" << name;
  }
}


} //  namespace D3PD
