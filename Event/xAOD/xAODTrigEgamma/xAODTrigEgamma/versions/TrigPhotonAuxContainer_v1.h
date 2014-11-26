// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigPhotonAuxContainer_v1.h 631414 2014-11-26 22:03:16Z gwatts $
#ifndef XAODTRIGEGAMMA_VERSIONS_TRIGPHOTONAUXCONTAINTER_V1_H
#define XAODTRIGEGAMMA_VERSIONS_TRIGPHOTONAUXCONTAINTER_V1_H

// System include(s):
#include <vector>
extern "C" {
#   include <stdint.h>
}

// EDM include(s):
#include "AthLinks/ElementLink.h"
#include "xAODCore/AuxContainerBase.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"

namespace xAOD {

   /// Class holding the payload of xAOD::TrigPhotonContainer_v1
   ///
   /// This is the class holding all the data that the HLT saves for
   /// <code>xAOD::TrigPhotonContainer_v1</code>.
   ///
   /// @author Fabrizio Salvatore <Fabrizio.Salvatore@cern.ch>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 631414 $
   /// $Date: 2014-11-26 23:03:16 +0100 (Wed, 26 Nov 2014) $
   ///
   class TrigPhotonAuxContainer_v1 : public AuxContainerBase {

   public:
      /// Default constructor
      TrigPhotonAuxContainer_v1();

   private:
      /// @name Simple photon properties
      /// @{
      std::vector< uint32_t > roiWord;
      std::vector< float > rcore;
      std::vector< float > eratio;
      std::vector< float > etHad;
      std::vector< float > etHad1;
      std::vector< float > f0;
      std::vector< float > f1;
      std::vector< float > f2;
      std::vector< float > f3;
      std::vector< float > fside;
      std::vector< float > weta2;
      /// @}

      /// @name 4-momentum properties
      /// @{
      std::vector< float > pt;
      std::vector< float > caloEta;
      std::vector< float > caloPhi;
      std::vector< float > dEta;
      std::vector< float > dPhi;
      /// @}

      /// @name Link(s) to the photon constituents
      /// @{
      std::vector< ElementLink< TrigEMClusterContainer > > emClusterLink;
      /// @}

   }; // class TrigPhotonAuxContainer_v1

} // namespace xAOD

SG_BASE( xAOD::TrigPhotonAuxContainer_v1, xAOD::AuxContainerBase );

#endif // not XAODTRIGEGAMMA_VERSIONS_TRIGPHOTONAUXCONTAINTER_V1_H
