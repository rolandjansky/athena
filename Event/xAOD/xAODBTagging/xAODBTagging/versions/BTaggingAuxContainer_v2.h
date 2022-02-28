// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODBTAGGING_VERSIONS_BTAGGINGAUXCONTAINER_V2_H
#define XAODBTAGGING_VERSIONS_BTAGGINGAUXCONTAINER_V2_H

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD {

   // Changes since BTaggingAuxContainer_v1: Remove all aux data,
   // making it dynamic instead.

   class BTaggingAuxContainer_v2 : public AuxContainerBase {

   public:
      /// Default constructor
      BTaggingAuxContainer_v2();

   private:

   }; // class BTaggingAuxContainer_v2

} // namespace xAOD

#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::BTaggingAuxContainer_v2, xAOD::AuxContainerBase );

#endif // XAODBTAGGING_VERSIONS_BTAGGINGAUXCONTAINER_V2_H
