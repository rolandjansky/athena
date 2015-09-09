// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ALFADataAuxContainer_v1.h 693858 2015-09-09 10:30:15Z krasznaa $
#ifndef XAODFORWARD_VERSIONS_ALFADATAAUXCONTAINER_V1_H
#define XAODFORWARD_VERSIONS_ALFADATAAUXCONTAINER_V1_H

// System include(s):
#include <vector>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD {

   /// Auxiliary store for xAOD::ALFADataContainer_v1
   ///
   /// This is a custom auxiliary container for xAOD::ALFADataContainer_v1,
   /// to be used in the main reconstruction code.
   ///
   /// @author Libor Nozka <Libor.Nozka@cern.ch>
   ///
   /// $Revision: 693858 $
   /// $Date: 2015-09-09 12:30:15 +0200 (Wed, 09 Sep 2015) $
   ///
   class ALFADataAuxContainer_v1 : public AuxContainerBase {

   public:
      /// Default constructor
      ALFADataAuxContainer_v1();

   private:
		/// @name Information from LocRecEvCollection and LocRecODEvCollection
      /// @{
		std::vector< std::vector< int > > detectorPartID;
		std::vector< int > maxTrackCnt;
		std::vector< std::vector< float > > overU;
		std::vector< std::vector< float > > overV;
		std::vector< std::vector< float > > overY;
		std::vector< std::vector< int > > numU;
		std::vector< std::vector< int > > numV;
		std::vector< std::vector< int > > numY;
		std::vector< std::vector< int > > mdFibSel;
		std::vector< std::vector< int > > odFibSel;
      std::vector< std::vector< float > > xDetCS;
		std::vector< std::vector< float > > yDetCS;
      /// @}

      /// @name Information from LocRecCorrEvCollection and
      ///       LocRecCorrODEvCollection
      /// @{
		std::vector< std::vector< float > > xLhcCS;
		std::vector< std::vector< float > > yLhcCS;
		std::vector< std::vector< float > > zLhcCS;
		std::vector< std::vector< float > > xRPotCS;
		std::vector< std::vector< float > > yRPotCS;
		std::vector< std::vector< float > > xStatCS;
		std::vector< std::vector< float > > yStatCS;
		std::vector< std::vector< float > > xBeamCS;
		std::vector< std::vector< float > > yBeamCS;
      /// @}

		/// @name Information from RawDataContainer
      /// @{
		std::vector< std::vector< int > > scaler;
		std::vector< std::vector< int > > trigPat;
      /// @}

		/// @name Information from DigitCollection
      /// @{
		std::vector< std::vector< int > > mdFiberHits;
		std::vector< std::vector< int > > mdMultiplicity;
      /// @}

		/// @name Information from ODDigitCollection
      /// @{
		std::vector< std::vector< int > > odFiberHitsPos;
		std::vector< std::vector< int > > odFiberHitsNeg;
		std::vector< std::vector< int > > odMultiplicityPos;
		std::vector< std::vector< int > > odMultiplicityNeg;
      /// @}

   }; // class ALFADataAuxContainer_v1

} // namespace xAOD

// Declare the inheritance of the class:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::ALFADataAuxContainer_v1, xAOD::AuxContainerBase );

#endif //XAODFORWARD_VERSIONS_ALFADATAAUXCONTAINER_V1_H
