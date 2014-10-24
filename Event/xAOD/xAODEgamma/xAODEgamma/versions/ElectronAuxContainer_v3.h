// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ElectronAuxContainer_v3.h 619839 2014-10-03 13:38:44Z christos $
#ifndef XAODEGAMMA_VERSIONS_ELECTRONAUXCONTAINER_V3_H
#define XAODEGAMMA_VERSIONS_ELECTRONAUXCONTAINER_V3_H

// System include(s):
#include <vector>

// Core EDM include(s):
#include "AthLinks/ElementLink.h"
#include "xAODCore/AuxContainerBase.h"

// xAOD include(s):
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

namespace xAOD {

   /// Container holding the payload of the electron container
   ///
   /// This is a specific container for xAOD::ElectronCollection, holding all
   /// the "usual" properties of electron reconstructed in the offline
   /// software
   ///
   /// @author Christos Anastopoulos
   /// @author Anthony Morley
   ///
   /// $Revision: 619839 $
   /// $Date: 2014-10-03 15:38:44 +0200 (Fri, 03 Oct 2014) $
   ///
   class ElectronAuxContainer_v3 : public AuxContainerBase {

   public :
      /// @name xAOD::ElectronAuxContainer constructor
      ElectronAuxContainer_v3();

   private:
      /// Include the egamma variables:
#define EGAMMA_AUX_VARIABLE( TYPE, NAME ) TYPE NAME
#include "xAODEgamma/versions/EgammaAuxVariables_v3.def"
#undef EGAMMA_AUX_VARIABLE

      ///@name vector of links to trackParticles 
      /// @{

      std::vector< std::vector< ElementLink< TrackParticleContainer > > >
      trackParticleLinks;   

      /// @}

      ///@name Electron Charge
      ///@{

      std::vector< float > charge;

      ///@}

      /// @name Track Match details
      /// @{

      /// Difference between the cluster eta (presampler) and
      /// the eta of the track extrapolated to the presampler 
      std::vector< float > deltaEta0;
      /// Difference between the cluster eta (first sampling) and the eta of
      /// the track extrapolated to the first sampling:
      ///   |eta_stripscluster - eta_ID|,
      /// where eta_stripscluster is computed in the first sampling of the
      /// electromagnetic calorimeter, where the granularity is very fine,
      /// and eta_ID is the pseudo-rapidity of the track extrapolated to the
      /// calorimeter 
      std::vector< float > deltaEta1;
      /// Difference between the cluster eta (second sampling) and the eta of
      /// the track extrapolated to the second sampling 
      std::vector< float > deltaEta2;
      /// Difference between the cluster eta (3rd sampling) and
      /// the eta of the track extrapolated to the 3rd sampling 
      std::vector< float > deltaEta3;
      /// Difference between the cluster phi (presampler) and
      /// the eta of the track extrapolated to the presampler 
      std::vector< float > deltaPhi0;
      /// Difference between the cluster eta (1st sampling) and
      /// the eta of the track extrapolated to the 1st sampling (strips) 
      std::vector< float > deltaPhi1;
      /// Difference between the cluster phi (second sampling) and the phi
      /// of the track extrapolated to the second sampling:
      ///   |phi_middlecluster - phi_ID|,
      /// where phi_middlecluster is computed in the second compartment of
      /// the electromagnetic calorimeter and phi_ID is the
      /// azimuth of the track extrapolated to the calorimeter 
      std::vector< float > deltaPhi2;
      /// Difference between the cluster eta (3rd sampling) and
      /// the eta of the track extrapolated to the 3rd sampling 
      std::vector< float > deltaPhi3;
      /// Difference between the cluster phi (sampling 2) and the
      /// eta of the track extrapolated from the last measurement point.
      std::vector< float > deltaPhiFromLastMeasurement;
      /// Difference between the cluster phi (presampler) and
      /// the eta of the track extrapolated to the presampler from the
      /// perigee with a rescaled momentum. 
      std::vector< float > deltaPhiRescaled0;
      /// Difference between the cluster eta (1st sampling) and
      /// the eta of the track extrapolated to the 1st sampling (strips)
      /// from the perigee with a rescaled momentum. 
      std::vector< float > deltaPhiRescaled1;
      /// Difference between the cluster phi (second sampling) and the phi
      /// of the track extrapolated to the second sampling from the perigee
      /// with a rescaled momentum. 
      std::vector< float > deltaPhiRescaled2;
      /// Difference between the cluster eta (3rd sampling) and
      /// the eta of the track extrapolated to the 3rd sampling from the
      /// perigee with a rescaled momentum. 
      std::vector< float > deltaPhiRescaled3;

      /// @}

   }; // class ElectronAuxContainer_v3

} // namespace xAOD

// Set up the StoreGate inheritance for the class:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::ElectronAuxContainer_v3, xAOD::AuxContainerBase );

#endif // XAODEGAMMA_VERSIONS_ELECTRONAUXCONTAINER_V3_H
