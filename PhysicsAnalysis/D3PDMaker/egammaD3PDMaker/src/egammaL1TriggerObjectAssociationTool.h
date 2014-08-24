// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDMaker/src/egammaL1TriggerObjectAssociationTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2009
 * @brief Associate from an egamma to its matching L1 trigger object.
 */


#ifndef EGAMMAD3PDMAKER_EGAMMAL1TRIGGEROBJECTASSOCIATIONTOOL_H
#define EGAMMAD3PDMAKER_EGAMMAL1TRIGGEROBJECTASSOCIATIONTOOL_H


#include "TriggerD3PDMaker/TriggerObjectAssociationTool.h"
#include "AnalysisTriggerEvent/EmTau_ROI.h"
#include "egammaEvent/egamma.h"


namespace D3PD {


/**
 * @brief Associate from an egamma to its matching L1 trigger object.
 */
typedef TriggerObjectAssociationTool<egamma, EmTau_ROI>
  egammaL1TriggerObjectAssociationToolBase;

class egammaL1TriggerObjectAssociationTool
  : public egammaL1TriggerObjectAssociationToolBase
{
public:
  typedef egammaL1TriggerObjectAssociationToolBase Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  egammaL1TriggerObjectAssociationTool (const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent);
};


} // namespace D3PD



#endif // not EGAMMAD3PDMAKER_EGAMMAL1TRIGGEROBJECTASSOCIATIONTOOL_H
