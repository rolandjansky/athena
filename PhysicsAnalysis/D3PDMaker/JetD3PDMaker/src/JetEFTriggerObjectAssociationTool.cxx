// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file JetD3PDMaker/src/JetEFTriggerObjectAssociationTool.cxx
 * @author Haifeng Li <Haifeng.Li@cern.ch>
 * @date Oct, 2010
 * @brief Associate from an Jet to its matching EF trigger object.
 */


#include "JetEFTriggerObjectAssociationTool.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
JetEFTriggerObjectAssociationTool::JetEFTriggerObjectAssociationTool
    (const std::string& type,
     const std::string& name,
     const IInterface* parent)
      : Base (type, name, parent)
{
}


} //  namespace D3PD
