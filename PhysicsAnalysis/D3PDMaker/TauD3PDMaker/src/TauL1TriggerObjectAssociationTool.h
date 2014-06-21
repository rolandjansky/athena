// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TauD3PDMaker/src/TauL1TriggerObjectAssociationTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2009
 * @brief Associate from an Tau to its matching L1 trigger object.
 */


#ifndef TAUD3PDMAKER_TAUL1TRIGGEROBJECTASSOCIATIONTOOL_H
#define TAUD3PDMAKER_TAUL1TRIGGEROBJECTASSOCIATIONTOOL_H


#include "TriggerD3PDMaker/TriggerObjectAssociationTool.h"
#include "AnalysisTriggerEvent/EmTau_ROI.h"
#include "tauEvent/TauJet.h"


namespace D3PD {


/**
 * @brief Associate from an Tau to its matching L1 trigger object.
 */
typedef TriggerObjectAssociationTool<Analysis::TauJet, EmTau_ROI>
  TauL1TriggerObjectAssociationToolBase;

class TauL1TriggerObjectAssociationTool
  : public TauL1TriggerObjectAssociationToolBase
{
public:
  typedef TauL1TriggerObjectAssociationToolBase Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TauL1TriggerObjectAssociationTool (const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent);
};


} // namespace D3PD



#endif // not TAUD3PDMAKER_TAUL1TRIGGEROBJECTASSOCIATIONTOOL_H
