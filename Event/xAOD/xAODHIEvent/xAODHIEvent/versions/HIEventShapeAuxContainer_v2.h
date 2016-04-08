// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: HIEventShapeAuxContainer_v2.h 693494 2015-09-07 14:59:45Z krasznaa $
#ifndef XAODHIEVENT_VERSIONS_HIEVENTSHAPEAUXCONTAINER_V2_H
#define XAODHIEVENT_VERSIONS_HIEVENTSHAPEAUXCONTAINER_V2_H

// System include(s):
#include <vector>

// xAOD include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD {

   /// Auxiliary store for xAOD::HIEventShape_v2
   ///
   /// This auxiliary store declares all the variables handled by
   /// xAOD::HIEventShape_v2 explicitly, as "static auxiliary
   /// variables".
   ///
   /// @author Aaron Angerami <angerami@cern.ch>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 693494 $
   /// $Date: 2015-09-07 16:59:45 +0200 (Mon, 07 Sep 2015) $
   ///
   class HIEventShapeAuxContainer_v2 : public AuxContainerBase {

   public:
      /// Default constructor
      HIEventShapeAuxContainer_v2();

   private:
      /// @name Energy density information
      /// @{
      std::vector< float > et;
      std::vector< float > area;
      std::vector< float > rho;
      std::vector< int > nCells;
      /// @}

      /// @name Harmonic modulation
      /// @{
      std::vector< std::vector< float > > etCos; 
      std::vector< std::vector< float > > etSin; 
      /// @}

      /// @name Information about the slice
      /// @{
      std::vector< float > etaMin;
      std::vector< float > etaMax;
      std::vector< int > layer;
      /// @}

   }; // class HIEventShapeAuxContainer_v2

} // namespace xAOD

// Declare xAOD::AuxContainerBase as a base of the class:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::HIEventShapeAuxContainer_v2, xAOD::AuxContainerBase );

#endif // XAODHIEVENT_VERSIONS_HIEVENTSHAPEAUXCONTAINER_V2_H
