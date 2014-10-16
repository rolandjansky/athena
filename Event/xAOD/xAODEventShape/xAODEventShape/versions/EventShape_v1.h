// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODEVENTSHAPE_VERSIONS_EVENTSHAPE_V1_H
#define XAODEVENTSHAPE_VERSIONS_EVENTSHAPE_V1_H

#include "AthContainers/AuxElement.h"
#include "AthContainers/DataVector.h"

namespace xAOD {

  /// Data class for event shapes.
  ///
  /// This class is used to save event-level quantities such as event shapes or
  ///  event density.
  /// One instance holds the shapes/densities corresponding to a given input source.
  /// An input source is typically the set of CaloClusters or of TrackParticles or TruthParticles.
  ///
  /// @author P-A Delsart
  ///


  class EventShape_v1 : public SG::AuxElement {       
  public:
    enum EventShapeID {
      UnknownShape = -1,
      Thrust,
      ThrustEta,
      ThrustPhi,
      ThrustMinor,      
      Sphericity,
      FoxWolfram,
    } ;
    
    enum EventDensityID {
      UnknownDensity = -1,

      Density,
      DensitySigma,
      DensityArea,

    } ;

    /// Default constructor
    EventShape_v1();
    

    /// Access shapes
    bool getShape(EventShapeID id, double &v) const ;
    bool setShape(EventShapeID id, double v)  ;
    
    /// Access densities
    bool getDensity(EventDensityID id, double &v) const ; 
    bool setDensity(EventDensityID id, double v) ;

    /// Prepare the object for writing
    void toPersistent();


  private:
    
  }; // class EventShape_v1
  
  
} // namespace xAOD

// Specify a CLID for the class for Athena:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::EventShape_v1 , 242958214 , 1 )
#endif // XAOD_STANDALONE

#endif // XAODEVENTFORMAT_VERSIONS_EVENTDATA_V1_H
