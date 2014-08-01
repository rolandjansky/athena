/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file MuonD3PDMaker/src/MuonL2TriggerObjectAssociationTool.h
 * @author Srivas Prasad <srivas.prasad@cern.ch>
 * @date Jan 2010
 * @brief Associate from an Analysis::Muon to matching L2 trigger object.
 */


#include "MuonL2TriggerObjectAssociationTool.h"
#include "SGTools/BaseInfo.h"


SG_ADD_BASE(CombinedMuonFeature, SG_VIRTUAL(INavigable4Momentum));


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
MuonL2TriggerObjectAssociationTool::MuonL2TriggerObjectAssociationTool
    (const std::string& type,
     const std::string& name,
     const IInterface* parent)
      : Base (type, name, parent)
{
  if (setProperty ("ChainPattern", "L2_mu.*").isFailure()) {
    REPORT_MESSAGE (MSG::ERROR) << "Can't set ChainPattern property for tool "
                                << type << "/" << name;
  }
}


} //  namespace D3PD
