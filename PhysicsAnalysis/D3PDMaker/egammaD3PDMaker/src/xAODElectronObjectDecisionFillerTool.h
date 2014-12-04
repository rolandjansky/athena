// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDMaker/src/xAODElectronObjectDecisionFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2010
 * @brief Record which triggers are passed by an Electron HLT object.
 */


#ifndef EGAMMAD3PDMAKER_XAODELECTRONOBJECTDECISIONFILLERTOOL_H
#define EGAMMAD3PDMAKER_XAODELECTRONOBJECTDECISIONFILLERTOOL_H


#include "TriggerD3PDMaker/TriggerObjectDecisionFillerTool.h"
#include "xAODEgamma/Electron.h"


namespace D3PD {


/**
 * @brief Record which triggers are passed by an Electron HLT object.
 */
class xAODElectronObjectDecisionFillerTool
  : public TriggerObjectDecisionFillerTool<xAOD::Electron>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  xAODElectronObjectDecisionFillerTool (const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent);
};


} // namespace D3PD


#endif // not EGAMMAD3PDMAKER_XAODELECTRONOBJECTDECISIONFILLERTOOL_H
