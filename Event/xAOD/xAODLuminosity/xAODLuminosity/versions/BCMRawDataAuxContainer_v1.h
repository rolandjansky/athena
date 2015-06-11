// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BCMRawDataAuxContainer_v1.h 652111 2015-03-06 10:13:22Z krasznaa $
#ifndef XAODLUMINOSITY_VERSIONS_BCMRAWDATAAUXCONTAINER_V1_H
#define XAODLUMINOSITY_VERSIONS_BCMRAWDATAAUXCONTAINER_V1_H

// System include(s):
extern "C" {
#   include "stdint.h"
}
#include <vector>

// xAOD include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD {

   /// Class holding the payload of xAOD::BCMRawDataContainer
   ///
   /// This is the concrete auxiliary store type associated with the
   /// xAOD::BCMRawDataContainer type.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 652111 $
   /// $Date: 2015-03-06 11:13:22 +0100 (Fri, 06 Mar 2015) $
   ///
   class BCMRawDataAuxContainer_v1 : public AuxContainerBase {

   public:
      /// Default constructor
      BCMRawDataAuxContainer_v1();

   private:
      std::vector< uint32_t > bcmWord1; ///< First word of the BCM raw data
      std::vector< uint32_t > bcmWord2; ///< Second word of the BCM raw data

   }; // class BCMRawDataAuxContainer_v1

} // namespace xAOD

#endif // XAODLUMINOSITY_VERSIONS_BCMRAWDATAAUXCONTAINER_V1_H
