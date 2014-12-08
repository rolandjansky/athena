// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: L2IsoMuonAuxContainer_v1.h 631524 2014-11-27 14:08:12Z gwatts $
#ifndef XAODTRIGMUON_VERSIONS_L2ISOMUONAUXCONTAINER_V1_H
#define XAODTRIGMUON_VERSIONS_L2ISOMUONAUXCONTAINER_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD {

   /// Auxiliary container for muon iso containers
   ///
   /// This is a fixed implementation of the muon iso auxiliary store.
   ///
   /// @author Stefano Giagu <stefano.giagu@cern.ch>
   ///
   /// V1.0 2014-06-26 
   ///
   class L2IsoMuonAuxContainer_v1 : public AuxContainerBase {

   public:
      /// Default constructor
      L2IsoMuonAuxContainer_v1();

   private:
      /// @name 4-momentum variables
      /// @{
      std::vector< float > pt;
      std::vector< float > phi;
      std::vector< float > eta;
      /// @}

      /// @name L2 muon specific variables
      /// @{
      std::vector< uint32_t > roiWord;
      std::vector< float >    charge;
      std::vector< int >      errorFlag;
      std::vector< float >    sumPt01;
      std::vector< float >    sumPt02;
      std::vector< float >    sumPt03;
      std::vector< float >    sumPt04;
      std::vector< float >    sumEt01;
      std::vector< float >    sumEt02;
      std::vector< float >    sumEt03;
      std::vector< float >    sumEt04;
      /// @}

   }; // class L2IsoMuonAuxContainer_v1

} // namespace xAOD

#include "xAODCore/CLASS_DEF.h"
SG_BASE( xAOD::L2IsoMuonAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODTRIGMUON_VERSIONS_L2ISOMUONAUXCONTAINER_V1_H
