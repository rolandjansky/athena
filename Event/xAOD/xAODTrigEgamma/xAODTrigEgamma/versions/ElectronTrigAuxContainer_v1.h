// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ElectronTrigAuxContainer_v1.h 707252 2015-11-11 19:27:56Z rwhite $
#ifndef XAODTRIGEGAMMA_VERSIONS_ELECTRONTRIGAUXCONTAINER_V1_H
#define XAODTRIGEGAMMA_VERSIONS_ELECTRONTRIGAUXCONTAINER_V1_H

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
   // This is the trigger version to inherit from ByteStreamAuxContainer to persist all dynamic varibles
   /// $Revision: 707252 $
   /// $Date: 2015-11-11 20:27:56 +0100 (Wed, 11 Nov 2015) $
   ///
   class ElectronTrigAuxContainer_v1 : public AuxContainerBase {

   public :
      /// @name xAOD::ElectronAuxContainer constructor
      ElectronTrigAuxContainer_v1();

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

      /// @brief ET in a cone with half-opening angle 0.2, with exclusion of a window of size 7x5 in electromagnetic calorimeter and exclude CaloSampling::TileGap3
      std::vector< float >  etcone20; 
      /// @brief ET in a cone with half-opening angle 0.3, with exclusion of a window of size 7x5 in electromagnetic calorimeter and exclude CaloSampling::TileGap3
      std::vector< float >  etcone30; 
      /// @brief ET in a cone with half-opening angle 0.4, with exclusion of a window of size 7x5 in electromagnetic calorimeter and exclude CaloSampling::TileGap3
      std::vector< float >  etcone40; 
      /// @brief summed pt of tracks in a cone with half-opening angle 0.2 (no zvx cut photons, 1mm electrons)
      std::vector< float > ptcone20;
      /// @brief summed pt of tracks in a cone with half-opening angle 0.3
      std::vector< float > ptcone30;
      /// @brief summed pt of tracks in a cone with half-opening angle 0.4
      std::vector< float > ptcone40;
      /// @brief summed pt of tracks in a cone with half-opening angle 0.2 (no zvx cut photons, 1mm electrons)
      std::vector< float > ptvarcone20;
      /// @brief summed pt of tracks in a cone with half-opening angle 0.3
      std::vector< float > ptvarcone30;
      /// @brief summed pt of tracks in a cone with half-opening angle 0.4
      std::vector< float > ptvarcone40;
      //et cone for topo cluster
      /// @brief ET in a cone with half-opening angle 0.20
      std::vector< float >  topoetcone20;
      /// @brief ET in a cone with half-opening angle 0.30
      std::vector< float >  topoetcone30;
      /// @brief ET in a cone with half-opening angle 0.40
      std::vector< float >  topoetcone40;
      /// @brief Pid decision
      std::vector< char > VLoose;
      /// @brief Pid decision
      std::vector< char > Loose;
      /// @brief Pid decision
      std::vector< char > Medium;
      /// @brief Pid decision
      std::vector< char > Tight;
      /// @brief Pid isEM word
      std::vector< unsigned int > isEMVLoose;
      /// @brief Pid decision isEM word
      std::vector< unsigned int > isEMLoose;
      /// @brief Pid decision isEM word
      std::vector< unsigned int > isEMMedium;
      /// @brief Pid decision isEM word
      std::vector< unsigned int > isEMTight;
      /// @brief Pid decision
      std::vector< char > LHVLoose;
      /// @brief Pid decision
      std::vector< char > LHLoose;
      /// @brief Pid decision
      std::vector< char > LHMedium;
      /// @brief Pid decision
      std::vector< char > LHTight;
      /// @brief Pid isEM word
      std::vector< unsigned int > isEMLHVLoose;
      /// @brief Pid decision isEM word
      std::vector< unsigned int > isEMLHLoose;
      /// @brief Pid decision isEM word
      std::vector< unsigned int > isEMLHMedium;
      /// @brief Pid decision isEM word
      std::vector< unsigned int > isEMLHTight;
      /// @brief Pid decision
      std::vector< char > LHCaloVLoose;
      /// @brief Pid decision
      std::vector< char > LHCaloLoose;
      /// @brief Pid decision
      std::vector< char > LHCaloMedium;
      /// @brief Pid decision
      std::vector< char > LHCaloTight;
      /// @brief Pid isEM word
      std::vector< unsigned int > isEMLHCaloVLoose;
      /// @brief Pid decision isEM word
      std::vector< unsigned int > isEMLHCaloLoose;
      /// @brief Pid decision isEM word
      std::vector< unsigned int > isEMLHCaloMedium;
      /// @brief Pid decision isEM word
      std::vector< unsigned int > isEMLHCaloTight;
      /// @brief Pid LH value
      std::vector< float > LHValue;
      /// @brief Pid LH calo value
      std::vector< float > LHCaloValue;

      /// @}

   }; // class ElectronTrigAuxContainer_v1

} // namespace xAOD

// Set up the StoreGate inheritance for the class:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::ElectronTrigAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODTRIGEGAMMA_VERSIONS_ELECTRONAUXCONTAINER_V3_H
