// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDMaker/src/xAODPhotonObjectDecisionFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2010
 * @brief Record which triggers are passed by an Photon HLT object.
 */


#ifndef EGAMMAD3PDMAKER_XAODPHOTONOBJECTDECISIONFILLERTOOL_H
#define EGAMMAD3PDMAKER_XAODPHOTONOBJECTDECISIONFILLERTOOL_H


#include "TriggerD3PDMaker/TriggerObjectDecisionFillerTool.h"
#include "xAODEgamma/Photon.h"


namespace D3PD {


/**
 * @brief Record which triggers are passed by an egamma HLT object.
 */
class xAODPhotonObjectDecisionFillerTool
  : public TriggerObjectDecisionFillerTool<xAOD::Photon>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  xAODPhotonObjectDecisionFillerTool (const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent);
};


} // namespace D3PD


#endif // not EGAMMAD3PDMAKER_XAODPHOTONOBJECTDECISIONFILLERTOOL_H
