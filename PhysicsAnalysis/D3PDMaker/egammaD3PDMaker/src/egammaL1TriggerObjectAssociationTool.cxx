// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDMaker/src/egammaL1TriggerObjectAssociationTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Associate from an egamma to its matching L1 trigger object.
 */


#include "egammaL1TriggerObjectAssociationTool.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
egammaL1TriggerObjectAssociationTool::egammaL1TriggerObjectAssociationTool
    (const std::string& type,
     const std::string& name,
     const IInterface* parent)
      : Base (type, name, parent)
{
  if (setProperty ("ChainPattern", "L1_EM.*").isFailure()) {
    REPORT_MESSAGE (MSG::ERROR) << "Can't set ChainPattern property for tool "
                                << type << "/" << name;
  }
}


} //  namespace D3PD
