// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TauD3PDMaker/src/TauEFTriggerObjectAssociationTool.cxx
 * @date Nov, 2009
 * @brief Associate from an Tau to its matching EF trigger object.
 */


#include "TauEFTriggerObjectAssociationTool.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TauEFTriggerObjectAssociationTool::TauEFTriggerObjectAssociationTool
    (const std::string& type,
     const std::string& name,
     const IInterface* parent)
      : Base (type, name, parent)
{}


} //  namespace D3PD
