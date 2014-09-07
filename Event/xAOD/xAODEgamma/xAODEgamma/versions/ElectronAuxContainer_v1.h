// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ElectronAuxContainer_v1.h 614829 2014-09-03 09:39:36Z krasznaa $
#ifndef XAODEGAMMA_VERSIONS_ELECTRONAUXCONTAINER_V1_H
#define XAODEGAMMA_VERSIONS_ELECTRONAUXCONTAINER_V1_H

// System include(s):
#include <vector>
#include "xAODEgamma/EgammaDefs.h"
#include "xAODEgamma/versions/EgammaAuxContainer_v1.h"
#include "xAODTracking/TrackParticleContainer.h" 


namespace xAOD {


  /// Temporary container used until we have I/O for AuxStoreInternal
  ///
  /// This class is meant to serve as a temporary way to provide an auxiliary
  /// store with Athena I/O capabilities for the electron EDM. Will be exchanged for
  /// a generic auxiliary container type (AuxStoreInternal) later on.
  ///
  /// @author Christos Anastopoulos
  /// @author Anthony Morley
  ///
  /// $Revision: 614829 $
  /// $Date: 2014-09-03 11:39:36 +0200 (Wed, 03 Sep 2014) $
  ///

  class ElectronAuxContainer_v1 : public EgammaAuxContainer_v1 {


  public :
    /// @name xAOD::ElectronAuxContainer constructors
    ElectronAuxContainer_v1();

  private:

    ///@name vector of links to trackParticles 
    /// @{

    std::vector< std::vector< ElementLink< TrackParticleContainer > > > trackParticleLinks;   

    /// @}

    ///@name Electron Charge
    ///@{
	std::vector<float> charge;
    ///@}

    /// @name Track Match details
    /// @{

    /// @brief difference between the cluster eta (presampler) and
    ///the eta of the track extrapolated to the presampler 
    std::vector< float > deltaEta0;
    ///@brief difference between the cluster eta (first sampling) and the eta of the track extrapolated to the first sampling: |eta_stripscluster -eta_ID|, where eta_stripscluster is computed 
    ///in the first sampling of the electromagnetic calorimeter, where the granularity is very fine, and eta_ID is the pseudo-rapidity of the track extrapolated to the calorimeter 
    std::vector< float > deltaEta1 ;
    /// @brief difference between the cluster eta (second sampling) and the eta of the track extrapolated to the second sampling 
    std::vector< float > deltaEta2 ;
    /// @brief difference between the cluster eta (3rd sampling) and
    ///	  the eta of the track extrapolated to the 3rd sampling 
    std::vector< float >  deltaEta3 ;
    /// @brief difference between the cluster phi (presampler) and
    ///	  the eta of the track extrapolated to the presampler 
    std::vector< float >    deltaPhi0;
    /// @brief difference between the cluster eta (1st sampling) and
    ///	  the eta of the track extrapolated to the 1st sampling (strips) 
    std::vector< float >   deltaPhi1 ;
    /// @brief difference between the cluster phi (second sampling) and the phi of the track
    ///	  extrapolated to the second sampling : |phi_middlecluster -phi_ID|, where phi_middlecluster
    ///	  is computed in the second compartment of the electromagnetic calorimeter and phi_ID is the
    ///  azimuth of the track extrapolated to the calorimeter 
    std::vector< float >    deltaPhi2 ;
    ///@brief difference between the cluster eta (3rd sampling) and
    ///	  the eta of the track extrapolated to the 3rd sampling 
    std::vector< float >    deltaPhi3 ;
    /// @brief difference between the cluster phi (sampling 2) and the
    /// eta of the track extrapolated from the last measurement point.
    std::vector< float >  deltaPhiFromLastMeasurement;
    /// @brief difference between the cluster phi (presampler) and
    ///	  the eta of the track extrapolated to the presampler  from the perigee with a rescaled
    /// momentum. 
    std::vector< float >    deltaPhiRescaled0;
    /// @brief difference between the cluster eta (1st sampling) and
    ///	  the eta of the track extrapolated to the 1st sampling (strips) from the perigee with a rescaled
    /// momentum. 
    std::vector< float >   deltaPhiRescaled1 ;
    /// @brief difference between the cluster phi (second sampling) and the phi of the track
    ///	  extrapolated to the second sampling from the perigee with a rescaled
    /// momentum. 
    std::vector< float >    deltaPhiRescaled2 ;
    ///@brief difference between the cluster eta (3rd sampling) and
    /// the eta of the track extrapolated to the 3rd sampling from the perigee with a rescaled
    /// momentum. 
    std::vector< float >    deltaPhiRescaled3 ;

    ///@}

  }; // class ElectronAuxContainer_v1

} // namespace xAOD

// Set up the StoreGate inheritance for the class:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::ElectronAuxContainer_v1, xAOD::EgammaAuxContainer_v1 );

#endif // XAODEGAMMA_VERSIONS_ELECTRONAUXCONTAINER_V1_H
