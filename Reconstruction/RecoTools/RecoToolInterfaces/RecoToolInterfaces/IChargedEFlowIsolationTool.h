/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IChargedEFlowIsolationTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef CHARGEDEFLOWISOLATIONTOOLS_ICHARGEDEFLOWISOLATIONTOOL_H
#define CHARGEDEFLOWISOLATIONTOOLS_ICHARGEDEFLOWISOLATIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"
#include "xAODPrimitives/IsolationType.h"
#include "IsolationCommon.h"
#include <set>

namespace xAOD {

  static const InterfaceID IID_IChargedEFlowIsolationTool("xAOD::IChargedEFlowIsolationTool", 1, 0);

  /** @class IChargedEFlowIsolationTool
      @brief interface for tools calculating charged eflow isolation
 
      @author Niels van Eldik, Sandrine Laplace
   */

  class IChargedEFlowIsolationTool : virtual public IAlgTool {
  public:

    static const InterfaceID& interfaceID( ) ;

    /**IChargedEFlowIsolationTool interface: 
       Calculates the track isolation. 
       The tool expects the cones to be order in decreasing order (ptcone40 -> ptcone10)
       Internally it reorders the cones so the output isolation values are also in the same order. 
       @param[in] result              output object to be filled
       @param[in] tp                  input iparticle
       @param[in] cones               vector of input cones to be used
       @param[in] corrections bitset  specifying which corrections to apply to isolation
       @param[in] vertex              if provided vertex to be used for track selection
       @param[in] exclusionSet        if provided track particles in the set are ignored
       @return true if the calculation was successfull
     */    
    virtual bool chargedEflowIsolation( xAOD::TrackIsolation& result, const IParticle& tp, 
					const std::vector<Iso::IsolationType>& cones, 
					xAOD::TrackCorrection corrections,
					const Vertex* vertex = 0, 
					const std::set<const xAOD::TrackParticle*>* exclusionSet = 0) = 0; 
  };

  inline const InterfaceID& IChargedEFlowIsolationTool::interfaceID() { 
    return IID_IChargedEFlowIsolationTool; 
  }

} // end of namespace

#endif 
