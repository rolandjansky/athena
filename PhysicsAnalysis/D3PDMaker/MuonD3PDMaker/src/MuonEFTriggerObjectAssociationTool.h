/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file MuonD3PDMaker/src/MuonEFTriggerObjectAssociationTool.h
 * @author Srivas Prasad <srivas.prasad@cern.ch>
 * @date Jan 2010
 * @brief Associate from an Analysis::Muon to matching Event Filter trigger object.
 */

#ifndef MUOND3PDMAKER_MUONEFTRIGGEROBJECTASSOCIATIONTOOL_H
#define MUOND3PDMAKER_MUONEFTRIGGEROBJECTASSOCIATIONTOOL_H 1


#include "TriggerD3PDMaker/TriggerObjectAssociationTool.h"
#include "muonEvent/Muon.h"
#include "muonEvent/MuonContainer.h"
#include "TrigMuonEvent/TrigMuonEF.h"
#include "TrigMuonEvent/TrigMuonEFContainer.h"



namespace D3PD {


/**
 * @brief Associate from an Analysis::Muon to matching Event Filter trigger object.
 */
  typedef TriggerObjectAssociationTool<Analysis::Muon, TrigMuonEF>
  MuonEFTriggerObjectAssociationToolBase;

class MuonEFTriggerObjectAssociationTool
  : public MuonEFTriggerObjectAssociationToolBase
{
public:
  typedef MuonEFTriggerObjectAssociationToolBase Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  MuonEFTriggerObjectAssociationTool (const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent);
};


} // namespace D3PD



#endif
