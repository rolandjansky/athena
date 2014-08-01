/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file MuonD3PDMaker/src/MuonL1TriggerObjectAssociationTool.h
 * @author Srivas Prasad <srivas.prasad@cern.ch>
 * @date Jan 2010
 * @brief Associate from an Analysis::Muon to matching L1 trigger object.
 */


#ifndef MUOND3PDMAKER_MUONL1TRIGGEROBJECTASSOCIATIONTOOL_H
#define MUOND3PDMAKER_MUONL1TRIGGEROBJECTASSOCIATIONTOOL_H


#include "TriggerD3PDMaker/TriggerObjectAssociationTool.h"
#include "AnalysisTriggerEvent/Muon_ROI.h"
#include "muonEvent/Muon.h"


namespace D3PD {


/**
 * @brief Associate from an Analysis::Muon to matching L1 trigger object.
 */
  typedef TriggerObjectAssociationTool<Analysis::Muon, Muon_ROI>
  MuonL1TriggerObjectAssociationToolBase;

class MuonL1TriggerObjectAssociationTool
  : public MuonL1TriggerObjectAssociationToolBase
{
public:
  typedef MuonL1TriggerObjectAssociationToolBase Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  MuonL1TriggerObjectAssociationTool (const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent);
};


} // namespace D3PD



#endif
