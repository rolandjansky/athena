// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef XAODCUTFLOW_VERSIONS_CUTBOOKKEEPERCONTAINER_V1_H
#define XAODCUTFLOW_VERSIONS_CUTBOOKKEEPERCONTAINER_V1_H

// DataModel includes
#include "AthContainers/DataVector.h"
#include "xAODCutFlow/versions/CutBookkeeper_v1.h"



namespace xAOD {

  /// Container that holds the Container of all CutBookkeepers.
  ///
  /// @author Karsten Koeneke <karsten.koeneke@cern.ch>
  ///
  /// $Revision$
  /// $Date$
  ///
  class CutBookkeeperContainer_v1: public DataVector<CutBookkeeper_v1>
  {
  public:
    /// Default Constructor
    //CutBookkeeperContainer_v1();

    /// Constructor
    CutBookkeeperContainer_v1( SG::OwnershipPolicy ownPolicy = SG::OWN_ELEMENTS,
                               SG::IndexTrackingPolicy trackIndices = SG::DEFAULT_TRACK_INDICES );

    /// Constructor
    CutBookkeeperContainer_v1( CutBookkeeperContainer_v1::iterator first,
                               CutBookkeeperContainer_v1::iterator last,
                               SG::OwnershipPolicy ownPolicy = SG::OWN_ELEMENTS,
                               SG::IndexTrackingPolicy trackIndices = SG::DEFAULT_TRACK_INDICES );

    // /// Copy constructor:
    // CutBookkeeperContainer_v1( const CutBookkeeperContainer_v1& rhs );
    //
    // /// Assignment operator:
    // CutBookkeeperContainer_v1& operator=( const CutBookkeeperContainer_v1& rhs );
    //
    // /// Constructor with parameters: to setup the ownership policy
    // CutBookkeeperContainer_v1( const SG::OwnershipPolicy own );

    /// Destructor
    virtual ~CutBookkeeperContainer_v1();

    /// Pull in usage of the base class operator[]
    using DataVector<CutBookkeeper_v1>::operator[];

    /// Merge the content of another xAOD::CutBookkeeperContainer into this one
    void merge( const CutBookkeeperContainer_v1* otherCont );

    /// Get the minimum cycle number of any xAOD::CutBookkeepers in the container
    int minCycle() const;

    /// Get the maximum cycle number of any xAOD::CutBookkeepers in the container
    int maxCycle() const;

  };

} // namespace xAOD


// Set up the StoreGate inheritance for the class:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::CutBookkeeperContainer_v1, DataVector<xAOD::CutBookkeeper_v1> );


#endif //> XAODCUTFLOW_VERSIONS_CUTBOOKKEEPERCONTAINER_V1_H
