/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Implementation file for class CutBookkeeperContainer_v1
// Author: Karsten Koeneke, May 2014 <karsten.koeneke@cern.ch>
///////////////////////////////////////////////////////////////////

#include "xAODCutFlow/versions/CutBookkeeperContainer_v1.h"

namespace xAOD {


  ////////////////
  // Constructors
  ////////////////

  // CutBookkeeperContainer_v1::CutBookkeeperContainer_v1()
  //   : DataVector<CutBookkeeper_v1> (SG::OWN_ELEMENTS)
  // { }


  /// Constructor
  CutBookkeeperContainer_v1::CutBookkeeperContainer_v1( SG::OwnershipPolicy ownPolicy,
                                                        SG::IndexTrackingPolicy trackIndices )
    : DataVector<xAOD::CutBookkeeper_v1>(ownPolicy,trackIndices)
  { }

  /// Constructor
  CutBookkeeperContainer_v1::CutBookkeeperContainer_v1( CutBookkeeperContainer_v1::iterator first,
                                                        CutBookkeeperContainer_v1::iterator last,
                                                        SG::OwnershipPolicy ownPolicy,
                                                        SG::IndexTrackingPolicy trackIndices )
    : DataVector<xAOD::CutBookkeeper_v1>(first,last,ownPolicy,trackIndices)
  { }


  /// Destructor
  CutBookkeeperContainer_v1::~CutBookkeeperContainer_v1()
  { }



  // CutBookkeeperContainer_v1::CutBookkeeperContainer_v1( const CutBookkeeperContainer_v1& rhs ) :
  //   DataVector<CutBookkeeper_v1> (rhs)
  // { }
  //
  //
  // CutBookkeeperContainer_v1&
  // CutBookkeeperContainer_v1::operator=( const CutBookkeeperContainer_v1& rhs ) {
  //   if ( this != &rhs ) {
  //     DataVector<CutBookkeeper_v1>::operator=(rhs);
  //   }
  //   return *this;
  // }
  //
  //
  // CutBookkeeperContainer_v1::CutBookkeeperContainer_v1( const SG::OwnershipPolicy own ) :
  //   DataVector<CutBookkeeper_v1>(own)
  // { }

} // namespace xAOD
