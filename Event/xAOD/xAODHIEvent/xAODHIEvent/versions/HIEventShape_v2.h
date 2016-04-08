// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: HIEventShape_v2.h 693494 2015-09-07 14:59:45Z krasznaa $
#ifndef XAODHIEVENT_VERSIONS_HIEVENTSHAPE_V2_H
#define XAODHIEVENT_VERSIONS_HIEVENTSHAPE_V2_H

// System include(s):
#include <vector>

// EDM include(s):
#include "AthContainers/AuxElement.h"

/// Namespace holding all the xAOD classes/functions
namespace xAOD {

   /// Interface class for the HI reconstruction EDM
   ///
   /// This class describes the output of the Heavy-Ion reconstruction,
   /// saving the reconstructed properties.
   ///
   /// @author Aaron Angerami <angerami@cern.ch>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 693494 $
   /// $Date: 2015-09-07 16:59:45 +0200 (Mon, 07 Sep 2015) $
   ///
   class HIEventShape_v2 : public SG::AuxElement {

   public:
      /// Default constructor
      HIEventShape_v2();

      /// @name Energy density information
      /// @{

      /// Transverse energy reconstructed
      float et() const;
      /// Set the reconstructed transverse energy
      void setEt( float value );

      /// @todo Write documentation
      float area() const;
      /// @todo Write documentation
      void setArea( float value );

      /// @todo Write documentation
      float rho() const;
      /// @todo Write documentation
      void setRho( float value );

      /// @todo Write documentation
      int nCells() const;
      /// @todo Write documentation
      void setNCells( int value );

      /// @}

      /// @name Harmonic modulation
      /// @{

      /// @todo Write documentation
      const std::vector< float >& etCos() const;
      /// @todo Write documentation
      std::vector< float >& etCos();
      /// @todo Write documentation
      void setEtCos( const std::vector< float >& value );

      /// @todo Write documentation
      const std::vector<float>& etSin() const;
      /// @todo Write documentation
      std::vector<float>& etSin();
      /// @todo Write documentation
      void setEtSin( const std::vector< float >& value );

      /// @}

      /// @name Information about the slice
      /// @{

      /// @todo Write documentation
      float etaMin() const;
      /// @todo Write documentation
      void setEtaMin( float value );

      /// @todo Write documentation
      float etaMax() const;
      /// @todo Write documentation
      void setEtaMax( float value );

      /// @todo Write documentation
      int layer() const;
      /// @todo Write documentation
      void setLayer( int value );

      /// @}

   }; // class HIEventShape_v2

} // namespace xAOD

// Declare SG::AuxElement as a base of the class:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::HIEventShape_v2, SG::AuxElement );

#endif // XAODHIEVENT_VERSIONS_HIEVENTSHAPE_V2_H
