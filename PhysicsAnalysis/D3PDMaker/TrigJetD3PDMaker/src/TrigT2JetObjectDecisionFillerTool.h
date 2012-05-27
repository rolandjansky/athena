// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigT2JetObjectDecisionFillerTool.h 307763 2010-06-29 14:25:58Z krasznaa $
/**
 * @file TrigEgammaD3PDMaker/src/TrigT2JetObjectDecisionFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2010
 * @brief Record which triggers are passed by a L2 jet object.
 */


#ifndef TRIGJETD3PDMAKER_TRIGT2JETOBJECTDECISIONFILLERTOOL_H
#define TRIGJETD3PDMAKER_TRIGT2JETOBJECTDECISIONFILLERTOOL_H


#include "TriggerD3PDMaker/TriggerObjectDecisionFillerTool.h"
class TrigT2Jet;


namespace D3PD {


/**
 * @brief Record which triggers are passed by a L2 electron object.
 */
class TrigT2JetObjectDecisionFillerTool
  : public TriggerObjectDecisionFillerTool<TrigT2Jet>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TrigT2JetObjectDecisionFillerTool (const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent);
};


} // namespace D3PD


#endif 
