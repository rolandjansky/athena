// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TauD3PDMaker/src/TauL2TriggerObjectAssociationTool.h
 * @date Nov, 2009
 * @brief Associate from an Tau to its matching L2 trigger object.
 */


#ifndef TAUD3PDMAKER_TAUL2TRIGGEROBJECTASSOCIATIONTOOL_H
#define TAUD3PDMAKER_TAUL2TRIGGEROBJECTASSOCIATIONTOOL_H


#include "TriggerD3PDMaker/TriggerObjectAssociationTool.h"
#include "tauEvent/TauJet.h"
#include "TrigParticle/TrigTau.h"
#include "TrigParticle/TrigTauContainer.h"


namespace D3PD {


/**
 * @brief Associate from an electron to its matching L2 trigger object.
 */
typedef TriggerObjectAssociationTool<Analysis::TauJet, TrigTau>
  TauL2TriggerObjectAssociationToolBase;

class TauL2TriggerObjectAssociationTool
  : public TauL2TriggerObjectAssociationToolBase
{
public:
  typedef TauL2TriggerObjectAssociationToolBase Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TauL2TriggerObjectAssociationTool (const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent);
};


} // namespace D3PD



#endif // not TAUD3PDMAKER_TAUL2TRIGGEROBJECTASSOCIATIONTOOL_H
