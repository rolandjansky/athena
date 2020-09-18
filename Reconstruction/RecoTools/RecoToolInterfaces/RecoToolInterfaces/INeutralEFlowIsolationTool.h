/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITrackIsolationTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef NEUTRALEFLOWCLUSTERISOLATIONTOOLS_INEUTRALEFLOWCLUSTERISOLATIONTOOL_H
#define NEUTRALEFLOWCLUSTERISOLATIONTOOLS_INEUTRALEFLOWCLUSTERISOLATIONTOOL_H

#include "AsgTools/AsgTool.h"
#include "xAODPrimitives/IsolationType.h"
#include "xAODBase/IParticle.h"
#include "IsolationCommon.h"

class CaloClusterContainer;

namespace xAOD {

  /** @class INeutralEFlowIsolationTool
      @brief interface for tools calculating neutral eflow isolation
 
      @author Niels van Eldik, Sandrine Laplace
   */

  class INeutralEFlowIsolationTool : virtual public asg::IAsgTool {
    ASG_TOOL_INTERFACE( xAOD::INeutralEFlowIsolationTool )
  public:

    /**INeutralEFlowIsolationTool interface for eflow isolation:
       The tool expects the cones to be order in decreasing order (neflowiso40 -> neflowiso20)
       Internally it reorders the cones so the output isolation values are also in the same order. 
       @param[in] result    output object to be filled
       @param[in] tp        input iparticle
       @param[in] cones     vector of input cones to be used
       @param[in] corrections bitset specifying which corrections to apply to isolation
       @return true if the calculation was successfull
    */    
    virtual bool neutralEflowIsolation(CaloIsolation& result, const IParticle& tp, 
				       const std::vector<Iso::IsolationType>& cones, 
				       CaloCorrection corrections) const = 0; 

  };
 
} // end of namespace

#endif 
