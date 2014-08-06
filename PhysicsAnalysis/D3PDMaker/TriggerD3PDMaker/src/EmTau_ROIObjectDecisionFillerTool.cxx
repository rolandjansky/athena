/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TriggerD3PDMaker/src/EmTau_ROIObjectDecisionFillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2010
 * @brief Record which triggers are passed by a L1 EM/Tau object.
 */


#include "EmTau_ROIObjectDecisionFillerTool.h"
#include "AnalysisTriggerEvent/EmTau_ROI.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
EmTau_ROIObjectDecisionFillerTool::EmTau_ROIObjectDecisionFillerTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : TriggerObjectDecisionFillerTool<EmTau_ROI> (type, name, parent)
{
}


} // namespace D3PD
