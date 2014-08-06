// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TriggerD3PDMaker/src/EmTau_ROIObjectDecisionFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2010
 * @brief Record which triggers are passed by a L1 EM/Tau object.
 */


#ifndef TRIGGERD3PDMAKER_EMTAU_ROIOBJECTDECISIONFILLERTOOL_H
#define TRIGGERD3PDMAKER_EMTAU_ROIOBJECTDECISIONFILLERTOOL_H


#include "TriggerD3PDMaker/TriggerObjectDecisionFillerTool.h"
class EmTau_ROI;


namespace D3PD {


/**
 * @brief Record which triggers are passed by a L2 electron object.
 */
class EmTau_ROIObjectDecisionFillerTool
  : public TriggerObjectDecisionFillerTool<EmTau_ROI>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  EmTau_ROIObjectDecisionFillerTool (const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent);
};


} // namespace D3PD


#endif // not TRIGGERD3PDMAKER_EMTAU_ROIOBJECTDECISIONFILLERTOOL_H
