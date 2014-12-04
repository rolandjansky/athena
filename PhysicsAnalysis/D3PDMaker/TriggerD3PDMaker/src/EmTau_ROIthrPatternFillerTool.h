// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TriggerD3PDMaker/EmTau_ROIthrPatternFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2014
 * @brief xxx
 */


#ifndef TRIGGERD3PDMAKER_EMTAU_ROITHRPATTERNFILLERTOOL_H
#define TRIGGERD3PDMAKER_EMTAU_ROITHRPATTERNFILLERTOOL_H


#include "ROIthrPatternFillerTool.h"


namespace D3PD {


class EmTau_ROIthrPatternFillerTool
  : public ROIthrPatternFillerTool<xAOD::EmTauRoI>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  EmTau_ROIthrPatternFillerTool (const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent)
    : ROIthrPatternFillerTool<xAOD::EmTauRoI> (type, name, parent)
  {
  }
};


} // namespace D3PD


#endif // not TRIGGERD3PDMAKER_EMTAU_ROITHRPATTERNFILLERTOOL_H
