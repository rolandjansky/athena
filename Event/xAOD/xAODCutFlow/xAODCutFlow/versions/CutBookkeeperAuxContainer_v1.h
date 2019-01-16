// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef XAODCUTFLOW_VERSIONS_CUTBOOKKEEPERAUXCONTAINER_V1_H
#define XAODCUTFLOW_VERSIONS_CUTBOOKKEEPERAUXCONTAINER_V1_H

// STL include(s):
#include <vector>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"
#include "xAODCutFlow/versions/CutBookkeeper_v1.h"
#include "xAODCutFlow/versions/CutBookkeeperContainer_v1.h"
#include "AthLinks/ElementLink.h"


namespace xAOD {

  typedef ElementLink< CutBookkeeperContainer_v1>  CutBookkeeperLink_t;
  typedef std::vector< xAOD::CutBookkeeperLink_t > CutBookkeeperLinks_t;


  /// Auxiliary container for CutBookkeeper containers
  ///
  /// @author Karsten Koeneke <karsten.koeneke@cern.ch>
  ///
  /// $Revsision$
  /// $Date$
  ///
  class CutBookkeeperAuxContainer_v1 : public AuxContainerBase {

  public:
    /// Default constructor
    CutBookkeeperAuxContainer_v1();



  private:
    /// @name Descriptive values
    /// @{

    std::vector< unsigned int >                 nameIdentifier;
    std::vector< unsigned int >                 uniqueIdentifier;
    std::vector< std::string >                  name;
    std::vector< std::string >                  description;
    std::vector< unsigned int >                 cutLogic;
    std::vector< int >                          isTopFilter;
    std::vector< int >                          cycle;
    std::vector< std::string >                  inputStream;
    std::vector< std::vector< std::string > >   outputStreams;
    std::vector< xAOD::CutBookkeeperLink_t >    parentLink;
    std::vector< xAOD::CutBookkeeperLinks_t >   childrenLinks;
    std::vector< xAOD::CutBookkeeperLinks_t >   othersLinks;
    std::vector< xAOD::CutBookkeeperLinks_t >   siblingsLinks;

    /// @}

    /// @name The payload property values
    /// @{
    std::vector< uint64_t >                     nAcceptedEvents;
    std::vector< double >                       sumOfEventWeights;
    std::vector< double >                       sumOfEventWeightsSquared;
    /// @}

  }; // class CutBookkeeperAuxContainer_v1

} // namespace xAOD


// Set up the StoreGate inheritance for the class:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::CutBookkeeperAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODCUTFLOW_VERSIONS_CUTBOOKKEEPERAUXCONTAINER_V1_H
