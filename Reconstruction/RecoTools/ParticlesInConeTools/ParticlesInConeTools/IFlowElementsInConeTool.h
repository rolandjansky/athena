/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IFlowElementsInConeTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef PARTICLESINCONETOOLS_IFLOWELEMENTSINCONETOOL_H
#define PARTICLESINCONETOOLS_IFLOWELEMENTSINCONETOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODPFlow/FlowElement.h"
#include <vector>

namespace xAOD {

  static const InterfaceID IID_IFlowElementsInConeTool("xAOD::IFlowElementsInConeTool", 1, 0);

  /** @class IFlowElementsInConeTool
      @brief interface for collecting pflow objects inside a cone
 
      @author Bill Balunas
   */
  class IFlowElementsInConeTool : virtual public IAlgTool {
  public:

    static const InterfaceID& interfaceID( ) ;

    /**IFlowElementsInConeTool interface: 
       @param[in] eta       eta for matching
       @param[in] phi       phi for matching
       @param[in] dr        cone size
       @param[in] output    output vector to be filled
       @return true if the calculation was successfull
     */    
    virtual bool particlesInCone( float eta, float phi, float dr, std::vector<const FlowElement*>& output ) const = 0;
  };

  inline const InterfaceID& IFlowElementsInConeTool::interfaceID() { 
    return IID_IFlowElementsInConeTool; 
  }

} // end of namespace

#endif 
