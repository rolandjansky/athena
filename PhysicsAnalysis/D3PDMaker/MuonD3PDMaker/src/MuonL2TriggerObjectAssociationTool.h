/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file MuonD3PDMaker/src/MuonL2TriggerObjectAssociationTool.h
 * @author Srivas Prasad <srivas.prasad@cern.ch>
 * @date Jan 2010
 * @brief Associate from an Analysis::Muon to matching L2 trigger object.
 */


#ifndef MUOND3PDMAKER_MUONL2TRIGGEROBJECTASSOCIATIONTOOL_H
#define MUOND3PDMAKER_MUONL2TRIGGEROBJECTASSOCIATIONTOOL_H


#include "TriggerD3PDMaker/TriggerObjectAssociationTool.h"
#include "muonEvent/Muon.h"
#include "TrigMuonEvent/CombinedMuonFeature.h"
#include "TrigMuonEvent/CombinedMuonFeatureContainer.h"

namespace D3PD {


/**
 * @brief Associate from a muon to its matching L2 trigger object.
 */
  typedef TriggerObjectAssociationTool<Analysis::Muon, CombinedMuonFeature>
  MuonL2TriggerObjectAssociationToolBase;

class MuonL2TriggerObjectAssociationTool
  : public MuonL2TriggerObjectAssociationToolBase
{
public:
  typedef MuonL2TriggerObjectAssociationToolBase Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  MuonL2TriggerObjectAssociationTool (const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent);
};


} // namespace D3PD



#endif
