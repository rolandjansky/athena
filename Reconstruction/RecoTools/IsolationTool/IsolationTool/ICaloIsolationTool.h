/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITrackIsolationTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef CALOISOLATIONTOOLS_ICALOISOALTIONTOOL_H
#define CALOISOLATIONTOOLS_ICALOISOALTIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODPrimitives/IsolationType.h"
#include "xAODBase/IParticle.h"
#include "xAODTracking/TrackParticle.h"

namespace xAOD {

  static const InterfaceID IID_ICaloIsolationTool("xAOD::ICaloIsolationTool", 1, 0);

  /** @class ICaloIsolationTool
      @brief interface for tools calculating track isolation
 
      @author Niels van Eldik
   */

  struct CaloIsolation {
    float ecore;
    std::vector<float> etcones;
  };

  class ICaloIsolationTool : virtual public IAlgTool {
  public:
    enum SubtractionStrategy {
      Ecore = 0,
      Eeg57 = 1,
      NoSubtraction
    };

    static const InterfaceID& interfaceID( ) ;

    /**ICaloIsolationTool interface: 
       @param[in] result    output object to be filled
       @param[in] tp        input iparticle
       @param[in] cones     vector of input cones to be used
       @param[in] strategy  overlap removal strategy to be used
       @return true if the calculation was successfull
     */    
    virtual bool caloIsolation( CaloIsolation& result, const IParticle& tp, const std::vector<Iso::IsolationType>& cones, 
                                SubtractionStrategy strategy = Ecore ) = 0; 

    /**ICaloIsolationTool interface: 
       @param[in] result    output object to be filled
       @param[in] tp        input iparticle
       @param[in] cones     vector of input cones to be used
       @param[in] strategy  overlap removal strategy to be used
       @return true if the calculation was successfull
     */    
    virtual bool caloIsolation( CaloIsolation& result, const TrackParticle& tp, const std::vector<Iso::IsolationType>& cones, 
                                SubtractionStrategy strategy = Ecore ) = 0;

  };

  inline const InterfaceID& ICaloIsolationTool::interfaceID() { 
    return IID_ICaloIsolationTool; 
  }

} // end of namespace

#endif 
