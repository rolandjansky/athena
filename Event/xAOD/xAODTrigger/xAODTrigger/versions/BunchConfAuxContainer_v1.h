// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BunchConfAuxContainer_v1.h 631149 2014-11-26 12:26:18Z krasznaa $
#ifndef XAODTRIGGER_VERSIONS_BUNCHCONFAUXCONTAINER_V1_H
#define XAODTRIGGER_VERSIONS_BUNCHCONFAUXCONTAINER_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD {

   /// Auxiliary store for BunchConfContainer_v1
   ///
   /// This object holds all the variables that BunchConf_v1 needs access
   /// to. Mean to provide efficient I/O when the full bunch configuration
   /// information is to be saved/read.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 631149 $
   /// $Date: 2014-11-26 13:26:18 +0100 (Wed, 26 Nov 2014) $
   ///
   class BunchConfAuxContainer_v1 : public AuxContainerBase {

   public:
      /// Default constructor
      BunchConfAuxContainer_v1();

   private:
      /// @name Configuration identifier variable(s)
      /// @{
      std::vector< uint32_t > configID;
      /// @}

      /// @name Basic configuration variables
      /// @{
      std::vector< std::vector< int > > collidingBCIDs;
      std::vector< std::vector< float > > intensitiesBeam1;
      std::vector< std::vector< float > > intensitiesBeam2;
      /// @}

      /// @name Unpaired bunch configuration variables
      /// @{
      std::vector< std::vector< int > > unpairedBCIDsBeam1;
      std::vector< std::vector< int > > unpairedBCIDsBeam2;
      std::vector< std::vector< float > > unpairedIntensisitesBeam1;
      std::vector< std::vector< float > > unpairedIntensisitesBeam2;
      /// @}

   }; // class BunchConfAuxContainer_v1

} // namespace xAOD

// Declare the inheritance of the container:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::BunchConfAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODTRIGGER_VERSIONS_BUNCHCONFAUXCONTAINER_V1_H
