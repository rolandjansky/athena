/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IPFlowObjectsInConeTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef PARTICLESINCONETOOLS_IPFLOWOBJECTSINCONETOOL_H
#define PARTICLESINCONETOOLS_IPFLOWOBJECTSINCONETOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODPFlow/PFO.h"
#include <vector>

namespace xAOD {

  static const InterfaceID IID_IPFlowObjectsInConeTool("xAOD::IPFlowObjectsInConeTool", 1, 0);

  /** @class IPFlowObjectsInConeTool
      @brief interface for collecting pflow objects inside a cone
 
      @author Niels van Eldik
   */
  class IPFlowObjectsInConeTool : virtual public IAlgTool {
  public:

    static const InterfaceID& interfaceID( ) ;

    /**IPFlowObjectsInConeTool interface: 
       @param[in] eta       eta for matching
       @param[in] phi       phi for matching
       @param[in] dr        cone size
       @param[in] output    output vector to be filled
       @return true if the calculation was successfull
     */    
    virtual bool particlesInCone( float eta, float phi, float dr, std::vector<const PFO*>& output ) const = 0;
  };

  inline const InterfaceID& IPFlowObjectsInConeTool::interfaceID() { 
    return IID_IPFlowObjectsInConeTool; 
  }

} // end of namespace

#endif 
