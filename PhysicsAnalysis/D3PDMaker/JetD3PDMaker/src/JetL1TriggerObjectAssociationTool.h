// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file JetD3PDMaker/src/JetL1TriggerObjectAssociationTool.h
 * @author Haifeng Li <Haifeng.Li@cern.ch>
 * @date Oct, 2010
 * @brief Associate from an Jet to its matching L1 trigger object.
 */


#ifndef JETD3PDMAKER_JETL1TRIGGEROBJECTASSOCIATIONTOOL_H
#define JETD3PDMAKER_JETL1TRIGGEROBJECTASSOCIATIONTOOL_H


#include "TriggerD3PDMaker/TriggerObjectAssociationTool.h"
#include "AnalysisTriggerEvent/Jet_ROI.h"
#include "JetEvent/Jet.h"

namespace D3PD {


/**
 * @brief Associate from an Tau to its matching L1 trigger object.
 */
typedef TriggerObjectAssociationTool<Jet, Jet_ROI>
  JetL1TriggerObjectAssociationToolBase;

class JetL1TriggerObjectAssociationTool
  : public JetL1TriggerObjectAssociationToolBase
{
public:
  typedef JetL1TriggerObjectAssociationToolBase Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  JetL1TriggerObjectAssociationTool (const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent);
};


} // namespace D3PD



#endif // not JETD3PDMAKER_JETL1TRIGGEROBJECTASSOCIATIONTOOL_H
