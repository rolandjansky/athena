// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TauD3PDMaker/src/TauEFTriggerObjectAssociationTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Associate from an Tau to its matching EF trigger object.
 */


#ifndef TAUD3PDMAKER_TAUEFTRIGGEROBJECTASSOCIATIONTOOL_H
#define TAUD3PDMAKER_TAUEFTRIGGEROBJECTASSOCIATIONTOOL_H


#include "TriggerD3PDMaker/TriggerObjectAssociationTool.h"
#include "tauEvent/TauJet.h"
#include "tauEvent/TauJetContainer.h"


namespace D3PD {


/**
 * @brief Associate from an Tau to its matching EF trigger object.
 */
typedef TriggerObjectAssociationTool<Analysis::TauJet, Analysis::TauJet>
  TauEFTriggerObjectAssociationToolBase;

class TauEFTriggerObjectAssociationTool
  : public TauEFTriggerObjectAssociationToolBase
{
public:
  typedef TauEFTriggerObjectAssociationToolBase Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TauEFTriggerObjectAssociationTool (const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent);
};


} // namespace D3PD



#endif // not TAUD3PDMAKER_TAUEFTRIGGEROBJECTASSOCIATIONTOOL_H
