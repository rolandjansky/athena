/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITrackIsolationTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef NEUTRALEFLOWCLUSTERISOLATIONTOOLS_INEUTRALEFLOWCLUSTERISOLATIONTOOL_H
#define NEUTRALEFLOWCLUSTERISOLATIONTOOLS_INEUTRALEFLOWCLUSTERISOLATIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODPrimitives/IsolationType.h"
#include "xAODBase/IParticle.h"
#include "IsolationCommon.h"

class CaloClusterContainer;

namespace xAOD {

  static const InterfaceID IID_INeutralEFlowIsolationTool("xAOD::INeutralEFlowIsolationTool", 1, 0);

  /** @class INeutralEFlowIsolationTool
      @brief interface for tools calculating neutral eflow isolation
 
      @author Niels van Eldik, Sandrine Laplace
   */

  class INeutralEFlowIsolationTool : virtual public IAlgTool {
  public:

    static const InterfaceID& interfaceID( ) ;


    /**INeutralEFlowIsolationTool interface for eflow isolation: 
       @param[in] result    output object to be filled
       @param[in] tp        input iparticle
       @param[in] cones     vector of input cones to be used
       @param[in] corrections bitset specifying which corrections to apply to isolation
       @return true if the calculation was successfull
    */    
    virtual bool neutralEflowIsolation(CaloIsolation& result, const IParticle& tp, 
				       const std::vector<Iso::IsolationType>& cones, 
				       CaloCorrection corrections) = 0; 
 
    virtual bool decorateParticle_eflowIso(IParticle& tp,
                                           const std::vector<Iso::IsolationType>& cones,
                                           CaloCorrection corrections) = 0;
  };
 
  inline const InterfaceID& INeutralEFlowIsolationTool::interfaceID() { 
    return IID_INeutralEFlowIsolationTool; 
  }

} // end of namespace

#endif 
