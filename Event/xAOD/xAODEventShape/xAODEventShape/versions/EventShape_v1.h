// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventShape_v1.h 629489 2014-11-19 14:37:55Z krasznaa $
#ifndef XAODEVENTSHAPE_VERSIONS_EVENTSHAPE_V1_H
#define XAODEVENTSHAPE_VERSIONS_EVENTSHAPE_V1_H

// Base EDM include(s):
#include "AthContainers/AuxElement.h"

namespace xAOD {

   /// Data class for event shapes.
   ///
   /// This class is used to save event-level quantities such as event shapes or
   /// event density. One instance holds the shapes/densities corresponding to a
   /// given input source. An input source is typically the set of CaloClusters
   /// or of TrackParticles or TruthParticles.
   ///
   /// @author P-A Delsart
   ///
   /// $Revision: 629489 $
   /// $Date: 2014-11-19 15:37:55 +0100 (Wed, 19 Nov 2014) $
   ///
   class EventShape_v1 : public SG::AuxElement {

   public:
      /// Default constructor
      EventShape_v1();

      /// Event shape variable types
      enum EventShapeID {
         UnknownShape = -1,
         Thrust,
         ThrustEta,
         ThrustPhi,
         ThrustMinor,
         Sphericity,
         FoxWolfram
      };
      /// Event density variable types
      enum EventDensityID {
         UnknownDensity = -1,
         Density,
         DensitySigma,
         DensityArea
      };

      /// Get a shape variable from the object
      bool getShape( EventShapeID id, double &v ) const;
      /// Get a shape variable in a PyROOT friendly way (can throw exception!)
      double getShape( EventShapeID id ) const;
      /// Set a shape variable on the object
      bool setShape( EventShapeID id, double v );

      /// Get a density variable from the object
      bool getDensity( EventDensityID id, double &v ) const;
      /// Get a density variable in a PyROOT friendly way (can throw exception!)
      double getDensity( EventDensityID id ) const;
      /// Set a density variable on the object
      bool setDensity( EventDensityID id, double v );

   }; // class EventShape_v1

} // namespace xAOD

// Declare the inheritance of the class towards SG:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::EventShape_v1, SG::AuxElement );

#endif // XAODEVENTSHAPE_VERSIONS_EVENTSHAPE_V1_H
