/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODASSOCIATIONS_VERSIONS_TRACKPARTICLECLUSTERASSOCIATION_V1_H
#define XAODASSOCIATIONS_VERSIONS_TRACKPARTICLECLUSTERASSOCIATION_V1_H

// Core include(s):
#include "AthContainers/AuxElement.h"
#include "AthLinks/ElementLink.h"

// xAOD include(s):
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

namespace xAOD {

  /// Class describing a TrackParticleClusterAssociation.
  ///
  /// @author Niels Van Eldik <niels.van.eldik@cern.ch>
  /// @author Edward Moyse <Edward.Moyse@cern.ch>
  /// @author Heberth Torres <htorres@cern.ch>
  /// @nosubgrouping
  ///
  class TrackParticleClusterAssociation_v1 : public SG::AuxElement {

  public:
    
    /// Default constructor
    TrackParticleClusterAssociation_v1();

    /// @name Links
    /// Returns 
    /// @{
    /// Returns 
    const ElementLink< TrackParticleContainer >& trackParticleLink() const;
    /// Sets
    void setTrackParticleLink(const ElementLink< TrackParticleContainer >& trackParticleLink);
    /// @}

    /// @name Links
    /// Returns 
    /// @{
    /// Returns
    const std::vector< ElementLink< CaloClusterContainer > >& caloClusterLinks() const;
    /// Sets
    void setCaloClusterLinks(const std::vector< ElementLink< CaloClusterContainer > >& caloClusterLinks);
    /// @}

  }; // end of the TrackParticleClusterAssociation_v1 class definitions

} // end of the xAOD namespace

#endif // XAODASSOCIATIONS_VERSIONS_TRACKPARTICLECLUSTERASSOCIATION_V1_H
