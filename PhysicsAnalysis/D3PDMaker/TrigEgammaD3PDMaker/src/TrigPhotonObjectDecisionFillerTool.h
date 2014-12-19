// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigEgammaD3PDMaker/src/TrigPhotonObjectDecisionFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2010
 * @brief Record which triggers are passed by a L2 photon object.
 */


#ifndef TRIGEGAMMAD3PDMAKER_TRIGPHOTONOBJECTDECISIONFILLERTOOL_H
#define TRIGEGAMMAD3PDMAKER_TRIGPHOTONOBJECTDECISIONFILLERTOOL_H


#include "TriggerD3PDMaker/TriggerObjectDecisionFillerTool.h"
class TrigPhoton;


namespace D3PD {


/**
 * @brief Record which triggers are passed by a L2 photon object.
 */
class TrigPhotonObjectDecisionFillerTool
  : public TriggerObjectDecisionFillerTool<TrigPhoton>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TrigPhotonObjectDecisionFillerTool (const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent);
};


} // namespace D3PD


#endif // not TRIGEGAMMAD3PDMAKER_TRIGPHOTONOBJECTDECISIONFILLERTOOL_H
